#pragma once
#include "dsu.h"
#include "solve.h"
#include <map>
#include <queue>

using namespace std;
struct FCurricula;

// Тип зависит от версии графа, сейчас ETG_Common - обычный,
// ETG_Alt - альтернативный, ETG_Reverse - Обратный
enum ETypeGraph : int
{
    ETG_Common  = -1,
    ETG_Alt     = -2,
    ETG_Reverse = -3,
    ETG_0Course = 0, //Нумерация с нуля
    ETG_1Course = 1, //Нумерация с нуля
    ETG_2Course = 2, //Нумерация с нуля
    ETG_3Course = 3, //Нумерация с нуля
    ETG_4Course = 4, //Нумерация с нуля
    ETG_5Course = 5, //Нумерация с нуля
    ETG_6Course = 6, //Нумерация с нуля
    ETG_7Course = 7, //Нумерация с нуля
    ETG_8Course = 8, //Нумерация с нуля
    ETG_9Course = 9, //Нумерация с нуля
    ETG_10Course = 10, //Нумерация с нуля
    ETG_11Course = 11, //Нумерация с нуля
    ETG_Size = 12
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
    static const double dNoInit;
    static const double dINF;

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

    double dMinDiscScore = FTypeGraph::dNoInit;
    double dMaxDiscScore = FTypeGraph::dNoInit;

    double dMinRib = FTypeGraph::dNoInit;
    double dMaxRib = FTypeGraph::dNoInit;

    double dDiametrLen  = FTypeGraph::dNoInit;
    double dDiametrStep = FTypeGraph::dNoInit;

    double dMinSpanTree = FTypeGraph::dNoInit;
    double dMaxSpanTree = FTypeGraph::dNoInit;

    vector<int>
        arrLocalQuarAllPairDistance;    // Локальное квартильное распределение

    double dGraphAllScore   = FTypeGraph::dNoInit;
    int    iGraphAmountDisc = int(FTypeGraph::dNoInit);
    double dDense           = FTypeGraph::dNoInit;

    map<ETypeDisc, int>    mapGraphAmountTypeDisc;
    map<ETypeDisc, double> mapGraphCreditsTypeDisc;
    vector<int>            arrAmountCountCompDisc;

    vector<vector<double>> arrAllDistance;

    int iComponent = int(FTypeGraph::dNoInit);
};

struct FGraph
{
    //static const int    iCommon;
    //static const int    iAlt;
    //static const int    iReverse;
    //static const double dAllScoreNotEqualError;

    // Инверсия зависимости
    explicit FGraph(shared_ptr<FCurricula> _ptrTree);

    // Использовать только после вызова Read у ptrSolve, то есть, когда ptrTree
    // заполнен
    void Create();

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
        vector<int>& arrLocalQuarAmount,
        const vector<vector<double>>&
            arrAllDistance);    // Считаем квартильное распределение в частности

    void CalculateAllPairDistance(
        vector<vector<double>>& arrAllDistance,
        const vector<vector<pair<int, double>>>&
            fCurrentAdj);    // Высчитываем минимальные пути алгоритмом Флойда -
                             // Уоршелла (O(n^3))

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

    void CountAllMetric(ETypeGraph eTypeGraph);
};
