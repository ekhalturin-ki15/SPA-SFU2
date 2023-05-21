#pragma once
#include "global.h"

struct FConfig
{
    vector<wstring> arrNameKeyPage;

    vector<wstring> arrNameFileIn;
    vector<wstring> arrNameFileOut;
    wstring wsNameLableFile;
    wstring wsNameRibFile;

    FConfig();

    int iMaxLen;

    void Init(string _sNameConfig, string _sNamePage);

    void SetParams(wstring wsKey, OpenXLSX::XLRow row);


};