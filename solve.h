#pragma once

#include "global.h"

struct FGlobal;

struct FTreeElement
{
	FTreeElement();

	bool bAllow; //Учитывать ли при подсчёте зачётных единиц (ЗЕ)
	int iSumScore; // Количество зачётных единиц (ЗЕ)
	wstring wsName;
	wstring wsIndexName;

	FTreeElement* ptrPerent;
	vector<FTreeElement*> arrChild;  //Дисциплины внутри модуля
	vector<string> fComp;

};

struct FTreeDisc
{
	FTreeDisc();
	~FTreeDisc();

	void DeleteDFS(FTreeElement* ptrThis); // Поиск в глубину для очистки памяти

	FTreeElement* ptrRoot;
	map<wstring, FTreeElement*> mapDisc; //Поиск указателя на дисциплину по её индексу

	set< string > fAllComp; //Множество всех компетенций, присутствующих в учебном плане (УП)
};

struct FSolve
{
	FSolve(FGlobal* _ptrGlobal);
	~FSolve();

	void ClearTreeDisc();

	void Read(string sInPath, string sOutPath);

	void CreateDiscTree(OpenXLSX::XLWorkbook& fBook, int iKeyPageNumber);

	void AddCompIndicator(OpenXLSX::XLWorkbook& fBook, int iKeyPageNumber);

	vector<FTreeDisc*> arrDisc; // Указатели на все УП, которые считали (все они одновременно хранятся в памяти)

	regex fRegexComp;

	string sInPath; // Для отладки
	string sOutPath;

	FGlobal* ptrGlobal; //Синглтон
};


