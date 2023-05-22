#pragma once
#include "global.h"

struct FConfig
{
    vector<wstring> arrNameKeyPage;

    vector<wstring> arrNameFileIn;
    vector<wstring> arrNameFileOut;
    wstring wsNameLableFile;
    wstring wsNameRibFile;

    wstring wsNameDebugFile;
    wstring wsNameLogFile;

    bool bCreateFolder;

    FConfig();

    int iMaxLen;
    int iWeigthRib;

    void Init(wstring _sNameConfig, wstring _sNamePage);

    void SetParams(wstring wsKey, OpenXLSX::XLRow row);


};