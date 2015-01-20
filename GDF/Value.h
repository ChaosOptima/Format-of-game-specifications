#pragma once
#include <list>
#include <vector>
#include "Allocator.h"

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
	class GDF_Context;
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
	};

	struct ValueData
	{
		ValueData(GDF_Context* _context);
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
		GDF_Context* m_Context = 0;
		ValueItem* m_CurrentValue = 0;
		bool m_IgnorNext = false;
		bool m_ValueReady = false;
		char* m_End = 0;
	
	};

	extern Allocator<sizeof(ValueItem)> ValueItemAlloc;
	extern Allocator<sizeof(ValueData)> ValueDataAlloc;

	class GDF_ValueItem
	{
	public:
		GDF_ValueItem(ValueItem* _data);

		std::string Lable();
		GDF_ValueItem& Lable(const std::string& _val);

		ValueType Type();

		std::string AsString();
		long double AsDouble();
		long long AsInt();
		bool AsBool();

		GDF_ValueItem& SetConstant(const std::string& _val);
		GDF_ValueItem& SetString(const std::string& _val);
		GDF_ValueItem& SetInt(long long _val);
		GDF_ValueItem& SetFloat(long double _val);
		GDF_ValueItem& SetBool(long long _val);
		
		GDF_ValueItem& operator = (int _val);
		GDF_ValueItem& operator = (long long _val);
		GDF_ValueItem& operator = (float _val);
		GDF_ValueItem& operator = (double _val);
		GDF_ValueItem& operator = (long double _val);
		GDF_ValueItem& operator = (bool _val);
		GDF_ValueItem& operator = (const std::string& _val);
		GDF_ValueItem& operator = (const char* _val);
		
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

	class GDF_Value
	{
	public:
		GDF_Value(ValueData* _data);

		bool IsArray();
		bool IsEmpty();

		unsigned int ItemsCount();
		GDF_ValueItem Item();
		GDF_ValueItem Item(int _ind);
		GDF_ValueItem AppendItem();
		std::vector<GDF_ValueItem> Items();

		operator bool();
		operator GDF_ValueItem();
		GDF_ValueItem operator[](int _ind);
		GDF_ValueItem operator*();

		template<class T>
		GDF_ValueItem operator+=(T _val)
		{
			auto lv_Item = AppendItem();
			lv_Item = _val;
			return lv_Item;
		}

		template<class T>
		GDF_ValueItem operator=(T _val)
		{
			auto lv_Item = Item();
			lv_Item = _val;
			return lv_Item;
		}

	private:
		ValueData* m_Data = 0;
	};

}