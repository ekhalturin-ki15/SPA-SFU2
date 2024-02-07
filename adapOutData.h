#pragma once
#include "global.h"

struct FGlobal;
struct FTreeDisc;
struct FTreeMetric;
struct FGraphType;

// Куда производится вывод (на главную горизонтально, или в побочные файлы
// вертикально)
// enum EOutType : int
//{
//    EOT_Head = 1,
//    EOT_Added
//};
//
// struct FСorridor
//{
//    vector<double> dMaxMin = { 0, 0 };
//    vector<string> sMaxMin = { "", "" };
//};

struct FAdapOutData
{
    explicit FAdapOutData(shared_ptr<FGlobal> _ptrGlobal);

    bool Init();

    // Начальная точка входа
    void Create(string sOutPath);    // sOutPath - Каталог, где будут файлы

public:
    // vector<vector<string>> arrAllCurriculaTotalData;

    // vector<vector<string>> arrAllCommonCoursesGraphData;

    // map<int, vector<vector<string>>> mapArrCourseGraphData;

    // map<FMetric, vector<vector<string>>> mapAddInfo;

private:
    // const vector<wstring> arrMetricHead;    // Единожды задаётся в
    // конструкторе const vector<wstring>
    //     arrCompetenceHead;    // Единожды задаётся в конструкторе

    //// Составляет таблицу для вывода информации общего вида (не имеющей
    //// отношения к графам) всех УП FSolve
    // void CreateAllCurriculaTotalData(vector<vector<string>>& arrReturnData);

    //// Составляет таблицу для вывода информации о всех графов одного типа из
    //// всех УП FSolve
    // void CreateSummaryTotalData(vector<vector<string>>& arrReturnData,
    //                             const int&              iGraphType);

    // void CreateTotalInfo(vector<vector<string>>& arrReturnData,
    //                      const FGraphType* fGraph, const EOutType& eOutType);

    // void CreateTotalInfo(vector<double>&   arrReturnDataMetrics,
    //                      const FGraphType* fGraph, const EOutType& eOutType);

    // void CreateTotalInfo(vector<string>&   arrReturnDataHeader,
    //                      const FGraphType* fGraph, const EOutType& eOutType);

    // void CreateTableInfoInit(vector<vector<string>>& arrReturnData,
    //                          FTreeMetric* ptrMetric,    // const double
    //                          dAllSum, bool         bIsLocal = false);

    // void CreateTableRectInfo(
    //     const bool& bIsCounting,    // Если true - то проход в холостую для
    //                                 // определения iSizeX
    //     vector<vector<string>>&
    //         arrReturnData,    // Возвращаемое значение с функции
    //     int x,
    //     int& iSizeX,    // Если считаем вхолостую, когда bIsCounting == true,
    //                     // то записываем в iSizeX результат
    //     int&         iCurrentY,    // Глобальная переменая
    //     FTreeMetric* ptrMetric, int iDeep,
    //     const bool& bIsLocal = false);    // Возвращает Актуальное занчение y

    //// Записать в arrReturn только те Заголовки, которые разрешены в
    //// arrIsAllowed
    // void CreateOnlyAllowedHeaderRow(vector<string>&        arrReturn,
    //                                 vector<bool>&          arrIsAllowed,
    //                                 const vector<wstring>& arrParams);

    //// Вначале строки добавляет имя учебного плана (sCurName)
    // void CreateOnlyAllowedResultRow(vector<string>&       arrReturn,
    //                                 const int&            iSizeHeader,
    //                                 vector<bool>&         arrIsAllowed,
    //                                 const string&         sCurName,
    //                                 const vector<double>& arrResult,
    //                                 map<int, FСorridor>&  mapCorridorData);

    // void AddTableMaxMinData(vector<vector<string>>& arrToAddedData,
    //                         map<int, FСorridor>&    mapCorridorData);

    // vector<string> CreateCommonNameLabel(const int& iGraphType,
    //                                      FTreeDisc* fTree);

    // vector<string> CreateTag(const int& iGraphType, FTreeDisc* fTree);
    //
    //// map<int, FСorridor> mapSaveData;
    // void RetakeMinMax(FСorridor& fSaveData, const double& dNewData,
    //                   const string& sNewData);

    //// Вместо вывода теперь формирую строки при помощи TakePasteData, а потом
    //// единожды всё вывожу
    //// Возвращает true, если данные выводятся и надо увеличить значение x
    // bool TakePasteData(const int& x, vector<string>& arrCurRow,
    //                    const bool& bIsOutData, const double& dDate,
    //                    const string& sOutData, const string& sCurName,
    //                    const bool&          bIsConsider,
    //                    map<int, FСorridor>& mapCorridorData);

    // string AddCompString(const map<string, vector<string>>& mapComp);

private:
    static int iSinglControll;        // Проверка на синглтон
    shared_ptr<FGlobal> ptrGlobal;    // Синглтон
};
