#include "config.h"
#include "error.h"
#include "global.h"
#include "solve.h"


void FSolve::ThirdPageFindTypePlan(const OpenXLSX::XLWorksheet& fSheet,
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

            if (arrDisc.back()->sTypePlan == "None")
            {
                for (const auto& setFinder :
                     ptrGlobal->ptrConfig->GetKeyPage(iKeyPageNumber).arrHeader)
                {
                    for (const auto& wData : setFinder)
                    {
                        if (wsData.find(wData) != wstring::npos)
                        {
                            arrDisc.back()->sTypePlan =
                                ptrGlobal->ConwertToString(*setFinder.begin());
                            // return;
                        }
                    }
                }
            }

            string sData = ptrGlobal->ConwertToString(
                ptrGlobal->ConwertPathFormat(wsData));
            if (arrDisc.back()->iYearStart == 0)
                if (sData.size() == 4)    // Скорее всего год начала подготовки
                {
                    int iBuf = atoi(sData.c_str());
                    if ((iBuf) && (iBuf > 1000))
                    {
                        arrDisc.back()->iYearStart = iBuf;
                    }
                }

            if (arrDisc.back()->iCodeUGSN == 0)
            {
                bool bIsTrueMatchComp = false;
                vector<smatch> matchesHeaderComp;

                for (const auto& HeaderComp : arrRegexCodeUGSN)
                {
                    vector<smatch> matchesBuf { sregex_iterator { ALL(sData),
                                                                  HeaderComp },
                        sregex_iterator {}
                    };
                    if (matchesBuf.size() == 3)
                    {
                        matchesHeaderComp = matchesBuf;
                        bIsTrueMatchComp  = true;
                        break;
                    }
                }

                if (bIsTrueMatchComp)
                {
                    string sCodeUGSN = matchesHeaderComp[0][1].str();
                    int    iBuf      = atoi(sCodeUGSN.c_str());
                    if (iBuf)
                    {
                        arrDisc.back()->iCodeUGSN = iBuf;
                    }
                    
                }
            }

        }
    }
}