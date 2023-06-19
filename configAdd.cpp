#include "config.h"

void FConfig::TakeData(bool& outBData, OpenXLSX::XLRow& row)
{
    int i = -1; for (auto& it : row.cells())
    {
        i++;
        if (i)
        {
            wstring wsLine = ptrGlobal->GetValue(it);
            outBData = (wsLine.find(L"да") != wstring::npos);
            return;
        }
    }
}

void FConfig::TakeData(vector<wstring>& outArrData, OpenXLSX::XLRow& row)
{
    outArrData.clear();
    int i = -1; for (auto& it : row.cells())
    {
        i++;
        if (i)
            outArrData.push_back(ptrGlobal->GetValue(it));
    }
}

void FConfig::TakeData(wstring& outWsData, OpenXLSX::XLRow& row)
{
    int i = -1; for (auto& it : row.cells())
    {
        i++;
        if (i)
        {
            outWsData = ptrGlobal->GetValue(it);
            return;
        }
    }
}

void FConfig::TakeData(int& outIData, OpenXLSX::XLRow& row)
{
    int i = -1; for (auto& it : row.cells())
    {
        i++;
        if (i)
        {
            outIData = it.value().get<int>();
            return;
        }
    }
}