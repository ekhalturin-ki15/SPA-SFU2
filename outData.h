#pragma once
#include "global.h"

struct FGlobal;

struct FOutData
{
	FOutData(FGlobal* _ptrGlobal);

	void Out(string sOutPath); // Каталог, где будут файлы

	void Init();

	FGlobal* ptrGlobal; //Синглтон

	OpenXLSX::XLWorksheet CreateAndTake(string sName, string sPath);
};