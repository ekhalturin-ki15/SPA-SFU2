#include "outData.h"
#include "global.h"
#include "solve.h"

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
	//wks.cell(1, 1).value() = "Учебный план";
	wks.cell(1, 1).value() = "Curricula Name";
	//wks.cell(1, 2).value() = "Всего ЗЕ";
	wks.cell(1, 2).value() = "Total credits";
	//wks.cell(1, 3).value() = "Кол-во дисциплин";
	wks.cell(1, 3).value() = "Number of disciplines";
	int x = 1, y = 2;

	for (const auto& it : ptrGlobal->ptrSolve->arrDisc)
	{
		x = 1;
		wks.cell(y, x++).value() = it->sNamePlan;
		wks.cell(y, x++).value() = it->dAllSumScore;
		wks.cell(y, x++).value() = it->mapDisc.size();
		++y;
	}
	fOutFile.save();
	fOutFile.close();
}