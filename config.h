#pragma once
#include "global.h"

struct FGlobal;
//
struct FPageInfo
{
    wstring wsName;
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
    void TakeData(bool& outBData, OpenXLSX::XLRow& row); // Возвращение результата через параметры
    void TakeData(vector<wstring>& outArrData, OpenXLSX::XLRow& row); // Возвращение результата через параметры
    void TakeData(wstring& outWsData, OpenXLSX::XLRow& row); // Возвращение результата через параметры
    void TakeData(int& outIData, OpenXLSX::XLRow& row); // Возвращение результата через параметры

    void SetParams(OpenXLSX::XLWorkbook& fBook, wstring wsKey, OpenXLSX::XLRow row);
    vector<set<wstring>> SetParsingParams(OpenXLSX::XLWorksheet& fPage, const wstring& wsNamePage, const int& iNumPage);
    vector<pair<wstring, wstring>> SetMapParams(const OpenXLSX::XLWorksheet& fPage);

    FGlobal* ptrGlobal; //Синглтон
};