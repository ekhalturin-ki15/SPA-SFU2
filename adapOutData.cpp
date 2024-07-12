#include "adapOutData.h"

#include "config.h"
#include "error.h"
#include "graph.h"
#include "metric.h"
#include "solve.h"

#include <typeinfo>

int FAdapOutData::iSinglControll = 0;

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
      arrOriginCorridorHeader({ L"Максимальные значения:",
                                L"Минимальные значения:",
                          L"Мода:", L"Медиана:", L"Среднее усечённое:" }),
      arrOriginCompTreeHeader(
              { L"Название УП",
                L"За какой курс",

                L"Заголовок компетенции", L"ЗЕ Заголовка компетенции",
                L"Кол-во дисциплин заголовка компетенции",

                L"Процент распределения Заголовка компетенции",

                L"Компетенция", L"Полное название компетенции",
                L"ЗЕ Компетенций", L"Кол-во дисциплин компетенции",
                L"Процент распределения Компетенции",

                L"Индикатор", L"Полное название индикатора", L"ЗЕ индикаторов",
                L"Кол-во дисциплин индикатора",
                L"Процент распределения Индикатора" })
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

void FAdapOutData::CreateCompTreeHeader()
{
    for (auto& [sName, fCompTreeData] : mapCompTreeData)
    {
        for (const auto& wsNameHeader : arrOriginCompTreeHeader)
        {
            if (ptrGlobal->ptrConfig->mapArrOutParams.count(
                    wsNameHeader))    // Если правильно указано название в
                                      // параметрах
            {
                fCompTreeData.arrHeader.push_back(ptrGlobal->ConwertToString(
                    ptrGlobal->ptrConfig->mapArrOutParams[wsNameHeader]
                        .GetName()));
            }
        }
    }
}

void FAdapOutData::CreateRectCompTreeData(
    vector<vector<FTableData>>& arrReturnData,
    shared_ptr<FTreeMetric>
    ptrMetric)
{
    int iSizeX = 0, iSizeY = 0; 

    CreateTableRectInfo(true,    // Считаем вхолостую
                        arrReturnData, iSizeX,
                        iSizeY,  
                        ptrMetric, 0);


}

void FAdapOutData::CreateTableRectInfo(
    const bool& bIsCounting,    // Если true - то проход в холостую для
                                // определения iSizeX
    vector<vector<FTableData>>&
        arrReturnData,    // Возвращаемое значение с функции
    int& iSizeX,    // Если считаем вхолостую, когда bIsCounting == true, то
                    // записываем в iSizeX результат
    int& iCurrentY,    // Глобальная переменая
    shared_ptr<FTreeMetric> ptrMetric, int iDeep)
{

}

void FAdapOutData::CreateCompTreeData()
{
    for (int iCur = 0; iCur < ptrGlobal->ptrSolve->N; ++iCur)
    {
        const auto& it            = ptrGlobal->ptrSolve->GetCurricula(iCur);
        auto        ptrTreeMetric = it->ptrMetric->ptrTreeMetric;

        for (auto& [sName, fData] : mapCompTreeData)
        {
            const auto& ptrCurrentTree = ptrTreeMetric->mapChild[sName];

            CreateRectCompTreeData(fData.arrData, ptrCurrentTree); 
        }
    }
}

void FAdapOutData::CreateTotalHeader()
{
    auto& fTotalOutHeader = fTotalData.arrHeader;

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

    //Теперь в CreateType
    //fTotalData.arrType.resize(fTotalOutHeader.size());
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

        // Теперь в CreateType
        // fData.arrType.resize(fData.arrHeader.size());
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
    CreateTotalHeader();

    mapOutData[ETypeGraph::ETG_Common];
    mapOutData[ETypeGraph::ETG_Alt];
    mapOutData[ETypeGraph::ETG_Reverse];

    mapCompTreeData[FMetric::sAllMetric];
    for (int iCourse = 0; iCourse < ptrGlobal->ptrSolve->iMaxCourse; ++iCourse)
    {
        mapOutData[ETypeGraph(iCourse)];
        mapCompTreeData[to_string(iCourse)];
    }

    CreateGraphHeader();

    CreateCompTreeHeader();
}

void FAdapOutData::CreateTotalData()
{
    auto& fTotalOutData = fTotalData.arrData;

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

                arrRow.push_back(fCredit);
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

                            arrRow.push_back(fDataContainer);
                        }
                    }
                }
            }

            fTotalOutData.push_back(arrRow);
        }
    }
}

template<typename T>
vector<T> AdapterTemplateCalcDataCorridorNoDiv(
    vector<vector<FTableData>>&       arrDataCorridor,
    const vector<vector<FTableData>>& arrData,
    const int&                        iCol)
{
    int       H = arrData.size();
    vector<T> arrLocalData;
    for (int y = 0; y < H; ++y)
    {
        if (arrData[y][iCol].fData.has_value())
            arrLocalData.push_back(std::any_cast<T>(arrData[y][iCol].fData));
    }
    if (arrLocalData.size() < 2)
        return arrLocalData;

    sort(ALL(arrLocalData));

    /*
     arrCorridorHeader({ L"Максимальные значения:", L"Минимальные значения:",
                         L"Мода:", L"Медиана:", L"Среднее усечённое:" })
     */

    auto& fMax  = arrDataCorridor[0][iCol].fData;
    auto& fMin  = arrDataCorridor[1][iCol].fData;
    auto& fMode = arrDataCorridor[2][iCol].fData;
    // Требуют деление, поэтому не будут подсчитаны, как положены
    auto& fMean = arrDataCorridor[3][iCol].fData;
    // auto& fAvg  = arrDataCorridor[4][iCol].fData;

    fMax = arrLocalData.back();
    fMin = arrLocalData.front();

    // Подсчёт моды
    {
        map<T, int> mapCounting;
        T           fAppMode;
        bool        bIsUniq   = true;
        int         iMaxCount = 0;
        for (const auto& it : arrLocalData)
        {
            auto& fCount = mapCounting[it];
            ++fCount;

            if (iMaxCount < fCount)
            {
                bIsUniq   = true;
                iMaxCount = fCount;
                fAppMode  = it;
            }
            else 
                if (iMaxCount == fCount)
            {
                bIsUniq = false;
            }
        }

        if (bIsUniq)
            fMode = fAppMode;
    }


    // Подсчёт медианы (Но без деления)
    fMean = arrLocalData[arrLocalData.size() / 2];

    return arrLocalData;
}

template<typename T>
bool TemplateCalcDataCorridorNoDiv(vector<vector<FTableData>>& arrDataCorridor,
                                   const vector<vector<FTableData>>& arrData,
                                   const int&                        iCol)
{
    auto arrLocalData =
        AdapterTemplateCalcDataCorridorNoDiv<T>(arrDataCorridor, arrData, iCol);

    if (arrLocalData.size() < 2)
        return false;

    return true;
}

template<typename T>
bool TemplateCalcDataCorridorWithDiv(
    vector<vector<FTableData>>&       arrDataCorridor,
    const vector<vector<FTableData>>& arrData,
    const int&                        iCol,
    const double&                     dTruncAvg)
{

    auto& fMode = arrDataCorridor[2][iCol].fData;
    auto& fMean = arrDataCorridor[3][iCol].fData;
    auto& fAvg  = arrDataCorridor[4][iCol].fData;

    auto arrLocalData =
        AdapterTemplateCalcDataCorridorNoDiv<T>(arrDataCorridor, arrData, iCol);

    int N = arrLocalData.size();

    if (N < 2)
        return false;

    if (!fMode.has_value())
    {
        map<T, int> mapCounting;
        T           fMaxCount = 1;
        for (const auto& it : arrLocalData)
        {
            auto& fCount = mapCounting[it];
            ++fCount;
            if (fCount > fMaxCount)
                fMaxCount = fCount;
        }

        int iCountMax = 0;
        T   fSum      = 0;
        for (const auto& [key, val] : mapCounting)
        {
            if (val == fMaxCount)
            {
                ++iCountMax;
                fSum += key;
            }  
        }
        fMode = fSum / iCountMax;
    }


    if (!(N & 1))
    {
        fMean = (arrLocalData[N / 2] + arrLocalData[(N / 2) - 1]) / 2.0;
    }

    // Подсчёт Усечённого среднего
    T dSum = 0;

    int iAmountCut = N * dTruncAvg;

    int iSizeSampling = N - 2 * iAmountCut;

    if (iSizeSampling > 0)
    {
        for (int i = iAmountCut; i < N - iAmountCut; ++i)
        {
            dSum += arrLocalData[i];
        }
        fAvg = dSum / iSizeSampling;    // Усечённое среднее
    }

    return true;
}

bool FAdapOutData::CalcDataCorridor(vector<vector<FTableData>>& arrDataCorridor,
                                    const vector<vector<FTableData>>& arrData,
                                    const int&                        iCol,
                                    const string&                     sType)

{
    bool bIsCheck = false;

    if (sType == "")
        return true;    // Данный столбец не будет выводится

    if (sType == typeid(string).name())
    {
        bIsCheck = TemplateCalcDataCorridorNoDiv<string>(arrDataCorridor,
                                                         arrData, iCol);
    }
    if (sType == typeid(int).name())
    {
        bIsCheck = TemplateCalcDataCorridorWithDiv<int>(
            arrDataCorridor, arrData, iCol,
            ptrGlobal->ptrConfig->GetDTruncAvg());
    }
    if (sType == typeid(double).name())
    {
        bIsCheck = TemplateCalcDataCorridorWithDiv<double>(
            arrDataCorridor, arrData, iCol,
            ptrGlobal->ptrConfig->GetDTruncAvg());
    }
    if (sType == typeid(char).name())
    {
        bIsCheck =
            TemplateCalcDataCorridorNoDiv<char>(arrDataCorridor, arrData, iCol);
    }

    if (!bIsCheck)
        return false;

    return true;
}


void FAdapOutData::CreateType(FDataType& fData)
{
    int W = fData.arrHeader.size();
    fData.arrType.resize(W);

    for (int x = 0; x < W; ++x)
    {
        for (int y = 0; y < fData.arrData.size(); ++y)
        {
            if (fData.arrData[y][x].fData.has_value())
            {
                fData.arrType[x] = fData.arrData[y][x].fData.type().name();
                break;
            }
        }
    }
}

void FAdapOutData::CreateDataCorridorAndType()
{
    vector<FDataType*> arrAllData = { &fTotalData };
    for (auto& [eType, fData] : mapOutData)
    {
        arrAllData.push_back(&fData);
    }

    for (auto& ptrData : arrAllData)
    {
        auto& fData = *ptrData;

        CreateType(fData);

        int W = fData.arrHeader.size();
        fData.arrDataCorridor.assign(arrOriginCorridorHeader.size(),
                                     vector<FTableData>(W));
        for (int iCol = 0; iCol < W; ++iCol)
        {
            bool bIsCorrect =
                CalcDataCorridor(fData.arrDataCorridor, fData.arrData, iCol,
                                 fData.arrType[iCol]);

            if (!bIsCorrect)
            {
                ptrGlobal->ptrError->ErrorBadDataCorridor(
                    fData.arrHeader[iCol]);
            }
        }
    }

}

void FAdapOutData::CreateData()
{
    CreateTotalData();

    CreateGraphData();
    
    CreateCompTreeData();

    //В самом конце, так как считаем меры центральной тенденции для всего ранее посчитанного
    CreateDataCorridorAndType();

}

void FAdapOutData::Create()
{
    CreateHeader();

    CreateData();
}