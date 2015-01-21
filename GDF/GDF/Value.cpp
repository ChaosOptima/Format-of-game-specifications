#include "Value.h"
#include "Node.h"
#include "Context.h"
#include <ctype.h>
#include <assert.h>
#include <string>

namespace GameDataFormat
{
	Allocator<sizeof(ValueItem)> ValueItemAlloc;
	Allocator<sizeof(ValueData)> ValueDataAlloc;

	ValueData::ValueData(GDF_Context* _context)
	{
		m_Context = _context;
	}

	void ValueData::ReadValue()
	{
		m_End = 0;
		*m_Context += new(ReaderAlloc.Alloc()) Reader<ValueData>(this, &ValueData::ValueReader);
	}

	bool ValueData::ValueReader(char* _char)
	{
		if (m_Context->CanSkip(_char))
		{
			return true;
		}
		else if (IsUnexpacted(_char))
			return false;
		else if (*_char == '\"')
			return ReadString();
		else if (*_char == '[')
			return ReadMeta();
		else if (*_char == ',')
		{
			if (m_CurrentValue && m_CurrentValue->String)
				*(m_End + 1) = 0;

			m_CurrentValue = 0;
			m_ValueReady = false;
			return true;
		}
		else if (*_char == ';')
		{
			m_Context->PopReader();
			if (m_CurrentValue && m_CurrentValue->String)
				*(m_End + 1) = 0;
			return true;
		}
		else if (*_char == ')')
		{
			if (!m_Attribute)
				return false;

			m_Context->PopReader();
			auto lv_Ret = m_Context->Read(_char);
			if (m_CurrentValue && m_CurrentValue->String)
				*(m_End + 1) = 0;
			return lv_Ret;
		}
		else if (*_char == '}')
		{
			if (!m_Node)
				return false;

			m_Context->PopReader();
			auto lv_Ret = m_Context->Read(_char);
			if (m_CurrentValue && m_CurrentValue->String)
				*(m_End + 1) = 0;
			return lv_Ret;
		}
		else if (*_char == '{')
		{
			if (!m_Node)
				return false;

			m_Context->PopReader();
			m_Node->ReadNodes();

			if (m_CurrentValue && m_CurrentValue->String)
				*(m_End + 1) = 0;
			return true;
		}
		else
		{
			if (m_ValueReady)
				return false;

			if (!m_CurrentValue)
			{
				m_CurrentValue = AddItem();
			}
			
			m_End = _char;
			if (!m_CurrentValue->String)
				m_CurrentValue->String = _char;
		}

		return true;
	}
	

	void ValueData::ReadValue(char* _lable)
	{
		ReadValue();
		m_CurrentValue = AddItem();
		m_CurrentValue->Lable = _lable;
		ReadMeta();
	}

	bool ValueData::IsUnexpacted(char* _char)
	{
		return
			*_char == '(' || *_char == ']' ||
			*_char == '=';
	}

	bool ValueData::ReadString()
	{
		m_Context->ReadAsString(true);
		if (!m_CurrentValue)
		{
			m_CurrentValue = AddItem();
		}

		m_CurrentValue->Type = VT_STRING;

		*m_Context += new(ReaderAlloc.Alloc()) Reader<ValueData>(this, &ValueData::StringReader);
		
		return true;
	}

	bool ValueData::StringReader(char* _char)
	{
		if (m_IgnorNext)
		{
			m_IgnorNext = false;
			return true;
		}

		if (*_char == '\\')
			m_IgnorNext = true;
		else if (*_char == '\"')
		{
			m_Context->ReadAsString(false);
			if (m_CurrentValue->String)
				*(m_End + 1) = 0;
			m_CurrentValue = 0;
			m_ValueReady = true;
			m_Context->PopReader();
			return true;
		}

		m_End = _char;
		if (!m_CurrentValue->String)
			m_CurrentValue->String = _char;

		return true;
	}

	bool ValueData::ReadMeta()
	{
		if (m_ValueReady)
			return false;

		if (!m_CurrentValue)
		{
			m_CurrentValue = AddItem();
		}
		else
		{
			if (m_CurrentValue->String)
			{
				*(m_End + 1) = 0;
				m_CurrentValue->Lable = m_CurrentValue->String;
				m_CurrentValue->String = 0;
			}
		}

		m_CurrentValue->Type = VT_META;

		m_Context->ReadAsMeta(true);
		*m_Context += new(ReaderAlloc.Alloc()) Reader<ValueData>(this, &ValueData::MetaReader);

		return true;
	}

	bool ValueData::MetaReader(char* _char)
	{
		if (m_IgnorNext)
		{
			m_IgnorNext = false;
			return true;
		}

		if (*_char == '\\')
			m_IgnorNext = true;
		else if (*_char == ']')
		{
			m_Context->ReadAsMeta(false);
			if (m_CurrentValue->String)
				*(m_End + 1) = 0;
			m_CurrentValue = 0;
			m_ValueReady = true;
			m_Context->PopReader();
			return true;
		}

		m_End = _char;
		if (!m_CurrentValue->String)
			m_CurrentValue->String = _char;

		return true;
	}

	ValueData::~ValueData()
	{
		auto lv_Val = m_Values;
		while (lv_Val)
		{
			auto lv_ForFree = lv_Val;
			lv_Val = lv_Val->Sibling;
			ValueItemAlloc.Free(lv_ForFree);
		}
	}

	ValueItem* ValueData::AddItem()
	{
		auto lv_NewItem = new(ValueItemAlloc.Alloc()) ValueItem();

		if (!m_Values)
		{
			m_Values = lv_NewItem;
			m_LastVal = lv_NewItem;
		}
		else
		{
			m_LastVal->Sibling = lv_NewItem;
			m_LastVal = lv_NewItem;
		}

		m_ValueSize++;
		return lv_NewItem;
	}



	GDF_Value::GDF_Value(ValueData* _data)
	{
		m_Data = _data;
	}

	GDF_Value::operator bool()
	{
		return m_Data->m_Values != 0;
	}

	bool GDF_Value::IsArray()
	{
		return m_Data->m_Values && m_Data->m_Values->Sibling;
	}

	GDF_ValueItem GDF_Value::Item()
	{
		if (!m_Data->m_Values)
			m_Data->AddItem();

		return m_Data->m_Values;
	}

	GameDataFormat::GDF_ValueItem GDF_Value::Item(int _ind)
	{
		auto lv_Val = m_Data->m_Values;
		for (int i = 0; lv_Val && i < _ind; i++)
			lv_Val = lv_Val->Sibling;

		return lv_Val;
	}

	unsigned int GDF_Value::ItemsCount()
	{
		return m_Data->m_ValueSize;
	}

	GameDataFormat::GDF_ValueItem GDF_Value::operator[](int _ind)
	{
		return Item(_ind);
	}

	std::vector<GDF_ValueItem> GDF_Value::Items()
	{
		std::vector<GDF_ValueItem> lv_Items;
		lv_Items.reserve(m_Data->m_ValueSize);
		for (auto lv_Val = m_Data->m_Values; lv_Val; lv_Val = lv_Val->Sibling)
			lv_Items.push_back(lv_Val);

		return lv_Items;
	}

	GDF_Value::operator GDF_ValueItem()
	{
		return m_Data->m_Values;
	}

	GDF_ValueItem GDF_Value::AppendItem()
	{
		auto lv_NewItem = m_Data->AddItem();
		return lv_NewItem;
	}

	GDF_ValueItem GDF_Value::operator*()
	{
		return Item();
	}

	bool GDF_Value::IsEmpty()
	{
		return !m_Data->m_Values;
	}

	//////////////////////////////////////////////////////////////////////////
	GDF_ValueItem::GDF_ValueItem(ValueItem* _data)
	{
		m_Data = _data;
	}

	std::string GDF_ValueItem::Lable()
	{
		return m_Data->Lable ?m_Data->Lable :"";
	}

	GDF_ValueItem& GDF_ValueItem::Lable(const std::string& _val)
	{
		m_Data->Type = VT_META;

		auto lv_Size = _val.size();
		if (!m_Data->ContexHolder)
			delete m_Data->Lable;

		m_Data->ContexHolder = false;

		m_Data->Lable = new char[lv_Size + 1];
		m_Data->Lable[lv_Size] = 0;
		memcpy(m_Data->Lable, _val.c_str(), lv_Size);
		return *this;
	}

	ValueType GDF_ValueItem::Type()
	{
		if (!m_Data)
			return VT_NON;

		if (m_Data->Type)
			return m_Data->Type;

		
		ValueType lv_Posible = VT_INT;
		bool DidgitPass = false;
		bool lv_EPassed = false;
		bool lv_ESignPassed = false;

		
		for (auto lv_Char = m_Data->String; lv_Char; lv_Char++)
		{
			if (lv_Posible == VT_INT)
			{
				if (!((*lv_Char >= '0' && *lv_Char <= '9') || ((*lv_Char == '-' || *lv_Char == '+') && !DidgitPass)))
				{
					if (*lv_Char == '.')
						lv_Posible = VT_FLOAT;
					else
					{
						lv_Posible = VT_CONSTANT;
						break;
					}
				}
				if ((*lv_Char == '-' || *lv_Char == '+') && !DidgitPass)
					DidgitPass = true;
			}
			else if (lv_Posible == VT_FLOAT)
			{
				if (!(*lv_Char >= '0' && *lv_Char <= '9'))
				{
					if (tolower(*lv_Char) == 'e' && !lv_EPassed)
						lv_EPassed = true;
					else if ((*lv_Char == '-' || *lv_Char == '+') && lv_EPassed && !lv_ESignPassed)
						lv_ESignPassed = true;
					else
					{
						lv_Posible = VT_CONSTANT;
						break;
					}
				}
			}
		}

		if (lv_Posible == VT_INT)
		{
			m_Data->AsInt = stoll((std::string)m_Data->String);
		}
		else if (lv_Posible == VT_FLOAT)
		{
			m_Data->AsDouble = stod((std::string)m_Data->String);
		}
		if (lv_Posible == VT_CONSTANT)
		{
			std::string lv_Tmp;
			for (auto lv_Char : (std::string)m_Data->String)
				lv_Tmp += tolower(lv_Char);

			if (lv_Tmp == "true" || lv_Tmp == "yes" || lv_Tmp == "on")
			{
				lv_Posible = VT_BOOL;
				m_Data->AsBool = true;
			}
			else if (lv_Tmp == "false" || lv_Tmp == "no" || lv_Tmp == "off")
			{
				lv_Posible = VT_BOOL;
				m_Data->AsBool = false;
			}
			else if (lv_Tmp == "pi")
			{
				lv_Posible = VT_FLOAT;
				m_Data->AsDouble = 3.141592653589;
			}
			else if (lv_Tmp == "e")
			{
				lv_Posible = VT_FLOAT;
				m_Data->AsDouble = 2.718281828459;
			}
		}

		m_Data->Type = lv_Posible;
		return m_Data->Type;
	}

	GDF_ValueItem& GDF_ValueItem::SetConstant(const std::string& _val)
	{
		m_Data->Type = VT_CONSTANT;

		auto lv_Size = _val.size();
		if (!m_Data->ContexHolder)
			delete m_Data->String;
		m_Data->ContexHolder = false;


		m_Data->String = new char[lv_Size + 1];
		m_Data->String[lv_Size] = 0;
		memcpy(m_Data->String, _val.c_str(), lv_Size);
		return *this;
	}

	GDF_ValueItem& GDF_ValueItem::operator=(long long _val)
	{
		m_Data->Type = VT_INT;
		m_Data->AsInt = _val;
		return *this;
	}

	GDF_ValueItem& GDF_ValueItem::operator=(long double _val)
	{
		m_Data->Type = VT_FLOAT;
		m_Data->AsDouble = _val;
		return *this;
	}

	GDF_ValueItem& GDF_ValueItem::operator=(bool _val)
	{
		m_Data->Type = VT_BOOL;
		m_Data->AsDouble = _val;
		return *this;
	}

	GDF_ValueItem& GDF_ValueItem::operator=(const std::string& _val)
	{
		m_Data->Type = VT_STRING;

		auto lv_Size = _val.size();
		if (!m_Data->ContexHolder)
			delete m_Data->String;
		m_Data->ContexHolder = false;

		m_Data->String = new char[lv_Size + 1];
		m_Data->String[lv_Size] = 0;
		memcpy(m_Data->String, _val.c_str(), lv_Size);

		return *this;
	}

	GDF_ValueItem& GDF_ValueItem::operator=(int _val)
	{
		m_Data->Type = VT_INT;
		m_Data->AsInt = _val;
		return *this;
	}

	GDF_ValueItem& GDF_ValueItem::operator=(float _val)
	{
		m_Data->Type = VT_FLOAT;
		m_Data->AsDouble = _val;
		return *this;
	}

	GDF_ValueItem& GDF_ValueItem::operator=(const char* _val)
	{
		m_Data->Type = VT_STRING;
		auto lv_Size = strlen(_val);
		if (!m_Data->ContexHolder)
			delete m_Data->String;
		m_Data->ContexHolder = false;

		m_Data->String = new char[lv_Size + 1];
		m_Data->String[lv_Size] = 0;
		memcpy(m_Data->String, _val, lv_Size);
		return *this;
	}

	GDF_ValueItem& GDF_ValueItem::operator=(double _val)
	{
		m_Data->Type = VT_FLOAT;
		m_Data->AsDouble = _val;
		return *this;
	}

	long long GDF_ValueItem::AsInt()
	{
		auto lv_Type = Type();
		if (lv_Type == VT_INT)
			return m_Data->AsInt;
		else if (lv_Type == VT_FLOAT)
			return (long long)m_Data->AsDouble;
		else if (lv_Type == VT_BOOL)
			return (long long)m_Data->AsBool;
		else if (lv_Type == VT_CONSTANT || lv_Type == VT_STRING || lv_Type == VT_META)
		{
			ASSERT_BREAK(false);
			return std::stoi(m_Data->String);
		}
		else
		{
			ASSERT_BREAK(false);
			return 0;
		}
	}

	long double GDF_ValueItem::AsDouble()
	{
		auto lv_Type = Type();
		if (lv_Type == VT_FLOAT)
			return m_Data->AsDouble;
		else if (lv_Type == VT_INT)
			return (long double)m_Data->AsInt;
		else if (lv_Type == VT_BOOL)
				return (long double)m_Data->AsBool;
		else if (lv_Type == VT_CONSTANT || lv_Type == VT_STRING || lv_Type == VT_META)
		{
			ASSERT_BREAK(false);
			return std::stod(m_Data->String);
		}
		else
		{
			ASSERT_BREAK(false);
			return 0;
		}
		
	}

	bool GDF_ValueItem::AsBool()
	{
		auto lv_Type = Type();
		if (lv_Type == VT_BOOL)
			return m_Data->AsBool;
		else
		{
			if (lv_Type == VT_FLOAT)
				return m_Data->AsDouble != 0;
			else if(lv_Type == VT_INT)
				return m_Data->AsInt != 0;
			else if (lv_Type == VT_CONSTANT || lv_Type == VT_STRING || lv_Type == VT_META)
				return m_Data->String != 0;
			else
				return false;
		}
	}

	std::string GDF_ValueItem::AsString()
	{
		auto lv_Type = Type();

		if (lv_Type == VT_STRING || lv_Type == VT_META || lv_Type == VT_CONSTANT)
		{
			if (m_Data->ClearString)
			{
				m_Data->ClearString = false;
				std::string lv_out;
				bool lv_ignore = false;

				for (auto lv_Char = m_Data->String; *lv_Char; lv_Char++)
				{
					if (lv_ignore)
					{
						lv_ignore = false;
						lv_out += *lv_Char;
						continue;
					}

					if (*lv_Char == '\\')
					{
						lv_ignore = true;
						continue;
					}

					if (lv_Type == VT_STRING && (*lv_Char == '\t' || *lv_Char == '\n'))
						continue;

					lv_out += *lv_Char;
				}

				auto lv_Size = lv_out.size();
				if (!m_Data->ContexHolder)
					delete m_Data->String;
				m_Data->ContexHolder = false;

				m_Data->String = new char[lv_Size + 1];
				m_Data->String[lv_Size] = 0;
				memcpy(m_Data->String, lv_out.c_str(), lv_Size);
			}


			return m_Data->String;
		}
		else
		{
			if (lv_Type == VT_FLOAT)
			{
				return std::to_string(m_Data->AsDouble);
			}
			else if (lv_Type == VT_INT)
			{
				return std::to_string(m_Data->AsInt);
			}
			else if (lv_Type == VT_BOOL)
			{
				m_Data->String = m_Data->AsBool ? "true" : "false";
				return m_Data->String;
			}
			else
			{
				m_Data->String = 0;
				return m_Data->String;
			}
		}
	}

	GDF_ValueItem::operator int()
	{
		return (int)AsInt();
	}

	GDF_ValueItem::operator unsigned int()
	{
		return (unsigned int)AsInt();
	}

	GDF_ValueItem::operator long()
	{
		return (long)AsInt();
	}

	GDF_ValueItem::operator unsigned long()
	{
		return (unsigned long)AsInt();
	}

	GDF_ValueItem::operator unsigned long long()
	{
		return (unsigned long long)AsInt();
	}

	GDF_ValueItem::operator long long()
	{
		return AsInt();
	}

	GDF_ValueItem::operator float()
	{
		return (float)AsDouble();
	}

	GDF_ValueItem::operator double()
	{
		return AsDouble();
	}

	GDF_ValueItem::operator long double()
	{
		return AsDouble();
	}

	GDF_ValueItem::operator bool()
	{
		return AsBool();
	}

	GDF_ValueItem::operator std::string()
	{
		return AsString();
	}

	GDF_ValueItem& GDF_ValueItem::SetString(const std::string& _val)
	{
		return *this = _val;
	}

	GDF_ValueItem& GDF_ValueItem::SetInt(long long _val)
	{
		return *this = _val;
	}

	GDF_ValueItem& GDF_ValueItem::SetFloat(long double _val)
	{
		return *this = _val;
	}

	GDF_ValueItem& GDF_ValueItem::SetBool(long long _val)
	{
		return *this = _val;
	}

	

}

