#pragma once
#include "global.h"

struct FGlobal;

struct FOutData
{
	FOutData(FGlobal* _ptrGlobal);

	void Out(string sOutPath);

	void Init();

	FGlobal* ptrGlobal; //Синглтон
};