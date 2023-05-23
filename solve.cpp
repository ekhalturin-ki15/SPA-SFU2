#include "solve.h"
#include "error.h"
#include "config.h"
#include "global.h"

FTreeElement::FTreeElement() : iSumScore(0), wsName(L""), ptrPerent(nullptr), bAllow(true)
{
}

FTreeDisc::FTreeDisc()
{
	ptrRoot = new FTreeElement;
}

FTreeDisc::~FTreeDisc()
{
	delete ptrRoot;
}

FSolve::FSolve(FGlobal* _ptrGlobal) : ptrGlobal(_ptrGlobal)
{
	fRegexComp = (ptrGlobal->ConwertToString(ptrGlobal->ptrConfig->wsRegexComp));
}

void FSolve::ClearTreeDisc()
{
	arrDisc.clear();
}

void FSolve::Read(string _sInPath, string _sOutPath)
{
	ClearTreeDisc();

	sInPath = ptrGlobal->ConwertPathFormat(_sInPath, true);
	sOutPath = ptrGlobal->ConwertPathFormat(_sOutPath);

	OpenXLSX::XLDocument fDoc;
	OpenXLSX::XLWorkbook fBook;

	try
	{
		fDoc.open(sInPath);
		fBook = fDoc.workbook();
		CreateDiscTree(fBook,
			next(ptrGlobal->ptrConfig->mapKeyPage.begin(), 1)->first); // Вторая страница
	}
	catch (logic_error eError)
	{
		ptrGlobal->ptrError->ErrorBadTree(sInPath);
		return; //Но продолжаем работать с другими файлами
	}
	catch(...)
	{
		ptrGlobal->ptrError->ErrorInFileNotFind(ptrGlobal->ConwertToWstring(sInPath));
		return; //Но продолжаем работать с другими файлами
	}

	fBook = fDoc.workbook();
	auto arrNamePage = fBook.worksheetNames();


}

void FSolve::CreateDiscTree(OpenXLSX::XLWorkbook& fBook, wstring wsNamePage)
{
	int iIdIndex = 0;
	int iIdName = 0;
	int iIdComp = 0;

	auto fSheet = fBook.worksheet(ptrGlobal->ConwertToString( wsNamePage ));

	arrDisc.resize(arrDisc.size() + 1);
	auto& fTree = arrDisc.back();
	FTreeElement* ptrThis = fTree.ptrRoot;
	FTreeElement* ptrNewNode = nullptr;
	//Считываем заголовок
	{
		int x = 0;
		for (auto it : fSheet.rows().begin()->cells())
		{
			if (ptrGlobal->GetValue(it) == 
				ptrGlobal->ptrConfig->mapKeyPage[wsNamePage][0])
				iIdIndex = x;
			if (ptrGlobal->GetValue(it) ==
				ptrGlobal->ptrConfig->mapKeyPage[wsNamePage][1])
				iIdName = x;
			if (ptrGlobal->GetValue(it) ==
				ptrGlobal->ptrConfig->mapKeyPage[wsNamePage][2])
				iIdComp = x;
			++x;
		}
	}

#ifdef DEBUG
	ofstream out(ptrGlobal->ptrConfig->wsNameDebugFile);
#endif // DEBUG

	
	int y = 0;
	int iPreX = -1; // Root вне учебного плана (в нём все модули)

	for (auto row : fSheet.rows())
	{
		int x = -1;
		if (y != 0)
			for (auto it : row.cells())
			{
				++x;
				wstring wsData = ptrGlobal->GetValue(it);
				if (wsData != L"")
				{
					if (x < iIdName) // Ищем позицию в дереве для нового предмета
					{
						while (iPreX >= x)
						{
							if (ptrThis == nullptr)
							{
								throw std::logic_error("BadTree");
								return;
							}
							ptrThis = ptrThis->ptrPerent;
							

							iPreX--;
						}
						iPreX = x;
						ptrNewNode = new FTreeElement;
						ptrThis->arrChild.push_back(ptrNewNode);
						ptrNewNode->ptrPerent = ptrThis;
						ptrThis = ptrNewNode;
						continue;
					}
					if (x < iIdComp)
					{
						ptrNewNode->wsName = wsData;
						continue;
					}

					{
						string sParsingData = ptrGlobal->ConwertToString(ptrGlobal->ConwertPathFormat(wsData)) + ";";

						vector<smatch> matches{
							sregex_iterator{ALL(sParsingData), fRegexComp},
							sregex_iterator{}
						};


						for (auto sData : matches)
						{
							ptrNewNode->fComp.push_back(sData.str());
						}
						continue;
					}
				}

			}
		++y;
	}
}
	
