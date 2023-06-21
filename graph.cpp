#include "graph.h"
#include "global.h"
#include "error.h"
#include "config.h"

#include "formulaParser.h"

//Инверсия зависимости
FGraph::FGraph(FTreeDisc* _ptrTree) : ptrTree(_ptrTree)
{
	arrRel.resize(_ptrTree->mapDisc.size());
	fAdjacency.resize(_ptrTree->mapDisc.size());

	int i = 0;
	for (auto& [key, it] : _ptrTree->mapDisc)
	{
		arrRel[i] = key;
		mapReversRel[key] = i;
		++i;
	}
}

void FGraph::Create()
{


	FormulaParser fFormulaParser(
		ptrTree->ptrGlobal->ptrConfig->sFormula
		, ptrTree->dAllSumScore, ptrTree->iAmountDisc);

	int iL = -1;
	for (auto& [keyL, L] : ptrTree->mapDisc)
	{
		++iL;

		if (!L->bAllow) continue;
		if (L->arrChild.size()) continue;

		int iR = -1;
		for (auto& [keyR, R] : ptrTree->mapDisc)
		{
			++iR;

			if (!R->bAllow) continue;
			if (R->arrChild.size()) continue;

			if (iL != iR)
			{
				int iPowerComp = 0; //Сколько компетенций совпало

				for (auto& [keyCompL, arrCompL] : L->mapComp)
				{
					if (R->mapComp.count(keyCompL)) ++iPowerComp; // Совпала компетенция
				}

				try
				{

					double dRibWeight = fFormulaParser.TakeResult(L->dSumScore, R->dSumScore, iPowerComp);

					if (dRibWeight > ptrTree->ptrGlobal->ptrConfig->dMinWeigthRib)
					{
						fAdjacency[iL].push_back({ iR, dRibWeight });
					}
				}
				catch (runtime_error eError)
				{
					ptrTree->ptrGlobal->ptrError->ErrorBadFormula();
					return;
				}
				catch (...)
				{
					//Игнорируем ошибки, работаем как ни в чём не бывало
				}
			
			}
		}
	}
}