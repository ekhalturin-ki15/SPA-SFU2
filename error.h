#pragma once
#include "global.h"

struct FGlobal;

struct FError
{

    FError(FGlobal* _ptrGlobal);
    void Init();
        
    void ErrorInFileNotFind(wstring wsName);// Если ошибки при открытии файла в виде zip
    void ErrorOutFileNotFind(wstring wsName);

    void ErrorBadTree(string sName); // Если ошибки при построении дерева дисциплин


    FGlobal* ptrGlobal; //Синглтон
};