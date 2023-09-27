#include "outData.h"
#include "config.h"
#include "error.h"
#include "global.h"
#include "graph.h"
#include "metric.h"
#include "solve.h"

FOutData::FOutData(FGlobal* _ptrGlobal)
    : ptrGlobal(_ptrGlobal),
      arrCompetenceHead({ L"За какой курс", L"Заголовок компетенции",
                          L"Процент распределения Заголовка компетенции",
                          L"Компетенция", L"Процент распределения Компетенции",
                          L"Индекс", L"Процент распределения Индекса" }),
      arrMetricHead(
          { // L"Название учебного плана",
            L"Всего ЗЕ в графе", L"Кол-во дисциплин в графе",
            // L"(Расш.) Общее кол-во ЗЕ в УП",
            // L"(Расш.) Кол-во дисциплин в УП",    // Включает те,
            // что указаны в
            //  УП, как неучитываемые
            L"Минимальное ЗЕ у дисциплины", L"Максимальное ЗЕ у дисциплины",
            L"Минимальный вес ребра", L"Максимальный вес ребра",
            L"Диаметр графа по расстоянию",
            L"Диаметр графа по количеству рёбер",
            L"Количество компонент связности", L"Минимальное оставное дерево",
            L"Максимальное оставное дерево", L"Количество основных дисциплин",
            L"Количество дисциплин по выбору", L"Количество факультативов" })
{
}

void FOutData::CreateTotalInfo(vector<double>&   arrReturnDataMetrics,
                               const FGraphType* fGraph,
                               EOutType          eOutType)
{
    arrReturnDataMetrics.clear();
    vector<double> arrResult;    // Соответствует arrHead который
                                 // проинициализирован в конструкторе
    arrResult = { fGraph->dGraphAllScore, double(fGraph->iGraphAmountDisc),
                  // fGraph->dGraphAllScore,
                  // double(fGraph->iGraphAmountDisc),
                  fGraph->dMinDiscScore, fGraph->dMaxDiscScore, fGraph->dMinRib,
                  fGraph->dMaxRib, fGraph->dDiametrLen, fGraph->dDiametrStep,
                  double(fGraph->iComponent), fGraph->dMinSpanTree,
                  fGraph->dMaxSpanTree };

    for (int iTag = 0; iTag < ETagDisc::ETD_Size; ++iTag)
    {
        if (fGraph->mapGraphAmountTagDisc.count(ETagDisc(iTag)))
            arrResult.push_back(double(
                fGraph->mapGraphAmountTagDisc.find(ETagDisc(iTag))->second));
        else
        {
            arrResult.push_back(0);    // Нет такого вида дисциплин
        }
    }

    for (int y = 0; y < arrMetricHead.size(); ++y)
    {
        if (ptrGlobal->ptrConfig->mapArrOutParams.count(arrMetricHead[y]))
        {
            if (ptrGlobal->ptrConfig->mapArrOutParams[arrMetricHead[y]].at(
                    eOutType) == L"да")
            {
                arrReturnDataMetrics.push_back(arrResult[y]);
            }
        }
    }

    wstring wsNameSoMachComp =
        L"Количество дисциплин с несколькими дисциплинами";
    if (ptrGlobal->ptrConfig->mapArrOutParams[wsNameSoMachComp].at(eOutType) ==
        L"да")
    {
        // Так как дисциплин с 0-компетенциями исключаются
        for (int iAmountComp = 1;
             iAmountComp <= this->ptrGlobal->ptrConfig->iSoMachComp;
             ++iAmountComp)
        {
            arrReturnDataMetrics.push_back(
                fGraph->arrAmountCountCompDisc[iAmountComp]);
        }
    }
}

void FOutData::CreateTotalInfo(vector<string>&   arrReturnDataHeader,
                               const FGraphType* fGraph,
                               EOutType          eOutType)
{
    arrReturnDataHeader.clear();

    for (int y = 0; y < arrMetricHead.size(); ++y)
    {
        if (ptrGlobal->ptrConfig->mapArrOutParams.count(arrMetricHead[y]))
        {
            if (ptrGlobal->ptrConfig->mapArrOutParams[arrMetricHead[y]].at(
                    eOutType) == L"да")
            {
                arrReturnDataHeader.push_back(ptrGlobal->ConwertToString(
                    ptrGlobal->ptrConfig->mapArrOutParams[arrMetricHead[y]].at(
                        0)));
            }
        }
    }

    wstring wsNameSoMachComp =
        L"Количество дисциплин с несколькими дисциплинами";
    if (ptrGlobal->ptrConfig->mapArrOutParams[wsNameSoMachComp].at(eOutType) ==
        L"да")
    {
        for (int iAmountComp = 1;
             iAmountComp <= this->ptrGlobal->ptrConfig->iSoMachComp;
             ++iAmountComp)
        {
            string sAddedHead = ptrGlobal->ConwertToString(
                ptrGlobal->ptrConfig->mapArrOutParams[wsNameSoMachComp].at(0));

            sAddedHead += " ";
            if (iAmountComp == ptrGlobal->ptrConfig->iSoMachComp)
                sAddedHead += ">=";

            sAddedHead += to_string(iAmountComp) + " ";
            sAddedHead += ptrGlobal->ConwertToString(
                ptrGlobal->ptrConfig->wsOutSufAmountComp);

            arrReturnDataHeader.push_back(sAddedHead);
        }
    }
}

void FOutData::CreateTotalInfo(vector<vector<string>>& arrReturnData,
                               const FGraphType*       fGraph,
                               EOutType                eOutType)
{
    arrReturnData.clear();

    vector<double> arrResult;    // Соответствует arrHead который
                                 // проинициализирован в конструкторе
    CreateTotalInfo(arrResult, fGraph, eOutType);

    vector<string> arrHeader;
    CreateTotalInfo(arrHeader, fGraph, eOutType);

    arrReturnData.clear();
    arrReturnData.assign(min(arrResult.size(), arrHeader.size()),
                         vector<string>(2));
    for (int y = 0; y < arrReturnData.size(); ++y)
    {
        arrReturnData[y][0] = arrHeader[y];
        arrReturnData[y][1] = to_string(arrResult[y]);
    }
}

bool FOutData::Init() { return true; }

void FOutData::Out(string sOutPath)
{
    OpenXLSX::XLDocument fOutFile;
    fOutFile.create(sOutPath + "/TotalData.xlsx");
    fOutFile.workbook().addWorksheet("Total Data");
    fOutFile.workbook().deleteSheet("Sheet1");    // Стартовая страница не нужна
    OpenXLSX::XLWorksheet wks = fOutFile.workbook().worksheet("Total Data");

    int i = 0;    // Задаём порядок вывода
    int x = 0;
    int y = 0;

    vector<wstring> arrAddedHead;

    // Параметры для всего УП целиком
    vector<wstring> arrPrefixHead = { L"Название учебного плана",
                                      L"Общее кол-во ЗЕ в УП",
                                      L"Общее кол-во дисциплин в УП"

    };

    for (auto& wsNameTag : ptrGlobal->ptrConfig->arrNameTagDisc)
    {
        arrPrefixHead.push_back(L"!" + wsNameTag);
    }

    arrAddedHead.insert(arrAddedHead.end(),
                        arrPrefixHead.begin(),
                        arrPrefixHead.end());

    arrAddedHead.insert(arrAddedHead.end(),
                        arrMetricHead.begin(),
                        arrMetricHead.end());

    wstring wsNameSoMachComp =
        L"Количество дисциплин с несколькими дисциплинами";

    for (int iAmountComp = 1;
         iAmountComp <= this->ptrGlobal->ptrConfig->iSoMachComp;
         ++iAmountComp)
    {
        arrAddedHead.push_back(wsNameSoMachComp);
    }

    for (auto& sHeaderComp : ptrGlobal->ptrSolve->setHeaderComp)
    {
        wstring wsCompScore = ptrGlobal->ConwertToWstring(sHeaderComp);
        wsCompScore =
            ptrGlobal->ptrConfig->mapArrOutParams[L"ЗЕ у компетенции"].at(0) +
            wsCompScore;
        arrAddedHead.push_back(wsCompScore);
        arrAddedHead.push_back(ptrGlobal->ConwertToWstring(sHeaderComp));
    }

    // Вывод заголовка таблицы
    arrOutColm.assign(arrAddedHead.size() + 1,
                      true);    // Так как нумерация с 1, поэтому и +1

    // Вывод заголовка (на главной странице)
    y = 1, x = 1, i = 1;

#pragma region OutHeader

    int iAmountComp = 1;    // Так как предметы с 0-компетенциями исключаются
    for (const auto& it : arrAddedHead)
    {
        if (ptrGlobal->ptrConfig->mapArrOutParams.count(it))
        {
            arrOutColm[i] = (ptrGlobal->ptrConfig->mapArrOutParams[it].at(
                                 EOutType::EOT_Head) == L"да");
            if (arrOutColm[i++])
            {
                string sOut = ptrGlobal->ConwertToString(
                    ptrGlobal->ptrConfig->mapArrOutParams[it].at(0));
                if (it == L"Количество дисциплин с несколькими дисциплинами")
                {
                    sOut += " ";
                    if (iAmountComp == ptrGlobal->ptrConfig->iSoMachComp)
                        sOut += ">=";

                    sOut += to_string(iAmountComp++) + " ";
                    sOut += ptrGlobal->ConwertToString(
                        ptrGlobal->ptrConfig->wsOutSufAmountComp);
                }

                wks.cell(y, x++).value() = sOut;
            }
        }
        else
        {
            arrOutColm[i] = true;
            if (it.find(
                    ptrGlobal->ptrConfig->mapArrOutParams[L"ЗЕ у компетенции"]
                        .at(0)) != wstring::npos)
            {
                if (ptrGlobal->ptrConfig->mapArrOutParams[L"ЗЕ у компетенции"]
                        .at(EOutType::EOT_Head) != L"да")
                    arrOutColm[i] = false;
            }
            if (arrOutColm[i++])
            {
                wks.cell(y, x++).value() = ptrGlobal->ConwertToString(it);
            }
        }
    }
#pragma endregion

    // Вывод данных
    int iXShift = 0;
    int iYShift = 1;
    y           = 1;
    for (const auto& it : ptrGlobal->ptrSolve->arrDisc)
    {
        // Выводить короткое, или помное имя
        string sOutName = (ptrGlobal->ptrConfig->bOutShortNameCurr)
                              ? it->sShortNamePlan
                              : it->sNamePlan;

        i = 1;
        x = 1;

        // double dSumScoreExt   = 0;
        // int    iAmountDiscExt = 0;
        // it->dFindAllScore(dSumScoreExt, iAmountDiscExt);
        vector<double> arrResult;
        CreateTotalInfo(arrResult,
                        &it->ptrGraph->mapGraph[FGraph::iCommon],
                        EOutType::EOT_Head);

        vector<double> arrCurriculaAllResult;

        // Вывод названия УП
        OutData(x, i, y, sOutName.size(), sOutName, wks, sOutName, false,
                iXShift, iYShift);

        // Общие для УП метрики (перечислены в arrPrefixHead)
        arrCurriculaAllResult.push_back(it->dAllSumScore);
        arrCurriculaAllResult.push_back(it->iAmountDisc);

        // Дисциплин основных, по выбору, факультативов и т.д во всём УП
        {
            int iNumberTag = -1;
            for (auto& wsNameTag : ptrGlobal->ptrConfig->arrNameTagDisc)
            {
                ++iNumberTag;
                arrCurriculaAllResult.push_back(
                    it->mapAmountTagDisc[ETagDisc(iNumberTag)]);
            }
        }

        // Теперь это считается в CreateTotalInfo
        /*{
            int iNumberTag = -1;
            for (auto& wsNameTag : ptrGlobal->ptrConfig->arrNameTagDisc)
            {
                ++iNumberTag;
                auto ptrTreeMetric = it->ptrGraph->mapGraph[FGraph::iCommon];

                arrResult.push_back(
                    ptrTreeMetric.mapGraphAmountTagDisc[ETagDisc(iNumberTag)]
                    );
            }
        }*/

        arrCurriculaAllResult.insert(arrCurriculaAllResult.end(),
                                     arrResult.begin(),
                                     arrResult.end());

        for (const auto& dValue : arrCurriculaAllResult)
        {
            OutData(x, i, y, dValue, sOutName, wks, to_string(dValue), true,
                    iXShift, iYShift);
        }

        for (auto& sHeaderComp : ptrGlobal->ptrSolve->setHeaderComp)
        {
            if (it->ptrMetric)
            {
                auto ptrTreeMetric =
                    it->ptrMetric->ptrTreeMetric->mapChild[FMetric::sAllMetric];
                double dScore = 0.;
                double dRes   = 0.;
                if (ptrTreeMetric->mapChild.count(sHeaderComp))
                {
                    dScore =
                        (ptrGlobal->ptrConfig->bCompInterDelete)
                            ? ptrTreeMetric->mapChild[sHeaderComp]->dBalanceSum
                            : ptrTreeMetric->mapChild[sHeaderComp]
                                  ->dNoBalanceSum;

                    dRes = (ptrGlobal->ptrConfig->bCompInterDelete)
                               ? dScore / ptrTreeMetric->dBalanceSum
                               : dScore / it->dAllSumScore;
                }
                else
                {
                    dScore = 0.;
                    dRes   = 0.;
                }

                // Если ЗЕ равно 0, то и не выводить
                if (dScore > 0)
                {
                    OutData(x, i, y, dScore, sOutName, wks, to_string(dScore),
                            true, iXShift, iYShift);
                }
                else
                {
                    OutData(x, i, y, 0, sOutName, wks, "-", false, iXShift,
                            iYShift);
                }

                if (dRes > ptrGlobal->ptrConfig->dMinComp)
                {
                    OutData(x, i, y, dRes * 100, sOutName, wks,
                            to_string(dRes * 100) + "%", true, iXShift,
                            iYShift);
                }
                else
                {
                    OutData(x, i, y, 0, sOutName, wks, "-", false, iXShift,
                            iYShift);
                }
            }
            else
            {
                x++;    // В холостую пропускаем столбцы, они не будут заполнены
                i++;
            }
        }

        ++y;    // Перевод строки
    }

    iYShift += y;
    //  Вывод коридора минимума максимума
    {
        if (ptrGlobal->ptrConfig->mapArrOutParams.count(
                L"Максимальные значения:"))
        {
            if (ptrGlobal->ptrConfig->mapArrOutParams[L"Максимальные значения:"]
                    .at(1) == L"да")
            {
                wks.cell(iYShift, 1 + iXShift).value() =
                    ptrGlobal->ConwertToString(
                        ptrGlobal->ptrConfig
                            ->mapArrOutParams[L"Максимальные значения:"]
                            .at(0));
                for (auto& [id, fСorridor] : mapSaveData)
                {
                    wks.cell(iYShift, id + iXShift).value() =
                        to_string(fСorridor.dMax) + " (" +
                        ptrGlobal->ConwertToString(
                            ptrGlobal->ptrConfig->wsOutPrefMinMax) +
                        fСorridor.sMax + ")";
                }
                ++iYShift;
            }
        }
        if (ptrGlobal->ptrConfig->mapArrOutParams.count(
                L"Минимальные значения:"))
        {
            if (ptrGlobal->ptrConfig->mapArrOutParams[L"Минимальные значения:"]
                    .at(1) == L"да")
            {
                wks.cell(iYShift, 1 + iXShift).value() =
                    ptrGlobal->ConwertToString(
                        ptrGlobal->ptrConfig
                            ->mapArrOutParams[L"Минимальные значения:"]
                            .at(0));

                for (auto& [id, fСorridor] : mapSaveData)
                {
                    wks.cell(iYShift, id + iXShift).value() =
                        to_string(fСorridor.dMin) + " (" +
                        ptrGlobal->ConwertToString(
                            ptrGlobal->ptrConfig->wsOutPrefMinMax) +
                        fСorridor.sMin + ")";
                }
                ++iYShift;
                ;
            }
        }
    }

    fOutFile.save();
    fOutFile.close();

    for (auto& it : ptrGlobal->ptrSolve->arrDisc)
    {
        // Выводить короткое, или помное имя
        string sOutName = (ptrGlobal->ptrConfig->bOutShortNameCurr)
                              ? it->sShortNamePlan
                              : it->sNamePlan;

        string sNewFile = sOutPath + "/" + sOutName;

        if (!filesystem::exists(sNewFile))
        {
            try
            {
                filesystem::create_directories(sNewFile);
            }
            catch (...)
            {
                ptrGlobal->ptrError->ErrorOutFileCreate(sNewFile);
                continue;    // Продолжаем работать, но с другим файлом
            }
        }

        OutGephiData(sOutName, sOutPath, it);
        CreateAndTake(sOutName, sOutPath);
        OutAddInfo(sOutName, sOutPath, it);
    }
}

void FOutData::OutAddInfo(string sName, string sPath, FTreeDisc* ptrTree)
{
    // int x = 1;
    // int y = 1;
    //  Теперь это выводится в OutAddInfoInit

    // vector<wstring> arrHead = { L"За какой курс",
    //                             L"Заголовок компетенции",
    //                             L"Процент распределения Заголовка
    //                             компетенции", L"Компетенция", L"Процент
    //                             распределения Компетенции", L"Индекс",
    //                             L"Процент распределения Индекса" };
    //// Вывод заголовка
    // y = 1, x = 1;
    // for (const auto& it : arrHead)
    //{
    //     if (ptrTree->ptrGlobal->ptrConfig->mapAddOutParams.count(it))
    //     {
    //         arrSinglOpenWKS.back().cell(1, x++).value() =
    //             ptrTree->ptrGlobal->ConwertToString(ptrTree->ptrGlobal->ptrConfig->mapAddOutParams[it]);
    //     }
    // }
    // y         = 2;
    // iXShift   = 1;
    // int iOldX = 1;
    // iXShift += arrHead.size();
    // iYShift               = 1;
    auto& ptrCurrentTree =
        ptrTree->ptrMetric->ptrTreeMetric->mapChild[FMetric::sAllMetric];
    double dAllSum = 1.;
    dAllSum        = (ptrGlobal->ptrConfig->bCompInterDelete)
                         ? ptrCurrentTree->dBalanceSum
                         : ptrCurrentTree->dNoBalanceSum;

    // OutRectAddInfo(iOldX, y, ptrCurrentTree, true, dAllSum);
    vector<vector<string>> arrDataAll;

    // OutRectAddInfo(arrData, 0, 0, ptrCurrentTree, true, dAllSum); //
    // Вывод FMetric::sAllMetric
    int iXShift = 1;
    CreateTableInfoInit(arrDataAll, ptrCurrentTree, true, dAllSum);
    OutTableInfo(iXShift, 1, arrDataAll, arrSinglOpenWKS.back());
    iXShift +=
        arrDataAll.front().size();    // Сдвигаемся на ширину выведеной таблицы
    int iYShift = arrDataAll.size();
    for (auto& [sKey, ptrCurrentTree] :
         ptrTree->ptrMetric->ptrTreeMetric->mapChild)
    {
        if (sKey == FMetric::sAllMetric) continue;
        // Теперь это выводится в OutAddInfoInit
        /*for (const auto& it : arrHead)
        {
            if (ptrTree->ptrGlobal->ptrConfig->mapAddOutParams.count(it))
            {
                arrSinglOpenWKS.back().cell(1, x++).value() =
                    ptrTree->ptrGlobal->ConwertToString(ptrTree->ptrGlobal->ptrConfig->mapAddOutParams[it]);
            }
        }
        iOldX = iXShift;
        iXShift += arrHead.size();*/

        dAllSum = (ptrGlobal->ptrConfig->bCompInterDelete)
                      ? ptrCurrentTree->dBalanceSum
                      : ptrCurrentTree->dNoBalanceSum;

        vector<vector<string>> arrDataCourse;
        // Вывод конкретного курса
        // OutRectAddInfo(iOldX, y, ptrCurrentTree, true, dAllSum);
        CreateTableInfoInit(arrDataCourse, ptrCurrentTree, true, dAllSum);
        OutTableInfo(iXShift, 1, arrDataCourse, arrSinglOpenWKS.back());
        iXShift += arrDataAll.front()
                       .size();    // Сдвигаемся на ширину выведеной таблицы
        iYShift = max(iYShift + 0ull, arrDataCourse.size() + 0ull);
        {
            arrSinglLocalCurrentCourseOpenFile.clear();
            arrSinglLocalCurrentCourseOpenFile.resize(1);
            arrSinglLocalCurrentCourseOpenWKS.clear();
            arrSinglLocalCurrentCourseOpenWKS.resize(1);
            arrSinglLocalCurrentCourseOpenFile[0].create(
                sPath + "/" + sName + "/" + "Data_" + sKey + ".xlsx");
            arrSinglLocalCurrentCourseOpenFile[0].workbook().addWorksheet(
                sName);
            arrSinglLocalCurrentCourseOpenFile[0].workbook().deleteSheet(
                "Sheet1");    // Стартовая страница не нужна
            arrSinglLocalCurrentCourseOpenFile[0].save();
            arrSinglLocalCurrentCourseOpenWKS[0] =
                arrSinglLocalCurrentCourseOpenFile[0].workbook().worksheet(
                    sName);
            // Теперь это выводится в OutAddInfoInit
            // int iLocalX = 1;
            // for (const auto& it : arrHead)
            //{
            //    if (ptrTree->ptrGlobal->ptrConfig->mapAddOutParams.count(it))
            //    {
            //        arrSinglLocalCurrentCourseOpenWKS.back().cell(1,
            // iLocalX++).value() =
            //            ptrTree->ptrGlobal->ConwertToString(ptrTree->ptrGlobal->ptrConfig->mapAddOutParams[it]);
            //    }
            //}
            // Вывод в отдельные файлы (уже делал расчёт раньше, он в
            // arrDataCourse OutRectAddInfo(1, y, ptrCurrentTree, true, dAllSum,
            // true);

            // Выводим локально в отдельный файл
            OutTableInfo(1,
                         1,
                         arrDataCourse,
                         arrSinglLocalCurrentCourseOpenWKS.back());

            vector<vector<string>> arrTotalCourseGraphData;
            CreateTotalInfo(arrTotalCourseGraphData,
                            &ptrTree->ptrGraph->mapGraph[sKey[0] - '1'],
                            EOutType::EOT_Added);

            OutTableInfo(1,    // Так как 1-индексация
                         arrDataCourse.size() + 1,    // Так как 1-индексация
                         arrTotalCourseGraphData,
                         arrSinglLocalCurrentCourseOpenWKS.back());

            arrSinglLocalCurrentCourseOpenFile.back().save();
            arrSinglLocalCurrentCourseOpenFile.back().close();

            // Очищаем память от использованных объектов
            arrSinglLocalCurrentCourseOpenFile.clear();
            arrSinglLocalCurrentCourseOpenWKS.clear();
        }
    }

    vector<vector<string>> arrTotalGraphData;
    CreateTotalInfo(arrTotalGraphData,
                    &ptrTree->ptrGraph->mapGraph[FGraph::iCommon],
                    EOutType::EOT_Added);
    OutTableInfo(1, iYShift + 1,    // Так как 1-индексация
                 arrTotalGraphData, arrSinglOpenWKS.back());

    arrSinglOpenFile.back().save();
    arrSinglOpenFile.back().close();

    // Очищаем память от использованных объектов
    arrSinglOpenFile.clear();
    arrSinglOpenWKS.clear();
}

void FOutData::OutTableInfo(const int&                    iShiftX,
                            const int&                    iShiftY,
                            const vector<vector<string>>& arrData,
                            OpenXLSX::XLWorksheet&        WKS)
{
    for (int y = 0; y < arrData.size(); ++y)
    {
        for (int x = 0; x < arrData[y].size(); ++x)
        {
            WKS.cell(y + iShiftY, x + iShiftX).value() = arrData[y][x];
        }
    }
}

void FOutData::CreateTableInfoInit(vector<vector<string>>& arrReturnData,
                                   FTreeMetric*            ptrMetric,
                                   bool                    bIsCourse,
                                   const double            dAllSum,
                                   bool                    bIsLocal)
{
    int iSizeX = 0, iSizeY = 1;    // iSizeY = 1 из-за заголовка
    arrReturnData.clear();
    CountRectArraySize(iSizeX, iSizeY, 0, 0, ptrMetric);
    arrReturnData.assign(iSizeY, vector<string>(iSizeX));

    // Вывод заголовка
    int x = 0;
    for (const auto& it : arrCompetenceHead)
    {
        if (x >= iSizeX) break;
        if (this->ptrGlobal->ptrConfig->mapAddOutParams.count(it))
        {
            arrReturnData[0][x++] = this->ptrGlobal->ConwertToString(
                this->ptrGlobal->ptrConfig->mapAddOutParams[it]);
        }
    }

    int iCurrentY = 1;
    CreateTableRectInfo(arrReturnData, 0, iSizeX,
                        iCurrentY,    // 0-строка под заголовок
                        ptrMetric, bIsCourse, dAllSum, bIsLocal);
}

void FOutData::CountRectArraySize(int& iSizeX, int& iSizeY, int x, int y,
                                  FTreeMetric* ptrMetric)
{
    if (x > iSizeX) iSizeX = x + 1;    // Ищем максимум, чтобы отмерить ширину

    if (ptrMetric->mapChild.size() == 1)
        if (ptrMetric->mapChild.begin()->first == FMetric::sEmptyIndicator)
        {
            ++iSizeY;
            return;    // Нет индикаторов, значит и выводить не надо
        }

    // отступ, поэтому увеличиваем iSizeY
    if (ptrMetric->mapChild.size() == 0)
    {
        ++iSizeY;
        return;
    }

    for (auto& [sName, ptrChild] : ptrMetric->mapChild)
    {
        CountRectArraySize(iSizeX, iSizeY, x + 2, y, ptrChild);
    }
    return;
}

void FOutData::CreateTableRectInfo(
    vector<vector<string>>& arrReturnData,    // Возвращаемое значение с функции
    int x, const int& iSizeX, int& iCurrentY, FTreeMetric* ptrMetric,
    bool bIsCourse, const double dAllSum, bool bIsLocal)
{
    // Достигнут лимит глубины вывода
    if (x >= iSizeX)
    {
        ++iCurrentY;
        return;
    }

    if (ptrMetric->sName == FMetric::sEmptyIndicator)
    {
        ++iCurrentY;
        return;
    }

    arrReturnData[iCurrentY][x] = ptrMetric->sName;
    // Выводим процент распределения
    if (!bIsCourse)
    {
        ++x;
        double dRes = 0.;
        if (ptrGlobal->ptrConfig->bIsPercentRegAll)
        {
            dRes = (ptrGlobal->ptrConfig->bCompInterDelete)
                       ? ptrMetric->dBalanceSum / dAllSum
                       : ptrMetric->dNoBalanceSum / dAllSum;
        }
        else
        {
            dRes =
                (ptrGlobal->ptrConfig->bCompInterDelete)
                    ? ptrMetric->dBalanceSum / ptrMetric->ptrParent->dBalanceSum
                    : ptrMetric->dNoBalanceSum /
                          ptrMetric->ptrParent->dNoBalanceSum;
        }

        arrReturnData[iCurrentY][x] = to_string(dRes);
    }

    if (ptrMetric->mapChild.size() == 0)
    {
        ++iCurrentY;
        return;
    }

    for (auto& [sName, ptrChild] : ptrMetric->mapChild)
    {
        CreateTableRectInfo(arrReturnData, x + 1, iSizeX, iCurrentY, ptrChild,
                            false, dAllSum, bIsLocal);
    }
}

//
// int FOutData::OutRectAddInfo(int x, int y, FTreeMetric* ptrMetric, bool
// bIsCourse, const double dAllSum, bool bIsLocal)
//{
//    // Достигнут лимит глубины вывода
//    if (x >= iXShift)
//    {
//        return y + 1;
//    }
//
//    if (ptrMetric->sName == FMetric::sEmptyIndicator)
//    {
//        return y + 1;
//    }
//    if (bIsLocal)
//    {
//        arrSinglLocalCurrentCourseOpenWKS.back().cell(y, x).value() =
//        ptrMetric->sName;
//    }
//    else
//    {
//        arrSinglOpenWKS.back().cell(y, x).value() = ptrMetric->sName;
//    }
//    // Выводим процент распределения
//    if (!bIsCourse)
//    {
//        ++x;
//        double dRes = 0.;
//        if (ptrGlobal->ptrConfig->bIsPercentRegAll)
//        {
//            dRes = (ptrGlobal->ptrConfig->bCompInterDelete) ?
//            ptrMetric->dBalanceSum / dAllSum : ptrMetric->dNoBalanceSum /
//            dAllSum;
//        }
//        else
//        {
//            dRes = (ptrGlobal->ptrConfig->bCompInterDelete) ?
//            ptrMetric->dBalanceSum / ptrMetric->ptrParent->dBalanceSum
//                                                            :
//                                                            ptrMetric->dNoBalanceSum
//                                                            /
//                                                            ptrMetric->ptrParent->dNoBalanceSum;
//        }
//        if (bIsLocal)
//        {
//            arrSinglLocalCurrentCourseOpenWKS.back().cell(y, x).value() =
//            dRes;
//        }
//        else
//        {
//            arrSinglOpenWKS.back().cell(y, x).value() = dRes;
//        }
//    }
//
//    if (y > iYShift) iYShift = y;    // Ищем максимум, чтобы отмерить отступ
//
//    if (ptrMetric->mapChild.size() == 0)
//    {
//        return y + 1;
//    }
//
//    for (auto& [sName, ptrChild] : ptrMetric->mapChild)
//    {
//        y = OutRectAddInfo(x + 1, y, ptrChild, false, dAllSum, bIsLocal);
//    }
//
//    return y;
//}

void FOutData::CreateAndTake(string sName, string sPath)
{
    arrSinglOpenFile.clear();
    arrSinglOpenFile.resize(1);

    arrSinglOpenWKS.clear();
    arrSinglOpenWKS.resize(1);

    if (ptrGlobal->ptrConfig->bCompactOutput)
    {
        arrSinglOpenFile[0].open(sPath + "/TotalData.xlsx");
        if (arrSinglOpenFile[0].workbook().worksheetExists(sName))
            arrSinglOpenFile[0].workbook().deleteSheet(sName);

        arrSinglOpenFile[0].workbook().addWorksheet(sName);
        arrSinglOpenWKS[0] = arrSinglOpenFile[0].workbook().worksheet(sName);
        return;
    }
    else
    {
        arrSinglOpenFile[0].create(sPath + "/" + sName + "/" + "Data.xlsx");
        arrSinglOpenFile[0].workbook().addWorksheet(sName);
        arrSinglOpenFile[0].workbook().deleteSheet(
            "Sheet1");    // Стартовая страница не нужна
        arrSinglOpenFile[0].save();
        arrSinglOpenWKS[0] = arrSinglOpenFile[0].workbook().worksheet(sName);
    }
}

string FOutData::AddCompString(const map<string, vector<string>>& mapComp)
{
    string sReturn = "(";
    int    j       = -1;
    for (const auto& [sNameComp, arrIndicator] : mapComp)
    {
        ++j;
        if (j) sReturn += ",";
        sReturn += sNameComp;
    }
    sReturn += ")";
    return sReturn;
}

vector<string> FOutData::CreateCommonNameLabel(const int& iGraphType,
                                               FTreeDisc* fTree)
{
    vector<string> arrCommonNameLabel;
    for (auto& it : fTree->ptrGraph->mapGraph[iGraphType].arrRel)
    {
        FTreeElement* fThis = fTree->mapDisc[it.first];
        // wstring       wsNameRaw = fThis->wsName;
        string sName = fThis->sName;

        // Не забываем про нуль нумерацию курсов
        if (iGraphType == FGraph::iAlt)
        {
            sName += ptrGlobal->ptrConfig->sPrefCourseNumber +
                     to_string(it.second + 1);
        }

        // ptrGlobal->ReversUTF16RU(ptrGlobal->ConwertToString(wsNameRaw));
        // Выводим ещё и компетенции
        if (ptrGlobal->ptrConfig->bOutCompWithName)
        {
            sName += AddCompString(fThis->mapComp);
        }
        arrCommonNameLabel.push_back(sName);
    }
    return arrCommonNameLabel;
}

void FOutData::OutGephiData(string sName, string sPath, FTreeDisc* fTree)
{
    {
        // Выводим обычный граф
        // vector<string> arrCommonNameLabel;
        // for (auto& it : fTree->ptrGraph->mapGraph[FGraph::iCommon].arrRel)
        //{
        //    FTreeElement* fThis     = fTree->mapDisc[it.first];
        //    wstring       wsNameRaw = fThis->wsName;
        //    string        sName     =
        //    ptrGlobal->ReversUTF16RU(ptrGlobal->ConwertToString(wsNameRaw));
        //    // Выводим ещё и компетенции
        //    if (ptrGlobal->ptrConfig->bOutCompWithName)
        //    {
        //        sName += AddCompString(fThis->mapComp);
        //    }
        //    arrCommonNameLabel.push_back(sName);
        //}
        // OutGephiLable(sName, sName, sPath, arrCommonNameLabel);
        OutGephiLable(sName, sName, sPath,
                      CreateCommonNameLabel(FGraph::iCommon, fTree));
        OutGephiRib(sName, sName, sPath,
                    fTree->ptrGraph->mapGraph[FGraph::iCommon].fAdjList);
    }

    {
        // Выводим альтернативный граф
        // vector<string> arrCommonNameLabel;
        // for (auto& it : fTree->ptrGraph->mapGraph[FGraph::iAlt].arrRel)
        //{
        //    FTreeElement* fThis     = fTree->mapDisc[it.first];
        //    wstring       wsNameRaw = fThis->wsName;
        //    string        sName     =
        //    ptrGlobal->ReversUTF16RU(ptrGlobal->ConwertToString(wsNameRaw));
        //    sName += ptrGlobal->ptrConfig->sPrefCourseNumber +
        //    to_string(it.second + 1);    // Не забываем про нуль нумерацию
        //    курсов
        //    // Выводим ещё и компетенции
        //    if (ptrGlobal->ptrConfig->bOutCompWithName)
        //    {
        //        sName += AddCompString(fThis->mapComp);
        //    }
        //    arrCommonNameLabel.push_back(sName);
        //}
        // OutGephiLable(sName, sName + ptrGlobal->ptrConfig->sSufAltGraphFile,
        // sPath, arrCommonNameLabel);
        OutGephiLable(sName, sName + ptrGlobal->ptrConfig->sSufAltGraphFile,
                      sPath, CreateCommonNameLabel(FGraph::iAlt, fTree));
        OutGephiRib(sName, sName + ptrGlobal->ptrConfig->sSufAltGraphFile,
                    sPath, fTree->ptrGraph->mapGraph[FGraph::iAlt].fAdjList);
    }

    if (fTree->ptrGlobal->ptrConfig->bCourseOutput)
    {
        for (int iCourse = 0; iCourse < fTree->iAmountCourse; ++iCourse)
        {
            OutGephiLable(sName, sName + "(" + to_string(iCourse + 1) + ")",
                          sPath, CreateCommonNameLabel(iCourse, fTree));
            OutGephiRib(sName, sName + "(" + to_string(iCourse + 1) + ")",
                        sPath, fTree->ptrGraph->mapGraph[iCourse].fAdjList);
        }
    }
}

void FOutData::OutGephiLable(const string& sName, const string& sNameFile,
                             const string&         sPath,
                             const vector<string>& arrNameLabel)
{
    ofstream outLabel(sPath + "/" + sName + "/" + sNameFile + "Lable.csv");

    outLabel << ptrGlobal->ptrConfig->sNameLableHeader << "\n";

    // Сначало id, потом имя
    int i = -1;
    for (auto& it : arrNameLabel)
    {
        ++i;
        outLabel << i << ";";
        outLabel << it << "";
        outLabel << "\n";
    }
}

void FOutData::OutGephiRib(const string& sName, const string& sNameFile,
                           const string&                            sPath,
                           const vector<vector<pair<int, double>>>& fAdjList)
{
    ofstream outLabel(sPath + "/" + sName + "/" + sNameFile + "Rib.csv");

    outLabel << ptrGlobal->ptrConfig->sNameRibHeader << "\n";

    // Откуда, куда, тип (неориентированный), Вес
    for (int l = 0; l < fAdjList.size(); ++l)
    {
        for (auto [r, dLen] : fAdjList[l])
        {
            // Чтобы не дублировать, он же неориентированный
            if ((l < r) || (!ptrGlobal->ptrConfig->bIsUnDirected))
            {
                outLabel << l << ";";
                outLabel << r << ";";
                outLabel << ptrGlobal->ptrConfig->sNameRibDir << ";";
                outLabel << dLen << "";
                outLabel << "\n";
            }
        }
    }
}

void FOutData::RetakeMinMax(FСorridor& fSaveData, const double& dNewData,
                            const string& sNewData)
{
    if ((dNewData > fSaveData.dMax) || (fSaveData.sMax == ""))
    {
        fSaveData.dMax = dNewData;
        fSaveData.sMax = sNewData;
    }

    if ((dNewData < fSaveData.dMin) || (fSaveData.sMin == ""))
    {
        fSaveData.dMin = dNewData;
        fSaveData.sMin = sNewData;
    }
    return;
}

void FOutData::OutData(int& x, int& index, const int& y, double dDate,
                       string sDate, OpenXLSX::XLWorksheet& wks,
                       string sOutData, const bool& bIsConsider,
                       const int& iXShift, const int& iYShift)
{
    if (arrOutColm[index++])
    {
        if (bIsConsider) RetakeMinMax(mapSaveData[x], dDate, sDate);

        wks.cell(iYShift + y, iXShift + x++).value() =
            ptrGlobal->ConwertUTF16RU(sOutData);
    }
}