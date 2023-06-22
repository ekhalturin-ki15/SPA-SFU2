#pragma once

#include "global.h"
#include "solveSecondPage.h"

struct FGlobal;
struct FGraph;
struct FMetric;

//struct FSemesterScore
//{
//	int iNumSemester;
//	double dScore;
//};

struct FTreeElement
{
	explicit FTreeElement();

	bool bAllow; //Учитывать ли при подсчёте зачётных единиц (ЗЕ)
	bool bNotIgnore; //Если предмет входит в перечень игнорируемых

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
	explicit FTreeDisc(FGlobal* _ptrGlobal);
	~FTreeDisc();

	void CountDisc();

	//Получить только дисциплины (без модулей)
	map<wstring, FTreeElement*> GewMapAllowDisc(bool IsNecessaryAllow, bool IsNecessaryNotIgnore);

	FTreeElement* ptrRoot;
	map<wstring, FTreeElement*> mapDisc; //Поиск указателя на дисциплину по её индексу
	string sNamePlan;

	int iAmountCourse; // Количество курсов (именно курсов, не семестров)
	double dAllSumScore; //Общее кол-во ЗЕ курса (только дисциплин, и только основный (не по выбору))
	int iAmountDisc; // Количество основных дисциплин (не по выбору)

	set< string > fAllComp; //Множество всех компетенций, присутствующих в учебном плане (УП)

	void dFindAllScore(double& outDSum, int& outIAmountDisc); //Вывод через параметры
	void dFindAllScore(double& outDSum);
	
	FMetric* ptrMetric; //У каждого УП свой объект класса FMetric
	FGraph* ptrGraph; //У каждого УП свой объект класса FGraph

	//Добавил, чтобы можно было обращаться к Config
	FGlobal* ptrGlobal; //Синглтон

private:
	void DeleteDFS(FTreeElement* ptrThis); // Поиск в глубину для очистки памяти
};

struct FSolve
{
	explicit FSolve(FGlobal* _ptrGlobal);
	~FSolve();

	void Init();

	//Основная функция
	bool Read(string sInPath, string sNamePlan);

	//Использовать только после полного считывания, строит Графы всем УП из arrDisc
	void CreateAllGraph();

	//Использовать только после полного считывания, строит Графы всем УП из arrDisc
	void CreateAllMetric();

public:
	vector<FTreeDisc*> arrDisc; // Указатели на все УП, которые считали (все они одновременно хранятся в памяти)

	int iCurrentPage; // Какая по счёту страница обрабатывается в данный момент
	int iCurrentRow; // Какая по счёту строка обрабатывается 

	bool bIsCorrectParsing;

	string sInPath; // Для отладки
	string sOutPath;

	set< string > setHeaderComp; //Все заголовки компетенций (УК, ПК, ОПК). Нужно для вывода

private:

	void ClearTreeDisc();

	// По идеи, там передаётся всегда this->iKeyPageNumber

	void CreateDiscTree(const OpenXLSX::XLWorksheet& fSheet, int iKeyPageNumber); // Находится в solveZeroPage.cpp 
	void AddCompIndicator(const OpenXLSX::XLWorksheet& fSheet, int iKeyPageNumber); // Находится в solveFirstPage.cpp

	//В solveSecondPage.h и solveSecondPage.cpp 
	FSolveSecondPage* ptrSolveSecondPage; //Композиция (вынес в отдельный класс, так как много методов)
	FGlobal* ptrGlobal; //Синглтон
	regex fRegexComp;
	regex fRegexHeaderComp;
};


