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
	}
	catch (logic_error eError)
	{
		if (FError::sDontHaveIndex== eError.what())
		{
			ptrGlobal->ptrError->ErrorBadParser(sInPath);
		}
		else
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
			if (ptrGlobal->ptrConfig->arrKeyPage[iKeyPageNumber].
				arrHeader[0].count(ptrGlobal->GetValue(it)))
				iIdIndex = x;

			if (ptrGlobal->ptrConfig->arrKeyPage[iKeyPageNumber].
				arrHeader[1].count(ptrGlobal->GetValue(it)))
				iIdName = x;

			if (ptrGlobal->ptrConfig->arrKeyPage[iKeyPageNumber].
				arrHeader[2].count(ptrGlobal->GetValue(it)))
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
		bool bReadIndex = false;
		bool bReadName = false;
		bool bReadComp = false;

		if (y != 0)
		{
			for (auto it : row.cells())
			{
				++x;
				wstring wsData = ptrGlobal->GetValue(it);
				if (wsData != L"")
				{
					if ((iIdIndex <= x) && (x < iIdName) && (!bReadIndex)) // Ищем позицию в дереве для нового предмета
					{
						bReadIndex = true; //Чтобы повторно не находить индекс в строке
						while (iPreX >= x)
						{
							if (ptrThis == nullptr)
							{
								throw std::logic_error(FError::sBadTree);
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
					else
					if ((iIdName <= x) && (x < iIdComp) && (!bReadName))
					{
						bReadName = true;
						ptrNewNode->wsName = wsData;
						continue;
					}
					else
					if ((iIdComp <= x) && (!bReadComp))
					{
						bReadComp = true;
						string sParsingData = ptrGlobal->ConwertToString(ptrGlobal->ConwertPathFormat(wsData)) + ";";

						vector<smatch> matches{
							sregex_iterator{ALL(sParsingData), fRegexComp},
							sregex_iterator{}
						};


						for (auto sData : matches)
						{
							ptrNewNode->mapComp[sData[1].str()] = {};
							ptrTree->fAllComp.insert(sData[1].str());
						}
						break;
					}
				}

			}

			//Игнорируем неправильно оформленную дисциплины
			//Однозначного исправления нет
			if (!bReadIndex)
			{
				throw std::logic_error(FError::sDontHaveIndex); // Неуказанный индекс, это критично
			}
			if (!bReadName)
			{
				ptrGlobal->ptrError->ErrorBadParserName(ptrGlobal->ConwertToWstring(sInPath), ptrNewNode->wsIndexName);
				//throw std::logic_error(FError::sNotAllData); // Щадящий режим, игнорируем неправильные дисциплины
			}
			if (!bReadComp)// Если не считали, значит не указаны
			{
				ptrGlobal->ptrError->ErrorBadParserComp(ptrGlobal->ConwertToWstring(sInPath), ptrNewNode->wsIndexName);
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
	//int iIdСontent = 0;

	auto fSheet = fBook.worksheet(ptrGlobal->ConwertToString(ptrGlobal->ptrConfig->arrKeyPage[iKeyPageNumber].wsName));

	//Считываем заголовок
	{
		int x = 0;
		for (auto it : fSheet.rows().begin()->cells())
		{
			if (ptrGlobal->ptrConfig->arrKeyPage[iKeyPageNumber].
				arrHeader[0].count(ptrGlobal->GetValue(it)))
				iIdIndex = x; //

			//Закоментировал, так как достаточно только одного столбца
			/*if (ptrGlobal->ptrConfig->arrKeyPage[iKeyPageNumber].
				arrHeader[1].count(ptrGlobal->GetValue(it)))
				iIdСontent = x;*/

			++x;
		}
	}

	string sLastComp = "";
	string sLastIndicator = "";
	FTreeElement* ptrThis = nullptr;


	int y = 0;

	for (auto row : fSheet.rows())
	{
		int x = -1;
		bool bReadIndex = false;
		//bool bReadСontent = false;
		bool bThisRowIsDisc = false;

		if (y != 0)
		{
			for (auto it : row.cells())
			{
				++x;
				wstring wsData = ptrGlobal->GetValue(it);

				if (wsData != L"")
				{
					string sData = ptrGlobal->ConwertToString(ptrGlobal->ConwertPathFormat(wsData));
					if ((iIdIndex <= x) && (!bReadIndex))
					{
						bReadIndex = true; //Чтобы повторно не находить индекс в строке
						//Если указан среди перечня компетенций
						if (arrDisc.back()->fAllComp.count(sData))
						{
							sLastComp = sData;
							continue;
						}
						//Если указан среди перечня кодов дисциплин
						if (arrDisc.back()->mapDisc.count(wsData))
						{
							bThisRowIsDisc = true;
							ptrThis = arrDisc.back()->mapDisc[wsData];

							//Значит, мы нашли дисциплину, у которой указать индикатор 
							//соответсвующей компетенции

							if (!ptrThis->mapComp.count(sLastComp))
							{
								//Неправильно указаны компетенции у дисциплины
								//Не к чему соотнести индикатор
								ptrGlobal->ptrError->ErrorBadIndicatorBind
									(sInPath, sLastComp, sLastIndicator);

								break;
							}

							ptrThis->mapComp[sLastComp].push_back(sLastIndicator);

							break;
						}
						//Иначе это индикатор
						sLastIndicator = sData;
						continue;
					}

				}


			}

			//Игнорируем неправильно оформленную дисциплины
			//Однозначного исправления нет
			if (!bReadIndex)
			{
				ptrGlobal->ptrError->ErrorBadParser(sInPath);
			}
		}
		++y;
	}

}

FSolve::~FSolve()
{
	for (auto& it : arrDisc)
		delete it;
}