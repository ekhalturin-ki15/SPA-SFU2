#include "adapOutData.h"

#include "config.h"
#include "error.h"
#include "graph.h"
#include "metric.h"
#include "solve.h"

#include <typeinfo>

int              FAdapOutData::iSinglControll = 0;
const ETypeGraph FAdapOutData::ETG_Total(-10);

FAdapOutData::FAdapOutData(shared_ptr<FGlobal> _ptrGlobal)
    : ptrGlobal(_ptrGlobal),

      arrOriginMetricTotalHead(
          { L"Название учебного плана", L"Тип учебного плана",
            L"Год начала подготовки", L"Код направления",
            L"Общее кол-во ЗЕ в УП", L"!ЗЕ основных дисциплин",
            L"!ЗЕ дисциплин по выбору", L"!ЗЕ факультативов",
            L"Общее кол-во дисциплин в УП", L"!Количество основных дисциплин",
            L"!Количество дисциплин по выбору", L"!Количество факультативов" }),
      arrOriginMetricGraphHead(
          { L"Всего ЗЕ в графе", L"ЗЕ факультативов", L"ЗЕ основных дисциплин",
            L"ЗЕ дисциплин по выбору",

            L"Кол-во дисциплин в графе", L"Количество основных дисциплин",
            L"Количество дисциплин по выбору", L"Количество факультативов",

            L"Максимальное ЗЕ у дисциплины", L"Минимальное ЗЕ у дисциплины",

            L"Максимальный вес ребра", L"Минимальный вес ребра",

            L"Диаметр графа по расстоянию",
            L"Диаметр графа по количеству рёбер",
            L"Количество компонент связности",

            L"Максимальное оставное дерево", L"Минимальное оставное дерево",
            L"Плотность графа", L"Коэффициент кластеризации (Глобальный)" }),
      arrOriginQuartileHead(
          { { L"Количество рёбер указанного квартиля для всей выборки",
              L"Суффикс после вывода квартиля" },

            { L"Локальное количество рёбер указанного квартиля",
              L"Суффикс после вывода квартиля" } }),
      arrCorridorHeader({ L"Максимальные значения:", L"Минимальные значения:",
                          L"Мода:", L"Медиана:", L"Среднее усечённое:" })
{
    // Unit test против такого
    // if (iSinglControll > 0) throw std::runtime_error("Re-creation
    // Singleton");
    ++iSinglControll;
}

bool FAdapOutData::Init()
{
    return true;
}

void FAdapOutData::CreateTotalHeader()
{
    auto& fTotalOutHeader = mapOutData[ETG_Total].arrHeader;

    for (const auto& wsNameHeader : arrOriginMetricTotalHead)
    {
        if (ptrGlobal->ptrConfig->mapArrOutParams.count(
                wsNameHeader))    // Если правильно указано название в
                                  // параметрах
        {
            fTotalOutHeader.push_back(ptrGlobal->ConwertToString(
                ptrGlobal->ptrConfig->mapArrOutParams[wsNameHeader].GetName()));
        }
    }

    wstring wsName = L"ЗЕ у компетенции";
    if (ptrGlobal->ptrConfig->mapArrOutParams.count(wsName))
    {
        for (const auto& sCompName : ptrGlobal->ptrSolve->setHeaderComp)
        {
            wstring wsCompScore = ptrGlobal->ConwertToWstring(sCompName);
            wsCompScore =
                ptrGlobal->ptrConfig->mapArrOutParams[wsName].GetName() +
                wsCompScore;
            fTotalOutHeader.push_back(ptrGlobal->ConwertToString(wsCompScore));
            fTotalOutHeader.push_back(sCompName);
        }
    }

    mapOutData[ETG_Total].arrType.resize(fTotalOutHeader.size());
}

void FAdapOutData::CreateGraphHeader()
{
    vector<pair<string, string>> arrQuartileHead;

    for (const auto& [wsNamePref, wsNameSuf] : arrOriginQuartileHead)
    {
        if (ptrGlobal->ptrConfig->mapArrOutParams.count(wsNamePref))
        {
            arrQuartileHead.push_back(
                { ptrGlobal->ConwertToString(
                      ptrGlobal->ptrConfig->mapArrOutParams[wsNamePref]
                          .GetName()),
                  ptrGlobal->ConwertToString(
                      ptrGlobal->ptrConfig->mapArrOutParams[wsNameSuf]
                          .GetName()) });
        }
    }

    for (auto& [eType, fData] : mapOutData)
    {
        if (eType == ETG_Total)
            continue;    // Для общей статистики особый заголовок
        for (const auto& wsNameHeader : arrOriginMetricGraphHead)
        {
            if (ptrGlobal->ptrConfig->mapArrOutParams.count(wsNameHeader))
            {
                fData.arrHeader.push_back(ptrGlobal->ConwertToString(
                    ptrGlobal->ptrConfig->mapArrOutParams[wsNameHeader]
                        .GetName()));
            }
        }

        CompHeaderCreate(fData.arrHeader);

        QuartileHeaderCreate(fData.arrHeader, arrQuartileHead);

        fData.arrType.resize(fData.arrHeader.size());
    }
}

void FAdapOutData::QuartileHeaderCreate(
    vector<string>&                     arrHeader,
    const vector<pair<string, string>>& arrQuartileHead)
{
    for (int i = 1; i <= arrQuartileHead.size(); ++i)
    {
        const auto& [sPref, sSuf] = arrQuartileHead[i - 1];

        for (int iAmountQuartile = 1;
             iAmountQuartile <= ptrGlobal->ptrConfig->GetIAmountQuar() +
                                    (i == arrQuartileHead.size());
             ++iAmountQuartile)
        {
            // Последний квартиль отвечает за вывод отсутствующих путей
            if (iAmountQuartile > ptrGlobal->ptrConfig->GetIAmountQuar())
            {
                arrHeader.push_back(
                    "No" + ptrGlobal->ptrConfig->GetSSeparator() + "Path");
                continue;
            }

            string sAddedHead = sPref;

            sAddedHead += ptrGlobal->ptrConfig->GetSSeparator();

            sAddedHead += to_string(iAmountQuartile) +
                          ptrGlobal->ptrConfig->GetSSeparator();

            sAddedHead += sSuf;

            arrHeader.push_back(sAddedHead);
        }
    }
}

void FAdapOutData::CompHeaderCreate(vector<string>& arrHeader)
{
    wstring wsNameSoMachComp =
        L"Количество дисциплин, формирующих несколько компетенций";

    if (ptrGlobal->ptrConfig->mapArrOutParams.count(wsNameSoMachComp))
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

            arrHeader.push_back(sAddedHead);
        }
    }
}

void FAdapOutData::CreateHeader()
{
    mapOutData[ETypeGraph::ETG_Common];
    mapOutData[ETypeGraph::ETG_Alt];
    mapOutData[ETypeGraph::ETG_Reverse];

    for (int iCourse = 0; iCourse < ptrGlobal->ptrSolve->iMaxCourse; ++iCourse)
    {
        mapOutData[ETypeGraph(iCourse)];
    }

    CreateGraphHeader();

    CreateTotalHeader();
}

void FAdapOutData::CreateTotalData()
{
    auto& fTotalOutData = mapOutData[ETG_Total].arrData;

    for (int iCur = 0; iCur < ptrGlobal->ptrSolve->N; ++iCur)
    {
        const auto& it = ptrGlobal->ptrSolve->GetCurricula(iCur);

        vector<FTableData> arrRow;
        for (int iColumnNum = 0; iColumnNum < arrOriginMetricTotalHead.size();
             ++iColumnNum)
        {
            auto& wsNameHeader = arrOriginMetricTotalHead[iColumnNum];
            if (ptrGlobal->ptrConfig->mapArrOutParams.count(wsNameHeader))
            {
                FTableData fDataContainer;
                switch (iColumnNum)
                {
                    case 0:
                        fDataContainer.fData = it->sCurName;
                        break;
                    case 1:
                        fDataContainer.fData = it->sTypePlan;
                        break;
                    case 2:
                        fDataContainer.fData = it->iYearStart;
                        break;
                    case 3:
                        fDataContainer.fData = it->iCodeUGSN;
                        break;
                    case 4:
                    case 5:
                    case 6:
                    case 7:
                        if (it->mapETMTypeDisc[ETM_NoExtended].count(
                                ETypeDisc(iColumnNum - 4)))
                        {
                            fDataContainer.fData =
                                it->mapETMTypeDisc[ETM_NoExtended]
                                    .at(ETypeDisc(iColumnNum - 4))
                                    .dCredits;
                        }
                        else
                        {
                            // fDataContainer.fData = FTypeGraph::dNoInit;
                        }
                        break;
                    case 8:
                    case 9:
                    case 10:
                    case 11:
                        if (it->mapETMTypeDisc[ETM_NoExtended].count(
                                ETypeDisc(iColumnNum - 8)))
                        {
                            fDataContainer.fData =
                                it->mapETMTypeDisc[ETM_NoExtended]
                                    .at(ETypeDisc(iColumnNum - 8))
                                    .iAmount;
                        }
                        else
                        {
                            // fDataContainer.fData = FTypeGraph::dNoInit;
                        }
                        break;
                }

                if (fDataContainer.fData.has_value())
                    mapOutData[ETG_Total].arrType[arrRow.size()] =
                        fDataContainer.fData.type().name();

                arrRow.push_back(fDataContainer);
            }
        }

        wstring wsName = L"ЗЕ у компетенции";
        if (ptrGlobal->ptrConfig->mapArrOutParams.count(wsName))
        {
            for (auto& sHeaderComp : ptrGlobal->ptrSolve->setHeaderComp)
            {
                FTableData fCredit  = { FTypeGraph::dINF };
                FTableData fPercent = { FTypeGraph::dINF };

                if (it->ptrMetric)
                {
                    auto ptrTreeMetric = it->ptrMetric->ptrTreeMetric
                                             ->mapChild[FMetric::sAllMetric];
                    double dScore = 0.;
                    double dRes   = 0.;
                    if (ptrTreeMetric->mapChild.count(sHeaderComp))
                    {
                        dScore =
                            ptrTreeMetric->mapChild[sHeaderComp]->dChosenSum;
                        dRes = ptrTreeMetric->mapChild[sHeaderComp]
                                   ->dInclusionPercent;
                    }
                    else
                    {
                        dScore = 0.;
                        dRes   = 0.;
                    }

                    // Если ЗЕ равно 0, то и не выводить
                    if (dScore > 0)
                    {
                        fCredit.fData = dScore;
                    }
                    else
                    {
                        // fCredit.fData = FTypeGraph::dNoInit;
                    }

                    if (dRes > ptrGlobal->ptrConfig->GetDMinComp())
                    {
                        fPercent.fData = dRes * 100;
                    }
                    else
                    {
                        // fPercent.fData = FTypeGraph::dNoInit;
                    }
                }

                if (fCredit.fData.has_value())
                    mapOutData[ETG_Total].arrType[arrRow.size()] =
                        fCredit.fData.type().name();
                arrRow.push_back(fCredit);

                if (fPercent.fData.has_value())
                    mapOutData[ETG_Total].arrType[arrRow.size()] =
                        fPercent.fData.type().name();
                arrRow.push_back(fPercent);
            }

            fTotalOutData.push_back(arrRow);
        }
    }
}

void FAdapOutData::CreateGraphData()
{
    for (auto& [eType, fData] : mapOutData)
    {
        if (eType == ETG_Total)
            continue;    // Для общей статистики особый заголовок

        auto& fTotalOutData = mapOutData[eType].arrData;

        for (int iCur = 0; iCur < ptrGlobal->ptrSolve->N; ++iCur)
        {
            const auto& it = ptrGlobal->ptrSolve->GetCurricula(iCur);

            const auto& fGraph = it->ptrGraph->mapGraph[eType];

            vector<FTableData> arrRow;

            for (int iColumnNum = 0;
                 iColumnNum < arrOriginMetricGraphHead.size();
                 ++iColumnNum)
            {
                FTableData fDataContainer;
                auto&      wsNameHeader = arrOriginMetricGraphHead[iColumnNum];
                if (ptrGlobal->ptrConfig->mapArrOutParams.count(wsNameHeader))
                {
                    switch (iColumnNum)
                    {
                        case 0:
                        case 1:
                        case 2:
                        case 3:
                            if (fGraph.mapGraphDataTypeDisc.count(
                                    ETypeDisc(iColumnNum)))
                            {
                                fDataContainer.fData =
                                    fGraph.mapGraphDataTypeDisc
                                        .at(ETypeDisc(iColumnNum))
                                        .dCredits;
                            }
                            else
                            {
                                // fDataContainer.fData = FTypeGraph::dNoInit;
                            }
                            break;
                        case 4:
                        case 5:
                        case 6:
                        case 7:
                            if (fGraph.mapGraphDataTypeDisc.count(
                                    ETypeDisc(iColumnNum - 4)))
                            {
                                fDataContainer.fData =
                                    fGraph.mapGraphDataTypeDisc
                                        .at(ETypeDisc(iColumnNum - 4))
                                        .iAmount;
                            }
                            else
                            {
                                // fDataContainer.fData = FTypeGraph::dNoInit;
                            }
                            break;
                        case 8:
                            fDataContainer.fData = fGraph.dMaxDiscScore;
                            break;
                        case 9:
                            fDataContainer.fData = fGraph.dMinDiscScore;
                            break;
                        case 10:
                            fDataContainer.fData = fGraph.dMaxRib;
                            break;
                        case 11:
                            fDataContainer.fData = fGraph.dMinRib;
                            break;
                        case 12:
                            fDataContainer.fData = fGraph.dDiametrLen;
                            break;
                        case 13:
                            fDataContainer.fData = fGraph.dDiametrStep;
                            break;
                        case 14:
                            fDataContainer.fData = fGraph.iComponent;
                            break;
                        case 15:
                            fDataContainer.fData = fGraph.dMaxSpanTree;
                            break;
                        case 16:
                            fDataContainer.fData = fGraph.dMinSpanTree;
                            break;
                        case 17:
                            fDataContainer.fData = fGraph.dDense;
                            break;
                        case 18:
                            fDataContainer.fData = fGraph.dGlobalСluster;
                            break;
                    }

                    if (fDataContainer.fData.has_value())
                        fData.arrType[arrRow.size()] =
                            fDataContainer.fData.type().name();
                    arrRow.push_back(fDataContainer);
                }
            }

            wstring wsNameSoMachComp =
                L"Количество дисциплин, формирующих несколько "
                L"компетенций";

            if (ptrGlobal->ptrConfig->mapArrOutParams.count(wsNameSoMachComp))
            {
                for (int iAmountComp = 1;
                     iAmountComp <=
                     this->ptrGlobal->ptrConfig->GetISoMachComp();
                     ++iAmountComp)
                {
                    FTableData fDataContainer;
                    fDataContainer.fData =
                        fGraph.arrAmountCountCompDisc[iAmountComp];

                    if (fDataContainer.fData.has_value())
                        fData.arrType[arrRow.size()] =
                            fDataContainer.fData.type().name();
                    arrRow.push_back(fDataContainer);
                }
            }

            for (int iColumnNum = 0; iColumnNum < arrOriginQuartileHead.size();
                 ++iColumnNum)
            {
                const vector<int>* ptrArrQuar = nullptr;
                auto& wsNameHeader = arrOriginQuartileHead[iColumnNum].first;

                if (ptrGlobal->ptrConfig->mapArrOutParams.count(wsNameHeader))
                {
                    switch (iColumnNum)
                    {
                        case 0:
                            ptrArrQuar = &fGraph.arrGlobalQuarAllPairDistance;
                            break;
                        case 1:
                            ptrArrQuar = &fGraph.arrLocalQuarAllPairDistance;
                            break;
                    }

                    if (ptrArrQuar)
                    {
                        for (const auto& it : *ptrArrQuar)
                        {
                            FTableData fDataContainer;
                            fDataContainer.fData = it;

                            if (fDataContainer.fData.has_value())
                                fData.arrType[arrRow.size()] =
                                    fDataContainer.fData.type().name();
                            arrRow.push_back(fDataContainer);
                        }
                    }
                }
            }

            fTotalOutData.push_back(arrRow);
        }
    }
}

void FAdapOutData::CalcDataCorridor(vector<vector<FTableData>>& arrDataCorridor,
                                    const vector<vector<FTableData>>& arrData,
                                    const string&                     sType,
                                    const int&                        iCol)
{
    int H = arrData.size();

}

void FAdapOutData::CreateDataCorridor()
{
    for (auto& [eType, fData] : mapOutData)
    {
        int W = fData.arrHeader.size();
        fData.arrDataCorridor.assign(arrCorridorHeader.size(),
                                     vector<FTableData>(W));
        for (int iCol = 0; iCol < W; ++iCol)
        {
            if (fData.arrType[iCol] == "")
            {
                ptrGlobal->ptrError->ErrorBadDataCorridor(
                    fData.arrHeader[iCol]);
            }
            
            CalcDataCorridor(fData.arrDataCorridor, fData.arrData,
                                 fData.arrType[iCol], iCol);
            
        }
    }
}

void FAdapOutData::CreateData()
{
    CreateTotalData();

    CreateGraphData();

    CreateDataCorridor();
}

void FAdapOutData::Create()
{
    CreateHeader();

    CreateData();
}