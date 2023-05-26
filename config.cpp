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

    wsRegexComp = L" {0,1}(.{0,}?);";
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

    // Названия анализируемых страниц
    wsPatern = L"Названия анализируемых страниц";
    if (wsKey == wsPatern)
    {
        arrKeyPage.clear();
        int i = 0; for (auto& page : row.cells())
        {
            if (i)
            {
                //arrKeyPage.resize(arrKeyPage.size() + 1);

                wstring wsNamePage = ptrGlobal->GetValue(page);

                auto fExtraPage = FBook.worksheet(ptrGlobal->ConwertToString(wsNamePage));

                vector<set<wstring>> arrReadHeader(fExtraPage.rows().begin()->cells().size());

                for (auto row : fExtraPage.rows())
                {
                    int i = 0;
                    for (auto& column : row.cells())
                    {
                        wstring wsData = ptrGlobal->GetValue(column);
                        if (wsData != L"")
                        {
                            arrReadHeader[i].insert(wsData);
                        }
                        ++i;
                    }
                }
                arrKeyPage.push_back({ wsNamePage, arrReadHeader });
            }
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
            if (i) arrNameFileIn.push_back(ptrGlobal->GetValue(it));
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
            if (i) arrNameFileOut.push_back(ptrGlobal->GetValue(it));
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
                wsNameLableFile = ptrGlobal->GetValue(it);
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
                wsNameRibFile = ptrGlobal->GetValue(it);
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
                wsNameDebugFile = ptrGlobal->GetValue(it);
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
                wsNameLogFile = ptrGlobal->GetValue(it);
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
                wstring wsLine = ptrGlobal->GetValue(it);
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
                wstring wsLine = ptrGlobal->GetValue(it);
                bReloadLogFile = (wsLine.find(L"да") != wstring::npos);
                return;
            }
            ++i;
        }
    }

    wsPatern = L"Использовать многоуровневые индикаторы";
    if (wsKey == wsPatern)
    {
        int i = 0; for (auto& it : row.cells())
        {
            if (i)
            {
                wstring wsLine = ptrGlobal->GetValue(it);
                bMultiIndicator = (wsLine.find(L"да") != wstring::npos);
                return;
            }
            ++i;
        }
    }


    wsPatern = L"Регулярное выражения разбивки строки ([Компетенции(2)] Формируемые компетенции)";
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