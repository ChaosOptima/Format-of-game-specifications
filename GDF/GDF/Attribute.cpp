#include "Attribute.h"
#include "Node.h"
#include "Context.h"

namespace GameDataFormat
{
	AttributeData::AttributeData(GDF_Context* _context)
	{
		m_Context = _context;
	}

	void AttributeData::ReadName()
	{
		*m_Context += new(ReaderAlloc.Alloc()) Reader<AttributeData>(this, &AttributeData::NameReader);
	}

	bool AttributeData::NameReader(char* _char)
	{
		if (m_Context->CanSkip(_char))
		{
			m_EndName = true;
			return true;
		}
		else if (IsUnexpacted(_char))
			return false;
		else if (*_char == ')')
		{
			if (!m_Name)
			{
				m_Context->PopReader();
				m_Context->PopReader();
				delete this;
				return true;
			}
			else if (!m_Value)
				NameToValue();

			m_Sibling = m_Parent->m_Attributes;
			m_Parent->m_Attributes = this;
			m_Parent->m_AttrSize++;

			m_Context->PopReader();
			m_Context->PopReader();
			if (m_Name)
				*(m_End + 1) = 0;
			return true;
		}
		else if (*_char == ';')
		{
			if (!m_Name)
			{
				m_Context->PopReader();
				delete this;
				return true;
			}
			else if (!m_Value)
				NameToValue();
				
			m_Sibling = m_Parent->m_Attributes;
			m_Parent->m_Attributes = this;
			m_Parent->m_AttrSize++;
			m_Context->PopReader();
			if (m_Name)
				*(m_End + 1) = 0;
			return true;
		}
		else if (*_char == ',')
		{
			if (m_Value || !m_Name)
				return false;
				
			NameToValue();
			m_Context->PopReader();
			m_Value->ReadValue();
			m_Value->m_Attribute = this;

			AppendAttribute();
			if (m_Name)
				*(m_End + 1) = 0;
			return true;
		}
		else if (*_char == '[')
		{
			if (m_Value)
				return false;

			m_Context->PopReader();
			AppendAttribute();
			m_Value = new(ValueDataAlloc.Alloc()) ValueData(m_Context);
			m_Value->m_Attribute = this;
			m_Value->ReadValue(m_Name);
			m_Name = 0;
			return true;
		}
		else if (*_char == '=')
		{
			if (m_Value && !m_Name)
				return false;

			m_Context->PopReader();
			AppendAttribute();
			m_Value = new(ValueDataAlloc.Alloc()) ValueData(m_Context);
			m_Value->m_Attribute = this;
			m_Value->ReadValue();
			if (m_Name)
				*(m_End + 1) = 0;
			return true;
		}
		else if (*_char == '"')
		{
			if (m_Name)
				return false;

			m_Context->PopReader();
			AppendAttribute();
			m_Value = new(ValueDataAlloc.Alloc()) ValueData(m_Context);
			m_Value->m_Attribute = this;
			m_Value->ReadValue();
			auto lv_Ret = m_Context->Read(_char);
			if (m_Name)
				*(m_End + 1) = 0;
			return lv_Ret;
		}
		else
		{
			if (m_EndName)
				return false;
			
			m_End = _char;
			if (!m_Name)
				m_Name = _char;
		}
			
		return true;
	}

	bool AttributeData::IsUnexpacted(char* _char)
	{
		return
			*_char == '{' || *_char == '}' ||
			*_char == ']' ||	*_char == '(';
	}

	void AttributeData::NameToValue()
	{
		if (!m_Value)
			m_Value = new(ValueDataAlloc.Alloc()) ValueData(m_Context);
		auto Item = m_Value->AddItem(); 

		if (m_Name)
			*(m_End + 1) = 0;
		Item->String = m_Name;
		m_Name = 0;
	}

	AttributeData::~AttributeData()
	{
		ValueDataAlloc.Free(m_Value);
	}

	void AttributeData::AppendAttribute()
	{
		if (!m_Parent->m_Attributes)
			m_Parent->m_Attributes = this;
		else
			m_Parent->m_AttLast->m_Sibling = this;

		m_Parent->m_AttLast = this;
		m_Parent->m_AttrSize++;
	}


	GDF_Attribute::GDF_Attribute(AttributeData* _data)
	{
		m_Data = _data;
	}

	std::string GDF_Attribute::Name()
	{
		return m_Data->m_Name;
	}

	GDF_Attribute& GDF_Attribute::Name(const std::string& _val)
	{
		auto lv_Size = _val.size();
		if (!m_Data->ContexHolder)
			delete m_Data->m_Name;
		m_Data->ContexHolder = false;

		m_Data->m_Name = new char[lv_Size + 1];
		m_Data->m_Name[lv_Size] = 0;
		memcpy(m_Data->m_Name, _val.c_str(), lv_Size);

		return *this;
	}

	GDF_Attribute& GDF_Attribute::Name(const char* _val)
	{
		auto lv_Size = strlen(_val);
		if (!m_Data->ContexHolder)
			delete m_Data->m_Name;
		m_Data->ContexHolder = false;

		m_Data->m_Name = new char[lv_Size + 1];
		m_Data->m_Name[lv_Size] = 0;
		memcpy(m_Data->m_Name, _val, lv_Size);
		return *this;
	}

	GDF_Value GDF_Attribute::Value()
	{
		if (!m_Data)
			return 0;

		if (!m_Data->m_Value)
		{
			m_Data->m_Value = new(ValueDataAlloc.Alloc()) ValueData(0);
			m_Data->m_Value->m_Attribute = m_Data;
		}

		return m_Data->m_Value;
	}

	GDF_Attribute::operator bool()
	{
		return m_Data != 0;
	}

	bool GDF_Attribute::IsNull()
	{
		return m_Data == 0;
	}

}