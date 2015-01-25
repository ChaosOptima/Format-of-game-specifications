#include "Context.h"
#include "Value.h"

namespace FOGS
{
	Allocator<sizeof(Reader<IReader>)> ReaderAlloc;

	FOGS_Context::FOGS_Context(const std::string& _data)
	{
		m_Data = _data.c_str();
		m_DataCar = m_Data;
	}


	void FOGS_Context::PushReader(IReader* _reader)
	{
		_reader->Priv = m_Readers;
		m_Readers = _reader;
	}

	void FOGS_Context::operator+=(IReader* _reader)
	{
		PushReader(_reader);
	}


	void FOGS_Context::PopReader()
	{
		ASSERT_BREAK(m_Readers);
		auto lv_Cur = m_Readers;
		m_Readers = lv_Cur->Priv;
		ReaderAlloc.Free(lv_Cur);
	}

	bool FOGS_Context::Read(const char* _val)
	{
		if (!m_Readers)
			return false;

		return m_Readers->Read(_val);
	}

	void FOGS_Context::UpdatePositionInf(const char* _char)
	{
		if (*_char == '\n')
		{
			m_Line++;
			m_Char = 1;
		}
		else
		{
			m_Char++;
		}
	}

	bool FOGS_Context::GetReady()
	{
		for (; *m_DataCar; m_DataCar++)
		{
			UpdatePositionInf(m_DataCar);

			if (*m_DataCar == '{')
			{
				m_DataCar++;
				return true;
			}
		}

		return false;
	}

	unsigned int FOGS_Context::Line()
	{
		return m_Line;
	}

	unsigned int FOGS_Context::Char()
	{
		return m_Char;
	}

	std::string FOGS_Context::String()
	{
		int lv_size = m_DataCar - m_Data;
		lv_size = lv_size > 10 ? 10 : lv_size;

		m_String.clear();
		m_String.insert(lv_size, m_DataCar - lv_size);
		return m_String;
	}

	bool FOGS_Context::Parse()
	{
		for (; *m_DataCar; m_DataCar++)
		{
			UpdatePositionInf(m_DataCar);

			if (m_InString || m_InMeta)
			{
				if (!Read(m_DataCar))
					return false;
				
				continue;
			}
			
			if (!m_Readers)
				return true;
			else
			{
				if (!CheckComments())
					if (!Read(m_DataCar))
						return false;
			}
		}

		return !m_Readers;
	}

	bool FOGS_Context::CheckComments()
	{
		if (*(m_DataCar + 1))
		{
			if (*m_DataCar == '/' && *(m_DataCar + 1) == '/')
			{
				for (m_DataCar++; *m_DataCar; m_DataCar++)
				{
					UpdatePositionInf(m_DataCar);

					if (*m_DataCar == '\n')
						break;
				}

				return true;
			}
			else if (*m_DataCar == '/' && *(m_DataCar + 1) == '*')
			{
				m_DataCar++;
				UpdatePositionInf(m_DataCar);

				for (m_DataCar++; *m_DataCar; m_DataCar++)
				{
					UpdatePositionInf(m_DataCar);

					if (*m_DataCar == '*' && *(m_DataCar + 1) == '/')
					{
						m_DataCar++;
						break;
					}
				}

				return true;
			}
		}

		return false;			
	}

	bool FOGS_Context::CanSkip(const char* _char)
	{
		return		
			*_char == ' ' ||
			*_char == '\n' ||
			*_char == '\t';
	}

	void FOGS_Context::ReadAsString(bool _val)
	{
		m_InString = _val;
	}

	void FOGS_Context::ReadAsMeta(bool _val)
	{
		m_InMeta = _val;
	}





}