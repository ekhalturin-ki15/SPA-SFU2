#pragma once

#include "global.h"

struct FGlobal;

struct FTreeDisc
{

};

struct FSolve
{
	FSolve(FGlobal* _fGlobal);

	void ClearTreeDisc();

	void Read(string sInPath, string sOutPath);

	FGlobal* fGlobal; //Синглтон
};


static vector<FTreeDisc*> arrDisc;