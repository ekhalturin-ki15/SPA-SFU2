#include "solve.h"
#include "error.h"
#include "config.h"
#include "global.h"

FTreeElement::FTreeElement() : iSumScore(0), wsName(L""), wsIndexName(L""), ptrPerent(nullptr), bAllow(true)
{
}

FTreeDisc::FTreeDisc()
{
	ptrRoot = new FTreeElement;
}

FTreeDisc::~FTreeDisc()
{
	DeleteDFS(ptrRoot);
}

void FTreeDisc::DeleteDFS(FTreeElement* ptrThis)
{
	for (auto it : ptrThis->arrChild)
	{
		DeleteDFS(it);
	}
	delete ptrThis;
}

FSolve::FSolve(FGlobal* _ptrGlobal) : ptrGlobal(_ptrGlobal)
{
}

void FSolve::Init()
{
	fRegexComp = (ptrGlobal->ConwertToString(ptrGlobal->ptrConfig->wsRegexComp));
}

void FSolve::ClearTreeDisc()
{
	arrDisc.clear();
}

void FSolve::Read(string _sInPath, string _sOutPath)
{
	//ClearTreeDisc();

	sInPath = ptrGlobal->ConwertPathFormat(_sInPath, true);
	sOutPath = ptrGlobal->ConwertPathFormat(_sOutPath);

	OpenXLSX::XLDocument fDoc;
	OpenXLSX::XLWorkbook fBook;
	iCurrentPage = 0;
	bIsCorrectParsing = true;

	try
	{
		fDoc.open(sInPath);
		fBook = fDoc.workbook();
		CreateDiscTree(fBook, iCurrentPage);
			//ptrGlobal->ptrConfig->arrKeyPage[0].wsName);
		++iCurrentPage;

		AddCompIndicator(fBook, iCurrentPage);
		++iCurrentPage;
			//ptrGlobal->ptrConfig->arrKeyPage[1].wsName);

		AddDiscScore(fBook, iCurrentPage);
		++iCurrentPage;
	}
	catch (logic_error eError)
	{
		/*if (FError::sDontHaveIndex== eError.what())
		{
			ptrGlobal->ptrError->ErrorBadParser(sInPath);
		}
		else*/
		if (FError::sBadTree == eError.what())
		{
			ptrGlobal->ptrError->ErrorBadTree(sInPath);
		}
		fDoc.close();
		return; //Но продолжаем работать с другими файлами
	}
	catch(...)
	{
		if (sInPath.find(".xlsx") == string::npos)
		{
			ptrGlobal->ptrError->ErrorUncorrectExtension(sInPath);
		}
		else
		{
			ptrGlobal->ptrError->ErrorInFileNotFind(sInPath);
		}
		return; //Но продолжаем работать с другими файлами
	}

	if (this->bIsCorrectParsing)
		ptrGlobal->ptrError->OKParsing(sInPath);
	else
		ptrGlobal->ptrError->WAParsing(sInPath);
	fDoc.close();
}

FSolve::~FSolve()
{
	for (auto& it : arrDisc)
		delete it;
}