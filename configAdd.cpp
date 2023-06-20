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


vector<set<wstring>> FConfig::SetParsingParams(OpenXLSX::XLWorksheet& fPage, const wstring& wsNamePage, const int& iNumPage)
{
    vector<set<wstring>> arrResult(fPage.rows().begin()->cells().size());

    int y = 0;
    for (auto row : fPage.rows())
    {
        if (y++) //Игнорируем первую строку, в ней описание столбцов
        {
            int i = 0;
            for (auto& column : row.cells())
            {
                wstring wsData = ptrGlobal->GetValue(column);
                if (wsData != L"")
                {
                    arrResult[i].insert(wsData);
                }
                ++i;
            }
        }
    }


    return arrResult;
}

vector<pair<wstring, wstring>> FConfig::SetMapParams(const OpenXLSX::XLWorksheet& fPage)
{
    vector<pair<wstring, wstring>> arrReturn;
    int y = -1;
    for (auto row : fPage.rows())
    {
        y++;
        if (y) //Игнорируем первую строку, в ней описание столбцов
        {
            wstring wsKeyName;
            wstring wsRename;

            int i = -1;
            for (auto& column : row.cells())
            {
                ++i;
                if (i == 0)
                {
                    wsKeyName = ptrGlobal->GetValue(column);
                }
                else
                {
                    wsRename = ptrGlobal->GetValue(column);
                    arrReturn.push_back({ wsKeyName , wsRename });
                }
            }
        }
    }

    return arrReturn;
}