#pragma once
#include <stack>
#include <functional>
#include <memory>
#include "Allocator.h"

namespace GameDataFormat
{
	struct IReader
	{
		IReader* Priv = 0;

		virtual bool Read(const char*) = 0;
		virtual ~IReader(){}
	};

	template<class T>
	struct Reader : IReader
	{
		T* Obj = 0;
		bool(T::*Meth)(char*) = 0;

		Reader(T* _Obj, bool(T::*_Meth)(char*)) :
			IReader(),
			Obj(_Obj),
			Meth(_Meth)
		{}
		
		bool Read(const char* _char)
		{
			return (Obj->*Meth)((char*)_char);
		}
	};

	extern Allocator<sizeof(Reader<IReader>)> ReaderAlloc;

	class FOGS_Context
	{
	public:
		//typedef std::function<bool(char)> Reader;

		FOGS_Context(const std::string& _data);

		void operator+=(IReader* _reader);

		void PushReader(IReader* _reader);
		void PopReader();
		bool GetReady();

		unsigned int Line();
		unsigned int Char();
		std::string String();

		bool Read(const char* _val);
		bool Parse();
		bool CanSkip(const char* _char);
		void ReadAsString(bool _val);
		void ReadAsMeta(bool _val);

	private:
		void UpdatePositionInf(const char* _char);
		bool CheckComments();

		IReader* m_Readers = 0;

		//unsigned int m_Position = 0;
		unsigned int m_Line = 1;
		unsigned int m_Char = 1;
		std::string m_String;
		const char* m_Data = 0;
		const char* m_DataCar = 0;
		//unsigned int m_DataLenght;
		bool m_InString = false;
		bool m_InMeta = false;
	};


}