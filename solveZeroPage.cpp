#include "config.h"
#include "error.h"
#include "global.h"
#include "solve.h"

void FSolve::CreateDiscTreeZeroPage(const OpenXLSX::XLWorksheet& fSheet,
                            int                          iKeyPageNumber)
{
    int h = ptrGlobal->HeightPage(fSheet);

    int iIdIndex = -1;
    int iIdName  = -1;
    int iIdComp  = -1;

    FTreeElement* ptrThis    = arrDisc.back()->ptrRoot;
    FTreeElement* ptrNewNode = nullptr;
    // Считываем заголовок
    {
        int x = 0;
        for (auto it : fSheet.rows().begin()->cells())
        {
            if (ptrGlobal->ptrConfig->arrKeyPage[iKeyPageNumber]
                    .arrHeader[0]
                    .count(ptrGlobal->GetValue(it)))
                iIdIndex = x;

            if (ptrGlobal->ptrConfig->arrKeyPage[iKeyPageNumber]
                    .arrHeader[1]
                    .count(ptrGlobal->GetValue(it)))
                iIdName = x;

            if (ptrGlobal->ptrConfig->arrKeyPage[iKeyPageNumber]
                    .arrHeader[2]
                    .count(ptrGlobal->GetValue(it)))
                iIdComp = x;

            ++x;
        }
    }

#ifdef DEBUG
    ofstream out(ptrGlobal->ptrConfig->wsNameDebugFile);
#endif    // DEBUG

    int iPreX = -1;    // Root вне учебного плана (в нём все модули) у него
                       // индекс вложенности -1

    iCurrentRow = -1;
    for (auto row : fSheet.rows())
    {
        ++iCurrentRow;

        if (iCurrentRow == h) break;    // Далее только пустые строки

        int  x          = -1;
        bool bReadIndex = false;
        bool bReadName  = false;
        bool bReadComp  = false;

        if (iCurrentRow != 0)
        {
            for (auto it : row.cells())
            {
                ++x;
                wstring wsData = ptrGlobal->GetValue(it);
                if (wsData != L"")
                {
                    if ((iIdIndex <= x) && (x < iIdName) &&
                        (!bReadIndex))    // Ищем позицию в дереве для нового
                                          // предмета
                    {
                        bReadIndex = true;    // Чтобы повторно не находить
                                              // индекс в строке
                        while (iPreX >= x)
                        {
                            if (ptrThis == nullptr)
                            {
                                throw std::logic_error(FError::sBadTree);
                                return;
                            }
                            ptrThis = ptrThis->ptrParent;

                            iPreX--;
                        }
                        iPreX      = x;
                        ptrNewNode = new FTreeElement;
                        ptrThis->arrChild.push_back(ptrNewNode);
                        ptrNewNode->ptrParent = ptrThis;
                        ptrThis               = ptrNewNode;

                        ptrNewNode->wsIndexName = wsData;

                        // Смотрим, что это за дисциплина (основная, по выбору
                        // или факультатив)
                        int iTypeNumber = 0;
                        for (const auto& wsType :
                             ptrGlobal->ptrConfig->arrTypeDisc)
                        {
                            if (wsData.find(wsType) != wstring::npos)
                            {
                                ptrNewNode->eTypeDisc = ETypeDisc(iTypeNumber);
                                break;
                            }
                            ++iTypeNumber;
                        }
                        //

                        arrDisc.back()->mapDisc[wsData] = ptrNewNode;
                        continue;
                    }
                    else if ((iIdName <= x) && (x < iIdComp) && (!bReadName))
                    {
                        bReadName = true;

                        // Удаляем лишние пробелы
                        wsData = std::regex_replace(wsData,
                                                    std::wregex(L" {2,}"),
                                                    L" ");

                        // Меняем на псевдоним из "Псевдонимы" файла config.xlsx
                        if (ptrGlobal->ptrConfig->mapAliasRename.count(wsData))
                        {
                            /*ptrNewNode->wsName =
                                ptrGlobal->ptrConfig->mapAliasRename[wsData];*/

                            ptrNewNode->sName = ptrGlobal->ReversUTF16RU(
                                ptrGlobal->ConwertToString(
                                    ptrGlobal->ptrConfig
                                        ->mapAliasRename[wsData]));
                        }
                        else
                        {
                            ptrNewNode->sName = ptrGlobal->ReversUTF16RU(
                                ptrGlobal->ConwertToString(wsData));

                            ptrNewNode->sName = ptrNewNode->sName.substr(
                                0,
                                ptrGlobal->ptrConfig->iMaxNameDiscLen);

                            if (ptrGlobal->ptrConfig->bDeletingSpecCharDiscName)
                            {
                                ptrNewNode->sName =
                                    std::regex_replace(ptrNewNode->sName,
                                                       std::regex("[(),.]"),
                                                       "");
                                ptrNewNode->sName =
                                    std::regex_replace(ptrNewNode->sName,
                                                       std::regex("[-]"),
                                                       " ");
                            }

                            // Убираем лишние пробелы в конце
                            while (ptrNewNode->sName.size() > 1)
                            {
                                if (ptrNewNode->sName.back() == ' ')
                                    ptrNewNode->sName.pop_back();
                                else
                                    break;
                            }
                        }

                        ptrNewNode->bNotIgnore = !(
                            ptrGlobal->ptrConfig->setIgnoreDisc.count(wsData));

                        // Смотрим, какие теги есть у дисциплины
                        // Гуманитарные, естеств, общепроф. и т.д.
                        if (ptrGlobal->ptrConfig->mapTagDisc.count(wsData))
                        {
                            ptrNewNode->setTagDisc =
                                ptrGlobal->ptrConfig->mapTagDisc[wsData];
                        }
                        else
                        {
                            
                            ptrGlobal->ptrError->mapIndexDiscWithoutTag
                                [ptrNewNode->wsIndexName] = {
                                wsData, ptrGlobal->ptrSolve->arrDisc.back()
                                            ->sShortNamePlan
                            };
                        }

                        continue;
                    }
                    else if ((iIdComp <= x) && (!bReadComp))
                    {
                        bReadComp = true;
                        string sParsingData =
                            ptrGlobal->ConwertToString(
                                ptrGlobal->ConwertPathFormat(wsData)) +
                            ";";

                        vector<smatch> matches {
                            sregex_iterator { ALL(sParsingData), fRegexComp },
                            sregex_iterator {}
                        };

                        for (auto sData : matches)
                        {
                            string sCompName = sData[1].str();
                            // Есть ошибка оператора: иногда вместо компетенции
                            // указан индикатор
                            if (ptrGlobal->ptrConfig->bMultiIndicator)
                                sCompName =
                                    sCompName.substr(0, sCompName.find('.'));

                            ptrGlobal->DeleteSpechChars(sCompName);

                            ptrNewNode->mapComp[sCompName] = {};
                            arrDisc.back()->fAllComp.insert(sCompName);

                            // vector<smatch> matchesHeader{
                            //	sregex_iterator{ALL(sCompName),
                            // fRegexHeaderComp}, 		sregex_iterator{}};

                            // for (auto sData : matchesHeader)
                            //{
                            //	string sCompHeaderName = sData[1].str();
                            //	setHeaderComp.insert(sCompHeaderName);
                            // }
                        }
                        break;
                    }
                }
            }

            // Игнорируем неправильно оформленную дисциплины
            // Однозначного исправления нет

            // Возможно, у нас пустая строка в самом конце страницы, поэтому,
            // можем завершить считывание Дисциплин на данном моменте
            if (!bReadIndex)
            {
                ptrGlobal->ptrError->ErrorBadParser();
                // throw std::logic_error(FError::sDontHaveIndex); //
                // Неуказанный индекс, это критично
                continue;
            }
            if (!bReadName)
            {
                ptrGlobal->ptrError->ErrorBadParserName(
                    ptrNewNode->wsIndexName);
                // throw std::logic_error(FError::sNotAllData); // Щадящий
                // режим, игнорируем неправильные дисциплины
                continue;
            }
            if (!bReadComp)    // Если не считали, значит не указаны
            {
                ptrGlobal->ptrError->ErrorBadParserComp(
                    ptrNewNode->wsIndexName);
                // throw std::logic_error(FError::sNotAllData); // Щадящий
                // режим, игнорируем неправильные дисциплины
                continue;
            }
        }
    }
}
