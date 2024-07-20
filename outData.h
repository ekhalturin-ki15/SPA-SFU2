#pragma once
#include "global.h"
#include <map>

struct FGlobal;
struct FCurricula;
struct FTreeMetric;
struct FTypeGraph;
struct ETypeGraph;
struct FOutData;



struct FOutData
{
    explicit FOutData(shared_ptr<FGlobal> _ptrGlobal);
    ~FOutData();

    bool Init();

    // Начальная точка входа
    void Create(string sOutPath);    // sOutPath - Каталог, где будут файлы

private:
    static int iSinglControll;    // Проверка на синглтон

    shared_ptr<FGlobal> ptrGlobal;    // Синглтон
};