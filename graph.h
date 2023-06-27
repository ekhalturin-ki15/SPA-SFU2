#pragma once
#include "solve.h"
#include "dsu.h"
#include <map>
#include <queue>

using namespace std;
struct FTreeDisc;

struct FGraph
{
	//Инверсия зависимости
	explicit FGraph(FTreeDisc* _ptrTree);

	//Использовать только после вызова Read у ptrSolve, то есть, когда ptrTree заполнен
	void Create();

	void CalcDiametrAndComp(double& dResult, int& iComponent, const vector< vector <pair<int, double>>>& fCurrentAdj, bool IsConsLen); // Высчитываем диаметр и кол-во компонент связности за O(n log(n))
	void CalculateMST(double& dResult, const vector< vector <pair<int, double>>>& fCurrentAdj, auto cmp); // Высчитываем минимальное остовное дерево за O(m log(n))

	//Сопоставление id его названия (инициализируются в конструкторе)
	vector<wstring> arrRel;

	//Сопоставление названию его id (инициализируются в конструкторе)
	map < wstring, int> mapReversRel;

	vector< vector <pair<int, double>>> fAdjLust;

	double dMaxDiscScore;

	double dDiametrLen;
	double dDiametrStep;

	double dMinSpanTree;
	double dMaxSpanTree;

	int iComponent;

private:
	FTreeDisc* ptrTree; 

	map<wstring, FTreeElement*> mapAllowDisc; //Оставляем только разрешённые дисциплины (без модулей) для анализа

	//Регулирование посещённых вершин 0 - не были, 1 - были
	//vector<int> arrColor;
	//Поиск компонент связаности
	//vector<int> arrPreColor;

	//Передача выходных результатов через аргументы
	//Флаг IsСonsDist отвечает за то, будем ли мы учитывать растояние, или же только кол-во рёбер в пути
	void MaxDist(double& dMaxDist, int& iIdNode, vector<int>& arrColor, int iIdStart, bool IsСonsDist, const vector< vector <pair<int, double>>>& fCurrentAdj);
};
