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


namespace GameDataFormat
{
	class FOGS_Context;
	struct NodeData;
	struct AttributeData;

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

	struct ValueItem
	{
		char* Lable = 0;
		char* String = 0;

		long long AsInt = 0;
		long double AsDouble = 0;
		bool AsBool = false;
		bool ClearString = true;
		bool ContexHolder = true;

		ValueType Type = VT_NON;
		ValueItem* Sibling = 0;

		void FreeStrings();
		~ValueItem();
	};

	struct ValueData
	{
		ValueData(FOGS_Context* _context);
		~ValueData();

		void ReadValue();
		void ReadValue(char* _lable);
		bool IsUnexpacted(char* _char);
		bool ReadString();
		bool ReadMeta();
		ValueItem* AddItem();
		
		bool ValueReader(char* _char);
		bool StringReader(char* _char);
		bool MetaReader(char* _char);

		ValueItem* m_Values = 0;
		ValueItem* m_LastVal = 0;
		unsigned int m_ValueSize = 0;

		NodeData* m_Node = 0;
		AttributeData* m_Attribute = 0;
		FOGS_Context* m_Context = 0;
		ValueItem* m_CurrentValue = 0;
		bool m_IgnorNext = false;
		bool m_ValueReady = false;
		char* m_End = 0;
	
	};

	extern Allocator<sizeof(ValueItem)> ValueItemAlloc;
	extern Allocator<sizeof(ValueData)> ValueDataAlloc;

	class FOGS_ValueItem
	{
	public:
		FOGS_ValueItem(ValueItem* _data);

		std::string Lable();
		FOGS_ValueItem& Lable(const std::string& _val);

		ValueType Type();

		std::string AsString();
		long double AsDouble();
		long long AsInt();
		bool AsBool();

		FOGS_ValueItem& SetConstant(const std::string& _val);
		FOGS_ValueItem& SetString(const std::string& _val);
		FOGS_ValueItem& SetInt(long long _val);
		FOGS_ValueItem& SetFloat(long double _val);
		FOGS_ValueItem& SetBool(long long _val);
		
		FOGS_ValueItem& operator = (int _val);
		FOGS_ValueItem& operator = (long long _val);
		FOGS_ValueItem& operator = (float _val);
		FOGS_ValueItem& operator = (double _val);
		FOGS_ValueItem& operator = (long double _val);
		FOGS_ValueItem& operator = (bool _val);
		FOGS_ValueItem& operator = (const std::string& _val);
		FOGS_ValueItem& operator = (const char* _val);
		
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
		ValueItem* m_Data = 0;
	};

	class FOGS_Value
	{
	public:
		FOGS_Value(ValueData* _data);

		bool IsArray();
		bool IsEmpty();

		unsigned int ItemsCount();
		FOGS_ValueItem Item();
		FOGS_ValueItem Item(int _ind);
		FOGS_ValueItem AppendItem();
		std::vector<FOGS_ValueItem> Items();

		operator bool();
		operator FOGS_ValueItem();
		FOGS_ValueItem operator[](int _ind);
		FOGS_ValueItem operator*();

		template<class T>
		FOGS_ValueItem operator+=(T _val)
		{
			auto lv_Item = AppendItem();
			lv_Item = _val;
			return lv_Item;
		}

		template<class T>
		FOGS_ValueItem operator=(T _val)
		{
			auto lv_Item = Item();
			lv_Item = _val;
			return lv_Item;
		}

	private:
		ValueData* m_Data = 0;
	};

}