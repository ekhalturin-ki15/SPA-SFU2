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
		, L"Диаметр графа по расстоянию", L"Диаметр графа по количеству рёбер", L"Количество компонент связности", L"Минимальное оставное дерево"
	};

	for (auto& sHeaderComp : ptrGlobal->ptrSolve->setHeaderComp)
	{
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
			i++;
			wks.cell(1, x++).value() = ptrGlobal->ConwertToString(it);
		}
	
	}
	x = 1; y = 2;

	for (const auto& it : ptrGlobal->ptrSolve->arrDisc)
	{
		i = 1;
		x = 1;

		double dSumScoreExt = 0; int iAmountDiscExt = 0;
		it->dFindAllScore(dSumScoreExt, iAmountDiscExt);

		vector< double > arrResult = { 
		it->dAllSumScore, double(it->iAmountDisc)
		,dSumScoreExt, double(iAmountDiscExt), it->ptrGraph->dMaxDiscScore
		,it->ptrGraph->dDiametrLen, it->ptrGraph->dDiametrStep
		, double(it->ptrGraph->iComponent), it->ptrGraph->dMinSpanTree
		};

		fOutFile.workbook().addWorksheet(it->sNamePlan);
		OutData(x, i, y,  it->sNamePlan.size(), it->sNamePlan, wks, it->sNamePlan);
		//wks.cell(y, x++).value() = it->sNamePlan;

		for (const auto& dValue : arrResult)
		{
			OutData(x, i, y, dValue, it->sNamePlan, wks, to_string(dValue));
		}

		for (auto& sHeaderComp : ptrGlobal->ptrSolve->setHeaderComp)
		{
			if (it->ptrMetric)
			{
				double dRes =
					(ptrGlobal->ptrConfig->bCompInterDelete) ?
					it->ptrMetric->mapCompDistr[sHeaderComp] / double(it->ptrMetric->iBalancAmountComp) :
					it->ptrMetric->mapCompDistr[sHeaderComp] / double(it->iAmountDisc);
				
				if (dRes > ptrGlobal->ptrConfig->dMinComp)
				{
					OutData(x, i, y,  dRes * 100, it->sNamePlan, wks, to_string(dRes * 100) + "%");
				}
				else
				{
					i++;
					wks.cell(y, x++).value() = "-";
				}

			}
			else
			{
				i++;
				x++; //В холостую пропускаем столбцы, они не будут заполнены
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
						wks.cell(y, id).value() = to_string(fСorridor.dMin) + " (" + fСorridor.sMin + ")";
				}
				++y;
			}
		}

	}


	for (const auto& it : ptrGlobal->ptrSolve->arrDisc)
	{
		string sNewFile = sOutPath + "/" + it->sNamePlan;

		if (!filesystem::exists(sNewFile))
		{
			try
			{
				filesystem::create_directories(sNewFile);
			}
			catch (...)
			{
				ptrGlobal->ptrError->ErrorOutFileCreate(sNewFile);
			}
		}

		CreateAndTake(it->sNamePlan, sOutPath);
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