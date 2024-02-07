#pragma once
#include "global.h"

struct FGlobal;
struct FSolve;
struct FTreeElement;

struct FSolveSecondPage
{
    explicit FSolveSecondPage(shared_ptr<FGlobal> _ptrGlobal);

    void AddDiscScore(const OpenXLSX::XLWorksheet& fSheet, int iKeyPageNumber);
    double DFSCountingScore(shared_ptr<FTreeElement> ptrThis);

private:
    // FSolve* ptrSolve; //Позднее связывание, но не требуется, так как могу
    // обратиться через Global

    shared_ptr<FGlobal> ptrGlobal;    // Синглтон
};
