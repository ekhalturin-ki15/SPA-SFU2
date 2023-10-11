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

    void ErrorInFileNotFind(
        string sPathName) const;    // Если ошибки при открытии файла в виде zip
    void ErrorInFileNotFind(wstring wsPathName)
        const;    // Если ошибки при открытии файла в виде zip

    void ErrorOutFileNotFind(string wsPathName) const;
    void ErrorOutFileNotFind(wstring wsPathName) const;

    void ErrorOutFileCreate(string wsPathName) const;
    void ErrorOutFileCreate(wstring wsPathName) const;

    void ErrorNotFoundConfig() const;

    void FatalErrorFewConfigPages() const;

    void ErrorUncorrectExtension() const;

    void ErrorBadTree() const;    // Если ошибки при построении дерева дисциплин

    void ErrorNotFoundKeyCol()
        const;    // Если ошибки при нахождении ключевого поля

    void ErrorToMuchColums()
        const;    // Если ошибки при построении дерева дисциплин

    void ErrorNotEqualSum() const;

    void ErrorBadConfigSizeParams(string sNameParams, string sInfo) const;

    void ErrorBadParser() const;

    void ErrorBadFormula();

    void ErrorGraphNoInitWeightDisc(string  sNamePlan,
                                    wstring wsNameIndex) const;
    void ErrorGraphNoInitWeightDisc(string sNamePlan, string sNameIndex) const;

    // void ErrorGraphBadAllScore(string sNamePlan, int iTypeGraph, int
    // iTypeError);
    void ErrorGraphZeroValue(string sNamePlan, wstring wsNameIndex) const;
    void ErrorGraphZeroValue(string sNamePlan, string sNameIndex) const;

    void ErrorGraphZeroComp(string sNamePlan, wstring wsNameIndex) const;
    void ErrorGraphZeroComp(string sNamePlan, string sNameIndex) const;

    bool bIsPrint;    // Выводим один раз (пока false)

    void        ErrorBadRegex(string sName);
    set<string> setBadRegexName;

    void ErrorEmptyLine() const;

    void ErrorNoFindCourse(wstring wsNameDisc) const;
    void ErrorNoFindCourse(string sNameDisc) const;

    void ErrorBadParserName(wstring wsIndexName) const;
    void ErrorBadParserName(string sIndexName) const;

    void ErrorConfiqDublicateNameDisc(wstring wsNameDisc) const;
    void ErrorConfiqDublicateNameDisc(string sNameDisc) const;

    void ErrorBadParserComp(wstring wsIndexName) const;
    void ErrorBadParserComp(string sIndexName) const;

    void ErrorBadIndicatorBind(wstring wsIndexName, wstring wsIndicator) const;
    void ErrorBadIndicatorBind(string sIndexName, string sIndicator) const;

    void ErrorAnomalBigScore(double dAmount) const;

    void OKParsing() const;

    void WAParsing() const;    // WA - wrong answer

    map<pair<wstring, string>, wstring>
        mapIndexDiscWithoutTag;    // Хранит Index и Name дисциплины
    // Дисциплины, для которых не указаны
    // теги, выводим их через FEroor в лог

    void OutDiscWithoutTag() const;

private:
    static int iSinglControll;

    ofstream OutHeader() const;

    FGlobal* ptrGlobal;    // Синглтон
};
