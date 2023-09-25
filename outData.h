#pragma once
#include "global.h"

struct FGlobal;
struct FTreeDisc;
struct FTreeMetric;
struct FGraphType;

// Куда производится вывод (на главную горизонтально, или в побочные фалы
// вертикально)
enum EOutType : int
{
    EOT_Head = 1,
    EOT_Added
};

struct FСorridor
{
    double dMin = 0;
    string sMin = "";
    double dMax = 0;
    string sMax = "";
};

struct FOutData
{
    explicit FOutData(FGlobal* _ptrGlobal);

    void Out(string sOutPath);    // Каталог, где будут файлы

    bool Init();

private:
    const vector<wstring> arrMetricHead;    // Единожды задаётся в конструкторе
    const vector<wstring>
        arrCompetenceHead;    // Единожды задаётся в конструкторе

    vector<int> arrOutColm;
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
    void OutAddInfo(string sName, string sPath, FTreeDisc* ptrTree);
    // void OutAddTotalInfo(FTreeDisc* ptrTree, int y);
    void CreateTotalInfo(vector<vector<string>>& arrReturnData,
                         const FGraphType* fGraph, EOutType eOutType);

    void CreateTotalInfo(vector<double>&   arrReturnDataMetrics,
                         const FGraphType* fGraph, EOutType eOutType);

    void CreateTotalInfo(vector<string>&   arrReturnDataHeader,
                         const FGraphType* fGraph, EOutType eOutType);

    // Лучше буду генерировать матрицу, и выводить её сразу же
    void OutTableInfo(const int& iShiftX,    // С каким смещением выводим
                      const int& iShiftY,    // С каким смещением выводим
                      const vector<vector<string>>& arrData,    // Что выводим
                      OpenXLSX::XLWorksheet& WKS);    // Куда выводим
    // int OutRectAddInfo(int x, int y, FTreeMetric* ptrMetric, bool bIsCourse,
    // const double dAllSum, bool bIsLocal = false);    // Возвращает Актуальное
    // занчение y

    void CreateTableInfoInit(vector<vector<string>>& arrReturnData,
                             FTreeMetric* ptrMetric, bool bIsCourse,
                             const double dAllSum, bool bIsLocal = false);

    void CreateTableRectInfo(
        vector<vector<string>>&
            arrReturnData,    // Возвращаемое значение с функции
        int x, const int& iSizeX,
        int&         iCurrentY,    // Глобальная переменая
        FTreeMetric* ptrMetric, bool bIsCourse, const double dAllSum,
        bool bIsLocal = false);    // Возвращает Актуальное занчение y

    // Проход рекурсии вхолостую
    void CountRectArraySize(
        int& iSizeX,
        int& iSizeY,
        int  x,
        int  y,
        FTreeMetric* ptrMetric);    // Возвращает Актуальное занчение y

    vector<string> CreateCommonNameLabel(const int& iGraphType,
                                         FTreeDisc* fTree);
    void           OutGephiData(
                  string sName,
                  string sPath,
                  FTreeDisc* fTree);    // Вывод данных о графе для Gephi в формате csv

    FGlobal* ptrGlobal;    // Синглтон

    map<int, FСorridor> mapSaveData;
    void RetakeMinMax(FСorridor& fSaveData, const double& dNewData,
                      const string& sNewData);

    void OutData(int& x, int& index, const int& y, double dDate, string sDate,
                 OpenXLSX::XLWorksheet& wks, string sOutData,
                 const bool& bIsConsider, const int& iXShift,
                 const int& iYShift);

    void OutGephiLable(
        const string& sName, const string& sNameFile, const string& sPath,
        const vector<string>&
            arrNameLabel);    // Вывод данных о графе для Gephi в формате csv

    void OutGephiRib(
        const string& sName, const string& sNameFile, const string& sPath,
        const vector<vector<pair<int, double>>>&
            fAdjList);    // Вывод данных о графе для Gephi в формате csv

    string AddCompString(const map<string, vector<string>>& mapComp);

    // int iXShift, iYShift;
};