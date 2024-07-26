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
    OutLocalData();
    OutLocalGephiCSVData();
}

// FAdapOutData
// vector<map<ETypeGraph, pair<FDataType, FDataType>>> arrMapGephiCSVData;
void FOutData::OutLocalGephiCSVData()
{
    for (int iCur = 0; iCur < ptrGlobal->ptrSolve->N; ++iCur)
    {
        const auto& it = ptrGlobal->ptrSolve->GetCurricula(iCur);
        // Выводить короткое, или помное имя
        string sCurPlanName = (ptrGlobal->ptrConfig->GetBOutShortNameCur())
                                  ? it->sShortNamePlan
                                  : it->sNamePlan;

        string sNewFile = sOutPath + "/" + sCurPlanName;

        if (!filesystem::exists(sNewFile))
        {
            try
            {
                filesystem::create_directories(sNewFile);
            }
            catch (...)
            {
                ptrGlobal->ptrError->ErrorOutFileCreate(sNewFile);
                continue;    // Продолжаем работать, но с другим
                // файлом
            }
        }

        for (const auto& [eType, fPairData] :
             ptrGlobal->ptrAdapOutData->arrMapGephiCSVData[iCur])
        {
            string sLableGephiPath   = ptrGlobal->ptrConfig->GetSNoInitData();
            string sRibGephiPath   = ptrGlobal->ptrConfig->GetSNoInitData();
            {
                string sGephiPath = sCurPlanName;
                sGephiPath += ptrGlobal->ptrConfig->GetSSeparator();
                if (eType.Get() >= 0)    // Значит, это данные о курсе
                {
                    sGephiPath += ptrGlobal->ptrConfig->GetSCurrentCourse();
                    sGephiPath += ptrGlobal->ptrConfig->GetSSeparator();
                    sGephiPath += to_string(eType.Get() + 1);
                }
                else
                {
                    sGephiPath += eType.Name();
                }

                sGephiPath = sNewFile + "/" + sGephiPath +
                               ptrGlobal->ptrConfig->GetSSeparator();


                sLableGephiPath =
                    sGephiPath +
                    ptrGlobal->ptrConfig->GetSSufLableGephiFile();
                
                sRibGephiPath =
                    sGephiPath +
                    ptrGlobal->ptrConfig->GetSSufRibGephiFile();
            }

            const auto& [fLableData, fRibData] = fPairData;

            //Вывод вершин
            OutTable(0, 0, true, fLableData, false, nullptr, sLableGephiPath + CSV);

            //Вывод рёбер
            OutTable(0, 0, true, fRibData, false, nullptr, sRibGephiPath + CSV);
        }
    }
}

// FAdapOutData
// vector<map<string, FDataType>> arrMapCompTreeData;
void FOutData::OutLocalData()
{
    // Чтобы пересоздавать через синглтон (
    vector<OpenXLSX::XLDocument> arrOutFile;

    for (int iCur = 0; iCur < ptrGlobal->ptrSolve->N; ++iCur)
    {
        const auto& it = ptrGlobal->ptrSolve->GetCurricula(iCur);
        // Выводить короткое, или помное имя
        string sCurPlanName = (ptrGlobal->ptrConfig->GetBOutShortNameCur())
                                  ? it->sShortNamePlan
                                  : it->sNamePlan;

        string sNewFile = sOutPath + "/" + sCurPlanName;

        if (!filesystem::exists(sNewFile))
        {
            try
            {
                filesystem::create_directories(sNewFile);
            }
            catch (...)
            {
                ptrGlobal->ptrError->ErrorOutFileCreate(sNewFile);
                continue;    // Продолжаем работать, но с другим
                // файлом
            }
        }

        for (const auto& [sName, fData] :
             ptrGlobal->ptrAdapOutData->arrMapCompTreeData[iCur])
        {
            const string sCreatePath =
                sNewFile + "/" + ptrGlobal->ptrConfig->GetSNameFileCompData() +
                ptrGlobal->ptrConfig->GetSSeparator() + sName;

            const string sPageName = sCurPlanName;

            if (arrOutFile.size() == 1)
            {
                arrOutFile.back().save();
                arrOutFile.back().close();
                arrOutFile.clear();
            }

            arrOutFile.resize(1);

            OpenXLSX::XLDocument& fLocalOutFile = arrOutFile.back();

            fLocalOutFile.create(sCreatePath + XLSX);
            fLocalOutFile.workbook().addWorksheet(sPageName);
            fLocalOutFile.workbook().deleteSheet(sDefaultName);
            OpenXLSX::XLWorksheet wksLocal =
                fLocalOutFile.workbook().worksheet(sPageName);

            OutTable(0, 0, true, fData, false, &wksLocal,
                     (ptrGlobal->ptrConfig->GetBIsOutCSVDate()
                          ? sCreatePath + CSV
                          : ""));
        }
    }

    arrOutFile.back().save();
    arrOutFile.back().close();
}

// FAdapOutData
// FDataType fTotalData;
// map<ETypeGraph, FDataType> mapGraphData;
void FOutData::OutTotalData()
{
    // Чтобы пересоздавать через синглтон (
    vector<OpenXLSX::XLDocument> arrOutFile(1);

    OpenXLSX::XLDocument& fTotalOutFile = arrOutFile.back();
    // arrOutFile.push_back(&fTotalOutFile);
    {
        const string sCreatePath =
            sOutPath + "/" + ptrGlobal->ptrConfig->GetSNameFileTotalData();

        const string sPageName = ptrGlobal->ptrConfig->GetSNameFileTotalData();

        fTotalOutFile.create(sCreatePath + XLSX);
        fTotalOutFile.workbook().addWorksheet(sPageName);
        fTotalOutFile.workbook().deleteSheet(sDefaultName);
        OpenXLSX::XLWorksheet wks =
            fTotalOutFile.workbook().worksheet(sPageName);

        OutTable(0, 0, true, ptrGlobal->ptrAdapOutData->fTotalData,
                 ptrGlobal->ptrConfig->GetBOutDataCorridor(), &wks,
            (ptrGlobal->ptrConfig->GetBIsOutCSVDate() ? sCreatePath+ CSV
                                                           : ""));
    }

    for (const auto& [eType, fData] : ptrGlobal->ptrAdapOutData->mapGraphData)
    {
        OpenXLSX::XLWorksheet wksCourse;

        string sGraphPageName = ptrGlobal->ptrConfig->GetSNoInitData();
        if (eType.Get() >= 0)    // Значит, это данные о курсе
        {
            sGraphPageName = ptrGlobal->ptrConfig->GetSCurrentCourse();
            sGraphPageName += ptrGlobal->ptrConfig->GetSSeparator();
            sGraphPageName += to_string(eType.Get() + 1);
        }
        else
        {
            sGraphPageName = eType.Name();
        }

        const string sGraphCreatePath = sOutPath + "/" + sGraphPageName;

        // Выводить в этом же файле
        if (ptrGlobal->ptrConfig->GetBCompactOutput())
        {
            fTotalOutFile.workbook().addWorksheet(sGraphPageName);
            wksCourse = fTotalOutFile.workbook().worksheet(sGraphPageName);
        }
        else
        {
            if (arrOutFile.size() == 1)
            {
                arrOutFile.back().save();
                arrOutFile.back().close();
                arrOutFile.clear();
            }

            arrOutFile.resize(1);

            OpenXLSX::XLDocument& fGraphOutFile = arrOutFile.back();

            fGraphOutFile.create(sGraphCreatePath + XLSX);
            fGraphOutFile.workbook().addWorksheet(sGraphPageName);
            fGraphOutFile.workbook().deleteSheet(sDefaultName);
            wksCourse = fGraphOutFile.workbook().worksheet(sGraphPageName);
        }

        OutTable(0, 0, true, fData, ptrGlobal->ptrConfig->GetBOutDataCorridor(),
                 &wksCourse,
                 (ptrGlobal->ptrConfig->GetBIsOutCSVDate()
                      ? sGraphCreatePath + CSV
                      : ""));
    }

    arrOutFile.back().save();
    arrOutFile.back().close();
}

void FOutData::IncRow(int& iRow, ofstream* outDataCSVStream)
{
    if (outDataCSVStream != nullptr)    // Продублируем в виде CSV
    {
        (*outDataCSVStream) << ENDL;
    }
    ++iRow;
}

void FOutData::OutTable(const int& iYShift, const int& iXShift,
                        const bool& bIsOutHeader, FDataType fData,
                        const bool& bIsOutCorridor, OpenXLSX::XLWorksheet* WKS, 
                        const string& sNameDataCSVStream)
{
    ofstream* ptrDataCSVStream = nullptr;
    ofstream  outDataCSVStream;
    if (bIsOutHeader)
    {
        // Выводим с нуля, иначе дополняем
        ptrGlobal->ClearFile(sNameDataCSVStream);
        outDataCSVStream.open(sNameDataCSVStream, std::ios::app);
        ptrDataCSVStream = &outDataCSVStream;
    }

    const auto& arrIsOut = fData.arrIsOut;

    int iRealY = 1;

    if (bIsOutHeader)
    {
        const auto& arrHeader = fData.arrHeader;
        int         iRealX    = 1;

        for (int x = 0; x < arrHeader.size(); ++x)
        {
            if (arrIsOut[x])
            {
                if (ptrDataCSVStream != nullptr)    // Продублируем в виде CSV
                {
                    if (iRealX > 1)
                        outDataCSVStream
                            << ptrGlobal->ptrConfig->GetSCSVSeparator();

                    outDataCSVStream << arrHeader[x];
                }

                if (WKS != nullptr)
                {
                    OutDataCeil(iYShift + iRealY, iXShift + iRealX, *WKS,
                                arrHeader[x]);
                }

                ++iRealX;
            }
        }

        IncRow(iRealY, ptrDataCSVStream);
    }

    {
        const auto& arrData = fData.arrData;
        for (int y = 0; y < arrData.size(); ++y)
        {
            int iRealX = 1;
            for (int x = 0; x < arrData[y].size(); ++x)
            {
                if (arrIsOut[x])
                {
                    if (ptrDataCSVStream !=
                        nullptr)    // Продублируем в виде CSV
                    {
                        if (iRealX > 1)
                            outDataCSVStream
                                << ptrGlobal->ptrConfig->GetSCSVSeparator();

                        outDataCSVStream
                            << ConvertAnyToString(arrData[y][x]);
                    }
                    if (WKS != nullptr)
                    {
                        OutDataCeil(iYShift + iRealY, iXShift + iRealX, *WKS,
                                    arrData[y][x]);
                    }

                    ++iRealX;
                }
            }
            IncRow(iRealY, ptrDataCSVStream);
        }
    }

    if (bIsOutCorridor)
    {
        const auto& arrCorridor = fData.arrDataCorridor;

        for (int y = 0; y < arrCorridor.size(); ++y)
        {
            if (arrIsOutCorridor[y])
            {
                if (ptrDataCSVStream != nullptr)    // Продублируем в виде CSV
                {
                    outDataCSVStream << arrNameCorridor[y];
                }
                if (WKS != nullptr)
                {
                    OutDataCeil(iYShift + iRealY, iXShift + 1, *WKS,
                                arrNameCorridor[y]);
                }

                for (int x = 1; x < arrCorridor[y].size(); ++x)
                {
                    string sOutString =
                        ConvertAnyToString(arrCorridor[y][x].fData);
                    if (arrCorridor[y][x].iAddInfo != FTypeGraph::dNoInit)
                    {
                        sOutString +=
                            " (" +
                            ptrGlobal->ptrConfig->GetSCorridorCurricula();

                        auto ptrCurricula = ptrGlobal->ptrSolve->GetCurricula(
                            arrCorridor[y][x].iAddInfo);
                        sOutString += ptrCurricula->sShortNamePlan + ")";
                    }

                    if (ptrDataCSVStream !=
                        nullptr)    // Продублируем в виде CSV
                    {
                        outDataCSVStream
                            << ptrGlobal->ptrConfig->GetSCSVSeparator()
                            << sOutString;
                    }
                    if (WKS != nullptr)
                    {
                        OutDataCeil(iYShift + iRealY, iXShift + x + 1, *WKS,
                                    sOutString);
                    }
                }
            }
            IncRow(iRealY, ptrDataCSVStream);
        }
    }
}

void FOutData::OutDataCeil(const int& y, const int& x,
                           OpenXLSX::XLWorksheet& WKS, const string& sData)
{
    WKS.cell(y, x).value() = sData;
}

string FOutData::ConvertAnyToString(const any& fData)
{
    string sType;
    if (fData.has_value())
        sType = fData.type().name();

    bool   bIsCheck = false;
    string sOutData;

    if (sType == typeid(string).name())
    {
        bIsCheck = true;
        sOutData = std::any_cast<string>(fData);
    }
    if (sType == typeid(int).name())
    {
        bIsCheck = true;
        sOutData = to_string(std::any_cast<int>(fData));
    }
    if (sType == typeid(double).name())
    {
        bIsCheck = true;
        sOutData = to_string(std::any_cast<double>(fData));
    }
    if (sType == typeid(char).name())
    {
        bIsCheck = true;
        sOutData = to_string(std::any_cast<char>(fData));
    }

    if (bIsCheck)
    {
        return sOutData;
    }
    else
    {
        return ptrGlobal->ptrConfig->GetSNoInitData();
    }
}

void FOutData::OutDataCeil(const int& y, const int& x,
                           OpenXLSX::XLWorksheet& WKS, const any& fData)
{
    string sType;
    if (fData.has_value())
        sType = fData.type().name();

    bool bIsCheck = false;

    if (sType == typeid(string).name())
    {
        bIsCheck               = true;
        WKS.cell(y, x).value() = std::any_cast<string>(fData);
    }
    if (sType == typeid(int).name())
    {
        bIsCheck               = true;
        WKS.cell(y, x).value() = std::any_cast<int>(fData);
    }
    if (sType == typeid(double).name())
    {
        bIsCheck               = true;
        WKS.cell(y, x).value() = std::any_cast<double>(fData);
    }
    if (sType == typeid(char).name())
    {
        bIsCheck               = true;
        WKS.cell(y, x).value() = std::any_cast<char>(fData);
    }

    if (!bIsCheck)
    {
        WKS.cell(y, x).value() = ptrGlobal->ptrConfig->GetSNoInitData();
    }
}

bool FOutData::Init()
{
    for (const auto& wsName :
         ptrGlobal->ptrAdapOutData->arrOriginCorridorHeader)
    {
        if (ptrGlobal->ptrConfig->mapArrOutParams.count(wsName))
        {
            arrNameCorridor.push_back(ptrGlobal->ConwertToString(
                ptrGlobal->ptrConfig->mapArrOutParams[wsName].GetName()));

            arrIsOutCorridor.push_back(
                ptrGlobal->ptrConfig->mapArrOutParams[wsName].GetTotal());
        }
    }

    return true;
}

FOutData::~FOutData()
{
    --iSinglControll;
}