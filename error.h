#pragma once
#include "global.h"

struct FGlobal;

struct FError
{

    FError(FGlobal* _ptrGlobal);
    void Init();
        
    void ErrorInFileNotFind(wstring wsName);// ���� ������ ��� �������� ����� � ���� zip
    void ErrorInFileNotFind(string sName);
    void ErrorOutFileNotFind(wstring wsName);
    void ErrorOutFileNotFind(string sName);

    void ErrorUncorrectExtension(wstring wsName);
    void ErrorUncorrectExtension(string sName);

    void ErrorBadTree(wstring wsName); // ���� ������ ��� ���������� ������ ���������
    void ErrorBadTree(string sName); // ���� ������ ��� ���������� ������ ���������


    FGlobal* ptrGlobal; //��������
};