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

    vector<FPageInfo> arrKeyPage;

    vector<wstring> arrNameFileIn;

    vector<wstring> arrNameFileOut;
    map< wstring, wstring > mapOutParams;

    wstring wsNameLableFile;
    wstring wsNameRibFile;

    wstring wsRegexComp;
    wstring wsFormula;

    wstring wsNameDebugFile;
    wstring wsNameLogFile;

    bool bCreateFolder;
    bool bCompactOutput;
    bool bReloadLogFile;
    bool bMultiIndicator;

    int iMaxLen;
    int iWeigthRib;
    int iCourseLen;
    int iIgnoreEmptyLine;

    std::wstring wsNameConfig; // "Параметры"
    std::wstring wsNamePage; 

    void Init();

    Alias fAlias; //Явная композиция

private:

    void SetParams(OpenXLSX::XLWorkbook& fBook, wstring wsKey, OpenXLSX::XLRow row);
    vector<set<wstring>> SetParsingParams(OpenXLSX::XLWorksheet& fPage);

    FGlobal* ptrGlobal; //Синглтон
};