#include "config.h"
#include "error.h"
#include "global.h"
#include "graph.h"
#include "metric.h"
#include "solve.h"

FTreeElement::FTreeElement()
    : dSumScore(0.),
      //wsName(L""),
      sName(""),
      wsIndexName(L""),
      ptrParent(nullptr),
      bAllow(true),
      bNotIgnore(true)
{
}

FTreeDisc::FTreeDisc(FGlobal* _ptrGlobal)
    : ptrGlobal(_ptrGlobal), iAmountCourse(0), dAllSumScore(0.), iAmountDisc(0)
    , iExtendedAmountDisc(0)
{
    ptrRoot = new FTreeElement;
    ptrGraph = nullptr;    // Только после Read можно строить граф
    ptrMetric = nullptr;    // Только после Read высчитывать метрики
}

FTreeDisc::~FTreeDisc()
{
    DeleteDFS(ptrRoot);
    if (ptrGraph)
    {
        delete ptrGraph;
    }
    if (ptrMetric)
    {
        delete ptrMetric;
    }
}

void FTreeDisc::DeleteDFS(FTreeElement* ptrThis)
{
    for (auto it : ptrThis->arrChild)
    {
        DeleteDFS(it);
    }
    delete ptrThis;
}

void FTreeDisc::CountDisc()
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
        }
    }
    return;
}

map<wstring, FTreeElement*>
    FTreeDisc::GewMapAllowDisc(bool IsNecessaryAllow, bool IsNecessaryNotIgnore)
{
    map<wstring, FTreeElement*> mapReturn;
    for (const auto& [key, it] : mapDisc)
    {
        // Нулевые дисциплины тоже нужно убрать
        if ((it->arrChild.size() == 0) && (it->dSumScore > 0))
        {
            if (IsNecessaryNotIgnore)
                if (!it->bNotIgnore) continue;

            if (IsNecessaryAllow)
                if (!it->bAllow) continue;

            mapReturn[key] = it;
        }
        else
        {
            //Удаляем заголовки модулей, так как им не нужны теги
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