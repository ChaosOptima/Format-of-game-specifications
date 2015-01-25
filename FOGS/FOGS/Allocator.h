#pragma once



// 1MB
#define PAGE_SIZE 1048576
// 1KB
// #define PAGE_SIZE 1024

//#define LEACK_DEBUG

namespace FOGS
{
	template< int Size >
	class Allocator
	{
	public:

		Allocator()
		{
			static_assert(Size >= sizeof(char*), "Only big size");
		}

		void* Alloc()
		{
			if (!m_Memory)
			{
				AllocPage();
				m_Memory = m_Page;
			}

			void* lv_Dest = m_Curpos;

			if (*((char**)m_Curpos))
				m_Curpos = *((char**)m_Curpos);
			else if (m_Curpos + Size >= m_Page + m_PageSize)
				AllocPage();
			else
			{
				m_Curpos += Size;
				*((char**)m_Curpos) = 0;
			}

			m_Allocs++;
			return lv_Dest;
		}

		template<class T>
		void Free(T* _ptr)
		{
#ifdef LEACK_DEBUG
			delete _ptr;
			return;
#endif

			if (!_ptr)
				return;

			char* lv_Ptr = (char*)_ptr;
			_ptr->~T();
			*((char**)lv_Ptr) = m_Curpos;
			m_Curpos = lv_Ptr;

			m_Allocs--;
			if (!m_Allocs)
			{
				Clear();
			}

		}

		void Clear()
		{
			char* lv_Page = m_Memory;
			while (lv_Page)
			{
				auto m_Cur = lv_Page;
				lv_Page = *((char**)lv_Page);
				delete[] m_Cur;
			}

			m_Memory = 0;
			m_Page = 0;
			m_Curpos = 0;
		}


	private:
		static const long m_PageSize = (PAGE_SIZE / Size) * Size + sizeof(char*);
		long m_Allocs = 0;

		char* m_Memory = 0;
		char* m_Page = 0;
		char* m_Curpos = 0;

		void AllocPage()
		{
			char* lv_NewPage = new char[m_PageSize];

			if (m_Page)
				*((char**)m_Page) = lv_NewPage;

			m_Page = lv_NewPage;
			m_Curpos = m_Page;
			*((char**)m_Curpos) = 0;
			m_Curpos += sizeof(char*);
			*((char**)m_Curpos) = 0;
		}
	};
}
