#pragma once

#include "global.h"
#include "solveSecondPage.h"

struct FGlobal;
struct FGraph;
struct FMetric;

// struct FSemesterScore
//{
//	int iNumSemester;
//	double dScore;
// };

// Тип дисциплины (основная, по выбору, факультатив)
enum ETagDisc : int
{
    ETD_Common,
    ETD_Chosen,
    ETD_Elective,
    ETD_Size
};

struct FTreeElement
{
    explicit FTreeElement();

    bool bAllow;    // Учитывать ли при подсчёте зачётных единиц (ЗЕ)
    bool bNotIgnore;    // Если предмет входит в перечень игнорируемых

    double dSumScore;    // Количество зачётных единиц (ЗЕ)
    map<int, double>
        mapCourseScore;    // В каком семестре (ключ) сколько ЗЕ (значение)

    // wstring wsName;
    string  sName;
    wstring wsIndexName;

    FTreeElement* ptrParent;
    vector<FTreeElement*> arrChild;    // Дисциплины внутри модуля
    // Указаны компетенции с индикаторами для дополнительной валидации УП (что у
    // страницы Компетенции (2) есть весь перечень компетенций)
    map<string, vector<string>>
        mapComp;    // Компетенции, у каждой из которых перечень индикаторов

    ETagDisc eTagDisc = ETagDisc::ETD_Common;

    // vector<string> arrIndicator; // Индикаторы (из индикатора можно извлечь
    // компетенцию)
};

struct FTreeDisc
{
    explicit FTreeDisc(FGlobal* _ptrGlobal);
    ~FTreeDisc();

    void CountDisc();

    // Получить только дисциплины (без модулей)
    map<wstring, FTreeElement*> GewMapAllowDisc(bool IsNecessaryAllow,
                                                bool IsNecessaryNotIgnore);

    FTreeElement* ptrRoot;
    map<wstring, FTreeElement*>
        mapDisc;    // Поиск указателя на дисциплину по её индексу
    string sNamePlan;
    string sShortNamePlan;    // Имя УП без расширения

    int     iAmountCourse;    // Количество курсов (именно курсов, не семестров)
    double  dAllSumScore;    // Общее кол-во ЗЕ курса (только дисциплин, и только
                            // тех, что учитываются)
    int iAmountDisc;    // Количество учитываемых дисциплин (не по выбору)
    int iExtendedAmountDisc;    // Количество всех дисциплин (не модулей)
    map<ETagDisc, int>
        mapAmountTagDisc;    // Количество дисциплин по типу (основные, по
                             // выбору, факультативы)

    set<string> fAllComp;    // Множество всех компетенций, присутствующих в
                             // учебном плане (УП)

    // Не нужно, так как эти значения хранятся в iAmountDisc iExtendedAmountDisc
    // и mapAmountTagDisc void FindAllScore(double& outDSum, int&
    // outIAmountDisc);    // Вывод через параметры void FindAllScore(double&
    // outDSum);

    FMetric* ptrMetric;    // У каждого УП свой объект класса FMetric
    FGraph* ptrGraph;    // У каждого УП свой объект класса FGraph

    // Добавил, чтобы можно было обращаться к Config
    FGlobal* ptrGlobal;    // Синглтон

private:
    void DeleteDFS(
        FTreeElement* ptrThis);    // Поиск в глубину для очистки памяти
};

struct FSolve
{
    explicit FSolve(FGlobal* _ptrGlobal);
    ~FSolve();

    bool Init();

    // Основная функция
    bool Read(string sInPath, string sNamePlan);

    // Использовать только после полного считывания, строит Графы всем УП из
    // arrDisc
    void CreateAllGraph();

    // Использовать только после полного считывания, строит Графы всем УП из
    // arrDisc
    void CreateAllMetric();

public:
    vector<FTreeDisc*> arrDisc;    // Указатели на все УП, которые считали (все
                                   // они одновременно хранятся в памяти)

    int iCurrentPage;    // Какая по счёту страница обрабатывается в данный
                         // момент
    int iCurrentRow;    // Какая по счёту строка обрабатывается

    bool bIsCorrectParsing;

    int iMaxCourse;    // Максимально встреченый курс (для сводной таблицы по
                       // всем курсам)

    string sInPath;    // Для отладки
    string sOutPath;
    // Не могу перенести в FMetric, так как это общее для всех УП сразу
    set<string> setHeaderComp;    // Все заголовки компетенций (УК, ПК, ОПК).
                                  // Нужно для вывода

private:
    static int iSinglControll;    // Проверка на синглтон

    void ClearTreeDisc();

    // По идеи, там передаётся всегда this->iKeyPageNumber

    void CreateDiscTree(const OpenXLSX::XLWorksheet& fSheet,
                        int iKeyPageNumber);    // Находится в solveZeroPage.cpp
    void AddCompIndicator(
        const OpenXLSX::XLWorksheet& fSheet,
        int iKeyPageNumber);    // Находится в solveFirstPage.cpp

    // В solveSecondPage.h и solveSecondPage.cpp
    FSolveSecondPage* ptrSolveSecondPage;    // Композиция (вынес в отдельный
                                             // класс, так как много методов)
    FGlobal* ptrGlobal;    // Синглтон
    regex    fRegexComp;
    regex    fRegexHeaderComp;
    regex    fRegexHeaderInd;
};
