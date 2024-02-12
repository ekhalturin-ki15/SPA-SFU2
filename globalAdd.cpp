#include "config.h"
#include "error.h"
#include "global.h"

void FGlobal::DeleteSpechChars(string& sData) const
{
    if (ptrConfig == nullptr)
        return;

    string sNewData;
    for (const auto& it : sData)
    {
        if (ptrConfig->setIgnoreCharCompHeader.count(it))
            continue;
        sNewData.push_back(it);
    }
    sData = sNewData;
    return;
}

string FGlobal::DoubletWithPrecision(const double& dNum) const
{
    std::ostringstream fOut;
    fOut << std::setprecision(ptrConfig->GetIPrecision()) << std::fixed << dNum;

    string buf = fOut.str();
    if (buf.find('.') != string::npos)
    {
        while (buf.back() == '0')
            buf.pop_back();
    }
    if (buf.back() == '.')
        buf.pop_back();

    return buf;
}

filesystem::path FGlobal::GetCurrentPath() const
{
#ifdef UNIT_TEST
    filesystem::path fFile =
        filesystem::current_path().parent_path().parent_path();
    fFile /= "UnitTest";
    return fFile;
#else
    return filesystem::current_path();
#endif
}

bool FGlobal::IsThatIsTrue(wstring wsData)
{
    return IsThatIsTrue(ConwertToString(wsData));
}

bool FGlobal::IsThatIsTrue(string sData) const
{
    string sRet = ReversUTF16RU(sData); //На случай, если криво сопоставлено

    for (auto& it : { sData, sRet })
    {
        if (it.find("+") != string::npos)
            return true;
        if (it.find("Да") != string::npos)
            return true;
        if (it.find("да") != string::npos)
            return true;
        if (it.find("Yes") != string::npos)
            return true;
        if (it.find("yes") != string::npos)
            return true;
    }
    return false;
}

void FGlobal::TakeData(bool& outBData, const OpenXLSX::XLRow& row)
{
    for (auto& it : row.cells(2, 2))    // Считываем только вторую ячейку
    {
        wstring wsLine = GetValue(it);
        outBData       = (wsLine.find(L"да") != wstring::npos);
    }
}

void FGlobal::TakeData(vector<wstring>&       outArrData,
                       const OpenXLSX::XLRow& row,
                       int                    iSize)
{
    outArrData.clear();
    int i = -1;
    for (auto& it : row.cells())    // Считываем, начиная со 2 ячейки
    {
        i++;
        if (i)
            outArrData.push_back(GetValue(it));
    }
    if (iSize)
        if (i != (iSize - 1))
            throw std::out_of_range("No equeal " + to_string(i));
}

void FGlobal::TakeData(vector<string>&        outArrData,
                       const OpenXLSX::XLRow& row,
                       int                    iSize)
{
    outArrData.clear();
    int i = -1;
    for (auto& it : row.cells())    // Считываем, начиная со 2 ячейки
    {
        i++;
        if (i)
            outArrData.push_back(it.value().get<string>());
    }
    if (iSize)
        if (i != (iSize - 1))
            throw std::out_of_range("No equeal " + to_string(i));
}

void FGlobal::TakeData(wstring& outWsData, const OpenXLSX::XLRow& row)
{
    for (auto& it : row.cells(2, 2))    // Считываем только вторую ячейку
        outWsData = GetValue(it);    // Так как нет value().get<wstring>
}

void FGlobal::TakeData(string& outWsData, const OpenXLSX::XLRow& row)
{
    for (auto& it : row.cells(2, 2))    // Считываем только вторую ячейку
        outWsData = it.value().get<string>();
}

void FGlobal::TakeData(int& outIData, const OpenXLSX::XLRow& row)
{
    for (auto& it : row.cells(2, 2))    // Считываем только вторую ячейку
    {
        try
        {
            outIData = it.value().get<int>();
        }
        catch (...)
        {
            outIData = it.value().get<double>();
        }
    }
}

void FGlobal::TakeData(double& outIData, const OpenXLSX::XLRow& row)
{
    for (auto& it : row.cells(2, 2))    // Считываем только вторую ячейку
    {
        try
        {
            outIData = it.value().get<double>();
        }
        catch (...)
        {
            outIData = it.value().get<int>();
        }
    }
}

vector<pair<wstring, vector<wstring>>>
    FGlobal::SetMapParams(const OpenXLSX::XLWorksheet& fPage,
                          set<int>
                              setUsedSize)
{
    vector<pair<wstring, vector<wstring>>> arrReturn;
    int                                    y = -1;
    for (auto row : fPage.rows())
    {
        y++;
        if (y)    // Игнорируем первую строку, в ней описание столбцов
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
                    arrReturn.push_back({ wsKeyName, vector<wstring>() });
                }
                else
                {
                    wsRename = GetValue(column);
                    arrReturn.back().second.push_back(wsRename);
                }
            }

            if (!setUsedSize.count(i + 1))
                throw std::out_of_range("No equeal " + to_string(i) + "!+");
        }
    }

    return arrReturn;
}

vector<pair<wstring, vector<wstring>>>
    FGlobal::SetMapParams(const OpenXLSX::XLWorksheet& fPage, int iSize)
{
    vector<pair<wstring, vector<wstring>>> arrReturn;
    int                                    y = -1;
    for (auto row : fPage.rows())
    {
        y++;
        if (y)    // Игнорируем первую строку, в ней описание столбцов
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
                    arrReturn.push_back({ wsKeyName, vector<wstring>() });
                }
                else
                {
                    wsRename = GetValue(column);
                    arrReturn.back().second.push_back(wsRename);
                }
            }
            if (iSize)
                if (i != (iSize - 1))
                    throw std::out_of_range("No equeal " + to_string(i) + "!+");
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

    int         h = 0;
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
            if (arrEmptyRow.size() >= ptrConfig->GetIIgnoreEmptyLine())
            {
                if (arrEmptyRow[arrEmptyRow.size() -
                                ptrConfig->GetIIgnoreEmptyLine()] ==
                    (h - ptrConfig->GetIIgnoreEmptyLine() +
                     1))    // Нашли iIgnoreEmptyLine подряд идущих пустых строк
                {
                    return h - ptrConfig->GetIIgnoreEmptyLine() + 1;
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