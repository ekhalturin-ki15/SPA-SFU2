#pragma once
#include "global.h"
#include <map>

struct FGlobal;
struct FTreeDisc;
struct FTreeMetric;
struct FGraphType;

// Куда производится вывод (на главную горизонтально, или в побочные файлы
// вертикально)
enum EOutType : int
{
    EOT_Head = 1,
    EOT_Added
};

struct FСorridor
{
    vector<double> dMaxMin = { 0, 0 };
    vector<string> sMaxMin = { "", "" };
};

struct FOutData
{
    explicit FOutData(FGlobal* _ptrGlobal);

    // Начальная точка входа
    void Out(string sOutPath);    // sOutPath - Каталог, где будут файлы

    bool Init();

private:

    string sCurPlanName;

    int iSizeOnlyAllow;

    std::map< string, vector<vector<string>>> mapTotalDataOut;


    // void OutAddTotalInfo(FTreeDisc* ptrTree, int y);
    // Составляет таблицу для вывода информации общего вида (не имеющей
    // отношения к графам) всех УП FSolve
    void CreateAllCurriculaTotalData(vector<vector<string>>& arrReturnData);

    // Составляет таблицу для вывода информации о всех графов одного типа из
    // всех УП FSolve
    // Метрики графового представления УП
    void CreateSummaryTotalData(vector<vector<string>>& arrReturnData,
                                const int&              iGraphType);

    // Данные про дерево компетенций УП
    void OutAddInfo(string sName, string sPath, FTreeDisc* ptrTree);

    //Нельзя обобщать заголовки, это нарушает правило Лисков
    //const vector<wstring> arrMetricHead;    // Единожды задаётся в конструкторе
    //const vector<wstring>
    //    arrCompetenceHead;    // Единожды задаётся в конструкторе

    // vector<int> arrOutColm;
    // vector<double>  arrResult;

    // Пропроска страницы Excel для вывода данных (например, при помощи
    // OutAddInfo)

    // Библиотека OpenXLSX кривая, объекты XLDocument не очищаются после close,
    // поэтому, делаем дубликаты Синглтон использование
    vector<OpenXLSX::XLDocument>  arrSinglOpenFile;
    vector<OpenXLSX::XLWorksheet> arrSinglOpenWKS;

    vector<OpenXLSX::XLDocument>  arrSinglLocalCurrentCourseOpenFile;
    vector<OpenXLSX::XLWorksheet> arrSinglLocalCurrentCourseOpenWKS;

    void CreateAndTake(
        string sName,
        string sPath);    // Вывод через глобальные fOpenFile и fOpenWKS

    // Записать в arrReturn только те Заголовки, которые разрешены в
    // arrIsAllowed
    void CreateOnlyAllowedHeaderRow(vector<string>&        arrReturn,
                                    vector<bool>&          arrIsAllowed,
                                    const vector<wstring>& arrParams,
                                    const vector<wstring>& arrOnlyAllow);

    // Записать в arrReturn только те Результаты, которые разрешены в
    // arrIsAllowed

    // Вначале строки добавляет имя учебного плана (sCurName)
    void CreateOnlyAllowedResultRow(vector<string>&       arrReturn,
                                    const int&            iSizeHeader,
                                    vector<bool>&         arrIsAllowed,
                                    //const string&         sCurName, // Лучше передать FTreeDisc*
                                    const vector<double>& arrResult,
                                    map<int, FСorridor>&  mapCorridorData,
        FTreeDisc* ptrTree, const vector<wstring>& arrOnlyAllow);

    void AddTableMaxMinData(vector<vector<string>>& arrToAddedData,
                            map<int, FСorridor>&    mapCorridorData);

    void CreateTotalInfo(vector<vector<string>>& arrReturnData,
                         const FGraphType* fGraph, const EOutType& eOutType);

    void CreateTotalInfo(vector<double>&   arrReturnDataMetrics,
                         const FGraphType* fGraph, const EOutType& eOutType);

    void CreateTotalInfo(vector<string>&   arrReturnDataHeader,
                         const FGraphType* fGraph, const EOutType& eOutType);

    // Лучше буду генерировать матрицу, и выводить её сразу же
    // Не забываем, что OpenXLSX нумерует всё от единицы (1)
    void OutTableInfo(
        const int& iShiftX,    // С каким смещением выводим
        const int& iShiftY,    // С каким смещением выводим
        const vector<vector<string>>& arrData,    // Что выводим
        OpenXLSX::XLWorksheet&        WKS,
        int iShiftDataX = 0,    // Возможно, потребуется выводить не всю таблицу
                                // целиком, а только её часть
        int iShiftDataY = 0

    );    // Куда выводим

    void OutTableInfoCSV(
        const vector<vector<string>>& arrData,    // Что выводим
                        const string& sPath, const string& sNameFile,
                        const string& sName,
                        const int& iShiftX = 0,    // С каким смещением выводим
                        const int& iShiftY = 0   // С каким смещением выводим
    );    // Куда выводим
    // int OutRectAddInfo(int x, int y, FTreeMetric* ptrMetric, bool bIsCourse,
    // const double dAllSum, bool bIsLocal = false);    // Возвращает Актуальное
    // занчение y


    
    void OutGephiLabel(
        const string& sPath, const string& sNameFile, const string& sName,
        const vector<string>& arrNameLabel, const vector<double>& arrWeightNode,
        const vector<string>& arrTag);    // Вывод данных о графе
                                          // для Gephi в формате csv

    void OutGephiRib(
        const string& sPath, const string& sNameFile, const string& sName,
        const vector<vector<pair<int, double>>>&
            fAdjList);    // Вывод данных о графе для Gephi в формате csv

    void CreateTableInfoInit(vector<vector<string>>& arrReturnData,
                             FTreeMetric*            ptrMetric,
                             bool bIsOutNameCur    // const double dAllSum,
                             // bool         bIsLocal = false
    );

    void CreateTableRectInfo(
        const bool& bIsCounting,    // Если true - то проход в холостую для
                                    // определения iSizeX
        vector<vector<string>>&
            arrReturnData,    // Возвращаемое значение с функции
        int x,
        int& iSizeX,    // Если считаем вхолостую, когда bIsCounting == true, то
                        // записываем в iSizeX результат
        int&         iCurrentY,    // Глобальная переменая
        FTreeMetric* ptrMetric, int iDeep
        // const bool& bIsLocal = false
    );    // Возвращает Актуальное значение y

    void CreateGraphE1TableInfoInit(
        map<string, map<string, string>>& fReturnData,
        FTreeMetric*                      ptrMetric);

    void CreateGraphE1TableRectInfo(
        const bool& bIsCounting,    // Если true - то проход в холостую для
                                    // определения iSizeX
        vector<vector<string>>&
            arrReturnData,    // Возвращаемое значение с функции
        int x,
        int& iSizeX,    // Если считаем вхолостую, когда bIsCounting == true, то
                        // записываем в iSizeX результат
        int&         iCurrentY,    // Глобальная переменая
        FTreeMetric* ptrMetric,
        int iDeep);    // Возвращает Актуальное значение y

    //// Проход рекурсии вхолостую (теперь находится в CreateTableRectInfo)
    // void CountRectArraySize(
    //     int& iSizeX,
    //     int& iSizeY,
    //     int  x,
    //     FTreeMetric* ptrMetric);    // Возвращает Актуальное занчение y

    vector<string> CreateCommonNameLabel(const int& iGraphType,
                                         FTreeDisc* fTree);

    vector<string> CreateTag(const int& iGraphType, FTreeDisc* fTree);
    void           OutGephiData(
                  string sName,
                  string sPath,
                  FTreeDisc* fTree);    // Вывод данных о графе для Gephi в формате csv

    FGlobal* ptrGlobal;    // Синглтон

    // map<int, FСorridor> mapSaveData;
    void RetakeMinMax(FСorridor& fSaveData, const double& dNewData,
                      const string& sNewData);

    // Вместо вывода теперь формирую строки при помощи TakePasteData, а потом
    // единожды всё вывожу
    // Возвращает true, если данные выводятся и надо увеличить значение x
    bool TakePasteData(const int& x, vector<string>& arrCurRow,
                       const bool& bIsOutData, const double& dDate,
                       const string& sOutData, const string& sCurName,
                       const bool&          bIsConsider,
                       map<int, FСorridor>& mapCorridorData);

    string AddCompString(const map<string, vector<string>>& mapComp);

    // int iXShift, iYShift;
};