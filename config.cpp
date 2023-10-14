#include "config.h"
#include "error.h"

int FConfig::iSinglControll = 0;

FConfig::FConfig(FGlobal* _ptrGlobal)
    : ptrGlobal(_ptrGlobal),
      iCourseLen(2),
      iMaxNameDiscLen(15),
      iIgnoreEmptyLine(5),
      iWeigthRib(10),
      iSoMachComp(6),
      iPrecision(5),
      iIndicatorDeep(3),
      dMinWeigthRib(0.01),
      dMinComp(0.01),
      dAnomalBigScore(40.0),
      bCreateFolder(true),
      bCompactOutput(true),
      bCourseOutput(true),
      bReloadLogFile(true),
      bMultiIndicator(true),
      bCompInterDelete(true),
      bIsPercentRegAll(true),
      bOutCompWithName(true),
      bOutShortNameCur(true),
      bIsUnDirected(true),
      bIsNormalizeScoreComp(true),
      bDeletingSpecCharDiscName(true),
      bOutAllInfoWithoutTag(true),
      bOutEmptyComp(true),
      bOutTotalInfo(true),
      bOutMaxMinData(true),
      //bSetNameIfNotIndex(true),
      //bOutIndicatorsInfo(true),
      wsNameConfig(L"./config.xlsx"),
      wsNamePage(L"Параметры"),
      sNameLabelHeader("Id;Label"),
      sNameRibHeader("Source;Target;Type;Weight"),
      sNameRibDir("Undirected"),
      arrNameFileIn({ L"plans\grad", L"plans\spec" }),
      arrNameFileOut({ L"result\grad", L"result\spec" }),
      wsNameDebugFile(L"debugFile.txt"),
      wsNameLogFile(L"logFile.txt"),
      // wsOutPrefMinMax(L"у УП "),
      // wsOutPrefAllCourse(L"Все курсы"),
      // sPrefCourseNumber("_"),
      // sSufAltGraphFile("Alt"),
      sRegexComp("{0, 1}(.{0, } ? ); "),
      sRegexHeaderIndicator("(.{1,})-(.{1,})\.(.{1,})"),
      sFormula("((L + R) / 2) * K")
{
    if (iSinglControll > 0) throw std::runtime_error("Re-creation Singleton");
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
    fBook            = fDoc.workbook();
    auto arrNamePage = fBook.worksheetNames();

    auto fMainPage = fBook.worksheet(ptrGlobal->ConwertToString(wsNamePage));
    for (auto& row : fMainPage.rows())
    {
        auto    it        = row.cells().begin();
        wstring sDescript = ptrGlobal->GetValue(*it);

        if (!SetParams(fBook, sDescript, row)) return false;
    }

    fDoc.close();

    return true;
}

bool FConfig::SetParams(OpenXLSX::XLWorkbook& fBook, wstring wsKey,
                        OpenXLSX::XLRow row)
{
    wstring wsPatern;
    try
    {
        if (wsKey == L"") return true;

        // Названия анализируемых страниц
        wsPatern = L"Названия анализируемых страниц";
        // Не обобщить, сложная логика
        if (wsKey == wsPatern)
        {
            arrKeyPage.clear();
            int i = -1;
            for (auto& page : row.cells())
            {
                i++;
                if (i)
                {
                    wstring wsNamePage = ptrGlobal->GetValue(page);
                    string  sNamePage  = ptrGlobal->ConwertToString(wsNamePage);

                    OpenXLSX::XLWorksheet fExtraPage =
                        fBook.worksheet(sNamePage);

                    arrKeyPage.push_back({ wsNamePage, sNamePage,
                                           SetParsingParams(fExtraPage) });
                }
            }
            return true;
        }

        // Каталог данных УП
        wsPatern = L"Считать ли метрики";
        if (wsKey == wsPatern)
        {
            vector<wstring> arrBool;
            ptrGlobal->TakeData(arrBool, row, 0);
            bArrIsSolveGraphMetric.resize(arrBool.size());
            for (int i = 0; i < arrBool.size(); ++i)
                bArrIsSolveGraphMetric[i] =
                    (arrBool[i].find(L"да") != wstring::npos);

            return true;
        }

        wsPatern = L"Объединить вывод данных УП с данными по графам";
        if (wsKey == wsPatern)
        {
            vector<wstring> arrBool;
            ptrGlobal->TakeData(arrBool, row, 3);
            bArrIsconcatGraphData.resize(arrBool.size());
            for (int i = 0; i < arrBool.size(); ++i)
                bArrIsconcatGraphData[i] =
                    (arrBool[i].find(L"да") != wstring::npos);

            return true;
        }

        // Категории дисциплин (основные, по выбору, факультативы)
        wsPatern = L"Определение вида дисциплины";
        if (wsKey == wsPatern)
        {
            ptrGlobal->TakeData(arrTypeDisc, row,
                                0);    // Нет ограничение на кол-во тегов,
            // но считается, что 0 - основной, 1 - по выбору, 2 - факультатив
            // (используется enum) Далее используется ETagDisc из solve.h
            return true;
        }

        // Проверка, что размер совпадает с arrTypeDisc.size()
        wsPatern = L"Названия видов дисциплин при выводе количества";
        if (wsKey == wsPatern)
        {
            int iSize = 0;
            if (arrTypeDisc.size() != 0) iSize = arrTypeDisc.size() + 1;
            ptrGlobal->TakeData(arrNameTypeDisc, row, iSize);
            return true;
        }

        // Каталог данных УП
        wsPatern = L"Каталоги данных УП";
        if (wsKey == wsPatern)
        {
            ptrGlobal->TakeData(arrNameFileIn, row, 0);
            return true;
        }

        // Каталог вывода результата
        wsPatern = L"Каталоги вывода результата";
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
            ptrGlobal->TakeData(sNameLabelHeader, row);
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

        wsPatern =
            L"Считать кол-во ЗЕ аномально большим, если его значение больше X=";
        if (wsKey == wsPatern)
        {
            ptrGlobal->TakeData(dAnomalBigScore, row);
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

        wsPatern = L"Выводить графы по курсам";
        if (wsKey == wsPatern)
        {
            ptrGlobal->TakeData(bCourseOutput, row);
            return true;
        }

        wsPatern = L"Если у предмета несколько групп компетенций, учитывать "
                   L"пересечение дважды (для 100% суммы)";
        if (wsKey == wsPatern)
        {
            ptrGlobal->TakeData(bCompInterDelete, row);
            return true;
        }

        wsPatern = L"Выводить процентную долю индикатора от всех ЗЕ УП";
        if (wsKey == wsPatern)
        {
            ptrGlobal->TakeData(bIsPercentRegAll, row);
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
            ptrGlobal->TakeData(bOutShortNameCur, row);
            return true;
        }

        wsPatern = L"Удалить спецсимволы из названия дисциплин (не влияет на "
                   L"игнорируемые)";
        if (wsKey == wsPatern)
        {
            ptrGlobal->TakeData(bDeletingSpecCharDiscName, row);
            return true;
        }

        wsPatern =
            L"Делить ЗЕ у компетениции на кол-во компетенций в дисциплине";
        if (wsKey == wsPatern)
        {
            ptrGlobal->TakeData(bIsNormalizeScoreComp, row);
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

        wsPatern = L"Вывод полной информации о дисциплин без тега";
        if (wsKey == wsPatern)
        {
            ptrGlobal->TakeData(bOutAllInfoWithoutTag, row);
            return true;
        }

        wsPatern = L"Выводить компетенции с 0 ЗЕ";
        if (wsKey == wsPatern)
        {
            ptrGlobal->TakeData(bOutEmptyComp, row);
            return true;
        }

        wsPatern = L"Выводить итоговую статистику";
        if (wsKey == wsPatern)
        {
            ptrGlobal->TakeData(bOutTotalInfo, row);
            return true;
        }

        wsPatern = L"Выводить коридор макс-мин";
        if (wsKey == wsPatern)
        {
            ptrGlobal->TakeData(bOutMaxMinData, row);
            return true;
        }

        //wsPatern = L"Выводить префикс название УП";
        //if (wsKey == wsPatern)
        //{
        //    ptrGlobal->TakeData(bOutCurriculaName, row);
        //    return true;
        //}

        wsPatern = L"Индикатор находится на глубине X=";
        if (wsKey == wsPatern)
        {
            ptrGlobal->TakeData(iIndicatorDeep, row);
            return true;
        }


        wsPatern = L"Спецсимволы для удаления из наименования компетенции";
        if (wsKey == wsPatern)
        {
            string sData;
            ptrGlobal->TakeData(sData, row);
            for (auto& it : sData) setIgnoreCharCompHeader.insert(it);
            return true;
        }

        wsPatern = L"Регулярное выражение разбивки строки ([Компетенции(2)] "
                   L"Формируемые компетенции)";
        if (wsKey == wsPatern)
        {
            ptrGlobal->TakeData(sRegexComp, row);
            return true;
        }

        wsPatern = L"Регулярное выражение разбивки индикатора";
        if (wsKey == wsPatern)
        {
            ptrGlobal->TakeData(sRegexHeaderIndicator, row);
            return true;
        }

        wsPatern = L"Регулярное выражение разбивки компетенции";
        if (wsKey == wsPatern)
        {
            ptrGlobal->TakeData(sRegexHeaderComp, row);
            return true;
        }

        wsPatern = L"Формула расчёта весов рёбер";
        if (wsKey == wsPatern)
        {
            ptrGlobal->TakeData(sFormula, row);
            return true;
        }

        wsPatern = L"Разделитель между названием дисциплины и её курсом";
        if (wsKey == wsPatern)
        {
            ptrGlobal->TakeData(sPrefCourseNumber, row);
            return true;
        }

        wsPatern = L"Игнорировать пустые строки в конце странице, если их не "
                   L"менее X =";
        if (wsKey == wsPatern)
        {
            ptrGlobal->TakeData(iIgnoreEmptyLine, row);
            return true;
        }

        wsPatern = L"Считать, что у дисциплины компетенций много, если их "
                   L"число больше X =";
        if (wsKey == wsPatern)
        {
            ptrGlobal->TakeData(iSoMachComp, row);
            return true;
        }

        wsPatern = L"Макс кол-во знаков после запятой";
        if (wsKey == wsPatern)
        {
            ptrGlobal->TakeData(iPrecision, row);
            return true;
        }

        wsPatern = L"Псевдонимы (название страницы)";
        if (wsKey == wsPatern)
        {
            mapAliasRename.clear();
            int i = -1;
            for (auto& page : row.cells())
            {
                i++;
                if (i)
                {
                    wstring wsNamePage = ptrGlobal->GetValue(page);
                    for (auto& [key, val] : ptrGlobal->SetMapParams(
                             fBook.worksheet(
                                 ptrGlobal->ConwertToString(wsNamePage)),
                             2))
                    {
                        mapAliasRename[key] = val.at(0);
                    }
                }
            }
            return true;
        }

        wsPatern = L"Тег дисциплины (название страницы)";
        if (wsKey == wsPatern)
        {
            mapAliasRename.clear();
            int i = -1;
            for (auto& page : row.cells())
            {
                i++;
                if (i)
                {
                    wstring wsNamePage = ptrGlobal->GetValue(page);
                    for (auto& [key, val] : ptrGlobal->SetMapParams(
                             fBook.worksheet(
                                 ptrGlobal->ConwertToString(wsNamePage)),
                             0))
                    {
                        int iNum = -1;
                        if (key.size() < 3)// У нас не будет более 100 тегов
                        {
                            try
                            {
                                iNum = stoi(key);
                            }
                            catch (...)
                            {
                                iNum = -1;
                            }
                        }

                        if (iNum != -1)
                        {
                            if (val.size() != 1)    // Если это id тега, то у
                                                    // него ровно одно значение
                            {
                                throw std::out_of_range("Tag size no equeal 1 (" +
                                                        to_string(val.size()) +
                                                        ")!+");
                            }

                            arrTagName.push_back(val.at(0));
                        }
                        else
                        {
                            if (mapTagDisc.count(key))
                            {
                                ptrGlobal->ptrError
                                    ->ErrorConfiqDublicateNameDisc(key);
                                return false;    // Дублирование одного и
                                                 // того же предмета
                            }
                            for (auto& it : val)
                            {
                                int iIdTag = stoi(it);
                                mapTagDisc[key].insert(iIdTag);
                            }
                        }
                    }
                }
            }
            return true;
        }

        wsPatern = L"Параметры расширенного вывода (название страницы)";
        if (wsKey == wsPatern)
        {
            mapAddOutParams.clear();
            int i = -1;
            for (auto& page : row.cells())
            {
                i++;
                if (i)
                {
                    wstring wsNamePage = ptrGlobal->GetValue(page);
                    for (auto& [key, val] : ptrGlobal->SetMapParams(
                             fBook.worksheet(
                                 ptrGlobal->ConwertToString(wsNamePage)),
                             3)) //Теперь ещё и считываем, требуется ли выводить
                    {
                        mapAddOutParams[key] = val;
                    }
                }
            }
            return true;
        }

        wsPatern = L"Параметры вывода (название страницы)";
        if (wsKey == wsPatern)
        {
            mapArrOutParams.clear();
            int i = -1;
            for (auto& page : row.cells())
            {
                i++;
                if (i)
                {
                    wstring wsNamePage = ptrGlobal->GetValue(page);
                    for (auto& [key, val] : ptrGlobal->SetMapParams(
                             fBook.worksheet(
                                 ptrGlobal->ConwertToString(wsNamePage)),
                             { 2, 3, 4 }))
                    {
                        mapArrOutParams[key] = val;
                    }
                }
            }
            // Принудительно ставим "да", так как имя у УП обязательно нужно
            // выводить
            mapArrOutParams[L"Название учебного плана"].push_back(L"да");
            return true;
        }

        wsPatern = L"Игнорируемые предметы (название страницы)";
        if (wsKey == wsPatern)
        {
            setIgnoreDisc.clear();
            int i = -1;
            for (auto& page : row.cells())
            {
                i++;
                if (i)
                {
                    wstring wsNamePage = ptrGlobal->GetValue(page);
                    for (auto& [key, val] : ptrGlobal->SetMapParams(
                             fBook.worksheet(
                                 ptrGlobal->ConwertToString(wsNamePage)),
                             1))
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
            int i = -1;
            for (auto& page : row.cells())
            {
                i++;
                if (i)
                {
                    wstring wsNamePage = ptrGlobal->GetValue(page);
                    for (auto& [key, val] : ptrGlobal->SetMapParams(
                             fBook.worksheet(
                                 ptrGlobal->ConwertToString(wsNamePage)),
                             1))
                    {
                        setIgnoreСurriculum.insert(
                            ptrGlobal->ConwertToString(key));
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
            ptrGlobal->ptrError->ErrorBadConfigSizeParams(
                ptrGlobal->ConwertToString(wsPatern), eError.what());
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
    for (const auto& row : fPage.rows())
    {
        if (y++)    // Игнорируем первую строку, в ней описание столбцов
        {
            int i = 0;
            for (const auto& column : row.cells())
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

FConfig::~FConfig() { --iSinglControll; }