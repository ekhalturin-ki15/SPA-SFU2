#include "config.h"
#include "error.h"
#include "global.h"
#include "solve.h"


void FSolve::FindTypePlanThirdPage(const OpenXLSX::XLWorksheet& fSheet,
                                   int                          iKeyPageNumber)
{
    //int h = ptrGlobal->HeightPage(fSheet);

    iCurrentRow = -1;
    for (auto row : fSheet.rows())
    {
        ++iCurrentRow;

        //if (iCurrentRow == h) break;    // Далее только пустые строки

        for (auto it : row.cells())
        {
            wstring wsData = ptrGlobal->GetValue(it);
            if (wsData == L"") continue;

            for (const auto& setFinder :
                 ptrGlobal->ptrConfig->arrKeyPage[iKeyPageNumber].arrHeader)
            {
                for (const auto& wData : setFinder)
                {
                    if (wsData.find(wData) != wstring::npos)
                    {
                        arrDisc.back()->sTypePlan =
                            ptrGlobal->ConwertToString(*setFinder.begin());
                        return;
                    }
                }
            }
        }
    }
}