#include "graph.h"
#include "config.h"
#include "error.h"
#include "global.h"

#include "formulaParser.h"

const int FGraph::iCommon =
    -1;    // Так как 0 - ... зарезервированы для графов по курсам
const int    FGraph::iAlt                   = -2;
const int    FGraph::iReverse               = -3;
const double FGraph::dAllScoreNotEqualError = -10;

const double FGraphType::dNoInit = -2e4;

// Инверсия зависимости
FGraph::FGraph(shared_ptr<FCurricula> _ptrTree) : ptrTree(_ptrTree)
{
    // Теперь mapAllowDisc хранится в _ptrTree
    // mapAllowDisc = _ptrTree->GewMapAllowDisc(true, true);
}

void FGraph::Create()
{
    // Генерируем основной граф
    GenerateGraph();

    // Теперь генерируем альтернативный граф
    GenerateAltGraph();

    // Создание обрартного графа, в котором компетенции - это вершины
    GenerateReverseGraph();

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
            if (bArrIsSolveGraphMetric[0])
                CountAllMetric(FGraph::iCommon);
            // Проверка будет выполнена только в том случае, если нет
            // принудительно исключённых дисциплин
            /*if (ptrTree->dAllSumScore !=
                mapGraph[FGraph::iCommon].dGraphAllScore)
                this->ptrTree->ptrGlobal->ptrError->ErrorBadAllScore(
                    FGraph::iCommon,
                    FGraph::dAllScoreNotEqualError);*/
        }

        if (bArrIsSolveGraphMetric.size() > 1)
            if (bArrIsSolveGraphMetric[1])
                CountAllMetric(FGraph::iAlt);

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

    CalculateAllPairDistance(mapGraph[iTypeGraph].arrAllPairDistanceQuartile,
                             mapGraph[iTypeGraph].fAdjList);
}

void FGraph::CalcAllScoreAndAmount(FGraphType& fGraph)
{
    const int& iSoManyComp =
        this->ptrTree->ptrGlobal->ptrConfig->GetISoMachComp();
    fGraph.arrAmountCountCompDisc.resize(iSoManyComp + 1);

    fGraph.iGraphAmountDisc = 0;    // Отчёт от нуля
    fGraph.dGraphAllScore   = 0;    // Отчёт от нуля

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
        fGraph.mapGraphAmountTypeDisc[fDisc->eTypeDisc]++;

        // Определяем, сколько компетенций формируется
        if (fDisc->mapComp.size() == 0)
        {
            ptrTree->ptrGlobal->ptrError->ErrorGraphZeroComp(
                this->ptrTree->sNamePlan, l);
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
        fGraph.dDense = FGraphType::dNoInit;
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

void FGraph::GenerateReverseGraph()
{
    FGraphType& fGraph = mapGraph[iReverse];

    int i = -1;
    for (const auto& sKeyIt : ptrTree->fAllComp)
    {
        ++i;
        wstring wsKeyIt = ptrTree->ptrGlobal->ConwertToWstring(sKeyIt);
        fGraph.mapReversRel[{ wsKeyIt, FGraph::iReverse }] = i;
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
    for (const auto& [key, it] : ptrTree->mapAllowDisc)
    {
        for (const auto& [sComp, arrInd] : it->mapComp)
        {
            wstring wsComp  = ptrTree->ptrGlobal->ConwertToWstring(sComp);
            double& dWeight = fGraph.arrNodeWeight[fGraph.mapReversRel[{
                wsComp, FGraph::iReverse }]];

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
        ptrTree->dAllSumScore,
        ptrTree->iAmountDisc);
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
}

void FGraph::GenerateGraph()
{
    FGraphType& fGraph = mapGraph[iCommon];

    int i;

    i = -1;
    for (const auto& [key, it] : ptrTree->mapAllowDisc)
    {
        ++i;
        // arrRel[i] = key;
        fGraph.mapReversRel[{ key, FGraph::iCommon }] =
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
        const auto& Disc        = ptrTree->mapDisc[fGraph.arrRel[i].first];
        fGraph.arrNodeWeight[i] = Disc->dSumScore;
    }

    FormulaParser fFormulaParser(ptrTree->ptrGlobal->ptrConfig->GetSFormula(),
                                 ptrTree->dAllSumScore,
                                 ptrTree->iAmountDisc);

    for (int iL = 0; iL < n - 1; ++iL)
    {
        const auto& L = ptrTree->mapDisc[fGraph.arrRel[iL].first];

        // Теперь считается в CountAllMetric
        // if (fGraph.dMaxDiscScore < L->dSumScore)
        // fGraph.dMaxDiscScore = L->dSumScore;

        for (int iR = iL + 1; iR < n; ++iR)
        {
            const auto& R = ptrTree->mapDisc[fGraph.arrRel[iR].first];

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
}

void FGraph::GenerateAltGraph()
{
    FGraphType& fGraph = mapGraph[iAlt];

    int i;
    i = -1;
    for (const auto& [key, it] : ptrTree->mapAllowDisc)
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
        const auto& Disc    = ptrTree->mapDisc[fGraph.arrRel[i].first];
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

    FormulaParser fFormulaParser(ptrTree->ptrGlobal->ptrConfig->GetSFormula(),
                                 ptrTree->dAllSumScore,
                                 ptrTree->iAmountDisc);

    for (int iL = 0; iL < n - 1; ++iL)
    {
        const auto& L        = ptrTree->mapDisc[fGraph.arrRel[iL].first];
        const auto& iCourseL = fGraph.arrRel[iL].second;
        // Теперь считается в CountAllMetric
        // if (fGraph.dMaxDiscScore < L->mapCourseScore[iCourseL])
        // fGraph.dMaxDiscScore = L->mapCourseScore[iCourseL];

        for (int iR = iL + 1; iR < n; ++iR)
        {
            const auto& R        = ptrTree->mapDisc[fGraph.arrRel[iR].first];
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
            const auto& R        = ptrTree->mapDisc[fGraph.arrRel[iR].first];
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
        for (auto& it : mapGraph[FGraph::iAlt].arrRel)
        {
            ++iRealNumber;
            if (it.second == iCourse)
            {
                mapGraph[iCourse].mapReversRel[it] = iSize;
                mapLabelAccordance[iRealNumber]    = iSize;
                ++iSize;
            }
        }

        // Сохраняем вес каждой вершины
        {
            int i = -1;
            mapGraph[iCourse].arrNodeWeight.resize(iSize);
            for (auto& it : mapLabelAccordance)
            {
                ++i;
                mapGraph[iCourse].arrNodeWeight[i] =
                    mapGraph[FGraph::iAlt].arrNodeWeight[it.first];
            }
        }

        mapGraph[iCourse].fAdjList.resize(iSize);
        mapGraph[iCourse].arrRel.resize(iSize);
        // Сопостовление в обратную сторону
        for (const auto& [key, val] : mapGraph[iCourse].mapReversRel)
            mapGraph[iCourse].arrRel[val] = key;

        for (auto& it : mapLabelAccordance)
        {
            for (auto& et : mapGraph[FGraph::iAlt].fAdjList[it.first])
            {
                if (mapLabelAccordance.count(et.first))
                {
                    mapGraph[iCourse].fAdjList[it.second].push_back(
                        { mapLabelAccordance[et.first], et.second });
                }
            }
        }
    }
}

void FGraph::CalculateAllPairDistance(
    vector<int>&                             arrQuarAmount,
    const vector<vector<pair<int, double>>>& fCurrentAdj)
{
    arrQuarAmount.clear();

    const int              N   = fCurrentAdj.size();
    const double           INF = 1e8;
    vector<vector<double>> arrAllDistance(N, vector<double>(N, INF));

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

    int iAmountNoLink = 0;

    // Усечение выборки разбития по квартилям
    vector<double> arrPathLen;

    for (int i = 0; i < N; ++i)
    {
        for (int j = i + 1; j < N; ++j)
        {
            if (arrAllDistance[i][j] == INF)
            {
                ++iAmountNoLink;
                continue;
            }
            arrPathLen.push_back(arrAllDistance[i][j]);
        }
    }

    sort(arrPathLen.begin(), arrPathLen.end());

    const double& dTranc =
        ptrTree->ptrGlobal->ptrConfig->GetDTruncQuarPathLen();

    int iMinInd = arrPathLen.size() * dTranc;
    int iMaxInd = arrPathLen.size() - iMinInd - 1;

    int iAmountQuar = ptrTree->ptrGlobal->ptrConfig->GetIAmountQuar();
    arrQuarAmount.resize(iAmountQuar);

    if (iMinInd > iMaxInd)
        return;

    double dMinVal = arrPathLen[iMinInd], dMaxVal = arrPathLen[iMaxInd];
    double dLenght = dMaxVal - dMinVal;
    // iAmountQuar

    arrQuarAmount.front() += iMinInd;
    arrQuarAmount.back()  += iMinInd;
    for (int i = iMinInd; i <= iMaxInd; ++i)
    {
        if (arrPathLen[i] == dMaxVal)
        {
            ++arrQuarAmount.back();
            continue;
        }

        double index = (arrPathLen[i] - dMinVal) / dLenght;
        ++arrQuarAmount[int(index * iAmountQuar)];
    }

    arrQuarAmount.push_back(iAmountNoLink);
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
        for (const auto& [r, len] : fCurrentAdj[l])
        {
            // Чтобы не дублировать, он же неориентированный
            if ((l < r) || (!ptrTree->ptrGlobal->ptrConfig->GetBIsUnDirected()))
                q.push_back({ len, { l, r } });
        }
    }
    sort(ALL(q), cmp);

    True_DSU<int> fDSU(N);

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