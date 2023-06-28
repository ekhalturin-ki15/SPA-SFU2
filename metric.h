#pragma once
#include "solve.h"
#include <map>

using namespace std;
struct FTreeDisc;

//Класс похож на FGraph по идеологии, но в нём другие данные
struct FMetric
{
	static const int iAllMetric;
	//Инверсия зависимости
	explicit FMetric(FTreeDisc* _ptrTree);

	//Использовать только после вызова Read у ptrSolve, то есть, когда ptrTree заполнен
	void Create();

	//У всего графа целиком тег sAllMetric
	map<int, map<string, int>> mapCompDistr;
	map<int, int> mapBalancAmountComp;

private:
	FTreeDisc* ptrTree;
	regex fRegexHeaderComp;

	map<wstring, FTreeElement*> mapAllowDisc; //Оставляем только разрешённые дисциплины (без модулей) для анализа
};

