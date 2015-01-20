#pragma once
#include "Value.h"

namespace GameDataFormat
{
	class GDF_Context;
	struct NodeData;
	struct ValueData;

	struct AttributeData
	{
		AttributeData(GDF_Context* _context);
		~AttributeData();

		void ReadName();
		void NameToValue();
		bool IsUnexpacted(char* _char);
		bool NameReader(char* _char);

		void AppendAttribute();

		char* m_Name = 0;
		char* m_End = 0;

		AttributeData* m_Sibling = 0;
		GDF_Context* m_Context = 0;
		NodeData* m_Parent = 0;
		ValueData* m_Value = 0;
		bool m_EndName = false;
		bool ContexHolder = true;
	};

	class GDF_Attribute
	{
	public:
		GDF_Attribute(AttributeData* _data);

		std::string Name();
		GDF_Attribute& Name(const std::string& _val);
		GDF_Attribute& Name(const char* _val);
		GDF_Value Value();
		bool IsNull();

		operator bool();
	private:
		AttributeData* m_Data = 0;
	};
}