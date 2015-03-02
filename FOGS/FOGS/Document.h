#pragma once
#include <string>
#include "Node.h"

namespace FOGS
{
	struct Node_impl;

	class FOGS_Document
	{
	public:
		FOGS_Document();
		~FOGS_Document();

		bool Load(const std::string& _string);
		std::string Save();
		Node Root();
		
		int ErrorLine();
		int ErrorChar();
		std::string ErrorString();
		
	private:
		void AddChilds(std::string& _string, Node_impl* param2);

		void WriteChild(Node_impl * lv_Node, std::string& _string, std::string &lv_Tabs);

		void AddAttributes(std::string& _string, Node_impl* _node);

		void WriteAttribute(Attribute_impl * lv_Attr, std::string& _string);

		void AddValue(std::string& _string, ValueData_impl* _value);
		void WriteValue(std::string& _string, ValueItem_impl *lv_Val);
		bool SkipInConstant(char _Char);

		Node_impl* m_Root = 0;
		unsigned int m_ErrorLine = 0;
		unsigned int m_ErrorChar = 0;
		//std::string m_ErrorString;
	};
}
