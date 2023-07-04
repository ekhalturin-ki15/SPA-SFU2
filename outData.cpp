#include "outData.h"
#include "config.h"
#include "error.h"
#include "global.h"
#include "graph.h"
#include "metric.h"
#include "solve.h"

FOutData::FOutData(FGlobal* _ptrGlobal) : ptrGlobal(_ptrGlobal) {}

bool FOutData::Init() { return true; }

void FOutData::Out(string sOutPath)
{
    OpenXLSX::XLDocument fOutFile;
    fOutFile.create(sOutPath + "/TotalData.xlsx");
    fOutFile.workbook().addWorksheet("Total Data");
    fOutFile.workbook().deleteSheet("Sheet1");    // Стартовая страница не нужна
    OpenXLSX::XLWorksheet wks = fOutFile.workbook().worksheet("Total Data");

    int i   = 0;    // Задаём порядок вывода
    int x   = 0;
    int y   = 0;
    arrHead = { L"Название учебного плана",
                L"Всего ЗЕ",
                L"Кол-во дисциплин",
                L"(Расш.) Общее кол-во ЗЕ в УП",
                L"(Расш.) Кол-во дисциплин в УП",
                L"Максимальное ЗЕ у дисциплины",
                L"Диаметр графа по расстоянию",
                L"Диаметр графа по количеству рёбер",
                L"Количество компонент связности",
                L"Минимальное оставное дерево",
                L"Максимальное оставное дерево" };

    for (auto& sHeaderComp : ptrGlobal->ptrSolve->setHeaderComp)
    {
        wstring wsCompScore = ptrGlobal->ConwertToWstring(sHeaderComp);
        wsCompScore         = ptrGlobal->ptrConfig->mapArrOutParams[L"ЗЕ у компетенции"].at(0) + wsCompScore;
        arrHead.push_back(wsCompScore);
        arrHead.push_back(ptrGlobal->ConwertToWstring(sHeaderComp));
    }

    arrOutColm.assign(arrHead.size() + 1, true);    // Так как нумерация с 1, поэтому и +1

    // Вывод заголовка
    y = 1, x = 1, i = 1;
    for (const auto& it : arrHead)
    {
        if (ptrGlobal->ptrConfig->mapArrOutParams.count(it))
        {
            arrOutColm[i] = (ptrGlobal->ptrConfig->mapArrOutParams[it].at(1) == L"да");
            if (arrOutColm[i++])
            {
                wks.cell(y, x++).value() = ptrGlobal->ConwertToString(ptrGlobal->ptrConfig->mapArrOutParams[it].at(0));
            }
        }
        else
        {
            arrOutColm[i] = true;
            if (it.find(ptrGlobal->ptrConfig->mapArrOutParams[L"ЗЕ у компетенции"].at(0)) != wstring::npos)
            {
                if (ptrGlobal->ptrConfig->mapArrOutParams[L"ЗЕ у компетенции"].at(1) != L"да") arrOutColm[i] = false;
            }
            if (arrOutColm[i++])
            {
                wks.cell(y, x++).value() = ptrGlobal->ConwertToString(it);
            }
        }
    }

    // Вывод данных
    iXShift = 0;
    iYShift = 1;
    y       = 1;
    for (const auto& it : ptrGlobal->ptrSolve->arrDisc)
    {
        // Выводить короткое, или помное имя
        string sOutName = (ptrGlobal->ptrConfig->bOutShortNameCurr) ? it->sShortNamePlan : it->sNamePlan;

        i = 1;
        x = 1;

        double dSumScoreExt   = 0;
        int    iAmountDiscExt = 0;
        it->dFindAllScore(dSumScoreExt, iAmountDiscExt);

        arrResult = { it->dAllSumScore,
                      double(it->iAmountDisc),
                      dSumScoreExt,
                      double(iAmountDiscExt),
                      it->ptrGraph->mapGraph[FGraph::iCommon].dMaxDiscScore,
                      it->ptrGraph->mapGraph[FGraph::iCommon].dDiametrLen,
                      it->ptrGraph->mapGraph[FGraph::iCommon].dDiametrStep,
                      double(it->ptrGraph->mapGraph[FGraph::iCommon].iComponent),
                      it->ptrGraph->mapGraph[FGraph::iCommon].dMinSpanTree,
                      it->ptrGraph->mapGraph[FGraph::iCommon].dMaxSpanTree };

        fOutFile.workbook().addWorksheet(sOutName);
        OutData(x, i, y, sOutName.size(), sOutName, wks, sOutName, false, iXShift, iYShift);

        for (const auto& dValue : arrResult)
        {
            OutData(x, i, y, dValue, sOutName, wks, to_string(dValue), true, iXShift, iYShift);
        }

        for (auto& sHeaderComp : ptrGlobal->ptrSolve->setHeaderComp)
        {
            if (it->ptrMetric)
            {
                auto   ptrTreeMetric = it->ptrMetric->ptrTreeMetric->mapChild[FMetric::sAllMetric];
                double dScore        = 0.;
                double dRes          = 0.;
                if (ptrTreeMetric->mapChild.count(sHeaderComp))
                {
                    dScore = (ptrGlobal->ptrConfig->bCompInterDelete) ? ptrTreeMetric->mapChild[sHeaderComp]->dBalanceSum
                                                                      : ptrTreeMetric->mapChild[sHeaderComp]->dNoBalanceSum;

                    dRes = (ptrGlobal->ptrConfig->bCompInterDelete) ? dScore / ptrTreeMetric->dBalanceSum : dScore / it->dAllSumScore;
                }
                else
                {
                    dScore = 0.;
                    dRes   = 0.;
                }

                // Если ЗЕ равно 0, то и не выводить
                if (dScore > 0)
                {
                    OutData(x, i, y, dScore, sOutName, wks, to_string(dScore), true, iXShift, iYShift);
                }
                else
                {
                    OutData(x, i, y, 0, sOutName, wks, "-", false, iXShift, iYShift);
                }

                if (dRes > ptrGlobal->ptrConfig->dMinComp)
                {
                    OutData(x, i, y, dRes * 100, sOutName, wks, to_string(dRes * 100) + "%", true, iXShift, iYShift);
                }
                else
                {
                    OutData(x, i, y, 0, sOutName, wks, "-", false, iXShift, iYShift);
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
    // Вывод коридора минимума максимума
    {
        if (ptrGlobal->ptrConfig->mapArrOutParams.count(L"Максимальные значения:"))
        {
            if (ptrGlobal->ptrConfig->mapArrOutParams[L"Максимальные значения:"].at(1) == L"да")
            {
                wks.cell(iYShift, 1 + iXShift).value() =
                    ptrGlobal->ConwertToString(ptrGlobal->ptrConfig->mapArrOutParams[L"Максимальные значения:"].at(0));
                for (auto& [id, fСorridor] : mapSaveData)
                {
                    wks.cell(iYShift, id + iXShift).value() = to_string(fСorridor.dMax) + " (" +
                                                              ptrGlobal->ConwertToString(ptrGlobal->ptrConfig->wsOutPrefMinMax) +
                                                              fСorridor.sMax + ")";
                }
                ++iYShift;
            }
        }
        if (ptrGlobal->ptrConfig->mapArrOutParams.count(L"Минимальные значения:"))
        {
            if (ptrGlobal->ptrConfig->mapArrOutParams[L"Минимальные значения:"].at(1) == L"да")
            {
                wks.cell(iYShift, 1 + iXShift).value() =
                    ptrGlobal->ConwertToString(ptrGlobal->ptrConfig->mapArrOutParams[L"Минимальные значения:"].at(0));

                for (auto& [id, fСorridor] : mapSaveData)
                {
                    wks.cell(iYShift, id + iXShift).value() = to_string(fСorridor.dMin) + " (" +
                                                              ptrGlobal->ConwertToString(ptrGlobal->ptrConfig->wsOutPrefMinMax) +
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
        string sOutName = (ptrGlobal->ptrConfig->bOutShortNameCurr) ? it->sShortNamePlan : it->sNamePlan;

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
        OutAddInfo(it);

        fOpenFile.save();
        fOpenFile.close();
    }
}

void FOutData::OutAddInfo(FTreeDisc* ptrTree)
{
    int x = 1;
    int y = 1;

    vector<wstring> arrHead = { L"За какой курс", L"Название компетенции", L"Компетенция", L"Индекс", L"Процент распределения" };
    // Вывод заголовка
    y = 1, x = 1;
    for (const auto& it : arrHead)
    {
        if (ptrTree->ptrGlobal->ptrConfig->mapAddOutParams.count(it))
        {
            fOpenWKS.cell(1, x++).value() = ptrTree->ptrGlobal->ConwertToString(ptrTree->ptrGlobal->ptrConfig->mapAddOutParams[it]);
        }
    }

    y       = 2;
    iXShift = 1;
    iYShift = 1;
    OutRectAddInfo(iXShift, y, ptrTree->ptrMetric->ptrTreeMetric->mapChild[FMetric::sAllMetric]);
    for (auto& [sKey, ptrCurrentTree] : ptrTree->ptrMetric->ptrTreeMetric->mapChild)
    {
        if (sKey == FMetric::sAllMetric) continue;
        //Заголовок дублируется
        for (const auto& it : arrHead)
        {
            if (ptrTree->ptrGlobal->ptrConfig->mapAddOutParams.count(it))
            {
                fOpenWKS.cell(1, x++).value() = ptrTree->ptrGlobal->ConwertToString(ptrTree->ptrGlobal->ptrConfig->mapAddOutParams[it]);
            }
        }
        iXShift += arrHead.size(); 
        OutRectAddInfo(iXShift, y, ptrCurrentTree);
    }
}

int FOutData::OutRectAddInfo(int x, int y, FTreeMetric* ptrMetric)
{
    if (ptrMetric->sName == FMetric::sEmptyIndicator)
    {
        return y + 1;
    }

    fOpenWKS.cell(y, x).value() = ptrMetric->sName;

    if (y > iYShift) iYShift = y;

    if (ptrMetric->mapChild.size() == 0)
    {
        return y + 1;
    }

    for (auto& [sName, ptrChild] : ptrMetric->mapChild)
    {
        y = OutRectAddInfo(x + 1, y, ptrChild);
    }

    return y;
}

void FOutData::CreateAndTake(string sName, string sPath)
{
    if (ptrGlobal->ptrConfig->bCompactOutput)
    {
        fOpenFile.open(sPath + "/TotalData.xlsx");
        if (fOpenFile.workbook().worksheetExists(sName)) fOpenFile.workbook().deleteSheet(sName);

        fOpenFile.workbook().addWorksheet(sName);
        fOpenWKS = fOpenFile.workbook().worksheet(sName);
        return;
    }
    else
    {
        fOpenFile.create(sPath + "/" + sName + "/" + "Data.xlsx");
        fOpenFile.workbook().addWorksheet("Total Data");
        fOpenFile.workbook().deleteSheet("Sheet1");    // Стартовая страница не нужна
        fOpenFile.save();
        fOpenWKS = fOpenFile.workbook().worksheet("Total Data");
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

void FOutData::OutGephiData(string sName, string sPath, FTreeDisc* fTree)
{
    {
        // Выводим обычный граф
        vector<string> arrCommonNameLabel;
        for (auto& it : fTree->ptrGraph->mapGraph[FGraph::iCommon].arrRel)
        {
            FTreeElement* fThis     = fTree->mapDisc[it.first];
            wstring       wsNameRaw = fThis->wsName;
            string        sName     = ptrGlobal->ReversUTF16RU(ptrGlobal->ConwertToString(wsNameRaw));
            // Выводим ещё и компетенции
            if (ptrGlobal->ptrConfig->bOutCompWithName)
            {
                sName += AddCompString(fThis->mapComp);
            }
            arrCommonNameLabel.push_back(sName);
        }
        OutGephiLable(sName, sName, sPath, arrCommonNameLabel);
        OutGephiRib(sName, sName, sPath, fTree->ptrGraph->mapGraph[FGraph::iCommon].fAdjList);
    }

    {
        // Выводим альтернативный граф
        vector<string> arrCommonNameLabel;
        for (auto& it : fTree->ptrGraph->mapGraph[FGraph::iAlt].arrRel)
        {
            FTreeElement* fThis     = fTree->mapDisc[it.first];
            wstring       wsNameRaw = fThis->wsName;
            string        sName     = ptrGlobal->ReversUTF16RU(ptrGlobal->ConwertToString(wsNameRaw));
            sName += ptrGlobal->ptrConfig->sPrefCourseNumber + to_string(it.second + 1); // Не забываем про нуль нумерацию курсов
            // Выводим ещё и компетенции
            if (ptrGlobal->ptrConfig->bOutCompWithName)
            {
                sName += AddCompString(fThis->mapComp);
            }
            arrCommonNameLabel.push_back(sName);
        }
        OutGephiLable(sName, sName + ptrGlobal->ptrConfig->sSufAltGraphFile, sPath, arrCommonNameLabel);
        OutGephiRib(sName, sName + ptrGlobal->ptrConfig->sSufAltGraphFile, sPath, fTree->ptrGraph->mapGraph[FGraph::iAlt].fAdjList);
    }
}

void FOutData::OutGephiLable(const string& sName, const string& sNameFile, const string& sPath, const vector<string>& arrNameLabel)
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

void FOutData::OutGephiRib(const string&                            sName,
                           const string&                            sNameFile,
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

void FOutData::RetakeMinMax(FСorridor& fSaveData, const double& dNewData, const string& sNewData)
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

void FOutData::OutData(int&                   x,
                       int&                   index,
                       const int&             y,
                       double                 dDate,
                       string                 sDate,
                       OpenXLSX::XLWorksheet& wks,
                       string                 sOutData,
                       const bool&            bIsConsider,
                       const int&             iXShift,
                       const int&             iYShift)
{
    if (arrOutColm[index++])
    {
        if (bIsConsider) RetakeMinMax(mapSaveData[x], dDate, sDate);

        wks.cell(iYShift + y, iXShift + x++).value() = ptrGlobal->ConwertUTF16RU(sOutData);
    }
}