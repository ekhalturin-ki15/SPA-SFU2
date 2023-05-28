#include "solve.h"
#include "error.h"
#include "config.h"
#include "global.h"



void FSolve::AddCompIndicator(OpenXLSX::XLWorkbook& fBook, int iKeyPageNumber)
{
	int iIdIndex = -1;
	//int iIdСontent = -1;

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

	iCurrentRow = -1;
	for (auto row : fSheet.rows())
	{
		++iCurrentRow;
		int x = -1;
		bool bReadIndex = false;
		//bool bReadСontent = false;
		bool bThisRowIsDisc = false;

		if (iCurrentRow != 0)
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
								(sLastComp, sLastIndicator);

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
				ptrGlobal->ptrError->ErrorEmptyLine();
			}
		}
	}

}
