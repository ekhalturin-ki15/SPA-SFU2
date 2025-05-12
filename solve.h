#pragma once

#include "global.h"
#include "solveSecondPage.h"

struct FGlobal;
struct FGraphAllData;
struct FSolveSecondPage;
struct FSolve;
struct FCurricula;
    
enum ETypeMetric : int
{
    ETM_NoExtended,
    ETM_Extended,    // Самый мягкий режим, больше всего дисциплин (в том числе
                     // и те, что не учитываются в УП)
    ETM_NoIgnore,    // Самый строгий режим, меньше всего дисциплин (применён
                     // список, кого исключить)
    ETM_Size
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
    unique_ptr<FGraphAllData>
        ptrGraphAllData;    // Объект для хранения общих данных
                            // всех объектов класса FGraph

    int                    N;
    shared_ptr<FCurricula> GetCurricula(int iNum);

    int iCurrentPage;    // Какая по счёту страница обрабатывается в данный
                         // момент
    int iCurrentRow;    // Какая по счёту строка обрабатывается

    bool bIsCorrectParsing;

    int iMaxCourse;    // Максимально встреченый курс (для сводной таблицы по
                       // всем курсам)

    string sInPath;    // Для отладки
    //string sOutPath;
    // Не могу перенести в FMetric, так как это общее для всех УП сразу
    set<string> setHeaderComp;    // Все заголовки компетенций (УК, ПК, ОПК).
                                  // Нужно для вывода

    friend FSolveSecondPage;

private:
    // Доступ осуществляется через Гетер  shared_ptr<FCurricula>
    // GetCurricula(int iNum)
    vector<shared_ptr<FCurricula>>
        arrDisc;    // Указатели на все УП, которые считали (все
                    // они одновременно хранятся в памяти)

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
