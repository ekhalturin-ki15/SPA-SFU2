#pragma once
#include "global.h"
#include <map>

struct FGlobal;
struct FCurricula;
struct FTreeMetric;
struct FTypeGraph;
struct FDataType;
struct FTableData;
struct FOutData;

struct FOutData
{
    explicit FOutData(shared_ptr<FGlobal> _ptrGlobal);
    ~FOutData();

    bool Init();

    // Начальная точка входа
    void Create(string _sOutPath);    // sOutPath - Каталог, где будут файлы

private:
    // FAdapOutData
    // FDataType fTotalData;
    // map<ETypeGraph, FDataType> mapGraphData;
    void OutTotalData();
    // FAdapOutData
    // vector<map<string, FDataType>> arrMapCompTreeData;
    void OutLocalData();
    // FAdapOutData
    // vector<map<ETypeGraph, pair<FDataType, FDataType>>> arrMapGephiCSVData;
    void OutLocalGephiCSVData();

    // Не забываем, что OpenXLSX нумерует всё от единицы (1)
    void OutTable(const int& iYShift, const int& iXShift,
                  const bool&            bIsOutHeader,
                  FDataType              fData,    // Что выводим
                  const bool&            bIsOutCorridor,
                  OpenXLSX::XLWorksheet* WKS = nullptr,
                  const string&          sNameDataCSVStream =
                      ""    // Если есть, то выводим через файл CVS файлы
    );    // Куда выводим

    void OutDataCeil(const int& y, const int& x, OpenXLSX::XLWorksheet& WKS,
                     const any& fData);

    string ConvertAnyToString(const any& fData);
    void   IncRow(int& iRow, ofstream* outDataCSVStream);

    void OutDataCeil(const int& y, const int& x, OpenXLSX::XLWorksheet& WKS,
                     const string& sData);

    string         sOutPath;
    vector<bool>   arrIsOutCorridor;
    vector<string> arrNameCorridor;

private:
    static int iSinglControll;        // Проверка на синглтон
    shared_ptr<FGlobal> ptrGlobal;    // Синглтон
};