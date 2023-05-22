#include "solve.h"
#include "error.h"
#include "global.h"

FSolve::FSolve(FGlobal* _fGlobal) : fGlobal(_fGlobal)
{
}



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

	sInPath = fGlobal->ConwertPathFormat(sInPath, true);
	sOutPath = fGlobal->ConwertPathFormat(sOutPath);

	OpenXLSX::XLDocument fDoc;
	OpenXLSX::XLWorkbook fBook;

	try
	{
		fDoc.open(sInPath);
	}
	catch(...)
	{
		fGlobal->fError->ErrorInFileNotFind(fGlobal->ConwertToWstring(sInPath));
		return; //Но продолжаем работать с другими файлами
	}
	fBook = fDoc.workbook();
	auto arrNamePage = fBook.worksheetNames();


}