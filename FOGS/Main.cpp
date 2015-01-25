
#include <iostream>
#include "FOGS/FOGS.h"

using namespace std;
using namespace FOGS;

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

	FOGS_Document lv_doc;
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

class Ins : public Serializable < FOGS_Node >
{
	float x = 20;
	float y = 523;
	float z = 12;

	serialize(x);
	serialize(y);
	serialize(z);
};


class SerTest : public Serializable < FOGS_Node >
{
	int i = 10;
	float j = 5;

	list<Ins*> List;

	serialize(List);
	serialize(j);
	serialize(i);
	
public:
	SerTest()
	{
		for (int i = 0; i < 5; i++)
			List.push_back(new Ins());
	}
};

void FillTest() 
{
	FOGS_Document lv_doc;
	auto lv_Root = lv_doc.Root();

	{
		SerTest lv_Test;
		lv_Test.Serialize(lv_Root);
	}

	string lv_Str;
	lv_Str = lv_doc.Save();
	cout << lv_Str;

	{
		SerTest lv_Test2;
		lv_Test2.Deserialize(lv_Root);

		cout << "\n";
	}

	 
	
	FILE* lv_File;
	fopen_s(&lv_File, "../Out2.txt", "w");
	fwrite(lv_Str.c_str(), lv_Str.length(), 1, lv_File);
	fclose(lv_File);
	
	
}




void main()
{
	auto lv_Test = FOGS::CreateOverloaded
		(
			[](int)
			{
				cout << "int\n";
			},
			[](float)
			{
				cout << "float\n";
			},
			[](...)
			{
				cout << "...\n";
			}
		);

	lv_Test(1);
	lv_Test(1.0f);
	lv_Test("asd");
	


	_CheckLeaks;

	//TestFile();
	FillTest();

	cin.ignore();
}