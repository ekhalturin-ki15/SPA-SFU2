#pragma once
#include "global.h"

struct FGlobal;
//
struct FPageInfo
{
    wstring              wsName;
    string               sName;        // Лучше один раз конвертировать (быстрее по времени)
    vector<set<wstring>> arrHeader;    // Для каждого столбца есть множество синонимов
};

struct Alias
{
    map<wstring, wstring> mapRename;
};

struct FConfig
{
    explicit FConfig(FGlobal* _ptrGlobal);
    ~FConfig();

    vector<FPageInfo> arrKeyPage;

    vector<wstring>               arrTagDisc;

    vector<wstring>               arrNameFileIn;
    vector<wstring>               arrNameFileOut;

    map<wstring, vector<wstring>> mapArrOutParams;
    map<wstring, wstring>         mapAddOutParams;

    set<wstring>                  setIgnoreDisc;
    set<string>                   setIgnoreСurriculum;    // Какие УП игнорировать впринципе

    set<char> setIgnoreCharCompHeader;

    string sNameLableHeader;
    string sNameRibHeader;
    string sNameRibDir;
    string sPrefCourseNumber;
    string sSufAltGraphFile;

    string sRegexComp;
    string sRegexHeaderIndicator;
    string sRegexHeaderComp;
    string sFormula;

    wstring wsOutPrefMinMax;
    wstring wsOutPrefAllCourse;
    wstring wsNameDebugFile;
    wstring wsNameLogFile;

    bool bCreateFolder;
    bool bCompactOutput;
    bool bCourseOutput;
    bool bReloadLogFile;
    bool bMultiIndicator;
    bool bCompInterDelete;
    bool bIsPercentRegAll;
    bool bOutCompWithName;
    bool bOutShortNameCurr;
    bool bIsUnDirected;

    vector<bool> bArrIsSolveGraphMetric;

    double dMinWeigthRib;
    double dMinComp;

    double dAnomalBigScore;

    int iMaxNameDiscLen;
    int iWeigthRib;
    int iCourseLen;
    int iIgnoreEmptyLine;

    std::wstring wsNameConfig;    // "Параметры"
    std::wstring wsNamePage;

    bool Init();

    Alias fAlias;    // Явная композиция

private:
    static int iSinglControll;

    bool                 SetParams(OpenXLSX::XLWorkbook& fBook, wstring wsKey, OpenXLSX::XLRow row);
    vector<set<wstring>> SetParsingParams(OpenXLSX::XLWorksheet& fPage);

    FGlobal* ptrGlobal;    // Синглтон
};