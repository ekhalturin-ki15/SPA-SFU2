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

	vector<wstring> arrHead = { L"Название учебного плана", L"Всего ЗЕ", L"Кол-во дисциплин"
		, L"(Расш.) Общее колв-о ЗЕ в УП", L"(Расш.) Кол-во дисциплин в УП" };

	for (auto& sHeaderComp : ptrGlobal->ptrSolve->setHeaderComp)
	{
		arrHead.push_back(ptrGlobal->ConwertToWstring(sHeaderComp));
	}

	for (const auto& it : arrHead)

	{
		++i; // Начинаем с 1
		if (ptrGlobal->ptrConfig->mapOutParams.count(it))
			wks.cell(1, i).value() = ptrGlobal->ConwertToString(ptrGlobal->ptrConfig->mapOutParams[it]);
		else
			wks.cell(1, i).value() = ptrGlobal->ConwertToString(it);
	
	}
	int x = 1, y = 2;

	for (const auto& it : ptrGlobal->ptrSolve->arrDisc)
	{
		x = 1;

		fOutFile.workbook().addWorksheet(it->sNamePlan);

		OutData(x, y, it->sNamePlan.size(), it->sNamePlan, wks, it->sNamePlan);
		//wks.cell(y, x++).value() = it->sNamePlan;

		OutData(x, y, it->dAllSumScore, it->sNamePlan, wks, to_string(it->dAllSumScore));
		//wks.cell(y, x++).value() = it->dAllSumScore;

		OutData(x, y, it->iAmountDisc, it->sNamePlan, wks, to_string(it->iAmountDisc));
		//wks.cell(y, x++).value() = it->iAmountDisc;

		double dSumScoreExt = 0; int iAmountDiscExt = 0;
		it->dFindAllScore(dSumScoreExt, iAmountDiscExt);
		OutData(x, y, dSumScoreExt, it->sNamePlan, wks, to_string(dSumScoreExt));
		//wks.cell(y, x++).value() = dSumScoreExt;
		OutData(x, y, iAmountDiscExt, it->sNamePlan, wks, to_string(iAmountDiscExt));
		//wks.cell(y, x++).value() = iAmountDiscExt;

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
					OutData(x, y, dRes * 100, it->sNamePlan, wks, to_string(dRes * 100) + "%");
				}
				else
				{
					wks.cell(y, x++).value() = "-";
				}

			}
			else
			{
				x++; //В холостую пропускаем столбцы, они не будут заполнены
			}
		}

		++y; //Перевод строки
	}

	//Вывод коридора минимума максимума
	{
		for (auto& [id, fСorridor]: mapSaveData)
		{
			if (id == 1)
			{
				wks.cell(y, id).value() = "Максимальные значения:";
			}
			else
				wks.cell(y, id).value() = to_string(fСorridor.dMax) + " у УП " + fСorridor.sMax;
		}
		++y;
		for (auto& [id, fСorridor] : mapSaveData)
		{
			if (id == 1)
			{
				wks.cell(y, id).value() = "Минимальные значения:";
			}
			else
				wks.cell(y, id).value() = to_string(fСorridor.dMin) + " у УП " + fСorridor.sMin;
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

void FOutData::OutData(int& x, const int& y, double dDate, string sDate, OpenXLSX::XLWorksheet& wks, string sOutData)
{
	RetakeMinMax(mapSaveData[x], dDate, sDate);
	wks.cell(y, x++).value() = sOutData;
}