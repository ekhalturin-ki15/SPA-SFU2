#include "config.h"
#include "error.h"

int FConfig::iSinglControll = 0;

FConfig::FConfig(FGlobal* _ptrGlobal) : ptrGlobal(_ptrGlobal)
,iCourseLen(2), iMaxNameDiscLen(15), iIgnoreEmptyLine(5) ,iWeigthRib(10), dMinWeigthRib(0.01)
,bCreateFolder(true), bCompactOutput(true), bReloadLogFile(true)
, bMultiIndicator(true), bCompInterDelete(true), bOutCompWithName(true)
, bOutShortNameCurr(true), bIsUnDirected(true)
,wsNameConfig(L"./config.xlsx"), wsNamePage(L"Параметры")
, sNameLableHeader("Id;Label"), sNameRibHeader("Source;Target;Type;Weight"), sNameRibDir("Undirected")
, arrNameFileIn(), arrNameFileOut()
, wsNameDebugFile(L"debugFile.txt"), wsNameLogFile(L"logFile.txt")
, sOutPrefMinMax("Предлог перед выводом результата мин. макс.")
, sRegexComp("{0, 1}(.{0, } ? ); "), sRegexHeaderIndicator("(.{1,})-(.{1,})\.(.{1,})"), sFormula("((L + R) / 2) * K")
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

        if (!SetParams(fBook, sDescript, row)) return false;
    }

    fDoc.close();

    return true;
}

bool FConfig::SetParams(OpenXLSX::XLWorkbook& fBook, wstring wsKey, OpenXLSX::XLRow row)
{
    wstring wsPatern;
    try
    {
        if (wsKey == L"") return true;

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
            return true;
        }

        //Каталог данных УП
        wsPatern = L"Каталог данных УП";
        if (wsKey == wsPatern)
        {
            int iSize = 0;
            if (arrNameFileOut.size() != 0) iSize = arrNameFileOut.size() + 1;
            ptrGlobal->TakeData(arrNameFileIn, row, iSize);
            return true;
        }

        //Каталог вывода результата
        wsPatern = L"Каталог вывода результата";
        if (wsKey == wsPatern)
        {
            int iSize = 0;
            if (arrNameFileIn.size() != 0) iSize = arrNameFileIn.size() + 1;
            ptrGlobal->TakeData(arrNameFileOut, row, iSize);
            return true;
        }

        wsPatern = L"Заголовок файла с названиями вершин";
        if (wsKey == wsPatern)
        {
            ptrGlobal->TakeData(sNameLableHeader, row);
            return true;
        }

        wsPatern = L"Заголовок файла с рёбрами";
        if (wsKey == wsPatern)
        {
            ptrGlobal->TakeData(sNameRibHeader, row);
            return true;
        }

        wsPatern = L"Тип рёбер";
        if (wsKey == wsPatern)
        {
            ptrGlobal->TakeData(sNameRibDir, row);
            return true;
        }

        wsPatern = L"Макс длина названия дисциплин";
        if (wsKey == wsPatern)
        {
            ptrGlobal->TakeData(iMaxNameDiscLen, row);
            return true;
        }

        wsPatern = L"Сколько семестров в одном курсе";
        if (wsKey == wsPatern)
        {
            ptrGlobal->TakeData(iCourseLen, row);
            return true;
        }

        wsPatern = L"Создавать ребро, если его вес больше X=";
        if (wsKey == wsPatern)
        {
            ptrGlobal->TakeData(dMinWeigthRib, row);
            return true;
        }

        wsPatern = L"Считать компетенцию отсутсвующей, если значение меньше X=";
        if (wsKey == wsPatern)
        {
            ptrGlobal->TakeData(dMinComp, row);
            return true;
        }

        wsPatern = L"Размер рёбер";
        if (wsKey == wsPatern)
        {
            ptrGlobal->TakeData(iWeigthRib, row);
            return true;
        }

        wsPatern = L"Название файла отладки";
        if (wsKey == wsPatern)
        {
            ptrGlobal->TakeData(wsNameDebugFile, row);
            return true;
        }

        wsPatern = L"Название лог файла (для пользователя)";
        if (wsKey == wsPatern)
        {
            ptrGlobal->TakeData(wsNameLogFile, row);
            return true;
        }

        wsPatern = L"Создать новый каталог";
        if (wsKey == wsPatern)
        {
            ptrGlobal->TakeData(bCreateFolder, row);
            return true;
        }

        wsPatern = L"Компактный вывод результата";
        if (wsKey == wsPatern)
        {
            ptrGlobal->TakeData(bCompactOutput, row);
            return true;
        }

        wsPatern = L"Если у предмета несколько групп компетенций, учитывать пересечение дважды (для 100% суммы)";
        if (wsKey == wsPatern)
        {
            ptrGlobal->TakeData(bCompInterDelete, row);
            return true;
        }

        wsPatern = L"Отображать компетенции в названии";
        if (wsKey == wsPatern)
        {
            ptrGlobal->TakeData(bOutCompWithName, row);
            return true;
        }


        wsPatern = L"Выводить короткое имя для УП";
        if (wsKey == wsPatern)
        {
            ptrGlobal->TakeData(bOutShortNameCurr, row);
            return true;
        }

        wsPatern = L"Граф неориентированный";
        if (wsKey == wsPatern)
        {
            ptrGlobal->TakeData(bIsUnDirected, row);
            return true;
        }

        wsPatern = L"Перезаписывать лог файл";
        if (wsKey == wsPatern)
        {
            ptrGlobal->TakeData(bReloadLogFile, row);
            return true;
        }

        wsPatern = L"Использовать многоуровневые индикаторы";
        if (wsKey == wsPatern)
        {
            ptrGlobal->TakeData(bMultiIndicator, row);
            return true;
        }

        wsPatern = L"Удалить спецсимволы из наименования компетенции";
        if (wsKey == wsPatern)
        {
            string sData;
            ptrGlobal->TakeData(sData, row);
            for (auto& it : sData)
                setIgnoreCharCompHeader.insert(it);
            return true;
        }

        wsPatern = L"Регулярное выражение разбивки строки ([Компетенции(2)] Формируемые компетенции)";
        if (wsKey == wsPatern)
        {
            ptrGlobal->TakeData(sRegexComp, row);
            return true;
        }

        wsPatern = L"Регулярное выражение поиска заголовка компетенции";
        if (wsKey == wsPatern)
        {
            ptrGlobal->TakeData(sRegexHeaderIndicator, row);
            return true;
        }

        wsPatern = L"Формула расчёта весов рёбер";
        if (wsKey == wsPatern)
        {
            ptrGlobal->TakeData(sFormula, row);
            return true;
        }

        wsPatern = L"Предлог перед выводом результата мин. макс.";
        if (wsKey == wsPatern)
        {
            ptrGlobal->TakeData(sOutPrefMinMax, row);
            return true;
        }

        wsPatern = L"Игнорировать пустые строки в конце странице, если их не менее X =";
        if (wsKey == wsPatern)
        {
            ptrGlobal->TakeData(iIgnoreEmptyLine, row);
            return true;
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
                        ptrGlobal->SetMapParams(fBook.worksheet(ptrGlobal->ConwertToString(wsNamePage)), 2)
                        )
                    {
                        fAlias.mapRename[key] = val.at(0);
                    }
                }
            }
            return true;
        }

        wsPatern = L"Параметры выводы (название страницы)";
        if (wsKey == wsPatern)
        {
            mapArrOutParams.clear();
            int i = -1; for (auto& page : row.cells())
            {
                i++;
                if (i)
                {
                    wstring wsNamePage = ptrGlobal->GetValue(page);
                    for (auto& [key, val] :
                        ptrGlobal->SetMapParams(fBook.worksheet(ptrGlobal->ConwertToString(wsNamePage)), 4)
                        )
                    {
                        mapArrOutParams[key] = val;
                    }
                }
            }
            return true;

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
                        ptrGlobal->SetMapParams(fBook.worksheet(ptrGlobal->ConwertToString(wsNamePage)), 1)
                        )
                    {
                        setIgnoreDisc.insert(key);
                    }
                }
            }
            return true;
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
                        ptrGlobal->SetMapParams(fBook.worksheet(ptrGlobal->ConwertToString(wsNamePage)), 1)
                        )
                    {
                        setIgnoreСurriculum.insert(ptrGlobal->ConwertToString(key));
                    }
                }
            }
            return true;
        }
    }
    catch (out_of_range eError)
    {
        if (ptrGlobal->ptrError)
        {
            ptrGlobal->ptrError->ErrorBadConfigSizeParams(ptrGlobal->ConwertToString(wsPatern), eError.what());
        }
        return false;
    }
    catch (...)
    {
        return false;
    }
    return true;
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