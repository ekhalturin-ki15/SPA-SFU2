#pragma once
#include "global.h"

#define XLSX ".xlsx"
#define CSV ".csv"
#define sDefaultName "Sheet1"
#define ENDL "\n"

struct FGlobal;
struct FConfig;

// Тег дисциплины (гуманитарные, технические, естественнонаучные)
enum ETagDisc : int
{
    ETagD_Human,      // гуманитарные
    ETagD_Tech,       // технические
    ETagD_Natur,      // естественнонаучные
    ETagD_Another,    // те, что не были определены или сопоставленны чему-либо
                      // (ОШИБКА)
    ETagD_Size
};

// Тип вывода (в Excel таблицы) в основные или в подробные сведения
// enum EOutType : int
//{
//     EOT_Head = 0,
//     EOT_Added,
//     EOT_Size
// };

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

    // bIsLocal - Требуется ли переводить?
    wstring GetName() const
    {
        return wsName;
    }

    bool GetTotal() const
    {
        if (arrFlag.size() < 1)
            throw std::out_of_range("Do not have Total Data |_|");
        return arrFlag.at(0);
    }

private:
    wstring          wsName;
    std::vector<int> arrFlag;
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
    // string sNameLabelHeader;
    // string sNameRibHeader;
    string sNameRibDir;
    string sSeparator;
    string sCSVSeparator;
    string sPrefFullNameCourse;
    // string sRegexComp;
    string sFormula;
    string sFormulaReverseGraph;
    string sNoInitData;
    string sNameFileTotalData;
    string sNameFileAllLocalData;
    string sNameFileCompData;

    string sNameDebugFile;
    string sNameLogFile;

    string sAllCourses;
    string sCurrentCourse;
    string sCorridorCurricula;
    string sSufComp;
    string sCommonCurricula;
    string sAltCurricula;
    string sRevCurricula;
    string sSufQuar;

    string sSufLableGephiFile;
    string sSufRibGephiFile;

    // String Params adapter
private:
    void InitStringMap();
    map<wstring, string*>
        mapStringParamsReadKey;    // Инициализируется в конструкторе
public:
    const string& GetSAllCourses() const
    {
        return sAllCourses;
    }

    const string& GetSCurrentCourse() const
    {
        return sCurrentCourse;
    }

    const string& GetSCorridorCurricula() const
    {
        return sCorridorCurricula;
    }

    const string& GetSSufQuar() const
    {
        return sSufQuar;
    }

    const string& GetSSufLableGephiFile() const
    {
        return sSufLableGephiFile;
    }

    const string& GetSSufRibGephiFile() const
    {
        return sSufRibGephiFile;
    }

    const string& GetSSufComp() const
    {
        return sSufComp;
    }

    /*const string& GetSCommonCurricula() const
    {
        return sCommonCurricula;
    }

    const string& GetSAltCurricula() const
    {
        return sAltCurricula;
    }

    const string& GetSRevCurricula() const
    {
        return sRevCurricula;
    }*/

    const string& GetSNameRibDir() const
    {
        return sNameRibDir;
    }
    const string& GetSSeparator() const
    {
        return sSeparator;
    }

    const string& GetSCSVSeparator() const
    {
        return sCSVSeparator;
    }
    const string& GetSPrefFullNameCourse() const
    {
        return sPrefFullNameCourse;
    }
    /*const string& GetSRegexComp() const
    {
        return sRegexComp;
    }*/
    const string& GetSFormula() const
    {
        return sFormula;
    }
    const string& GetSFormulaReverseGraph() const
    {
        return sFormulaReverseGraph;
    }
    const string& GetSNoInitData() const
    {
        return sNoInitData;
    }
    const string GetSNameFileTotalData() const
    {
        // if (bIsExtension)
        //     return sNameFileTotalData + XLSX;
        // else
        return sNameFileTotalData;
    }
    const string GetSNameFileAllLocalData() const
    {
        // if (bIsExtension)
        //     return sNameFileLocalData + XLSX;
        // else
        return sNameFileAllLocalData;
    }
    const string& GetSNameFileCompData() const
    {
        return sNameFileCompData;
    }

    const string& GetSNameDebugFile() const
    {
        return sNameDebugFile;
    }
    const string& GetSNameLogFile() const
    {
        return sNameLogFile;
    }

    // Int Params
private:
    int iMaxNameDiscLen;
    int iCourseLen;
    int iIgnoreEmptyLine;
    int iSoMachComp;
    int iAmountQuar;
    int iPrecision;
    int iHeaderRow;
    
    int iRowSizeGraph;
    int iShiftXGraph;
    int iShiftYGraph;
    int iShiftZGraph;
    // int iIndicatorDeep;

    // Int Params adapter
private:
    void InitIntMap();
    map<wstring, int*>
        mapIntParamsReadKey;    // Инициализируется в конструкторе
public:
    const int& GetIMaxNameDiscLen() const
    {
        return iMaxNameDiscLen;
    }
    const int& GetICourseLen() const
    {
        return iCourseLen;
    }
    const int& GetIIgnoreEmptyLine() const
    {
        return iIgnoreEmptyLine;
    }
    const int& GetISoMachComp() const
    {
        return iSoMachComp;
    }
    const int& GetIAmountQuar() const
    {
        return iAmountQuar;
    }
    const int& GetIPrecision() const
    {
        return iPrecision;
    } 
    const int& GetIHeaderRow() const
    {
        return iHeaderRow;
    }

    const int& GetIRowSizeGraph() const
    {
        return iRowSizeGraph;
    }
    const int& GetIShiftXGraph() const
    {
        return iShiftXGraph;
    }
    const int& GetIShiftYGraph() const
    {
        return iShiftYGraph;
    }
    const int& GetIShiftZGraph() const
    {
        return iShiftZGraph;
    }
    // const int& GetIIndicatorDeep() const
    //{
    //     return iIndicatorDeep;
    // }

    // Bool Params
private:
    bool bCreateFolder;
    bool bCompactOutput;
    bool bCourseOutput;
    bool bReloadLogFile;
    bool bIsOutCSVDate;
    bool bIsOutFileAllLocalData;
    bool bIsLocaleData;
    bool bIsOutCorridorCurricula;
    bool bMultiIndicator;
    bool bCompInterDelete;
    bool bIsPercentRegAll;

    bool bOutCompWithName;
    bool bOutCourseNumWithName;

    bool bOutShortNameCur;
    bool bIsUnDirected;
    bool bDelSpecCharDiscName;
    bool bIsNormalizeScoreComp;
    bool bIsNormalizeScoreDisc;
    bool bIsIgnoreTreeInd;

    bool bOutAllInfoWithoutTag;
    bool bOutEmptyComp;
    bool bOutTotalInfo;
    bool bOutDataCorridor;
    bool bOutWithoutEmptyCell;
    bool bOutCompRU;

    // Bool Params adapter
private:
    void InitBoolMap();
    map<wstring, bool*>
        mapBoolParamsReadKey;    // Инициализируется в конструкторе
public:
    const bool& GetBCreateFolder() const
    {
        return bCreateFolder;
    }
    const bool& GetBCompactOutput() const
    {
        return bCompactOutput;
    }
    const bool& GetBCourseOutput() const
    {
        return bCourseOutput;
    }
    const bool& GetBReloadLogFile() const
    {
        return bReloadLogFile;
    }
    const bool& GetBIsOutCSVDate() const
    {
        return bIsOutCSVDate;
    }
    const bool& GetBIsOutFileAllLocalData() const
    {
        return bIsOutFileAllLocalData;
    }
    const bool& GetBIsLocaleData() const
    {
        return bIsLocaleData;
    }
    const bool& GetBIsOutCorridorCurricula() const
    {
        return bIsOutCorridorCurricula;
    }
    const bool& GetBMultiIndicator() const
    {
        return bMultiIndicator;
    }
    const bool& GetBCompInterDelete() const
    {
        return bCompInterDelete;
    }
    const bool& GetBIsPercentRegAll() const
    {
        return bIsPercentRegAll;
    }
    const bool& GetBOutCompWithName() const
    {
        return bOutCompWithName;
    }

    const bool& GetBOutCourseNumWithName() const
    {
        return bOutCourseNumWithName;
    }
    const bool& GetBOutShortNameCur() const
    {
        return bOutShortNameCur;
    }
    const bool& GetBIsUnDirected() const
    {
        return bIsUnDirected;
    }
    const bool& GetBDelSpecCharDiscName() const
    {
        return bDelSpecCharDiscName;
    }
    const bool& GetBIsNormalizeScoreComp() const
    {
        return bIsNormalizeScoreComp;
    }
    const bool& GetBIsNormalizeScoreDisc() const
    {
        return bIsNormalizeScoreDisc;
    }
    const bool& GetBIsIgnoreTreeInd() const
    {
        return bIsIgnoreTreeInd;
    }
    const bool& GetBOutAllInfoWithoutTag() const
    {
        return bOutAllInfoWithoutTag;
    }
    const bool& GetBOutEmptyComp() const
    {
        return bOutEmptyComp;
    }
    const bool& GetBOutTotalInfo() const
    {
        return bOutTotalInfo;
    }

    const bool& GetBOutDataCorridor() const
    {
        return bOutDataCorridor;
    }
    const bool& GetBOutWithoutEmptyCell() const
    {
        return bOutWithoutEmptyCell;
    }
    
    const bool& GetBOutCompRU() const
    {
        return bOutCompRU;
    }

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
    const double& GetDMinWeigthRib() const
    {
        return dMinWeigthRib;
    }
    const double& GetDMinComp() const
    {
        return dMinComp;
    }
    const double& GetDAnomalBigScore() const
    {
        return dAnomalBigScore;
    }
    const double& GetDTruncAvg() const
    {
        return dTruncAvg;
    }
    const double& GetDTruncQuarPathLen() const
    {
        return dTruncQuarPathLen;
    }

    // vector<wstring> Params
private:
    vector<wstring>
        arrTypeDisc;    // Определяем вид дисциплины (основ, факульт, по выбору)
    vector<wstring> arrTypeDiscAmount;
    vector<wstring> arrTypeDiscCredits;
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
    const vector<wstring>& GetArrTypeDisc() const
    {
        return arrTypeDisc;
    }
    const vector<wstring>& GetArrTypeDiscAmount() const
    {
        return arrTypeDiscAmount;
    }
    const vector<wstring>& GetArrTypeDiscCredits() const
    {
        return arrTypeDiscCredits;
    }
    const vector<wstring>& GetArrNameFileIn() const
    {
        return arrNameFileIn;
    }
    const vector<wstring>& GetArrNameFileOut() const
    {
        return arrNameFileOut;
    }
    const vector<wstring>& GetArrTagName() const
    {
        return arrTagName;
    }

public:
    map<wstring, FTranslateData> mapArrOutParams;
    map<wstring, FTranslateData>
        mapAddOutParams;    // Вывод рекурсивного дерева

    map<wstring, wstring> mapAliasRename;

    map<wstring, set<ETagDisc>>
        mapTagDisc;    // Определяем, какая дисциплина
                       // (гуманитарна, естеств, общепроф)

    set<wstring> setIgnoreDisc;
    set<string> setIgnoreСurriculum;    // Какие УП игнорировать впринципе

    set<char> setIgnoreCharCompHeader;

    vector<vector<int>> arrRegexIndexGroup;    // Нумерование групп
    vector<int> bArrIsSolveGraphMetric;

    // Объединять ли данные УП с данными графов
    vector<int> bArrIsconcatGraphData;

private:
    void InitVectorStringMap();
    map<wstring, pair<vector<string>*, int>>
        mapVectorStringParamsReadKey;    // Инициализируется в конструкторе
                                         // (значение и размер (ограничение))
    vector<string> arrNameLabelHeader;
    vector<string> arrNameRibHeader;
    vector<string> arrRegexComp;
    vector<string> arrRegexHeaderInd;
    vector<string> arrRegexHeaderComp;
    vector<string> arrRegexCodeUGSN;

public:
    const vector<string>& GetArrNameLabelHeader() const
    {
        return arrNameLabelHeader;
    }

    const vector<string>& GetArrNameRibHeader() const
    {
        return arrNameRibHeader;
    }

    const vector<string>& GetArrRegexComp() const
    {
        return arrRegexComp;
    }

    const vector<string>& GetArrRegexHeaderInd() const
    {
        return arrRegexHeaderInd;
    }

    const vector<string>& GetArrRegexHeaderComp() const
    {
        return arrRegexHeaderComp;
    }

    const vector<string>& GetArrRegexCodeUGSN() const
    {
        return arrRegexCodeUGSN;
    }

private:
    static int iSinglControll;

    bool                 SetParams(OpenXLSX::XLWorkbook& fBook, wstring wsKey,
                                   OpenXLSX::XLRow row);
    vector<set<wstring>> SetParsingParams(OpenXLSX::XLWorksheet& fPage);

    wstring wsNamePage;    // Страница, откуда следует начать считывать
                           // параметры
    // wstring             wsFileConfig;    // Теперь хранится у Глобал
    shared_ptr<FGlobal> ptrGlobal;    // Синглтон
};