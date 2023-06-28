#include "metric.h"
#include "global.h"
#include "error.h"
#include "config.h"

#include "formulaParser.h"

const int FMetric::iAllMetric = -1;

//Инверсия зависимости
FMetric::FMetric(FTreeDisc* _ptrTree) : ptrTree(_ptrTree)
{
	try
	{
		fRegexHeaderComp = _ptrTree->ptrGlobal->ptrConfig->sRegexHeaderComp;
	}
	catch (...)
	{
		_ptrTree->ptrGlobal->ptrError->ErrorBadRegex("Регулярное выражение поиска заголовка компетенции");
	}

	mapAllowDisc = _ptrTree->GewMapAllowDisc(true, true);
}

void FMetric::Create()
{
	int iL = -1;
	for (auto& [key, it] : mapAllowDisc)
	{
		++iL;

		set<string> setComp;

		for (auto& [comp, arrInd] : it->mapComp)
		{
			vector<smatch> matches{
				sregex_iterator{ALL(comp), fRegexHeaderComp},
					sregex_iterator{}
			};

			for (auto sData : matches)
			{
				string sCompName = sData[1].str();
				setComp.insert(sCompName);
			}
		}

		//Пересечение компетенций учитываем несколько раз, чтобы получилось 100%
		mapBalancAmountComp[iAllMetric] += setComp.size() * it->dSumScore;
		for (auto& et : setComp)
		{
			mapCompDistr[iAllMetric][et] += it->dSumScore;
		}

		//Теперь высчитываем по каждому курсу по отдельности
		for (int iCourse = 0; iCourse < ptrTree->iAmountCourse; ++iCourse)
		{
			if (!it->mapCourseScore.count(iCourse)) continue; // Чтобы не забивать map нулями
			//Пересечение компетенций учитываем несколько раз, чтобы получилось 100%
			mapBalancAmountComp[iCourse] += setComp.size() * it->mapCourseScore[iCourse];
			for (auto& et : setComp)
			{
				mapCompDistr[iCourse][et] += it->mapCourseScore[iCourse];
			}
		}

	}

}
