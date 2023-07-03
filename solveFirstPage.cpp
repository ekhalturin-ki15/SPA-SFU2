#include "config.h"
#include "error.h"
#include "global.h"
#include "solve.h"

void FSolve::AddCompIndicator(const OpenXLSX::XLWorksheet& fSheet, int iKeyPageNumber)
{
    int h = ptrGlobal->HeightPage(fSheet);

    int iIdIndex = -1;
    // int iIdСontent = -1;

    // Считываем заголовок
    {
        int x = -1;
        for (auto it : fSheet.rows().begin()->cells())
        {
            ++x;
            if (ptrGlobal->ptrConfig->arrKeyPage[iKeyPageNumber].arrHeader[0].count(ptrGlobal->GetValue(it))) iIdIndex = x;
        }
    }

    string        sLastComp      = "";
    string        sLastIndicator = "";
    FTreeElement* ptrThis        = nullptr;

    iCurrentRow = -1;
    for (auto row : fSheet.rows())
    {
        ++iCurrentRow;

        if (iCurrentRow == h) break;    // Далее только пустые строки

        int  x          = -1;
        bool bReadIndex = false;    // Удалось ли считать индекс дисциплины в строке
        // bool bReadСontent = false;
        bool bThisRowIsDisc = false;

        if (iCurrentRow != 0)
        {
            for (auto it : row.cells())
            {
                ++x;
                wstring wsData = ptrGlobal->GetValue(it);

                if (wsData != L"")
                {
                    string sData = ptrGlobal->ConwertToString(ptrGlobal->ConwertPathFormat(wsData));
                    if ((iIdIndex <= x) && (!bReadIndex))
                    {
                        bReadIndex = true;    // Чтобы повторно не находить индекс в строке
                        // Если указан среди перечня компетенций
                        if (arrDisc.back()->fAllComp.count(sData))
                        {
                            sLastComp = sData;
                            continue;
                        }
                        // Если указан среди перечня кодов дисциплин
                        if (arrDisc.back()->mapDisc.count(wsData))
                        {
                            bThisRowIsDisc = true;
                            ptrThis        = arrDisc.back()->mapDisc[wsData];

                            // Значит, мы нашли дисциплину, у которой указать индикатор
                            // соответсвующей компетенции

                            if (!ptrThis->mapComp.count(sLastComp))
                            {
                                // Неправильно указаны компетенции у дисциплины
                                // Не к чему соотнести индикатор
                                ptrGlobal->ptrError->ErrorBadIndicatorBind(sLastComp, sLastIndicator);

                                break;
                            }

                            ptrThis->mapComp[sLastComp].push_back(sLastIndicator);

                            vector<smatch> matchesHeader { sregex_iterator { ALL(sLastIndicator), fRegexHeaderIndicator },
                                                           sregex_iterator {} };
                            
                            if (matchesHeader.size() > 0)
                            {
                                for (auto sData : matchesHeader)
                                {
                                    string sCompHeaderName = sData[1].str();
                                    setHeaderComp.insert(sCompHeaderName);
                                }
                            }
                            else
                            {
                                string sParserIndicator = sLastComp + '.';
                                vector<smatch> matchesHeader2 { sregex_iterator { ALL(sParserIndicator), fRegexHeaderIndicator },
                                                               sregex_iterator {} };
                                for (auto sData : matchesHeader2)
                                {
                                    string sCompHeaderName = sData[1].str();
                                    setHeaderComp.insert(sCompHeaderName);
                                }
                            }

                            break;
                        }
                        // Иначе это индикатор
                        sLastIndicator = sData;
                        continue;
                    }
                }
            }

            // Игнорируем неправильно оформленную дисциплины
            // Однозначного исправления нет
            if (!bReadIndex)
            {
                ptrGlobal->ptrError->ErrorEmptyLine();
                // может присутствовать множество пустых строк в конце, сделал усечение и предварительный выход
                continue;
            }
        }
    }
}
