#include "solveSecondPage.h"
#include "solve.h"
#include "error.h"
#include "config.h"
#include "global.h"

FSolveSecondPage::FSolveSecondPage(FGlobal* _ptrGlobal): ptrGlobal(_ptrGlobal)
{
}



void FSolveSecondPage::AddDiscScore(const OpenXLSX::XLWorksheet& fSheet, int iKeyPageNumber)
{
	int h = ptrGlobal->HeightPage(fSheet);

	//Инициализируются один раз
	int iIdAllow = -1; // Не инициализированы
	int iIdIndex = -1; // Позиция столбца, где указан индекс
	int iIdLScore = -1; //  Позиция столбца, где начинается перечесление ЗЕ за семестр
	int iIdRScore = -1;//  Позиция столбца, где заканчивается перечесление ЗЕ за семестр

	//Иная логика считывания, нежеле на предыдущих таблицах
	//Здесь более мягкий режим, так как вся ключевая информация уже была получена, и мы выискиваем доп
	//информацию, чтобы дополнить данные

	//auto fSheet = fBook.worksheet(ptrGlobal->ConwertToString(ptrGlobal->ptrConfig->arrKeyPage[iKeyPageNumber].wsName));

	ptrGlobal->ptrSolve->iCurrentRow = -1;
	for (auto row : fSheet.rows())
	{
		++ptrGlobal->ptrSolve->iCurrentRow;

		if (ptrGlobal->ptrSolve->iCurrentRow == h) break;//Далее только пустые строки

		int x = -1;
		bool bReadAllow = false;
		bool bReadIndex = false;
		//bool bReadScore = false; Их надо считать несколько раз 

		bool bIsAllow = false;
		wstring wsCurrentIndex;

		for (auto it : row.cells())
		{
			++x;
			wstring wsData = ptrGlobal->GetValue(it);

			if (wsData != L"")
			{
				if (ptrGlobal->ptrConfig->arrKeyPage[iKeyPageNumber].
					arrHeader[0].count(wsData))
					iIdAllow = x;
				if (ptrGlobal->ptrConfig->arrKeyPage[iKeyPageNumber].
					arrHeader[1].count(wsData))
					iIdIndex = x;
				if (ptrGlobal->ptrConfig->arrKeyPage[iKeyPageNumber].
					arrHeader[2].count(wsData))
					iIdLScore = x;
				if (ptrGlobal->ptrConfig->arrKeyPage[iKeyPageNumber].
					arrHeader[3].count(wsData))
					iIdRScore = x;

				if ((iIdAllow != -1) && (iIdAllow <= x) && (!bReadAllow))
				{
					bReadAllow = true;
					bIsAllow = (wsData.find(L"+") != wstring::npos);
				}

				if ((iIdIndex != -1) && (iIdIndex <= x) && (x < iIdLScore) && (!bReadIndex))
				{
					bReadIndex = true;
					wsCurrentIndex = wsData;
					//Если указан среди перечня кодов дисциплин
					if (ptrGlobal->ptrSolve->arrDisc.back()->mapDisc.count(wsCurrentIndex))
					{
						auto& ptrThis = ptrGlobal->ptrSolve->arrDisc.back()->mapDisc[wsCurrentIndex];
						ptrThis->bAllow = bIsAllow;
					}
				}


				if ((iIdLScore != -1) && (iIdRScore != -1) && (iIdLScore <= x) && (x < iIdRScore))
				{
					//bReadScore = true;
					//Если указан среди перечня кодов дисциплин
					if (ptrGlobal->ptrSolve->arrDisc.back()->mapDisc.count(wsCurrentIndex))
					{
						auto& ptrThis = ptrGlobal->ptrSolve->arrDisc.back()->mapDisc[wsCurrentIndex];
						int iNumCource;
						double dScore;
						iNumCource = x - iIdLScore; // Нуль нумерация семестров
						iNumCource /= ptrGlobal->ptrConfig->iCourseLen; // Определяем курс, а не семестр
						dScore = atof(ptrGlobal->ConwertToString(wsData).c_str());
						ptrThis->bAllow = bIsAllow;
						ptrThis->dSumScore += dScore;
						ptrThis->mapCourseScore[iNumCource] += dScore;

						if ((bIsAllow) && (ptrThis->arrChild.size() == 0)) // Т.е разрешён и является дисциплиной (не модулем)
						{
							ptrGlobal->ptrSolve->arrDisc.back()->dAllSumScore += dScore;
						}
					
					}
				}

			}
		}
		
	}

	if ((iIdAllow == -1) || (iIdIndex == -1) || (iIdLScore == -1) || (iIdRScore == -1))
	{
		throw std::logic_error(FError::sNotFoundKeyCol); // Неудалось найти ключевой столбец
	}

	ptrGlobal->ptrSolve->arrDisc.back()->iAmountCourse = (iIdRScore - iIdLScore) / ptrGlobal->ptrConfig->iCourseLen;

}


double FSolveSecondPage::DFSCountingScore(FTreeElement* ptrThis)
{
	if (ptrThis->arrChild.size() == 0)
	{
		if (ptrThis->bAllow)
		{
			return ptrThis->dSumScore;
		}
		else
		{
			return 0;
		}
	}

	double dScore = 0;

	for (auto it : ptrThis->arrChild)
	{
		dScore += DFSCountingScore(it);
	}

	return ptrThis->dSumScore = dScore;
}