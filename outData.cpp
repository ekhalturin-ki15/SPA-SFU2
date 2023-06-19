#include "outData.h"
#include "global.h"
#include "solve.h"
#include "config.h"

FOutData::FOutData(FGlobal* _ptrGlobal) : ptrGlobal(_ptrGlobal)
{
}

void FOutData::Init()
{
}

void FOutData::Out(string sOutPath)
{
	OpenXLSX::XLDocument fOutFile;
	fOutFile.create(sOutPath);
	fOutFile.workbook().addWorksheet("Total Data");
	fOutFile.workbook().deleteSheet("Sheet1"); // Стартовая страница не нужна
	auto wks = fOutFile.workbook().worksheet("Total Data");

	int i = 0; //Задаём порядок вывода
	for (const auto& it : vector<wstring>{ L"Название учебного плана" , L"Всего ЗЕ" , L"Кол-во дисциплин"
		, L"(Расш.) Общее колв-о ЗЕ в УП" , L"(Расш.) Кол-во дисциплин в УП" })

	{
		++i; // Начинаем с 1
		wks.cell(1, i).value() = ptrGlobal->ConwertToString(ptrGlobal->ptrConfig->mapOutParams[it]);
	}
	int x = 1, y = 2;

	for (const auto& it : ptrGlobal->ptrSolve->arrDisc)
	{
		x = 1;
		wks.cell(y, x++).value() = it->sNamePlan;
		fOutFile.workbook().addWorksheet(it->sNamePlan);

		wks.cell(y, x++).value() = it->dAllSumScore;
		wks.cell(y, x++).value() = it->iAmountDisc;

		double dSumScoreExt = 0; int iAmountDiscExt = 0;
		it->dFindAllScore(dSumScoreExt, iAmountDiscExt);
		wks.cell(y, x++).value() = dSumScoreExt;
		wks.cell(y, x++).value() = iAmountDiscExt;



		++y;
	}
	fOutFile.save();
	fOutFile.close();
}