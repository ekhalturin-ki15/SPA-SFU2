#include "config.h"
#include "error.h"

int FConfig::iSinglControll = 0;

FConfig::FConfig(FGlobal* _ptrGlobal) : ptrGlobal(_ptrGlobal)
,iCourseLen(2), iMaxLen(15), iIgnoreEmptyLine(5) ,iWeigthRib(10), dMinWeigthRib(0.01)
,bCreateFolder(true), bCompactOutput(true), bReloadLogFile(true)
, bMultiIndicator(true), bCompInterDelete(true)
,wsNameConfig(L"./config.xlsx"), wsNamePage(L"Параметры")
, wsNameLableFile(L"Id,Label"), wsNameRibFile(L"Source,Target,Type,Kind,Id,Label,timeset,Weight")
, arrNameFileIn({ L"plans/grad" }), arrNameFileOut({ L"result/grad" })
, wsNameDebugFile(L"debugFile.txt"), wsNameLogFile(L"logFile.txt")
, sRegexComp("{0, 1}(.{0, } ? ); "), sRegexHeaderComp("(.{1,})-"), sFormula("((L + R) / 2) * K")
{
    if (iSinglControll > 0)
        throw std::runtime_error("Re-creation Singleton");
    ++iSinglControll;
}

bool FConfig::Init()
{

    OpenXLSX::XLDocument fDoc;
    OpenXLSX::XLWorkbook fBook;
    try
    {
        fDoc.open(ptrGlobal->ConwertToString(wsNameConfig));
    }
    catch (...)
    {
        ptrGlobal->ptrError->ErrorNotFoundConfig();
        return false;
    }
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

    return true;
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
                string sNamePage = ptrGlobal->ConwertToString(wsNamePage);

                OpenXLSX::XLWorksheet fExtraPage = fBook.worksheet(sNamePage);
               
                arrKeyPage.push_back({ wsNamePage, sNamePage, SetParsingParams(fExtraPage) });
            }
        }
        return;
    }

    //Каталог данных УП
    wsPatern = L"Каталог данных УП";
    if (wsKey == wsPatern)
    {
        ptrGlobal->TakeData(arrNameFileIn, row);
        return;
    }

    //Каталог вывода результата
    wsPatern = L"Каталог вывода результата";
    if (wsKey == wsPatern)
    {
        ptrGlobal->TakeData(arrNameFileOut, row);
        return;
    }

    wsPatern = L"Заголовок файла с названиями вершин";
    if (wsKey == wsPatern)
    {
        ptrGlobal->TakeData(wsNameLableFile, row);
        return;        
    }

    wsPatern = L"Заголовок файла с рёбрами";
    if (wsKey == wsPatern)
    {
        ptrGlobal->TakeData(wsNameRibFile, row);
        return;
    }

    wsPatern = L"Макс длина названия дисциплин";
    if (wsKey == wsPatern)
    {
        ptrGlobal->TakeData(iMaxLen, row);
        return;
    }

    wsPatern = L"Сколько семестров в одном курсе";
    if (wsKey == wsPatern)
    {
        ptrGlobal->TakeData(iCourseLen, row);
        return;
    }

    wsPatern = L"Создавать ребро, если его вес больше X=";
    if (wsKey == wsPatern)
    {
        ptrGlobal->TakeData(dMinWeigthRib, row);
        return;
    }

    wsPatern = L"Считать компетенцию отсутсвующей, если значение меньше X=";
    if (wsKey == wsPatern)
    {
        ptrGlobal->TakeData(dMinComp, row);
        return;
    }

    wsPatern = L"Размер рёбер";
    if (wsKey == wsPatern)
    {
        ptrGlobal->TakeData(iWeigthRib, row);
        return;
    }

    wsPatern = L"Название файла отладки";
    if (wsKey == wsPatern)
    {
        ptrGlobal->TakeData(wsNameDebugFile, row);
        return;
    }

    wsPatern = L"Название лог файла (для пользователя)";
    if (wsKey == wsPatern)
    {
        ptrGlobal->TakeData(wsNameLogFile, row);
        return;
    }

    wsPatern = L"Создать новый каталог";
    if (wsKey == wsPatern)
    {
        ptrGlobal->TakeData(bCreateFolder, row);
        return;
    }

    wsPatern = L"Компактный вывод результата";
    if (wsKey == wsPatern)
    {
        ptrGlobal->TakeData(bCompactOutput, row);
        return;
    }

    wsPatern = L"Если у предмета несколько групп компетенций, учитывать пересечение дважды (для 100% суммы)";
    if (wsKey == wsPatern)
    {
        ptrGlobal->TakeData(bCompInterDelete, row);
        return;
    }

    wsPatern = L"Перезаписывать лог файл";
    if (wsKey == wsPatern)
    {
        ptrGlobal->TakeData(bReloadLogFile, row);
        return;
    }

    wsPatern = L"Использовать многоуровневые индикаторы";
    if (wsKey == wsPatern)
    {
        ptrGlobal->TakeData(bMultiIndicator, row);
        return;
    }

    wsPatern = L"Удалить спецсимволы из наименования компетенции";
    if (wsKey == wsPatern)
    {
        string sData;
        ptrGlobal->TakeData(sData, row);
        for (auto& it : sData)
            setIgnoreCharCompHeader.insert(it);
        return;
    }

    wsPatern = L"Регулярное выражение разбивки строки ([Компетенции(2)] Формируемые компетенции)";
    if (wsKey == wsPatern)
    {
        ptrGlobal->TakeData(sRegexComp, row);
        return;
    }

    wsPatern = L"Регулярное выражение поиска заголовка компетенции";
    if (wsKey == wsPatern)
    {
        ptrGlobal->TakeData(sRegexHeaderComp, row);
        return;
    }

    wsPatern = L"Формула расчёта весов рёбер";
    if (wsKey == wsPatern)
    {
        ptrGlobal->TakeData(sFormula, row);
        return;
    }
    
    wsPatern = L"Игнорировать пустые строки в конце странице, если их не менее X =";
    if (wsKey == wsPatern)
    {
        ptrGlobal->TakeData(iIgnoreEmptyLine, row);
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
                    ptrGlobal->SetMapParams(fBook.worksheet(ptrGlobal->ConwertToString(wsNamePage)))
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
                    ptrGlobal->SetMapParams(fBook.worksheet(ptrGlobal->ConwertToString(wsNamePage)))
                    )
                {
                    mapOutParams[key] = val;
                }
            }
        }
        return;
        
    }

    wsPatern = L"Игнорируемые предметы (название страницы)";
    if (wsKey == wsPatern)
    {
        setIgnoreDisc.clear();
        int i = -1; for (auto& page : row.cells())
        {
            i++;
            if (i)
            {
                wstring wsNamePage = ptrGlobal->GetValue(page);
                for (auto& [key, val] :
                    ptrGlobal->SetMapParams(fBook.worksheet(ptrGlobal->ConwertToString(wsNamePage)))
                    )
                {
                    setIgnoreDisc.insert(key);
                }
            }
        }
        return;
    }

    wsPatern = L"Игнорируемые УП (название страницы)";
    if (wsKey == wsPatern)
    {
        setIgnoreСurriculum.clear();
        int i = -1; for (auto& page : row.cells())
        {
            i++;
            if (i)
            {
                wstring wsNamePage = ptrGlobal->GetValue(page);
                for (auto& [key, val] :
                    ptrGlobal->SetMapParams(fBook.worksheet(ptrGlobal->ConwertToString(wsNamePage)))
                    )
                {
                    setIgnoreСurriculum.insert(ptrGlobal->ConwertToString(key));
                }
            }
        }
        return;
    }

}


vector<set<wstring>> FConfig::SetParsingParams(OpenXLSX::XLWorksheet& fPage)
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


FConfig::~FConfig()
{
    --iSinglControll;
}