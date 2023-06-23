#pragma once
#include "global.h"

struct FGlobal;
//
struct FPageInfo
{
    wstring wsName;
    string sName; // Лучше один раз конвертировать (быстрее по времени)
    vector<set<wstring>> arrHeader; // Для каждого столбца есть множество синонимов
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

    vector<wstring> arrNameFileIn;

    vector<wstring> arrNameFileOut;
    map< wstring, wstring > mapOutParams;
    set< wstring > setIgnoreDisc;

    wstring wsNameLableFile;
    wstring wsNameRibFile;

    string sRegexComp;
    string sRegexHeaderComp;
    string sFormula;

    wstring wsNameDebugFile;
    wstring wsNameLogFile;

    bool bCreateFolder;
    bool bCompactOutput;
    bool bReloadLogFile;
    bool bMultiIndicator;
    bool bCompInterDelete;

    double dMinWeigthRib;
    double dMinComp;

    int iMaxLen;
    int iWeigthRib;
    int iCourseLen;
    int iIgnoreEmptyLine;

    std::wstring wsNameConfig; // "Параметры"
    std::wstring wsNamePage; 

    bool Init();

    Alias fAlias; //Явная композиция

private:

    static int iSinglControll;

    void SetParams(OpenXLSX::XLWorkbook& fBook, wstring wsKey, OpenXLSX::XLRow row);
    vector<set<wstring>> SetParsingParams(OpenXLSX::XLWorksheet& fPage);

    FGlobal* ptrGlobal; //Синглтон
};