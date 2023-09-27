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
    static const string sNotInitSolve;

    explicit FError(FGlobal* _ptrGlobal);
    ~FError();
    bool Init();

    void ErrorInFileNotFind(string sPathName);      // Если ошибки при открытии файла в виде zip
    void ErrorInFileNotFind(wstring wsPathName);    // Если ошибки при открытии файла в виде zip

    void ErrorOutFileNotFind(string wsPathName);
    void ErrorOutFileNotFind(wstring wsPathName);

    void ErrorOutFileCreate(string wsPathName);
    void ErrorOutFileCreate(wstring wsPathName);

    void ErrorNotFoundConfig();

    void FatalErrorFewConfigPages();

    void ErrorUncorrectExtension();

    void ErrorBadTree();    // Если ошибки при построении дерева дисциплин

    void ErrorNotFoundKeyCol();    // Если ошибки при нахождении ключевого поля

    void ErrorToMuchColums();    // Если ошибки при построении дерева дисциплин

    void ErrorNotEqualSum();

    void ErrorBadConfigSizeParams(string sNameParams, string sInfo);

    void ErrorBadParser();

    void ErrorBadFormula();

    void ErrorGraphNoInitWeightDisc(string sNamePlan, wstring wsNameIndex);
    void ErrorGraphNoInitWeightDisc(string sNamePlan, string sNameIndex);

    //void ErrorGraphBadAllScore(string sNamePlan, int iTypeGraph, int iTypeError);
    void ErrorGraphZeroValue(string sNamePlan, wstring wsNameIndex);
    void ErrorGraphZeroValue(string sNamePlan, string sNameIndex);

    void ErrorGraphZeroComp(string sNamePlan, wstring wsNameIndex);
    void ErrorGraphZeroComp(string sNamePlan, string sNameIndex);


    bool bIsPrint;    // Выводим один раз (пока false)

    void        ErrorBadRegex(string sName);
    set<string> setBadRegexName;

    void ErrorEmptyLine();

    void ErrorBadParserName(wstring wsIndexName);
    void ErrorBadParserName(string sIndexName);

    void ErrorBadParserComp(wstring wsIndexName);
    void ErrorBadParserComp(string sIndexName);

    void ErrorBadIndicatorBind(wstring wsIndexName, wstring wsIndicator);
    void ErrorBadIndicatorBind(string sIndexName, string sIndicator);

    void ErrorAnomalBigScore(double dAmount);

    void OKParsing();

    void WAParsing();    // WA - wrong answer

private:
    static int iSinglControll;

    ofstream OutHeader();

    FGlobal* ptrGlobal;    // Синглтон
};
