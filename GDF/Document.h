#pragma once
#include <string>
#include "Node.h"

namespace GameDataFormat
{
	struct NodeData;

	class GDF_Document
	{
	public:
		GDF_Document();
		~GDF_Document();

		bool Load(const std::string& _string);
		std::string Save();
		GDF_Node Root();
		
		int ErrorLine();
		int ErrorChar();
		std::string ErrorString();
		
	private:
		void AddChilds(std::string& _string, NodeData* param2);

		void WriteChild(NodeData * lv_Node, std::string& _string, std::string &lv_Tabs);

		void AddAttributes(std::string& _string, NodeData* _node);

		void WriteAttribute(AttributeData * lv_Attr, std::string& _string);

		void AddValue(std::string& _string, ValueData* _value);
		void WriteValue(std::string& _string, ValueItem *lv_Val);
		bool SkipInConstant(char _Char);

		NodeData* m_Root = 0;
		unsigned int m_ErrorLine = 0;
		unsigned int m_ErrorChar = 0;
		//std::string m_ErrorString;
	};
}
