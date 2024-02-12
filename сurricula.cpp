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
      bNotIgnore(true)
{
}

FCurricula::FCurricula(shared_ptr<FGlobal> _ptrGlobal)
    : ptrGlobal(_ptrGlobal),
      iAmountCourse(0),
      dAllSumScore(0.),
      iAmountDisc(0),
      iExtendedAmountDisc(0),
      iYearStart(0),
      iCodeUGSN(0),
      sTypePlan("None")
{
    ptrRoot = make_shared<FTreeElement>();
    ptrGraph = nullptr;    // Только после Read можно строить граф
    ptrMetric = nullptr;    // Только после Read высчитывать метрики
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
    this->iAmountDisc = 0;
    for (const auto& [key, it] : mapDisc)
    {
        if (it->dSumScore <= 0)
        {
            continue;    // Баг в УП (ложные дисциплины из страницы
                         // Компетенции(2))
        }

        if ((it->arrChild.size() == 0) && (it->bAllow))
        {
            this->iAmountDisc++;
        }

        if (it->arrChild.size() == 0)
        {
            this->iExtendedAmountDisc++;
            mapAmountTypeDisc[it->eTypeDisc]++;
            // Нераспознаный тег дисциплины (указания о тегах данной дисциплины
            // нет в файле config)
            if (it->setTagDisc.size() == 0)
            {
                mapAmountTagDisc[ETagDisc::ETagD_Another]++;
            }
            for (const auto& et : it->setTagDisc)
            {
                mapAmountTagDisc[et]++;
            }
        }
    }
    return;
}

map<wstring, shared_ptr<FTreeElement>>
    FCurricula::GewMapAllowDisc(bool IsNecessaryAllow,
                                bool IsNecessaryNotIgnore)
{
    map<wstring, shared_ptr<FTreeElement>> mapReturn;
    for (const auto& [key, it] : mapDisc)
    {
        // Нулевые дисциплины тоже нужно убрать
        if ((it->arrChild.size() == 0) && (it->dSumScore > 0))
        {
            if (IsNecessaryNotIgnore)
                if (!it->bNotIgnore)
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