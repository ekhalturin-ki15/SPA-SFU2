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
    FDataType                  fTotalData;
    map<ETypeGraph, FDataType> mapOutData;

    // По курсам и по всем курсам вместе (FMetric::sAllMetric)
    map<string, FDataType>
        mapCompTreeData;    // Так как названия курсов даны строкой

private:
    void CreateHeader();
    void CreateData();

    void CreateTotalHeader();
    void CreateGraphHeader();
    void CreateCompTreeHeader();

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
    void CreateRectCompTreeData(vector<vector<FTableData>>& arrReturnData,
                                shared_ptr<FTreeMetric>
                                    ptrMetric);

    void CreateTableRectInfo(
        const bool& bIsCounting,    // Если true - то проход в холостую для
                                    // определения iSizeX
        vector<vector<FTableData>>&
            arrReturnData,    // Возвращаемое значение с функции
        int& iSizeX,    // Если считаем вхолостую, когда bIsCounting == true, то
                        // записываем в iSizeX результат
        int& iCurrentY,    // Глобальная переменая
        shared_ptr<FTreeMetric> ptrMetric, int iDeep);

    void CompHeaderCreate(vector<string>& arrHeader);
    void QuartileHeaderCreate(
        vector<string>&                     arrHeader,
        const vector<pair<string, string>>& arrQuartileHead);

private:
    const vector<wstring>                arrOriginMetricGraphHead;
    const vector<wstring>                arrOriginMetricTotalHead;
    const vector<pair<wstring, wstring>> arrOriginQuartileHead;
    const vector<wstring>                arrOriginCorridorHeader;
    const vector<wstring>                arrOriginCompTreeHeader;

    static int iSinglControll;        // Проверка на синглтон
    shared_ptr<FGlobal> ptrGlobal;    // Синглтон
};
