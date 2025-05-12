#include "solveSecondPage.h"
#include "config.h"
#include "error.h"
#include "global.h"
#include "solve.h"
#include "сurricula.h"

FSolveSecondPage::FSolveSecondPage(shared_ptr<FGlobal> _ptrGlobal)
    : ptrGlobal(_ptrGlobal)
{
}

void FSolveSecondPage::AddDiscScore(const OpenXLSX::XLWorksheet& fSheet,
                                    int                          iKeyPageNumber)
{
    int h = ptrGlobal->HeightPage(fSheet);

    // Инициализируются один раз
    int iIdAllow = -1;    // Не инициализированы
    int iIdIndex = -1;    // Позиция столбца, где указан индекс
    int iIdName =
        -1;    // Позиция столбца, где указано имя (будем исп. если нет имени)
    int iIdLScore =
        -1;    //  Позиция столбца, где начинается перечесление ЗЕ за семестр
    int iIdRScore =
        -1;    //  Позиция столбца, где заканчивается перечесление ЗЕ за семестр

    // Иная логика считывания, нежеле на предыдущих таблицах
    // Здесь более мягкий режим, так как вся ключевая информация уже была
    // получена, и мы выискиваем доп информацию, чтобы дополнить данные

    // auto fSheet =
    // fBook.worksheet(ptrGlobal->ConwertToString(ptrGlobal->ptrConfig->arrKeyPage[iKeyPageNumber].wsName));

    ptrGlobal->ptrSolve->iCurrentRow = -1;
    for (auto row : fSheet.rows())
    {
        ++ptrGlobal->ptrSolve->iCurrentRow;

        if (ptrGlobal->ptrSolve->iCurrentRow == h)
            break;    // Далее только пустые строки

        int  x          = -1;
        bool bReadAllow = false;
        bool bReadIndex = false;
        // bool bReadScore = false; Их надо считать несколько раз

        bool    bIsAllow = false;
        wstring wsCurrentIndex;

        for (auto it : row.cells())
        {
            ++x;
            wstring wsData = ptrGlobal->GetValue(it);

            if (wsData != L"")
            {
                if (ptrGlobal->ptrConfig->GetKeyPage(iKeyPageNumber)
                        .arrHeader[0]
                        .count(wsData))
                    iIdAllow = x;
                if (ptrGlobal->ptrConfig->GetKeyPage(iKeyPageNumber)
                        .arrHeader[1]
                        .count(wsData))
                    iIdIndex = x;
                if (ptrGlobal->ptrConfig->GetKeyPage(iKeyPageNumber)
                        .arrHeader[2]
                        .count(wsData))
                    iIdName =
                        x - 1;    // Так как в УП есть два столбца с ОДИНАКОВЫМИ
                                  // названиями "Наименование", поэтому
                                  // ориентироваться по ним не получится, зато
                                  // получится ориентироваться по следующему за
                                  // ним столбцу
                if (ptrGlobal->ptrConfig->GetKeyPage(iKeyPageNumber)
                        .arrHeader[3]
                        .count(wsData))
                    iIdLScore = x;
                if (ptrGlobal->ptrConfig->GetKeyPage(iKeyPageNumber)
                        .arrHeader[4]
                        .count(wsData))
                    iIdRScore = x;

                if ((iIdAllow != -1) && (iIdAllow <= x) && (!bReadAllow))
                {
                    bReadAllow = true;
                    bIsAllow   = ptrGlobal->IsThatIsTrue(wsData);
                }

                if (((iIdIndex != -1) && (iIdIndex <= x) && (x < iIdLScore) &&
                     (!bReadIndex)) ||
                    ((iIdName != -1) && (iIdName <= x) && (x < iIdLScore) &&
                     (!bReadIndex)))
                {
                    if (iIdIndex == -1)
                    {
                        // Если нет индексов, то ориентируемся по названиям
                        wsData = ptrGlobal->ptrSolve->arrDisc.back()
                                     ->mapNameToIndexDisc[wsData];
                    }

                    bReadIndex     = true;
                    wsCurrentIndex = wsData;
                    // Если указан среди перечня кодов дисциплин
                    if (ptrGlobal->ptrSolve->arrDisc.back()->mapAllDisc.count(
                            wsCurrentIndex))
                    {
                        auto& ptrThis = ptrGlobal->ptrSolve->arrDisc.back()
                                            ->mapAllDisc[wsCurrentIndex];
                        ptrThis->bAllow = bIsAllow;
                    }
                }

                if ((iIdLScore != -1) && (iIdRScore != -1) &&
                    (iIdLScore <= x) && (x < iIdRScore))
                {
                    // bReadScore = true;
                    // Если указан среди перечня кодов дисциплин
                    if (ptrGlobal->ptrSolve->arrDisc.back()->mapAllDisc.count(
                            wsCurrentIndex))
                    {
                        auto& ptrThis = ptrGlobal->ptrSolve->arrDisc.back()
                                            ->mapAllDisc[wsCurrentIndex];
                        int    iNumCource;
                        double dScore;
                        iNumCource =
                            x - iIdLScore;    // Нуль нумерация семестров
                        iNumCource /=
                            ptrGlobal->ptrConfig
                                ->GetICourseLen();    // Определяем курс, а
                                                      // не семестр
                        dScore =
                            atof(ptrGlobal->ConwertToString(wsData).c_str());
                        ptrThis->bAllow = bIsAllow;
                        ptrThis->dSumScore += dScore;
                        ptrThis->mapCourseScore[iNumCource] += dScore;

                        // if ((bIsAllow) && (ptrThis->arrChild.size() ==
                        //                    0))    // Т.е разрешён и является
                        //  дисциплиной (не модулем)

                        // Является дисциплиной (не модулем)
                        if (ptrThis->arrChild.size() == 0)
                        {
                            if (dScore >
                                ptrGlobal->ptrConfig->GetDAnomalBigScore())
                            {
                                ptrGlobal->ptrError->ErrorAnomalBigScore(
                                    dScore);
                            }
                            // ptrGlobal->ptrSolve->arrDisc.back()->dAllSumScore
                            // +=
                            //    dScore;
                        }
                    }
                }
            }
        }
    }

    if ((iIdAllow == -1) || ((iIdIndex == -1) && (iIdName == -1)) ||
        (iIdLScore == -1) || (iIdRScore == -1))
    {
        throw std::logic_error(
            FError::sNotFoundKeyCol);    // Неудалось найти ключевой столбец
    }

    auto& iAmountCource = ptrGlobal->ptrSolve->arrDisc.back()->iAmountCourse =
        ((iIdRScore - iIdLScore) / ptrGlobal->ptrConfig->GetICourseLen());

    if (ptrGlobal->ptrSolve->iMaxCourse < iAmountCource)
        ptrGlobal->ptrSolve->iMaxCourse =
            iAmountCource;    // Максимально найденый курс среди всех УП,
                              // которые вводятся
}

double FSolveSecondPage::DFSCountingScore(shared_ptr<FTreeElement> ptrThis)
{
    if (ptrThis->arrChild.size() == 0)
    {
        if ((ptrThis->bAllow)
            &&(ptrThis->bNoIgnore))
        {
            return ptrThis->dSumScore;
        }
        else
        {
            return 0;
        }
    }

    double dScore = 0;

    for (auto it : ptrThis->arrChild)
    {
        dScore += DFSCountingScore(it);
    }

    return ptrThis->dSumScore = dScore;
}