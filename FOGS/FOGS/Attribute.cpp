#include "Attribute.h"
#include "Node.h"
#include "Context.h"



namespace FOGS
{
	Attribute_impl::Attribute_impl(FOGS_Context* _context)
	{
		m_Context = _context;
	}

	void Attribute_impl::ReadName()
	{
		*m_Context += new READER_ALLOC Reader<Attribute_impl>(this, &Attribute_impl::NameReader);
	}

	bool Attribute_impl::NameReader(char* _char)
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
			m_Value = new VALUE_DATA_ALLOC ValueData_impl(m_Context);
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
			m_Value = new VALUE_DATA_ALLOC ValueData_impl(m_Context);
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
			m_Value = new VALUE_DATA_ALLOC ValueData_impl(m_Context);
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

	bool Attribute_impl::IsUnexpacted(char* _char)
	{
		return
			*_char == '{' || *_char == '}' ||
			*_char == ']' ||	*_char == '(';
	}

	void Attribute_impl::NameToValue()
	{
		if (!m_Value)
			m_Value = new VALUE_DATA_ALLOC ValueData_impl(m_Context);
		auto Item = m_Value->AddItem(); 

		if (m_Name)
			*(m_End + 1) = 0;
		Item->String = m_Name;
		m_Name = 0;
	}

	Attribute_impl::~Attribute_impl()
	{
		ValueDataAlloc.Free(m_Value);

		if (!ContextHolder)
			delete[] m_Name;
	}

	void Attribute_impl::AppendAttribute()
	{
		if (!m_Parent->m_Attributes)
			m_Parent->m_Attributes = this;
		else
			m_Parent->m_AttLast->m_Sibling = this;

		m_Parent->m_AttLast = this;
		m_Parent->m_AttrSize++;
	}


	Attribute::Attribute(Attribute_impl* _data)
	{
		m_Data = _data;
	}

	std::string Attribute::Name()
	{
		return m_Data->m_Name;
	}

	Attribute& Attribute::Name(const std::string& _val)
	{
		auto lv_Size = _val.size();
		if (!m_Data->ContextHolder)
			delete m_Data->m_Name;
		m_Data->ContextHolder = false;

		if (_val.empty())
		{
			m_Data->m_Name = 0;
			return *this;
		}

		m_Data->m_Name = new char[lv_Size + 1];
		m_Data->m_Name[lv_Size] = 0;
		memcpy(m_Data->m_Name, _val.c_str(), lv_Size);

		return *this;
	}

	Attribute& Attribute::Name(const char* _val)
	{
		auto lv_Size = strlen(_val);
		if (!m_Data->ContextHolder)
			delete m_Data->m_Name;
		m_Data->ContextHolder = false;

		if (!_val)
		{
			m_Data->m_Name = 0;
			return *this;
		}

		m_Data->m_Name = new char[lv_Size + 1];
		m_Data->m_Name[lv_Size] = 0;
		memcpy(m_Data->m_Name, _val, lv_Size);
		return *this;
	}

	ValueData Attribute::Value()
	{
		if (!m_Data)
			return 0;

		if (!m_Data->m_Value)
		{
			m_Data->m_Value = new VALUE_DATA_ALLOC ValueData_impl(0);
			m_Data->m_Value->m_Attribute = m_Data;
		}

		return m_Data->m_Value;
	}

	Attribute::operator bool()
	{
		return m_Data != 0;
	}

	bool Attribute::IsNull()
	{
		return m_Data == 0;
	}

}