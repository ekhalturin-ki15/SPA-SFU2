#pragma once
#include "global.h"

struct FGlobal;
struct FConfig;


struct FPageInfo
{
    wstring wsName;
    string sName;    // Лучше один раз конвертировать (быстрее по времени)
    vector<set<wstring>>
        arrHeader;    // Для каждого столбца есть множество синонимов
};


struct FTranslateData
{
    friend class FConfig;

    wstring GetName() const { return wsName; }
    
    bool GetTotal() const 
    {  
        if (arrFlag.size() < 1)
            throw std::out_of_range("Do not have Total Data |_|");
        return arrFlag.at(0);
    }

    bool GetLocal() const
    {
        if (arrFlag.size() < 2)
            throw std::out_of_range("Do not have Total Data \\_/");
        return arrFlag.at(1);
    }

    bool GetType(int iType) const
    {
        if (arrFlag.size() < iType)
            throw std::out_of_range("Do not have Total Data ~_~");
        return arrFlag.at(iType - 1);
    }

private: 
    wstring wsName;
    std::vector<bool> arrFlag;
};

struct FConfig
{
public:
    explicit FConfig(FGlobal* _ptrGlobal);
    ~FConfig();

    bool Init();

    // FPageInfo Params
private:
    vector<FPageInfo> arrKeyPage;

public:
    const FPageInfo& GetKeyPage(int iIndex) const;
    int              GetSizeKeyPage() const;

    // String Params
private:
    string sNameLabelHeader;
    string sNameRibHeader;
    string sNameRibDir;
    string sSeparator;
    string sPrefFullNameCourse;
    string sRegexComp;
    string sFormula;
    string sFormulaReverseGraph;
    string sNoInitData;
    string sNameFileTotalData;
    string sNameFileLocalData;
    string sNameFileCompData;

    // String Params adapter
private:
    void InitStringMap();
    map<wstring, string*>
        mapStringParamsReadKey;    // Инициализируется в конструкторе
public:
    string GetSNameLabelHeader() const { return sNameLabelHeader; }
    string GetSNameRibHeader() const { return sNameRibHeader; }
    string GetSNameRibDir() const { return sNameRibDir; }
    string GetSSeparator() const { return sSeparator; }
    string GetSPrefFullNameCourse() const { return sPrefFullNameCourse; }
    string GetSRegexComp() const { return sRegexComp; }
    string GetSFormula() const { return sFormula; }
    string GetSFormulaReverseGraph() const { return sFormulaReverseGraph; }
    string GetSNoInitData() const { return sNoInitData; }
    string GetSNameFileTotalData() const { return sNameFileTotalData; }
    string GetSNameFileLocalData() const { return sNameFileLocalData; }
    string GetSNameFileCompData() const { return sNameFileCompData; }

    // Int Params
private:
    int iMaxNameDiscLen;
    int iWeigthRib;
    int iCourseLen;
    int iIgnoreEmptyLine;
    int iSoMachComp;
    int iAmountQuar;
    int iPrecision;
    int iIndicatorDeep;

    // Int Params adapter
private:
    void InitIntMap();
    map<wstring, int*>
        mapIntParamsReadKey;    // Инициализируется в конструкторе
public:
    int GetIMaxNameDiscLen() const { return iMaxNameDiscLen; }
    int GetIWeigthRib() const { return iWeigthRib; }
    int GetICourseLen() const { return iCourseLen; }
    int GetIIgnoreEmptyLine() const { return iIgnoreEmptyLine; }
    int GetISoMachComp() const { return iSoMachComp; }
    int GetIAmountQuar() const { return iAmountQuar; }
    int GetIPrecision() const { return iPrecision; }
    int GetIIndicatorDeep() const { return iIndicatorDeep; }

    // Bool Params
private:
    bool bCreateFolder;
    bool bCompactOutput;
    bool bCourseOutput;
    bool bReloadLogFile;
    bool bIsOutCSVDate;
    bool bMultiIndicator;
    bool bCompInterDelete;
    bool bIsPercentRegAll;
    bool bOutCompWithName;
    bool bOutShortNameCur;
    bool bIsUnDirected;
    bool bDelSpecCharDiscName;
    bool bIsNormalizeScoreComp;
    bool bOutAllInfoWithoutTag;
    bool bOutEmptyComp;
    bool bOutTotalInfo;
    bool bOutWithoutEmptyCell;

    // Bool Params adapter
private:
    void InitBoolMap();
    map<wstring, bool*>
        mapBoolParamsReadKey;    // Инициализируется в конструкторе
public:
    bool GetBCreateFolder() const { return bCreateFolder; }
    bool GetBCompactOutput() const { return bCompactOutput; }
    bool GetBCourseOutput() const { return bCourseOutput; }
    bool GetBReloadLogFile() const { return bReloadLogFile; }
    bool GetBIsOutCSVDate() const { return bIsOutCSVDate; }
    bool GetBMultiIndicator() const { return bMultiIndicator; }
    bool GetBCompInterDelete() const { return bCompInterDelete; }
    bool GetBIsPercentRegAll() const { return bIsPercentRegAll; }
    bool GetBOutCompWithName() const { return bOutCompWithName; }
    bool GetBOutShortNameCur() const { return bOutShortNameCur; }
    bool GetBIsUnDirected() const { return bIsUnDirected; }
    bool GetBDelSpecCharDiscName() const { return bDelSpecCharDiscName; }
    bool GetBIsNormalizeScoreComp() const { return bIsNormalizeScoreComp; }
    bool GetBOutAllInfoWithoutTag() const { return bOutAllInfoWithoutTag; }
    bool GetBOutEmptyComp() const { return bOutEmptyComp; }
    bool GetBOutTotalInfo() const { return bOutTotalInfo; }
    bool GetBOutWithoutEmptyCell() const { return bOutWithoutEmptyCell; }

public:
    vector<wstring>
        arrTypeDisc;    // Определяем вид дисциплины (основ, факульт, по выбору)
    vector<wstring> arrNameTypeDisc;

    vector<wstring> arrNameFileIn;
    vector<wstring> arrNameFileOut;

    map < wstring, FTranslateData > mapArrOutParams;
    map<wstring, FTranslateData>
        mapAddOutParams;    // Вывод рекурсивного дерева

    map<wstring, wstring> mapAliasRename;

    vector<wstring> arrTagName;
    map<wstring, set<int>> mapTagDisc;    // Определяем, какая дисциплина
                                          // (гуманитарна, естеств, общепроф)

    set<wstring> setIgnoreDisc;
    set<string> setIgnoreСurriculum;    // Какие УП игнорировать впринципе

    set<char> setIgnoreCharCompHeader;

    vector<string> arrRegexHeaderInd;
    vector<string> arrRegexHeaderComp;

    wstring wsNameDebugFile;
    wstring wsNameLogFile;

    vector<bool> bArrIsSolveGraphMetric;

    // Объединять ли данные УП с данными графов
    vector<bool> bArrIsconcatGraphData;

    double dMinWeigthRib;
    double dMinComp;

    double dAnomalBigScore;
    double dTruncationAvg;

    wstring wsNameConfig;    // "Параметры"
    wstring wsNamePage;

private:
    static int iSinglControll;

    bool                 SetParams(OpenXLSX::XLWorkbook& fBook, wstring wsKey,
                                   OpenXLSX::XLRow row);
    vector<set<wstring>> SetParsingParams(OpenXLSX::XLWorksheet& fPage);

    FGlobal* ptrGlobal;    // Синглтон
};