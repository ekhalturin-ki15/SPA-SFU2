#include "config.h"
#include "error.h"
#include "global.h"
#include "solve.h"
#include "сurricula.h"

void FSolve::FirstPageAddCompIndicator(const OpenXLSX::XLWorksheet& fSheet,
                                       int iKeyPageNumber)
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
            if (ptrGlobal->ptrConfig->GetKeyPage(iKeyPageNumber)
                    .arrHeader[0]
                    .count(ptrGlobal->GetValue(it)))
                iIdIndex = x;
        }
    }

    string                   sLastComp      = "";
    string                   sLastIndicator = "";
    shared_ptr<FTreeElement> ptrThis        = nullptr;

    iCurrentRow = -1;
    for (auto row : fSheet.rows())
    {
        ++iCurrentRow;

        if (iCurrentRow == h)
            break;    // Далее только пустые строки

        int  x = -1;
        bool bReadIndex =
            false;    // Удалось ли считать индекс дисциплины в строке
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
                    string sData = ptrGlobal->ConwertToString(
                        ptrGlobal->ConwertPathFormat(wsData));
                    if ((iIdIndex <= x) && (!bReadIndex))
                    {
                        bReadIndex = true;    // Чтобы повторно не находить
                                              // индекс в строке
                        // Если указан среди перечня компетенций

                        if (arrDisc.back()->setAllComp.count(sData))
                        {
                            sLastComp = sData;
                            continue;
                        }
                        // Если указан среди перечня кодов дисциплин
                        if (arrDisc.back()->mapAllDisc.count(wsData))
                        {
                            bThisRowIsDisc = true;
                            ptrThis        = arrDisc.back()->mapAllDisc[wsData];

                            // Значит, мы нашли дисциплину, у которой указан
                            // индикатор соответсвующей компетенции

                            if (!ptrThis->mapComp.count(sLastComp))
                            {
                                // Иногда ОПК-4.1 Это индикатор, а инога
                                // компетенция с индикатором ОПК-4.1.1

                                string sReserveData =
                                    sLastComp.substr(0, sLastComp.find('.'));

                                if (!ptrThis->mapComp.count(sReserveData))
                                {
                                    // Неправильно указаны компетенции у
                                    // дисциплины Не к чему соотнести индикатор
                                    ptrGlobal->ptrError->ErrorBadIndicatorBind(
                                        ptrGlobal->ConwertToString(
                                            ptrThis->wsIndexName),
                                        sLastComp, sLastIndicator);

                                    break;
                                }
                                else
                                {
                                    sLastIndicator = sLastComp;
                                    sLastComp      = sReserveData;
                                }
                            }

                            ptrThis->mapComp[sLastComp].push_back(
                                sLastIndicator);

                            vector<smatch> matchesHeaderComp;
                            /* { sregex_iterator { ALL(sLastComp),
                                                 fRegexHeaderComp },
                                                            sregex_iterator {}
                               };*/

                            bool bIsTrueMatchComp = false;

                            for (const auto& HeaderComp : arrRegexHeaderComp)
                            {
                                vector<smatch> matchesBuf {
                                    sregex_iterator { ALL(sLastComp),
                                                      HeaderComp },
                                    sregex_iterator {}
                                };
                                if (matchesBuf.size() > 0)
                                {
                                    matchesHeaderComp = matchesBuf;
                                    bIsTrueMatchComp  = true;
                                    break;
                                }
                            }

                            if (bIsTrueMatchComp)
                            {
                                for (auto sData : matchesHeaderComp)
                                {
                                    string sCompHeaderName = sData[1].str();
                                    setHeaderComp.insert(sCompHeaderName);
                                }
                            }
                            else
                            {
                                vector<smatch> matchesHeaderInd;

                                /*{ sregex_iterator { ALL(sLastIndicator),
                                                    fRegexHeaderInd },
                                                               sregex_iterator
                                   {} };*/
                                bool bIsTrueMatchInd = false;
                                for (const auto& HeaderInd : arrRegexHeaderInd)
                                {
                                    vector<smatch> matchesBuf {
                                        sregex_iterator { ALL(sLastIndicator),
                                                          HeaderInd },
                                        sregex_iterator {}
                                    };
                                    if (matchesBuf.size() > 0)
                                    {
                                        matchesHeaderInd = matchesBuf;
                                        bIsTrueMatchInd  = true;
                                        break;
                                    }
                                }

                                for (auto sData : matchesHeaderInd)
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
                // может присутствовать множество пустых строк в конце, сделал
                // усечение и предварительный выход
                continue;
            }
        }
    }
}
