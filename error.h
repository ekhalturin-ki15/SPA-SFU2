#pragma once
#include "global.h"

struct FGlobal;

struct FError
{

    FError(FGlobal* _ptrGlobal);
    void Init();
        
    void ErrorInFileNotFind(wstring wsName);// ���� ������ ��� �������� ����� � ���� zip
    void ErrorOutFileNotFind(wstring wsName);

    void ErrorBadTree(string sName); // ���� ������ ��� ���������� ������ ���������


    FGlobal* ptrGlobal; //��������
};