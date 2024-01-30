#pragma once
#include "global.h"

struct FGlobal;
struct FConfig;

// Тег дисциплины (гуманитарные, технические, естественнонаучные)
enum ETagDisc : int
{
    ETagD_Human, // гуманитарные
    ETagD_Tech, // технические
    ETagD_Natur, // естественнонаучные
    ETagD_Another, // те, что не были определены или сопоставленны чему-либо (ОШИБКА)
    ETagD_Size
};

struct FPageInfo
{
    wstring wsName;
    string sName;    // Лучше один раз конвертировать (быстрее по времени)
    vector<set<wstring>>
        arrHeader;    // Для каждого столбца есть множество синонимов
};

struct FTranslateData
{
    friend struct FConfig;

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
    wstring           wsName;
    std::vector<bool> arrFlag;
};

struct FConfig
{
public:
    explicit FConfig(shared_ptr<FGlobal> _ptrGlobal);
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
    const string& GetSNameLabelHeader() const { return sNameLabelHeader; }
    const string& GetSNameRibHeader() const { return sNameRibHeader; }
    const string& GetSNameRibDir() const { return sNameRibDir; }
    const string& GetSSeparator() const { return sSeparator; }
    const string& GetSPrefFullNameCourse() const { return sPrefFullNameCourse; }
    const string& GetSRegexComp() const { return sRegexComp; }
    const string& GetSFormula() const { return sFormula; }
    const string& GetSFormulaReverseGraph() const
    {
        return sFormulaReverseGraph;
    }
    const string& GetSNoInitData() const { return sNoInitData; }
    const string& GetSNameFileTotalData() const { return sNameFileTotalData; }
    const string& GetSNameFileLocalData() const { return sNameFileLocalData; }
    const string& GetSNameFileCompData() const { return sNameFileCompData; }

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
    const int& GetIMaxNameDiscLen() const { return iMaxNameDiscLen; }
    const int& GetIWeigthRib() const { return iWeigthRib; }
    const int& GetICourseLen() const { return iCourseLen; }
    const int& GetIIgnoreEmptyLine() const { return iIgnoreEmptyLine; }
    const int& GetISoMachComp() const { return iSoMachComp; }
    const int& GetIAmountQuar() const { return iAmountQuar; }
    const int& GetIPrecision() const { return iPrecision; }
    const int& GetIIndicatorDeep() const { return iIndicatorDeep; }

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
    const bool& GetBCreateFolder() const { return bCreateFolder; }
    const bool& GetBCompactOutput() const { return bCompactOutput; }
    const bool& GetBCourseOutput() const { return bCourseOutput; }
    const bool& GetBReloadLogFile() const { return bReloadLogFile; }
    const bool& GetBIsOutCSVDate() const { return bIsOutCSVDate; }
    const bool& GetBMultiIndicator() const { return bMultiIndicator; }
    const bool& GetBCompInterDelete() const { return bCompInterDelete; }
    const bool& GetBIsPercentRegAll() const { return bIsPercentRegAll; }
    const bool& GetBOutCompWithName() const { return bOutCompWithName; }
    const bool& GetBOutShortNameCur() const { return bOutShortNameCur; }
    const bool& GetBIsUnDirected() const { return bIsUnDirected; }
    const bool& GetBDelSpecCharDiscName() const { return bDelSpecCharDiscName; }
    const bool& GetBIsNormalizeScoreComp() const
    {
        return bIsNormalizeScoreComp;
    }
    const bool& GetBOutAllInfoWithoutTag() const
    {
        return bOutAllInfoWithoutTag;
    }
    const bool& GetBOutEmptyComp() const { return bOutEmptyComp; }
    const bool& GetBOutTotalInfo() const { return bOutTotalInfo; }
    const bool& GetBOutWithoutEmptyCell() const { return bOutWithoutEmptyCell; }

    // double Params
private:
    double dMinWeigthRib;
    double dMinComp;
    double dAnomalBigScore;
    double dTruncAvg;
    double dTruncQuarPathLen;

    // double Params adapter
private:
    void InitDoubleMap();
    map<wstring, double*>
        mapDoubleParamsReadKey;    // Инициализируется в конструкторе
                                   // (значение и размер (ограничение))
public:
    const double& GetDMinWeigthRib() const { return dMinWeigthRib; }
    const double& GetDMinComp() const { return dMinComp; }
    const double& GetDAnomalBigScore() const { return dAnomalBigScore; }
    const double& GetDTruncAvg() const { return dTruncAvg; }
    const double& GetDTruncQuarPathLen() const { return dTruncQuarPathLen; }

    // wstring Params
private:
    wstring wsNameDebugFile;
    wstring wsNameLogFile;

    // wstring Params adapter
private:
    void InitWStringMap();
    map<wstring, wstring*>
        mapWStringParamsReadKey;    // Инициализируется в конструкторе
                                    // (значение и размер (ограничение))

public:
    const wstring& GetWSNameDebugFile() const { return wsNameDebugFile; }
    const wstring& GetWSNameLogFile() const { return wsNameLogFile; }

    // vector<wstring> Params
private:
    vector<wstring>
        arrTypeDisc;    // Определяем вид дисциплины (основ, факульт, по выбору)
    vector<wstring> arrNameTypeDisc;
    vector<wstring> arrNameFileIn;
    vector<wstring> arrNameFileOut;
    vector<wstring> arrTagName;

    // vector<wstring> Params adapter
private:
    void InitVectorWStringMap();
    map<wstring, pair<vector<wstring>*, int>>
        mapVectorWStringParamsReadKey;    // Инициализируется в конструкторе
                                          // (значение и размер (ограничение))

public:
    const vector<wstring>& GetArrTypeDisc() const { return arrTypeDisc; }
    const vector<wstring>& GetArrNameTypeDisc() const
    {
        return arrNameTypeDisc;
    }
    const vector<wstring>& GetArrNameFileIn() const { return arrNameFileIn; }
    const vector<wstring>& GetArrNameFileOut() const { return arrNameFileOut; }
    const vector<wstring>& GetArrTagName() const { return arrTagName; }

public:
    map<wstring, FTranslateData> mapArrOutParams;
    map<wstring, FTranslateData>
        mapAddOutParams;    // Вывод рекурсивного дерева

    map<wstring, wstring> mapAliasRename;

    map<wstring, set<ETagDisc>> mapTagDisc;    // Определяем, какая дисциплина
                                          // (гуманитарна, естеств, общепроф)

    set<wstring> setIgnoreDisc;
    set<string> setIgnoreСurriculum;    // Какие УП игнорировать впринципе

    set<char> setIgnoreCharCompHeader;

    vector<string> arrRegexHeaderInd;
    vector<string> arrRegexHeaderComp;
    vector<string> arrRegexCodeUGSN;

    vector<bool> bArrIsSolveGraphMetric;

    // Объединять ли данные УП с данными графов
    vector<bool> bArrIsconcatGraphData;

private:
    static int iSinglControll;

    bool                 SetParams(OpenXLSX::XLWorkbook& fBook, wstring wsKey,
                                   OpenXLSX::XLRow row);
    vector<set<wstring>> SetParsingParams(OpenXLSX::XLWorksheet& fPage);

    wstring wsNamePage;    // Страница, откуда следует начать считывать
                           // параметры
    wstring  wsNameConfig;    // "Параметры"
    shared_ptr<FGlobal> ptrGlobal;       // Синглтон
};