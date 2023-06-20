#include "config.h"


FConfig::FConfig(FGlobal* _ptrGlobal) : ptrGlobal(_ptrGlobal)
,iCourseLen(2), iMaxLen(15), iIgnoreEmptyLine(5) ,iWeigthRib(10)
,bCreateFolder(false), bCompactOutput(false), bReloadLogFile(false), bMultiIndicator(false)
,wsNameConfig(L"./config.xlsx"), wsNamePage(L"Параметры")
, wsNameLableFile(L"Id,Label"), wsNameRibFile(L"Source,Target,Type,Kind,Id,Label,timeset,Weight")
, arrNameFileIn({ L"plans/grad" }), arrNameFileOut({ L"result/grad" })
, wsNameDebugFile(L"debugFile.txt"), wsNameLogFile(L"logFile.txt")
, wsRegexComp(L" {0,1}(.{0,}?);"), wsFormula(L"((L + R) / 2) * K")
{
}

void FConfig::Init()
{

    OpenXLSX::XLDocument fDoc;
    OpenXLSX::XLWorkbook fBook;

    fDoc.open(ptrGlobal->ConwertToString(wsNameConfig));
    fBook = fDoc.workbook();
    auto arrNamePage = fBook.worksheetNames();

    auto fMainPage = fBook.worksheet(ptrGlobal->ConwertToString(wsNamePage));
    for (auto& row : fMainPage.rows())
    {
        auto it = row.cells().begin();
        wstring sDescript = ptrGlobal->GetValue(*it);

        SetParams(fBook, sDescript, row);
    }

    fDoc.close();
}

void FConfig::SetParams(OpenXLSX::XLWorkbook& fBook, wstring wsKey, OpenXLSX::XLRow row)
{
    if (wsKey == L"") return;
    wstring wsPatern;

    // Названия анализируемых страниц
    wsPatern = L"Названия анализируемых страниц";
    //Не обобщить, сложная логика
    if (wsKey == wsPatern)
    {
        arrKeyPage.clear();
        int i = -1; for (auto& page : row.cells())
        {
            i++;
            if (i)
            {
                wstring wsNamePage = ptrGlobal->GetValue(page);

                OpenXLSX::XLWorksheet fExtraPage = fBook.worksheet(ptrGlobal->ConwertToString(wsNamePage));
               
                arrKeyPage.push_back({ wsNamePage, SetParsingParams(fExtraPage, wsNamePage, i) });
            }
        }
        return;
    }

    //Каталог данных УП
    wsPatern = L"Каталог данных УП";
    if (wsKey == wsPatern)
    {
        TakeData(arrNameFileIn, row);
        return;
    }

    //Каталог вывода результата
    wsPatern = L"Каталог вывода результата";
    if (wsKey == wsPatern)
    {
        TakeData(arrNameFileOut, row);
        return;
    }

    wsPatern = L"Заголовок файла с названиями вершин";
    if (wsKey == wsPatern)
    {
        TakeData(wsNameLableFile, row);
        return;        
    }

    wsPatern = L"Заголовок файла с рёбрами";
    if (wsKey == wsPatern)
    {
        TakeData(wsNameRibFile, row);
        return;
    }

    wsPatern = L"Макс длина названия дисциплин";
    if (wsKey == wsPatern)
    {
        TakeData(iMaxLen, row);
        return;
    }

    wsPatern = L"Сколько семестров в одном курсе";
    if (wsKey == wsPatern)
    {
        TakeData(iCourseLen, row);
        return;
    }

    wsPatern = L"Размер рёбер";
    if (wsKey == wsPatern)
    {
        TakeData(iWeigthRib, row);
        return;
    }

    wsPatern = L"Название файла отладки";
    if (wsKey == wsPatern)
    {
        TakeData(wsNameDebugFile, row);
        return;
    }

    wsPatern = L"Название лог файла (для пользователя)";
    if (wsKey == wsPatern)
    {
        TakeData(wsNameLogFile, row);
        return;
    }

    wsPatern = L"Создать новый каталог";
    if (wsKey == wsPatern)
    {
        TakeData(bCreateFolder, row);
        return;
    }

    wsPatern = L"Компактный вывод результата";
    if (wsKey == wsPatern)
    {
        TakeData(bCompactOutput, row);
        return;
    }

    wsPatern = L"Перезаписывать лог файл";
    if (wsKey == wsPatern)
    {
        TakeData(bReloadLogFile, row);
        return;
    }

    wsPatern = L"Использовать многоуровневые индикаторы";
    if (wsKey == wsPatern)
    {
        TakeData(bMultiIndicator, row);
        return;
    }


    wsPatern = L"Регулярное выражения разбивки строки ([Компетенции(2)] Формируемые компетенции)";
    if (wsKey == wsPatern)
    {
        TakeData(wsRegexComp, row);
        return;
    }

    wsPatern = L"Формула расчёта весов рёбер";
    if (wsKey == wsPatern)
    {
        TakeData(wsFormula, row);
        return;
    }

    wsPatern = L"Псевдонимы (название страницы)";
    if (wsKey == wsPatern)
    {
        fAlias.mapRename.clear();
        int i = -1; for (auto& page : row.cells())
        {
            i++;
            if (i)
            {
                wstring wsNamePage = ptrGlobal->GetValue(page);
                for (auto& [key, val] : 
                    SetMapParams(fBook.worksheet(ptrGlobal->ConwertToString(wsNamePage)))
                    )
                {
                    fAlias.mapRename[key] = val;
                }
            }
        }
        return;
    }

    wsPatern = L"Параметры выводы (название страницы)";
    if (wsKey == wsPatern)
    {
        mapOutParams.clear();
        int i = -1; for (auto& page : row.cells())
        {
            i++;
            if (i)
            {
                wstring wsNamePage = ptrGlobal->GetValue(page);
                for (auto& [key, val] :
                    SetMapParams(fBook.worksheet(ptrGlobal->ConwertToString(wsNamePage)))
                    )
                {
                    mapOutParams[key] = val;
                }
            }
        }
        return;
        
    }

}
