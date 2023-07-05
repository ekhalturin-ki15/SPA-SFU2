#pragma once
#include "global.h"

struct FGlobal;
struct FTreeDisc;
struct FTreeMetric;

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
    vector<wstring> arrHead;
    vector<int>     arrOutColm;
    vector<double>  arrResult;

    // Пропроска страницы Excel для вывода данных (например, при помощи OutAddInfo)

    // Библиотека OpenXLSX кривая, объекты XLDocument не очищаются после close, поэтому, делаем дубликаты
    vector<OpenXLSX::XLDocument>  arrOpenFile;
    vector<OpenXLSX::XLWorksheet> arrOpenWKS;
   
    void CreateAndTake(string sName, string sPath);    // Вывод через глобальные fOpenFile и fOpenWKS
    void OutAddInfo(FTreeDisc* ptrTree);
    void OutAddTotalInfo(FTreeDisc* ptrTree, int y);

    int OutRectAddInfo(int x, int y, FTreeMetric* ptrMetric, bool bIsCourse, const double dAllSum);    // Возвращает Актуальное занчение y

    void OutGephiData(string sName, string sPath, FTreeDisc* fTree);    // Вывод данных о графе для Gephi в формате csv

    FGlobal* ptrGlobal;    // Синглтон

    map<int, FСorridor> mapSaveData;
    void                RetakeMinMax(FСorridor& fSaveData, const double& dNewData, const string& sNewData);
    void                OutData(int&                   x,
                                int&                   index,
                                const int&             y,
                                double                 dDate,
                                string                 sDate,
                                OpenXLSX::XLWorksheet& wks,
                                string                 sOutData,
                                const bool&            bIsConsider,
                                const int&             iXShift,
                                const int&             iYShift);

    void OutGephiLable(const string&         sName,
                       const string&         sNameFile,
                       const string&         sPath,
                       const vector<string>& arrNameLabel);    // Вывод данных о графе для Gephi в формате csv

    void OutGephiRib(const string& sName,
                     const string& sNameFile,
                     const string& sPath,
                     const vector<vector<pair<int, double>>>& fAdjList);    // Вывод данных о графе для Gephi в формате csv

    string AddCompString(const map<string, vector<string>>& mapComp);

    int iXShift, iYShift;
};