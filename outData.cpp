#include "outData.h"
#include "adapOutData.h"
#include "config.h"
#include "error.h"
#include "global.h"
#include "graph.h"
#include "metric.h"
#include "solve.h"

#include <iomanip>
#include <sstream>
int FOutData::iSinglControll = 0;

FOutData::FOutData(shared_ptr<FGlobal> _ptrGlobal) : ptrGlobal(_ptrGlobal)
{
    // Unit test против такого
#ifndef UNIT_TEST
    if (iSinglControll > 0)
        throw std::runtime_error("Re-creation Singleton");
#endif
    ++iSinglControll;
}

void FOutData::Create(string _sOutPath)
{
    sOutPath = _sOutPath;
    OutTotalData();
}

// FAdapOutData
// FDataType fTotalData;
void FOutData::OutTotalData()
{
    OpenXLSX::XLDocument fOutFile;
    const string sPageName = ptrGlobal->ptrConfig->GetSNameFileTotalData(false);
    const string sCreatePath =
        sOutPath + "/" + ptrGlobal->ptrConfig->GetSNameFileTotalData(true);
    fOutFile.create(sCreatePath);
    fOutFile.workbook().addWorksheet(sPageName);
    fOutFile.workbook().deleteSheet("Sheet1");
    OpenXLSX::XLWorksheet wks = fOutFile.workbook().worksheet(sPageName);

    int                    iShiftX = 1;
    vector<vector<string>> arrAllCurriculaTotalData;
    OutTable(0, 0, ptrGlobal->ptrAdapOutData->fTotalData, wks);
}

void FOutData::OutTable(const int& iShiftX, const int& iShiftY, FDataType fData,
                        OpenXLSX::XLWorksheet& WKS)
{
    if ((iShiftX < 0) || (iShiftY < 0))
    {
        this->ptrGlobal->ptrError->ErrorBadShiftTable();
        return;
    }

    const auto& arrHeader = fData.arrHeader;
    for (int x = 0; x < arrHeader.size(); ++x)
    {
        OutDataCeil(iShiftY, x + iShiftX, WKS, arrHeader[x]);
        // WKS.cell(y + iShiftY, x + iShiftX).value() = arrData[y][x].fData;
    }

    const auto& arrData = fData.arrData;
    for (int y =  1; y < arrData.size(); ++y)
    {
        for (int x = 0; x < arrData[y].size(); ++x)
        {
            OutDataCeil(y + iShiftY, x + iShiftX, WKS, arrData[y][x]);
            // WKS.cell(y + iShiftY, x + iShiftX).value() = arrData[y][x].fData;
        }
    }
}

void FOutData::OutDataCeil(const int& x, const int& y,
                           OpenXLSX::XLWorksheet& WKS, const string& sData)
{
    WKS.cell(y, x).value() = sData;
}

void FOutData::OutDataCeil(const int& x, const int& y,
                           OpenXLSX::XLWorksheet& WKS, const FTableData& fData)
{
    string sType;
    if (fData.fData.has_value())
        sType = fData.fData.type().name();

    bool bIsCheck = false;

    if (sType == typeid(string).name())
    {
        bIsCheck               = true;
        WKS.cell(y, x).value() = std::any_cast<string>(fData.fData);
    }
    if (sType == typeid(int).name())
    {
        bIsCheck               = true;
        WKS.cell(y, x).value() = std::any_cast<int>(fData.fData);
    }
    if (sType == typeid(double).name())
    {
        bIsCheck               = true;
        WKS.cell(y, x).value() = std::any_cast<double>(fData.fData);
    }
    if (sType == typeid(char).name())
    {
        bIsCheck               = true;
        WKS.cell(y, x).value() = std::any_cast<char>(fData.fData);
    }

    if (!bIsCheck)
    {
        WKS.cell(y, x).value() = ptrGlobal->ptrConfig->GetSNoInitData();
    }
}

bool FOutData::Init()
{
    return true;
}

FOutData::~FOutData()
{
    --iSinglControll;
}