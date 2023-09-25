#include "graph.h"
#include "config.h"
#include "error.h"
#include "global.h"

#include "formulaParser.h"

const int FGraph::iCommon =
    -1;    // Так как 0 - ... зарезервированы для графов по курсам
const int    FGraph::iAlt                   = -2;
const double FGraph::dAllScoreNotEqualError = -10;

const double FGraphType::dNoInit = -1;

// Инверсия зависимости
FGraph::FGraph(FTreeDisc* _ptrTree) : ptrTree(_ptrTree)
{
    mapAllowDisc = _ptrTree->GewMapAllowDisc(true, true);
}

void FGraph::Create()
{
    // Генерируем основной граф
    GenerateGraph();

    // Теперь генерируем альтернативный граф
    GenerateAltGraph();

    if (mapGraph[FGraph::iAlt].fAdjList.size() != 0)    // Т.е альт граф создан
    {
        GenerateCourseGraph();
    }

    try
    {
        const auto& bArrIsSolveGraphMetric =
            ptrTree->ptrGlobal->ptrConfig->bArrIsSolveGraphMetric;

        if (bArrIsSolveGraphMetric.size() > 0)
        {
            if (bArrIsSolveGraphMetric[0]) CountAllMetric(FGraph::iCommon);
            //Проверка будет выполнена только в том случае, если нет принудительно исключённых дисциплин
            /*if (ptrTree->dAllSumScore !=
                mapGraph[FGraph::iCommon].dGraphAllScore)
                this->ptrTree->ptrGlobal->ptrError->ErrorBadAllScore(
                    FGraph::iCommon,
                    FGraph::dAllScoreNotEqualError);*/
        }

        if (bArrIsSolveGraphMetric.size() > 1)
            if (bArrIsSolveGraphMetric[1]) CountAllMetric(FGraph::iAlt);

        if (bArrIsSolveGraphMetric.size() > 2)
            if (bArrIsSolveGraphMetric[2])
            {
                for (int iCourse = 0; iCourse < this->ptrTree->iAmountCourse;
                     ++iCourse)
                    CountAllMetric(iCourse);
            }
    }
    catch (...)
    {
        // Игнорируем ошибки, работаем как ни в чём не бывало
    }
}

void FGraph::CountAllMetric(int iTypeGraph)
{
    CalcAllScoreAndAmount(mapGraph[iTypeGraph]);

#pragma region Weight
    CalcMinMaxWeight(mapGraph[iTypeGraph].dMinDiscScore,
                     mapGraph[iTypeGraph].arrRel,
                     std::less<double>());
    CalcMinMaxWeight(mapGraph[iTypeGraph].dMaxDiscScore,
                     mapGraph[iTypeGraph].arrRel,
                     std::greater<double>());
#pragma endregion
#pragma region Edge
    CalcMinMaxEdge(mapGraph[iTypeGraph].dMinRib,
                   mapGraph[iTypeGraph].fAdjList,
                   std::less<double>());
    CalcMinMaxEdge(mapGraph[iTypeGraph].dMaxRib,
                   mapGraph[iTypeGraph].fAdjList,
                   std::greater<double>());
#pragma endregion
#pragma region DiametrAndComp
    CalcDiametrAndComp(mapGraph[iTypeGraph].dDiametrLen,
                       mapGraph[iTypeGraph].iComponent,
                       mapGraph[iTypeGraph].fAdjList,
                       true);
    CalcDiametrAndComp(mapGraph[iTypeGraph].dDiametrStep,
                       mapGraph[iTypeGraph].iComponent,
                       mapGraph[iTypeGraph].fAdjList,
                       false);
#pragma endregion
#pragma region MST
    CalculateMST(mapGraph[iTypeGraph].dMinSpanTree,
                 mapGraph[iTypeGraph].fAdjList,
                 std::less<pair<double, pair<int, int>>>());
    CalculateMST(mapGraph[iTypeGraph].dMaxSpanTree,
                 mapGraph[iTypeGraph].fAdjList,
                 std::greater<pair<double, pair<int, int>>>());
#pragma endregion
}

void FGraph::CalcAllScoreAndAmount(FGraphType& fGraph)
{
    for (const auto& [l, r] : fGraph.arrRel)
    {
        const auto& fDisc     = ptrTree->mapDisc[l];
        double      dCurScore = FGraphType::dNoInit;

        if (r == FGraph::iCommon)    // Значит считаем все дисциплины полностью,
                                     // а не по курсам
        {
            dCurScore = fDisc->dSumScore;
        }
        else
        {
            if (fDisc->mapCourseScore.count(r))
                dCurScore = fDisc->mapCourseScore[r];
        }
        if (dCurScore == FGraphType::dNoInit)
        {
            ptrTree->ptrGlobal->ptrError->ErrorGraphNoInitWeightDisc(
                this->ptrTree->sNamePlan, l);
            continue;
        }
        fGraph.dGraphAllScore += dCurScore;
        fGraph.iGraphAmountDisc++;
        fGraph.mapGraphAmountTagDisc[fDisc->eTagDisc]++;
    }
}

void FGraph::CalcMinMaxWeight(double&                           dResult,
                              const vector<pair<wstring, int>>& fCurrentNode,
                              auto                              cmp)
{
    // Подсчёт мин макс веса вершины (Min-Max DiscScore)
    dResult = FGraphType::dNoInit;
    for (const auto& [l, r] : fCurrentNode)
    {
        const auto& fDisc     = ptrTree->mapDisc[l];
        double      dCurScore = FGraphType::dNoInit;

        if (r == FGraph::iCommon)    // Значит считаем все дисциплины полностью,
                                     // а не по курсам
        {
            dCurScore = fDisc->dSumScore;
        }
        else
        {
            if (fDisc->mapCourseScore.count(r))
                dCurScore = fDisc->mapCourseScore[r];
        }

        if (dCurScore == FGraphType::dNoInit)
        {
            ptrTree->ptrGlobal->ptrError->ErrorGraphNoInitWeightDisc(
                this->ptrTree->sNamePlan, l);
            continue;
        }

        if (dCurScore == 0)
            ptrTree->ptrGlobal->ptrError->ErrorGraphZeroValue(
                this->ptrTree->sNamePlan, l);

        if (dResult == FGraphType::dNoInit)
        {
            dResult = dCurScore;
        }
        else
        {
            if (cmp(dCurScore, dResult)) dResult = dCurScore;
        }
    }
}

void FGraph::CalcMinMaxEdge(
    double&                                  dResult,
    const vector<vector<pair<int, double>>>& fCurrentAdj,
    auto                                     cmp)
{
    dResult = FGraphType::dNoInit;
    for (const auto& it : fCurrentAdj)
    {
        for (const auto& [iDest, dIdge] : it)
        {
            if (dResult == FGraphType::dNoInit)
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

void FGraph::GenerateGraph()
{
    int i;

    i = -1;
    for (const auto& [key, it] : mapAllowDisc)
    {
        ++i;
        // arrRel[i] = key;
        mapGraph[iCommon].mapReversRel[{ key, FGraph::iCommon }] =
            i;    // FGraph::iCommon - это заглушка (или некий коментарий к
                  // дисциплине) В данной заглушке предполагалось записывать
                  // номер курса
    }
    int n;

    n = mapGraph[iCommon]
            .mapReversRel.size();    // Кол-во вершин в графе (У нас несколько
                                     // графов, убрал из полей)
    mapGraph[iCommon].arrRel.resize(n);
    mapGraph[iCommon].fAdjList.resize(n);
    for (auto& [key, val] : mapGraph[iCommon].mapReversRel)
        mapGraph[iCommon].arrRel[val] = key;

    FormulaParser fFormulaParser(ptrTree->ptrGlobal->ptrConfig->sFormula,
                                 ptrTree->dAllSumScore,
                                 ptrTree->iAmountDisc);

    for (int iL = 0; iL < n - 1; ++iL)
    {
        const auto& L = ptrTree->mapDisc[mapGraph[iCommon].arrRel[iL].first];

        // Теперь считается в CountAllMetric
        // if (mapGraph[iCommon].dMaxDiscScore < L->dSumScore)
        // mapGraph[iCommon].dMaxDiscScore = L->dSumScore;

        for (int iR = iL + 1; iR < n; ++iR)
        {
            const auto& R =
                ptrTree->mapDisc[mapGraph[iCommon].arrRel[iR].first];

            int iPowerComp = 0;    // Сколько компетенций совпало

            for (auto& [keyCompL, arrCompL] : L->mapComp)
            {
                if (R->mapComp.count(keyCompL))
                    ++iPowerComp;    // Совпала компетенция
            }

            try
            {
                double dRibWeight = fFormulaParser.TakeResult(L->dSumScore,
                                                              R->dSumScore,
                                                              iPowerComp);

                if (dRibWeight > ptrTree->ptrGlobal->ptrConfig->dMinWeigthRib)
                {
                    mapGraph[iCommon].fAdjList[iL].push_back(
                        { iR, dRibWeight });
                    mapGraph[iCommon].fAdjList[iR].push_back(
                        { iL, dRibWeight });
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
}

void FGraph::GenerateAltGraph()
{
    int i;
    i = -1;
    for (const auto& [key, it] : mapAllowDisc)
    {
        for (const auto& [iCourse, val] : it->mapCourseScore)
        {
            ++i;
            mapGraph[iAlt].mapReversRel[{ key, iCourse }] = i;
        }
    }

    int n;
    n = mapGraph[iAlt]
            .mapReversRel.size();    // Для альтернативного графа аналогично
    mapGraph[iAlt].arrRel.resize(n);
    mapGraph[iAlt].fAdjList.resize(n);
    // Сопостовление в обратную сторону
    for (const auto& [key, val] : mapGraph[iAlt].mapReversRel)
        mapGraph[iAlt].arrRel[val] = key;

    FormulaParser fFormulaParser(ptrTree->ptrGlobal->ptrConfig->sFormula,
                                 ptrTree->dAllSumScore,
                                 ptrTree->iAmountDisc);

    for (int iL = 0; iL < n - 1; ++iL)
    {
        const auto& L = ptrTree->mapDisc[mapGraph[iAlt].arrRel[iL].first];
        const auto& iCourseL = mapGraph[iAlt].arrRel[iL].second;
        // Теперь считается в CountAllMetric
        // if (mapGraph[iAlt].dMaxDiscScore < L->mapCourseScore[iCourseL])
        // mapGraph[iAlt].dMaxDiscScore = L->mapCourseScore[iCourseL];

        for (int iR = iL + 1; iR < n; ++iR)
        {
            const auto& R = ptrTree->mapDisc[mapGraph[iAlt].arrRel[iR].first];
            const auto& iCourseR = mapGraph[iAlt].arrRel[iR].second;

            if (iCourseL != iCourseR) continue;    // Только одинаковые курсы

            int iPowerComp = 0;    // Сколько компетенций совпало

            for (auto& [keyCompL, arrCompL] : L->mapComp)
            {
                if (R->mapComp.count(keyCompL))
                    ++iPowerComp;    // Совпала компетенция
            }

            try
            {
                double dRibWeight =
                    fFormulaParser.TakeResult(L->mapCourseScore[iCourseL],
                                              R->mapCourseScore[iCourseR],
                                              iPowerComp);

                if (dRibWeight > ptrTree->ptrGlobal->ptrConfig->dMinWeigthRib)
                {
                    mapGraph[iAlt].fAdjList[iL].push_back({ iR, dRibWeight });
                    mapGraph[iAlt].fAdjList[iR].push_back({ iL, dRibWeight });
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
        if (mapGraph[iAlt].mapReversRel.count(
                { mapGraph[iAlt].arrRel[iL].first,
                  iCourseL - 1 }))    // То есть, если есть предыдущий курс
        {
            int         iR = mapGraph[iAlt].mapReversRel[{
                mapGraph[iAlt].arrRel[iL].first, iCourseL - 1 }];
            const auto& R  = ptrTree->mapDisc[mapGraph[iAlt].arrRel[iR].first];
            const auto& iCourseR = mapGraph[iAlt].arrRel[iR].second;

            int iPowerComp = L->mapComp.size();    // С сами собой все совпали

            try
            {
                double dRibWeight =
                    fFormulaParser.TakeResult(L->mapCourseScore[iCourseL],
                                              R->mapCourseScore[iCourseR],
                                              iPowerComp);

                if (dRibWeight > ptrTree->ptrGlobal->ptrConfig->dMinWeigthRib)
                {
                    mapGraph[iAlt].fAdjList[iL].push_back({ iR, dRibWeight });
                    mapGraph[iAlt].fAdjList[iR].push_back({ iL, dRibWeight });
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
}

void FGraph::GenerateCourseGraph()
{
    // Делаем граф для всех курсов
    for (int iCourse = 0; iCourse < this->ptrTree->iAmountCourse; ++iCourse)
    {
        // Выводим  граф по курсам
        map<int, int> mapLableAccordance;    // Новая индексация дисциплин
                                             // (локальная для данного курса)
        int iSize       = 0;
        int iRealNumber = -1;
        for (auto& it : mapGraph[FGraph::iAlt].arrRel)
        {
            ++iRealNumber;
            if (it.second == iCourse)
            {
                mapGraph[iCourse].mapReversRel[it] = iSize;
                mapLableAccordance[iRealNumber]    = iSize;
                ++iSize;
            }
        }

        mapGraph[iCourse].fAdjList.resize(iSize);
        mapGraph[iCourse].arrRel.resize(iSize);
        // Сопостовление в обратную сторону
        for (const auto& [key, val] : mapGraph[iCourse].mapReversRel)
            mapGraph[iCourse].arrRel[val] = key;

        for (auto& it : mapLableAccordance)
        {
            for (auto& et : mapGraph[FGraph::iAlt].fAdjList[it.first])
            {
                if (mapLableAccordance.count(et.first))
                {
                    mapGraph[iCourse].fAdjList[it.second].push_back(
                        { mapLableAccordance[et.first], et.second });
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
    int                                  n = fCurrentAdj.size();

    for (int l = 0; l < n; ++l)
    {
        for (const auto& [r, len] : fCurrentAdj[l])
        {
            // Чтобы не дублировать, он же неориентированный
            if ((l < r) || (!ptrTree->ptrGlobal->ptrConfig->bIsUnDirected))
                q.push_back({ len, { l, r } });
        }
    }
    sort(ALL(q), cmp);

    vector<int> arrDSU(n);
    int         i = 0;
    for (auto& it : arrDSU) it = i++;
    True_DSU<int> fDSU(arrDSU);

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
    dResult    = 0.;
    iComponent = 0;
    int n      = fCurrentAdj.size();

    vector<int> arrColor(n);
    for (int i = 0; i < n; ++i)
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
    int n = fCurrentAdj.size();

    dMaxDist = 0.;
    iIdNode  = iIdStart;

    priority_queue<pair<double, int>> q;
    q.push({ 0., iIdStart });

    vector<double> arrLen(n, 1e9);
    vector<int>    arrPass(n, 0);
    arrLen[iIdStart] = 0.;

    while (q.size())
    {
        auto [dLen, id] = q.top();
        dLen            = -dLen;
        q.pop();
        if (arrPass[id]) continue;
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