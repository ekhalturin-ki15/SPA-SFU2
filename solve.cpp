#include "solve.h"
#include "error.h"
#include "config.h"
#include "global.h"

FTreeElement::FTreeElement() : dSumScore(0.), wsName(L""), wsIndexName(L""), ptrPerent(nullptr), bAllow(true)
{
}

FTreeDisc::FTreeDisc() : iAmountCourse(0), dAllSumScore(0.)
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
	fSolveSecondPage.ptrGlobal = _ptrGlobal;
}

void FSolve::Init()
{
	fRegexComp = (ptrGlobal->ConwertToString(ptrGlobal->ptrConfig->wsRegexComp));
}

void FSolve::ClearTreeDisc()
{
	arrDisc.clear();
}

bool FSolve::Read(string _sInPath, string sNamePlan)
{
	//ClearTreeDisc();

	sInPath = ptrGlobal->ConwertPathFormat(_sInPath, true);

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

		fSolveSecondPage.AddDiscScore(fBook, iCurrentPage);
		fSolveSecondPage.DFSCountingScore(arrDisc.back()->ptrRoot);
		arrDisc.back()->sNamePlan = sNamePlan;

		++iCurrentPage;
	}
	catch (logic_error eError)
	{
		arrDisc.pop_back();
		/*if (FError::sDontHaveIndex== eError.what())
		{
			ptrGlobal->ptrError->ErrorBadParser(sInPath);
		}
		else*/
		if (FError::sBadTree == eError.what())
		{
			ptrGlobal->ptrError->ErrorBadTree();
		}
		else
		if (FError::sNotFoundKeyCol == eError.what())
		{
			ptrGlobal->ptrError->ErrorNotFoundKeyCol();
		}

		fDoc.close();
		return false; //Но продолжаем работать с другими файлами
	}
	catch(...)
	{
		arrDisc.pop_back();
		if (sInPath.find(".xlsx") == string::npos)
		{
			ptrGlobal->ptrError->ErrorUncorrectExtension();
		}
		else
		{
			ptrGlobal->ptrError->ErrorInFileNotFind(sInPath);
		}
		return false; //Но продолжаем работать с другими файлами
	}

	if (this->bIsCorrectParsing)
		ptrGlobal->ptrError->OKParsing();
	else
		ptrGlobal->ptrError->WAParsing();
	fDoc.close();

	return bIsCorrectParsing;
}

FSolve::~FSolve()
{
	for (auto& it : arrDisc)
		delete it;
}