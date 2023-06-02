#pragma once
#include "global.h"

struct FGlobal;
struct FSolve;
struct FTreeElement;

struct FSolveSecondPage
{
	void AddDiscScore(OpenXLSX::XLWorkbook& fBook, int iKeyPageNumber);
	double DFSCountingScore(FTreeElement* ptrThis);

	FGlobal* ptrGlobal; //Синглтон
};
