#include <iostream>
#include "GDF/GDF.h"

using namespace std;
using namespace GameDataFormat;

void TestFile()
{
	string TestString;
	{
		FILE* lv_File;
		fopen_s(&lv_File, "../Test.txt", "r");
		fseek(lv_File, 0, SEEK_END);
		unsigned int lv_size = ftell(lv_File);
		fseek(lv_File, 0, SEEK_SET);
		TestString.resize(lv_size);
		fread((void*)TestString.c_str(), lv_size, 1, lv_File);
		fclose(lv_File);
	}

	GDF_Document lv_doc;
	if (!lv_doc.Load(TestString))
		cout << "Error at ln:" << lv_doc.ErrorLine() << " ch:" << lv_doc.ErrorChar();// << " " << lv_doc.ErrorString();
	else
	{
		string lv_Str;
		lv_Str = lv_doc.Save();
		FILE* lv_File;
		fopen_s(&lv_File, "../Out1.txt", "w");
		fwrite(lv_Str.c_str(), lv_Str.length(), 1, lv_File);
		fclose(lv_File);
	}
}


/*
void FillTest()
{
	GDF_Document lv_doc;
	auto lv_Root = lv_doc.Root();
	for (int i = 0; i < 20; i++)
	{
		auto lv_Ch = lv_Root.AppendChild().Name("ololo");
		{
			lv_Ch.AppendAttribute().Name("Id").Value() = i;

			lv_Ch.Value() += 666;
			lv_Ch.Value() += 42.2f;
			lv_Ch.Value().AppendItem().SetString("ya\n.ru").Lable("url");

			lv_Ch.AppendChild().Value() = 13;
			lv_Ch.AppendChild().Value() = 42.2f;
			lv_Ch.AppendChild().Value().Item().SetConstant("url[ya.ru]");
		}

		auto lv_at = lv_Ch.AppendAttribute().Name("atrasd");
		{
			lv_at.Value() += 10;
			lv_at.Value() += 4.55f;
			lv_at.Value() += "Array";
		}		
	}

	string lv_Str;
	lv_Str = lv_doc.Save();
	FILE* lv_File;
	fopen_s(&lv_File, "../Out2.txt", "w");
	fwrite(lv_Str.c_str(), lv_Str.length(), 1, lv_File);
	fclose(lv_File);
	cout << lv_Str;
	
}*/


void main()
{

	TestFile();
	//FillTest();

	cin.ignore();
}