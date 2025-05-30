﻿#include "graph.h"
#include "config.h"
#include "error.h"
#include "global.h"

#include "formulaParser.h"

// Теперь в Enum
// const int FGraph::iCommon =
//     -1;    // Так как 0 - ... зарезервированы для графов по курсам
// const int    FGraph::iAlt                   = -2;
// const int    FGraph::iReverse               = -3;
ETypeGraph ETypeGraph::ETG_Common(-1, "Common");
ETypeGraph ETypeGraph::ETG_Alt(-2, "Alt");
ETypeGraph ETypeGraph::ETG_Reverse(-3, "Rev");

ETypeGraph::ETypeGraph() : iType(0), sName("NoInit")
{
}

ETypeGraph::ETypeGraph(int _iType) : iType(_iType)
{
    switch (_iType)
    {
       case -1:
           //Так как оператор = переопределён
           this->sName = ETypeGraph::ETG_Common.sName;
           return;
       case -2:
           this->sName = ETypeGraph::ETG_Alt.sName;
           return;
       case -3:
           this->sName = ETypeGraph::ETG_Reverse.sName;
           return;
    }
    sName = to_string(_iType);
}

ETypeGraph::ETypeGraph(int _iType, string _sName) : iType(_iType), sName(_sName)
{
}

// Инверсия зависимости
FGraph::FGraph(shared_ptr<FCurricula> _ptrTree) : ptrTree(_ptrTree)
{
    // Теперь mapAllowDisc хранится в _ptrTree
    // mapAllowDisc = _ptrTree->GewMapAllowDisc(true, true);
}

FGraphAllData::FGraphAllData(shared_ptr<FGlobal> _ptrGlobal)
    : ptrGlobal(_ptrGlobal)
{
}

void FGraph::Create()
{
    // Генерируем основной граф
    GenerateGraph();

    // Теперь генерируем альтернативный граф
    GenerateAltGraph();

    // Создание обрартного графа, в котором компетенции - это вершины
    GenerateReverseGraph();

    if (mapGraph[ETypeGraph::ETG_Alt].fAdjList.size() !=
        0)    // Т.е альт граф создан
    {
        GenerateCourseGraph();
    }

    try
    {
        const auto& bArrIsSolveGraphMetric =
            ptrTree->ptrGlobal->ptrConfig->bArrIsSolveGraphMetric;

        int iNumberParams = 0;

        //Аналогично считается в CountAfterAllMetric
        for (int iType = ETypeGraph::ETG_Common.Get();
             iType >= ETypeGraph::ETG_Reverse.Get();
             --iType)
        {
            if (bArrIsSolveGraphMetric.size() > iNumberParams)
            {
                if (bArrIsSolveGraphMetric[iNumberParams++])
                    CountAllMetric(ETypeGraph(iType));
            }
        }

        if (bArrIsSolveGraphMetric.size() > iNumberParams)
            if (bArrIsSolveGraphMetric[iNumberParams++])
            {
                for (int iCourse = 0; iCourse < this->ptrTree->iAmountCourse;
                     ++iCourse)
                    CountAllMetric(ETypeGraph(iCourse));
            }

        if (iNumberParams != bArrIsSolveGraphMetric.size())
        {
            ptrTree->ptrGlobal->ptrError->ErrorNoCountAllGraphType();
        }
    }
    catch (...)
    {
        // Игнорируем ошибки, работаем как ни в чём не бывало
    }
}

void FGraph::CreateAfter()
{
    try
    {
        const auto& bArrIsSolveGraphMetric =
            ptrTree->ptrGlobal->ptrConfig->bArrIsSolveGraphMetric;

        int iNumberParams = 0;

        for (int iType = ETypeGraph::ETG_Common.Get();
             iType >= ETypeGraph::ETG_Reverse.Get();
             --iType)
        {
            if (bArrIsSolveGraphMetric.size() > iNumberParams)
            {
                if (bArrIsSolveGraphMetric[iNumberParams++])
                    CountAfterAllMetric(ETypeGraph(iType));
            }
        }

        if (bArrIsSolveGraphMetric.size() > iNumberParams)
            if (bArrIsSolveGraphMetric[iNumberParams++])
            {
                for (int iCourse = 0; iCourse < this->ptrTree->iAmountCourse;
                     ++iCourse)
                    CountAfterAllMetric(ETypeGraph(iCourse));
            }

        if (iNumberParams != bArrIsSolveGraphMetric.size())
        {
            ptrTree->ptrGlobal->ptrError->ErrorNoCountAllGraphType();
        }
    }
    catch (...)
    {
        // Игнорируем ошибки, работаем как ни в чём не бывало
    }
}

void FGraph::CountAfterAllMetric(const ETypeGraph& eTypeGraph)
{
    auto& arrTotal = ptrTree->ptrGlobal->ptrSolve->ptrGraphAllData
                         ->mapGraphQuarAllWeigth[eTypeGraph];
    sort(ALL(arrTotal));

    CalculateTotalQuarAllPairDistance(
        mapGraph[eTypeGraph].arrGlobalQuarAllPairDistance,
        arrTotal,
        mapGraph[eTypeGraph].arrQuarMinPathLen);
}

void FGraph::CountAllMetric(const ETypeGraph& eTypeGraph)
{
    CalcAllScoreAndAmount(mapGraph[eTypeGraph]);

#pragma region Weight
    CalcMinMaxWeight(mapGraph[eTypeGraph].dMinDiscScore,
                     mapGraph[eTypeGraph].arrRel,
                     std::less<double>());
    CalcMinMaxWeight(mapGraph[eTypeGraph].dMaxDiscScore,
                     mapGraph[eTypeGraph].arrRel,
                     std::greater<double>());
#pragma endregion
#pragma region Edge
    CalcMinMaxEdge(mapGraph[eTypeGraph].dMinRib,
                   mapGraph[eTypeGraph].fAdjList,
                   std::less<double>());
    CalcMinMaxEdge(mapGraph[eTypeGraph].dMaxRib,
                   mapGraph[eTypeGraph].fAdjList,
                   std::greater<double>());
#pragma endregion
#pragma region DiametrAndComp
    CalcDiametrAndComp(mapGraph[eTypeGraph].dDiametrLen,
                       mapGraph[eTypeGraph].iComponent,
                       mapGraph[eTypeGraph].fAdjList,
                       true);
    CalcDiametrAndComp(mapGraph[eTypeGraph].dDiametrStep,
                       mapGraph[eTypeGraph].iComponent,
                       mapGraph[eTypeGraph].fAdjList,
                       false);
#pragma endregion
#pragma region MST
    CalculateMST(mapGraph[eTypeGraph].dMinSpanTree,
                 mapGraph[eTypeGraph].fAdjList,
                 std::less<pair<double, pair<int, int>>>());
    CalculateMST(mapGraph[eTypeGraph].dMaxSpanTree,
                 mapGraph[eTypeGraph].fAdjList,
                 std::greater<pair<double, pair<int, int>>>());
#pragma endregion

    CalculateAllPairDistance(mapGraph[eTypeGraph].arrAllDistance,
                             mapGraph[eTypeGraph].fAdjList);

    CalculateCluster(mapGraph[eTypeGraph].dGlobalСluster,
                     mapGraph[eTypeGraph].fAdjList);

    CalculateLocalQuarAllPairDistance(
        ptrTree->ptrGlobal->ptrSolve->ptrGraphAllData
            ->mapGraphQuarAllWeigth[eTypeGraph],
        mapGraph[eTypeGraph].arrQuarMinPathLen,
        mapGraph[eTypeGraph].arrLocalQuarAllPairDistance,
        mapGraph[eTypeGraph].arrAllDistance);

#ifdef DEBUG
    ofstream out(ptrTree->ptrGlobal->ptrConfig->GetSNameDebugFile() + ".txt",
                 std::ios::app);
    out << ENDL << ptrTree->sNamePlan << ENDL;
    out << "Cluster coef" << ENDL << mapGraph[eTypeGraph].dGlobalСluster
        << ENDL;
#endif
}

shared_ptr<FTreeElement> FGraph::GetGraphDisc(const wstring& wsKey)
{
    shared_ptr<FTreeElement> ptrReturn = nullptr;
    if (ptrTree->mapNoIgnoreDisc.count(wsKey))
        ptrReturn = ptrTree->mapNoIgnoreDisc[wsKey];
    else
    {
        if (ptrTree->mapNoIgnoreComp.count(wsKey))
            ptrReturn = ptrTree->mapNoIgnoreComp[wsKey];
    }

    if (ptrReturn == nullptr)
    {
        ptrTree->ptrGlobal->ptrError->ErrorNoFindDisc(wsKey);
    }

    return ptrReturn;
}

void FGraph::CalcAllScoreAndAmount(FTypeGraph& fGraph)
{
    const int& iSoManyComp =
        this->ptrTree->ptrGlobal->ptrConfig->GetISoMachComp();
    fGraph.arrAmountCountCompDisc.resize(iSoManyComp + 1);

    // fGraph.iGraphAmountDisc = 0;    // Отчёт от нуля
    // fGraph.dGraphAllScore   = 0;    // Отчёт от нуля

    for (const auto& [wsKey, iCourse] : fGraph.arrRel)
    {
        shared_ptr<FTreeElement> fDisc = GetGraphDisc(wsKey);
        if (fDisc == nullptr)
            continue;

        double dCurScore = FGlobal::dNoInit;

        if (iCourse < 0)

        // Значит считаем
        // все
        // дисциплины
        // полностью,
        // а не по курсам
        {
            dCurScore = fDisc->dSumScore;
        }
        else
        {
            if (fDisc->mapCourseScore.count(iCourse))
                dCurScore = fDisc->mapCourseScore[iCourse];
        }
        if (dCurScore == FGlobal::dNoInit)
        {
            ptrTree->ptrGlobal->ptrError->ErrorGraphNoInitWeightDisc(
                this->ptrTree->sNamePlan, wsKey);
            continue;
        }

        // fGraph.dGraphAllScore += dCurScore;
        // fGraph.iGraphAmountDisc++;

        fGraph.mapGraphDataTypeDisc[ETypeDisc::ETD_Total].iAmount++;
        fGraph.mapGraphDataTypeDisc[ETypeDisc::ETD_Total].dCredits += dCurScore;

        fGraph.mapGraphDataTypeDisc[fDisc->eTypeDisc].iAmount++;
        fGraph.mapGraphDataTypeDisc[fDisc->eTypeDisc].dCredits += dCurScore;

        // Определяем, сколько компетенций формируется
        if (fDisc->mapComp.size() == 0)
        {
            ptrTree->ptrGlobal->ptrError->ErrorGraphZeroComp(
                this->ptrTree->sNamePlan, wsKey);
            continue;
        }
        else if (fDisc->mapComp.size() >= iSoManyComp)
        {
            fGraph.arrAmountCountCompDisc.back()++;
        }
        else
        {
            fGraph.arrAmountCountCompDisc[fDisc->mapComp.size()]++;
        }
    }

    double dAmountRib = 0;
    for (const auto& it : fGraph.fAdjList)
    {
        dAmountRib += it.size();
    }

    double N = fGraph.fAdjList.size();

    if (N <= 1)
    {
        fGraph.dDense = FGlobal::dNoInit;
    }
    else
    {
        fGraph.dDense = dAmountRib / (N * (N - 1));
    }
}

void FGraph::CalcMinMaxWeight(double&                           dResult,
                              const vector<pair<wstring, int>>& fCurrentNode,
                              auto                              cmp)
{
    // Подсчёт мин макс веса вершины (Min-Max DiscScore)
    dResult = FGlobal::dNoInit;
    for (const auto& [l, r] : fCurrentNode)
    {
        shared_ptr<FTreeElement> fDisc = GetGraphDisc(l);
        if (fDisc == nullptr)
            continue;

        double dCurScore = FGlobal::dNoInit;

        if (r < 0)    // Значит считаем все дисциплины
                      // полностью, а не по курсам
        {
            dCurScore = fDisc->dSumScore;
        }
        else
        {
            if (fDisc->mapCourseScore.count(r))
                dCurScore = fDisc->mapCourseScore[r];
        }

        if (dCurScore == FGlobal::dNoInit)
        {
            ptrTree->ptrGlobal->ptrError->ErrorGraphNoInitWeightDisc(
                this->ptrTree->sNamePlan, l);
            continue;
        }

        if (dCurScore == 0)
            ptrTree->ptrGlobal->ptrError->ErrorGraphZeroValue(
                this->ptrTree->sNamePlan, l);

        if (dResult == FGlobal::dNoInit)
        {
            dResult = dCurScore;
        }
        else
        {
            if (cmp(dCurScore, dResult))
                dResult = dCurScore;
        }
    }
}

void FGraph::CalcMinMaxEdge(
    double&                                  dResult,
    const vector<vector<pair<int, double>>>& fCurrentAdj,
    auto                                     cmp)
{
    dResult = FGlobal::dNoInit;

    for (const auto& it : fCurrentAdj)
    {
        for (const auto& [iDest, dIdge] : it)
        {
            if (dResult == FGlobal::dNoInit)
            {
                dResult = dIdge;
            }
            else if (cmp(dIdge, dResult))
            {
                dResult = dIdge;
            }
        }
    }
}

void FGraph::GenerateReverseGraph()
{
    FTypeGraph& fGraph = mapGraph[ETypeGraph::ETG_Reverse];

    int i = -1;
    for (const auto& sKeyIt : ptrTree->setAllComp)
    {
        ++i;
        wstring wsKeyIt = ptrTree->ptrGlobal->ConwertToWstring(sKeyIt);
        fGraph.mapReversRel[{ wsKeyIt, ETypeGraph::ETG_Reverse.Get() }] = i;
    }

    int n;
    n = fGraph.mapReversRel.size();    // Для альтернативного графа аналогично
    fGraph.arrRel.resize(n);
    fGraph.fAdjList.resize(n);

    // Сопостовление в обратную сторону
    for (const auto& [key, val] : fGraph.mapReversRel)
        fGraph.arrRel[val] = key;

    map<wstring, set<wstring>>
        mapReverseCompDisc;    // Для кажлой компетенции указываются, какие
                               // дисциплины её формируют

    fGraph.arrNodeWeight.resize(n);
    for (const auto& [key, it] : ptrTree->mapNoIgnoreDisc)
    {
        for (const auto& [sComp, arrInd] : it->mapComp)
        {
            wstring wsComp  = ptrTree->ptrGlobal->ConwertToWstring(sComp);
            double& dWeight = fGraph.arrNodeWeight[fGraph.mapReversRel[{
                wsComp, ETypeGraph::ETG_Reverse.Get() }]];

            double dAddScore = it->dSumScore;
            if (ptrTree->ptrGlobal->ptrConfig->GetBIsNormalizeScoreDisc())
                dAddScore /=
                    it->mapComp
                        .size();    // Нормализируем, так как одна дисциплина
                                    // может отвечать за несколько компетенций
            dWeight += dAddScore;
            mapReverseCompDisc[wsComp].insert(key);
        }
    }

    FormulaParser fFormulaParser(
        ptrTree->ptrGlobal->ptrConfig->GetSFormulaReverseGraph(),
        ptrTree->mapETMTypeDisc[ETM_NoIgnore][ETypeDisc::ETD_Total].dCredits,
        ptrTree->mapETMTypeDisc[ETM_NoIgnore][ETypeDisc::ETD_Total].iAmount);
    for (int iL = 0; iL < n - 1; ++iL)
    {
        const auto& L = fGraph.arrRel[iL].first;
        for (int iR = iL + 1; iR < n; ++iR)
        {
            const auto& R = fGraph.arrRel[iR].first;

            int iPowerDisc = 0;    // Сколько дисциплин совпало

            for (auto& wsNameDisc : mapReverseCompDisc[L])
            {
                if (mapReverseCompDisc[R].count(wsNameDisc))
                    ++iPowerDisc;    // Совпала дисциплина
            }

            try
            {
                double dRibWeight = fFormulaParser.TakeResult(
                    fGraph.arrNodeWeight[iL], fGraph.arrNodeWeight[iR], 0,
                    iPowerDisc);

                if (dRibWeight >
                    ptrTree->ptrGlobal->ptrConfig->GetDMinWeigthRib())
                {
                    fGraph.fAdjList[iL].push_back({ iR, dRibWeight });
                    fGraph.fAdjList[iR].push_back({ iL, dRibWeight });
                }
            }
            catch (runtime_error eError)
            {
                ptrTree->ptrGlobal->ptrError->ErrorBadFormula();
                return;
            }
            catch (...)
            {
                // Игнорируем ошибки, работаем как ни в чём не бывало
            }
        }
    }

    // Отсортируем список смежности
    for (auto& it : fGraph.fAdjList)
    {
        sort(ALL(it));
    }
}

void FGraph::GenerateGraph()
{
    FTypeGraph& fGraph = mapGraph[ETypeGraph::ETG_Common];

    int i;

    i = -1;
    for (const auto& [key, it] : ptrTree->mapNoIgnoreDisc)
    {
        ++i;
        // arrRel[i] = key;
        fGraph.mapReversRel[{ key, ETypeGraph::ETG_Common.Get() }] =
            i;    // FGraph::iCommon - это заглушка (или некий коментарий к
                  // дисциплине) В данной заглушке предполагалось записывать
                  // номер курса
    }
    int n;

    n = fGraph.mapReversRel.size();    // Кол-во вершин в графе (У нас несколько
                                       // графов, убрал из полей)
    fGraph.arrRel.resize(n);
    fGraph.fAdjList.resize(n);
    for (auto& [key, val] : fGraph.mapReversRel)
        fGraph.arrRel[val] = key;

    // Сохраняем вес каждой вершины
    fGraph.arrNodeWeight.resize(n);
    for (int i = 0; i < n; ++i)
    {
        const auto& Disc = ptrTree->mapNoIgnoreDisc[fGraph.arrRel[i].first];
        fGraph.arrNodeWeight[i] = Disc->dSumScore;
    }

    FormulaParser fFormulaParser(
        ptrTree->ptrGlobal->ptrConfig->GetSFormula(),
        ptrTree->mapETMTypeDisc[ETM_NoIgnore][ETypeDisc::ETD_Total].dCredits,
        ptrTree->mapETMTypeDisc[ETM_NoIgnore][ETypeDisc::ETD_Total].iAmount);

    for (int iL = 0; iL < n - 1; ++iL)
    {
        const auto& L = ptrTree->mapNoIgnoreDisc[fGraph.arrRel[iL].first];

        // Теперь считается в CountAllMetric
        // if (fGraph.dMaxDiscScore < L->dSumScore)
        // fGraph.dMaxDiscScore = L->dSumScore;

        for (int iR = iL + 1; iR < n; ++iR)
        {
            const auto& R = ptrTree->mapNoIgnoreDisc[fGraph.arrRel[iR].first];

            int iPowerComp = 0;    // Сколько компетенций совпало

            for (auto& [keyCompL, arrCompL] : L->mapComp)
            {
                if (R->mapComp.count(keyCompL))
                    ++iPowerComp;    // Совпала компетенция
            }

            try
            {
                double dRibWeight = fFormulaParser.TakeResult(
                    L->dSumScore, R->dSumScore, iPowerComp, 0);

                if (dRibWeight >
                    ptrTree->ptrGlobal->ptrConfig->GetDMinWeigthRib())
                {
                    fGraph.fAdjList[iL].push_back({ iR, dRibWeight });
                    fGraph.fAdjList[iR].push_back({ iL, dRibWeight });
                }
            }
            catch (runtime_error eError)
            {
                ptrTree->ptrGlobal->ptrError->ErrorBadFormula();
                return;
            }
            catch (...)
            {
                // Игнорируем ошибки, работаем как ни в чём не бывало
            }
        }
    }

    // Отсортируем список смежности
    for (auto& it : fGraph.fAdjList)
    {
        sort(ALL(it));
    }
}

void FGraph::GenerateAltGraph()
{
    FTypeGraph& fGraph = mapGraph[ETypeGraph::ETG_Alt];

    int i;
    i = -1;
    for (const auto& [key, it] : ptrTree->mapNoIgnoreDisc)
    {
        for (const auto& [iCourse, val] : it->mapCourseScore)
        {
            ++i;
            fGraph.mapReversRel[{ key, iCourse }] = i;
        }
    }

    int n;
    n = fGraph.mapReversRel.size();    // Для альтернативного графа аналогично
    fGraph.arrRel.resize(n);
    fGraph.fAdjList.resize(n);
    // Сопостовление в обратную сторону
    for (const auto& [key, val] : fGraph.mapReversRel)
        fGraph.arrRel[val] = key;

    // Сохраняем вес каждой вершины
    fGraph.arrNodeWeight.resize(n);
    for (int i = 0; i < n; ++i)
    {
        const auto& Disc    = ptrTree->mapNoIgnoreDisc[fGraph.arrRel[i].first];
        const auto& iCourse = fGraph.arrRel[i].second;
        if (Disc->mapCourseScore.count(iCourse))
        {
            fGraph.arrNodeWeight[i] = Disc->mapCourseScore[iCourse];
        }
        else
        {
            ptrTree->ptrGlobal->ptrError->ErrorNoFindCourse(
                fGraph.arrRel[i].first);
        }
    }

    FormulaParser fFormulaParser(
        ptrTree->ptrGlobal->ptrConfig->GetSFormula(),
        ptrTree->mapETMTypeDisc[ETM_NoIgnore][ETypeDisc::ETD_Total].dCredits,
        ptrTree->mapETMTypeDisc[ETM_NoIgnore][ETypeDisc::ETD_Total].iAmount);

    for (int iL = 0; iL < n - 1; ++iL)
    {
        const auto& L = ptrTree->mapNoIgnoreDisc[fGraph.arrRel[iL].first];
        const auto& iCourseL = fGraph.arrRel[iL].second;
        // Теперь считается в CountAllMetric
        // if (fGraph.dMaxDiscScore < L->mapCourseScore[iCourseL])
        // fGraph.dMaxDiscScore = L->mapCourseScore[iCourseL];

        for (int iR = iL + 1; iR < n; ++iR)
        {
            const auto& R = ptrTree->mapNoIgnoreDisc[fGraph.arrRel[iR].first];
            const auto& iCourseR = fGraph.arrRel[iR].second;

            if (iCourseL != iCourseR)
                continue;    // Только одинаковые курсы

            int iPowerComp = 0;    // Сколько компетенций совпало

            for (auto& [keyCompL, arrCompL] : L->mapComp)
            {
                if (R->mapComp.count(keyCompL))
                    ++iPowerComp;    // Совпала компетенция
            }

            try
            {
                double dRibWeight = fFormulaParser.TakeResult(
                    L->mapCourseScore[iCourseL], R->mapCourseScore[iCourseR],
                    iPowerComp, 0);

                if (dRibWeight >
                    ptrTree->ptrGlobal->ptrConfig->GetDMinWeigthRib())
                {
                    fGraph.fAdjList[iL].push_back({ iR, dRibWeight });
                    fGraph.fAdjList[iR].push_back({ iL, dRibWeight });
                }
            }
            catch (runtime_error eError)
            {
                // Нет смысла выводить повторно ошибку
                // ptrTree->ptrGlobal->ptrError->ErrorBadFormula();
                return;
            }
            catch (...)
            {
                // Игнорируем ошибки, работаем как ни в чём не бывало
            }
        }
        if (fGraph.mapReversRel.count(
                { fGraph.arrRel[iL].first,
                  iCourseL - 1 }))    // То есть, если есть предыдущий курс
        {
            int iR =
                fGraph.mapReversRel[{ fGraph.arrRel[iL].first, iCourseL - 1 }];
            const auto& R = ptrTree->mapNoIgnoreDisc[fGraph.arrRel[iR].first];
            const auto& iCourseR = fGraph.arrRel[iR].second;

            int iPowerComp = L->mapComp.size();    // С сами собой все совпали

            try
            {
                double dRibWeight = fFormulaParser.TakeResult(
                    L->mapCourseScore[iCourseL], R->mapCourseScore[iCourseR],
                    iPowerComp, 0);

                if (dRibWeight >
                    ptrTree->ptrGlobal->ptrConfig->GetDMinWeigthRib())
                {
                    fGraph.fAdjList[iL].push_back({ iR, dRibWeight });
                    fGraph.fAdjList[iR].push_back({ iL, dRibWeight });
                }
            }
            catch (runtime_error eError)
            {
                // Нет смысла выводить повторно ошибку
                // ptrTree->ptrGlobal->ptrError->ErrorBadFormula();
                return;
            }
            catch (...)
            {
                // Игнорируем ошибки, работаем как ни в чём не бывало
            }
        }
    }

    // Отсортируем список смежности
    for (auto& it : fGraph.fAdjList)
    {
        sort(ALL(it));
    }
}

void FGraph::GenerateCourseGraph()
{
    // Делаем граф для всех курсов
    for (int iCourse = 0; iCourse < this->ptrTree->iAmountCourse; ++iCourse)
    {
        // Выводим  граф по курсам
        map<int, int> mapLabelAccordance;    // Новая индексация дисциплин
                                             // (локальная для данного курса)
        int iSize       = 0;
        int iRealNumber = -1;
        for (auto& it : mapGraph[ETypeGraph::ETG_Alt].arrRel)
        {
            ++iRealNumber;
            if (it.second == iCourse)
            {
                mapGraph[ETypeGraph(iCourse)].mapReversRel[it] = iSize;
                mapLabelAccordance[iRealNumber]                = iSize;
                ++iSize;
            }
        }

        // Сохраняем вес каждой вершины
        {
            int i = -1;
            mapGraph[ETypeGraph(iCourse)].arrNodeWeight.resize(iSize);
            for (auto& it : mapLabelAccordance)
            {
                ++i;
                mapGraph[ETypeGraph(iCourse)].arrNodeWeight[i] =
                    mapGraph[ETypeGraph::ETG_Alt].arrNodeWeight[it.first];
            }
        }

        mapGraph[ETypeGraph(iCourse)].fAdjList.resize(iSize);
        mapGraph[ETypeGraph(iCourse)].arrRel.resize(iSize);
        // Сопостовление в обратную сторону
        for (const auto& [key, val] :
             mapGraph[ETypeGraph(iCourse)].mapReversRel)
            mapGraph[ETypeGraph(iCourse)].arrRel[val] = key;

        for (auto& it : mapLabelAccordance)
        {
            for (auto& et : mapGraph[ETypeGraph::ETG_Alt].fAdjList[it.first])
            {
                if (mapLabelAccordance.count(et.first))
                {
                    mapGraph[ETypeGraph(iCourse)].fAdjList[it.second].push_back(
                        { mapLabelAccordance[et.first], et.second });
                }
            }
        }
    }
}

// Версия для всей совокупности УП (в общем)
void FGraph::CalculateTotalQuarAllPairDistance(
    vector<int>&          arrTotalQuarAmount,
    const vector<double>& arrTotalQuarAllWeigth,
    const vector<double>&
        arrLocalQuarMinPathLen)    // Считаем квартильное распределение в Общем
{
    int           iAmountQuar = ptrTree->ptrGlobal->ptrConfig->GetIAmountQuar();
    const double& dTranc =
        ptrTree->ptrGlobal->ptrConfig->GetDTruncQuarPathLen();

    arrTotalQuarAmount.clear();
    arrTotalQuarAmount.resize(iAmountQuar);

    int iMinInd   = arrTotalQuarAllWeigth.size() * dTranc;
    int iMaxInd   = arrTotalQuarAllWeigth.size() - iMinInd;
    int N         = iMaxInd - iMinInd - 1;
    int iQuarSize = N / iAmountQuar;

    // Аномалии выносим в крайние квартили
    arrTotalQuarAmount.front() += iMinInd;
    arrTotalQuarAmount.back() += iMinInd;

    int iNumQuar = 1;
    for (int i = 0; i < arrLocalQuarMinPathLen.size(); ++i)
    {
        // Можно было и Бин. Поиск, но квартилей мало
        // А лучше тегировать в общем массиве то, из какого УП вес и из какого
        // типа графа
        for (iNumQuar = 1; iNumQuar < iAmountQuar; ++iNumQuar)
        {
            if (arrLocalQuarMinPathLen[i] <=
                arrTotalQuarAllWeigth[iQuarSize * iNumQuar])
            {
                ++arrTotalQuarAmount[iNumQuar - 1];
                break;
            }
        }
        if (iNumQuar == iAmountQuar)
            ++arrTotalQuarAmount.back();
    }
}

// Версия для одного конкретного УП (в частности)
// Имеется версия для всей совокупности УП (в общем)
// FGraph::CalculateTotalQuarAllPairDistance
// которая Инициализируемая в CreateAfter
void FGraph::CalculateLocalQuarAllPairDistance(
    vector<double>& arrTotalQuarAllWeigth,
    vector<double>& arrLocalQuarMinPathLen,
    vector<int>&    arrLocalQuarAmount,
    const vector<vector<double>>&
        arrAllDistance)    // Считаем квартильное распределение в частности
{
    arrLocalQuarAmount.clear();

    const int N = arrAllDistance.size();

    int iAmountNoLink = 0;

    for (int i = 0; i < N; ++i)
    {
        for (int j = i + 1; j < N; ++j)
        {
            if (arrAllDistance[i][j] == FGlobal::dINF)
            {
                ++iAmountNoLink;
                continue;
            }
            arrLocalQuarMinPathLen.push_back(arrAllDistance[i][j]);

            // Добавляем в общий пул для подсчёта квартилей по всей выборки
            arrTotalQuarAllWeigth.push_back(arrAllDistance[i][j]);
        }
    }

    sort(ALL(arrLocalQuarMinPathLen));

    const double& dTranc =
        ptrTree->ptrGlobal->ptrConfig->GetDTruncQuarPathLen();

    int iMinInd = arrLocalQuarMinPathLen.size() * dTranc;
    int iMaxInd = arrLocalQuarMinPathLen.size() - iMinInd - 1;

    int iAmountQuar = ptrTree->ptrGlobal->ptrConfig->GetIAmountQuar();
    arrLocalQuarAmount.resize(iAmountQuar);

    if (iMinInd > iMaxInd)
        return;

    double dMinVal = arrLocalQuarMinPathLen[iMinInd],
           dMaxVal = arrLocalQuarMinPathLen[iMaxInd];
    double dLenght = dMaxVal - dMinVal;
    // iAmountQuar

    arrLocalQuarAmount.front() += iMinInd;
    arrLocalQuarAmount.back() += iMinInd;
    for (int i = iMinInd; i <= iMaxInd; ++i)
    {
        if (arrLocalQuarMinPathLen[i] == dMaxVal)
        {
            ++arrLocalQuarAmount.back();
            continue;
        }

        double index = (arrLocalQuarMinPathLen[i] - dMinVal) / dLenght;
        ++arrLocalQuarAmount[int(index * iAmountQuar)];
    }

    arrLocalQuarAmount.push_back(iAmountNoLink);
}

bool graphLess(pair<int, double> l, pair<int, double> r)
{
    if (l.first == r.first)
        return l.second < r.second;
    return l.first < r.first;
}

void FGraph::CalculateCluster(double&                                  dResult,
                              const vector<vector<pair<int, double>>>& fAdjList)
{
    const int N = fAdjList.size();

    int iAmountClosedTriag = 0;
    int iAmountOpenTriag   = 0;

    // Вершина треугольника
    for (int i = 0; i < N; ++i)
    {
        // Левая вершина
        for (int pairLeft = 0; pairLeft < int(fAdjList[i].size()) - 1;
             ++pairLeft)
        {
            const auto& [left, distanceLeft] = fAdjList[i][pairLeft];

            for (int pairRight = pairLeft + 1; pairRight < fAdjList[i].size();
                 ++pairRight)
            {
                const auto& [right, distancepairRight] = fAdjList[i][pairRight];
                ++iAmountOpenTriag;

                pair<int, double> serchRight = { right, -1 };

                // Если замыкает, то есть в i есть k
                const auto& it = lower_bound(ALL(fAdjList[left]), serchRight);

                if (it != fAdjList[left].end())
                    if (it->first == right)
                        ++iAmountClosedTriag;
            }
        }
        // pair<int, double> searchJ = { i, -1 };
        // auto              pairJ   = lower_bound(ALL(fAdjList[i]), searchJ);
        // for (; pairJ != fAdjList[i].end(); ++pairJ)
        //{
        //     auto& [j, valJ]           = *pairJ;
        //     pair<int, double> searchK = { j, -1 };
        //     auto              pairK   = lower_bound(ALL(fAdjList[j]),
        //     searchK); for (; pairK != fAdjList[j].end(); ++pairK)
        //     {
        //         auto& [k, valK] = *pairK;
        //         ++iAmountOpenTriag;
        //         pair<int, double> k2 = { k, -1 };
        //         // Если замыкает, то есть в i есть k
        //         const auto& it =
        //             lower_bound(fAdjList[i].begin(), fAdjList[i].end(), k2);
        //         if (it != fAdjList[i].end())
        //             if (it->first == k)
        //                 ++iAmountClosedTriag;
        //     }
        // }
    }

    if (iAmountOpenTriag)
    {
        dResult = iAmountClosedTriag / (double(iAmountOpenTriag));
    }
    else
    {
        // Слишком маленький граф для подсчёта кластеризации
        dResult = FGlobal::dNoInit;
    }

#ifdef DEBUG
    ofstream out(ptrTree->ptrGlobal->ptrConfig->GetSNameDebugFile() + ".txt",
                 std::ios::app);
    out << ENDL << iAmountClosedTriag << " " << iAmountOpenTriag;
#endif
}

void FGraph::CalculateAllPairDistance(
    vector<vector<double>>&                  arrAllDistance,
    const vector<vector<pair<int, double>>>& fCurrentAdj)
{
    const int N = fCurrentAdj.size();
    arrAllDistance.assign(N, vector<double>(N, FGlobal::dINF));

    for (int L = 0; L < N; ++L)
    {
        for (const auto& [R, dDis] : fCurrentAdj[L])
        {
            arrAllDistance[L][R] = dDis;
            arrAllDistance[R][L] = dDis;
        }
    }

    for (int k = 0; k < N; ++k)
    {
        for (int i = 0; i < N; ++i)
        {
            for (int j = 0; j < N; ++j)
            {
                if (arrAllDistance[i][j] >
                    arrAllDistance[i][k] + arrAllDistance[k][j])
                {
                    arrAllDistance[i][j] =
                        arrAllDistance[i][k] + arrAllDistance[k][j];
                }
            }
        }
    }
}

void FGraph::CalculateMST(double&                                  dResult,
                          const vector<vector<pair<int, double>>>& fCurrentAdj,
                          auto                                     cmp)
{
    dResult = 0;
    vector<pair<double, pair<int, int>>> q;
    const int                            N = fCurrentAdj.size();

    for (int l = 0; l < N; ++l)
    {
        auto it = fCurrentAdj[l].begin();
        if (ptrTree->ptrGlobal->ptrConfig->GetBIsUnDirected())
        {
            // Чтобы не дублировать, он же неориентированный
            pair<int, double> searchJ = { l, -1 };
            it = lower_bound(ALL(fCurrentAdj[l]), searchJ);
        }

        for (; it != fCurrentAdj[l].end(); ++it)
        {
            const auto& [r, len] = *it;

            q.push_back({ len, { l, r } });
        }
    }
    sort(ALL(q), cmp);

    DSU<int> fDSU(N);

    for (const auto& it : q)
    {
        auto [len, pr] = it;
        auto& [l, r]   = pr;
        if (!fDSU.isSame(l, r))
        {
            dResult += len;
            fDSU.Merge(l, r);
        }
    }
}

void FGraph::CalcDiametrAndComp(
    double& dResult, int& iComponent,
    const vector<vector<pair<int, double>>>& fCurrentAdj, bool IsConsLen)
{
    dResult     = 0.;
    iComponent  = 0;
    const int N = fCurrentAdj.size();

    vector<int> arrColor(N);
    for (int i = 0; i < N; ++i)
    {
        double dTempLen;
        int    iTemp = i;
        if (!arrColor[i])
        {
            ++iComponent;
            MaxDist(dTempLen, iTemp, arrColor, i, IsConsLen, fCurrentAdj);
            MaxDist(dTempLen, iTemp, arrColor, iTemp, IsConsLen, fCurrentAdj);
            if (dResult < dTempLen)
            {
                dResult = dTempLen;
            }
        }
    }
}

void FGraph::MaxDist(double& dMaxDist, int& iIdNode, vector<int>& arrColor,
                     int iIdStart, bool IsСonsDist,
                     const vector<vector<pair<int, double>>>& fCurrentAdj)
{
    const int N = fCurrentAdj.size();

    dMaxDist = 0.;
    iIdNode  = iIdStart;

    priority_queue<pair<double, int>> q;
    q.push({ 0., iIdStart });

    const double   INF = 1e8;
    vector<double> arrLen(N, INF);
    vector<int>    arrPass(N, 0);
    arrLen[iIdStart] = 0.;

    while (q.size())
    {
        auto [dLen, id] = q.top();
        dLen            = -dLen;
        q.pop();
        if (arrPass[id])
            continue;
        arrPass[id] = 1;

        arrColor[id] = 1;

        if (dMaxDist < dLen)
        {
            dMaxDist = dLen;
            iIdNode  = id;
        }

        for (auto [l, d] : fCurrentAdj[id])
        {
            double dNewLen =
                (IsСonsDist) ? d : 1;    // Учитываем ли мы растояние, или шаги
            if (arrLen[l] > dLen + dNewLen)
            {
                arrLen[l] = dLen + dNewLen;
                q.push({ -arrLen[l], l });
            }
        }
    }

    return;
}