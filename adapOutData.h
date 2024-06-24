#pragma once
#include "global.h"

struct FGlobal;
struct FCurricula;
struct FTreeMetric;
struct FTypeGraph;

struct ETypeGraph;

struct FAdapOutData
{
    explicit FAdapOutData(shared_ptr<FGlobal> _ptrGlobal);

    bool Init();

    // Начальная точка входа
    void Create();

public:
    vector<ETypeGraph> arrTypeTotalStatistic;
   

private:

    void CreateHeader();
    vector<string> arrHeaderRow;


private:
    static int iSinglControll;        // Проверка на синглтон
    shared_ptr<FGlobal> ptrGlobal;    // Синглтон
};
