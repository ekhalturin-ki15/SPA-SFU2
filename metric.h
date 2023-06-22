#pragma once
#include "solve.h"
#include <map>

using namespace std;
struct FTreeDisc;

//Класс похож на FGraph по идеологии, но в нём другие данные
struct FMetric
{
	//Инверсия зависимости
	explicit FMetric(FTreeDisc* _ptrTree);

	//Использовать только после вызова Read у ptrSolve, то есть, когда ptrTree заполнен
	void Create();

	map<string, int> mapCompDistr;

	// Так как у одного предмета могут быть несколько групп компетенций (и УК и ОПК), то сумма % будет больше 100 (из-за пересечений)

	int iBalancAmountComp;

private:
	FTreeDisc* ptrTree;
	regex fRegexHeaderComp;

	map<wstring, FTreeElement*> mapAllowDisc; //Оставляем только разрешённые дисциплины (без модулей) для анализа
};

