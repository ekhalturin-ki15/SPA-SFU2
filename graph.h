#pragma once
#include "solve.h"
#include <map>

using namespace std;
struct FTreeDisc;

struct FGraph
{
	//Инверсия зависимости
	explicit FGraph(FTreeDisc* _ptrTree);

	//Использовать только после вызова Read у ptrSolve, то есть, когда ptrTree заполнен
	void Create();

	//Сопоставление id его названия (инициализируются в конструкторе)
	vector<wstring> arrRel;

	//Сопоставление названию его id (инициализируются в конструкторе)
	map < wstring, int> mapReversRel;

	vector< vector <pair<int, double>>> fAdjacency;

	double dMaxDiscScore;

private:
	FTreeDisc* ptrTree; 

	map<wstring, FTreeElement*> mapAllowDisc; //Оставляем только разрешённые дисциплины (без модулей) для анализа
};
