#include "solve.h"
#include "error.h"
#include "config.h"
#include "global.h"



void FSolve::AddDiscScore(OpenXLSX::XLWorkbook& fBook, int iKeyPageNumber)
{
	int iIdAllow = -1; // Не инициализированы
	int iIdIndex = -1;
	int iIdLScore = -1;
	int iIdRScore = -1;

	auto fSheet = fBook.worksheet(ptrGlobal->ConwertToString(ptrGlobal->ptrConfig->arrKeyPage[iKeyPageNumber].wsName));

	iCurrentRow = -1;
	for (auto row : fSheet.rows())
	{
		++iCurrentRow;
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
					if (arrDisc.back()->mapDisc.count(wsCurrentIndex))
					{
						auto& ptrThis = arrDisc.back()->mapDisc[wsCurrentIndex];
						ptrThis->bAllow = bIsAllow;
					}
				}


				if ((iIdLScore != -1) && (iIdRScore != -1) && (iIdLScore <= x) && (x < iIdRScore))
				{
					//bReadScore = true;
					//Если указан среди перечня кодов дисциплин
					if (arrDisc.back()->mapDisc.count(wsCurrentIndex))
					{
						auto& ptrThis = arrDisc.back()->mapDisc[wsCurrentIndex];
						int iNumCource;
						double dScore;
						iNumCource = x - iIdLScore; // Нуль нумерация семестров
						iNumCource /= ptrGlobal->ptrConfig->iCourseLen; // Определяем курс, а не семестр
						dScore = atof(ptrGlobal->ConwertToString(wsData).c_str());
							
						ptrThis->dSumScore += dScore;
						ptrThis->mapCourseScore[iNumCource] += dScore;

						if ((bIsAllow) && (ptrThis->arrChild.size() == 0)) // Т.е разрешён и является дисциплиной (не модулем)
							arrDisc.back()->dAllSumScore += dScore;
					}
				}

			}
		}
		
	}

	if ((iIdAllow == -1) || (iIdIndex == -1) || (iIdLScore == -1) || (iIdRScore == -1))
	{
		throw std::logic_error(FError::sNotFoundKeyCol); // Неудалось найти ключевой столбец
	}

	arrDisc.back()->iAmountCourse = (iIdRScore - iIdLScore) / ptrGlobal->ptrConfig->iCourseLen;

}