#include "Document.h"
#include "Context.h"


namespace FOGS
{

	FOGS_Document::FOGS_Document()
	{

	}


	bool FOGS_Document::Load(const std::string& _string)
	{
		FOGS_Context lv_Context(_string);
		if (!lv_Context.GetReady())
			return false;
		
		m_Root = new Node_impl(&lv_Context);
		m_Root->ReadNodes();

		if (lv_Context.Parse())
			return true;
		else
		{
			m_ErrorLine = lv_Context.Line();
			m_ErrorChar = lv_Context.Char();
			//m_ErrorString = 0;// lv_Context.String();
			return false;
		}
	}

	int FOGS_Document::ErrorLine()
	{
		return m_ErrorLine;
	}

	int FOGS_Document::ErrorChar()
	{
		return m_ErrorChar;
	}

	std::string FOGS_Document::ErrorString()
	{
		return "";// m_ErrorString;
	}

	Node FOGS_Document::Root()
	{
		if (!m_Root)
			m_Root = new Node_impl(0);

		return m_Root;
	}

	FOGS_Document::~FOGS_Document()
	{
		delete m_Root;
	}

	std::string FOGS_Document::Save()
	{
		std::string lv_Out;
		lv_Out += "{\n";
		if (m_Root)
			AddChilds(lv_Out, m_Root);
		lv_Out += '}';

		return lv_Out;
	}

	void FOGS_Document::AddChilds(std::string& _string, Node_impl* _node)
	{
		static std::string lv_Tabs = "\t";
		if (!_node->m_Nodes)
			return;

		for (auto lv_Node = _node->m_Nodes; lv_Node; lv_Node = lv_Node->m_Sibling)
			WriteChild(lv_Node, _string, lv_Tabs);
	}

	void FOGS_Document::AddAttributes(std::string& _string, Node_impl* _node)
	{
		if (!_node->m_Attributes)
			return;

		_string += '(';
		for (auto lv_Node = _node->m_Attributes; lv_Node; lv_Node = lv_Node->m_Sibling)
			WriteAttribute(lv_Node, _string);
		_string += ')';
	}

	void FOGS_Document::AddValue(std::string& _string, ValueData_impl* _value)
	{
		for(auto lv_Val = _value->m_Values; lv_Val; lv_Val = lv_Val->Sibling)
		{
			WriteValue(_string, lv_Val);

			_string += (lv_Val->Sibling ? ", " : "");
		}
	}

	bool FOGS_Document::SkipInConstant(char _Char)
	{
		return
			_Char == '(' || _Char == ')' ||
			_Char == '{' || _Char == '}' ||
			_Char == '[' || _Char == ']' ||
			_Char == '"' || _Char == '=' ||
			_Char == ',' || _Char == ';' ;
	}

	void FOGS_Document::WriteValue(std::string& _string, ValueItem_impl *_Val)
	{
		ValueItem lv_Val = _Val;

		auto lv_Type = lv_Val.Type();

		if (lv_Type == VT_FLOAT || lv_Type == VT_INT || lv_Type == VT_BOOL)
		{
			_string += lv_Val.AsString();
		}
		else if (lv_Type == VT_CONSTANT)
		{
			for (auto lv_Char : lv_Val.AsString())
			{
				if (SkipInConstant(lv_Char))
					continue;
				_string += lv_Char;
			}
		}
		else if (lv_Type == VT_STRING)
		{
			_string += '"';
			for (auto lv_Char : lv_Val.AsString())
			{
				if (lv_Char == '"' || lv_Char == '\\' || lv_Char == '\t' || lv_Char == '\n')
					_string += '\\';

				_string += lv_Char;
			}
			_string += '"';
		}
		else if (lv_Type == VT_META)
		{
			_string += lv_Val.Lable();
			_string += '[';
			for (auto lv_Char : lv_Val.AsString())
			{	
				if (lv_Char == ']' || lv_Char == '\\')
					_string += '\\';
				_string += lv_Char;
			}
			_string += ']';
		}
	}

	void FOGS_Document::WriteChild(Node_impl * lv_Node, std::string& _string, std::string &_Tabs)
	{
		if (!lv_Node->m_Name && !lv_Node->m_Value && !lv_Node->m_Attributes)
			return;

		_string += _Tabs;

		if (lv_Node->m_Name)
			_string += lv_Node->m_Name;

		AddAttributes(_string, lv_Node);
		if (lv_Node->m_Value)
		{
			if (lv_Node->m_Name || lv_Node->m_Attributes)
				_string += " = ";
			AddValue(_string, lv_Node->m_Value);
		}

		if (!lv_Node->m_Nodes)
			_string += ";\n";
		else
		{
			_string += '\n';
			_string += _Tabs;
			_string += "{\n";
			_Tabs += '\t';
			AddChilds(_string, lv_Node);
			_Tabs.erase(0, 1);
			_string += _Tabs;
			_string += "}\n";
		}	

	}

	void FOGS_Document::WriteAttribute(Attribute_impl * lv_Attr, std::string& _string)
	{
		if (lv_Attr->m_Name)
		{
			_string += lv_Attr->m_Name;
			if (lv_Attr->m_Value)
				_string += " = ";
		}
		if (lv_Attr->m_Value)
			AddValue(_string, lv_Attr->m_Value);

		if (lv_Attr->m_Sibling)
			_string += "; ";
	}


}