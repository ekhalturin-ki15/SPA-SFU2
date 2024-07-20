#pragma once
#include "global.h"
#include <any>

struct FGlobal;
struct FCurricula;
struct FTreeMetric;
struct FTypeGraph;
struct FTreeElement;
struct ETypeGraph;

struct FTableData
{
    any fData;
};

struct FDataType
{
    vector<string> arrHeader;
    vector<string> arrType;

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
    // Данные по всей выборке УП
    FDataType                  fTotalData;
    map<ETypeGraph, FDataType> mapOutData;

    // По курсам и по всем курсам вместе (FMetric::sAllMetric)
    vector<map<string, FDataType>>    // Для каждого отдельного УП
        arrMapCompTreeData;    // Так как названия курсов даны строкой

    vector<map<ETypeGraph, FDataType>> arrMapGephiLableCSVData;
    vector<map<ETypeGraph, FDataType>> arrMapGephiRibCSVData;

private:
    // Header
    void CreateHeader();

    void CreateTotalHeader();
    void CreateGraphHeader();
    void CreateCompTreeHeader();

    void CreateGephiCSVHeader();

    void CompHeaderCreate(vector<string>& arrHeader);
    void QuartileHeaderCreate(
        vector<string>&                     arrHeader,
        const vector<pair<string, string>>& arrQuartileHead);
    // Data
    void CreateData();

    void CreateTotalData();
    void CreateGraphData();

    // В самом конце, так как считаем меры центральной тенденции для всего ранее
    // посчитанного
    void CreateDataCorridorAndType();

    // Вызывается внутри CreateDataCorridor
    void CreateType(FDataType& fData);

    bool CalcDataCorridor(vector<vector<FTableData>>&       arrDataCorridor,
                          const vector<vector<FTableData>>& arrData,
                          const int&                        iCol,
                          const string&                     sType);

    void CreateCompTreeData();
    void CreateCompTreeData(vector<vector<FTableData>>& arrReturnData,
                            shared_ptr<FTreeMetric>
                                ptrMetric);

    void CreateRectCompTreeData(vector<vector<FTableData>>& arrReturnData,
                                vector<FTableData>          arrRow,
                                shared_ptr<FTreeMetric> ptrMetric, int x,
                                int iDeep);

    void CreateGephiCSVData();
    void CreateGephiLableCSVData();
    void CreateGephiRibCSVData();

private:
    string CreateCommonNameLabel(const ETypeGraph&               eTypeGraph,
                                 const pair<wstring, int>&       fInfo,
                                 const shared_ptr<FTreeElement>& ptrNode);
    string AddCompString(const map<string, vector<string>>& mapComp);

    string CreateTag(
        const ETypeGraph& eGraphType, const wstring& wsName,
        const map<wstring, shared_ptr<FTreeElement>>& mapAllDisc);


private:
    const vector<wstring>                arrOriginMetricGraphHead;
    const vector<wstring>                arrOriginMetricTotalHead;
    const vector<pair<wstring, wstring>> arrOriginQuartileHead;
    const vector<wstring>                arrOriginCorridorHeader;
    const vector<wstring>                arrOriginCompTreeHeader;

    static int iSinglControll;        // Проверка на синглтон
    shared_ptr<FGlobal> ptrGlobal;    // Синглтон
};
