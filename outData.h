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

    // ��������� ����� �����
    void Create(string sOutPath);    // sOutPath - �������, ��� ����� �����

private:
    static int iSinglControll;    // �������� �� ��������

    shared_ptr<FGlobal> ptrGlobal;    // ��������
};