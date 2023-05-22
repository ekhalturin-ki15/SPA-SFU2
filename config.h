#pragma once
#include "global.h"

struct FGlobal;

struct FConfig
{

    FConfig(FGlobal* _fGlobal);

    vector<wstring> arrNameKeyPage;

    vector<wstring> arrNameFileIn;
    vector<wstring> arrNameFileOut;
    wstring wsNameLableFile;
    wstring wsNameRibFile;

    wstring wsNameDebugFile;
    wstring wsNameLogFile;

    bool bCreateFolder;
    bool bReloadLogFile;

    FConfig();

    int iMaxLen;
    int iWeigthRib;

    void Init(wstring _sNameConfig, wstring _sNamePage);

    void SetParams(wstring wsKey, OpenXLSX::XLRow row);

    FGlobal* fGlobal; //Синглтон
};