#pragma once
#include <list>
#include <vector>
#include "Allocator.h"

#ifdef LEACK_DEBUG

	#define _CRTDBG_MAP_ALLOC 1
	#define _CheckLeaks _CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF)
	#define _BreakAllock(x) _CrtSetBreakAlloc(x)
	#include <stdlib.h>
	#include <crtdbg.h>

	#define new new(_NORMAL_BLOCK, __FILE__, __LINE__)

	#define READER_ALLOC 
	#define VALUE_ITEM_ALLOC
	#define VALUE_DATA_ALLOC
	#define NODE_ALLOC 
	#define ATTR_ALLOC

#else
	#ifndef _CheckLeaks
		#define _CheckLeaks
	#endif // !_CheckLeaks

	
	#define READER_ALLOC (ReaderAlloc.Alloc())
	#define VALUE_ITEM_ALLOC (ValueItemAlloc.Alloc())
	#define VALUE_DATA_ALLOC (ValueDataAlloc.Alloc())
	#define NODE_ALLOC (NodeAlloc.Alloc())
	#define ATTR_ALLOC (AttrAlloc.Alloc())

#endif // LEACK_DEBUG



#ifdef _DEBUG
	#ifndef ASSERT_BREAK
		#define ASSERT_BREAK( X ) {if(!(X)){__debugbreak();}}
	#endif
#else
	#ifndef ASSERT_BREAK
		#define ASSERT_BREAK( IGNORE ) {}
	#endif
#endif // DEBUG


namespace FOGS
{
	class FOGS_Context;
	struct Node_impl;
	struct Attribute_impl;

	enum ValueType
	{
		VT_NON = 0,
		VT_INT,
		VT_FLOAT,
		VT_BOOL,
		VT_CONSTANT,
		VT_STRING,
		VT_META,
	};

	struct ValueItem_impl
	{
		char* Lable = 0;
		char* String = 0;

		long long AsInt = 0;
		long double AsDouble = 0;
		bool AsBool = false;
		bool ClearString = true;
		bool ContexHolder = true;

		ValueType Type = VT_NON;
		ValueItem_impl* Sibling = 0;

		void FreeStrings();
		~ValueItem_impl();
	};

	struct ValueData_impl
	{
		ValueData_impl(FOGS_Context* _context);
		~ValueData_impl();

		void ReadValue();
		void ReadValue(char* _lable);
		bool IsUnexpacted(char* _char);
		bool ReadString();
		bool ReadMeta();
		ValueItem_impl* AddItem();
		
		bool ValueReader(char* _char);
		bool StringReader(char* _char);
		bool MetaReader(char* _char);

		ValueItem_impl* m_Values = 0;
		ValueItem_impl* m_LastVal = 0;
		unsigned int m_ValueSize = 0;

		Node_impl* m_Node = 0;
		Attribute_impl* m_Attribute = 0;
		FOGS_Context* m_Context = 0;
		ValueItem_impl* m_CurrentValue = 0;
		bool m_IgnorNext = false;
		bool m_ValueReady = false;
		char* m_End = 0;
	
	};

	extern Allocator<sizeof(ValueItem_impl)> ValueItemAlloc;
	extern Allocator<sizeof(ValueData_impl)> ValueDataAlloc;

	class ValueItem
	{
	public:
		ValueItem(ValueItem_impl* _data);

		std::string Lable();
		ValueItem& Lable(const std::string& _val);

		ValueType Type();

		std::string AsString();
		long double AsDouble();
		long long AsInt();
		bool AsBool();

		ValueItem& SetConstant(const std::string& _val);
		ValueItem& SetString(const std::string& _val);
		ValueItem& SetInt(long long _val);
		ValueItem& SetFloat(long double _val);
		ValueItem& SetBool(long long _val);
		
		ValueItem& operator = (int _val);
		ValueItem& operator = (long long _val);
		ValueItem& operator = (float _val);
		ValueItem& operator = (double _val);
		ValueItem& operator = (long double _val);
		ValueItem& operator = (bool _val);
		ValueItem& operator = (const std::string& _val);
		ValueItem& operator = (const char* _val);
		
		operator int();
		operator unsigned int();
		operator long();
		operator unsigned long();
		operator long long();
		operator unsigned long long();
		operator float();
		operator double();
		operator long double();
		operator bool();
		operator std::string();

	private:
		ValueItem_impl* m_Data = 0;
	};

	class ValueData
	{
	public:
		ValueData(ValueData_impl* _data);

		bool IsArray();
		bool IsEmpty();

		unsigned int ItemsCount();
		ValueItem Item();
		ValueItem Item(int _ind);
		ValueItem AppendItem();
		std::vector<ValueItem> Items();

		operator bool();
		operator ValueItem();
		ValueItem operator[](int _ind);
		ValueItem operator*();

		template<class T>
		ValueData& operator+=(T _val)
		{
			auto lv_Item = AppendItem();
			lv_Item = _val;
			return *this;
		}

		template<class T>
		ValueData& operator=(T _val)
		{
			auto lv_Item = Item();
			lv_Item = _val;
			return *this;
		}

	private:
		ValueData_impl* m_Data = 0;
	};

	template<class T>
	ValueData& operator,(ValueData& _val, T _rh)
	{
		return _val += _rh;
	}

}