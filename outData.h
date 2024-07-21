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
    void OutTotalData();

    // Не забываем, что OpenXLSX нумерует всё от единицы (1)
    void OutTable(
        const int& iShiftX,    // С каким смещением выводим
        const int& iShiftY,    // С каким смещением выводим
        FDataType              fData,    // Что выводим
        OpenXLSX::XLWorksheet& WKS
    );    // Куда выводим

    void OutDataCeil(const int& x, const int& y, OpenXLSX::XLWorksheet& WKS,
                 const FTableData& fData);
    
    void OutDataCeil(const int& x, const int& y, OpenXLSX::XLWorksheet& WKS,
                     const string& sData);

    string sOutPath;

private:
    static int iSinglControll;    // Проверка на синглтон

    shared_ptr<FGlobal> ptrGlobal;    // Синглтон
};