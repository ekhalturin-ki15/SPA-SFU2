#pragma once

#include "global.h"
#include "solveSecondPage.h"

struct FGlobal;
struct FGraph;
struct FMetric;

struct FSolve;
// struct FSemesterScore
//{
//	int iNumSemester;
//	double dScore;
// };

// Тип дисциплины (основная, по выбору, факультатив)

enum ETagDisc;

enum ETypeDisc : int
{
    ETD_Elective,
    ETD_Common,
    ETD_Chosen,
    ETD_Size
};

struct FTreeElement
{
    explicit FTreeElement();

    bool bAllow;    // Учитывать ли при подсчёте зачётных единиц (ЗЕ)
    bool bNoIgnore;    // Если предмет входит в перечень игнорируемых

    double dSumScore;    // Количество зачётных единиц (ЗЕ)
    map<int, double>
        mapCourseScore;    // В каком семестре (ключ) сколько ЗЕ (значение)

    // wstring wsName;
    string  sName;
    wstring wsIndexName;

    shared_ptr<FTreeElement> ptrParent;
    vector<shared_ptr<FTreeElement>> arrChild;    // Дисциплины внутри модуля
    // Указаны компетенции с индикаторами для дополнительной валидации УП (что у
    // страницы Компетенции (2) есть весь перечень компетенций)
    map<string, vector<string>>
        mapComp;    // Компетенции, у каждой из которых перечень индикаторов

    ETypeDisc eTypeDisc = ETypeDisc::ETD_Common;
    // Перечисляются теги дисциплины (Гуманитарная, естеств., общепроф.)
    // Может быть несколько тегов у одной дисциплины
    set<ETagDisc> setTagDisc;

    // vector<string> arrIndicator; // Индикаторы (из индикатора можно извлечь
    // компетенцию)
};

enum ETypeMetric : int
{
    ETM_NoExtended,
    ETM_Extended,    // Самый мягкий режим, больше всего дисциплин (в том числе
                     // и те, что не учитываются в УП)
    ETM_NoIgnore,    // Самый строгий режим, меньше всего дисциплин (применён
                     // список, кого исключить)
    ETM_Size
};

struct FDiscParams
{
    int    iAmount;
    double dCredits;
};

// Один конкретный УП
struct FCurricula
{
    friend struct FSolve;

    explicit FCurricula(shared_ptr<FGlobal> _ptrGlobal);
    ~FCurricula();

    void CountDisc();

    shared_ptr<FTreeElement> ptrRoot;
    map<wstring, shared_ptr<FTreeElement>>
        mapAllDisc;    // Поиск указателя на дисциплину по её индексу

    //Сквозная индексация (совпадает с mapAllDisc)
    map<wstring, shared_ptr<FTreeElement>>
        mapNoIgnoreDisc;    // Оставляем только разрешённые дисциплины (без
                            // модулей) для анализа (и без тех, у кого ЗЕ = 0)

    map<wstring, wstring> mapNameToIndexDisc;

    string sNamePlan;
    string sShortNamePlan;    // Имя УП без расширения
    string sCurName;          // Имя УП без расширения
    string sTypePlan;    // Указывается: бакалавр, магистр, специалист или
                         // абитуриент

    int iYearStart;    // Год начала обучения
    int iCodeUGSN;     // Год начала обучения

    int iAmountCourse;    // Количество курсов (именно курсов, не семестров)

    // double dAllSumScore;    // Общее кол-во ЗЕ курса (только дисциплин, и
    // только
    //  тех, что учитываются)
    // int iAmountDisc;    // Количество учитываемых дисциплин (без игнорируемых
    // в списке) int iExtendedAmountDisc;    // Количество всех дисциплин (не
    // модулей)

    vector<double> arrETMAllSumScore;
    vector<int>    arrETMAmountDisc;
    vector<map<ETypeDisc, FDiscParams>>
        mapETMTypeDisc;    // Количество и ЗЕ дисциплин по типу (основные, по
                           // выбору, факультативы), Учитываются в том числе
                           // и те, что не считаются в Плане
    vector<map<ETagDisc, FDiscParams>>
        mapETMTagDisc;    // Количество дисциплин по теги
                          // (гуманитарные,
    // технические, естествонаучные)

    set<string> fAllComp;    // Множество всех компетенций, присутствующих в
                             // учебном плане (УП)

    // Не нужно, так как эти значения хранятся в iAmountDisc iExtendedAmountDisc
    // и mapAmountTypeDisc void FindAllScore(double& outDSum, int&
    // outIAmountDisc);    // Вывод через параметры void FindAllScore(double&
    // outDSum);

    shared_ptr<FGraph> ptrGraph;    // У каждого УП свой объект класса FGraph
    shared_ptr<FMetric> ptrMetric;    // У каждого УП свой объект класса FMetric

    // Добавил, чтобы можно было обращаться к Config
    shared_ptr<FGlobal> ptrGlobal;    // Синглтон

private:
    void DeleteDFS(shared_ptr<FTreeElement>
                       ptrThis);    // Поиск в глубину для очистки памяти

    // Получить только дисциплины (без модулей)
    map<wstring, shared_ptr<FTreeElement>>
        GetMapNoIgnoreDisc(bool IsNecessaryAllow, bool IsNecessaryNotIgnore);
};

struct FSolve
{
    explicit FSolve(shared_ptr<FGlobal> _ptrGlobal);
    ~FSolve();

    bool Init();

    // Основная функция
    bool Create(string sInPath, string sNamePlan);

    // Использовать только после полного считывания, строит Графы всем УП из
    // arrDisc
    void CreateAllGraph();

    // Использовать только после полного считывания, строит Графы всем УП из
    // arrDisc
    void CreateAllMetric();

public:
    vector<shared_ptr<FCurricula>>
        arrDisc;    // Указатели на все УП, которые считали (все
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

    void ClearCurricula();

    // По идеи, там передаётся всегда this->iKeyPageNumber

    void ZeroPageCreateDiscTree(
        const OpenXLSX::XLWorksheet& fSheet,
        int iKeyPageNumber);    // Находится в solveZeroPage.cpp

    void FirstPageAddCompIndicator(
        const OpenXLSX::XLWorksheet& fSheet,
        int iKeyPageNumber);    // Находится в solveFirstPage.cpp

    void ThirdPageFindTypePlan(
        const OpenXLSX::XLWorksheet& fSheet,
        int iKeyPageNumber);    // Находится в solveFirstPage.cpp

    // В solveSecondPage.h и solveSecondPage.cpp
    unique_ptr<FSolveSecondPage>
        ptrSolveSecondPage;    // Композиция (вынес в отдельный
                               // класс, так как много методов)
    shared_ptr<FGlobal> ptrGlobal;    // Синглтон
    vector<regex>       arrRegexComp;
    vector<regex> arrRegexHeaderInd;    // Может быть несколько, поэтому вектор
    vector<regex> arrRegexHeaderComp;    // Может быть несколько, поэтому вектор
    vector<regex> arrRegexCodeUGSN;    // Может быть несколько, поэтому вектор
};
