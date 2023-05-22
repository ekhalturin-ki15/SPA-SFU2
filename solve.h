#pragma once

#include "global.h"

struct FTreeDisc
{

};


struct FSolve
{
	void ClearTreeDisc();

	void Read(string sInPath, string sOutPath);

};


static vector<FTreeDisc*> arrDisc;