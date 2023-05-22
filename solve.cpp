#include "solve.h"
#include "error.h"
#include "global.h"


void FSolve::ClearTreeDisc()
{
	for (auto& it : arrDisc)
	{
		delete it;
	}
	arrDisc.clear();
}

void FSolve::Read(string sInPath, string sOutPath)
{
	ClearTreeDisc();

	sInPath = fGlobal->ConwertPathFormat(sInPath);
	sOutPath = fGlobal->ConwertPathFormat(sOutPath);

	OpenXLSX::XLDocument fDoc;
	OpenXLSX::XLWorkbook fBook;

	try
	{
		fDoc.open(sInPath);
	}
	catch(...)
	{
		fError->ErrorOutFileNotFind(fGlobal->ConwertToWstring(sInPath));
		exit(0); //Код ошибки - не удаётся создать папку для вывода
	}
	fBook = fDoc.workbook();
	auto arrNamePage = fBook.worksheetNames();


}