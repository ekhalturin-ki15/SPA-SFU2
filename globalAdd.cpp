#include "global.h"
#include "error.h"
#include "config.h"

void FGlobal::DeleteSpechChars(string& sData)
{
    if (ptrConfig == nullptr) return;
   
    string sNewData;
    for (const auto& it : sData)
    {
        if (ptrConfig->setIgnoreCharCompHeader.count(it)) continue;
        sNewData.push_back(it);
    }
    sData = sNewData;
    return;
    
}

void FGlobal::TakeData(bool& outBData, const OpenXLSX::XLRow& row)
{
    for (auto& it : row.cells(2,2)) // Считываем только вторую ячейку
    {
        wstring wsLine = GetValue(it);
        outBData = (wsLine.find(L"да") != wstring::npos);
    }
}

void FGlobal::TakeData(vector<wstring>& outArrData, const OpenXLSX::XLRow& row, int iSize)
{
    outArrData.clear();
    int i = -1; for (auto& it : row.cells()) // Считываем, начиная со 2 ячейки
    {
        i++;
        if (i)
            outArrData.push_back(GetValue(it));
    }
    if (iSize) if (i != (iSize - 1)) throw std::out_of_range("No equeal " + to_string(i));
}

void FGlobal::TakeData(wstring& outWsData, const OpenXLSX::XLRow& row)
{
    for (auto& it : row.cells(2, 2)) // Считываем только вторую ячейку
        outWsData = GetValue(it); // Так как нет value().get<wstring>
}

void FGlobal::TakeData(string& outWsData, const OpenXLSX::XLRow& row)
{
    for (auto& it : row.cells(2, 2)) // Считываем только вторую ячейку
        outWsData = it.value().get<string>();
}

void FGlobal::TakeData(int& outIData, const OpenXLSX::XLRow& row)
{
    for (auto& it : row.cells(2, 2)) // Считываем только вторую ячейку
        outIData = it.value().get<int>();       
}

void FGlobal::TakeData(double& outIData, const OpenXLSX::XLRow& row)
{
    for (auto& it : row.cells(2, 2)) // Считываем только вторую ячейку
        outIData = it.value().get<double>();
}

vector<pair<wstring, vector<wstring> >> FGlobal::SetMapParams(const OpenXLSX::XLWorksheet& fPage, int iSize)
{
    vector<pair<wstring, vector<wstring>>> arrReturn;
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
                    wsKeyName = GetValue(column);
                    arrReturn.push_back({ wsKeyName, vector< wstring >() });
                }
                else
                {
                    wsRename = GetValue(column);
                    arrReturn.back().second.push_back( wsRename );
                }
            }
            if (iSize) if (i != (iSize - 1)) throw std::out_of_range("No equeal " + to_string(i) + "!+");
        }
    }

    return arrReturn;
}

int FGlobal::HeightPage(const OpenXLSX::XLWorksheet& fSheet)
{
    if (ptrConfig == nullptr)
    {
        throw std::logic_error(FError::sNotInitConfig);
    }

    int h = 0;
    vector<int> arrEmptyRow;

    for (auto row : fSheet.rows())
    {
        bool bIsEmpty = true;
        for (auto it : row.cells())
        {
            wstring wsData = GetValue(it);
            if (wsData != L"")
            {
                bIsEmpty = false;
                break;
            }
        }

        if (bIsEmpty)
        {
            arrEmptyRow.push_back(h);
            if (arrEmptyRow.size() >= ptrConfig->iIgnoreEmptyLine)
            {
                if (arrEmptyRow[arrEmptyRow.size() - ptrConfig->iIgnoreEmptyLine]
                    ==
                    (h - ptrConfig->iIgnoreEmptyLine + 1)) // Нашли iIgnoreEmptyLine подряд идущих пустых строк
                {
                    return h - ptrConfig->iIgnoreEmptyLine + 1;
                }
            }
        }

        ++h;
    }

    while (arrEmptyRow.size() > 0)
    {
        if (arrEmptyRow.back() == h - 1)
        {
            --h;
            arrEmptyRow.pop_back();
        }
        else
        {
            break;
        }
    }

    return h;
}