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

	void CalculateDiametrAndComponent(); // Высчитываем диаметр и кол-во компонент связности за O(n log(n))
	void CalculateMST(double& dResult, auto cmp ); // Высчитываем минимальное остовное дерево за O(m log(n))

	//Сопоставление id его названия (инициализируются в конструкторе)
	vector<wstring> arrRel;

	//Сопоставление названию его id (инициализируются в конструкторе)
	map < wstring, int> mapReversRel;

	vector< vector <pair<int, double>>> fAdjacency;

	double dMaxDiscScore;

	double dDiametrLen;
	double dDiametrStep;

	double dMinSpanTree;
	double dMaxSpanTree;

	int iComponent;

private:
	FTreeDisc* ptrTree; 

	int n;//Кол-во вершин в графе

	map<wstring, FTreeElement*> mapAllowDisc; //Оставляем только разрешённые дисциплины (без модулей) для анализа

	//Регулирование посещённых вершин 0 - не были, 1 - были
	vector<int> arrColor;

	//Поиск компонент связаности
	//vector<int> arrPreColor;

	void UpdateArrColor();

	//Передача выходных результатов через аргументы
	//Флаг IsСonsDist отвечает за то, будем ли мы учитывать растояние, или же только кол-во рёбер в пути
	void MaxDist(double& dMaxDist, int& iIdNode, int iIdStart, bool IsСonsDist);
};
