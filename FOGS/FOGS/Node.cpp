#include "Node.h"
#include "Value.h"
#include "Context.h"
#include "Allocator.h"

namespace FOGS
{
	Allocator<sizeof(Node_impl)> NodeAlloc;
	Allocator<sizeof(Attribute_impl)> AttrAlloc;

	Node_impl::Node_impl(FOGS_Context* _context)
	{
		m_Context = _context;
	}

	void Node_impl::ReadNodes()
	{
		*m_Context += new READER_ALLOC Reader<Node_impl>(this, &Node_impl::NodesReader);
	}

	bool Node_impl::NodesReader(char* _char)
	{
		if (m_Context->CanSkip(_char))
			return true;
		else if (*_char == '}')
		{
			m_Context->PopReader();
			return true;
		}

		Node_impl* lv_NewNode = new NODE_ALLOC Node_impl(m_Context);
		lv_NewNode->m_Parent = this;
		lv_NewNode->ReadName();
		return m_Context->Read(_char);
	}

	void Node_impl::ReadAttributes()
	{
		*m_Context += new READER_ALLOC Reader<Node_impl>(this, &Node_impl::AttributesReader);
	}

	bool Node_impl::AttributesReader(char* _char)
	{
		if (m_Context->CanSkip(_char))
			return true;
		else if (*_char == ')')
		{
			m_Context->PopReader();
			return true;
		}

		Attribute_impl* lv_NewData = new ATTR_ALLOC Attribute_impl(m_Context);
		lv_NewData->m_Parent = this;
		lv_NewData->ReadName();
		return m_Context->Read(_char);

	}

	void Node_impl::ReadName()
	{
		m_End = 0;
		*m_Context += new READER_ALLOC Reader<Node_impl>(this, &Node_impl::NameReader);
	}

	bool Node_impl::NameReader(char* _char)
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
				m_Value = new VALUE_DATA_ALLOC ValueData_impl(m_Context);
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

			m_Value = new VALUE_DATA_ALLOC ValueData_impl(m_Context);
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
				m_Value = new VALUE_DATA_ALLOC ValueData_impl(m_Context);
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
			m_Value = new VALUE_DATA_ALLOC ValueData_impl(m_Context);
			m_Value->m_Node = this;
			m_Value->ReadValue();
			return true;
		}
		else if (*_char == '"')
		{
			if (m_Name)
				return false;

			m_Context->PopReader();
			m_Value = new VALUE_DATA_ALLOC ValueData_impl(m_Context);
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
			m_Value = new VALUE_DATA_ALLOC ValueData_impl(m_Context);
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

	bool Node_impl::IsUnexpected(char* _char)
	{
		return
			*_char == ')' ||
			*_char == ']' ;
	}

	void Node_impl::NameEnds()
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

	Node_impl::~Node_impl()
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

	void Node_impl::AppendNode()
	{
		if (!m_Parent->m_Nodes)
			m_Parent->m_Nodes = this;
		else
			m_Parent->m_Last->m_Sibling = this;

		m_Parent->m_Last = this;
		m_Parent->m_NodesSize++;
	}


//////////////////////////////////////////////////////////////////////////

	Node::Node(Node_impl* _data)
	{
		m_Data = _data;
	}

	unsigned int Node::ChildsCount()
	{
		return m_Data->m_NodesSize;
	}

	unsigned int Node::AttributesCount()
	{
		return m_Data->m_AttrSize;
	}

	Node Node::Childe(const std::string& _key)
	{
	
		for (auto lv_Node = m_Data->m_Nodes; lv_Node; lv_Node = lv_Node->m_Sibling)
		{
			if (lv_Node->m_Name == _key)
				return lv_Node;
		}

		return 0;
	}

	Node Node::operator[](const std::string& _key)
	{
		return Childe(_key);
	}

	FOGS::Node Node::operator[](const char* _key)
	{
		return Childe(std::string(_key));
	}

	Attribute Node::AttributeAt(const std::string& _key)
	{
		for (auto lv_Attr = m_Data->m_Attributes; lv_Attr; lv_Attr = lv_Attr->m_Sibling)
		{
			if (lv_Attr->m_Name == _key)
				return lv_Attr;
		}

		return 0;
	}

	Attribute Node::operator()(const std::string& _key)
	{
		return AttributeAt(_key);
	}
	
	Node::operator bool()
	{
		return m_Data != 0;
	}

	bool Node::IsNull()
	{
		return m_Data == 0;
	}

	ValueData Node::Value()
	{
		if (!m_Data->m_Value)
		{
			m_Data->m_Value = new VALUE_DATA_ALLOC ValueData_impl(0);
			m_Data->m_Value->m_Node = m_Data;
		}

		return m_Data->m_Value;
	}
	
	Node Node::Next()
	{
		for (auto lv_Node = m_Data->m_Sibling; lv_Node; lv_Node = lv_Node->m_Sibling)
		{
			if (lv_Node->m_Name == m_Data->m_Name)
					return lv_Node;
		}

		return 0;
	}

	std::string Node::Name()
	{
		return m_Data->m_Name;
	}

	Node& Node::Name(const std::string& _val)
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

	Node& Node::Name(const char* _val)
	{
		auto lv_Size = strlen(_val);
		if (!m_Data->ContextHolder)
			delete m_Data->m_Name;
		m_Data->ContextHolder = false;

		if (!_val)
		{
			m_Data->m_Name = 0;
			return *this;
		}

		m_Data->m_Name = new char[lv_Size + 1];
		m_Data->m_Name[lv_Size] = 0;
		memcpy(m_Data->m_Name, _val, lv_Size);
		return *this;
	}

	Node Node::AppendChild()
	{
		Node_impl* lv_Node = new NODE_ALLOC Node_impl(0);
		lv_Node->m_Parent = m_Data;
		lv_Node->AppendNode();
		return lv_Node;
	}

	Node Node::AppendChild(const char* _name)
	{
		Node lv_Node = AppendChild();
		lv_Node.Name(_name);
		return lv_Node;
	}

	Node Node::AppendChild(const std::string& _name)
	{
		Node lv_Node = AppendChild();
		lv_Node.Name(_name);
		return lv_Node;
	}

	Attribute Node::AppendAttribute()
	{
		auto lv_Attr = new ATTR_ALLOC  Attribute_impl(0);
		lv_Attr->m_Parent = m_Data;
		lv_Attr->m_Sibling = m_Data->m_Attributes;
		m_Data->m_Attributes = lv_Attr;
		m_Data->m_AttrSize++;
		return lv_Attr;
	}

	Attribute Node::AppendAttribute(const char* _name)
	{
		return AppendAttribute().Name(_name);
	}

	Attribute Node::AppendAttribute(const std::string& _name)
	{
		return AppendAttribute().Name(_name);
	}

	std::list<Node> Node::Childs()
	{
		std::list<Node> lv_Val;

		for (auto lv_Node = m_Data->m_Nodes; lv_Node; lv_Node = lv_Node->m_Sibling)
			lv_Val.push_back(lv_Node);

		return lv_Val;
	}

	std::list<Node> Node::Childs(const char* _name)
	{
		return Childs(std::string(_name));
	}

	std::list<Node> Node::Childs(const std::string& _name)
	{
		std::list<Node> lv_Val;

		for (auto lv_Node = m_Data->m_Nodes; lv_Node; lv_Node = lv_Node->m_Sibling)
			if (lv_Node->m_Name == _name)
				lv_Val.push_back(lv_Node);

		return lv_Val;
	}

	std::list<Attribute> Node::Attributes()
	{
		std::list<Attribute> lv_Val;
		for (auto lv_Attr = m_Data->m_Attributes; lv_Attr; lv_Attr = lv_Attr->m_Sibling)
			lv_Val.push_back(lv_Attr);

		return lv_Val;
	}

	std::list<Attribute> Node::Attributes(const char* _name)
	{
		return Attributes(std::string(_name));
	}

	std::list<Attribute> Node::Attributes(const std::string& _name)
	{
		std::list<Attribute> lv_Val;
		for (auto lv_Attr = m_Data->m_Attributes; lv_Attr; lv_Attr = lv_Attr->m_Sibling)
			if (lv_Attr->m_Name == _name)
				lv_Val.push_back(lv_Attr);

		return lv_Val;
	}

	std::map<std::string, std::list<Node>> Node::MapChilds()
	{
		std::map<std::string, std::list<Node>> lv_Map;
		for (auto lv_Node = m_Data->m_Nodes; lv_Node; lv_Node = lv_Node->m_Sibling)
			lv_Map[lv_Node->m_Name].push_back(lv_Node);
		
		return lv_Map;
	}

	std::map<std::string, std::list<Attribute>> Node::MapAttributes()
	{
		std::map<std::string, std::list<Attribute>> lv_Map;
		for (auto lv_Attr = m_Data->m_Attributes; lv_Attr; lv_Attr = lv_Attr->m_Sibling)
			lv_Map[lv_Attr->m_Name].push_back(lv_Attr);

		return lv_Map;
	}

	Node::Iterator Node::begin()
	{
		return m_Data->m_Nodes;
	}

	Node::Iterator Node::end()
	{
		return 0;
	}


	Node::Iterator::Iterator(Node_impl* _root)
	{
		m_Root = _root;
	}

	FOGS::Node Node::Iterator::operator*() const
	{
		return m_Root;
	}

	Node::Iterator& Node::Iterator::operator++()
	{
		m_Root = m_Root->m_Sibling;
		return *this;
	}

	bool Node::Iterator::operator!=(const Iterator& _rval)
	{
		return m_Root != _rval.m_Root;
	}

	Node::Iterator begin(Node& _val)
	{
		return _val.begin();
	}

	Node::Iterator end(Node& _val)
	{
		return _val.end();
	}

}