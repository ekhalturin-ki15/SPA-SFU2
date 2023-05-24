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
	catch (length_error eError)
	{
		ptrGlobal->ptrError->ErrorToMuchColums(sInPath);
		fDoc.close();
		return; //�� ���������� �������� � ������� �������
	}
	catch (logic_error eError)
	{
		ptrGlobal->ptrError->ErrorBadTree(sInPath);
		fDoc.close();
		return; //�� ���������� �������� � ������� �������
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
		return; //�� ���������� �������� � ������� �������
	}

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
	//��������� ���������
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
	int iPreX = -1; // Root ��� �������� ����� (� �� ��� ������)

	for (auto row : fSheet.rows())
	{
		int x = -1;
		bool bReadIndex = false;
		bool bReadName = false;
		bool bReadComp = false;

		if (y != 0)
			for (auto it : row.cells())
			{
				++x;
				wstring wsData = ptrGlobal->GetValue(it);
				if (wsData != L"")
				{
					if ((iIdIndex <= x) && (x < iIdName) && (!bReadIndex)) // ���� ������� � ������ ��� ������ ��������
					{
						bReadIndex = true; //����� �������� �� �������� ������ � ������
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
		++y;
	}

	arrDisc.push_back(ptrTree);
}
	
void FSolve::AddCompIndicator(OpenXLSX::XLWorkbook& fBook, int iKeyPageNumber)
{
	int iIdIndex = 0;
	int iId�ontent = 0;

	auto fSheet = fBook.worksheet(ptrGlobal->ConwertToString(ptrGlobal->ptrConfig->arrKeyPage[iKeyPageNumber].wsName));

	//��������� ���������
	{
		int x = 0;
		for (auto it : fSheet.rows().begin()->cells())
		{
			if (ptrGlobal->GetValue(it) ==
				ptrGlobal->ptrConfig->arrKeyPage[iKeyPageNumber].arrHeader[0])
				iIdIndex = x;
			if (ptrGlobal->GetValue(it) ==
				ptrGlobal->ptrConfig->arrKeyPage[iKeyPageNumber].arrHeader[1])
				iId�ontent = x;
			++x;
		}
	}

	if ()

}

FSolve::~FSolve()
{
	for (auto& it : arrDisc)
		delete it;
}