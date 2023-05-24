#include "solve.h"
#include "error.h"
#include "config.h"
#include "global.h"

const string FError::sBadTree = "Bad Tree";
const string FError::sNotAllData = "Not all data";

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

	try
	{
		fDoc.open(sInPath);
		fBook = fDoc.workbook();
		CreateDiscTree(fBook, 0);
			//ptrGlobal->ptrConfig->arrKeyPage[0].wsName);

		AddCompIndicator(fBook, 1);
			//ptrGlobal->ptrConfig->arrKeyPage[1].wsName);
	}
	catch (logic_error eError)
	{
		/*if (FError::sNotAllData == eError.what())
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

	ptrGlobal->ptrError->OKParsing(sInPath);
	fDoc.close();
}

void FSolve::CreateDiscTree(OpenXLSX::XLWorkbook& fBook, int iKeyPageNumber)
{
	int iIdIndex = 0;
	int iIdName = 0;
	int iIdComp = 0;

	auto fSheet = fBook.worksheet(ptrGlobal->ConwertToString(ptrGlobal->ptrConfig->arrKeyPage[iKeyPageNumber].wsName ));

	FTreeDisc* ptrTree = new FTreeDisc;
	FTreeElement* ptrThis = ptrTree->ptrRoot;
	FTreeElement* ptrNewNode = nullptr;
	//Считываем заголовок
	{
		int x = 0;
		for (auto it : fSheet.rows().begin()->cells())
		{
			if (ptrGlobal->GetValue(it) == 
				ptrGlobal->ptrConfig->arrKeyPage[iKeyPageNumber].arrHeader[0])
				iIdIndex = x;
			if (ptrGlobal->GetValue(it) ==
				ptrGlobal->ptrConfig->arrKeyPage[iKeyPageNumber].arrHeader[1])
				iIdName = x;
			if (ptrGlobal->GetValue(it) ==
				ptrGlobal->ptrConfig->arrKeyPage[iKeyPageNumber].arrHeader[2])
				iIdComp = x;
			++x;
		}
	}

#ifdef DEBUG
	ofstream out(ptrGlobal->ptrConfig->wsNameDebugFile);
#endif // DEBUG

	
	int y = 0;
	vector<int> arrPreX = { -1 }; // Root вне учебного плана (в нём все модули)

	for (auto row : fSheet.rows())
	{
		int x = -1;
		bool bReadIndex = false;
		bool bReadName = false;
		bool bReadComp = false;

		if (y != 0)
		{
			for (auto it : row.cells())
			{
				++x;
				arrPreX.push_back(arrPreX.back());

				wstring wsData = ptrGlobal->GetValue(it);
				if (wsData != L"")
				{
					if ((iIdIndex <= x) && (x < iIdName) && (!bReadIndex)) // Ищем позицию в дереве для нового предмета
					{
						bReadIndex = true; //Чтобы повторно не находить индекс в строке
						while (arrPreX.back() >= x)
						{
							if (ptrThis == nullptr)
							{
								throw std::logic_error(FError::sBadTree);
								return;
							}
							ptrThis = ptrThis->ptrPerent;


							arrPreX.back()--;
						}
						arrPreX.back() = x;
						ptrNewNode = new FTreeElement;
						ptrThis->arrChild.push_back(ptrNewNode);
						ptrNewNode->ptrPerent = ptrThis;
						ptrThis = ptrNewNode;

						ptrNewNode->wsIndexName = wsData;
						ptrTree->mapDisc[wsData] = ptrNewNode;
						continue;
					}
					if ((x < iIdComp) && (!bReadName))
					{
						bReadName = true;
						ptrNewNode->wsName = wsData;
						continue;
					}

					if (!bReadComp)
					{
						bReadComp = true;
						string sParsingData = ptrGlobal->ConwertToString(ptrGlobal->ConwertPathFormat(wsData)) + ";";

						vector<smatch> matches{
							sregex_iterator{ALL(sParsingData), fRegexComp},
							sregex_iterator{}
						};


						for (auto sData : matches)
						{
							ptrNewNode->fComp.push_back(sData.str());
							ptrTree->fAllComp.insert(sData.str());
						}
						continue;
					}
				}

			}

			if ((ptrNewNode->wsName == L"") || (ptrNewNode->fComp.size() == 0))
			{
				ptrGlobal->ptrError->ErrorBadParser(sInPath);

				//Удаление неправильной оформленной дисциплины
				ptrThis = ptrNewNode->ptrPerent;
				ptrNewNode->ptrPerent->arrChild.pop_back();
				delete ptrNewNode;
				arrPreX.pop_back();
				//throw std::logic_error(FError::sNotAllData); // Щадящий режим, игнорируем неправильные дисциплины
				
			}
		}

		++y;
	}

	


	arrDisc.push_back(ptrTree);
}
	
void FSolve::AddCompIndicator(OpenXLSX::XLWorkbook& fBook, int iKeyPageNumber)
{
	int iIdIndex = 0;
	int iIdСontent = 0;

	auto fSheet = fBook.worksheet(ptrGlobal->ConwertToString(ptrGlobal->ptrConfig->arrKeyPage[iKeyPageNumber].wsName));

	//Считываем заголовок
	{
		int x = 0;
		for (auto it : fSheet.rows().begin()->cells())
		{
			if (ptrGlobal->GetValue(it) ==
				ptrGlobal->ptrConfig->arrKeyPage[iKeyPageNumber].arrHeader[0])
				iIdIndex = x;
			if (ptrGlobal->GetValue(it) ==
				ptrGlobal->ptrConfig->arrKeyPage[iKeyPageNumber].arrHeader[1])
				iIdСontent = x;
			++x;
		}
	}


}

FSolve::~FSolve()
{
	for (auto& it : arrDisc)
		delete it;
}