#include "Node.h"
#include "Value.h"
#include "Context.h"
#include "Allocator.h"

namespace FOGS
{
	Allocator<sizeof(NodeData)> NodeAlloc;
	Allocator<sizeof(AttributeData)> AttrAlloc;

	NodeData::NodeData(FOGS_Context* _context)
	{
		m_Context = _context;
	}

	void NodeData::ReadNodes()
	{
		*m_Context += new READER_ALLOC Reader<NodeData>(this, &NodeData::NodesReader);
	}

	bool NodeData::NodesReader(char* _char)
	{
		if (m_Context->CanSkip(_char))
			return true;
		else if (*_char == '}')
		{
			m_Context->PopReader();
			return true;
		}

		NodeData* lv_NewNode = new NODE_ALLOC NodeData(m_Context);
		lv_NewNode->m_Parent = this;
		lv_NewNode->ReadName();
		return m_Context->Read(_char);
	}

	void NodeData::ReadAttributes()
	{
		*m_Context += new READER_ALLOC Reader<NodeData>(this, &NodeData::AttributesReader);
	}

	bool NodeData::AttributesReader(char* _char)
	{
		if (m_Context->CanSkip(_char))
			return true;
		else if (*_char == ')')
		{
			m_Context->PopReader();
			return true;
		}

		AttributeData* lv_NewData = new ATTR_ALLOC AttributeData(m_Context);
		lv_NewData->m_Parent = this;
		lv_NewData->ReadName();
		return m_Context->Read(_char);

	}

	void NodeData::ReadName()
	{
		m_End = 0;
		*m_Context += new READER_ALLOC Reader<NodeData>(this, &NodeData::NameReader);
	}

	bool NodeData::NameReader(char* _char)
	{
		if (IsUnexpected(_char))
			return false;

		if (m_Context->CanSkip(_char))
		{
			return true;
		}			
		else if (*_char == ';')
		{
			if (!m_Name && !m_Attributes)
			{
				delete this;
				return true;
			}

			if (!m_Value && !m_Attributes)
			{
				m_Value = new VALUE_DATA_ALLOC ValueData(m_Context);
				auto Item = m_Value->AddItem();
				if (m_Name)
					*(m_End + 1) = 0;
				Item->String = m_Name; 
				m_Name = 0;
			}

			NameEnds();				
			return true;
		}
		else if (*_char == ',')
		{
			if (!m_Name && !m_Attributes)
			{
				delete this;
				return false;
			}

			if (m_Value || m_Attributes)
				return false;

			m_Value = new VALUE_DATA_ALLOC ValueData(m_Context);
			auto Item = m_Value->AddItem();
			if (m_Name)
				*(m_End + 1) = 0;
			Item->String = m_Name; 
			m_Name = 0;

			NameEnds();
			m_Value->ReadValue();

			return true;
		}
		else if (*_char == '{')
		{
			if (!m_Name)
				return false;

			NameEnds();
			ReadNodes();
			return true;
		}
		else if (*_char == '}')
		{
			if (!m_Name && !m_Attributes)
			{
				delete this;
				return true;
			}

			if (!m_Value && !m_Attributes)
			{
				m_Value = new VALUE_DATA_ALLOC ValueData(m_Context);
				auto Item = m_Value->AddItem();
				if (m_Name)
					*(m_End + 1) = 0;
				Item->String = m_Name; 
				m_Name = 0;
			}

			NameEnds();
			return true;
		}
		else if (*_char == '(')
		{
			m_NameEnds = true;
				
			
			
			AppendNode();
			ReadAttributes();
			if (m_Name)
				*(m_End + 1) = 0;
			return true;
		}
		else if (*_char == '=')
		{
			if (!m_Name && !m_Attributes)
				return false;


			NameEnds();
			m_Value = new VALUE_DATA_ALLOC ValueData(m_Context);
			m_Value->m_Node = this;
			m_Value->ReadValue();
			return true;
		}
		else if (*_char == '"')
		{
			if (m_Name)
				return false;

			m_Context->PopReader();
			m_Value = new VALUE_DATA_ALLOC ValueData(m_Context);
			m_Value->m_Node = this;
			m_Value->ReadValue();
			auto lv_Ret = m_Context->Read(_char);
			{
				m_NameEnds = true;

				if (m_Name)
					*(m_End + 1) = 0;

				AppendNode();
			}
			return lv_Ret;
		}
		else if (*_char == '[')
		{
			char* lv_Tmp;
			if (m_Name)
				*(m_End + 1) = 0;
			lv_Tmp = m_Name; 
			m_Name = 0;
			NameEnds();
			m_Value = new VALUE_DATA_ALLOC ValueData(m_Context);
			m_Value->m_Node = this;
			m_Value->ReadValue(lv_Tmp);
			return true;
		}
		else if (!m_NameEnds)
		{
			m_End = _char;
			if (!m_Name)
				m_Name = _char;
		}
		else
			return false;

		return true;
		
	}

	bool NodeData::IsUnexpected(char* _char)
	{
		return
			*_char == ')' ||
			*_char == ']' ;
	}

	void NodeData::NameEnds()
	{
		m_Context->PopReader();		
		if (!m_NameEnds)
		{
			m_NameEnds = true;

			if (m_Name)
				*(m_End + 1) = 0;

			AppendNode();
		}
	}

	NodeData::~NodeData()
	{
		auto lv_Node = m_Nodes;
		while (lv_Node)
		{
			auto lv_ForFree = lv_Node;
			lv_Node = lv_Node->m_Sibling;
			NodeAlloc.Free(lv_ForFree);
		}
						
		auto lv_Attr = m_Attributes;
		while (lv_Attr)
		{
			auto lv_ForFree = lv_Attr;
			lv_Attr = lv_Attr->m_Sibling;
			AttrAlloc.Free(lv_ForFree);
		}
			
		ValueDataAlloc.Free(m_Value);

		if (!ContextHolder)
			delete[] m_Name;
	}

	void NodeData::AppendNode()
	{
		if (!m_Parent->m_Nodes)
			m_Parent->m_Nodes = this;
		else
			m_Parent->m_Last->m_Sibling = this;

		m_Parent->m_Last = this;
		m_Parent->m_NodesSize++;
	}


//////////////////////////////////////////////////////////////////////////

	FOGS_Node::FOGS_Node(NodeData* _data)
	{
		m_Data = _data;
	}

	unsigned int FOGS_Node::ChildsCount()
	{
		return m_Data->m_NodesSize;
	}

	unsigned int FOGS_Node::AttributesCount()
	{
		return m_Data->m_AttrSize;
	}

	FOGS_Node FOGS_Node::Childe(const std::string& _key)
	{
	
		for (auto lv_Node = m_Data->m_Nodes; lv_Node; lv_Node = lv_Node->m_Sibling)
		{
			if (lv_Node->m_Name == _key)
				return lv_Node;
		}

		return 0;
	}

	FOGS_Node FOGS_Node::operator[](const std::string& _key)
	{
		return Childe(_key);
	}

	FOGS::FOGS_Node FOGS_Node::operator[](const char* _key)
	{
		return Childe(std::string(_key));
	}

	FOGS_Attribute FOGS_Node::Attribute(const std::string& _key)
	{
		for (auto lv_Attr = m_Data->m_Attributes; lv_Attr; lv_Attr = lv_Attr->m_Sibling)
		{
			if (lv_Attr->m_Name == _key)
				return lv_Attr;
		}

		return 0;
	}

	FOGS_Attribute FOGS_Node::operator()(const std::string& _key)
	{
		return Attribute(_key);
	}
	
	FOGS_Node::operator bool()
	{
		return m_Data != 0;
	}

	bool FOGS_Node::IsNull()
	{
		return m_Data == 0;
	}

	FOGS_Value FOGS_Node::Value()
	{
		if (!m_Data->m_Value)
		{
			m_Data->m_Value = new VALUE_DATA_ALLOC ValueData(0);
			m_Data->m_Value->m_Node = m_Data;
		}

		return m_Data->m_Value;
	}
	
	FOGS_Node FOGS_Node::Next()
	{
		for (auto lv_Node = m_Data->m_Sibling; lv_Node; lv_Node = lv_Node->m_Sibling)
		{
			if (lv_Node->m_Name == m_Data->m_Name)
					return lv_Node;
		}

		return 0;
	}

	std::string FOGS_Node::Name()
	{
		return m_Data->m_Name;
	}

	FOGS_Node& FOGS_Node::Name(const std::string& _val)
	{
		auto lv_Size = _val.size();
		if (!m_Data->ContextHolder)
			delete m_Data->m_Name;
		m_Data->ContextHolder = false;

		if (_val.empty())
		{
			m_Data->m_Name = 0;
			return *this;
		}

		m_Data->m_Name = new char[lv_Size + 1];
		m_Data->m_Name[lv_Size] = 0;
		memcpy(m_Data->m_Name, _val.c_str(), lv_Size);
		return *this;
	}

	FOGS_Node& FOGS_Node::Name(const char* _val)
	{
		auto lv_Size = strlen(_val);
		if (!m_Data->ContextHolder)
			delete m_Data->m_Name;
		m_Data->ContextHolder = false;

		if (_val)
		{
			m_Data->m_Name = 0;
			return *this;
		}

		m_Data->m_Name = new char[lv_Size + 1];
		m_Data->m_Name[lv_Size] = 0;
		memcpy(m_Data->m_Name, _val, lv_Size);
		return *this;
	}

	FOGS_Node FOGS_Node::AppendChild()
	{
		NodeData* lv_Node = new NODE_ALLOC NodeData(0);
		lv_Node->m_Parent = m_Data;
		lv_Node->AppendNode();
		return lv_Node;
	}

	FOGS_Node FOGS_Node::AppendChild(const char* _name)
	{
		FOGS_Node lv_Node = AppendChild();
		lv_Node.Name(_name);
		return lv_Node;
	}

	FOGS_Node FOGS_Node::AppendChild(const std::string& _name)
	{
		FOGS_Node lv_Node = AppendChild();
		lv_Node.Name(_name);
		return lv_Node;
	}

	FOGS_Attribute FOGS_Node::AppendAttribute()
	{
		auto lv_Attr = new ATTR_ALLOC  AttributeData(0);
		lv_Attr->m_Parent = m_Data;
		lv_Attr->m_Sibling = m_Data->m_Attributes;
		m_Data->m_Attributes = lv_Attr;
		m_Data->m_AttrSize++;
		return lv_Attr;
	}

	FOGS_Attribute FOGS_Node::AppendAttribute(const char* _name)
	{
		return AppendAttribute().Name(_name);
	}

	FOGS_Attribute FOGS_Node::AppendAttribute(const std::string& _name)
	{
		return AppendAttribute().Name(_name);
	}

	std::list<FOGS_Node> FOGS_Node::Childs()
	{
		std::list<FOGS_Node> lv_Val;

		for (auto lv_Node = m_Data->m_Nodes; lv_Node; lv_Node = lv_Node->m_Sibling)
			lv_Val.push_back(lv_Node);

		return lv_Val;
	}

	std::list<FOGS_Node> FOGS_Node::Childs(const char* _name)
	{
		return Childs(std::string(_name));
	}

	std::list<FOGS_Node> FOGS_Node::Childs(const std::string& _name)
	{
		std::list<FOGS_Node> lv_Val;

		for (auto lv_Node = m_Data->m_Nodes; lv_Node; lv_Node = lv_Node->m_Sibling)
			if (lv_Node->m_Name == _name)
				lv_Val.push_back(lv_Node);

		return lv_Val;
	}

	std::list<FOGS_Attribute> FOGS_Node::Attributes()
	{
		std::list<FOGS_Attribute> lv_Val;
		for (auto lv_Attr = m_Data->m_Attributes; lv_Attr; lv_Attr = lv_Attr->m_Sibling)
			lv_Val.push_back(lv_Attr);

		return lv_Val;
	}

	std::list<FOGS_Attribute> FOGS_Node::Attributes(const char* _name)
	{
		return Attributes(std::string(_name));
	}

	std::list<FOGS_Attribute> FOGS_Node::Attributes(const std::string& _name)
	{
		std::list<FOGS_Attribute> lv_Val;
		for (auto lv_Attr = m_Data->m_Attributes; lv_Attr; lv_Attr = lv_Attr->m_Sibling)
			if (lv_Attr->m_Name == _name)
				lv_Val.push_back(lv_Attr);

		return lv_Val;
	}

	std::map<std::string, std::list<FOGS_Node>> FOGS_Node::MapChilds()
	{
		std::map<std::string, std::list<FOGS_Node>> lv_Map;
		for (auto lv_Node = m_Data->m_Nodes; lv_Node; lv_Node = lv_Node->m_Sibling)
			lv_Map[lv_Node->m_Name].push_back(lv_Node);
		
		return lv_Map;
	}

	std::map<std::string, std::list<FOGS_Attribute>> FOGS_Node::MapAttributes()
	{
		std::map<std::string, std::list<FOGS_Attribute>> lv_Map;
		for (auto lv_Attr = m_Data->m_Attributes; lv_Attr; lv_Attr = lv_Attr->m_Sibling)
			lv_Map[lv_Attr->m_Name].push_back(lv_Attr);

		return lv_Map;
	}

	FOGS_Node::Iterator FOGS_Node::begin()
	{
		return m_Data->m_Nodes;
	}

	FOGS_Node::Iterator FOGS_Node::end()
	{
		return 0;
	}


	FOGS_Node::Iterator::Iterator(NodeData* _root)
	{
		m_Root = _root;
	}

	FOGS::FOGS_Node FOGS_Node::Iterator::operator*() const
	{
		return m_Root;
	}

	FOGS_Node::Iterator& FOGS_Node::Iterator::operator++()
	{
		m_Root = m_Root->m_Sibling;
		return *this;
	}

	bool FOGS_Node::Iterator::operator!=(const Iterator& _rval)
	{
		return m_Root != _rval.m_Root;
	}

	FOGS_Node::Iterator begin(FOGS_Node& _val)
	{
		return _val.begin();
	}

	FOGS_Node::Iterator end(FOGS_Node& _val)
	{
		return _val.end();
	}

}