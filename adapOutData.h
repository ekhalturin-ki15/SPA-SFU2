#pragma once
#include "global.h"
#include <any>

struct FGlobal;
struct FCurricula;
struct FTreeMetric;
struct FTypeGraph;
struct FTreeElement;
struct ETypeGraph;
struct FOutData;

struct FTableData
{
    explicit FTableData();

    any fData;

    // По умолчанию = FGlobal::dNoInit;
    int iAddInfo;    // например, номер УП
};

struct FDataType
{
    vector<string> arrHeader;
    vector<int>   arrIsOut; // Требуется ли выводить

    vector<string> arrType; // Существуют следующие типы (из FGlobal ConvertAnyToString): 
    //string, int, double, char

    vector<vector<any>> arrData;
    vector<vector<FTableData>>
        arrDataCorridor;    // Меры центральной тенденции (мода, медиана и
                            // прочее...)
};

struct FAddGraphParams
{
    int X, Y, Z;
    int Color;
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
    map<ETypeGraph, FDataType> mapGraphData;

    // По курсам и по всем курсам вместе (FMetric::sAllMetric)
    vector<map<string, FDataType>>    // Для каждого отдельного УП
        arrMapCompTreeData;    // Так как названия курсов даны строкой

    vector<map<ETypeGraph, pair<FDataType, FDataType>>>
        arrMapGephiCSVData;    // first = Lable и second = Rib
    // vector<map<ETypeGraph, FDataType>> arrMapGephiRibCSVData;

private:
    // Header
    void CreateHeader();

    void CreateTotalHeader();
    void CreateGraphHeader();
    void CreateCompTreeHeader();

    void CreateGephiCSVHeader();

    void CompHeaderCreate(FDataType& fData);
    void QuartileHeaderCreate(FDataType&             fData,
                              const vector<wstring>& arrQuartileHead);
    // Data
    void CreateData();

    void CreateTotalData();
    void CreateGraphData();

    // В самом конце, так как считаем меры центральной тенденции для всего ранее
    // посчитанного
    void CreateDataCorridorAndType();

    // Вызывается внутри CreateDataCorridor
    void CreateType(FDataType& fData);

    bool CalcDataCorridor(vector<vector<FTableData>>& arrDataCorridor,
                          const vector<vector<any>>&  arrData,
                          const int&                  iCol,
                          const string&               sType);

    void CreateCompTreeData();
    int  CreateCompTreeData(
         int W, vector<vector<any>>& arrReturnData, const vector<int>& arrIsOut,
         shared_ptr<FTreeMetric>
             ptrMetric);    // Возвращает актуальную ширину дерева компетенций

    void CreateRectCompTreeData(vector<vector<any>>&    arrReturnData,
                                vector<any>&            arrRow,
                                shared_ptr<FTreeMetric> ptrMetric, int x,
                                int iDeep);

    void CreateGephiCSVData();
    void CreateGephiLableCSVData();
    void CreateGephiRibCSVData();

private:
    string CreateCommonNameLabel(const ETypeGraph&               eTypeGraph,
                                 const pair<wstring, int>&       fInfo,
                                 const shared_ptr<FTreeElement>& ptrNode);
    string AddCompString(const map<string, vector<string>>& mapComp,
                         bool                               bIsBrache = true);

    string AddCompString(const map<string, set<string>>& mapComp,
                         bool                            bIsBrache = true);

    string CreateTag(const ETypeGraph& eGraphType, const wstring& wsName,
                     const map<wstring, shared_ptr<FTreeElement>>& mapAllDisc);

    void SolveCompCreditAndPercent(any& fCredit, any& fPercent,
                                   shared_ptr<FTreeMetric> ptrTreeMetric,
                                   string                  sComp);

    //Выбор X Y Z и Color
    void CreateXYZColor(shared_ptr<FCurricula>         ptrCurricula,
                        const FTypeGraph&              fGraph,
                        map<pair<wstring, int>, FAddGraphParams>&
                            mapAddGraphParams);

public:
    const vector<wstring>
        arrOriginCorridorHeader;    // Для OutData (Разового подсчёта)
private:
    const vector<wstring> arrOriginMetricGraphHead;
    const vector<wstring> arrOriginMetricTotalHead;
    const vector<wstring> arrOriginQuartileHead;
    const vector<wstring> arrOriginCompTreeHeader;

    static int iSinglControll;        // Проверка на синглтон
    shared_ptr<FGlobal> ptrGlobal;    // Синглтон
};
