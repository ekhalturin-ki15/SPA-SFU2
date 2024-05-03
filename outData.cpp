#include "outData.h"
#include "config.h"
#include "error.h"
#include "global.h"
#include "graph.h"
#include "metric.h"
#include "solve.h"

#include <iomanip>
#include <sstream>

FCorridorAdapter::FCorridorAdapter(shared_ptr<FGlobal> _ptrGlobal)
    : ptrGlobal(_ptrGlobal)
{
}

map<int, vector<pair<double, string>>> FCorridorAdapter::Take(const int& iSize)
{
    map<int, vector<pair<double, string>>> mapReturn;

    for (auto& [key, fData] : mapCorridorData)
    {
        auto& arrData = fData;
        sort(arrData.begin(), arrData.end());

        mapReturn[key].resize(
            iSize,
            { FGraphType::dNoInit, ptrGlobal->ptrConfig->GetSNoInitData() });

        try
        {
            int iIndex = 0;
            // [0] - Макс [1] - Мин  [2] - Мода [3] - Медиана [4] - Среднее
            // усечённое
            if (iIndex >= iSize)
                continue;
            else
                mapReturn[key][iIndex++] = arrData.back();    // Макс

            if (iIndex >= iSize)
                continue;
            else
                mapReturn[key][iIndex++] = arrData.front();    // Мин

            if (iIndex >= iSize)    // Мода
                continue;
            else
            {
                map<double, int> mapCounting;
                int              iMax = 0;

                for (const auto& it : arrData)
                {
                    auto& fDataLocal = mapCounting[it.first];
                    ++fDataLocal;

                    if (iMax < fDataLocal)
                    {
                        iMax                   = fDataLocal;
                        mapReturn[key][iIndex] = it;
                    }
                }

                ++iIndex;
            }

            if (iIndex >= iSize)    // Медиана
                continue;
            else
            {
                mapReturn[key][iIndex] = arrData[arrData.size() / 2];
                if (arrData.size() & 1)    // Нечётное
                {
                }
                else    // Чётное
                {
                    if (arrData.size() > 0)
                    {
                        mapReturn[key][iIndex].first =
                            (arrData[arrData.size() / 2].first +
                             arrData[(arrData.size() / 2) - 1].first) /
                            2.0;
                    }
                }
                ++iIndex;
            }

            if (iIndex >= iSize)
                continue;
            else
            {
                if (fData.size())
                {
                    double dSum = 0;
                    int    iAmountCut =
                        fData.size() * ptrGlobal->ptrConfig->GetDTruncAvg();

                    int iSizeSampling = int(fData.size()) - 2 * iAmountCut;

                    if (iSizeSampling > 0)
                    {
                        for (int i = iAmountCut;
                             i < int(fData.size()) - iAmountCut;
                             ++i)
                        {
                            dSum += fData[i].first;
                        }
                        mapReturn[key][iIndex].first =
                            dSum / iSizeSampling;    // Усечённое среднее
                    }
                }
                ++iIndex;
            }
        }
        catch (...)    // Продолжать работать даже в случае ошибки
        {
        }
    }

    return mapReturn;
}

void FCorridorAdapter::Add(int key, pair<double, string> fData)
{
    mapCorridorData[key].push_back(fData);
    // mapCorridorData[key].dSum += fData.first;
}

int FOutData::iSinglControll = 0;

FOutData::FOutData(shared_ptr<FGlobal> _ptrGlobal)
    : ptrGlobal(_ptrGlobal), iSizeOnlyAllow(0)
{
    // Unit test против такого
#ifndef UNIT_TEST
    if (iSinglControll > 0)
        throw std::runtime_error("Re-creation Singleton");
#endif
    ++iSinglControll;

    fFileCache = make_shared<FFileCache>(ptrGlobal->ptrOutData);
}

void FOutData::CreateTotalInfo(vector<double>& arrReturnDataMetrics,
                               const shared_ptr<FGraphType>
                                               fGraph,
                               const EOutType& eOutType)
{
    const vector<wstring> arrMetricHead(
        { L"Всего ЗЕ в графе", L"Кол-во дисциплин в графе",
          L"Максимальное ЗЕ у дисциплины", L"Минимальное ЗЕ у дисциплины",
          L"Максимальный вес ребра", L"Минимальный вес ребра",
          L"Диаметр графа по расстоянию", L"Диаметр графа по количеству рёбер",
          L"Количество компонент связности", L"Максимальное оставное дерево",
          L"Минимальное оставное дерево", L"Плотность графа",
          L"Количество факультативов", L"Количество основных дисциплин",
          L"Количество дисциплин по выбору" });

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
            if (ptrGlobal->ptrConfig->mapArrOutParams[arrMetricHead[y]].GetType(
                    eOutType))
            {
                arrReturnDataMetrics.push_back(arrResult[y]);
            }
        }
    }

    wstring wsNameSoMachComp =
        L"Количество дисциплин, формирующих несколько компетенций";
    if (ptrGlobal->ptrConfig->mapArrOutParams[wsNameSoMachComp].GetType(
            eOutType))
    {
        // Так как дисциплин с 0-компетенциями исключаются
        for (int iAmountComp = 1;
             iAmountComp <= this->ptrGlobal->ptrConfig->GetISoMachComp();
             ++iAmountComp)
        {
            if (fGraph->arrAmountCountCompDisc.size() <= iAmountComp)
            {
                arrReturnDataMetrics.push_back(FGraphType::dNoInit);
            }
            else
            {
                arrReturnDataMetrics.push_back(
                    fGraph->arrAmountCountCompDisc[iAmountComp]);
            }
        }
    }

    wstring wsRibQuartile = L"Количество рёбер указанного квартиля";
    if (ptrGlobal->ptrConfig->mapArrOutParams[wsRibQuartile].GetType(eOutType))
    {
        for (auto& it : fGraph->arrAllPairDistanceQuartile)
        {
            arrReturnDataMetrics.push_back(it);
        }
    }
}

void FOutData::CreateTotalInfo(vector<string>& arrReturnDataHeader,
                               const shared_ptr<FGraphType>
                                               fGraph,
                               const EOutType& eOutType)
{
    const vector<wstring> arrMetricHead({
        L"Всего ЗЕ в графе",
        L"Кол-во дисциплин в графе",
        L"Максимальное ЗЕ у дисциплины",
        L"Минимальное ЗЕ у дисциплины",
        L"Максимальный вес ребра",
        L"Минимальный вес ребра",
        L"Диаметр графа по расстоянию",
        L"Диаметр графа по количеству рёбер",
        L"Количество компонент связности",
        L"Максимальное оставное дерево",
        L"Минимальное оставное дерево",
        L"Плотность графа",
        L"Количество факультативов",
        L"Количество основных дисциплин",
        L"Количество дисциплин по выбору",
    });

    arrReturnDataHeader.clear();

    for (int y = 0; y < arrMetricHead.size(); ++y)
    {
        if (ptrGlobal->ptrConfig->mapArrOutParams.count(arrMetricHead[y]))
        {
            if (ptrGlobal->ptrConfig->mapArrOutParams[arrMetricHead[y]].GetType(
                    eOutType))
            {
                arrReturnDataHeader.push_back(ptrGlobal->ConwertToString(
                    ptrGlobal->ptrConfig->mapArrOutParams[arrMetricHead[y]]
                        .GetName()));
            }
        }
    }

    wstring wsNameSoMachComp =
        L"Количество дисциплин, формирующих несколько компетенций";
    if (ptrGlobal->ptrConfig->mapArrOutParams[wsNameSoMachComp].GetType(
            eOutType))
    {
        for (int iAmountComp = 1;
             iAmountComp <= this->ptrGlobal->ptrConfig->GetISoMachComp();
             ++iAmountComp)
        {
            string sAddedHead = ptrGlobal->ConwertToString(
                ptrGlobal->ptrConfig->mapArrOutParams[wsNameSoMachComp]
                    .GetName());

            sAddedHead += ptrGlobal->ptrConfig->GetSSeparator();
            if (iAmountComp == ptrGlobal->ptrConfig->GetISoMachComp())
                sAddedHead += ">=";

            sAddedHead +=
                to_string(iAmountComp) + ptrGlobal->ptrConfig->GetSSeparator();
            sAddedHead += ptrGlobal->ConwertToString(
                ptrGlobal->ptrConfig
                    ->mapArrOutParams[L"Суффикс после вывода кол-во "
                                      L"компетенций у дисциплины"]
                    .GetName());
            // sAddedHead += ptrGlobal->ConwertToString(
            //     ptrGlobal->ptrConfig->wsOutSufAmountComp);

            arrReturnDataHeader.push_back(sAddedHead);
        }
    }

    wstring wsRibQuartile = L"Количество рёбер указанного квартиля";
    if (ptrGlobal->ptrConfig->mapArrOutParams[wsRibQuartile].GetType(eOutType))
    {
        int iAmountQuartile = -1;
        for (auto& it : fGraph->arrAllPairDistanceQuartile)
        {
            ++iAmountQuartile;
            string sAddedHead = ptrGlobal->ConwertToString(
                ptrGlobal->ptrConfig->mapArrOutParams[wsRibQuartile].GetName());

            sAddedHead += ptrGlobal->ptrConfig->GetSSeparator();
            if (iAmountQuartile == ptrGlobal->ptrConfig->GetIAmountQuar())
                sAddedHead +=
                    "No" + ptrGlobal->ptrConfig->GetSSeparator() + "Path";

            sAddedHead += to_string(iAmountQuartile) +
                          ptrGlobal->ptrConfig->GetSSeparator();
            sAddedHead += ptrGlobal->ConwertToString(
                ptrGlobal->ptrConfig
                    ->mapArrOutParams[L"Суффикс после вывода квартиля "
                                      L"распределения весов рёбер"]
                    .GetName());
            // sAddedHead += ptrGlobal->ConwertToString(
            //     ptrGlobal->ptrConfig->wsOutSufAmountComp);

            arrReturnDataHeader.push_back(sAddedHead);
        }
    }
}

void FOutData::CreateTotalInfo(vector<vector<string>>& arrReturnData,
                               const shared_ptr<FGraphType>
                                               fGraph,
                               const EOutType& eOutType)
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

bool FOutData::Init()
{
    return true;
}

void FOutData::CreateOnlyAllowedHeaderRow(vector<string>&        arrReturn,
                                          vector<bool>&          arrIsAllowed,
                                          const vector<wstring>& arrParams,
                                          const vector<wstring>& arrOnlyAllow)
{
    arrReturn.clear();
    arrIsAllowed.clear();
    // arrIsAllowed.assign(arrParams.size(), true);

    set<wstring> setOnlyAllow = { arrOnlyAllow.begin(), arrOnlyAllow.end() };

    int iAmountComp = 1;    // Так как предметы с 0-компетенциями исключаются
    int iQuartile = 1;      // Номер квартиля
    int i         = 0;
    for (const auto& it : arrParams)
    {
        if (ptrGlobal->ptrConfig->mapArrOutParams.count(it))
        {
            if (setOnlyAllow.count(it))
            {
                arrIsAllowed.push_back(true);
            }
            else
            {
                arrIsAllowed.push_back(
                    (ptrGlobal->ptrConfig->mapArrOutParams[it].GetType(
                        EOutType::EOT_Head)));    // Так как глобальная
                                                  // информация, то работаем с
                                                  // EOutType::EOT_Head
            }

            if (arrIsAllowed.back())
            {
                string sOut = ptrGlobal->ConwertToString(
                    ptrGlobal->ptrConfig->mapArrOutParams[it].GetName());
                if (it ==
                    L"Количество дисциплин, формирующих несколько компетенций")
                {
                    sOut += ptrGlobal->ptrConfig->GetSSeparator();
                    if (iAmountComp == ptrGlobal->ptrConfig->GetISoMachComp())
                        sOut += ">=";

                    sOut += to_string(iAmountComp++) +
                            ptrGlobal->ptrConfig->GetSSeparator();
                    sOut += ptrGlobal->ConwertToString(
                        ptrGlobal->ptrConfig
                            ->mapArrOutParams[L"Суффикс после вывода кол-во "
                                              L"компетенций у дисциплины"]
                            .GetName());
                }

                if (it == L"Количество рёбер указанного квартиля")
                {
                    sOut += ptrGlobal->ptrConfig->GetSSeparator();
                    if (iQuartile > ptrGlobal->ptrConfig->GetIAmountQuar())
                    {
                        sOut = "";
                        sOut += "No" + ptrGlobal->ptrConfig->GetSSeparator() +
                                "Path" + ptrGlobal->ptrConfig->GetSSeparator();
                    }
                    else
                    {
                        sOut += to_string(iQuartile++) +
                                ptrGlobal->ptrConfig->GetSSeparator();
                    }
                    sOut += ptrGlobal->ConwertToString(
                        ptrGlobal->ptrConfig
                            ->mapArrOutParams[L"Суффикс после вывода квартиля "
                                              L"распределения весов рёбер"]
                            .GetName());
                }

                arrReturn.push_back(sOut);
            }
        }
        else
        {
            arrIsAllowed.push_back(true);
            if (it.find(
                    ptrGlobal->ptrConfig->mapArrOutParams[L"ЗЕ у компетенции"]
                        .GetName()) != wstring::npos)
            {
                arrIsAllowed.back() =
                    (ptrGlobal->ptrConfig->mapArrOutParams[L"ЗЕ у компетенции"]
                         .GetType(EOutType::EOT_Head));
            }
            if (arrIsAllowed.back())
            {
                arrReturn.push_back(ptrGlobal->ConwertToString(it));
            }
        }
    }
}

void FOutData::CreateOnlyAllowedResultRow(vector<string>& arrReturn,
                                          const int&      iSizeHeader,
                                          vector<bool>&   arrIsAllowed,
                                          // const string&         sCurName,
                                          const vector<double>& arrResult,
                                          FCorridorAdapter&     fCorridorData,
                                          shared_ptr<FCurricula>
                                                                 ptrCurricula,
                                          const vector<wstring>& arrOnlyAllow)
{
    int i = 0;
    int x = 0;

    arrReturn.clear();

    arrReturn.resize(iSizeHeader);

    vector<string> arrAllowData = {
        ptrCurricula->sCurName,
        // ptrCurricula->sShortNamePlan.substr(ptrCurricula->sShortNamePlan.size()
        // - 2),
        ptrCurricula->sTypePlan,
    };

    for (const auto& sData : arrAllowData)
    {
        if (TakePasteData(x, arrReturn, arrIsAllowed[i++], sData.size(), sData,
                          ptrCurricula->sCurName, true, fCorridorData))
        {
            x++;
        }
    }

    for (const auto& it : arrResult)
    {
        if (it != FGraphType::dNoInit)
        {
            /*std::ostringstream fOut;
            fOut << std::setprecision(ptrGlobal->ptrConfig->iPrecision)
                 << std::noshowpoint << it;*/

            if (TakePasteData(x, arrReturn, arrIsAllowed[i++], it,
                              ptrGlobal->DoubletWithPrecision(it),
                              ptrCurricula->sCurName, true, fCorridorData))
            {
                x++;
            }
        }
        else
        {
            if (TakePasteData(x, arrReturn, arrIsAllowed[i++], it,
                              ptrGlobal->ptrConfig->GetSNoInitData(),
                              ptrCurricula->sCurName, false, fCorridorData))
            {
                x++;
            }
        }
    }
}

void FOutData::AddTableCommonData(vector<vector<string>>& arrToAddedData,
                                  FCorridorAdapter&       fCorridorData)
{
    // Надо посчитать общие данные о среднем и моде

    vector<wstring> arrHeader({ L"Максимальные значения:",
                                L"Минимальные значения:", L"Мода:", L"Медиана:",
                                L"Среднее усечённое:" });

    auto mapCorridorData = fCorridorData.Take(arrHeader.size());

    int i = -1;
    for (const auto& wsName : arrHeader)
    {
        ++i;
        if (ptrGlobal->ptrConfig->mapArrOutParams.count(wsName))
        {
            if (ptrGlobal->ptrConfig->mapArrOutParams[wsName].GetTotal())
            {
                vector<string> arrCommonData;

                // arrMinMaxData.push_back(ptrGlobal->ConwertToString(ptrGlobal->ptrConfig->mapArrOutParams[wsName].at(0)));

                for (const auto& [id, fСorridor] : mapCorridorData)
                {
                    const auto& [dVal, sName] = fСorridor[i];
                    string sResult = ptrGlobal->ptrConfig->GetSNoInitData();
                    if (id == 0)
                    {
                        // Выводим заголовок
                        sResult = ptrGlobal->ConwertToString(
                            ptrGlobal->ptrConfig->mapArrOutParams[wsName]
                                .GetName());
                    }
                    else
                    {
                        if (dVal != FGraphType::dNoInit)
                        {
                            sResult = ptrGlobal->DoubletWithPrecision(dVal);
                            if (sName != ptrGlobal->ptrConfig->GetSNoInitData())
                            {
                                sResult +=
                                    " (" +
                                    ptrGlobal->ConwertToString(
                                        ptrGlobal->ptrConfig
                                            ->mapArrOutParams
                                                [L"Предлог перед выводом "
                                                 L"общих результатов"]
                                            .GetName()) +
                                    sName + ")";
                            }
                        }
                    }

                    arrCommonData.push_back(sResult);
                }

                arrToAddedData.push_back(arrCommonData);
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
    vector<wstring> arrOnlyAllowHead = { L"Название учебного плана",
                                         L"Тип учебного плана" };

    iSizeOnlyAllow = arrOnlyAllowHead.size();

    vector<wstring> arrAddHead = { L"Год начала подготовки", L"Код направления",
                                   L"Общее кол-во ЗЕ в УП",
                                   L"Общее кол-во дисциплин в УП" };

    vector<wstring> arrBuf = arrOnlyAllowHead;
    arrBuf.insert(arrBuf.end(), arrAddHead.begin(), arrAddHead.end());

    for (auto& wsNameType : ptrGlobal->ptrConfig->GetArrTypeDiscCredits())
    {
        arrBuf.push_back(L"!" + wsNameType);
    }
    for (auto& wsNameType : ptrGlobal->ptrConfig->GetArrTypeDiscAmount())
    {
        arrBuf.push_back(L"!" + wsNameType);
    }

    for (auto& sHeaderComp : ptrGlobal->ptrSolve->setHeaderComp)
    {
        wstring wsCompScore = ptrGlobal->ConwertToWstring(sHeaderComp);
        wsCompScore = ptrGlobal->ptrConfig->mapArrOutParams[L"ЗЕ у компетенции"]
                          .GetName() +
                      wsCompScore;
        arrBuf.push_back(wsCompScore);
        arrBuf.push_back(ptrGlobal->ConwertToWstring(sHeaderComp));
    }

    // Флаги того, какие столбцы выводить
    vector<bool> arrOutColm;

    // Строка заголовка таблицы
    vector<string> arrHeader;
    CreateOnlyAllowedHeaderRow(arrHeader, arrOutColm, arrBuf, arrOnlyAllowHead);
    arrReturnData.push_back(arrHeader);

    // Вывод данных
    // map<int, FСorridor> mapSaveСorridorData;
    FCorridorAdapter fSaveСorridorData(ptrGlobal);

    for (const auto& it : ptrGlobal->ptrSolve->arrDisc)
    {
        // Выводить короткое, или помное имя
        /* sCurPlanName = (ptrGlobal->ptrConfig->bOutShortNameCur)
                            ? it->sShortNamePlan
                            : it->sNamePlan;*/

#pragma region FormationData
        vector<double> arrAllResult;
        // Общие для УП метрики (перечислены в arrPrefixHead)
        // arrAllResult.push_back(atoi(
        //    it->sShortNamePlan.substr(it->sShortNamePlan.size() -
        //    2).c_str()));
        // arrAllResult.push_back(atoi(it->sShortNamePlan.substr(0,
        // 2).c_str()));
        arrAllResult.push_back(it->iYearStart);
        arrAllResult.push_back(it->iCodeUGSN);

        //Extended
        //arrAllResult.push_back(it->dAllSumScore);
        arrAllResult.push_back(it->arrETMAllSumScore[ETM_Extended]);
        //arrAllResult.push_back(it->iExtendedAmountDisc);
        arrAllResult.push_back(it->arrETMAmountDisc[ETM_Extended]);

        // Дисциплин факультативов, основных, по выбору  и т.д во всём УП ЗЕ
        {
            int iNumberType = -1;
            for (auto& wsNameType : ptrGlobal->ptrConfig->GetArrTypeDiscCredits())
            {
                //redimer
                ++iNumberType;
                arrAllResult.push_back(
                    it->mapETMTypeDisc[ETM_Extended][ETypeDisc(iNumberType)]
                        .dCredits);
            }
        }

        // Дисциплин факультативов, основных, по выбору  и т.д во всём УП количество
        {
            int iNumberType = -1;
            for (auto& wsNameType : ptrGlobal->ptrConfig->GetArrTypeDiscAmount())
            {
                ++iNumberType;
                arrAllResult.push_back(
                    it->mapETMTypeDisc[ETM_Extended][ETypeDisc(iNumberType)]
                        .iAmount);
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

                if (dRes > ptrGlobal->ptrConfig->GetDMinComp())
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
                                   arrAllResult, fSaveСorridorData, it,
                                   arrOnlyAllowHead);

        arrReturnData.push_back(arrCurData);
    }

    //  Вывод коридора минимума максимума
    AddTableCommonData(arrReturnData, fSaveСorridorData);
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
          L"Количество факультативов", L"Количество основных дисциплин",
          L"Количество дисциплин по выбору" });

    arrReturnData.clear();
    int i = 0;    // Задаём порядок вывода
    int x = 0;
    int y = 0;

    vector<wstring> arrOnlyAllowHead = { L"Название учебного плана",
                                         // L"Год начала подготовки",
                                         L"Тип учебного плана" };
    vector<wstring> arrAddedHead = arrOnlyAllowHead;

    arrAddedHead.insert(arrAddedHead.end(),
                        arrMetricHead.begin(),
                        arrMetricHead.end());

    wstring wsNameSoMachComp =
        L"Количество дисциплин, формирующих несколько компетенций";

    wstring wsNameQuartileRib = L"Количество рёбер указанного квартиля";

    // Вывод компетенций по количеству
    for (int iAmountComp = 1;
         iAmountComp <= this->ptrGlobal->ptrConfig->GetISoMachComp();
         ++iAmountComp)
    {
        arrAddedHead.push_back(wsNameSoMachComp);
    }

    for (int iQuar = 0; iQuar <= this->ptrGlobal->ptrConfig->GetIAmountQuar();
         ++iQuar)    // На один больше квартилей, так как есть отдельно то
                     // количество, сколько рёбер не соединены
    {
        arrAddedHead.push_back(wsNameQuartileRib);
    }

    // Флаги того, какие столбцы выводить
    vector<bool> arrOutColm;

    // Строка заголовка таблицы
    vector<string> arrHeader;
    CreateOnlyAllowedHeaderRow(arrHeader, arrOutColm, arrAddedHead,
                               arrOnlyAllowHead);
    arrReturnData.push_back(arrHeader);

    // Вывод данных
    // map<int, FСorridor> mapSaveСorridorData;
    FCorridorAdapter fСorridorData(ptrGlobal);

    for (const auto& it : ptrGlobal->ptrSolve->arrDisc)
    {
        // Нет нужного нам курса (например, если 3 курс, а план магистратуры)
        if (it->iAmountCourse <= iGraphType)
            continue;

        // Выводить короткое, или помное имя
        /*sCurPlanName = (ptrGlobal->ptrConfig->bOutShortNameCur)
                           ? it->sShortNamePlan
                           : it->sNamePlan;*/

        vector<double> arrAllResult;

        CreateTotalInfo(
            arrAllResult,
            make_shared<FGraphType>(it->ptrGraph->mapGraph[iGraphType]),
            EOutType::EOT_Head);

        vector<string> arrCurData;
        CreateOnlyAllowedResultRow(arrCurData, arrHeader.size(), arrOutColm,
                                   // sCurPlanName,
                                   arrAllResult, fСorridorData, it,
                                   arrOnlyAllowHead);

        arrReturnData.push_back(arrCurData);
    }

    AddTableCommonData(arrReturnData, fСorridorData);
}

void FOutData::Create(string sOutPath)
{
    OpenXLSX::XLDocument fOutFile;
    const string sPageName = ptrGlobal->ptrConfig->GetSNameFileTotalData(false);
    const string sCreatePath =
        sOutPath + "/" + ptrGlobal->ptrConfig->GetSNameFileTotalData();
    fOutFile.create(sCreatePath);
    fOutFile.workbook().addWorksheet(sPageName);
    fOutFile.workbook().deleteSheet("Sheet1");    // Стартовая страница не нужна
    OpenXLSX::XLWorksheet wks = fOutFile.workbook().worksheet(sPageName);

    int                    iShiftX = 1;
    vector<vector<string>> arrAllCurriculaTotalData;
    CreateAllCurriculaTotalData(arrAllCurriculaTotalData);
    OutTableInfo(iShiftX, 1, arrAllCurriculaTotalData, wks);
    if (ptrGlobal->ptrConfig->GetBIsOutCSVDate())
        OutTableInfoCSV(arrAllCurriculaTotalData, sOutPath, "", sPageName);

    iShiftX += arrAllCurriculaTotalData.front().size() - iSizeOnlyAllow;

    string sNamePageAllData = this->ptrGlobal->ConwertToString(
        ptrGlobal->ptrConfig
            ->mapArrOutParams
                [L"Предлог перед выводом статистики по всем курсам"]
            .GetName());

    if (ptrGlobal->ptrConfig->bArrIsconcatGraphData.at(0))
    {
        vector<vector<string>> arrAllCoursesGraphData;
        CreateSummaryTotalData(arrAllCoursesGraphData, FGraph::iCommon);
        OutTableInfo(iShiftX, 1, arrAllCoursesGraphData, wks,
                     iSizeOnlyAllow);    // iShiftDataX = 1, так как заголовки
                                         // УП

        if (ptrGlobal->ptrConfig->GetBIsOutCSVDate())
            OutTableInfoCSV(arrAllCoursesGraphData, sOutPath, "",
                            sNamePageAllData);

        // выводить не надо
        iShiftX += arrAllCoursesGraphData.front().size() -
                   iSizeOnlyAllow;    // -iEscape так как без заголовка УП
    }
    else
    {
        /*string sNamePage = this->ptrGlobal->ConwertToString(
            ptrGlobal->ptrConfig
                ->mapArrOutParams
                    [L"Предлог перед выводом статистики по всем курсам"]
                .GetName());*/

        /*string sNamePage = this->ptrGlobal->ConwertToString(
            ptrGlobal->ptrConfig->wsOutPrefAllCourse);*/

        fOutFile.workbook().addWorksheet(sNamePageAllData);
        wks = fOutFile.workbook().worksheet(sNamePageAllData);
        vector<vector<string>> arrAllCoursesGraphData;
        CreateSummaryTotalData(arrAllCoursesGraphData, FGraph::iCommon);
        OutTableInfo(1, 1, arrAllCoursesGraphData, wks);
        if (ptrGlobal->ptrConfig->GetBIsOutCSVDate())
            OutTableInfoCSV(arrAllCoursesGraphData, sOutPath, "",
                            sNamePageAllData);
    }

    for (int iCourse = 0; iCourse < this->ptrGlobal->ptrSolve->iMaxCourse;
         ++iCourse)
    {
        if (ptrGlobal->ptrConfig->bArrIsconcatGraphData.at(1))
        {
            vector<vector<string>> arrCourseGraphData;
            CreateSummaryTotalData(arrCourseGraphData, iCourse);
            OutTableInfo(iShiftX, 1, arrCourseGraphData, wks,
                         iSizeOnlyAllow);    // iShiftDataX = iEscape, так как
                                             // заголовки УП выводить не надо

            iShiftX += arrCourseGraphData.front().size() -
                       iSizeOnlyAllow;    // -iEscape так как без заголовка УП
        }
        else
        {
            string sNamePage = ptrGlobal->ConwertToString(
                ptrGlobal->ptrConfig
                    ->mapArrOutParams
                        [L"Предлог перед выводом статистики по курсу "
                         L"определённого номера"]
                    .GetName());

            sNamePage = sNamePage + "_" + to_string(iCourse + 1);
            fOutFile.workbook().addWorksheet(sNamePage);
            wks = fOutFile.workbook().worksheet(sNamePage);
            vector<vector<string>> arrCourseGraphData;
            CreateSummaryTotalData(arrCourseGraphData, iCourse);
            OutTableInfo(1, 1, arrCourseGraphData, wks);

            string sNameCSV = ptrGlobal->ReversUTF16RU(sNamePage);
            if (ptrGlobal->ptrConfig->GetBIsOutCSVDate())
                OutTableInfoCSV(arrCourseGraphData, sOutPath, "", sNameCSV);
        }
    }

    fOutFile.save();
    fOutFile.close();

    for (auto& it : ptrGlobal->ptrSolve->arrDisc)
    {
        // Выводить короткое, или помное имя
        sCurPlanName = (ptrGlobal->ptrConfig->GetBOutShortNameCur())
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

    // Выводим файл единых данных
    for (const auto& [sKey, arrData] : mapTotalDataOut)
    {
        OpenXLSX::XLDocument fTotalAddOutFile;
        const string         sPageName = "Total_" + sKey;

        string sNewFile = sOutPath + "/TotalCompData";

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

        fTotalAddOutFile.create(sOutPath + "/TotalCompData/" + sPageName +
                                ".xlsx");
        fTotalAddOutFile.workbook().addWorksheet(sPageName);
        fTotalAddOutFile.workbook().deleteSheet(
            "Sheet1");    // Стартовая страница не нужна
        OpenXLSX::XLWorksheet wks =
            fTotalAddOutFile.workbook().worksheet(sPageName);
        OutTableInfo(1, 1, arrData, wks);
        if (ptrGlobal->ptrConfig->GetBIsOutCSVDate())
            OutTableInfoCSV(arrData, sOutPath, "TotalCompData", sPageName);

        fTotalAddOutFile.save();
        fTotalAddOutFile.close();
    }
}

void FOutData::OutAddInfo(string sName, string sPath,
                          shared_ptr<FCurricula> ptrTree)
{
    auto& ptrCurrentTree =
        ptrTree->ptrMetric->ptrTreeMetric->mapChild[FMetric::sAllMetric];

    vector<vector<string>> arrDataAll;

   

    //CreateTableInfoInit(arrDataAll, ptrCurrentTree,
    //                   true);    //, ptrCurrentTree->dChosenSum);

    //Не вижу смысла делать исключение
    // Вывод данных в TotalData файле
    //OutTableInfo(1, 1, arrDataAll, fFileCache->arrOpenWKS.back());
    //if (ptrGlobal->ptrConfig->GetBIsOutCSVDate())
    //    OutTableInfoCSV(arrDataAll, sPath, sName, FMetric::sAllMetric);

    // Плохое решение было выводить всё в строчку
    // iXShift +=
    //     arrDataAll.front().size();    // Сдвигаемся на ширину выведеной
    //     таблицы

    int iXShift = 1;
    int iYShift = 1;
    for (auto& [sKey, ptrCurrentTree] :
         ptrTree->ptrMetric->ptrTreeMetric->mapChild)
    {
        //if (sKey == FMetric::sAllMetric)
        //   continue;    // Мы его уже ранее проверили

        vector<vector<string>> arrDataAllCourse;
        // Вывод конкретного курса
        CreateTableInfoInit(arrDataAllCourse, ptrCurrentTree,
                            true);    // ptrCurrentTree->dChosenSum);

        if (mapTotalDataOut[sKey]
                .empty())    // Если пусто, то копируем с заголовками
        {
            mapTotalDataOut[sKey] = arrDataAllCourse;
        }
        else
        {
            // Копируем без заголовка
            for (int y = 1; y < arrDataAllCourse.size(); ++y)
            {
                mapTotalDataOut[sKey].push_back(arrDataAllCourse[y]);
            }
        }

        // Выводим в cтолбец (Если iYShift > 1 , то без заголовка)
        OutTableInfo(1, iYShift, arrDataAllCourse,
                     fFileCache->arrOpenWKS.back(), 0, min(iYShift-1,1));

        // Выводим в ширину (глупое решение)
        // OutTableInfo(iXShift, 1, arrDataAllCourse,
        //             fFileCache->arrOpenWKS.back());

        if (ptrGlobal->ptrConfig->GetBIsOutCSVDate())
            OutTableInfoCSV(arrDataAllCourse, sPath, sName,
                            ptrGlobal->ptrConfig->GetSNameFileCompData() +
                                ptrGlobal->ptrConfig->GetSSeparator() + sKey);

        // iXShift += arrDataAllCourse.front()
        //                .size();    // Сдвигаемся на ширину выведеной таблицы
        // iYShift = max(iYShift + 0ull, arrDataAllCourse.size() + 0ull);

        iYShift +=
            arrDataAllCourse.size() - 1;    // Сдвигаемся на высоту выведеной
                                            // таблицы (минус строка заголовка)

        {
            fFileCache->arrCourseOpenFile.clear();
            fFileCache->arrCourseOpenFile.resize(1);
            fFileCache->arrCourseOpenWKS.clear();
            fFileCache->arrCourseOpenWKS.resize(1);
            string sFullPathName =
                sPath + "/" + sName + "/" +
                ptrGlobal->ptrConfig->GetSNameFileCompData() +
                ptrGlobal->ptrConfig->GetSSeparator() + sKey + XLSX;
            fFileCache->arrCourseOpenFile[0].create(sFullPathName);
            fFileCache->arrCourseOpenFile[0].workbook().addWorksheet(sName);
            fFileCache->arrCourseOpenFile[0].workbook().deleteSheet(
                "Sheet1");    // Стартовая страница не нужна
            fFileCache->arrCourseOpenFile[0].save();
            fFileCache->arrCourseOpenWKS[0] =
                fFileCache->arrCourseOpenFile[0].workbook().worksheet(sName);
            // Теперь это выводится в OutAddInfoInit

            // Выводим локально в отдельный файл

            vector<vector<string>> arrDataAllCourseOnFile;
            // Вывод конкретного курса
            CreateTableInfoInit(arrDataAllCourseOnFile, ptrCurrentTree, true);

            OutTableInfo(1,
                         1,
                         arrDataAllCourseOnFile,
                         fFileCache->arrCourseOpenWKS.back());

            if (ptrGlobal->ptrConfig->GetBOutTotalInfo())
            {
                vector<vector<string>> arrTotalCourseGraphData;
                CreateTotalInfo(arrTotalCourseGraphData,
                                make_shared<FGraphType>(
                                    ptrTree->ptrGraph->mapGraph[sKey[0] - '1']),
                                EOutType::EOT_Added);

                OutTableInfo(1,    // Так как 1-индексация
                             arrDataAllCourse.size() +
                                 1,    // Так как 1-индексация
                             arrTotalCourseGraphData,
                             fFileCache->arrCourseOpenWKS.back());
            }

            fFileCache->arrCourseOpenFile.back().save();
            fFileCache->arrCourseOpenFile.back().close();

            // Очищаем память от использованных объектов
            fFileCache->arrCourseOpenFile.clear();
            fFileCache->arrCourseOpenWKS.clear();
        }
    }

    if (ptrGlobal->ptrConfig->GetBOutTotalInfo())
    {
        vector<vector<string>> arrTotalGraphData;
        CreateTotalInfo(arrTotalGraphData,
                        make_shared<FGraphType>(
                            ptrTree->ptrGraph->mapGraph[FGraph::iCommon]),
                        EOutType::EOT_Added);
        OutTableInfo(1, iYShift + 1,    // Так как 1-индексация
                     arrTotalGraphData, fFileCache->arrOpenWKS.back());
    }

    {
        // Новая визуализация
        map<string, map<string, map<string, string>>> mapGraphE1;
        for (auto& [sKey, ptrCurrentTree] :
             ptrTree->ptrMetric->ptrTreeMetric->mapChild)
        {
            if (sKey == FMetric::sAllMetric)
                continue;
            map<string, map<string, string>> fBuf;
            // Вывод конкретного курса
            CreateGraphE1TableInfoInit(fBuf, ptrCurrentTree);
            mapGraphE1[sKey] = fBuf;
        }

        // Нужно посмотреть по всем курсам и посмотреть общее пересечение

        map<pair<string, string>, int> mapAllComp;
        for (const auto& [sCourse, it] : mapGraphE1)
        {
            for (const auto& [sComp, et] : it)
            {
                for (const auto& [sNum, sVal] : et)
                {
                    if (!mapAllComp.count({ sComp, sNum }))
                        mapAllComp[{ sComp, sNum }] = 0;
                }
            }
        }
        int x = 0;
        for (auto& [key, val] : mapAllComp)
            val = x++;

        vector<vector<string>> arrData(mapAllComp.size(),
                                       vector<string>(mapGraphE1.size() + 2));

        {
            int y = 1;
            for (const auto& [sCourse, it] : mapGraphE1)
            {
                ++y;
                for (const auto& [sComp, et] : it)
                {
                    for (const auto& [sNum, sVal] : et)
                    {
                        int x         = mapAllComp[{ sComp, sNum }];
                        arrData[x][y] = sVal;
                        arrData[x][0] = sComp;
                        arrData[x][1] = sNum;
                    }
                }
                arrData[arrData.size() - 1][y] = sCourse;
            }
        }

        // Добавим обозначение того, что выводим курсы
        if (ptrGlobal->ptrConfig->mapAddOutParams.count(L"За какой курс"))
            arrData.back().front() = ptrGlobal->ConwertToString(
                ptrGlobal->ptrConfig->mapAddOutParams[L"За какой курс"]
                    .GetName());

        fFileCache->arrCourseOpenFile.clear();
        fFileCache->arrCourseOpenFile.resize(1);
        fFileCache->arrCourseOpenWKS.clear();
        fFileCache->arrCourseOpenWKS.resize(1);

        fFileCache->arrCourseOpenFile[0].create(sPath + "/" + sName + "/" +
                                                "E1Diagram.xlsx");
        fFileCache->arrCourseOpenFile[0].workbook().addWorksheet("E1Diagram");
        fFileCache->arrCourseOpenFile[0].workbook().deleteSheet(
            "Sheet1");    // Стартовая страница не нужна
        fFileCache->arrCourseOpenFile[0].save();
        fFileCache->arrCourseOpenWKS[0] =
            fFileCache->arrCourseOpenFile[0].workbook().worksheet("E1Diagram");
        // Теперь это выводится в OutAddInfoInit

        // Выводим локально в отдельный файл
        OutTableInfo(1, 1, arrData, fFileCache->arrCourseOpenWKS.back());

        fFileCache->arrCourseOpenFile.back().save();
        fFileCache->arrCourseOpenFile.back().close();

        // Очищаем память от использованных объектов
        fFileCache->arrCourseOpenFile.clear();
        fFileCache->arrCourseOpenWKS.clear();
    }

    fFileCache->arrOpenFile.back().save();
    fFileCache->arrOpenFile.back().close();

    // Очищаем память от использованных объектов
    fFileCache->arrOpenFile.clear();
    fFileCache->arrOpenWKS.clear();
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

void FOutData::OutTableInfoCSV(
    const vector<vector<string>>& arrData,    // Что выводим
    const string& sPath, const string& sNameFile, const string& sName,
    const int& iShiftDataX,    // С каким смещением выводим
    const int& iShiftDataY)    // С каким смещением выводим)
{
    string sAllPath = sPath;

    if (sNameFile != "")
        sAllPath += "/" + sNameFile;
    if (sName != "")
        sAllPath += "/" + sName;
    ofstream outLabel(sAllPath + ".csv");
    for (int y = iShiftDataY; y < arrData.size(); ++y)
    {
        for (int x = iShiftDataX; x < arrData[y].size(); ++x)
        {
            if (x)
                outLabel << ";";
            outLabel << arrData[y][x];
        }
        outLabel << "\n";
    }
}

void FOutData::CreateTableInfoInit(vector<vector<string>>& arrReturnData,
                                   shared_ptr<FTreeMetric>
                                        ptrMetric,
                                   bool bIsOutNameCur
                                   // const double            dAllSum,
                                   // bool bIsLocal
)
{
    deque<wstring> arrCompetenceHead(
        { // L"Название УП",
          L"За какой курс",

          L"Заголовок компетенции", L"ЗЕ Заголовка компетенции",
          L"Кол-во дисциплин заголовка компетенции",

          L"Процент распределения Заголовка компетенции",

          L"Компетенция", L"Полное название компетенции", L"ЗЕ Компетенций",
          L"Кол-во дисциплин компетенции", L"Процент распределения Компетенции",

          L"Индикатор", L"Полное название индикатора", L"ЗЕ индикаторов",
          L"Кол-во дисциплин индикатора",
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
                        arrAllData, iShiftX, iSizeX,
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
            if (x >= iSizeX)
                break;
            if (this->ptrGlobal->ptrConfig->mapAddOutParams.count(it))
            {
                arrAllData[0][x] = this->ptrGlobal->ConwertToString(
                    this->ptrGlobal->ptrConfig->mapAddOutParams[it].GetName());
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

            bool bIsTakeNoEmpty = false;
            for (const auto& it : arrCompetenceHead)
            {
                ++x;

                if (x >= iSizeX)
                    break;
                if (this->ptrGlobal->ptrConfig->mapAddOutParams.count(it))
                {
                    if (ptrGlobal->ptrConfig->mapAddOutParams[it].GetTotal())
                    {
                        if ((y == 1) && (it == L"Название УП"))
                            arrAllData[y][x] =
                                sCurPlanName;    // Если у нас самая верхняя
                                                 // правая позиция данных,
                                                 // выводим заголовок УП
                                                 // (костыль, надо бы push
                                                 // front, но это уже когда
                                                 // добавлю adapOutData

                        arrBuf.push_back(arrAllData[y][x]);
                        if (arrAllData[y][x] != "")
                        {
                            bIsTakeNoEmpty = true;
                        }
                    }
                }
            }
            if (bIsTakeNoEmpty)
                arrReturnData.push_back(arrBuf);
        }
    }

    if (ptrGlobal->ptrConfig->GetBOutWithoutEmptyCell())
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
    int x, int& iSizeX, int& iCurrentY, shared_ptr<FTreeMetric> ptrMetric,
    int iDeep
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

    // Выводим полное название компетенции
    if (iDeep > 1)
    {
        ++x;
        if (!bIsCounting)
        {
            vector<string> arrName;
            auto           ptrBuf = ptrMetric;
            for (int i = 0; i < iDeep; ++i)
            {
                arrName.push_back(ptrBuf->sName);
                ptrBuf = ptrBuf->ptrParent;
            }
            for (int i = iDeep - 1; i >= 0; --i)
            {
                arrReturnData[iCurrentY][x] += arrName[i];
                if (i)
                    arrReturnData[iCurrentY][x] +=
                        ptrGlobal->ptrConfig->GetSPrefFullNameCourse();
            }
        }
    }

    // Выводим кол-во ЗЕ
    if (iDeep > 0)
    {
        ++x;
        if (!bIsCounting)
        {
            double dRes = 0.;
            dRes        = (ptrGlobal->ptrConfig->GetBCompInterDelete())
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
            if (ptrGlobal->ptrConfig->GetBIsPercentRegAll())
            {
                dRes = (ptrGlobal->ptrConfig->GetBCompInterDelete())
                           ? ptrMetric->dBalanceSum /
                                 ptrMetric->ptrParent->dBalanceSum
                           : ptrMetric->dNoBalanceSum /
                                 ptrMetric->ptrParent->dNoBalanceSum;
            }
            else
            {
                dRes = (ptrGlobal->ptrConfig->GetBCompInterDelete())
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
    fFileCache->arrOpenFile.clear();
    fFileCache->arrOpenFile.resize(1);

    fFileCache->arrOpenWKS.clear();
    fFileCache->arrOpenWKS.resize(1);

    if (ptrGlobal->ptrConfig->GetBCompactOutput())
    {
        fFileCache->arrOpenFile[0].open(
            sPath + "/" + ptrGlobal->ptrConfig->GetSNameFileTotalData());
        if (fFileCache->arrOpenFile[0].workbook().worksheetExists(sName))
            fFileCache->arrOpenFile[0].workbook().deleteSheet(sName);

        fFileCache->arrOpenFile[0].workbook().addWorksheet(sName);
        fFileCache->arrOpenWKS[0] =
            fFileCache->arrOpenFile[0].workbook().worksheet(sName);
        return;
    }
    else
    {
        fFileCache->arrOpenFile[0].create(
            sPath + "/" + sName + "/" +
            ptrGlobal->ptrConfig->GetSNameFileLocalData());
        fFileCache->arrOpenFile[0].workbook().addWorksheet(sName);
        fFileCache->arrOpenFile[0].workbook().deleteSheet(
            "Sheet1");    // Стартовая страница не нужна
        fFileCache->arrOpenFile[0].save();
        fFileCache->arrOpenWKS[0] =
            fFileCache->arrOpenFile[0].workbook().worksheet(sName);
    }
}

string FOutData::AddCompString(const map<string, vector<string>>& mapComp)
{
    string sReturn = "(";
    int    j       = -1;
    for (const auto& [sNameComp, arrIndicator] : mapComp)
    {
        ++j;
        if (j)
            sReturn += ",";
        sReturn += sNameComp;
    }
    sReturn += ")";
    return sReturn;
}

vector<string> FOutData::CreateTag(const int& iGraphType,
                                   shared_ptr<FCurricula>
                                        fCurricula,
                                   bool bCheckTag)
{
    vector<string> arrTag;
    for (auto& [key, val] :
         fCurricula->ptrGraph->mapGraph[iGraphType].mapReversRel)
    {
        string sTag = ptrGlobal->ptrConfig->GetSNoInitData();
        if (bCheckTag)
        {
            if (fCurricula->mapAllDisc[key.first]->setTagDisc.size() > 0)
                sTag = to_string(
                    *fCurricula->mapAllDisc[key.first]->setTagDisc.begin());
        }
        arrTag.push_back(sTag);
    }

    return arrTag;
}

vector<string> FOutData::CreateCommonNameLabel(const int& iGraphType,
                                               shared_ptr<FCurricula>
                                                   fCurricula)
{
    vector<string> arrCommonNameLabel;
    for (auto& it : fCurricula->ptrGraph->mapGraph[iGraphType].arrRel)
    {
        if (iGraphType == FGraph::iReverse)
        {
            arrCommonNameLabel.push_back(ptrGlobal->ConwertToString(it.first));
            continue;
        }

        shared_ptr<FTreeElement> fThis = fCurricula->mapAllDisc[it.first];
        // wstring       wsNameRaw = fThis->wsName;
        string sName = fThis->sName;

        // Не забываем про нуль нумерацию курсов
        if (iGraphType == FGraph::iAlt)
        {
            sName += ptrGlobal->ptrConfig->GetSSeparator() +
                     to_string(it.second + 1);
        }

        // ptrGlobal->ReversUTF16RU(ptrGlobal->ConwertToString(wsNameRaw));
        // Выводим ещё и компетенции
        if (ptrGlobal->ptrConfig->GetBOutCompWithName())
        {
            sName += AddCompString(fThis->mapComp);
        }
        arrCommonNameLabel.push_back(sName);
    }
    return arrCommonNameLabel;
}

void FOutData::OutGephiData(string sName, string sPath,
                            shared_ptr<FCurricula> fCurricula)
{
    {
        const int& iTag = FGraph::iCommon;
        OutGephiLabel(sPath, sName, sName,
                      CreateCommonNameLabel(iTag, fCurricula),
                      fCurricula->ptrGraph->mapGraph[iTag].arrNodeWeight,
                      CreateTag(iTag, fCurricula));
        OutGephiRib(sPath, sName, sName,
                    fCurricula->ptrGraph->mapGraph[iTag].fAdjList);
    }

    // Вывод графа, где вершины - это компетенции
    {
        const int& iTag = FGraph::iReverse;
        string     sFileName =
            sName + ptrGlobal->ConwertToString(
                        ptrGlobal->ptrConfig
                            ->mapArrOutParams
                                [L"Суффикс названия обратного графа при выводе"]
                            .GetName());

        OutGephiLabel(sPath, sFileName, sName,
                      CreateCommonNameLabel(iTag, fCurricula),
                      fCurricula->ptrGraph->mapGraph[iTag].arrNodeWeight,
                      CreateTag(iTag, fCurricula, false));
        OutGephiRib(sPath, sFileName, sName,
                    fCurricula->ptrGraph->mapGraph[iTag].fAdjList);
    }

    {
        const int& iTag = FGraph::iAlt;
        string     sFileName =
            sName +
            ptrGlobal->ConwertToString(
                ptrGlobal->ptrConfig
                    ->mapArrOutParams[L"Суффикс названия альтернативного "
                                      L"графа при выводе"]
                    .GetName());

        OutGephiLabel(sPath, sFileName, sName,
                      CreateCommonNameLabel(iTag, fCurricula),
                      fCurricula->ptrGraph->mapGraph[iTag].arrNodeWeight,
                      CreateTag(iTag, fCurricula));

        OutGephiRib(sPath, sFileName, sName,
                    fCurricula->ptrGraph->mapGraph[iTag].fAdjList);
    }

    if (fCurricula->ptrGlobal->ptrConfig->GetBCourseOutput())
    {
        for (int iCourse = 0; iCourse < fCurricula->iAmountCourse; ++iCourse)
        {
            OutGephiLabel(sPath,
                          sName + "(" + to_string(iCourse + 1) + ")",
                          sName,
                          CreateCommonNameLabel(iCourse, fCurricula),
                          fCurricula->ptrGraph->mapGraph[iCourse].arrNodeWeight,
                          CreateTag(iCourse, fCurricula));
            OutGephiRib(sPath,
                        sName + "(" + to_string(iCourse + 1) + ")",
                        sName,
                        fCurricula->ptrGraph->mapGraph[iCourse].fAdjList);
        }
    }
}

void FOutData::OutGephiLabel(const string& sPath, const string& sNameFile,
                             const string&         sName,
                             const vector<string>& arrNameLabel,
                             const vector<double>& arrWeightNode,
                             const vector<string>& arrTag)
{
    ofstream outLabel(sPath + "/" + sName + "/" + sNameFile + "Label.csv");

    outLabel << ptrGlobal->ptrConfig->GetSNameLabelHeader() << "\n";

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

void FOutData::OutGephiRib(const string& sPath, const string& sNameFile,
                           const string&                            sName,
                           const vector<vector<pair<int, double>>>& fAdjList)
{
    ofstream outLabel(sPath + "/" + sName + "/" + sNameFile + "Rib.csv");

    outLabel << ptrGlobal->ptrConfig->GetSNameRibHeader() << "\n";

    // Откуда, куда, тип (неориентированный), Вес
    for (int l = 0; l < fAdjList.size(); ++l)
    {
        for (const auto& [r, dLen] : fAdjList[l])
        {
            // Чтобы не дублировать, он же неориентированный
            if ((l < r) || (!ptrGlobal->ptrConfig->GetBIsUnDirected()))
            {
                outLabel << l << ";";
                outLabel << r << ";";
                outLabel << ptrGlobal->ptrConfig->GetSNameRibDir() << ";";
                outLabel << dLen << ";";    // Вывод веса ребра в качестве метки
                outLabel << dLen << "";
                outLabel << "\n";
            }
        }
    }
}

void FOutData::RetakeCommon(FCorridorAdapter& fSaveData, const int& iKey,
                            const double& dNewData, const string& sNewData)
{
    fSaveData.Add(iKey, { dNewData, sNewData });
    /*  if ((dNewData > fSaveData.dMaxMin[0]) || (fSaveData.sMaxMin[0] == ""))
      {
          fSaveData.dMaxMin[0] = dNewData;
          fSaveData.sMaxMin[0] = sNewData;
      }

      if ((dNewData < fSaveData.dMaxMin[1]) || (fSaveData.sMaxMin[1] == ""))
      {
          fSaveData.dMaxMin[1] = dNewData;
          fSaveData.sMaxMin[1] = sNewData;
      }*/
    return;
}

bool FOutData::TakePasteData(const int& x, vector<string>& arrCurRow,
                             const bool& bIsOutData, const double& dDate,
                             const string& sOutData, const string& sCurName,
                             const bool&       bIsConsider,
                             FCorridorAdapter& fSaveData)
{
    if (bIsOutData)
    {
        if (bIsConsider)
            RetakeCommon(fSaveData, x, dDate, sCurName);

        arrCurRow[x] = ptrGlobal->ConwertUTF16RU(sOutData);
        return true;
    }
    return false;
}

void FOutData::CreateGraphE1TableInfoInit(
    map<string, map<string, string>>& fReturnData,
    shared_ptr<FTreeMetric>
        ptrMetric)
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
          L"Процент распределения Индикатора" });    // Свой независимый от
                                                     // других вывод

    for (int y = 0; y < arrAllData.size(); ++y)
    {
        int x = -1;

        for (const auto& it : arrCompetenceHead)
        {
            ++x;
            if (x >= iSizeX)
                break;
            if (it == L"Заголовок компетенции")
            {
                if (arrAllData[y][x] != "")
                    sHeadComp = arrAllData[y][x];
            }
            if (it == L"Компетенция")
            {
                if (arrAllData[y][x] != "")
                    sComp = arrAllData[y][x];
            }
            if (it == L"ЗЕ Компетенций")
            {
                if (arrAllData[y][x] != "")
                    sScore = arrAllData[y][x];
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
    int x, int& iSizeX, int& iCurrentY, shared_ptr<FTreeMetric> ptrMetric,
    int iDeep
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
            dRes        = (ptrGlobal->ptrConfig->GetBCompInterDelete())
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
            if (ptrGlobal->ptrConfig->GetBIsPercentRegAll())
            {
                dRes = (ptrGlobal->ptrConfig->GetBCompInterDelete())
                           ? ptrMetric->dBalanceSum /
                                 ptrMetric->ptrParent->dBalanceSum
                           : ptrMetric->dNoBalanceSum /
                                 ptrMetric->ptrParent->dNoBalanceSum;
            }
            else
            {
                dRes = (ptrGlobal->ptrConfig->GetBCompInterDelete())
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

FOutData::~FOutData()
{
    fFileCache.reset();
    --iSinglControll;
}