﻿#pragma once
#include "dsu.h"
#include "solve.h"
#include "сurricula.h"
#include <map>
#include <queue>

using namespace std;
struct FCurricula;
struct FDiscParams;

// Тип зависит от версии графа, сейчас ETG_Common - обычный,
// ETG_Alt - альтернативный, ETG_Reverse - Обратный
struct ETypeGraph
{
    int    iType;
    string sName;

    ETypeGraph(); // В graph.cpp //: iType(0), sName("NoInit") {};

    ETypeGraph(int _iType);

    ETypeGraph(int _iType, string _sName);

    int Get() const
    {
        return iType;
    }

    string Name() const
    {
        return sName;
    }

    static ETypeGraph ETG_Common;     // -1
    static ETypeGraph ETG_Alt;        // -2
    static ETypeGraph ETG_Reverse;    // -3

    bool operator<(const ETypeGraph& eRightType) const
    {
        return this->iType < eRightType.iType;
    }
    bool operator==(const ETypeGraph& eRightType) const
    {
        return this->iType == eRightType.iType;
    }
    void operator=(const ETypeGraph& eRightType)
    {
        this->iType = eRightType.iType;
    }
};

// Общие данные для графов (например, веса всех ребёр всей выборки и пр.)
struct FGraphAllData
{
    explicit FGraphAllData(shared_ptr<FGlobal> _ptrGlobal);

    map<ETypeGraph, vector<double>>
        mapGraphQuarAllWeigth;    // Индекс обозначает тип графа (обратный, за
                                  // опр. курс и т. д.)
private:
    // FSolve* ptrSolve; //Позднее связывание, но не требуется, так как могу
    // обратиться через Global

    shared_ptr<FGlobal> ptrGlobal;    // Синглтон
};

// Конкретный граф (с расчитаными метриками для него, лежащими внутри)
struct FTypeGraph
{
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

    double dMinDiscScore = FGlobal::dNoInit;
    double dMaxDiscScore = FGlobal::dNoInit;

    double dMinRib = FGlobal::dNoInit;
    double dMaxRib = FGlobal::dNoInit;

    double dDiametrLen  = FGlobal::dNoInit;
    double dDiametrStep = FGlobal::dNoInit;

    double dMinSpanTree = FGlobal::dNoInit;
    double dMaxSpanTree = FGlobal::dNoInit;

    // Квартильное разбиение
    //  Выборка разбития по квартилям
    vector<double> arrQuarMinPathLen;
    vector<int>
        arrLocalQuarAllPairDistance;    // Локальное квартильное распределение

    vector<int>
        arrGlobalQuarAllPairDistance;    // Локальное квартильное распределение

    // double dGraphAllScore   = FGlobal::dNoInit; // Теперь включена в
    // mapGraphDataTypeDisc по ETypeDisc::ETD_Total int    iGraphAmountDisc =
    // int(FGlobal::dNoInit);  // Теперь включена в mapGraphDataTypeDisc по
    // ETypeDisc::ETD_Total
    double dDense         = FGlobal::dNoInit;
    double dGlobalСluster = FGlobal::dNoInit;

    map<ETypeDisc, FDiscParams> mapGraphDataTypeDisc;

    vector<int> arrAmountCountCompDisc;

    vector<vector<double>> arrAllDistance;

    int iComponent = int(FGlobal::dNoInit);
};

struct FGraph
{
    // Инверсия зависимости
    explicit FGraph(shared_ptr<FCurricula> _ptrTree);

    // Использовать только после вызова Read у ptrSolve, то есть, когда ptrTree
    // заполнен
    void Create();

    // Использовать только после вызова Create у ptrTree
    void CreateAfter();

    void CalcAllScoreAndAmount(FTypeGraph& fGraph);

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

    void CalculateLocalQuarAllPairDistance(
        vector<double>& arrTotalQuarAllWeigth,
        vector<double>& arrLocalQuarMinPathLen,
        vector<int>&    arrLocalQuarAmount,
        const vector<vector<double>>&
            arrAllDistance);    // Считаем квартильное распределение в частности

    void CalculateTotalQuarAllPairDistance(
        vector<int>&          arrTotalQuarAmount,
        const vector<double>& arrTotalQuarAllWeigth,
        const vector<double>&
            arrLocalQuarMinPathLen);    // Считаем квартильное распределение в
                                        // Общем

    void CalculateAllPairDistance(
        vector<vector<double>>& arrAllDistance,
        const vector<vector<pair<int, double>>>&
            fCurrentAdj);    // Высчитываем минимальные пути алгоритмом Флойда -
                             // Уоршелла (O(n^3))

    // O(N^3), но на практике меньше, так как разреженный
    void CalculateCluster(double&                                  dResult,
                          const vector<vector<pair<int, double>>>& fAdjList);

    void CalculateMST(
        double&                                  dResult,
        const vector<vector<pair<int, double>>>& fCurrentAdj,
        auto cmp);    // Высчитываем минимальное остовное дерево за O(m log(n))

    map<ETypeGraph, FTypeGraph>
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

    void CountAllMetric(const ETypeGraph& eTypeGraph);

    // После вызова Create для всех ptrTree
    void CountAfterAllMetric(const ETypeGraph& eTypeGraph);

    shared_ptr<FTreeElement> GetGraphDisc(const wstring& wsKey);
};
