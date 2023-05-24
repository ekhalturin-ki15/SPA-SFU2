#include "config.h"

FConfig::FConfig(FGlobal* _ptrGlobal) : ptrGlobal(_ptrGlobal)
{
}


FConfig::FConfig() : iMaxLen(15), iWeigthRib(10), bCreateFolder(false), bReloadLogFile(false)
{
    wsNameLableFile = L"Id,Label";
    wsNameRibFile = L"Source,Target,Type,Kind,Id,Label,timeset,Weight";

    arrNameFileIn = {L"plans/grad"};
    arrNameFileOut = {L"result/grad"};

    wsNameDebugFile = L"debugFile.txt";
    wsNameLogFile = L"logFile.txt";
}

void FConfig::Init(wstring _sNameConfig, wstring _sNamePage)
{
    OpenXLSX::XLDocument fDoc;
    OpenXLSX::XLWorkbook fBook;

    fDoc.open(ptrGlobal->ConwertToString(_sNameConfig));
    fBook = fDoc.workbook();
    auto arrNamePage = fBook.worksheetNames();

    auto fMainPage = fBook.worksheet(ptrGlobal->ConwertToString(_sNamePage));
    for (auto& row : fMainPage.rows())
    {
        auto it = row.cells().begin();
        wstring sDescript = ptrGlobal->GetValue(*it);

        SetParams(fBook, sDescript, row);
    }

    fDoc.close();
}

void FConfig::SetParams(OpenXLSX::XLWorkbook& FBook, wstring wsKey, OpenXLSX::XLRow row)
{
    if (wsKey == L"") return;
    wstring wsPatern;

    // �������� ������������� �������
    wsPatern = L"�������� ������������� �������";
    if (wsKey == wsPatern)
    {
        arrKeyPage.clear();
        int i = 0; for (auto& it : row.cells())
        {
            if (i)
            {
                //arrKeyPage.resize(arrKeyPage.size() + 1);

                wstring wsNamePage = ptrGlobal->GetValue(it);

                auto fExtraPage = FBook.worksheet(ptrGlobal->ConwertToString(wsNamePage));

                vector<wstring> arrReadHeader;

                for (auto it : fExtraPage.rows().begin()->cells())
                {
                    wstring wsData = ptrGlobal->GetValue(it);
                    if (wsData != L"")
                    {
                        arrReadHeader.push_back(wsData);
                    }
                }
                arrKeyPage.push_back({ wsNamePage, arrReadHeader });
            }
            ++i;
        }
        return;
    }

    //������� ������ ��
    wsPatern = L"������� ������ ��";
    if (wsKey == wsPatern)
    {
        arrNameFileIn.clear();
        int i = 0; for (auto& it : row.cells())
        {
            if (i) arrNameFileIn.push_back(ptrGlobal->GetValue(it));
            ++i;
        }
        return;
    }

    //������� ������ ����������
    wsPatern = L"������� ������ ����������";
    if (wsKey == wsPatern)
    {
        arrNameFileOut.clear();
        int i = 0; for (auto& it : row.cells())
        {
            if (i) arrNameFileOut.push_back(ptrGlobal->GetValue(it));
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
                wsNameLableFile = ptrGlobal->GetValue(it);
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
                wsNameRibFile = ptrGlobal->GetValue(it);
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

    wsPatern = L"������ ����";
    if (wsKey == wsPatern)
    {
        int i = 0; for (auto& it : row.cells())
        {
            if (i)
            {
                iWeigthRib = it.value().get<int>();
                return;
            }
            ++i;
        }
    }

    wsPatern = L"�������� ����� �������";
    if (wsKey == wsPatern)
    {
        int i = 0; for (auto& it : row.cells())
        {
            if (i)
            {
                wsNameDebugFile = ptrGlobal->GetValue(it);
                return;
            }
            ++i;
        }
    }

    wsPatern = L"�������� ��� ����� (��� ������������)";
    if (wsKey == wsPatern)
    {
        int i = 0; for (auto& it : row.cells())
        {
            if (i)
            {
                wsNameLogFile = ptrGlobal->GetValue(it);
                return;
            }
            ++i;
        }
    }

    wsPatern = L"������� ����� �������";
    if (wsKey == wsPatern)
    {
        int i = 0; for (auto& it : row.cells())
        {
            if (i)
            {
                wstring wsLine = ptrGlobal->GetValue(it);
                bCreateFolder = (wsLine.find(L"��") != wstring::npos);
                return;
            }
            ++i;
        }
    }

    wsPatern = L"�������������� ��� ����";
    if (wsKey == wsPatern)
    {
        int i = 0; for (auto& it : row.cells())
        {
            if (i)
            {
                wstring wsLine = ptrGlobal->GetValue(it);
                bReloadLogFile = (wsLine.find(L"��") != wstring::npos);
                return;
            }
            ++i;
        }
    }

    wsPatern = L"���������� ��������� �������� ������ ([�����������(2)] ����������� �����������)";
    if (wsKey == wsPatern)
    {
        int i = 0; for (auto& it : row.cells())
        {
            if (i)
            {
                wsRegexComp = ptrGlobal->GetValue(it);
                return;
            }
            ++i;
        }
    }

}