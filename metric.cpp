#include "metric.h"
#include "global.h"
#include "error.h"
#include "config.h"

#include "formulaParser.h"

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

	iBalancAmountComp = _ptrTree->mapDisc.size();

	mapAllowDisc = _ptrTree->GewMapAllowDisc(true, true);
}

void FMetric::Create()
{

	iBalancAmountComp = 0;

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
		iBalancAmountComp += setComp.size();
		for (auto& it : setComp)
		{
			mapCompDistr[it]++;

		}
	}

}
