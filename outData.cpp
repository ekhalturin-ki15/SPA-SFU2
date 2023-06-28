#include "outData.h"
#include "global.h"
#include "solve.h"
#include "config.h"
#include "error.h"
#include "graph.h"
#include "metric.h"

FOutData::FOutData(FGlobal* _ptrGlobal) : ptrGlobal(_ptrGlobal)
{
}

bool FOutData::Init()
{
	return true;
}

void FOutData::Out(string sOutPath)
{
	OpenXLSX::XLDocument fOutFile;
	fOutFile.create(sOutPath + "/TotalData.xlsx");
	fOutFile.workbook().addWorksheet("Total Data");
	fOutFile.workbook().deleteSheet("Sheet1"); // Стартовая страница не нужна
	OpenXLSX::XLWorksheet wks = fOutFile.workbook().worksheet("Total Data");

	int i = 0; //Задаём порядок вывода
	int x = 0; int y = 0;
	arrHead = { L"Название учебного плана", L"Всего ЗЕ", L"Кол-во дисциплин"
		, L"(Расш.) Общее кол-во ЗЕ в УП", L"(Расш.) Кол-во дисциплин в УП", L"Максимальное ЗЕ у дисциплины"
		, L"Диаметр графа по расстоянию", L"Диаметр графа по количеству рёбер", L"Количество компонент связности"
		, L"Минимальное оставное дерево", L"Максимальное оставное дерево"
	};

	for (auto& sHeaderComp : ptrGlobal->ptrSolve->setHeaderComp)
	{
		wstring wsCompScore = ptrGlobal->ConwertToWstring(sHeaderComp);
		wsCompScore = ptrGlobal->ptrConfig->mapArrOutParams[L"ЗЕ у компетенции"].at(0) + wsCompScore;
		arrHead.push_back(wsCompScore);
		arrHead.push_back(ptrGlobal->ConwertToWstring(sHeaderComp));
	}

	arrOutColm.assign(arrHead.size() + 1, true); // Так как нумерация с 1, поэтому и +1
	
	x = 1, i = 1;for (const auto& it : arrHead)
	{
		if (ptrGlobal->ptrConfig->mapArrOutParams.count(it))
		{
			arrOutColm[i] = (ptrGlobal->ptrConfig->mapArrOutParams[it].at(1) == L"да");
			if (arrOutColm[i++])
			{
				wks.cell(1, x++).value() = ptrGlobal->ConwertToString(ptrGlobal->ptrConfig->mapArrOutParams[it].at(0));
			}
		}
		else
		{
			arrOutColm[i] = true;
			if (it.find(ptrGlobal->ptrConfig->mapArrOutParams[L"ЗЕ у компетенции"].at(0)) != wstring::npos)
			{
				if (ptrGlobal->ptrConfig->mapArrOutParams[L"ЗЕ у компетенции"].at(1) != L"да")
					arrOutColm[i] = false;
			}
			if (arrOutColm[i++])
			{
				wks.cell(1, x++).value() = ptrGlobal->ConwertToString(it);
			}
		}
	
	}
	x = 1; y = 2;

	for (const auto& it : ptrGlobal->ptrSolve->arrDisc)
	{
		//Выводить короткое, или помное имя 
		string sOutName = (ptrGlobal->ptrConfig->bOutShortNameCurr) ? it->sShortNamePlan : it->sNamePlan;

		i = 1;
		x = 1;

		double dSumScoreExt = 0; int iAmountDiscExt = 0;
		it->dFindAllScore(dSumScoreExt, iAmountDiscExt);

		arrResult = { 
		it->dAllSumScore, double(it->iAmountDisc)
		,dSumScoreExt, double(iAmountDiscExt), it->ptrGraph->dMaxDiscScore
		,it->ptrGraph->dDiametrLen, it->ptrGraph->dDiametrStep
		, double(it->ptrGraph->iComponent), it->ptrGraph->dMinSpanTree, it->ptrGraph->dMaxSpanTree
		};

		fOutFile.workbook().addWorksheet(sOutName);
		OutData(x, i, y, sOutName.size(), sOutName, wks, sOutName);
		//wks.cell(y, x++).value() = sOutName;

		for (const auto& dValue : arrResult)
		{
			OutData(x, i, y, dValue, sOutName, wks, to_string(dValue));
		}

		for (auto& sHeaderComp : ptrGlobal->ptrSolve->setHeaderComp)
		{
			if (it->ptrMetric)
			{
				double dRes =
					(ptrGlobal->ptrConfig->bCompInterDelete) ?
					it->ptrMetric->mapCompDistr[FMetric::iAllMetric][sHeaderComp] / double(it->ptrMetric->mapBalancAmountComp[FMetric::iAllMetric]) :
					it->ptrMetric->mapCompDistr[FMetric::iAllMetric][sHeaderComp] / double(it->iAmountDisc);

				
				//Если ЗЕ равно 0, то и не выводить
				if (it->ptrMetric->mapCompDistr[FMetric::iAllMetric][sHeaderComp] > 0)
				{
					OutData(x, i, y, it->ptrMetric->mapCompDistr[FMetric::iAllMetric][sHeaderComp], sOutName, wks, to_string(it->ptrMetric->mapCompDistr[FMetric::iAllMetric][sHeaderComp]));
				}
				else
				{
					if (arrOutColm[i])
					{
						wks.cell(y, x++).value() = "-";
					}
					i++;
				}
			

				if (dRes > ptrGlobal->ptrConfig->dMinComp)
				{
					OutData(x, i, y,  dRes * 100, sOutName, wks, to_string(dRes * 100) + "%");
				}
				else
				{
					wks.cell(y, x++).value() = "-";
					i++;
				}

			}
			else
			{
				x++; //В холостую пропускаем столбцы, они не будут заполнены
				i++;
			}
		}

		++y; //Перевод строки
	}

	//Вывод коридора минимума максимума
	{
		if (ptrGlobal->ptrConfig->mapArrOutParams.count(L"Максимальные значения:"))
		{
			if (ptrGlobal->ptrConfig->mapArrOutParams[L"Максимальные значения:"].at(1) == L"да")
			{
				for (auto& [id, fСorridor] : mapSaveData)
				{
					if (id == 1)
					{
						wks.cell(y, id).value() = ptrGlobal->ConwertToString(
							ptrGlobal->ptrConfig->mapArrOutParams[L"Максимальные значения:"].at(0));
					}
					else
						wks.cell(y, id).value() = to_string(fСorridor.dMax) + " (" + fСorridor.sMax + ")";
				}
				++y;
			}
		}
		if (ptrGlobal->ptrConfig->mapArrOutParams.count(L"Минимальные значения:"))
		{
			if (ptrGlobal->ptrConfig->mapArrOutParams[L"Минимальные значения:"].at(1) == L"да")
			{
				for (auto& [id, fСorridor] : mapSaveData)
				{
					if (id == 1)
					{
						wks.cell(y, id).value() = ptrGlobal->ConwertToString(
							ptrGlobal->ptrConfig->mapArrOutParams[L"Минимальные значения:"].at(0));
					}
					else
						wks.cell(y, id).value() = to_string(fСorridor.dMin) 
						+ " (" + ptrGlobal->ptrConfig->sOutPrefMinMax + fСorridor.sMin + ")";
				}
				++y;
			}
		}

	}


	for (auto& it : ptrGlobal->ptrSolve->arrDisc)
	{
		//Выводить короткое, или помное имя 
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
				continue;//Продолжаем работать, но с другим файлом
			}
		}

		OutGephiData(sOutName, sOutPath , it);

		CreateAndTake(sOutName, sOutPath);
	}
	

	fOutFile.save();
	fOutFile.close();
}


OpenXLSX::XLWorksheet FOutData::CreateAndTake(string sName, string sPath)
{
	OpenXLSX::XLDocument fFile;
	if (ptrGlobal->ptrConfig->bCompactOutput)
	{
		fFile.open(sPath + "/TotalData.xlsx");
		fFile.workbook().addWorksheet(sName);
		return fFile.workbook().worksheet(sName);
	}
	else
	{

		fFile.create(sPath + "/" + sName + "/" + "Data.xlsx");
		fFile.workbook().addWorksheet("Total Data");
		fFile.workbook().deleteSheet("Sheet1"); // Стартовая страница не нужна
		fFile.save();
		return fFile.workbook().worksheet("Total Data");
	}
}
void FOutData::OutGephiData(string sName, string sPath, FTreeDisc* fTree)
{
	OutGephiLable(sName, sPath, fTree);
	OutGephiRib(sName, sPath, fTree);
}

void FOutData::OutGephiLable(string sName, string sPath, FTreeDisc* fTree)
{
	ofstream outLabel(sPath + "/" + sName + "/" + sName + "Lable.csv");

	outLabel << ptrGlobal->ptrConfig->sNameLableHeader << "\n";

	//Сначало id, потом имя
	int i = -1;for (auto& it : fTree->ptrGraph->arrRel)
	{
		++i;
		outLabel << i << ";";
		FTreeElement* fThis = fTree->mapDisc[it];

		wstring wsNameRaw = fThis->wsName;
		string sName = ptrGlobal->ReversUTF16RU(ptrGlobal->ConwertToString(wsNameRaw));
		//Выводим ещё и компетенции
		if (ptrGlobal->ptrConfig->bOutCompWithName)
		{
			sName += "(";

			int j = -1;for (const auto& [sNameComp, arrIndicator] : fThis->mapComp)
			{
				++j;
				if (j) sName += ",";
				sName += sNameComp;
			}

			sName += ")";
		}
		outLabel << sName << "";
		outLabel << "\n";
	}
}

void FOutData::OutGephiRib(string sName, string sPath, FTreeDisc* fTree)
{
	ofstream outLabel(sPath + "/" + sName + "/" + sName + "Rib.csv");

	outLabel << ptrGlobal->ptrConfig->sNameRibHeader << "\n";

	//Откуда, куда, тип (неориентированный), Вес
	for (int l = 0; l < fTree->ptrGraph->fAdjList.size(); ++l)
	{
		for (auto [r, dLen] : fTree->ptrGraph->fAdjList[l])
		{
			//Чтобы не дублировать, он же неориентированный
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
		fSaveData.sMax= sNewData;
	}

	if ((dNewData < fSaveData.dMin) || (fSaveData.sMin == ""))
	{
		fSaveData.dMin = dNewData;
		fSaveData.sMin = sNewData;
	}
	return;
}

void FOutData::OutData(int& x, int& index, const int& y, double dDate, string sDate, OpenXLSX::XLWorksheet& wks, string sOutData)
{
	if (arrOutColm[index++])
	{
		RetakeMinMax(mapSaveData[x], dDate, sDate);
		wks.cell(y, x++).value() = ptrGlobal->ConwertUTF16RU(sOutData);
	}
}