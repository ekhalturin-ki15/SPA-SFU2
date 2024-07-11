#pragma once
#include "global.h"
#include <any>

struct FGlobal;
struct FCurricula;
struct FTreeMetric;
struct FTypeGraph;

struct ETypeGraph;

struct FTableData
{
    any fData;
};

struct FDataType
{
    vector<string>             arrHeader;
    vector<string>             arrType;

    vector<vector<FTableData>> arrData;
    vector<vector<FTableData>>
        arrDataCorridor;    // Меры центральной тенденции (мода, медиана и
                            // прочее...)
};

struct FAdapOutData
{
    explicit FAdapOutData(shared_ptr<FGlobal> _ptrGlobal);

    bool Init();

    // Начальная точка входа
    void Create();

public:
    // FDataType                  fTotalOutData;

    static const ETypeGraph    ETG_Total;
    map<ETypeGraph, FDataType> mapOutData;

private:
    void CreateHeader();
    void CreateData();

    void CreateTotalHeader();
    void CreateGraphHeader();
    void CreateDataCorridor();
    void CalcDataCorridor(vector<vector<FTableData>>&       arrDataCorridor,
                          const vector<vector<FTableData>>& arrData,
                          const string& sType, const int& iCol);

    void CreateTotalData();
    void CreateGraphData();

    void CompHeaderCreate(vector<string>& arrHeader);
    void QuartileHeaderCreate(
        vector<string>&                     arrHeader,
        const vector<pair<string, string>>& arrQuartileHead);

private:
    const vector<wstring>                arrOriginMetricGraphHead;
    const vector<wstring>                arrOriginMetricTotalHead;
    const vector<pair<wstring, wstring>> arrOriginQuartileHead;
    const vector<wstring>                arrCorridorHeader;

    static int iSinglControll;        // Проверка на синглтон
    shared_ptr<FGlobal> ptrGlobal;    // Синглтон
};
