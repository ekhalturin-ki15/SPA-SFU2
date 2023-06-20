#pragma once
#include "global.h"

struct FGlobal;

struct FOutData
{
	explicit FOutData(FGlobal* _ptrGlobal);

	void Out(string sOutPath); // Каталог, где будут файлы

	void Init();

private:
	OpenXLSX::XLWorksheet CreateAndTake(string sName, string sPath);

	FGlobal* ptrGlobal; //Синглтон
};