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
      // dAllSumScore(0.),
      // iAmountDisc(0),
      // iExtendedAmountDisc(0),
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

    // arrETMAllSumScore.resize(ETM_Size);
    // arrETMAmountDisc.resize(ETM_Size);
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
                    ((it->arrChild.size() == 0) && (iTypeMetric == 1)) ||    //
                    // No Ignore
                    ((it->arrChild.size() == 0) && (it->bAllow) &&
                     (it->bNoIgnore) && (iTypeMetric == 2))

                    //
                )
            {
                mapETMTypeDisc[iTypeMetric][ETypeDisc::ETD_Total].iAmount++;
                mapETMTypeDisc[iTypeMetric][ETypeDisc::ETD_Total].dCredits +=
                    it->dSumScore;

                mapETMTypeDisc[iTypeMetric][it->eTypeDisc].iAmount++;
                mapETMTypeDisc[iTypeMetric][it->eTypeDisc].dCredits +=
                    it->dSumScore;
                if (it->setTagDisc.size() == 0)
                {
                    mapETMTagDisc[iTypeMetric][ETagDisc::ETagD_Another]
                        .iAmount++;
                    mapETMTagDisc[iTypeMetric][ETagDisc::ETagD_Another]
                        .dCredits += it->dSumScore;
                }
                for (const auto& et : it->setTagDisc)
                {
                    mapETMTagDisc[iTypeMetric][et].iAmount++;
                    mapETMTagDisc[iTypeMetric][et].dCredits += it->dSumScore;
                }
            }
        }
    }
    return;
}

void FCurricula::GetMapNoIgnoreDisc(
    map<wstring, shared_ptr<FTreeElement>>& mapDiscReturn,
    bool IsNecessaryAllow,
    bool IsNecessaryNotIgnore)
{
    for (const auto& [wsKey, it] : mapAllDisc)
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

            mapDiscReturn[wsKey] = it;
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
}


void FCurricula::GetMapNoIgnoreComp(
    map<wstring, shared_ptr<FTreeElement>>& mapCompReturn,
    const map<wstring, shared_ptr<FTreeElement>>& mapNoIgnoreDisc)
{

    for (const auto& [wsKey, it] : mapNoIgnoreDisc)
    {
        shared_ptr<FTreeElement> ptrNewNode = nullptr;
        
        for (const auto& [sCompKey, arrInd] : it->mapComp)
        {
            auto wsCompKey = ptrGlobal->ConwertToWstring(sCompKey);
            if (!mapCompReturn.count(wsCompKey))
            {
                ptrNewNode = make_shared<FTreeElement>();
                mapCompReturn[wsCompKey] = ptrNewNode;
            }

            auto& ptrNode = *(mapCompReturn[wsCompKey]);
            if (ptrGlobal->ptrConfig->GetBIsNormalizeScoreComp())
                ptrNode.dSumScore += (it->dSumScore / it->mapComp.size());
            else
                ptrNode.dSumScore += it->dSumScore;

            ptrNode.mapComp[FError::sDontHaveIndex]; // Нет ни компетенций, ни индикаторов
        }
    }
}