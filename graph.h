﻿#pragma once
#include "dsu.h"
#include "solve.h"
#include <map>
#include <queue>

using namespace std;
struct FCurricula;

// Конкретный граф (с расчитаными метриками для него, лежащими внутри)
struct FGraphType
{
    static const double dNoInit;

    // Сопоставление id его названия и номера курса (инициализируются при
    // создании)
    vector<pair<wstring, int>> arrRel;

    // Сопоставление названию его id (инициализируются при создании)
    map<pair<wstring, int>, int> mapReversRel;

    // Вес каждого узла
    vector<double> arrNodeWeight;

    // Второй вариант графа, где дисциплины связаны либо по курсам,
    // либо +- 1 сама с собой, но разнй курс
    // В паре first куда, second вес
    vector<vector<pair<int, double>>> fAdjList;

    double dMinDiscScore = FGraphType::dNoInit;
    double dMaxDiscScore = FGraphType::dNoInit;

    double dMinRib = FGraphType::dNoInit;
    double dMaxRib = FGraphType::dNoInit;

    double dDiametrLen  = FGraphType::dNoInit;
    double dDiametrStep = FGraphType::dNoInit;

    double dMinSpanTree = FGraphType::dNoInit;
    double dMaxSpanTree = FGraphType::dNoInit;

    vector<int> arrAllPairDistanceQuartile;

    double dGraphAllScore   = FGraphType::dNoInit;
    int    iGraphAmountDisc = int(FGraphType::dNoInit);
    double dDense           = FGraphType::dNoInit;

    map<ETypeDisc, int> mapGraphAmountTypeDisc;
    vector<int>         arrAmountCountCompDisc;

    int iComponent = int(FGraphType::dNoInit);
};

struct FGraph
{
    static const int    iCommon;
    static const int    iAlt;
    static const int    iReverse;
    static const double dAllScoreNotEqualError;

    // Инверсия зависимости
    explicit FGraph(shared_ptr<FCurricula> _ptrTree);

    // Использовать только после вызова Read у ptrSolve, то есть, когда ptrTree
    // заполнен
    void Create();

    void CalcAllScoreAndAmount(FGraphType& fGraph);

    void CalcMinMaxWeight(double&                           dResult,
                          const vector<pair<wstring, int>>& fCurrentNode,
                          auto                              cmp);

    void CalcMinMaxEdge(double&                                  dResult,
                        const vector<vector<pair<int, double>>>& fCurrentAdj,
                        auto                                     cmp);

    void CalcDiametrAndComp(
        double&                                  dResult,
        int&                                     iComponent,
        const vector<vector<pair<int, double>>>& fCurrentAdj,
        bool IsConsLen);    // Высчитываем диаметр и кол-во компонент
                            // связности за O(n log(n))

    void CalculateAllPairDistance(
        vector<int>& arrQuarAmount,
        const vector<vector<pair<int, double>>>&
            fCurrentAdj);    // Высчитываем минимальные пути алгоритмом Флойда -
                             // Уоршелла (O(n^3))

    void CalculateMST(
        double&                                  dResult,
        const vector<vector<pair<int, double>>>& fCurrentAdj,
        auto cmp);    // Высчитываем минимальное остовное дерево за O(m log(n))

    map<int, FGraphType>
        mapGraph;    // Тип зависит от версии графа, сейчас iCommon - обычный,
                     // iAlt - альтернативный, iReverse - Обратный

private:
    shared_ptr<FCurricula> ptrTree;

    // mapAllowDisc теперь в FSolve вместе с УП
    // map<wstring, FTreeElement*>
    //    mapAllowDisc;    // Оставляем только разрешённые дисциплины (без
    // модулей) для анализа (и без тех, у кого ЗЕ = 0)

    // Регулирование посещённых вершин 0 - не были, 1 - были
    // vector<int> arrColor;
    // Поиск компонент связаности
    // vector<int> arrPreColor;

    // Передача выходных результатов через аргументы
    // Флаг IsСonsDist отвечает за то, будем ли мы учитывать растояние, или же
    // только кол-во рёбер в пути
    void MaxDist(double& dMaxDist, int& iIdNode, vector<int>& arrColor,
                 int iIdStart, bool IsСonsDist,
                 const vector<vector<pair<int, double>>>& fCurrentAdj);

    void GenerateGraph();
    void GenerateAltGraph();

    void GenerateReverseGraph();    // Граф, где вершины - это компетенции

    void GenerateCourseGraph();    // Графы для каждого курса по отдельности

    void CountAllMetric(int iTypeGraph);
};
