#pragma once
#include "global.h"

struct FGlobal;
//
struct FPageInfo
{
    wstring wsName;
    vector<set<wstring>> arrHeader; // Для каждого столбца есть множество синонимов
};


struct FConfig
{

    FConfig(FGlobal* _ptrGlobal);

    vector<FPageInfo> arrKeyPage;

    vector<wstring> arrNameFileIn;
    vector<wstring> arrNameFileOut;
    wstring wsNameLableFile;
    wstring wsNameRibFile;

    wstring wsRegexComp;

    wstring wsNameDebugFile;
    wstring wsNameLogFile;

    bool bCreateFolder;
    bool bReloadLogFile;
    bool bMultiIndicator;

    FConfig();

    int iMaxLen;
    int iWeigthRib;
    int iCourseLen;

    void Init(wstring _sNameConfig, wstring _sNamePage);

    void SetParams(OpenXLSX::XLWorkbook& FBook, wstring wsKey, OpenXLSX::XLRow row);

    FGlobal* ptrGlobal; //Синглтон
};