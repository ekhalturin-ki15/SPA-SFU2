#pragma once
#include "global.h"

struct FGlobal;
struct FSolve;
struct FTreeElement;

struct FSolveSecondPage
{
    explicit FSolveSecondPage(FGlobal* _ptrGlobal);

    void   AddDiscScore(const OpenXLSX::XLWorksheet& fSheet, int iKeyPageNumber);
    double DFSCountingScore(FTreeElement* ptrThis);

private:
    // FSolve* ptrSolve; //Позднее связывание, но не требуется, так как могу обратиться через Global

    FGlobal* ptrGlobal;    // Синглтон
};
