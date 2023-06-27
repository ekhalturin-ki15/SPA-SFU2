#include "solve.h"
#include "error.h"
#include "config.h"
#include "global.h"



void FSolve::CreateDiscTree(const OpenXLSX::XLWorksheet& fSheet, int iKeyPageNumber)
{
	int h = ptrGlobal->HeightPage(fSheet);

	int iIdIndex = -1;
	int iIdName = -1;
	int iIdComp = -1;

	FTreeDisc* ptrTree = new FTreeDisc(ptrGlobal);
	arrDisc.push_back(ptrTree);

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

	int iPreX = -1; // Root вне учебного плана (в нём все модули) у него индекс вложенности -1

	iCurrentRow = -1;
	for (auto row : fSheet.rows())
	{
		++iCurrentRow;

		if (iCurrentRow == h) break;//Далее только пустые строки

		int x = -1;
		bool bReadIndex = false;
		bool bReadName = false;
		bool bReadComp = false;

		if (iCurrentRow != 0)
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
							ptrThis = ptrThis->ptrParent;


							iPreX--;
						}
						iPreX = x;
						ptrNewNode = new FTreeElement;
						ptrThis->arrChild.push_back(ptrNewNode);
						ptrNewNode->ptrParent = ptrThis;
						ptrThis = ptrNewNode;

						ptrNewNode->wsIndexName = wsData;
						ptrTree->mapDisc[wsData] = ptrNewNode;
						continue;
					}
					else
						if ((iIdName <= x) && (x < iIdComp) && (!bReadName))
						{
							bReadName = true;

							//Меняем на псевдоним из "Псевдонимы" файла config.xlsx
							if (ptrGlobal->ptrConfig->fAlias.mapRename.count(wsData))
							{
								ptrNewNode->wsName = ptrGlobal->ptrConfig->fAlias.mapRename[wsData];
							}
							else
							{
								ptrNewNode->wsName = wsData;
								ptrNewNode->wsName = ptrNewNode->wsName.substr(
									0, ptrGlobal->ptrConfig->iMaxNameDiscLen);
							}

							ptrNewNode->bNotIgnore = !(ptrGlobal->ptrConfig->setIgnoreDisc.count(wsData));
							continue;
						}
						else
							if ((iIdComp <= x) && (!bReadComp))
							{
								bReadComp = true;
								string sParsingData = ptrGlobal->ConwertToString(ptrGlobal->ConwertPathFormat(wsData)) + ";";

								vector<smatch> matches{
									sregex_iterator{ALL(sParsingData), fRegexComp},
									sregex_iterator{}};


								for (auto sData : matches)
								{
									string sCompName = sData[1].str();
									//Есть ошибка оператора: иногда вместо компетенции указан индикатор
									if (ptrGlobal->ptrConfig->bMultiIndicator)
										sCompName = sCompName.substr(0, sCompName.find('.'));

									ptrGlobal->DeleteSpechChars(sCompName);

									ptrNewNode->mapComp[sCompName] = {};
									ptrTree->fAllComp.insert(sCompName);

									vector<smatch> matchesHeader{
										sregex_iterator{ALL(sCompName), fRegexHeaderComp},
											sregex_iterator{}};

									for (auto sData : matchesHeader)
									{
										string sCompHeaderName = sData[1].str();
										setHeaderComp.insert(sCompHeaderName);
									}

								}
								break;
							}
				}

			}

			//Игнорируем неправильно оформленную дисциплины
			//Однозначного исправления нет

			//Возможно, у нас пустая строка в самом конце страницы, поэтому, можем завершить считывание 
			//Дисциплин на данном моменте
			if (!bReadIndex)
			{
				ptrGlobal->ptrError->ErrorBadParser();
				//throw std::logic_error(FError::sDontHaveIndex); // Неуказанный индекс, это критично
				continue;
			}
			if (!bReadName)
			{
				ptrGlobal->ptrError->ErrorBadParserName(ptrNewNode->wsIndexName);
				//throw std::logic_error(FError::sNotAllData); // Щадящий режим, игнорируем неправильные дисциплины
				continue;
			}
			if (!bReadComp)// Если не считали, значит не указаны
			{
				ptrGlobal->ptrError->ErrorBadParserComp(ptrNewNode->wsIndexName);
				//throw std::logic_error(FError::sNotAllData); // Щадящий режим, игнорируем неправильные дисциплины
				continue;
			}
		}
	}

}
