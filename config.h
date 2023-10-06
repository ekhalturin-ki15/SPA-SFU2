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

struct FConfig
{
    explicit FConfig(FGlobal* _ptrGlobal);
    ~FConfig();

    vector<FPageInfo> arrKeyPage;

    vector<wstring>               arrTagDisc;
    vector<wstring>               arrNameTagDisc;

    vector<wstring>               arrNameFileIn;
    vector<wstring>               arrNameFileOut;

    map<wstring, vector<wstring>> mapArrOutParams;
    map<wstring, wstring>         mapAddOutParams;
    map<wstring, wstring>         mapAliasRename;

    set<wstring>                  setIgnoreDisc;
    set<string>                   setIgnoreСurriculum;    // Какие УП игнорировать впринципе

    set<char> setIgnoreCharCompHeader;

    string sNameLabelHeader;
    string sNameRibHeader;
    string sNameRibDir;
    string sPrefCourseNumber;
    

    string sRegexComp;
    string sRegexHeaderIndicator;
    string sRegexHeaderComp;
    string sFormula;

    //wstring wsOutPrefMinMax;
    //string sOutPrefAllCurriculaAllCourse;
    //string sOutPrefAllCurriculaCurrentCourse;
    //string sSufAltGraphFile;
    //wstring wsOutPrefAllCourse;
    //wstring wsOutSufAmountComp;

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
    bool bDeletingSpecCharDiscName;
    bool bIsNormalizeScoreComp;

    vector<bool> bArrIsSolveGraphMetric;

    double dMinWeigthRib;
    double dMinComp;

    double dAnomalBigScore;

    int iMaxNameDiscLen;
    int iWeigthRib;
    int iCourseLen;
    int iIgnoreEmptyLine;
    int iSoMachComp;

    std::wstring wsNameConfig;    // "Параметры"
    std::wstring wsNamePage;

    bool Init();

private:
    static int iSinglControll;

    bool                 SetParams(OpenXLSX::XLWorkbook& fBook, wstring wsKey, OpenXLSX::XLRow row);
    vector<set<wstring>> SetParsingParams(OpenXLSX::XLWorksheet& fPage);

    FGlobal* ptrGlobal;    // Синглтон
};