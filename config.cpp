﻿#include "config.h"
#include "error.h"
#include "graph.h"

#pragma warning(disable : 4267)    // size_t лучше не использовать,

int FConfig::iSinglControll = 0;

FConfig::FConfig(shared_ptr<FGlobal> _ptrGlobal)
    : ptrGlobal(_ptrGlobal),
      iCourseLen(2),
      iMaxNameDiscLen(15),
      iIgnoreEmptyLine(5),
      iSoMachComp(6),
      iPrecision(5),
      iHeaderRow(1),
      // iIndicatorDeep(3),
      dMinWeigthRib(0.01),
      dMinComp(0.01),
      dAnomalBigScore(40.0),
      dTruncAvg(0.07),
      dTruncQuarPathLen(0.05),
      bCreateFolder(true),
      bCompactOutput(true),
      bCourseOutput(true),
      bReloadLogFile(true),
      bMultiIndicator(true),
      bCompInterDelete(true),
      bIsPercentRegAll(true),
      bOutCompWithName(true),
      bOutCourseNumWithName(true),
      bOutShortNameCur(true),
      bIsUnDirected(true),
      bIsNormalizeScoreComp(true),
      bIsNormalizeScoreDisc(true),
      bIsIgnoreTreeInd(false),
      bDelSpecCharDiscName(true),
      bOutAllInfoWithoutTag(true),
      bOutEmptyComp(true),
      bOutTotalInfo(true),
      bOutWithoutEmptyCell(true),
      bOutCompRU(true),
      bIsOutCSVDate(false),
      arrNameLabelHeader({ "Id", "Label", "Weight", "Tag" }),
      arrNameRibHeader({ "Source", "Target", "Type", "Label", "Weight" }),
      sNameRibDir("Undirected"),
      arrNameFileIn({ L"plans\grad", L"plans\spec" }),
      arrNameFileOut({ L"result\grad", L"result\spec" }),
      sNameDebugFile("debugFile.txt"),
      sNameLogFile("logFile.txt"),
      sNameFileCompData("CompData"),
      sSeparator("_"),
      sPrefFullNameCourse("."),
      // sRegexComp("{0, 1}(.{0, } ? );"),
      sFormula("((L + R) / 2) * K"),
      sFormulaReverseGraph("((L + R) / 2) * D"),
      wsNamePage(L"Параметры")
{
    // Unit test против такого
#ifndef UNIT_TEST
    if (iSinglControll > 0)
        throw std::runtime_error("Re-creation Singleton");
#endif

    ++iSinglControll;

    wsNamePage = L"Параметры";

    InitStringMap();
    InitIntMap();
    InitBoolMap();
    InitDoubleMap();
    // InitWStringMap();
    InitVectorWStringMap();
    InitVectorStringMap();
}

void FConfig::InitStringMap()
{
    mapStringParamsReadKey[L"Предлог перед выводом статистики по всем курсам"] =
        &sAllCourses;
    mapStringParamsReadKey
        [L"Предлог перед выводом статистики по курсу определённого номера"] =
            &sCurrentCourse;
    mapStringParamsReadKey[L"Суффикс для вывода УП мер центральной тенденции"] =
        &sCorridorCurricula;

    mapStringParamsReadKey[L"Название обычного графа"] = &sCommonCurricula;
    mapStringParamsReadKey[L"Название альтернативного графа"] = &sAltCurricula;
    mapStringParamsReadKey[L"Название обратного графа"]       = &sRevCurricula;

    mapStringParamsReadKey[L"Суффикс после вывода квартиля"] = &sSufQuar;

    mapStringParamsReadKey[L"Суффикс файла с данными вершин Gephi"] =
        &sSufLableGephiFile;
    mapStringParamsReadKey[L"Суффикс файла с данными рёбер Gephi"] =
        &sSufRibGephiFile;

    mapStringParamsReadKey
        [L"Суффикс после вывода кол-во компетенций у дисциплины"] = &sSufComp;

    mapStringParamsReadKey[L"Тип рёбер"] = &sNameRibDir;

    mapStringParamsReadKey
        [L"Формула расчёта весов рёбер графа, где вершины - это дисциплина"] =
            &sFormula;

    mapStringParamsReadKey
        [L"Формула расчёта весов рёбер графа, где вершины - это компетенция"] =
            &sFormulaReverseGraph;

    mapStringParamsReadKey
        [L"Название файла вывода общей статистике по всей выборке УП"] =
            &sNameFileTotalData;

    mapStringParamsReadKey[L"Название файла вывода общей локальной статистики "
                           L"по конкретному УП"] = &sNameFileAllLocalData;

    mapStringParamsReadKey[L"Названия файлов с данными компетенций"] =
        &sNameFileCompData;

    mapStringParamsReadKey[L"Разделитель в названиях"] = &sSeparator;

    mapStringParamsReadKey
        [L"Разделитель для вывода полного названия компетенций и индикаторов"] =
            &sPrefFullNameCourse;

    mapStringParamsReadKey[L"Вывод строки, если данные не определены"] =
        &sNoInitData;

    mapStringParamsReadKey[L"Разделитель CSV файлов"] = &sCSVSeparator;

    mapStringParamsReadKey[L"Название файла отладки"] = &sNameDebugFile;
    mapStringParamsReadKey[L"Название лог файла (для пользователя)"] =
        &sNameLogFile;
}

void FConfig::InitIntMap()
{
    mapIntParamsReadKey[L"Макс длина названия дисциплин"]   = &iMaxNameDiscLen;
    mapIntParamsReadKey[L"Сколько семестров в одном курсе"] = &iCourseLen;
    mapIntParamsReadKey
        [L"Игнорировать пустые строки в конце странице, если их не менее X ="] =
            &iIgnoreEmptyLine;
    mapIntParamsReadKey[L"Считать, что у дисциплины компетенций много, если их "
                        L"число больше X ="]                 = &iSoMachComp;
    mapIntParamsReadKey[L"Количество квартилей"]             = &iAmountQuar;
    mapIntParamsReadKey[L"Макс кол-во знаков после запятой"] = &iPrecision;
    mapIntParamsReadKey[L"До какой строки считывать заголовок"] = &iHeaderRow;
    // mapIntParamsReadKey[L"Индикатор находится на глубине X="] =
    // &iIndicatorDeep;
}

void FConfig::InitBoolMap()
{
    mapBoolParamsReadKey[L"Создать новый каталог"]       = &bCreateFolder;
    mapBoolParamsReadKey[L"Компактный вывод результата"] = &bCompactOutput;
    mapBoolParamsReadKey[L"Перезаписывать лог файл"] = &bReloadLogFile;
    mapBoolParamsReadKey[L"Вывод доп файлов csv"]    = &bIsOutCSVDate;
    mapBoolParamsReadKey
        [L"Вывод файла общей локальной статистики по конкретному УП"] =
            &bIsOutFileAllLocalData;

    mapBoolParamsReadKey[L"Если у предмета несколько групп компетенций, "
                         L"учитывать пересечение дважды (для 100% суммы)"] =
        &bCompInterDelete;
    mapBoolParamsReadKey[L"Выводить процентную долю индикатора от всех ЗЕ УП"] =
        &bIsPercentRegAll;
    mapBoolParamsReadKey[L"Отображать компетенции в названии"] =
        &bOutCompWithName;
    mapBoolParamsReadKey[L"Отображать номер курса в названии"] =
        &bOutCourseNumWithName;

    mapBoolParamsReadKey[L"Выводить короткое имя для УП"] = &bOutShortNameCur;
    mapBoolParamsReadKey[L"Удалить спецсимволы из названия дисциплин"] =
        &bDelSpecCharDiscName;

    mapBoolParamsReadKey
        [L"Делить ЗЕ у компетенции на кол-во компетенций в дисциплине"] =
            &bIsNormalizeScoreComp;
    mapBoolParamsReadKey
        [L"Делить ЗЕ у компетенции на кол-во дисциплин в компетенции"] =
            &bIsNormalizeScoreDisc;

    mapBoolParamsReadKey[L"Игнорировать индикаторы в дереве компетенций"] =
        &bIsIgnoreTreeInd;

    mapBoolParamsReadKey[L"Граф неориентированный"]  = &bIsUnDirected;
    
    mapBoolParamsReadKey[L"Переводить на латиницу"] = &bIsLocaleData;

    mapBoolParamsReadKey[L"Выводить УП мер центральной тенденции"] =
        &bIsOutCorridorCurricula;

    mapBoolParamsReadKey[L"Использовать многоуровневые индикаторы"] =
        &bMultiIndicator;
    mapBoolParamsReadKey[L"Вывод полной информации о дисциплин без тега"] =
        &bOutAllInfoWithoutTag;

    mapBoolParamsReadKey[L"Выводить компетенции с 0 ЗЕ"] = &bOutEmptyComp;

    mapBoolParamsReadKey[L"Выводить меры центральной тенденции"] =
        &bOutDataCorridor;

    mapBoolParamsReadKey
        [L"Не оставлять пустые ячейки при выводе дерева компетенций"] =
            &bOutWithoutEmptyCell;
    
    mapBoolParamsReadKey
        [L"Компетенции на кириллице"] =
            &bOutCompRU;
}

void FConfig::InitDoubleMap()
{
    mapDoubleParamsReadKey[L"Создавать ребро, если его вес больше X="] =
        &dMinWeigthRib;
    mapDoubleParamsReadKey
        [L"Считать компетенцию отсутствующей, если значение меньше X="] =
            &dMinComp;
    mapDoubleParamsReadKey
        [L"Считать кол-во ЗЕ аномально большим, если его значение больше X="] =
            &dAnomalBigScore;
}

void FConfig::InitVectorWStringMap()
{
    mapVectorWStringParamsReadKey[L"Определение вида дисциплины"] = {
        &arrTypeDisc, 0
    };

    mapVectorWStringParamsReadKey[L"Каталоги данных УП"] = { &arrNameFileIn,
                                                             0 };
}

void FConfig::InitVectorStringMap()
{
    mapVectorStringParamsReadKey[L"Заголовок файла с названиями вершин"] = {
        &arrNameLabelHeader, 0
    };

    mapVectorStringParamsReadKey[L"Заголовок файла с рёбрами"] = {
        &arrNameRibHeader, 0
    };

    mapVectorStringParamsReadKey
        [L"Регулярное выражение разбивки строки ([Компетенции(2)] "
         L"Формируемые компетенции)"] = { &arrRegexComp, 0 };

    mapVectorStringParamsReadKey
        [L"Регулярное выражение разбивки индикатора"] = { &arrRegexHeaderInd,
                                                          0 };

    mapVectorStringParamsReadKey
        [L"Регулярное выражение разбивки компетенции"] = { &arrRegexHeaderComp,
                                                           0 };

    mapVectorStringParamsReadKey
        [L"Регулярное выражение разбивки кода направления подготовки"] = {
            &arrRegexCodeUGSN, 0
        };
}

bool FConfig::Init()
{
    OpenXLSX::XLDocument fDoc;
    OpenXLSX::XLWorkbook fBook;

    try
    {
        fDoc.open(ptrGlobal->GetSNameFileConfig());
    }
    catch (...)
    {
        ptrGlobal->ptrError->ErrorNotFoundConfig();
        return false;
    }
    fBook            = fDoc.workbook();
    auto arrNamePage = fBook.worksheetNames();

    auto fMainPage = fBook.worksheet(ptrGlobal->ConwertToString(wsNamePage));
    int  i         = -1;
    for (auto& row : fMainPage.rows())
    {
        ++i;
        auto    it        = row.cells().begin();
        wstring sDescript = ptrGlobal->GetValue(*it);

        if (!SetParams(fBook, sDescript, row))
        {
            ptrGlobal->ptrError->ErrorParams(sDescript);
            return false;
        }
    }

    fDoc.close();

    ETypeGraph::ETG_Common.sName  = sCommonCurricula;
    ETypeGraph::ETG_Alt.sName     = sAltCurricula;
    ETypeGraph::ETG_Reverse.sName = sRevCurricula;

    return true;
}

const FPageInfo& FConfig::GetKeyPage(int iIndex) const
{
    if (iIndex < arrKeyPage.size())
        return arrKeyPage[iIndex];
    throw(std::out_of_range("Bad data access index (arrKeyPage): " +
                            to_string(iIndex)));
}

int FConfig::GetSizeKeyPage() const
{
    return arrKeyPage.size();
}
//
// const string& FConfig::GetStringParams(string sKey) const
//{
//    if (mapStringParamsData.count(sKey)) return mapStringParamsData.at(sKey);
//    throw(std::out_of_range("Bad data access key (mapStringParamsData): " +
//                            sKey));
//}

bool FConfig::SetParams(OpenXLSX::XLWorkbook& fBook, wstring wsKey,
                        OpenXLSX::XLRow row)
{
    wstring wsPatern;
    try
    {
        if (wsKey == L"")
            return true;

        // Считать переменную из config.xlsx с типом данных string
        if (mapStringParamsReadKey.count(wsKey))
        {
            ptrGlobal->TakeData(*mapStringParamsReadKey[wsKey], row);
            return true;
        }

        // Считать переменную из config.xlsx с типом данных int
        if (mapIntParamsReadKey.count(wsKey))
        {
            ptrGlobal->TakeData(*mapIntParamsReadKey[wsKey], row);
            return true;
        }

        // Считать переменную из config.xlsx с типом данных double
        if (mapDoubleParamsReadKey.count(wsKey))
        {
            ptrGlobal->TakeData(*mapDoubleParamsReadKey[wsKey], row);
            return true;
        }

        // Считать переменную из config.xlsx с типом данных bool
        if (mapBoolParamsReadKey.count(wsKey))
        {
            ptrGlobal->TakeData(*mapBoolParamsReadKey[wsKey], row);
            return true;
        }

        // Считать переменную из config.xlsx с типом данных wstring
        if (mapStringParamsReadKey.count(wsKey))
        {
            ptrGlobal->TakeData(*mapStringParamsReadKey[wsKey], row);
            return true;
        }

        // Считать переменную из config.xlsx с типом данных vector<wstring>
        if (mapVectorWStringParamsReadKey.count(wsKey))
        {
            ptrGlobal->TakeData(*mapVectorWStringParamsReadKey[wsKey].first,
                                row,
                                mapVectorWStringParamsReadKey[wsKey].second);
            return true;
        }

        // Считать переменную из config.xlsx с типом данных vector<string>
        if (mapVectorStringParamsReadKey.count(wsKey))
        {
            ptrGlobal->TakeData(*mapVectorStringParamsReadKey[wsKey].first,
                                row,
                                mapVectorStringParamsReadKey[wsKey].second);
            return true;
        }

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
                    (ptrGlobal->IsThatIsTrue(arrBool[i]));

            return true;
        }

        wsPatern = L"Объединить вывод данных УП с данными по графам";
        if (wsKey == wsPatern)
        {
            vector<wstring> arrBool;
            ptrGlobal->TakeData(arrBool, row, 3);
            bArrIsconcatGraphData.resize(arrBool.size());
            for (int i = 0; i < arrBool.size(); ++i)
                bArrIsconcatGraphData[i] = ptrGlobal->IsThatIsTrue(arrBool[i]);

            return true;
        }

        // Проверка, что размер совпадает с arrTypeDisc.size()
        wsPatern = L"Названия видов дисциплин при выводе количества";
        if (wsKey == wsPatern)
        {
            int iSize = 0;
            if (arrTypeDisc.size() != 0)
                iSize = arrTypeDisc.size() + 1;
            ptrGlobal->TakeData(arrTypeDiscAmount, row, iSize);
            return true;
        }

        wsPatern = L"Названия видов дисциплин при выводе ЗЕ";
        if (wsKey == wsPatern)
        {
            int iSize = 0;
            if (arrTypeDisc.size() != 0)
                iSize = arrTypeDisc.size() + 1;
            ptrGlobal->TakeData(arrTypeDiscCredits, row, iSize);
            return true;
        }

        // Каталог вывода результата
        wsPatern = L"Каталоги вывода результата";
        if (wsKey == wsPatern)
        {
            int iSize = 0;
            if (arrNameFileIn.size() != 0)
                iSize = arrNameFileIn.size() + 1;
            ptrGlobal->TakeData(arrNameFileOut, row, iSize);
            return true;
        }

        wsPatern = L"Усечение среднего для мер центральной тенденции (процент)";
        if (wsKey == wsPatern)
        {
            ptrGlobal->TakeData(dTruncAvg, row);
            if (dTruncAvg > 0.5)
                dTruncAvg = 0.5;    // Нет смысла ставить больше 50% так
                                    // как уже не будет выборки
            return true;
        }

        wsPatern = L"Усечение среднего для квартилей (процент)";
        if (wsKey == wsPatern)
        {
            ptrGlobal->TakeData(dTruncQuarPathLen, row);
            if (dTruncQuarPathLen > 0.5)
                dTruncQuarPathLen = 0.5;    // Нет смысла ставить больше 50% так
                                            // как уже не будет выборки
            return true;
        }

        wsPatern = L"Спецсимволы для удаления из наименования компетенции";
        if (wsKey == wsPatern)
        {
            string sData;
            ptrGlobal->TakeData(sData, row);
            for (auto& it : sData)
                setIgnoreCharCompHeader.insert(it);
            return true;
        }

        wsPatern = L"Индексы групп регулярного выражения";
        if (wsKey == wsPatern)
        {
            vector<string> bufRegexIndexGroup;
            ptrGlobal->TakeData(bufRegexIndexGroup, row,
                                arrRegexHeaderInd.size() + 1);

            // Конвертация из строки массива чисел в массив чисел
            //  "1 2 3" -> {1,2,3}

            for (const auto& str : bufRegexIndexGroup)
            {
                int         num = 0;
                vector<int> bufInt;
                for (const auto& it : str)
                    if (it >= '0' && it < '9')
                    {
                        num *= 10;
                        num += it - '0';
                    }
                    else
                    {
                        bufInt.push_back(num);
                        num = 0;
                    }
                if (num)
                    bufInt.push_back(num);
                if (arrRegexIndexGroup.size())
                    if (arrRegexIndexGroup[0].size() != bufInt.size())
                        return false;    // Определили другое количество групп
                arrRegexIndexGroup.push_back(bufInt);
            }

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
                        if (key.size() < 3)    // У нас не будет более 100 тегов
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
                                throw std::out_of_range(
                                    "Tag size no equeal 1 (" +
                                    to_string(val.size()) + ")!+");
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
                                mapTagDisc[key].insert(ETagDisc(iIdTag));
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
                             3))    // Теперь ещё и считываем, требуется ли
                                    // выводить
                    {
                        auto& it  = mapAddOutParams[key];

                        if (GetBIsLocaleData())
                        {
                            it.wsName = val.at(0);
                        }
                        else
                        {
                            it.wsName = key;
                        }

                        for (int k = 1; k < val.size(); ++k)
                        {
                            it.arrFlag.push_back(
                                ptrGlobal->IsThatIsTrue(val[k]));
                        }
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
                        auto& it = mapArrOutParams[key];

                        if (GetBIsLocaleData())
                        {
                            it.wsName = val.at(0);
                        }
                        else
                        {
                            it.wsName = key;
                        }

                        for (int k = 1; k < val.size(); ++k)
                        {
                            it.arrFlag.push_back(
                                ptrGlobal->IsThatIsTrue(val[k]));
                        }
                    }
                }
            }
            // Принудительно ставим "да", так как имя у УП обязательно нужно
            // выводить
            // mapArrOutParams[L"Название учебного плана"].push_back(L"да");

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
                        // Иногда Excel воспринимает последнюю строку, как
                        // пустую
                        if (key != L"")
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
            if (wsPatern == L"")
            {
                ptrGlobal->ptrError->ErrorBadConfigSizeParams(
                    ptrGlobal->ConwertToString(wsKey), eError.what());
            }
            else
            {
                ptrGlobal->ptrError->ErrorBadConfigSizeParams(
                    ptrGlobal->ConwertToString(wsPatern), eError.what());
            }
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

FConfig::~FConfig()
{
    --iSinglControll;
}