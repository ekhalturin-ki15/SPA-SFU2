#include "outData.h"
#include "config.h"
#include "error.h"
#include "global.h"
#include "graph.h"
#include "metric.h"
#include "solve.h"

#include <iomanip>
#include <sstream>

FOutData::FOutData(FGlobal* _ptrGlobal)
    : ptrGlobal(_ptrGlobal)

      //arrCompetenceHead(
      //    { 
      //       L"Название УП", L"За какой курс",

      //      L"Заголовок компетенции", L"ЗЕ Заголовка компетенции",
      //      L"Кол-во дисциплин заголовка компетенции",

      //      L"Процент распределения Заголовка компетенции",

      //      L"Компетенция", L"ЗЕ Компетенций", L"Кол-во дисциплин компетенции",
      //      L"Процент распределения Компетенции",

      //      L"Индикатор", L"ЗЕ индикаторов", L"Кол-во дисциплин индикатора",
      //      L"Процент распределения Индикатора" }),

      //arrMetricHead(
      //    { // L"Название учебного плана",
      //      L"Всего ЗЕ в графе", L"Кол-во дисциплин в графе",
      //      // L"(Расш.) Общее кол-во ЗЕ в УП",
      //      // L"(Расш.) Кол-во дисциплин в УП",    // Включает те,
      //      // что указаны в
      //      //  УП, как неучитываемые
      //      L"Максимальное ЗЕ у дисциплины", L"Минимальное ЗЕ у дисциплины",
      //      L"Максимальный вес ребра", L"Минимальный вес ребра",
      //      L"Диаметр графа по расстоянию",
      //      L"Диаметр графа по количеству рёбер",
      //      L"Количество компонент связности", L"Максимальное оставное дерево",
      //      L"Минимальное оставное дерево", L"Плотность графа",
      //      L"Количество основных дисциплин", L"Количество дисциплин по выбору",
      //      L"Количество факультативов" })
{
}

void FOutData::CreateTotalInfo(vector<double>&   arrReturnDataMetrics,
                               const FGraphType* fGraph,
                               const EOutType&   eOutType)
{

    const vector<wstring> arrMetricHead(
        { L"Всего ЗЕ в графе", L"Кол-во дисциплин в графе",
          L"Максимальное ЗЕ у дисциплины", L"Минимальное ЗЕ у дисциплины",
          L"Максимальный вес ребра", L"Минимальный вес ребра",
          L"Диаметр графа по расстоянию", L"Диаметр графа по количеству рёбер",
          L"Количество компонент связности", L"Максимальное оставное дерево",
          L"Минимальное оставное дерево", L"Плотность графа",
          L"Количество основных дисциплин", L"Количество дисциплин по выбору",
          L"Количество факультативов" });

    arrReturnDataMetrics.clear();
    vector<double> arrResult;    // Соответствует arrHead который
                                 // проинициализирован в конструкторе
    arrResult = { fGraph->dGraphAllScore, double(fGraph->iGraphAmountDisc),
                  // fGraph->dGraphAllScore,
                  // double(fGraph->iGraphAmountDisc),
                  fGraph->dMaxDiscScore, fGraph->dMinDiscScore, fGraph->dMaxRib,
                  fGraph->dMinRib, fGraph->dDiametrLen, fGraph->dDiametrStep,
                  double(fGraph->iComponent), fGraph->dMaxSpanTree,
                  fGraph->dMinSpanTree, fGraph->dDense };

    for (int iType = 0; iType < ETypeDisc::ETD_Size; ++iType)
    {
        if (fGraph->mapGraphAmountTypeDisc.count(ETypeDisc(iType)))
            arrResult.push_back(double(
                fGraph->mapGraphAmountTypeDisc.find(ETypeDisc(iType))->second));
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
                               const EOutType&   eOutType)
{
    const vector<wstring> arrMetricHead(
        { L"Всего ЗЕ в графе", L"Кол-во дисциплин в графе",
          L"Максимальное ЗЕ у дисциплины", L"Минимальное ЗЕ у дисциплины",
          L"Максимальный вес ребра", L"Минимальный вес ребра",
          L"Диаметр графа по расстоянию", L"Диаметр графа по количеству рёбер",
          L"Количество компонент связности", L"Максимальное оставное дерево",
          L"Минимальное оставное дерево", L"Плотность графа",
          L"Количество основных дисциплин", L"Количество дисциплин по выбору",
          L"Количество факультативов" });

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
                ptrGlobal->ptrConfig
                    ->mapArrOutParams[L"Суффикс после вывода кол-во "
                                      L"компетенций у дисциплины"]
                    .at(0));
            // sAddedHead += ptrGlobal->ConwertToString(
            //     ptrGlobal->ptrConfig->wsOutSufAmountComp);

            arrReturnDataHeader.push_back(sAddedHead);
        }
    }
}

void FOutData::CreateTotalInfo(vector<vector<string>>& arrReturnData,
                               const FGraphType*       fGraph,
                               const EOutType&         eOutType)
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
        /*std::ostringstream fOut;
        fOut << std::setprecision(ptrGlobal->ptrConfig->iPrecision)
             << std::noshowpoint << arrResult[y];*/
        arrReturnData[y][1] = ptrGlobal->DoubletWithPrecision(arrResult[y]);
    }
}

bool FOutData::Init() { return true; }

void FOutData::CreateOnlyAllowedHeaderRow(vector<string>&        arrReturn,
                                          vector<bool>&          arrIsAllowed,
                                          const vector<wstring>& arrParams)
{
    arrReturn.clear();
    arrIsAllowed.clear();
    // arrIsAllowed.assign(arrParams.size(), true);

    int iAmountComp = 1;    // Так как предметы с 0-компетенциями исключаются
    int i = 0;
    for (const auto& it : arrParams)
    {
        if (ptrGlobal->ptrConfig->mapArrOutParams.count(it))
        {
            arrIsAllowed.push_back(
                (ptrGlobal->ptrConfig->mapArrOutParams[it].at(
                     EOutType::EOT_Head) ==
                 L"да"));    // Так как глобальная информация, то
                             // работаем с EOutType::EOT_Head
            if (arrIsAllowed.back())
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
                        ptrGlobal->ptrConfig
                            ->mapArrOutParams[L"Суффикс после вывода кол-во "
                                              L"компетенций у дисциплины"]
                            .at(0));

                    // sOut += ptrGlobal->ConwertToString(
                    //     ptrGlobal->ptrConfig->wsOutSufAmountComp);
                }

                arrReturn.push_back(sOut);
            }
        }
        else
        {
            arrIsAllowed.push_back(true);
            if (it.find(
                    ptrGlobal->ptrConfig->mapArrOutParams[L"ЗЕ у компетенции"]
                        .at(0)) != wstring::npos)
            {
                if (ptrGlobal->ptrConfig->mapArrOutParams[L"ЗЕ у компетенции"]
                        .at(EOutType::EOT_Head) != L"да")
                    arrIsAllowed.back() = false;
            }
            if (arrIsAllowed.back())
            {
                arrReturn.push_back(ptrGlobal->ConwertToString(it));
            }
        }
    }
}

void FOutData::CreateOnlyAllowedResultRow(vector<string>&       arrReturn,
                                          const int&            iSizeHeader,
                                          vector<bool>&         arrIsAllowed,
                                          const string&         sCurName,
                                          const vector<double>& arrResult,
                                          map<int, FСorridor>&  mapCorridorData)
{
    int i = 0;
    int x = 0;

    arrReturn.clear();

    arrReturn.resize(iSizeHeader);

    if (TakePasteData(x, arrReturn, arrIsAllowed[i++], sCurName.size(),
                      sCurName, sCurName, false, mapCorridorData))
    {
        x++;
    }

    for (const auto& it : arrResult)
    {
        if (it != FGraphType::dNoInit)
        {
            /*std::ostringstream fOut;
            fOut << std::setprecision(ptrGlobal->ptrConfig->iPrecision)
                 << std::noshowpoint << it;*/

            if (TakePasteData(x, arrReturn, arrIsAllowed[i++], it,
                              ptrGlobal->DoubletWithPrecision(it), sCurName,
                              true, mapCorridorData))
            {
                x++;
            }
        }
        else
        {
            if (TakePasteData(x, arrReturn, arrIsAllowed[i++], it, "-",
                              sCurName, false, mapCorridorData))
            {
                x++;
            }
        }
    }
}

void FOutData::AddTableMaxMinData(vector<vector<string>>& arrToAddedData,
                                  map<int, FСorridor>&    mapCorridorData)
{
    int i = -1;
    for (const auto& wsName : vector<wstring>(
             { L"Максимальные значения:", L"Минимальные значения:" }))
    {
        ++i;
        if (ptrGlobal->ptrConfig->mapArrOutParams.count(wsName))
        {
            if (ptrGlobal->ptrConfig->mapArrOutParams[wsName].at(1) == L"да")
            {
                vector<string> arrMinMaxData;

                arrMinMaxData.push_back(ptrGlobal->ConwertToString(
                    ptrGlobal->ptrConfig->mapArrOutParams[wsName].at(0)));

                for (auto& [id, fСorridor] : mapCorridorData)
                {
                    arrMinMaxData.push_back(
                        ptrGlobal->DoubletWithPrecision(fСorridor.dMaxMin[i]) +
                        " (" +
                        ptrGlobal->ConwertToString(
                            ptrGlobal->ptrConfig
                                ->mapArrOutParams[L"Предлог перед выводом "
                                                  L"результата мин. макс."]
                                .at(0)) +
                        fСorridor.sMaxMin[i] + ")");
                }

                arrToAddedData.push_back(arrMinMaxData);
            }
        }
    }
}

void FOutData::CreateAllCurriculaTotalData(
    vector<vector<string>>& arrReturnData)
{
    arrReturnData.clear();
    int i = 0;    // Задаём порядок вывода
    int x = 0;
    int y = 0;

    // Параметры для всего УП целиком
    vector<wstring> arrAddedHead = { L"Название учебного плана",
                                     L"Общее кол-во ЗЕ в УП",
                                     L"Общее кол-во дисциплин в УП"

    };

    for (auto& wsNameType : ptrGlobal->ptrConfig->arrNameTypeDisc)
    {
        arrAddedHead.push_back(L"!" + wsNameType);
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

    // Флаги того, какие столбцы выводить
    vector<bool> arrOutColm;

    // Строка заголовка таблицы
    vector<string> arrHeader;
    CreateOnlyAllowedHeaderRow(arrHeader, arrOutColm, arrAddedHead);
    arrReturnData.push_back(arrHeader);

    // Вывод данных
    map<int, FСorridor> mapSaveСorridorData;
    for (const auto& it : ptrGlobal->ptrSolve->arrDisc)
    {
        // Выводить короткое, или помное имя
        sCurPlanName = (ptrGlobal->ptrConfig->bOutShortNameCur)
                           ? it->sShortNamePlan
                           : it->sNamePlan;

#pragma region FormationData
        vector<double> arrAllResult;
        // Общие для УП метрики (перечислены в arrPrefixHead)
        arrAllResult.push_back(it->dAllSumScore);
        arrAllResult.push_back(it->iExtendedAmountDisc);

        // Дисциплин основных, по выбору, факультативов и т.д во всём УП
        {
            int iNumberType = -1;
            for (auto& wsNameType : ptrGlobal->ptrConfig->arrNameTypeDisc)
            {
                ++iNumberType;
                arrAllResult.push_back(
                    it->mapAmountTypeDisc[ETypeDisc(iNumberType)]);
            }
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
                    dScore = ptrTreeMetric->mapChild[sHeaderComp]->dChosenSum;
                    dRes =
                        ptrTreeMetric->mapChild[sHeaderComp]->dInclusionPercent;
                }
                else
                {
                    dScore = 0.;
                    dRes   = 0.;
                }

                // Если ЗЕ равно 0, то и не выводить
                if (dScore > 0)
                {
                    arrAllResult.push_back(dScore);
                }
                else
                {
                    arrAllResult.push_back(FGraphType::dNoInit);
                }

                if (dRes > ptrGlobal->ptrConfig->dMinComp)
                {
                    arrAllResult.push_back(dRes * 100);
                }
                else
                {
                    arrAllResult.push_back(FGraphType::dNoInit);
                }
            }
        }
#pragma endregion

        vector<string> arrCurData;
        CreateOnlyAllowedResultRow(arrCurData, arrHeader.size(), arrOutColm,
                                   sCurPlanName, arrAllResult,
                                   mapSaveСorridorData);

        arrReturnData.push_back(arrCurData);
    }

    //  Вывод коридора минимума максимума
    if (ptrGlobal->ptrConfig->bOutMaxMinData)
    {
        AddTableMaxMinData(arrReturnData, mapSaveСorridorData);
    }
}

void FOutData::CreateSummaryTotalData(vector<vector<string>>& arrReturnData,
                                      const int&              iGraphType)
{
    const vector<wstring> arrMetricHead(
        { L"Всего ЗЕ в графе", L"Кол-во дисциплин в графе",
          L"Максимальное ЗЕ у дисциплины", L"Минимальное ЗЕ у дисциплины",
          L"Максимальный вес ребра", L"Минимальный вес ребра",
          L"Диаметр графа по расстоянию", L"Диаметр графа по количеству рёбер",
          L"Количество компонент связности", L"Максимальное оставное дерево",
          L"Минимальное оставное дерево", L"Плотность графа",
          L"Количество основных дисциплин", L"Количество дисциплин по выбору",
          L"Количество факультативов" });

    arrReturnData.clear();
    int i = 0;    // Задаём порядок вывода
    int x = 0;
    int y = 0;

    vector<wstring> arrAddedHead = { L"Название учебного плана" };

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

    // Флаги того, какие столбцы выводить
    vector<bool> arrOutColm;

    // Строка заголовка таблицы
    vector<string> arrHeader;
    CreateOnlyAllowedHeaderRow(arrHeader, arrOutColm, arrAddedHead);
    arrReturnData.push_back(arrHeader);

    // Вывод данных
    map<int, FСorridor> mapSaveСorridorData;

    for (const auto& it : ptrGlobal->ptrSolve->arrDisc)
    {
        // Нет нужного нам курса (например, если 3 курс, а план магистратуры)
        if (it->iAmountCourse <= iGraphType) continue;

        // Выводить короткое, или помное имя
        sCurPlanName = (ptrGlobal->ptrConfig->bOutShortNameCur)
                           ? it->sShortNamePlan
                           : it->sNamePlan;

        vector<double> arrAllResult;

        CreateTotalInfo(arrAllResult,
                        &it->ptrGraph->mapGraph[iGraphType],
                        EOutType::EOT_Head);

        vector<string> arrCurData;
        CreateOnlyAllowedResultRow(arrCurData, arrHeader.size(), arrOutColm,
                                   sCurPlanName, arrAllResult,
                                   mapSaveСorridorData);

        arrReturnData.push_back(arrCurData);
    }

    if (ptrGlobal->ptrConfig->bOutMaxMinData)
    {
        AddTableMaxMinData(arrReturnData, mapSaveСorridorData);
    }
}

void FOutData::Out(string sOutPath)
{
    OpenXLSX::XLDocument fOutFile;
    fOutFile.create(sOutPath + "/TotalData.xlsx");
    fOutFile.workbook().addWorksheet("Total Data");
    fOutFile.workbook().deleteSheet("Sheet1");    // Стартовая страница не нужна
    OpenXLSX::XLWorksheet wks = fOutFile.workbook().worksheet("Total Data");

    int                    iShiftX = 1;
    vector<vector<string>> arrAllCurriculaTotalData;
    CreateAllCurriculaTotalData(arrAllCurriculaTotalData);
    OutTableInfo(iShiftX, 1, arrAllCurriculaTotalData, wks);
    iShiftX += arrAllCurriculaTotalData.front().size() - 1;
    if (ptrGlobal->ptrConfig->bArrIsconcatGraphData.at(0))
    {
        vector<vector<string>> arrAllCoursesGraphData;
        CreateSummaryTotalData(arrAllCoursesGraphData, FGraph::iCommon);
        OutTableInfo(
            iShiftX, 1, arrAllCoursesGraphData, wks,
            1);    // iShiftDataX = 1, так как заголовки УП выводить не надо
        iShiftX += arrAllCoursesGraphData.front().size() -
                   1;    // -1 так как без заголовка УП
    }
    else
    {
        string sNamePage = this->ptrGlobal->ConwertToString(
            ptrGlobal->ptrConfig
                ->mapArrOutParams
                    [L"Предлог перед выводом статистики по всем курсам"]
                .at(0));

        /*string sNamePage = this->ptrGlobal->ConwertToString(
            ptrGlobal->ptrConfig->wsOutPrefAllCourse);*/

        fOutFile.workbook().addWorksheet(sNamePage);
        wks = fOutFile.workbook().worksheet(sNamePage);
        vector<vector<string>> arrAllCoursesGraphData;
        CreateSummaryTotalData(arrAllCoursesGraphData, FGraph::iCommon);
        OutTableInfo(1, 1, arrAllCoursesGraphData, wks);
    }

    for (int iCourse = 0; iCourse < this->ptrGlobal->ptrSolve->iMaxCourse;
         ++iCourse)
    {
        if (ptrGlobal->ptrConfig->bArrIsconcatGraphData.at(1))
        {
            vector<vector<string>> arrCourseGraphData;
            CreateSummaryTotalData(arrCourseGraphData, iCourse);
            OutTableInfo(
                iShiftX, 1, arrCourseGraphData, wks,
                1);    // iShiftDataX = 1, так как заголовки УП выводить не надо

            iShiftX += arrCourseGraphData.front().size() -
                       1;    // -1 так как без заголовка УП
        }
        else
        {
            string sNamePage = ptrGlobal->ConwertToString(
                ptrGlobal->ptrConfig
                    ->mapArrOutParams
                        [L"Предлог перед выводом статистики по курсу "
                         L"определённого номера"]
                    .at(0));

            sNamePage = sNamePage + " " + to_string(iCourse + 1);
            fOutFile.workbook().addWorksheet(sNamePage);
            wks = fOutFile.workbook().worksheet(sNamePage);
            vector<vector<string>> arrCourseGraphData;
            CreateSummaryTotalData(arrCourseGraphData, iCourse);
            OutTableInfo(1, 1, arrCourseGraphData, wks);
        }
    }

    fOutFile.save();
    fOutFile.close();

    for (auto& it : ptrGlobal->ptrSolve->arrDisc)
    {
        // Выводить короткое, или помное имя
        sCurPlanName = (ptrGlobal->ptrConfig->bOutShortNameCur)
                           ? it->sShortNamePlan
                           : it->sNamePlan;

        string sNewFile = sOutPath + "/" + sCurPlanName;

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

        OutGephiData(sCurPlanName, sOutPath, it);
        CreateAndTake(sCurPlanName, sOutPath);
        OutAddInfo(sCurPlanName, sOutPath, it);
    }
}

void FOutData::OutAddInfo(string sName, string sPath, FTreeDisc* ptrTree)
{
    auto& ptrCurrentTree =
        ptrTree->ptrMetric->ptrTreeMetric->mapChild[FMetric::sAllMetric];
   
    vector<vector<string>> arrDataAll;

    int iXShift = 1;

    CreateTableInfoInit(arrDataAll,
                        ptrCurrentTree, true);    //, ptrCurrentTree->dChosenSum);

    OutTableInfo(iXShift, 1, arrDataAll, arrSinglOpenWKS.back());
    iXShift +=
        arrDataAll.front().size();    // Сдвигаемся на ширину выведеной таблицы
    int iYShift = arrDataAll.size();
    for (auto& [sKey, ptrCurrentTree] :
         ptrTree->ptrMetric->ptrTreeMetric->mapChild)
    {
        if (sKey == FMetric::sAllMetric) continue;

        vector<vector<string>> arrDataAllCourse;
        // Вывод конкретного курса
        CreateTableInfoInit(arrDataAllCourse,
                            ptrCurrentTree, false);    // ptrCurrentTree->dChosenSum);

        OutTableInfo(iXShift, 1, arrDataAllCourse, arrSinglOpenWKS.back());

        iXShift += arrDataAllCourse.front()
                       .size();    // Сдвигаемся на ширину выведеной таблицы
        iYShift = max(iYShift + 0ull, arrDataAllCourse.size() + 0ull);
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

            // Выводим локально в отдельный файл

            vector<vector<string>> arrDataAllCourseOnFile;
            // Вывод конкретного курса
            CreateTableInfoInit(arrDataAllCourseOnFile, ptrCurrentTree, true); 

            OutTableInfo(1,
                         1,
                         arrDataAllCourseOnFile,
                         arrSinglLocalCurrentCourseOpenWKS.back());

            if (ptrGlobal->ptrConfig->bOutTotalInfo)
            {
                vector<vector<string>> arrTotalCourseGraphData;
                CreateTotalInfo(arrTotalCourseGraphData,
                                &ptrTree->ptrGraph->mapGraph[sKey[0] - '1'],
                                EOutType::EOT_Added);

                OutTableInfo(1,    // Так как 1-индексация
                             arrDataAllCourse.size() +
                                 1,    // Так как 1-индексация
                             arrTotalCourseGraphData,
                             arrSinglLocalCurrentCourseOpenWKS.back());
            }

            arrSinglLocalCurrentCourseOpenFile.back().save();
            arrSinglLocalCurrentCourseOpenFile.back().close();

            // Очищаем память от использованных объектов
            arrSinglLocalCurrentCourseOpenFile.clear();
            arrSinglLocalCurrentCourseOpenWKS.clear();
        }
    }

    if (ptrGlobal->ptrConfig->bOutTotalInfo)
    {
        vector<vector<string>> arrTotalGraphData;
        CreateTotalInfo(arrTotalGraphData,
                        &ptrTree->ptrGraph->mapGraph[FGraph::iCommon],
                        EOutType::EOT_Added);
        OutTableInfo(1, iYShift + 1,    // Так как 1-индексация
                     arrTotalGraphData, arrSinglOpenWKS.back());
    }

    {
        // Новая визуализация
        map<string, map<string, map<string, string>>> mapGraphE1;
        for (auto& [sKey, ptrCurrentTree] :
             ptrTree->ptrMetric->ptrTreeMetric->mapChild)
        {
            if (sKey == FMetric::sAllMetric) continue;
            map<string, map<string, string>> fBuf;
            // Вывод конкретного курса
            CreateGraphE1TableInfoInit(fBuf, ptrCurrentTree);
            mapGraphE1[sKey] = fBuf;
        }

        int iSize = 1;
        for (const auto& [sComp, et] : mapGraphE1["1"])
        {
            iSize += et.size();
        }
        vector<vector<string>> arrData(iSize,
                                       vector<string>(mapGraphE1.size() + 2));

        {
            int y = 1;
            for (const auto& [sCourse, it] : mapGraphE1)
            {
                ++y;
                int x = -1;
                for (const auto& [sComp, et] : it)
                {
                    for (const auto& [sNum, sVal] : et)
                    {
                        ++x;
                        arrData[x][y] = sVal;
                        arrData[x][0] = sComp;
                        arrData[x][1] = sNum;
                    }
                }
                arrData[arrData.size() - 1][y] = sCourse;
            }
        }

        arrSinglLocalCurrentCourseOpenFile.clear();
        arrSinglLocalCurrentCourseOpenFile.resize(1);
        arrSinglLocalCurrentCourseOpenWKS.clear();
        arrSinglLocalCurrentCourseOpenWKS.resize(1);

        arrSinglLocalCurrentCourseOpenFile[0].create(sPath + "/" + sName + "/" +
                                                     "E1Diagram.xlsx");
        arrSinglLocalCurrentCourseOpenFile[0].workbook().addWorksheet(
            "E1Diagram");
        arrSinglLocalCurrentCourseOpenFile[0].workbook().deleteSheet(
            "Sheet1");    // Стартовая страница не нужна
        arrSinglLocalCurrentCourseOpenFile[0].save();
        arrSinglLocalCurrentCourseOpenWKS[0] =
            arrSinglLocalCurrentCourseOpenFile[0].workbook().worksheet(
                "E1Diagram");
        // Теперь это выводится в OutAddInfoInit

        // Выводим локально в отдельный файл
        OutTableInfo(1, 1, arrData, arrSinglLocalCurrentCourseOpenWKS.back());

        arrSinglLocalCurrentCourseOpenFile.back().save();
        arrSinglLocalCurrentCourseOpenFile.back().close();

        // Очищаем память от использованных объектов
        arrSinglLocalCurrentCourseOpenFile.clear();
        arrSinglLocalCurrentCourseOpenWKS.clear();
    }

    arrSinglOpenFile.back().save();
    arrSinglOpenFile.back().close();

    // Очищаем память от использованных объектов
    arrSinglOpenFile.clear();
    arrSinglOpenWKS.clear();
}

void FOutData::OutTableInfo(const int& iShiftX, const int& iShiftY,
                            const vector<vector<string>>& arrData,
                            OpenXLSX::XLWorksheet& WKS, int iShiftDataX,
                            int iShiftDataY)
{
    for (int y = iShiftDataY; y < arrData.size(); ++y)
    {
        for (int x = iShiftDataX; x < arrData[y].size(); ++x)
        {
            WKS.cell(y + iShiftY, x + iShiftX).value() = arrData[y][x];
        }
    }
}

void FOutData::CreateTableInfoInit(vector<vector<string>>& arrReturnData,
                                   FTreeMetric* ptrMetric, bool bIsOutNameCur
                                   // const double            dAllSum,
                                   // bool bIsLocal
)
{

    deque<wstring> arrCompetenceHead(
        { //L"Название УП",
        L"За какой курс",

          L"Заголовок компетенции", L"ЗЕ Заголовка компетенции",
          L"Кол-во дисциплин заголовка компетенции",

          L"Процент распределения Заголовка компетенции",

          L"Компетенция", L"ЗЕ Компетенций", L"Кол-во дисциплин компетенции",
          L"Процент распределения Компетенции",

          L"Индикатор", L"ЗЕ индикаторов", L"Кол-во дисциплин индикатора",
          L"Процент распределения Индикатора" });

    int iShiftX = 0;  

    if (bIsOutNameCur)
    {
        arrCompetenceHead.push_front(L"Название УП");
        ++iShiftX;    // ++ из-за вывода названия УП
    }

    int iSizeX = 0, iSizeY = 1;    // iSizeY = 1 из-за заголовка
    arrReturnData.clear();
    vector<vector<string>> arrAllData;

    CreateTableRectInfo(true,    // Считаем вхолостую
                        arrAllData, iShiftX,   
                        iSizeX,
                        iSizeY,    // 0-строка под заголовок
                        ptrMetric, 0    // dAllSum,
                                        // bIsLocal
    );

    arrAllData.assign(iSizeY, vector<string>(iSizeX));

    // Вывод заголовка
    {
        int x = -1;
        for (const auto& it : arrCompetenceHead)
        {
            ++x;
            if (x >= iSizeX) break;
            if (this->ptrGlobal->ptrConfig->mapAddOutParams.count(it))
            {
                arrAllData[0][x] = this->ptrGlobal->ConwertToString(
                    this->ptrGlobal->ptrConfig->mapAddOutParams[it].at(0));
            }
        }
    }

    int iCurrentY = 1;
    CreateTableRectInfo(false, arrAllData, iShiftX, iSizeX,
                        iCurrentY,    // 0-строка под заголовок
                        ptrMetric, 0    // dAllSum,
                                        // bIsLocal
    );

    // Теперь нужно оставить только те заголовки, которые требуется выводить

    // Вывод Только того, что требуется пользователю
    {
        for (int y = 0; y < arrAllData.size(); ++y)
        {
            int            x = -1;
            vector<string> arrBuf;

            bool           bIsTakeNoEmpty = false;
            for (const auto& it : arrCompetenceHead)
            {
                ++x;

                if (x >= iSizeX) break;
                if (this->ptrGlobal->ptrConfig->mapAddOutParams.count(it))
                {
                    if (ptrGlobal->ptrConfig->mapAddOutParams[it].at(1) ==
                        L"да")
                    {
                        if ((y == 1) && (it == L"Название УП"))
                            arrAllData[y][x] = sCurPlanName; // Если у нас самая верхняя правая позиция данных, выводим заголовок УП

                        arrBuf.push_back(arrAllData[y][x]);
                        if (arrAllData[y][x] != "")
                        {
                            bIsTakeNoEmpty = true;
                        }
                    }
                }
            }
            if (bIsTakeNoEmpty) arrReturnData.push_back(arrBuf);
        }
    }


    if (ptrGlobal->ptrConfig->bOutWithoutEmptyCell)
    {
        for (int y = 1; y < arrReturnData.size(); ++y)
        {
            for (int x = 0; x < arrReturnData[y].size(); ++x)
            {
                if (arrReturnData[y][x] == "")
                    arrReturnData[y][x] = arrReturnData[y - 1][x];
            }
        }
    }


}

void FOutData::CreateTableRectInfo(
    const bool& bIsCounting,
    vector<vector<string>>& arrReturnData,    // Возвращаемое значение с функции
    int x, int& iSizeX, int& iCurrentY, FTreeMetric* ptrMetric, int iDeep
    // const bool& bIsLocal
)
{
    if (!bIsCounting)
    {
        // Достигнут лимит глубины вывода
        if (x >= iSizeX)
        {
            ++iCurrentY;
            return;
        }
    }

    if (ptrMetric->sName == FMetric::sEmptyIndicator)
    {
        ++iCurrentY;
        return;
    }

    if (!bIsCounting)
    {
        arrReturnData[iCurrentY][x] = ptrMetric->sName;
    }
    // Выводим кол-во ЗЕ
    if (iDeep > 0)
    {
        ++x;
        if (!bIsCounting)
        {
            double dRes = 0.;
            dRes        = (ptrGlobal->ptrConfig->bCompInterDelete)
                              ? ptrMetric->dBalanceSum
                              : ptrMetric->dNoBalanceSum;

            arrReturnData[iCurrentY][x] =
                ptrGlobal->DoubletWithPrecision(ptrMetric->dChosenSum);
        }
    }

    // Выводим кол-во дисциплин
    if (iDeep > 0)
    {
        ++x;
        if (!bIsCounting)
        {
            arrReturnData[iCurrentY][x] =
                ptrGlobal->DoubletWithPrecision(ptrMetric->iAmountUsingDisc);
        }
    }

    // Выводим процент распределения
    if (iDeep > 0)
    {
        ++x;
        if (!bIsCounting)
        {
            double dRes = 0.;
            if (ptrGlobal->ptrConfig->bIsPercentRegAll)
            {
                dRes = (ptrGlobal->ptrConfig->bCompInterDelete)
                           ? ptrMetric->dBalanceSum /
                                 ptrMetric->ptrParent->dBalanceSum
                           : ptrMetric->dNoBalanceSum /
                                 ptrMetric->ptrParent->dNoBalanceSum;
            }
            else
            {
                dRes = (ptrGlobal->ptrConfig->bCompInterDelete)
                           ? ptrMetric->dBalanceSum /
                                 ptrMetric->ptrParent->dBalanceSum
                           : ptrMetric->dNoBalanceSum /
                                 ptrMetric->ptrParent->dNoBalanceSum;
            }

            arrReturnData[iCurrentY][x] =
                ptrGlobal->DoubletWithPrecision(ptrMetric->dInclusionPercent);
        }
    }

    if (bIsCounting)
    {
        if (x >= iSizeX)
            iSizeX = x + 1;    // Ищем максимум, чтобы отмерить ширину
    }

    if (ptrMetric->mapChild.size() == 0)
    {
        ++iCurrentY;
        return;
    }

    for (auto& [sName, ptrChild] : ptrMetric->mapChild)
    {
        CreateTableRectInfo(bIsCounting, arrReturnData, x + 1, iSizeX,
                            iCurrentY, ptrChild, iDeep + 1    // dAllSum,
                                                              // bIsLocal
        );
    }
}

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

vector<string> FOutData::CreateTag(const int& iGraphType, FTreeDisc* fTree)
{
    vector<string> arrTag;
    for (auto& [key, val] : fTree->ptrGraph->mapGraph[iGraphType].mapReversRel)
    {
        string sTag = "-";
        if (fTree->mapDisc[key.first]->setTagDisc.size() > 0)
            sTag = to_string(*fTree->mapDisc[key.first]->setTagDisc.begin());
        arrTag.push_back(sTag);
    }

    return arrTag;
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
    OutGephiLabel(sName, sName, sPath,
                  CreateCommonNameLabel(FGraph::iCommon, fTree),
                  fTree->ptrGraph->mapGraph[FGraph::iCommon].arrNodeWeight,
                  CreateTag(FGraph::iCommon, fTree));
    OutGephiRib(sName, sName, sPath,
                fTree->ptrGraph->mapGraph[FGraph::iCommon].fAdjList);

    OutGephiLabel(
        sName,
        sName +
            ptrGlobal->ConwertToString(
                ptrGlobal->ptrConfig
                    ->mapArrOutParams
                        [L"Суффикс названия альтернативного графа при выводе"]
                    .at(0)),
        sPath,
        CreateCommonNameLabel(FGraph::iAlt, fTree),
        fTree->ptrGraph->mapGraph[FGraph::iAlt].arrNodeWeight,
        CreateTag(FGraph::iAlt, fTree));

    OutGephiRib(
        sName,
        sName +
            ptrGlobal->ConwertToString(
                ptrGlobal->ptrConfig
                    ->mapArrOutParams
                        [L"Суффикс названия альтернативного графа при выводе"]
                    .at(0)),
        sPath,
        fTree->ptrGraph->mapGraph[FGraph::iAlt].fAdjList);

    if (fTree->ptrGlobal->ptrConfig->bCourseOutput)
    {
        for (int iCourse = 0; iCourse < fTree->iAmountCourse; ++iCourse)
        {
            OutGephiLabel(sName, sName + "(" + to_string(iCourse + 1) + ")",
                          sPath, CreateCommonNameLabel(iCourse, fTree),
                          fTree->ptrGraph->mapGraph[iCourse].arrNodeWeight,
                          CreateTag(iCourse, fTree));
            OutGephiRib(sName, sName + "(" + to_string(iCourse + 1) + ")",
                        sPath, fTree->ptrGraph->mapGraph[iCourse].fAdjList);
        }
    }
}

void FOutData::OutGephiLabel(const string& sName, const string& sNameFile,
                             const string&         sPath,
                             const vector<string>& arrNameLabel,
                             const vector<double>& arrWeightNode,
                             const vector<string>& arrTag)
{
    ofstream outLabel(sPath + "/" + sName + "/" + sNameFile + "Label.csv");

    outLabel << ptrGlobal->ptrConfig->sNameLabelHeader << "\n";

    // id, имя, вес узла

    for (int i = 0; i < arrWeightNode.size(); ++i)
    {
        outLabel << i << ";";
        outLabel << arrNameLabel[i] << ";";
        outLabel << arrWeightNode[i] << ";";
        outLabel << arrTag[i] << "";
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
        for (const auto& [r, dLen] : fAdjList[l])
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
    if ((dNewData > fSaveData.dMaxMin[0]) || (fSaveData.sMaxMin[0] == ""))
    {
        fSaveData.dMaxMin[0] = dNewData;
        fSaveData.sMaxMin[0] = sNewData;
    }

    if ((dNewData < fSaveData.dMaxMin[1]) || (fSaveData.sMaxMin[1] == ""))
    {
        fSaveData.dMaxMin[1] = dNewData;
        fSaveData.sMaxMin[1] = sNewData;
    }
    return;
}

bool FOutData::TakePasteData(const int& x, vector<string>& arrCurRow,
                             const bool& bIsOutData, const double& dDate,
                             const string& sOutData, const string& sCurName,
                             const bool&          bIsConsider,
                             map<int, FСorridor>& mapCorridorData)
{
    if (bIsOutData)
    {
        if (bIsConsider) RetakeMinMax(mapCorridorData[x], dDate, sCurName);

        arrCurRow[x] = ptrGlobal->ConwertUTF16RU(sOutData);
        return true;
    }
    return false;
}

void FOutData::CreateGraphE1TableInfoInit(
    map<string, map<string, string>>& fReturnData,
    FTreeMetric*                      ptrMetric)
{
    int                    iSizeX = 0, iSizeY = 0;
    vector<vector<string>> arrAllData;

    CreateGraphE1TableRectInfo(true,    // Считаем вхолостую
                               arrAllData, 0, iSizeX,
                               iSizeY,    // 0-строка под заголовок
                               ptrMetric, 0    // dAllSum,
                                               // bIsLocal
    );

    arrAllData.assign(iSizeY, vector<string>(iSizeX));

    set<wstring> setE1AcceptedHead = { L"Заголовок компетенции", L"Компетенция",
                                       L"ЗЕ Компетенций" };

    int iCurrentY = 0;
    CreateGraphE1TableRectInfo(false, arrAllData, 0, iSizeX,
                               iCurrentY,    // 0-строка под заголовок
                               ptrMetric, 0    // dAllSum,
                                               // bIsLocal
    );

    // Теперь нужно оставить только те заголовки setE1AcceptedHead
    string sHeadComp;
    string sComp;
    string sScore;

    const vector<wstring> arrCompetenceHead(
        { L"За какой курс",

          L"Заголовок компетенции", L"ЗЕ Заголовка компетенции",
          L"Кол-во дисциплин заголовка компетенции",

          L"Процент распределения Заголовка компетенции",

          L"Компетенция", L"ЗЕ Компетенций", L"Кол-во дисциплин компетенции",
          L"Процент распределения Компетенции",

          L"Индикатор", L"ЗЕ индикаторов", L"Кол-во дисциплин индикатора",
          L"Процент распределения Индикатора" }); // Свой независимый от других вывод

    for (int y = 0; y < arrAllData.size(); ++y)
    {
        int x = -1;

        for (const auto& it : arrCompetenceHead)
        {
            ++x;
            if (x >= iSizeX) break;
            if (it == L"Заголовок компетенции")
            {
                if (arrAllData[y][x] != "") sHeadComp = arrAllData[y][x];
            }
            if (it == L"Компетенция")
            {
                if (arrAllData[y][x] != "") sComp = arrAllData[y][x];
            }
            if (it == L"ЗЕ Компетенций")
            {
                if (arrAllData[y][x] != "") sScore = arrAllData[y][x];
            }
        }
        if (sScore == "0")
            fReturnData[sHeadComp][sComp] = "";
        else
            fReturnData[sHeadComp][sComp] = sScore;
    }
}

void FOutData::CreateGraphE1TableRectInfo(
    const bool& bIsCounting,
    vector<vector<string>>& arrReturnData,    // Возвращаемое значение с функции
    int x, int& iSizeX, int& iCurrentY, FTreeMetric* ptrMetric, int iDeep
    // const bool& bIsLocal
)
{
    if (iDeep == 3)
    {
        ++iCurrentY;
        return;
    }

    if (!bIsCounting)
    {
        arrReturnData[iCurrentY][x] = ptrMetric->sName;
    }

    // Выводим кол-во ЗЕ
    if (iDeep > 0)
    {
        ++x;
        if (!bIsCounting)
        {
            double dRes = 0.;
            dRes        = (ptrGlobal->ptrConfig->bCompInterDelete)
                              ? ptrMetric->dBalanceSum
                              : ptrMetric->dNoBalanceSum;

            arrReturnData[iCurrentY][x] = arrReturnData[iCurrentY][x] =
                ptrGlobal->DoubletWithPrecision(ptrMetric->dChosenSum);
        }
    }

    // Выводим кол-во дисциплин
    if (iDeep > 0)
    {
        ++x;
        if (!bIsCounting)
        {
            arrReturnData[iCurrentY][x] =
                ptrGlobal->DoubletWithPrecision(ptrMetric->iAmountUsingDisc);
        }
    }

    // Выводим процент распределения
    if (iDeep > 0)
    {
        ++x;
        if (!bIsCounting)
        {
            double dRes = 0.;
            if (ptrGlobal->ptrConfig->bIsPercentRegAll)
            {
                dRes = (ptrGlobal->ptrConfig->bCompInterDelete)
                           ? ptrMetric->dBalanceSum /
                                 ptrMetric->ptrParent->dBalanceSum
                           : ptrMetric->dNoBalanceSum /
                                 ptrMetric->ptrParent->dNoBalanceSum;
            }
            else
            {
                dRes = (ptrGlobal->ptrConfig->bCompInterDelete)
                           ? ptrMetric->dBalanceSum /
                                 ptrMetric->ptrParent->dBalanceSum
                           : ptrMetric->dNoBalanceSum /
                                 ptrMetric->ptrParent->dNoBalanceSum;
            }

            arrReturnData[iCurrentY][x] =
                ptrGlobal->DoubletWithPrecision(ptrMetric->dInclusionPercent);
        }
    }

    if (bIsCounting)
    {
        if (x >= iSizeX)
            iSizeX = x + 1;    // Ищем максимум, чтобы отмерить ширину
    }

    if (ptrMetric->mapChild.size() == 0)
    {
        ++iCurrentY;
        return;
    }

    for (auto& [sName, ptrChild] : ptrMetric->mapChild)
    {
        CreateGraphE1TableRectInfo(bIsCounting, arrReturnData, x + 1, iSizeX,
                                   iCurrentY, ptrChild, iDeep + 1    // dAllSum,
                                                                     // bIsLocal
        );
    }
}
