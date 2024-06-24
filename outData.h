#pragma once
#include "global.h"
#include <map>

struct FGlobal;
struct FCurricula;
struct FTreeMetric;
struct FTypeGraph;
struct ETypeGraph;
struct FOutData;

// Куда производится вывод (на главную горизонтально, или в побочные файлы)
enum EOutType;

struct FCorridorAdapter
{
public:
    explicit FCorridorAdapter(shared_ptr<FGlobal> _ptrGlobal);

    map<int, vector<pair<double, string>>> Take(const int& iSize);

    void Add(int key, pair<double, string> fData);

private:
    map<int, vector<pair<double, string>>> mapCorridorData;
    shared_ptr<FGlobal>                    ptrGlobal;
};

struct FFileCache
{
    explicit FFileCache(shared_ptr<FOutData> _ptrPerent)
        : ptrPerent(_ptrPerent) {};

    // Проброска страницы Excel для вывода данных (например, при помощи
    // OutAddInfo)

    // Библиотека OpenXLSX кривая, объекты XLDocument не очищаются после close,
    // поэтому, делаем дубликаты Синглтон использование
    vector<OpenXLSX::XLDocument>  arrOpenFile;
    vector<OpenXLSX::XLWorksheet> arrOpenWKS;

    vector<OpenXLSX::XLDocument>  arrCourseOpenFile;
    vector<OpenXLSX::XLWorksheet> arrCourseOpenWKS;

private:
    shared_ptr<FOutData> ptrPerent;
};

struct FOutData
{
    explicit FOutData(shared_ptr<FGlobal> _ptrGlobal);
    ~FOutData();

    bool Init();

    // Начальная точка входа
    void Create(string sOutPath);    // sOutPath - Каталог, где будут файлы

private:
    string sCurPlanName;

    int iSizeOnlyAllow;

    std::map<string, vector<vector<string>>> mapTotalDataOut;

    shared_ptr<FFileCache> fFileCache;    // Класс хранения информации о файлах
                                          // Excel для
    // FOutData (агрегация, а не композиция, чтобы
    // сделать методы Out константными)

    // Составляет таблицу для вывода информации общего вида (не имеющей
    // отношения к графам) всех УП FSolve
    void CreateAllCurriculaTotalData(vector<vector<string>>& arrReturnData);

    // Составляет таблицу для вывода информации о всех графов одного типа из
    // всех УП FSolve
    // Метрики графового представления УП
    void CreateSummaryTotalData(vector<vector<string>>& arrReturnData,
                                const ETypeGraph&       eGraphType);

    // Данные про дерево компетенций УП
    void OutAddInfo(string sName, string sPath, shared_ptr<FCurricula> ptrTree);

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
    void CreateOnlyAllowedResultRow(
        vector<string>& arrReturn, const int& iSizeHeader,
        vector<bool>& arrIsAllowed,
        // const string&         sCurName, // Лучше передать FCurricula*
        const vector<double>& arrResult, FCorridorAdapter& fCorridorData,
        shared_ptr<FCurricula> ptrCurricula,
        const vector<wstring>& arrOnlyAllow);

    void AddTableCommonData(vector<vector<string>>& arrToAddedData,
                            FCorridorAdapter&       fCorridorData);

    // map<int, FСorridor> mapSaveData;
    void RetakeCommon(FCorridorAdapter& fSaveData, const int& iKey,
                      const double& dNewData, const string& sNewData);

    // Вместо вывода теперь формирую строки при помощи TakePasteData, а потом
    // единожды всё вывожу
    // Возвращает true, если данные выводятся и надо увеличить значение x
    bool TakePasteData(const int& x, vector<string>& arrCurRow,
                       const bool& bIsOutData, const double& dDate,
                       const string& sOutData, const string& sCurName,
                       const bool& bIsConsider, FCorridorAdapter& fSaveData);

    void CreateTotalInfo(vector<vector<string>>&      arrReturnData,
                         const shared_ptr<FTypeGraph> fGraph);
    // const EOutType& eOutType);

    void CreateTotalInfo(vector<double>&              arrReturnDataMetrics,
                         const shared_ptr<FTypeGraph> fGraph);
    // const EOutType& eOutType);

    void CreateTotalInfo(vector<string>&              arrReturnDataHeader,
                         const shared_ptr<FTypeGraph> fGraph);
    // const EOutType& eOutType);

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

    void
        OutTableInfoCSV(const vector<vector<string>>& arrData,    // Что выводим
                        const string& sPath, const string& sNameFile,
                        const string& sName,
                        const int& iShiftX = 0,    // С каким смещением выводим
                        const int& iShiftY = 0    // С каким смещением выводим
        );                                        // Куда выводим

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
                             shared_ptr<FTreeMetric>
                                  ptrMetric,
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
        int& iCurrentY,    // Глобальная переменая
        shared_ptr<FTreeMetric> ptrMetric, int iDeep
        // const bool& bIsLocal = false
    );    // Возвращает Актуальное значение y

    void CreateGraphE1TableInfoInit(
        map<string, map<string, string>>& fReturnData,
        shared_ptr<FTreeMetric>
            ptrMetric);

    void CreateGraphE1TableRectInfo(
        const bool& bIsCounting,    // Если true - то проход в холостую для
                                    // определения iSizeX
        vector<vector<string>>&
            arrReturnData,    // Возвращаемое значение с функции
        int x,
        int& iSizeX,    // Если считаем вхолостую, когда bIsCounting == true, то
                        // записываем в iSizeX результат
        int& iCurrentY,    // Глобальная переменая
        shared_ptr<FTreeMetric>
            ptrMetric,
        int iDeep);    // Возвращает Актуальное значение y

    vector<string> CreateCommonNameLabel(const ETypeGraph& eGraphType,
                                         shared_ptr<FCurricula>
                                             fCurricula);

    vector<string> CreateTag(const ETypeGraph& eGraphType,
                             shared_ptr<FCurricula>
                                  fCurricula,
                             bool bCheckTag = true);

    void OutGephiData(
        string sName,
        string sPath,
        shared_ptr<FCurricula>
            fCurricula);    // Вывод данных о графе для Gephi в формате csv

private:
    static int iSinglControll;    // Проверка на синглтон

    shared_ptr<FGlobal> ptrGlobal;    // Синглтон
    string AddCompString(const map<string, vector<string>>& mapComp);

    const vector<wstring> arrMetricHead;
};