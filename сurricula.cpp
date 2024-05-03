#include "config.h"
#include "error.h"
#include "global.h"
#include "graph.h"
#include "metric.h"
#include "solve.h"

FTreeElement::FTreeElement()
    : dSumScore(0.),
      // wsName(L""),
      sName(""),
      wsIndexName(L""),
      ptrParent(nullptr),
      bAllow(true),
      bNoIgnore(true)
{
}

FCurricula::FCurricula(shared_ptr<FGlobal> _ptrGlobal)
    : ptrGlobal(_ptrGlobal),
      iAmountCourse(0),
      //dAllSumScore(0.),
      //iAmountDisc(0),
      //iExtendedAmountDisc(0),
      iYearStart(0),
      iCodeUGSN(0),
      sTypePlan("None")
{
    ptrRoot = make_shared<FTreeElement>();
    ptrGraph = nullptr;    // Только после Read можно строить граф
    ptrMetric = nullptr;    // Только после Read высчитывать метрики

    // Теперь есть разделение на расширенный и только для учтённых
    mapETMTypeDisc.resize(ETM_Size);
    mapETMTagDisc.resize(ETM_Size);
    arrETMAllSumScore.resize(ETM_Size);
    arrETMAmountDisc.resize(ETM_Size);
}

FCurricula::~FCurricula()
{
    DeleteDFS(ptrRoot);
    if (ptrGraph)
    {
        ptrGraph.reset();
    }
    if (ptrMetric)
    {
        ptrMetric.reset();
    }
}

void FCurricula::DeleteDFS(shared_ptr<FTreeElement> ptrThis)
{
    for (auto it : ptrThis->arrChild)
    {
        DeleteDFS(it);
    }
    ptrThis.reset();
}

void FCurricula::CountDisc()
{
    for (int iTypeMetric = 0; iTypeMetric < ETM_Size; iTypeMetric++)
    {
        for (const auto& [key, it] : mapAllDisc)
        {
            if (it->dSumScore <= 0)
            {
                continue;    // Баг в УП (ложные дисциплины из страницы
                             // Компетенции(2))
            }

            if
                //
                (
                    // No Extended
                    ((it->arrChild.size() == 0) && (it->bAllow) &&
                     (iTypeMetric == 0)) ||
                    // Extended
                    ((it->arrChild.size() == 0) && (iTypeMetric == 1)) ||//
                    // No Ignore
                    ((it->arrChild.size() == 0) && (it->bAllow) &&
                     (it->bNoIgnore) &&
                     (iTypeMetric == 2))

                //
                )
            {
                arrETMAmountDisc[iTypeMetric]++;
                arrETMAllSumScore[iTypeMetric] += it->dSumScore;
                mapETMTypeDisc[iTypeMetric][it->eTypeDisc].iAmount++;
                mapETMTypeDisc[iTypeMetric][it->eTypeDisc].dCredits +=
                    it->dSumScore;
                if (it->setTagDisc.size() == 0)
                {
                    mapETMTagDisc[iTypeMetric][ETagDisc::ETagD_Another].iAmount++;
                    mapETMTagDisc[iTypeMetric][ETagDisc::ETagD_Another]
                        .dCredits += it->dSumScore;

                }
                for (const auto& et : it->setTagDisc)
                {
                    mapETMTagDisc[iTypeMetric][et]
                        .iAmount++;
                    mapETMTagDisc[iTypeMetric][et]
                        .dCredits += it->dSumScore;
                }
            }

        }
    }
    return;
}

map<wstring, shared_ptr<FTreeElement>>
    FCurricula::GetMapNoIgnoreDisc(bool IsNecessaryAllow,
                                bool IsNecessaryNotIgnore)
{
    map<wstring, shared_ptr<FTreeElement>> mapReturn;
    for (const auto& [key, it] : mapAllDisc)
    {
        // Нулевые дисциплины тоже нужно убрать
        if ((it->arrChild.size() == 0) && (it->dSumScore > 0))
        {
            if (IsNecessaryNotIgnore)
                if (!it->bNoIgnore)
                    continue;

            if (IsNecessaryAllow)
                if (!it->bAllow)
                    continue;

            mapReturn[key] = it;
        }
        else
        {
            // Удаляем заголовки модулей, так как им не нужны теги
            auto ptr = ptrGlobal->ptrError->mapIndexDiscWithoutTag.find(
                { it->wsIndexName, this->sShortNamePlan });

            if (ptr != ptrGlobal->ptrError->mapIndexDiscWithoutTag.end())
            {
                ptrGlobal->ptrError->mapIndexDiscWithoutTag.erase(ptr);
            }
        }
    }
    return mapReturn;
}