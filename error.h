#pragma once
#include "global.h"

struct FGlobal;

struct FError
{
    static const string sBadTree;
    static const string sDontHaveIndex;
    static const string sNotFoundKeyCol;
    static const string sNotEqualSum;
    static const string sNotInitConfig;

    explicit FError(FGlobal* _ptrGlobal);
    void Init();
        
    void ErrorInFileNotFind(string sPathName);// Если ошибки при открытии файла в виде zip
    void ErrorInFileNotFind(wstring wsPathName);// Если ошибки при открытии файла в виде zip
    
    void ErrorOutFileNotFind(string wsPathName);
    void ErrorOutFileNotFind(wstring wsPathName);

    void ErrorOutFileCreate(string wsPathName);
    void ErrorOutFileCreate(wstring wsPathName);

    void FatalErrorFewConfigPages();

    void ErrorUncorrectExtension();

    void ErrorBadTree(); // Если ошибки при построении дерева дисциплин

    void ErrorNotFoundKeyCol(); // Если ошибки при нахождении ключевого поля

    void ErrorToMuchColums(); // Если ошибки при построении дерева дисциплин

    void ErrorNotEqualSum();

    void ErrorBadParser();

    void ErrorEmptyLine();

    void ErrorBadParserName(wstring wsIndexName);
    void ErrorBadParserName(string sIndexName);

    void ErrorBadParserComp(wstring wsIndexName);
    void ErrorBadParserComp(string sIndexName);

    void ErrorBadIndicatorBind(wstring wsIndexName, wstring wsIndicator);
    void ErrorBadIndicatorBind(string sIndexName, string sIndicator);

    void OKParsing();

    void WAParsing(); // WA - wrong answer

private:

    ofstream OutHeader();

    FGlobal* ptrGlobal; //Синглтон
};
