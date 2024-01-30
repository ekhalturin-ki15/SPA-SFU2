#include "adapOutData.h"

#include "config.h"
#include "error.h"
#include "graph.h"
#include "metric.h"
#include "solve.h"

int FAdapOutData::iSinglControll = 0;

FAdapOutData::FAdapOutData(shared_ptr<FGlobal> _ptrGlobal)
    : ptrGlobal(_ptrGlobal)
{
    // Unit test против такого
    // if (iSinglControll > 0) throw std::runtime_error("Re-creation
    // Singleton");
    ++iSinglControll;
}

bool FAdapOutData::Init() { return true; }

void FAdapOutData::Create(string sOutPath)
{

}
      //
//FAdapOutData::FAdapOutData(FGlobal* _ptrGlobal) : ptrGlobal(_ptrGlobal),
//      arrCompetenceHead(
//          { L"За какой курс",
//
//            L"Заголовок компетенции", L"ЗЕ Заголовка компетенции",
//            L"Кол-во дисциплин заголовка компетенции",
//
//            L"Процент распределения Заголовка компетенции",
//
//            L"Компетенция", L"ЗЕ Компетенций", L"Кол-во дисциплин компетенции",
//            L"Процент распределения Компетенции",
//
//            L"Индикатор", L"ЗЕ индикаторов", L"Кол-во дисциплин индикатора",
//            L"Процент распределения Индикатора" }),
//      arrMetricHead(
//          { // L"Название учебного плана",
//            L"Всего ЗЕ в графе", L"Кол-во дисциплин в графе",
//            // L"(Расш.) Общее кол-во ЗЕ в УП",
//            // L"(Расш.) Кол-во дисциплин в УП",    // Включает те,
//            // что указаны в
//            //  УП, как неучитываемые
//            L"Максимальное ЗЕ у дисциплины", L"Минимальное ЗЕ у дисциплины",
//            L"Максимальный вес ребра", L"Минимальный вес ребра",
//            L"Диаметр графа по расстоянию",
//            L"Диаметр графа по количеству рёбер",
//            L"Количество компонент связности", L"Максимальное оставное дерево",
//            L"Минимальное оставное дерево", L"Плотность графа",
//            L"Количество основных дисциплин", L"Количество дисциплин по выбору",
//            L"Количество факультативов" })
//{
//
//}
//
//bool FAdapOutData::Init() { return true; }
//
//void FAdapOutData::Create(string sOutPath)
//{
//    CreateAllCurriculaTotalData(arrAllCurriculaTotalData);
//
//    CreateSummaryTotalData(arrAllCommonCoursesGraphData, FGraph::iCommon);
//
//     for (int iCourse = 0; iCourse < this->ptrGlobal->ptrSolve->iMaxCourse;
//         ++iCourse)
//    {
//        CreateSummaryTotalData(mapArrCourseGraphData[iCourse], iCourse);
//    }
//
//}
//
//void FAdapOutData::CreateOnlyAllowedHeaderRow(vector<string>& arrReturn,
//                                          vector<bool>&          arrIsAllowed,
//                                          const vector<wstring>& arrParams)
//{
//    arrReturn.clear();
//    arrIsAllowed.clear();
//    // arrIsAllowed.assign(arrParams.size(), true);
//
//    int iAmountComp = 1;    // Так как предметы с 0-компетенциями исключаются
//    int i = 0;
//    for (const auto& it : arrParams)
//    {
//        if (ptrGlobal->ptrConfig->mapArrOutParams.count(it))
//        {
//            arrIsAllowed.push_back(
//                (ptrGlobal->ptrConfig->mapArrOutParams[it].at(
//                     EOutType::EOT_Head) ==
//                 L"да"));    // Так как глобальная информация, то
//                             // работаем с EOutType::EOT_Head
//            if (arrIsAllowed.back())
//            {
//                string sOut = ptrGlobal->ConwertToString(
//                    ptrGlobal->ptrConfig->mapArrOutParams[it].at(0));
//                if (it == L"Количество дисциплин с несколькими дисциплинами")
//                {
//                    sOut += " ";
//                    if (iAmountComp == ptrGlobal->ptrConfig->iSoMachComp)
//                        sOut += ">=";
//
//                    sOut += to_string(iAmountComp++) + " ";
//                    sOut += ptrGlobal->ConwertToString(
//                        ptrGlobal->ptrConfig
//                            ->mapArrOutParams[L"Суффикс после вывода кол-во "
//                                              L"компетенций у дисциплины"]
//                            .at(0));
//
//                    // sOut += ptrGlobal->ConwertToString(
//                    //     ptrGlobal->ptrConfig->wsOutSufAmountComp);
//                }
//
//                arrReturn.push_back(sOut);
//            }
//        }
//        else
//        {
//            arrIsAllowed.push_back(true);
//            if (it.find(
//                    ptrGlobal->ptrConfig->mapArrOutParams[L"ЗЕ у компетенции"]
//                        .at(0)) != wstring::npos)
//            {
//                if (ptrGlobal->ptrConfig->mapArrOutParams[L"ЗЕ у компетенции"]
//                        .at(EOutType::EOT_Head) != L"да")
//                    arrIsAllowed.back() = false;
//            }
//            if (arrIsAllowed.back())
//            {
//                arrReturn.push_back(ptrGlobal->ConwertToString(it));
//            }
//        }
//    }
//}
//
//void FAdapOutData::CreateOnlyAllowedResultRow(
//    vector<string>&       arrReturn,
//                                          const int&            iSizeHeader,
//                                          vector<bool>&         arrIsAllowed,
//                                          const string&         sCurName,
//                                          const vector<double>& arrResult,
//                                          map<int, FСorridor>&  mapCorridorData)
//{
//    int i = 0;
//    int x = 0;
//
//    arrReturn.clear();
//
//    arrReturn.resize(iSizeHeader);
//
//    if (TakePasteData(x, arrReturn, arrIsAllowed[i++], sCurName.size(),
//                      sCurName, sCurName, false, mapCorridorData))
//    {
//        x++;
//    }
//
//    for (const auto& it : arrResult)
//    {
//        if (it != FGraphType::dNoInit)
//        {
//            /*std::ostringstream fOut;
//            fOut << std::setprecision(ptrGlobal->ptrConfig->iPrecision)
//                 << std::noshowpoint << it;*/
//
//            if (TakePasteData(x, arrReturn, arrIsAllowed[i++], it,
//                              ptrGlobal->DoubletWithPrecision(it), sCurName,
//                              true, mapCorridorData))
//            {
//                x++;
//            }
//        }
//        else
//        {
//            if (TakePasteData(x, arrReturn, arrIsAllowed[i++], it, "-",
//                              sCurName, false, mapCorridorData))
//            {
//                x++;
//            }
//        }
//    }
//}
//
//void FAdapOutData::AddTableMaxMinData(vector<vector<string>>& arrToAddedData,
//                                  map<int, FСorridor>&    mapCorridorData)
//{
//    int i = -1;
//    for (const auto& wsName : vector<wstring>(
//             { L"Максимальные значения:", L"Минимальные значения:" }))
//    {
//        ++i;
//        if (ptrGlobal->ptrConfig->mapArrOutParams.count(wsName))
//        {
//            if (ptrGlobal->ptrConfig->mapArrOutParams[wsName].at(1) == L"да")
//            {
//                vector<string> arrMinMaxData;
//
//                arrMinMaxData.push_back(ptrGlobal->ConwertToString(
//                    ptrGlobal->ptrConfig->mapArrOutParams[wsName].at(0)));
//
//                for (auto& [id, fСorridor] : mapCorridorData)
//                {
//                    /*arrMinMaxData.push_back(
//                        to_string(fСorridor.dMaxMin[i]) + " (" +
//                        ptrGlobal->ConwertToString(
//                            ptrGlobal->ptrConfig->wsOutPrefMinMax) +
//                        fСorridor.sMaxMin[i] + ")");  */
//                    /*std::ostringstream fOut;
//                    fOut << std::setprecision(ptrGlobal->ptrConfig->iPrecision)
//                         << std::noshowpoint << fСorridor.dMaxMin[i];*/
//
//                    arrMinMaxData.push_back(
//                        ptrGlobal->DoubletWithPrecision(fСorridor.dMaxMin[i]) +
//                        " (" +
//                        ptrGlobal->ConwertToString(
//                            ptrGlobal->ptrConfig
//                                ->mapArrOutParams
//                                .at(0)) +
//                        fСorridor.sMaxMin[i] + ")");
//                }
//
//                arrToAddedData.push_back(arrMinMaxData);
//            }
//        }
//    }
//}
//
//
//void FAdapOutData::CreateTotalInfo(vector<double>&   arrReturnDataMetrics,
//                               const FGraphType* fGraph,
//                               const EOutType&   eOutType)
//{
//    arrReturnDataMetrics.clear();
//    vector<double> arrResult;    // Соответствует arrHead который
//                                 // проинициализирован в конструкторе
//    arrResult = { fGraph->dGraphAllScore, double(fGraph->iGraphAmountDisc),
//                  // fGraph->dGraphAllScore,
//                  // double(fGraph->iGraphAmountDisc),
//                  fGraph->dMaxDiscScore, fGraph->dMinDiscScore, fGraph->dMaxRib,
//                  fGraph->dMinRib, fGraph->dDiametrLen, fGraph->dDiametrStep,
//                  double(fGraph->iComponent), fGraph->dMaxSpanTree,
//                  fGraph->dMinSpanTree, fGraph->dDense };
//
//    for (int iType = 0; iType < ETypeDisc::ETD_Size; ++iType)
//    {
//        if (fGraph->mapGraphAmountTypeDisc.count(ETypeDisc(iType)))
//            arrResult.push_back(double(
//                fGraph->mapGraphAmountTypeDisc.find(ETypeDisc(iType))->second));
//        else
//        {
//            arrResult.push_back(0);    // Нет такого вида дисциплин
//        }
//    }
//
//    for (int y = 0; y < arrMetricHead.size(); ++y)
//    {
//        if (ptrGlobal->ptrConfig->mapArrOutParams.count(arrMetricHead[y]))
//        {
//            if (ptrGlobal->ptrConfig->mapArrOutParams[arrMetricHead[y]].at(
//                    eOutType) == L"да")
//            {
//                arrReturnDataMetrics.push_back(arrResult[y]);
//            }
//        }
//    }
//
//    wstring wsNameSoMachComp =
//        L"Количество дисциплин с несколькими дисциплинами";
//    if (ptrGlobal->ptrConfig->mapArrOutParams[wsNameSoMachComp].at(eOutType) ==
//        L"да")
//    {
//        // Так как дисциплин с 0-компетенциями исключаются
//        for (int iAmountComp = 1;
//             iAmountComp <= this->ptrGlobal->ptrConfig->iSoMachComp;
//             ++iAmountComp)
//        {
//            arrReturnDataMetrics.push_back(
//                fGraph->arrAmountCountCompDisc[iAmountComp]);
//        }
//    }
//}
//
//void FAdapOutData::CreateTotalInfo(vector<string>&   arrReturnDataHeader,
//                               const FGraphType* fGraph,
//                               const EOutType&   eOutType)
//{
//    arrReturnDataHeader.clear();
//
//    for (int y = 0; y < arrMetricHead.size(); ++y)
//    {
//        if (ptrGlobal->ptrConfig->mapArrOutParams.count(arrMetricHead[y]))
//        {
//            if (ptrGlobal->ptrConfig->mapArrOutParams[arrMetricHead[y]].at(
//                    eOutType) == L"да")
//            {
//                arrReturnDataHeader.push_back(ptrGlobal->ConwertToString(
//                    ptrGlobal->ptrConfig->mapArrOutParams[arrMetricHead[y]].at(
//                        0)));
//            }
//        }
//    }
//
//    wstring wsNameSoMachComp =
//        L"Количество дисциплин с несколькими дисциплинами";
//    if (ptrGlobal->ptrConfig->mapArrOutParams[wsNameSoMachComp].at(eOutType) ==
//        L"да")
//    {
//        for (int iAmountComp = 1;
//             iAmountComp <= this->ptrGlobal->ptrConfig->iSoMachComp;
//             ++iAmountComp)
//        {
//            string sAddedHead = ptrGlobal->ConwertToString(
//                ptrGlobal->ptrConfig->mapArrOutParams[wsNameSoMachComp].at(0));
//
//            sAddedHead += " ";
//            if (iAmountComp == ptrGlobal->ptrConfig->iSoMachComp)
//                sAddedHead += ">=";
//
//            sAddedHead += to_string(iAmountComp) + " ";
//            sAddedHead += ptrGlobal->ConwertToString(
//                ptrGlobal->ptrConfig
//                    ->mapArrOutParams[L"Суффикс после вывода кол-во "
//                                      L"компетенций у дисциплины"]
//                    .at(0));
//            // sAddedHead += ptrGlobal->ConwertToString(
//            //     ptrGlobal->ptrConfig->wsOutSufAmountComp);
//
//            arrReturnDataHeader.push_back(sAddedHead);
//        }
//    }
//}
//
//void FAdapOutData::CreateTotalInfo(vector<vector<string>>& arrReturnData,
//                               const FGraphType*       fGraph,
//                               const EOutType&         eOutType)
//{
//    arrReturnData.clear();
//
//    vector<double> arrResult;    // Соответствует arrHead который
//                                 // проинициализирован в конструкторе
//    CreateTotalInfo(arrResult, fGraph, eOutType);
//
//    vector<string> arrHeader;
//    CreateTotalInfo(arrHeader, fGraph, eOutType);
//
//    arrReturnData.clear();
//    arrReturnData.assign(min(arrResult.size(), arrHeader.size()),
//                         vector<string>(2));
//    for (int y = 0; y < arrReturnData.size(); ++y)
//    {
//        arrReturnData[y][0] = arrHeader[y];
//        /*std::ostringstream fOut;
//        fOut << std::setprecision(ptrGlobal->ptrConfig->iPrecision)
//             << std::noshowpoint << arrResult[y];*/
//        arrReturnData[y][1] = ptrGlobal->DoubletWithPrecision(arrResult[y]);
//    }
//}
//
//void FAdapOutData::CreateAllCurriculaTotalData(
//    vector<vector<string>>& arrReturnData)
//{
//    arrReturnData.clear();
//    int i = 0;    // Задаём порядок вывода
//    int x = 0;
//    int y = 0;
//
//    // Параметры для всего УП целиком
//    vector<wstring> arrAddedHead = { L"Название учебного плана",
//                                     L"Общее кол-во ЗЕ в УП",
//                                     L"Общее кол-во дисциплин в УП"
//
//    };
//
//    for (auto& wsNameType : ptrGlobal->ptrConfig->arrNameTypeDisc)
//    {
//        arrAddedHead.push_back(L"!" + wsNameType);
//    }
//
//    for (auto& sHeaderComp : ptrGlobal->ptrSolve->setHeaderComp)
//    {
//        wstring wsCompScore = ptrGlobal->ConwertToWstring(sHeaderComp);
//        wsCompScore =
//            ptrGlobal->ptrConfig->mapArrOutParams[L"ЗЕ у компетенции"].at(0) +
//            wsCompScore;
//        arrAddedHead.push_back(wsCompScore);
//        arrAddedHead.push_back(ptrGlobal->ConwertToWstring(sHeaderComp));
//    }
//
//    // Флаги того, какие столбцы выводить
//    vector<bool> arrOutColm;
//
//    // Строка заголовка таблицы
//    vector<string> arrHeader;
//    CreateOnlyAllowedHeaderRow(arrHeader, arrOutColm, arrAddedHead);
//    arrReturnData.push_back(arrHeader);
//
//    // Вывод данных
//    map<int, FСorridor> mapSaveСorridorData;
//    for (const auto& it : ptrGlobal->ptrSolve->arrDisc)
//    {
//        // Выводить короткое, или помное имя
//        string sCurName = (ptrGlobal->ptrConfig->bOutShortNameCurr)
//                              ? it->sShortNamePlan
//                              : it->sNamePlan;
//
//#pragma region FormationData
//        vector<double> arrAllResult;
//        // Общие для УП метрики (перечислены в arrPrefixHead)
//        arrAllResult.push_back(it->dAllSumScore);
//        arrAllResult.push_back(it->iExtendedAmountDisc);
//
//        // Дисциплин основных, по выбору, факультативов и т.д во всём УП
//        {
//            int iNumberType = -1;
//            for (auto& wsNameType : ptrGlobal->ptrConfig->arrNameTypeDisc)
//            {
//                ++iNumberType;
//                arrAllResult.push_back(
//                    it->mapAmountTypeDisc[ETypeDisc(iNumberType)]);
//            }
//        }
//
//        for (auto& sHeaderComp : ptrGlobal->ptrSolve->setHeaderComp)
//        {
//            if (it->ptrMetric)
//            {
//                auto ptrTreeMetric =
//                    it->ptrMetric->ptrTreeMetric->mapChild[FMetric::sAllMetric];
//                double dScore = 0.;
//                double dRes   = 0.;
//                if (ptrTreeMetric->mapChild.count(sHeaderComp))
//                {
//                    /* dScore =
//                         (ptrGlobal->ptrConfig->bCompInterDelete)
//                             ? ptrTreeMetric->mapChild[sHeaderComp]->dBalanceSum
//                             : ptrTreeMetric->mapChild[sHeaderComp]
//                                   ->dNoBalanceSum;
//
//                     dRes = (ptrGlobal->ptrConfig->bCompInterDelete)
//                                ? dScore / ptrTreeMetric->dBalanceSum
//                                : dScore / ptrTreeMetric->dNoBalanceSum;*/
//
//                    dScore = ptrTreeMetric->dChosenSum;
//                    dRes   = ptrTreeMetric->dInclusionPercent;
//                }
//                else
//                {
//                    dScore = 0.;
//                    dRes   = 0.;
//                }
//
//                // Если ЗЕ равно 0, то и не выводить
//                if (dScore > 0)
//                {
//                    arrAllResult.push_back(dScore);
//                }
//                else
//                {
//                    arrAllResult.push_back(FGraphType::dNoInit);
//                }
//
//                if (dRes > ptrGlobal->ptrConfig->dMinComp)
//                {
//                    arrAllResult.push_back(dRes * 100);
//                }
//                else
//                {
//                    arrAllResult.push_back(FGraphType::dNoInit);
//                }
//            }
//        }
//#pragma endregion
//
//        vector<string> arrCurData;
//        CreateOnlyAllowedResultRow(arrCurData, arrHeader.size(), arrOutColm,
//                                   sCurName, arrAllResult, mapSaveСorridorData);
//
//        arrReturnData.push_back(arrCurData);
//    }
//
//    //  Вывод коридора минимума максимума
//    AddTableMaxMinData(arrReturnData, mapSaveСorridorData);
//}
//
//void FAdapOutData::CreateSummaryTotalData(vector<vector<string>>& arrReturnData,
//                                      const int&              iGraphType)
//{
//    arrReturnData.clear();
//    int i = 0;    // Задаём порядок вывода
//    int x = 0;
//    int y = 0;
//
//    vector<wstring> arrAddedHead = { L"Название учебного плана" };
//
//    arrAddedHead.insert(arrAddedHead.end(),
//                        arrMetricHead.begin(),
//                        arrMetricHead.end());
//
//    wstring wsNameSoMachComp =
//        L"Количество дисциплин с несколькими дисциплинами";
//
//    for (int iAmountComp = 1;
//         iAmountComp <= this->ptrGlobal->ptrConfig->iSoMachComp;
//         ++iAmountComp)
//    {
//        arrAddedHead.push_back(wsNameSoMachComp);
//    }
//
//    // Флаги того, какие столбцы выводить
//    vector<bool> arrOutColm;
//
//    // Строка заголовка таблицы
//    vector<string> arrHeader;
//    CreateOnlyAllowedHeaderRow(arrHeader, arrOutColm, arrAddedHead);
//    arrReturnData.push_back(arrHeader);
//
//    // Вывод данных
//    map<int, FСorridor> mapSaveСorridorData;
//
//    for (const auto& it : ptrGlobal->ptrSolve->arrDisc)
//    {
//        // Нет нужного нам курса (например, если 3 курс, а план магистратуры)
//        if (it->iAmountCourse <= iGraphType) continue;
//
//        // Выводить короткое, или помное имя
//        string sCurName = (ptrGlobal->ptrConfig->bOutShortNameCurr)
//                              ? it->sShortNamePlan
//                              : it->sNamePlan;
//
//        vector<double> arrAllResult;
//
//        CreateTotalInfo(arrAllResult,
//                        &it->ptrGraph->mapGraph[iGraphType],
//                        EOutType::EOT_Head);
//
//        vector<string> arrCurData;
//        CreateOnlyAllowedResultRow(arrCurData, arrHeader.size(), arrOutColm,
//                                   sCurName, arrAllResult, mapSaveСorridorData);
//
//        arrReturnData.push_back(arrCurData);
//    }
//
//    AddTableMaxMinData(arrReturnData, mapSaveСorridorData);
//}
//
//void FAdapOutData::CreateTableInfoInit(vector<vector<string>>& arrReturnData,
//                                   FTreeMetric*            ptrMetric,
//                                   // const double            dAllSum,
//                                   bool bIsLocal)
//{
//    int iSizeX = 0, iSizeY = 1;    // iSizeY = 1 из-за заголовка
//    arrReturnData.clear();
//    vector<vector<string>> arrAllData;
//
//    CreateTableRectInfo(true, arrAllData, 0, iSizeX,
//                        iSizeY,    // 0-строка под заголовок
//                        ptrMetric, 0,    // dAllSum,
//                        bIsLocal);       // Считаем вхолостую
//
//    arrAllData.assign(iSizeY, vector<string>(iSizeX));
//
//    // Вывод заголовка
//    {
//        int x = -1;
//        for (const auto& it : arrCompetenceHead)
//        {
//            ++x;
//            if (x >= iSizeX) break;
//            if (this->ptrGlobal->ptrConfig->mapAddOutParams.count(it))
//            {
//                arrAllData[0][x] = this->ptrGlobal->ConwertToString(
//                    this->ptrGlobal->ptrConfig->mapAddOutParams[it].at(0));
//            }
//        }
//    }
//
//    int iCurrentY = 1;
//    CreateTableRectInfo(false, arrAllData, 0, iSizeX,
//                        iCurrentY,    // 0-строка под заголовок
//                        ptrMetric, 0,    // dAllSum,
//                        bIsLocal);
//
//    // Теперь нужно оставить только те заголовки, которые требуется выводить
//
//    // Вывод Только того, что требуется пользователю
//    {
//        // arrReturnData.resize(arrAllData.size());
//
//        for (int y = 0; y < arrAllData.size(); ++y)
//        {
//            int            x = -1;
//            vector<string> arrBuf;
//            bool           bIsTakeNoEmpty = false;
//            for (const auto& it : arrCompetenceHead)
//            {
//                ++x;
//                if (x >= iSizeX) break;
//                if (this->ptrGlobal->ptrConfig->mapAddOutParams.count(it))
//                {
//                    if (ptrGlobal->ptrConfig->mapAddOutParams[it].at(1) ==
//                        L"да")
//                    {
//                        arrBuf.push_back(arrAllData[y][x]);
//                        if (arrAllData[y][x] != "")
//                        {
//                            bIsTakeNoEmpty = true;
//                        }
//                    }
//                }
//            }
//            if (bIsTakeNoEmpty) arrReturnData.push_back(arrBuf);
//        }
//    }
//}
//
//void FAdapOutData::CreateTableRectInfo(
//    const bool& bIsCounting,
//    vector<vector<string>>& arrReturnData,    // Возвращаемое значение с функции
//    int x, int& iSizeX, int& iCurrentY, FTreeMetric* ptrMetric, int iDeep,
//    const bool& bIsLocal)
//{
//    if (!bIsCounting)
//    {
//        // Достигнут лимит глубины вывода
//        if (x >= iSizeX)
//        {
//            ++iCurrentY;
//            return;
//        }
//    }
//
//    if (ptrMetric->sName == FMetric::sEmptyIndicator)
//    {
//        ++iCurrentY;
//        return;
//    }
//
//    if (!bIsCounting)
//    {
//        arrReturnData[iCurrentY][x] = ptrMetric->sName;
//    }
//    // Выводим кол-во ЗЕ
//    if (iDeep > 0)
//    {
//        ++x;
//        if (!bIsCounting)
//        {
//            double dRes = 0.;
//            dRes        = (ptrGlobal->ptrConfig->bCompInterDelete)
//                              ? ptrMetric->dBalanceSum
//                              : ptrMetric->dNoBalanceSum;
//
//            arrReturnData[iCurrentY][x] = arrReturnData[iCurrentY][x] =
//                ptrGlobal->DoubletWithPrecision(ptrMetric->dChosenSum);
//        }
//    }
//
//    // Выводим кол-во дисциплин
//    if (iDeep > 0)
//    {
//        ++x;
//        if (!bIsCounting)
//        {
//            arrReturnData[iCurrentY][x] =
//                ptrGlobal->DoubletWithPrecision(ptrMetric->iAmountUsingDisc);
//        }
//    }
//
//    // Выводим процент распределения
//    if (iDeep > 0)
//    {
//        ++x;
//        if (!bIsCounting)
//        {
//            double dRes = 0.;
//            if (ptrGlobal->ptrConfig->bIsPercentRegAll)
//            {
//                dRes = (ptrGlobal->ptrConfig->bCompInterDelete)
//                           ? ptrMetric->dBalanceSum /
//                                 ptrMetric->ptrParent->dBalanceSum
//                           : ptrMetric->dNoBalanceSum /
//                                 ptrMetric->ptrParent->dNoBalanceSum;
//            }
//            else
//            {
//                dRes = (ptrGlobal->ptrConfig->bCompInterDelete)
//                           ? ptrMetric->dBalanceSum /
//                                 ptrMetric->ptrParent->dBalanceSum
//                           : ptrMetric->dNoBalanceSum /
//                                 ptrMetric->ptrParent->dNoBalanceSum;
//            }
//            arrReturnData[iCurrentY][x] =
//                ptrGlobal->DoubletWithPrecision(ptrMetric->dInclusionPercent);
//        }
//    }
//
//    if (bIsCounting)
//    {
//        if (x >= iSizeX)
//            iSizeX = x + 1;    // Ищем максимум, чтобы отмерить ширину
//    }
//
//    // Убрал, так как пересекается с параметром "не отображать поля"
//    // if (!ptrGlobal->ptrConfig->bOutIndicatorsInfo)
//    //{
//    //     if (iDeep + 1 == ptrGlobal->ptrConfig
//    //                      ->iIndicatorDeep)    // На глубине 0 - курсы, 1 -
//    //                                           // компетенции, 2 - индикаторы
//    //     {
//    //         ++iCurrentY;
//    //         return;
//    //     }
//    // }
//
//    if (ptrMetric->mapChild.size() == 0)
//    {
//        ++iCurrentY;
//        return;
//    }
//
//    for (auto& [sName, ptrChild] : ptrMetric->mapChild)
//    {
//        CreateTableRectInfo(bIsCounting, arrReturnData, x + 1, iSizeX,
//                            iCurrentY, ptrChild, iDeep + 1,    // dAllSum,
//                            bIsLocal);
//    }
//}
//
//string FAdapOutData::AddCompString(const map<string, vector<string>>& mapComp)
//{
//    string sReturn = "(";
//    int    j       = -1;
//    for (const auto& [sNameComp, arrIndicator] : mapComp)
//    {
//        ++j;
//        if (j) sReturn += ",";
//        sReturn += sNameComp;
//    }
//    sReturn += ")";
//    return sReturn;
//}
//
//vector<string> FAdapOutData::CreateTag(const int& iGraphType, FTreeDisc* fTree)
//{
//    vector<string> arrTag;
//    for (auto& [key, val] : fTree->ptrGraph->mapGraph[iGraphType].mapReversRel)
//    {
//        string sTag = "-";
//        if (fTree->mapDisc[key.first]->setTagDisc.size() > 0)
//            sTag = to_string(*fTree->mapDisc[key.first]->setTagDisc.begin());
//        arrTag.push_back(sTag);
//    }
//
//    return arrTag;
//}
//
//
//vector<string> FAdapOutData::CreateCommonNameLabel(const int& iGraphType,
//                                               FTreeDisc* fTree)
//{
//    vector<string> arrCommonNameLabel;
//    for (auto& it : fTree->ptrGraph->mapGraph[iGraphType].arrRel)
//    {
//        FTreeElement* fThis = fTree->mapDisc[it.first];
//        // wstring       wsNameRaw = fThis->wsName;
//        string sName = fThis->sName;
//
//        // Не забываем про нуль нумерацию курсов
//        if (iGraphType == FGraph::iAlt)
//        {
//            sName += ptrGlobal->ptrConfig->sPrefCourseNumber +
//                     to_string(it.second + 1);
//        }
//
//        // ptrGlobal->ReversUTF16RU(ptrGlobal->ConwertToString(wsNameRaw));
//        // Выводим ещё и компетенции
//        if (ptrGlobal->ptrConfig->bOutCompWithName)
//        {
//            sName += AddCompString(fThis->mapComp);
//        }
//        arrCommonNameLabel.push_back(sName);
//    }
//    return arrCommonNameLabel;
//}
//
//void FAdapOutData::RetakeMinMax(FСorridor& fSaveData, const double& dNewData,
//                            const string& sNewData)
//{
//    if ((dNewData > fSaveData.dMaxMin[0]) || (fSaveData.sMaxMin[0] == ""))
//    {
//        fSaveData.dMaxMin[0] = dNewData;
//        fSaveData.sMaxMin[0] = sNewData;
//    }
//
//    if ((dNewData < fSaveData.dMaxMin[1]) || (fSaveData.sMaxMin[1] == ""))
//    {
//        fSaveData.dMaxMin[1] = dNewData;
//        fSaveData.sMaxMin[1] = sNewData;
//    }
//    return;
//}
//
//bool FAdapOutData::TakePasteData(const int& x, vector<string>& arrCurRow,
//                             const bool& bIsOutData, const double& dDate,
//                             const string& sOutData, const string& sCurName,
//                             const bool&          bIsConsider,
//                             map<int, FСorridor>& mapCorridorData)
//{
//    if (bIsOutData)
//    {
//        if (bIsConsider) RetakeMinMax(mapCorridorData[x], dDate, sCurName);
//
//        arrCurRow[x] = ptrGlobal->ConwertUTF16RU(sOutData);
//        return true;
//    }
//    return false;
//}