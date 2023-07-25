#pragma once
#include "dsu.h"
#include "solve.h"
#include <map>
#include <queue>

using namespace std;
struct FTreeDisc;

struct FGraphType
{
    // Сопоставление id его названия и номера курса (инициализируются в конструкторе)
    vector<pair<wstring, int>> arrRel;

    // Сопоставление названию его id (инициализируются в конструкторе)
    map<pair<wstring, int>, int> mapReversRel;

    // Второй вариант графа, где дисциплины связаны либо по курсам,
    // либо +- 1 сама с собой, но разнй курс
    vector<vector<pair<int, double>>> fAdjList;

    double dMaxDiscScore;

    double dDiametrLen;
    double dDiametrStep;

    double dMinSpanTree;
    double dMaxSpanTree;

    int iComponent;
};

struct FGraph
{
    static const int iCommon;
    static const int iAlt;

    // Инверсия зависимости
    explicit FGraph(FTreeDisc* _ptrTree);

    // Использовать только после вызова Read у ptrSolve, то есть, когда ptrTree заполнен
    void Create();

    void CalcDiametrAndComp(double&                                  dResult,
                            int&                                     iComponent,
                            const vector<vector<pair<int, double>>>& fCurrentAdj,
                            bool IsConsLen);    // Высчитываем диаметр и кол-во компонент связности за O(n log(n))
    void CalculateMST(double&                                  dResult,
                      const vector<vector<pair<int, double>>>& fCurrentAdj,
                      auto cmp);    // Высчитываем минимальное остовное дерево за O(m log(n))

    map<int, FGraphType> mapGraph;    // Тип зависит от версии графа, сейчас iCommon - обычный, iAlt - альтернативный

private:
    FTreeDisc* ptrTree;

    map<wstring, FTreeElement*> mapAllowDisc;    // Оставляем только разрешённые дисциплины (без модулей) для анализа

    // Регулирование посещённых вершин 0 - не были, 1 - были
    // vector<int> arrColor;
    // Поиск компонент связаности
    // vector<int> arrPreColor;

    // Передача выходных результатов через аргументы
    // Флаг IsСonsDist отвечает за то, будем ли мы учитывать растояние, или же только кол-во рёбер в пути
    void MaxDist(double&                                  dMaxDist,
                 int&                                     iIdNode,
                 vector<int>&                             arrColor,
                 int                                      iIdStart,
                 bool                                     IsСonsDist,
                 const vector<vector<pair<int, double>>>& fCurrentAdj);

    void GenerateGraph();
    void GenerateAltGraph();
};
