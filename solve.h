#pragma once

#include "global.h"
#include "solveSecondPage.h"

struct FGlobal;


//struct FSemesterScore
//{
//	int iNumSemester;
//	double dScore;
//};

struct FTreeElement
{
	FTreeElement();

	bool bAllow; //Учитывать ли при подсчёте зачётных единиц (ЗЕ)
	double dSumScore; // Количество зачётных единиц (ЗЕ)
	map<int, double> mapCourseScore; // В каком семестре (ключ) сколько ЗЕ (значение)

	wstring wsName;
	wstring wsIndexName;
	
	FTreeElement* ptrParent;
	vector<FTreeElement*> arrChild;  //Дисциплины внутри модуля
	map<string, vector<string>> mapComp; // Компетенции, у каждой из которых перечень индикаторов

};

struct FTreeDisc
{
	FTreeDisc();
	~FTreeDisc();

	void CountDisc();

	FTreeElement* ptrRoot;
	map<wstring, FTreeElement*> mapDisc; //Поиск указателя на дисциплину по её индексу
	string sNamePlan;

	int iAmountCourse; // Количество курсов (именно курсов, не семестров)
	double dAllSumScore; //Общее кол-во ЗЕ курса (только дисциплин, и только основный (не по выбору))
	int iAmountDisc; // Количество основных дисциплин (не по выбору)

	set< string > fAllComp; //Множество всех компетенций, присутствующих в учебном плане (УП)

	void dFindAllScore(double& outDSum, int& outIAmountDisc); //Вывод через параметры
	void dFindAllScore(double& outDSum);

private:
	void DeleteDFS(FTreeElement* ptrThis); // Поиск в глубину для очистки памяти
};

struct FSolve
{
	explicit FSolve(FGlobal* _ptrGlobal);
	~FSolve();

	void Init();

	bool Read(string sInPath, string sNamePlan);

	vector<FTreeDisc*> arrDisc; // Указатели на все УП, которые считали (все они одновременно хранятся в памяти)

	int iCurrentPage; // Какая по счёту страница обрабатывается в данный момент
	int iCurrentRow; // Какая по счёту строка обрабатывается 

	bool bIsCorrectParsing;

	string sInPath; // Для отладки
	string sOutPath;

private:

	void ClearTreeDisc();

	void CreateDiscTree(OpenXLSX::XLWorkbook& fBook, int iKeyPageNumber); // Находится в solveZeroPage.cpp

	void AddCompIndicator(OpenXLSX::XLWorkbook& fBook, int iKeyPageNumber); // Находится в solveFirstPage.cpp

	FSolveSecondPage* ptrSolveSecondPage; //Композиция (вынес в отдельный класс, так как много методов)
	FGlobal* ptrGlobal; //Синглтон
	regex fRegexComp;
};


