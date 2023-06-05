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
	wks.cell(1, 1).value() = ptrGlobal->ConwertToString(ptrGlobal->ptrConfig->mapOutParams[L"Учебный план"]);
	wks.cell(1, 2).value() = ptrGlobal->ConwertToString(ptrGlobal->ptrConfig->mapOutParams[L"Всего ЗЕ"]);
	wks.cell(1, 3).value() = ptrGlobal->ConwertToString(ptrGlobal->ptrConfig->mapOutParams[L"Кол-во дисциплин"]);
	int x = 1, y = 2;

	for (const auto& it : ptrGlobal->ptrSolve->arrDisc)
	{
		x = 1;
		wks.cell(y, x++).value() = it->sNamePlan;
		fOutFile.workbook().addWorksheet(it->sNamePlan);

		wks.cell(y, x++).value() = it->dAllSumScore;
		wks.cell(y, x++).value() = it->mapDisc.size();
		++y;
	}
	fOutFile.save();
	fOutFile.close();
}