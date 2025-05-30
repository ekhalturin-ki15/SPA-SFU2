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
      arrOriginMetricGraphHead({ L"Название учебного плана",
                                 L"Всего ЗЕ в графе",
                                 L"ЗЕ основных дисциплин",
                                 L"ЗЕ дисциплин по выбору",
                                 L"ЗЕ факультативов",

                                 L"Кол-во дисциплин в графе",
                                 L"Количество основных дисциплин",
                                 L"Количество дисциплин по выбору",
                                 L"Количество факультативов",

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
                                 L"Коэффициент кластеризации (Глобальный)" }),
      arrOriginQuartileHead(
          { L"Количество рёбер указанного квартиля для всей выборки",
            L"Локальное количество рёбер указанного квартиля" }),
      arrOriginCorridorHeader({ L"Максимальные значения:",
                                L"Минимальные значения:", L"Медиана:", L"Мода:",
                                L"Среднее усечённое:" }),
      arrOriginCompTreeHeader(
          { L"Название УП", L"За какой курс",

            L"Группа компетенции", L"ЗЕ группы компетенции",
            L"Кол-во дисциплин группы компетенции",

            L"Процент распределения группы компетенции",

            L"Компетенция", L"Полное название компетенции", L"ЗЕ Компетенций",
            L"Кол-во дисциплин компетенции",
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
    for (auto& it : arrMapCompTreeData)
    {
        for (auto& [sName, fCompTreeData] : it)
        {
            for (const auto& wsNameHeader : arrOriginCompTreeHeader)
            {
                if (ptrGlobal->ptrConfig->mapAddOutParams.count(
                        wsNameHeader))    // Если правильно указано название в
                                          // параметрах
                {
                    fCompTreeData.arrHeader.push_back(
                        ptrGlobal->ConwertToString(
                            ptrGlobal->ptrConfig->mapAddOutParams[wsNameHeader]
                                .GetName()));

                    fCompTreeData.arrIsOut.push_back(
                        ptrGlobal->ptrConfig->mapAddOutParams[wsNameHeader]
                            .GetTotal());
                }
            }
        }
    }
}

void FAdapOutData::CreateGephiCSVHeader()
{
    for (auto& mapGephiCSVData : arrMapGephiCSVData)
    {
        for (auto& [eType, fPairData] : mapGephiCSVData)
        {
            auto& [fLableData, fRibData] = fPairData;
            for (const auto& sHead :
                 ptrGlobal->ptrConfig->GetArrNameLabelHeader())
            {
                fLableData.arrHeader.push_back(sHead);
                fLableData.arrIsOut.push_back(true);    // В Gephi выводим всё
            }

            for (const auto& sHead :
                 ptrGlobal->ptrConfig->GetArrNameRibHeader())
            {
                fRibData.arrHeader.push_back(sHead);
                fRibData.arrIsOut.push_back(true);    // В Gephi выводим всё
            }
        }
    }
}

// Возвращает актуальную ширину дерева компетенций
int FAdapOutData::CreateCompTreeData(const int&           W,
                                     vector<vector<any>>& arrReturnData,
                                     shared_ptr<FTreeMetric>
                                         ptrMetric)
{
    vector<any> arrRow(W);
    CreateRectCompTreeData(arrReturnData, arrRow, ptrMetric, 0, 0);
    // Для усечения с конца таблицы в случаи, если индикаторы (в дальнейшем
    // дескрипторы) не были обнаружены Пробегаемся по всем строчкам записей
    // дерева, и если везде нет индикаторов, то прозводим учечение

    int iCheckRow = FTypeGraph::dNoInit;
    int iRowSize  = 0;
    if (arrReturnData.size())
        iRowSize = arrReturnData.at(0).size();
    for (auto& it : arrReturnData)
    {
        int* iBufSize = &iRowSize;
        if (iCheckRow == FTypeGraph::dNoInit)
        {
            iBufSize = &iCheckRow;
        }
        (*iBufSize) = it.size();
        for (int iX = it.size() - 1; iX >= 0; --iX)
        {
            any empty;
            if (it[iX].type() == empty.type())
                --(*iBufSize);
        }
        if ((*iBufSize) != iCheckRow)
        {
            iCheckRow = FTypeGraph::dNoInit;
            break;
        }
    }

    if (iCheckRow != FTypeGraph::dNoInit)
    {
        for (auto& it : arrReturnData)
        {
            it.resize(iCheckRow);
        }
        return iCheckRow;
    }

    return W;
}

void FAdapOutData::CreateRectCompTreeData(vector<vector<any>>&    arrReturnData,
                                          vector<any>&            arrRow,
                                          shared_ptr<FTreeMetric> ptrMetric,
                                          int x, int iDeep)
{
    if (x == 0)
    {
        // Вписываем название УП
        any fTableData;
        fTableData = ptrGlobal->ptrSolve->sInPath;
        arrRow[x]  = (fTableData);
        CreateRectCompTreeData(arrReturnData, arrRow, ptrMetric, x + 1, iDeep);
        return;
    }
    if (ptrMetric->sName == ptrGlobal->ptrConfig->GetSNoInitData())
    {
        arrReturnData.push_back(arrRow);
        return;
    }
    {
        any fTableData;
        fTableData = ptrMetric->sName;
        arrRow[x]  = (fTableData);
    }

    // Выводим полное название компетенции
    if (iDeep > 1)
    {
        ++x;
        {
            vector<string> arrName;
            auto           ptrBuf = ptrMetric;
            for (int i = 0; i < iDeep; ++i)
            {
                arrName.push_back(ptrBuf->sName);
                ptrBuf = ptrBuf->ptrParent;
            }
            string sResult = "";
            for (int i = iDeep - 1; i >= 0; --i)
            {
                sResult += arrName[i];
                if (i)
                    sResult += ptrGlobal->ptrConfig->GetSPrefFullNameCourse();
            }
            any fTableData;
            fTableData = sResult;
            arrRow[x]  = (fTableData);
        }
    }

    // Выводим кол-во ЗЕ
    if (iDeep > 0)
    {
        ++x;
        {
            any fTableData;
            fTableData = ptrMetric->dChosenSum;
            arrRow[x]  = (fTableData);
            // ptrGlobal->DoubletWithPrecision(ptrMetric->dChosenSum);
        }
    }

    // Выводим кол-во дисциплин
    if (iDeep > 0)
    {
        ++x;
        {
            any fTableData;
            fTableData = ptrMetric->iAmountUsingDisc;
            arrRow[x]  = (fTableData);
            // ptrGlobal->DoubletWithPrecision(ptrMetric->iAmountUsingDisc);
        }
    }

    // Выводим процент распределения
    if (iDeep > 0)
    {
        ++x;
        {
            any fTableData;
            fTableData = ptrMetric->dInclusionPercent;
            arrRow[x]  = (fTableData);
            // ptrGlobal->DoubletWithPrecision(ptrMetric->dInclusionPercent);
        }
    }
    if (ptrMetric->mapChild.size() == 0)
    {
        arrReturnData.push_back(arrRow);
        return;
    }

    for (auto& [sName, ptrChild] : ptrMetric->mapChild)
    {
        CreateRectCompTreeData(arrReturnData, arrRow, ptrChild, x + 1,
                               iDeep + 1);
    }
}

void FAdapOutData::CreateGephiCSVData()
{
    CreateGephiLableCSVData();
    CreateGephiRibCSVData();
}

void FAdapOutData::CreateGephiRibCSVData()
{
    for (int iCur = 0; iCur < ptrGlobal->ptrSolve->N; ++iCur)
    {
        for (auto& [eType, fPairData] : arrMapGephiCSVData[iCur])
        {
            auto& [fLableData, fRibData] = fPairData;

            const auto& it = ptrGlobal->ptrSolve->GetCurricula(iCur);

            auto& fTotalOutData   = fRibData.arrData;
            auto& fTotalOutHeader = fRibData.arrHeader;

            if (it->ptrGraph->mapGraph.count(eType))
            {
                const auto& fGraph = it->ptrGraph->mapGraph[eType];

                for (int l = 0; l < fGraph.fAdjList.size(); ++l)
                {
                    for (const auto& [r, dLen] : fGraph.fAdjList[l])
                    {
                        vector<any> arrRow(fTotalOutHeader.size());
                        // Чтобы не дублировать, он же неориентированный
                        if ((l < r) ||
                            (!ptrGlobal->ptrConfig->GetBIsUnDirected()))
                        {
                            for (int iColumnNum = 0;
                                 iColumnNum < fTotalOutHeader.size();
                                 ++iColumnNum)
                            {
                                any fDataContainer;

                                switch (iColumnNum)
                                {
                                    case 0:
                                        fDataContainer = l;
                                        break;
                                    case 1:
                                        fDataContainer = r;
                                        break;
                                    case 2:
                                        fDataContainer = ptrGlobal->ptrConfig
                                                             ->GetSNameRibDir();
                                        break;
                                    case 3:
                                    case 4:
                                        fDataContainer = dLen;
                                        break;
                                }
                                arrRow[iColumnNum] = (fDataContainer);
                            }

                            fTotalOutData.push_back(arrRow);
                        }
                    }
                }
            }
        }
    }
}

void FAdapOutData::CreateGephiLableCSVData()
{
    for (int iCur = 0; iCur < ptrGlobal->ptrSolve->N; ++iCur)
    {
        for (auto& [eType, fPairData] : arrMapGephiCSVData[iCur])
        {
            auto& [fLableData, fRibData] = fPairData;
            const auto& it = ptrGlobal->ptrSolve->GetCurricula(iCur);

            auto& fTotalOutData   = fLableData.arrData;
            auto& fTotalOutHeader = fLableData.arrHeader;

            if (it->ptrGraph->mapGraph.count(eType))
            {
                const auto& fGraph = it->ptrGraph->mapGraph[eType];
                try
                {
                    for (int iRowNum = 0;
                         iRowNum <
                         fGraph.mapGraphDataTypeDisc.at(ETypeDisc::ETD_Total)
                             .iAmount;
                         ++iRowNum)
                    {
                        const auto& fNodeName = fGraph.arrRel[iRowNum];
                        vector<any> arrRow(fTotalOutHeader.size());

                        for (int iColumnNum = 0;
                             iColumnNum < fTotalOutHeader.size();
                             ++iColumnNum)
                        {
                            any                      fDataContainer;
                            shared_ptr<FTreeElement> ptrNode;

                            switch (iColumnNum)
                            {
                                case 0:
                                    fDataContainer = iRowNum;    // Номер строки
                                    break;
                                case 1:
                                    if (it->mapAllDisc.count(fNodeName.first))
                                        ptrNode =
                                            it->mapAllDisc[fNodeName.first];

                                    fDataContainer = CreateCommonNameLabel(
                                        eType, fNodeName, ptrNode);

                                    break;
                                case 2:
                                    fDataContainer =
                                        fGraph.arrNodeWeight[iRowNum];
                                    break;

                                case 3:
                                    fDataContainer = CreateTag(
                                        eType, fNodeName.first, it->mapAllDisc);
                                    break;

                                case 4:
                                    // Не забываем про нуль нумерацию курсов
                                    if (fNodeName.second < 0)
                                    {
                                        if (it->mapAllDisc.count(
                                                fNodeName.first))
                                        {
                                            if (it->mapAllDisc[fNodeName.first]
                                                    ->mapCourseScore.size())
                                            {
                                                fDataContainer =
                                                    it->mapAllDisc[fNodeName
                                                                       .first]
                                                        ->mapCourseScore
                                                        .begin()
                                                        ->first +
                                                    1;
                                            }
                                            break;
                                        }

                                        // Курс неопределён
                                        fDataContainer = ptrGlobal->ptrConfig
                                                             ->GetSNoInitData();
                                        break;
                                    }
                                    fDataContainer = fNodeName.second + 1;
                                    break;

                                case 5:
                                    if (it->mapAllDisc.count(fNodeName.first))
                                        fDataContainer = AddCompString(
                                            it->mapAllDisc[fNodeName.first]
                                                ->mapComp,
                                            false);
                                    break;
                                case 6:
                                    if (it->mapAllDisc.count(fNodeName.first))
                                        fDataContainer =
                                            int(it->mapAllDisc[fNodeName.first]
                                                ->mapComp.size());
                                    break;
                                case 7:
                                    if (it->mapAllDisc.count(fNodeName.first))
                                        fDataContainer =
                                            fGraph.arrNodeWeight[iRowNum] /
                                            it->mapAllDisc[fNodeName.first]
                                                ->mapComp.size();
                                    break;
                            }

                            arrRow[iColumnNum] = (fDataContainer);
                        }
                        fTotalOutData.push_back(arrRow);
                    }
                }
                catch (...)    // Возможно, данного курса нет в данном УП
                {
                }
            }
        }
    }
}

string
    FAdapOutData::CreateCommonNameLabel(const ETypeGraph&         eTypeGraph,
                                        const pair<wstring, int>& fInfo,
                                        const shared_ptr<FTreeElement>& ptrNode)
{
    if (eTypeGraph == ETypeGraph::ETG_Reverse)
    {
        return ptrGlobal->ConwertToString(fInfo.first);
    }

    if (ptrNode == nullptr)
    {
        return ptrGlobal->ConwertToString(fInfo.first);
    }

    string sReturn = ptrNode->sName;

    // Не забываем про нуль нумерацию курсов
    if (ptrGlobal->ptrConfig->GetBOutCourseNumWithName())
    {
        if (eTypeGraph == ETypeGraph::ETG_Alt)
        {
            sReturn += ptrGlobal->ptrConfig->GetSSeparator() +
                       to_string(fInfo.second + 1);
        }
    }

    // ptrGlobal->ReversUTF16RU(ptrGlobal->ConwertToString(wsNameRaw));
    // Выводим ещё и компетенции
    if (ptrGlobal->ptrConfig->GetBOutCompWithName())
    {
        sReturn += AddCompString(ptrNode->mapComp);
    }

    return sReturn;
}

string FAdapOutData::AddCompString(const map<string, vector<string>>& mapComp,
                                   bool                               bIsBrache)
{
    string sReturn;
    if (bIsBrache)
        sReturn += "(";

    int j = -1;
    for (const auto& [sNameComp, arrIndicator] : mapComp)
    {
        ++j;
        if (j)
            sReturn += ",";
        sReturn += sNameComp;
    }
    if (bIsBrache)
        sReturn += ")";

    return sReturn;
}

string FAdapOutData::CreateTag(
    const ETypeGraph& eGraphType, const wstring& wsName,
    const map<wstring, shared_ptr<FTreeElement>>& mapAllDisc)
{
    /*for (auto& [key, val] :
         fCurricula->ptrGraph->mapGraph[eGraphType].mapReversRel)
    {*/
    string sReturn = ptrGlobal->ptrConfig->GetSNoInitData();
    if (eGraphType != ETypeGraph::ETG_Reverse)
    {
        if (mapAllDisc.at(wsName)->setTagDisc.size() > 0)
            sReturn = to_string(*mapAllDisc.at(wsName)->setTagDisc.begin());
    }

    return sReturn;
}

void FAdapOutData::CreateCompTreeData()
{
    for (int iCur = 0; iCur < ptrGlobal->ptrSolve->N; ++iCur)
    {
        const auto& it            = ptrGlobal->ptrSolve->GetCurricula(iCur);
        auto        ptrTreeMetric = it->ptrMetric->ptrTreeMetric;

        auto& mapCompTreeData = arrMapCompTreeData[iCur];
        for (auto& [sName, fData] : mapCompTreeData)
        {
            if (ptrTreeMetric->mapChild.count(sName))
            {
                const auto& ptrCurrentTree = ptrTreeMetric->mapChild[sName];

                int iSize = CreateCompTreeData(fData.arrHeader.size(),
                                               fData.arrData, ptrCurrentTree);
                fData.arrHeader.resize(iSize);
            }
        }
    }
}

void FAdapOutData::CreateTotalHeader()
{
    auto& fTotalOutHeader = fTotalData.arrHeader;
    auto& fIsOutHeader    = fTotalData.arrIsOut;

    for (const auto& wsNameHeader : arrOriginMetricTotalHead)
    {
        if (ptrGlobal->ptrConfig->mapArrOutParams.count(
                wsNameHeader))    // Если правильно указано название в
                                  // параметрах
        {
            fTotalOutHeader.push_back(ptrGlobal->ConwertToString(
                ptrGlobal->ptrConfig->mapArrOutParams[wsNameHeader].GetName()));

            fIsOutHeader.push_back(
                ptrGlobal->ptrConfig->mapArrOutParams[wsNameHeader].GetTotal());
        }
    }

    wstring wsName = L"ЗЕ у компетенции ";
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

            fIsOutHeader.push_back(
                ptrGlobal->ptrConfig->mapArrOutParams[wsName].GetTotal());
            fIsOutHeader.push_back(
                ptrGlobal->ptrConfig->mapArrOutParams[wsName].GetTotal());
        }
    }

    // Теперь в CreateType
    // fTotalData.arrType.resize(fTotalOutHeader.size());
}

void FAdapOutData::CreateGraphHeader()
{
    for (auto& [eType, fData] : mapGraphData)
    {
        for (const auto& wsNameHeader : arrOriginMetricGraphHead)
        {
            if (ptrGlobal->ptrConfig->mapArrOutParams.count(wsNameHeader))
            {
                fData.arrHeader.push_back(ptrGlobal->ConwertToString(
                    ptrGlobal->ptrConfig->mapArrOutParams[wsNameHeader]
                        .GetName()));

                fData.arrIsOut.push_back(
                    ptrGlobal->ptrConfig->mapArrOutParams[wsNameHeader]
                        .GetTotal());
            }
        }

        CompHeaderCreate(fData);

        QuartileHeaderCreate(fData, arrOriginQuartileHead);

        // Теперь в CreateType
        // fData.arrType.resize(fData.arrHeader.size());
    }
}

void FAdapOutData::QuartileHeaderCreate(FDataType&             fData,
                                        const vector<wstring>& arrQuartileHead)
{
    vector<string>& arrHeader = fData.arrHeader;
    vector<int>&   arrIsOut  = fData.arrIsOut;

    for (int i = 1; i <= arrQuartileHead.size(); ++i)
    {
        const auto& wsPref = arrQuartileHead[i - 1];

        for (int iAmountQuartile = 1;
             iAmountQuartile <= ptrGlobal->ptrConfig->GetIAmountQuar() +
                                    (i == arrQuartileHead.size());
             ++iAmountQuartile)
        {
            // Последний квартиль отвечает за вывод отсутствующих путей
            if (iAmountQuartile > ptrGlobal->ptrConfig->GetIAmountQuar())
            {
                wstring wsNoPath = L"Нет пути между вершинами";
                arrHeader.push_back(ptrGlobal->ConwertToString(
                    ptrGlobal->ptrConfig->mapArrOutParams[wsNoPath].GetName()));
                //"No" + ptrGlobal->ptrConfig->GetSSeparator() + "Path");

                arrIsOut.push_back(
                    ptrGlobal->ptrConfig->mapArrOutParams[wsPref].GetTotal());

                continue;
            }

            string sAddedHead = ptrGlobal->ConwertToString(
                ptrGlobal->ptrConfig->mapArrOutParams[wsPref].GetName());

            sAddedHead += ptrGlobal->ptrConfig->GetSSeparator();

            sAddedHead += to_string(iAmountQuartile) +
                          ptrGlobal->ptrConfig->GetSSeparator();

            sAddedHead += ptrGlobal->ptrConfig->GetSSufQuar();

            arrHeader.push_back(sAddedHead);
            arrIsOut.push_back(
                ptrGlobal->ptrConfig->mapArrOutParams[wsPref].GetTotal());
        }
    }
}

void FAdapOutData::CompHeaderCreate(FDataType& fData)
{
    vector<string>& arrHeader = fData.arrHeader;
    vector<int>&   arrIsOut  = fData.arrIsOut;

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

            if (iAmountComp == ptrGlobal->ptrConfig->GetISoMachComp())
                sAddedHead += ">=";

            sAddedHead +=
                to_string(iAmountComp) + ptrGlobal->ptrConfig->GetSSeparator();
            sAddedHead += ptrGlobal->ptrConfig->GetSSufComp();

            arrHeader.push_back(sAddedHead);
            arrIsOut.push_back(
                ptrGlobal->ptrConfig->mapArrOutParams[wsNameSoMachComp]
                    .GetTotal());
        }
    }
}

void FAdapOutData::CreateHeader()
{
    CreateTotalHeader();

    vector<ETypeGraph> arrType;
    arrType = { ETypeGraph::ETG_Common, ETypeGraph::ETG_Alt,
                ETypeGraph::ETG_Reverse };
    for (int iCourse = 0; iCourse < ptrGlobal->ptrSolve->iMaxCourse; ++iCourse)
    {
        arrType.push_back(ETypeGraph(iCourse));
    }

    for (const auto& eType : arrType)
    {
        mapGraphData[eType];
    }

    // Для каждого УП в отдельности
    arrMapCompTreeData.resize(ptrGlobal->ptrSolve->N);
    arrMapGephiCSVData.resize(ptrGlobal->ptrSolve->N);
    for (auto& it : arrMapCompTreeData)
    {
        it[ptrGlobal->ptrConfig->GetSAllCourses()];
        for (int iCourse = 1; iCourse <= ptrGlobal->ptrSolve->iMaxCourse;
             ++iCourse)
        {
            it[to_string(iCourse)];
        }
    }
    for (const auto& eType : arrType)
    {
        for (auto& it : arrMapGephiCSVData)
        {
            it[eType];
        }
    }

    CreateGraphHeader();

    CreateCompTreeHeader();

    CreateGephiCSVHeader();
}

void FAdapOutData::CreateTotalData()
{
    auto& fTotalOutData = fTotalData.arrData;

    for (int iCur = 0; iCur < ptrGlobal->ptrSolve->N; ++iCur)
    {
        const auto& it = ptrGlobal->ptrSolve->GetCurricula(iCur);

        vector<any> arrRow(
            fTotalData.arrHeader
                .size());    // Без push_back, чтобы гарантировать
                             // совпадение кол-во столбцов
        for (int iColumnNum = 0; iColumnNum < arrOriginMetricTotalHead.size();
             ++iColumnNum)
        {
            auto& wsNameHeader = arrOriginMetricTotalHead[iColumnNum];
            if (ptrGlobal->ptrConfig->mapArrOutParams.count(wsNameHeader))
            {
                any fDataContainer;
                switch (iColumnNum)
                {
                    case 0:
                        fDataContainer = it->sCurName;
                        break;
                    case 1:
                        fDataContainer = it->sTypePlan;
                        break;
                    case 2:
                        fDataContainer = it->iYearStart;
                        break;
                    case 3:
                        fDataContainer = it->iCodeUGSN;
                        break;

#define START_TYPE_DISC_CREDITS 4
                    case START_TYPE_DISC_CREDITS:
                    case START_TYPE_DISC_CREDITS + 1:
                    case START_TYPE_DISC_CREDITS + 2:
                    case START_TYPE_DISC_CREDITS + 3:
                        if (it->mapETMTypeDisc[ETM_NoExtended].count(ETypeDisc(
                                iColumnNum - START_TYPE_DISC_CREDITS)))
                        {
                            fDataContainer =
                                it->mapETMTypeDisc[ETM_NoExtended]
                                    .at(ETypeDisc(iColumnNum -
                                                  START_TYPE_DISC_CREDITS))
                                    .dCredits;
                        }
                        else
                        {
                            // fDataContainer.fData = FTypeGraph::dNoInit;
                        }
                        break;

#define START_TYPE_DISC_AMOUNT 8
                    case START_TYPE_DISC_AMOUNT:
                    case START_TYPE_DISC_AMOUNT + 1:
                    case START_TYPE_DISC_AMOUNT + 2:
                    case START_TYPE_DISC_AMOUNT + 3:
                        if (it->mapETMTypeDisc[ETM_NoExtended].count(
                                ETypeDisc(iColumnNum - START_TYPE_DISC_AMOUNT)))
                        {
                            fDataContainer =
                                it->mapETMTypeDisc[ETM_NoExtended]
                                    .at(ETypeDisc(iColumnNum -
                                                  START_TYPE_DISC_AMOUNT))
                                    .iAmount;
                        }
                        else
                        {
                            // fDataContainer.fData = FTypeGraph::dNoInit;
                        }
                        break;
                }

                arrRow[iColumnNum] = (fDataContainer);
            }
        }

        int iCurrentX = arrOriginMetricTotalHead.size();

        wstring wsName = L"ЗЕ у компетенции ";
        if (ptrGlobal->ptrConfig->mapArrOutParams.count(wsName))
        {
            for (auto& sHeaderComp : ptrGlobal->ptrSolve->setHeaderComp)
            {
                any  fCredit;
                bool bIsFCreditInit = false;
                any  fPercent;
                bool bIsFPercentInit = false;

                if (it->ptrMetric)
                {
                    auto ptrTreeMetric =
                        it->ptrMetric->ptrTreeMetric
                            ->mapChild[ptrGlobal->ptrConfig->GetSAllCourses()];
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
                        fCredit        = dScore;
                        bIsFCreditInit = true;
                    }
                    else
                    {
                        // fCredit.fData = FTypeGraph::dNoInit;
                    }

                    if (dRes > ptrGlobal->ptrConfig->GetDMinComp())
                    {
                        fPercent        = dRes * 100;
                        bIsFPercentInit = true;
                    }
                    else
                    {
                        // fPercent.fData = FTypeGraph::dNoInit;
                    }
                }

                if (bIsFCreditInit)
                    arrRow[iCurrentX] = (fCredit);
                ++iCurrentX;
                if (bIsFPercentInit)
                    arrRow[iCurrentX] = (fPercent);
                ++iCurrentX;
            }

            fTotalOutData.push_back(arrRow);
        }
    }
}

void FAdapOutData::CreateGraphData()
{
    for (auto& [eType, fData] : mapGraphData)
    {
        auto& fTotalOutData = mapGraphData[eType].arrData;

        for (int iCur = 0; iCur < ptrGlobal->ptrSolve->N; ++iCur)
        {
            const auto& it = ptrGlobal->ptrSolve->GetCurricula(iCur);

            const auto& fGraph = it->ptrGraph->mapGraph[eType];

            vector<any> arrRow(mapGraphData[eType].arrHeader.size());

            // Возможно, не все параметры присутсвуют в config, поэтому не
            // итерируемся по mapOutData[eType].arrHeader
            for (int iColumnNum = 0;
                 iColumnNum < arrOriginMetricGraphHead.size();
                 ++iColumnNum)
            {
                any   fDataContainer;
                auto& wsNameHeader = arrOriginMetricGraphHead[iColumnNum];
                if (ptrGlobal->ptrConfig->mapArrOutParams.count(wsNameHeader))
                {
                    switch (iColumnNum)
                    {
                        case 0:
                            fDataContainer = it->sCurName;
                            break;

#define START_GRAPH_DATA_CREDITS 1
                        case START_GRAPH_DATA_CREDITS:
                        case START_GRAPH_DATA_CREDITS + 1:
                        case START_GRAPH_DATA_CREDITS + 2:
                        case START_GRAPH_DATA_CREDITS + 3:
                            if (fGraph.mapGraphDataTypeDisc.count(ETypeDisc(
                                    iColumnNum - START_GRAPH_DATA_CREDITS)))
                            {
                                fDataContainer =
                                    fGraph.mapGraphDataTypeDisc
                                        .at(ETypeDisc(iColumnNum -
                                                      START_GRAPH_DATA_CREDITS))
                                        .dCredits;
                            }
                            else
                            {
                                // fDataContainer.fData =
                                // FTypeGraph::dNoInit;
                            }
                            break;

#define START_GRAPH_DATA_AMOUNT 5
                        case START_GRAPH_DATA_AMOUNT:
                        case START_GRAPH_DATA_AMOUNT + 1:
                        case START_GRAPH_DATA_AMOUNT + 2:
                        case START_GRAPH_DATA_AMOUNT + 3:
                            if (fGraph.mapGraphDataTypeDisc.count(ETypeDisc(
                                    iColumnNum - START_GRAPH_DATA_AMOUNT)))
                            {
                                fDataContainer =
                                    fGraph.mapGraphDataTypeDisc
                                        .at(ETypeDisc(iColumnNum -
                                                      START_GRAPH_DATA_AMOUNT))
                                        .iAmount;
                            }
                            else
                            {
                                // fDataContainer.fData =
                                // FTypeGraph::dNoInit;
                            }
                            break;
                        case 9:
                            fDataContainer = fGraph.dMaxDiscScore;
                            break;
                        case 10:
                            fDataContainer = fGraph.dMinDiscScore;
                            break;
                        case 11:
                            fDataContainer = fGraph.dMaxRib;
                            break;
                        case 12:
                            fDataContainer = fGraph.dMinRib;
                            break;
                        case 13:
                            fDataContainer = fGraph.dDiametrLen;
                            break;
                        case 14:
                            fDataContainer = fGraph.dDiametrStep;
                            break;
                        case 15:
                            fDataContainer = fGraph.iComponent;
                            break;
                        case 16:
                            fDataContainer = fGraph.dMaxSpanTree;
                            break;
                        case 17:
                            fDataContainer = fGraph.dMinSpanTree;
                            break;
                        case 18:
                            fDataContainer = fGraph.dDense;
                            break;
                        case 19:
                            fDataContainer = fGraph.dGlobalСluster;
                            break;
                    }

                    arrRow[iColumnNum] = (fDataContainer);
                }
            }

            int iCurrentX = arrOriginMetricGraphHead.size();

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
                    any fDataContainer;
                    try
                    {
                        // Если УП обработан некорректно
                        fDataContainer =
                            fGraph.arrAmountCountCompDisc.at(iAmountComp);

                        arrRow[iCurrentX] = (fDataContainer);
                    }
                    catch (...)
                    {
                    }

                    ++iCurrentX;
                }
            }

            for (int iColumnNum = 0; iColumnNum < arrOriginQuartileHead.size();
                 ++iColumnNum)
            {
                const vector<int>* ptrArrQuar = nullptr;
                auto& wsNameHeader = arrOriginQuartileHead[iColumnNum];

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
                            any fDataContainer;
                            fDataContainer = it;

                            arrRow[iCurrentX++] = (fDataContainer);
                        }
                    }
                }
            }

            fTotalOutData.push_back(arrRow);
        }
    }
}

template<typename T>
vector<pair<T, int>> AdapterTemplateCalcDataCorridorNoDiv(
    vector<vector<FTableData>>& arrDataCorridor,
    const vector<vector<any>>&  arrData,
    const int&                  iCol)
{
    int                  H = arrData.size();
    vector<pair<T, int>> arrLocalData;
    for (int y = 0; y < H; ++y)
    {
        if (arrData[y][iCol].has_value())
        {
            try
            {
                T fData = std::any_cast<T>(arrData[y][iCol]);
                arrLocalData.push_back({ fData, y });
            }
            catch (const std::bad_any_cast& fError)
            {
                continue;
            }
        }
    }
    if (arrLocalData.size() < 2)
        return arrLocalData;

    sort(ALL(arrLocalData));

    auto& fMax = arrDataCorridor[0][iCol];
    auto& fMin = arrDataCorridor[1][iCol];

    auto& fMean = arrDataCorridor[2][iCol];
    auto& fMode = arrDataCorridor[3][iCol];

    // auto& fAvg  = arrDataCorridor[4][iCol];

    if (arrLocalData.back().first != arrLocalData.front().first)
    {
        fMax.fData = arrLocalData.back().first;
        fMin.fData = arrLocalData.front().first;
        // Подсчёт медианы (Но без деления)
        fMean.fData = arrLocalData[arrLocalData.size() / 2].first;

        fMax.iAddInfo  = arrLocalData.back().second;
        fMin.iAddInfo  = arrLocalData.front().second;
        fMean.iAddInfo = arrLocalData[arrLocalData.size() / 2].second;
    }

    // Подсчёт моды
    {
        map<T, int> mapCounting;
        T           fAppMode;
        int         iMaxCount = 0;
        bool        bIsUniq   = false;

        {
            int i = -1;
            for (const auto& [it, inf] : arrLocalData)
            {
                ++i;
                auto& fCount = mapCounting[it];
                ++fCount;

                if (iMaxCount < fCount)
                {
                    bIsUniq   = true;
                    iMaxCount = fCount;
                    fAppMode  = it;
                }

                if (iMaxCount == fCount)
                {
                    bIsUniq = false;
                }
            }
        }

        if ((iMaxCount == 1) && (bIsUniq))
        {
            fMode.fData = fAppMode;
        }
    }

    return arrLocalData;
}

template<typename T>
bool TemplateCalcDataCorridorNoDiv(vector<vector<FTableData>>& arrDataCorridor,
                                   const vector<vector<any>>&  arrData,
                                   const int&                  iCol)
{
    auto arrLocalData =
        AdapterTemplateCalcDataCorridorNoDiv<T>(arrDataCorridor, arrData, iCol);

    if (arrLocalData.size() < 2)
        return false;

    return true;
}

template<typename T>
bool TemplateCalcDataCorridorWithDiv(
    vector<vector<FTableData>>& arrDataCorridor,
    const vector<vector<any>>&  arrData,
    const int&                  iCol,
    const double&               dTruncAvg)
{
    auto& fMode = arrDataCorridor[3][iCol];
    auto& fAvg  = arrDataCorridor[4][iCol];

    auto arrLocalData =
        AdapterTemplateCalcDataCorridorNoDiv<T>(arrDataCorridor, arrData, iCol);

    int N = arrLocalData.size();

    if (N < 2)
        return false;

    if (!fMode.fData.has_value())
    {
        map<T, int> mapCounting;
        T           fMaxCount = 1;
        for (const auto& [it, iInfo] : arrLocalData)
        {
            auto& fCount = mapCounting[it];
            ++fCount;
            if (fCount > fMaxCount)
                fMaxCount = fCount;
        }

        if (fMaxCount != 1)
        {
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
            fMode.fData = fSum / iCountMax;
        }
    }

    // Не буду делить попалам
    /*if (!(N & 1))
    {
        fMean = (arrLocalData[N / 2] + arrLocalData[(N / 2) - 1]) / 2.0;
    }*/

    // Подсчёт Усечённого среднего
    T dSum = 0;

    int iAmountCut = N * dTruncAvg;

    int iSizeSampling = N - 2 * iAmountCut;

    if (iSizeSampling > 0)
    {
        for (int i = iAmountCut; i < N - iAmountCut; ++i)
        {
            dSum += arrLocalData[i].first;
        }
        fAvg.fData = dSum / iSizeSampling;    // Усечённое среднее
    }

    return true;
}

bool FAdapOutData::CalcDataCorridor(vector<vector<FTableData>>& arrDataCorridor,
                                    const vector<vector<any>>&  arrData,
                                    const int&                  iCol,
                                    const string&               sType)

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
            if (fData.arrData[y][x].has_value())
            {
                fData.arrType[x] = fData.arrData[y][x].type().name();
                break;
            }
        }
    }
}

void FAdapOutData::CreateDataCorridorAndType()
{
    vector<FDataType*> arrAllData = { &fTotalData };
    // Для графовых
    for (auto& [eType, fData] : mapGraphData)
    {
        arrAllData.push_back(&fData);
    }
    // Для дерева компетенций
    for (auto& it : arrMapCompTreeData)
    {
        for (auto& [sType, fData] : it)
        {
            arrAllData.push_back(&fData);
        }
    }

    vector<FDataType*> arrOnlyType = arrAllData;
    // Для Gephi CSV файлов
    for (auto& it : arrMapGephiCSVData)
    {
        for (auto& [sType, fPairData] : it)
        {
            auto& [fLableData, fRibData] = fPairData;
            arrOnlyType.push_back(&fLableData);
            arrOnlyType.push_back(&fRibData);
        }
    }

    for (auto& ptrData : arrOnlyType)
    {
        auto& fData = *ptrData;

        CreateType(fData);
    }

    for (auto& ptrData : arrAllData)
    {
        auto& fData = *ptrData;

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

    CreateGephiCSVData();

    // В самом конце, так как считаем меры центральной тенденции для всего
    // ранее посчитанного
    CreateDataCorridorAndType();
}

void FAdapOutData::Create()
{
    CreateHeader();

    CreateData();
}

FTableData::FTableData()
{
    iAddInfo = FTypeGraph::dNoInit;    // например, номер УП
}
