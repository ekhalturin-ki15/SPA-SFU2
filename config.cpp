#include "config.h"

FConfig::FConfig(FGlobal* _fGlobal) : fGlobal(_fGlobal)
{
}


FConfig::FConfig() : iMaxLen(15), iWeigthRib(10), bCreateFolder(false), bReloadLogFile(false)
{
    wsNameLableFile = L"Id,Label";
    wsNameRibFile = L"Source,Target,Type,Kind,Id,Label,timeset,Weight";

    arrNameKeyPage = {};
    arrNameFileIn = {L"plans/grad"};
    arrNameFileOut = {L"result/grad"};

    wsNameDebugFile = L"debugFile.txt";
    wsNameLogFile = L"logFile.txt";
}

void FConfig::Init(wstring _sNameConfig, wstring _sNamePage)
{
    OpenXLSX::XLDocument fDoc;
    OpenXLSX::XLWorkbook fBook;

    fDoc.open(fGlobal->ConwertToString(_sNameConfig));
    fBook = fDoc.workbook();
    auto arrNamePage = fBook.worksheetNames();

    auto fMainPage = fBook.worksheet(fGlobal->ConwertToString(_sNamePage));
    for (auto& row : fMainPage.rows())
    {
        auto it = row.cells().begin();
        wstring sDescript = fGlobal->GetValue(*it);

        SetParams(sDescript, row);
    }

    fDoc.close();
}

void FConfig::SetParams(wstring wsKey, OpenXLSX::XLRow row)
{
    if (wsKey == L"") return;
    wstring wsPatern;

    // Названия анализируемых страниц
    wsPatern = L"Названия анализируемых страниц";
    if (wsKey == wsPatern)
    {
        arrNameKeyPage.clear();
        int i = 0; for (auto& it : row.cells())
        {
            if (i) arrNameKeyPage.push_back(fGlobal->GetValue(it));
            ++i;
        }
        return;
    }

    //Каталог данных УП
    wsPatern = L"Каталог данных УП";
    if (wsKey == wsPatern)
    {
        arrNameFileIn.clear();
        int i = 0; for (auto& it : row.cells())
        {
            if (i) arrNameFileIn.push_back(fGlobal->GetValue(it));
            ++i;
        }
        return;
    }

    //Каталог вывода результата
    wsPatern = L"Каталог вывода результата";
    if (wsKey == wsPatern)
    {
        arrNameFileOut.clear();
        int i = 0; for (auto& it : row.cells())
        {
            if (i) arrNameFileOut.push_back(fGlobal->GetValue(it));
            ++i;
        }
        return;
    }

    wsPatern = L"Заголовок файла с названиями вершин";
    if (wsKey == wsPatern)
    {
        int i = 0; for (auto& it : row.cells())
        {
            if (i)
            {
                wsNameLableFile = fGlobal->GetValue(it);
                return;
            }
            ++i;
        }
        
    }

    wsPatern = L"Заголовок файла с рёбрами";
    if (wsKey == wsPatern)
    {
        int i = 0; for (auto& it : row.cells())
        {
            if (i)
            {
                wsNameRibFile = fGlobal->GetValue(it);
                return;
            }
            ++i;
        }
    }

    wsPatern = L"Макс длина названия дисциплин";
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

    wsPatern = L"Размер рёбер";
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

    wsPatern = L"Название файла отладки";
    if (wsKey == wsPatern)
    {
        int i = 0; for (auto& it : row.cells())
        {
            if (i)
            {
                wsNameDebugFile = fGlobal->GetValue(it);
                return;
            }
            ++i;
        }
    }

    wsPatern = L"Название лог файла (для пользователя)";
    if (wsKey == wsPatern)
    {
        int i = 0; for (auto& it : row.cells())
        {
            if (i)
            {
                wsNameLogFile = fGlobal->GetValue(it);
                return;
            }
            ++i;
        }
    }

    wsPatern = L"Создать новый каталог";
    if (wsKey == wsPatern)
    {
        int i = 0; for (auto& it : row.cells())
        {
            if (i)
            {
                wstring wsLine = fGlobal->GetValue(it);
                bCreateFolder = (wsLine.find(L"да") != wstring::npos);
                return;
            }
            ++i;
        }
    }

    wsPatern = L"Перезаписывать лог файл";
    if (wsKey == wsPatern)
    {
        int i = 0; for (auto& it : row.cells())
        {
            if (i)
            {
                wstring wsLine = fGlobal->GetValue(it);
                bReloadLogFile = (wsLine.find(L"да") != wstring::npos);
                return;
            }
            ++i;
        }
    }
}