#include "config.h"

FConfig::FConfig() : iMaxLen(0)
{

}

void FConfig::Init(string _sNameConfig, string _sNamePage)
{
    OpenXLSX::XLDocument fDoc;
    OpenXLSX::XLWorkbook fBook;

    fDoc.open(_sNameConfig);
    fBook = fDoc.workbook();
    auto arrNamePage = fBook.worksheetNames();

    auto fMainPage = fBook.worksheet(_sNamePage);
    for (auto& row : fMainPage.rows())
    {
        auto it = row.cells().begin();
        wstring sDescript = fGlobal.GetValue(*it);

        SetParams(sDescript, row);
    }

    fDoc.close();
}

void FConfig::SetParams(wstring wsKey, OpenXLSX::XLRow row)
{
    if (wsKey == L"") return;
    wstring wsPatern;

    // �������� ������������� �������
    wsPatern = L"�������� ������������� �������";
    if (wsKey == wsPatern)
    {
        int i = 0; for (auto& it : row.cells())
        {
            if (i) arrNameKeyPage.push_back(fGlobal.GetValue(it));
            ++i;
        }
        return;
    }

    //������� ������ ��
    wsPatern = L"������� ������ ��";
    if (wsKey == wsPatern)
    {
        int i = 0; for (auto& it : row.cells())
        {
            if (i) arrNameFileIn.push_back(fGlobal.GetValue(it));
            ++i;
        }
        return;
    }

    //������� ������ ����������
    wsPatern = L"������� ������ ����������";
    if (wsKey == wsPatern)
    {
        int i = 0; for (auto& it : row.cells())
        {
            if (i) arrNameFileOut.push_back(fGlobal.GetValue(it));
            ++i;
        }
        return;
    }

    wsPatern = L"��������� ����� � ���������� ������";
    if (wsKey == wsPatern)
    {
        int i = 0; for (auto& it : row.cells())
        {
            if (i)
            {
                wsNameLableFile = fGlobal.GetValue(it);
                return;
            }
            ++i;
        }
        
    }

    wsPatern = L"��������� ����� � ������";
    if (wsKey == wsPatern)
    {
        int i = 0; for (auto& it : row.cells())
        {
            if (i)
            {
                wsNameRibFile = fGlobal.GetValue(it);
                return;
            }
            ++i;
        }
    }

    wsPatern = L"���� ����� �������� ���������";
    if (wsKey == wsPatern)
    {
        int i = 0; for (auto& it : row.cells())
        {
            if (i)
            {
                iMaxLen = it.value().get<int>();
                return;
            }
            ++i;
        }
    }

}