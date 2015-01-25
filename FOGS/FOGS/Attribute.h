#pragma once
#include "Value.h"

namespace FOGS
{
	class FOGS_Context;
	struct NodeData;
	struct ValueData;

	struct AttributeData
	{
		AttributeData(FOGS_Context* _context);
		~AttributeData();

		void ReadName();
		void NameToValue();
		bool IsUnexpacted(char* _char);
		bool NameReader(char* _char);

		void AppendAttribute();

		char* m_Name = 0;
		char* m_End = 0;

		AttributeData* m_Sibling = 0;
		FOGS_Context* m_Context = 0;
		NodeData* m_Parent = 0;
		ValueData* m_Value = 0;
		bool m_EndName = false;
		bool ContextHolder = true;
	};

	class FOGS_Attribute
	{
	public:
		FOGS_Attribute(AttributeData* _data);

		std::string Name();
		FOGS_Attribute& Name(const std::string& _val);
		FOGS_Attribute& Name(const char* _val);
		FOGS_Value Value();
		bool IsNull();

		operator bool();
	private:
		AttributeData* m_Data = 0;
	};
}