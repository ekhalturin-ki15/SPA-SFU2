#include "graph.h"
#include "global.h"
#include "error.h"
#include "config.h"

#include "formulaParser.h"

//Инверсия зависимости
FGraph::FGraph(FTreeDisc* _ptrTree) : ptrTree(_ptrTree), n(0), iComponent(0)
, dMaxDiscScore(0.), dDiametrLen(0.), dDiametrStep(0.), dMinSpanTree(0.)
{
	mapAllowDisc = _ptrTree->GewMapAllowDisc(true, true);
	n = mapAllowDisc.size(); //Кол-во вершин в графе
	arrRel.resize(n);
	fAdjacency.resize(n);
	arrColor.resize(n);
	//arrPreColor.resize(n);

	int i = 0;
	for (auto& [key, it] : mapAllowDisc)
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
	for (auto& [keyL, L] : mapAllowDisc)
	{
		++iL;

		if (!L->bAllow) continue;
		if (L->arrChild.size()) continue;

		int iR = -1;
		for (auto& [keyR, R] : mapAllowDisc)
		{
			++iR;

			if (!R->bAllow) continue;
			if (R->arrChild.size()) continue;

			if (dMaxDiscScore < R->dSumScore) dMaxDiscScore = R->dSumScore;

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
	try
	{
		CalculateDiametrAndComponent();
		CalculateMST();
	}
	catch (...)
	{
		//Игнорируем ошибки, работаем как ни в чём не бывало
	}
}

void FGraph::CalculateMST()
{
	vector< pair<double, pair<int, int>>> q;

	for (int l = 0; l < n; ++l)
	{
		for (const auto& [r, len] : fAdjacency[l])
		{
			q.push_back({ len, {l, r} });
		}
	}
	sort(ALL(q));

	vector<int> arrDSU(n);
	int i = 0; for (auto& it : arrDSU) it = i++;
	True_DSU<int> fDSU(arrDSU);

	for (const auto& it : q)
	{
		auto [len, pr] = it;
		auto& [l, r] = pr;
		if (!fDSU.isSame(l, r))
		{
			dMinSpanTree += len;
			fDSU.Merge(l, r);
		}
	}
}

void FGraph::CalculateDiametrAndComponent()
{
	UpdateArrColor();

	dDiametrLen = 0.;
	iComponent = 0;

	for (int i = 0; i < n; ++i)
	{
		double dTempLen;
		int iTemp = i;
		if (!arrColor[i])
		{
			++iComponent;
			MaxDist(dTempLen, iTemp, i, true);
			MaxDist(dTempLen, iTemp, iTemp, true);
			if (dDiametrLen < dTempLen)
			{
				dDiametrLen = dTempLen;
			}
			MaxDist(dTempLen, iTemp, i, false);
			MaxDist(dTempLen, iTemp, iTemp, false);
			if (dDiametrStep < dTempLen)
			{
				dDiametrStep = dTempLen;
			}
		}
	}

}

void FGraph::UpdateArrColor()
{
	arrColor.assign(n, 0);
}

void FGraph::MaxDist(double& dMaxDist, int& iIdNode, int iIdStart, bool IsСonsDist)
{
	dMaxDist = 0.;
	iIdNode = iIdStart;

	priority_queue<pair<double, int>> q;
	q.push({0., iIdStart });

	vector<double> arrLen(n, 1e9);
	vector<int> arrPass(n, 0);
	arrLen[iIdStart] = 0.;

	while (q.size())
	{
		auto [dLen, id] = q.top();
		dLen = -dLen;
		q.pop();
		if (arrPass[id]) continue;
		arrPass[id] = 1;

		arrColor[id] = 1;


		if (dMaxDist < dLen)
		{
			dMaxDist = dLen;
			iIdNode = id;
		}

		for (auto [l, d] : fAdjacency[id])
		{
			double dNewLen = (IsСonsDist) ? d : 1; // Учитываем ли мы растояние, или шаги
			if (arrLen[l] > dLen + dNewLen)
			{
				arrLen[l] = dLen + dNewLen;
				q.push({ -arrLen[l] , l});
			}
		}
	}

	return;
}