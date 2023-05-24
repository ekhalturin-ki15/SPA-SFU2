#pragma once
#include "global.h"

struct FGlobal;

struct FError
{
    static const string sBadTree;
    static const string sNotAllData;

    FError(FGlobal* _ptrGlobal);
    void Init();
        
    void ErrorInFileNotFind(wstring wsName);// Если ошибки при открытии файла в виде zip
    void ErrorInFileNotFind(string sName);
    void ErrorOutFileNotFind(wstring wsName);
    void ErrorOutFileNotFind(string sName);

    void ErrorUncorrectExtension(wstring wsName);
    void ErrorUncorrectExtension(string sName);

    void ErrorBadTree(wstring wsName); // Если ошибки при построении дерева дисциплин
    void ErrorBadTree(string sName); // Если ошибки при построении дерева дисциплин

    void ErrorToMuchColums(wstring wsName); // Если ошибки при построении дерева дисциплин
    void ErrorToMuchColums(string sName); // Если ошибки при построении дерева дисциплин

    void ErrorBadParser(wstring wsName);
    void ErrorBadParser(string sName);

    void OKParsing(wstring wsName);
    void OKParsing(string sName);

    FGlobal* ptrGlobal; //Синглтон
};
