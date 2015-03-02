#pragma once
#include "Value.h"

namespace FOGS
{
	class FOGS_Context;
	struct Node_impl;
	struct ValueData_impl;

	struct Attribute_impl
	{
		Attribute_impl(FOGS_Context* _context);
		~Attribute_impl();

		void ReadName();
		void NameToValue();
		bool IsUnexpacted(char* _char);
		bool NameReader(char* _char);

		void AppendAttribute();

		char* m_Name = 0;
		char* m_End = 0;

		Attribute_impl* m_Sibling = 0;
		FOGS_Context* m_Context = 0;
		Node_impl* m_Parent = 0;
		ValueData_impl* m_Value = 0;
		bool m_EndName = false;
		bool ContextHolder = true;
	};

	class Attribute
	{
	public:
		Attribute(Attribute_impl* _data);

		std::string Name();
		Attribute& Name(const std::string& _val);
		Attribute& Name(const char* _val);
		ValueData Value();
		bool IsNull();

		operator bool();
	private:
		Attribute_impl* m_Data = 0;
	};
}