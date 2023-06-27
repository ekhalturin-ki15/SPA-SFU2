#include "graph.h"
#include "global.h"
#include "error.h"
#include "config.h"

#include "formulaParser.h"

//Инверсия зависимости
FGraph::FGraph(FTreeDisc* _ptrTree) : ptrTree(_ptrTree), iComponent(0)
, dMaxDiscScore(0.), dDiametrLen(0.), dDiametrStep(0.), dMinSpanTree(0.), dMaxSpanTree(0.)
{
	mapAllowDisc = _ptrTree->GewMapAllowDisc(true, true);
	int n = mapAllowDisc.size(); //Кол-во вершин в графе (У нас несколько графов, убрал из полей)
	arrRel.resize(n);
	fAdjLust.resize(n);
	//arrColor.resize(n);

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
						fAdjLust[iL].push_back({ iR, dRibWeight });
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
		CalcDiametrAndComp(dDiametrLen, iComponent, fAdjLust, true);
		CalcDiametrAndComp(dDiametrStep, iComponent, fAdjLust, false);
		CalculateMST(dMinSpanTree, fAdjLust, std::less<pair<double, pair<int, int>>>());
		CalculateMST(dMaxSpanTree, fAdjLust, std::greater<pair<double, pair<int, int>>>());
	}
	catch (...)
	{
		//Игнорируем ошибки, работаем как ни в чём не бывало
	}
}

void FGraph::CalculateMST(double& dResult, const vector< vector <pair<int, double>>>& fCurrentAdj, auto cmp)
{
	dResult = 0;
	vector< pair<double, pair<int, int>>> q;
	int n = fCurrentAdj.size();

	for (int l = 0; l < n; ++l)
	{
		for (const auto& [r, len] : fCurrentAdj[l])
		{
			//Чтобы не дублировать, он же неориентированный
			if ((l < r) || (!ptrTree->ptrGlobal->ptrConfig->bIsUnDirected))
				q.push_back({ len, {l, r} });		
		}
	}
	sort(ALL(q), cmp);

	vector<int> arrDSU(n);
	int i = 0; for (auto& it : arrDSU) it = i++;
	True_DSU<int> fDSU(arrDSU);

	for (const auto& it : q)
	{
		auto [len, pr] = it;
		auto& [l, r] = pr;
		if (!fDSU.isSame(l, r))
		{
			dResult += len;
			fDSU.Merge(l, r);
		}
	}
}

void FGraph::CalcDiametrAndComp(double& dResult, int& iComponent, const vector< vector <pair<int, double>>>& fCurrentAdj, bool IsConsLen)
{
	dResult = 0.;
	iComponent = 0;
	int n = fCurrentAdj.size();

	vector<int> arrColor(n);
	for (int i = 0; i < n; ++i)
	{
		double dTempLen;
		int iTemp = i;
		if (!arrColor[i])
		{
			++iComponent;
			MaxDist(dTempLen, iTemp, arrColor, i, IsConsLen, fCurrentAdj);
			MaxDist(dTempLen, iTemp, arrColor, iTemp, IsConsLen, fCurrentAdj);
			if (dResult < dTempLen)
			{
				dResult = dTempLen;
			}
		}
	}

}


void FGraph::MaxDist(double& dMaxDist, int& iIdNode, vector<int>& arrColor, int iIdStart, bool IsСonsDist, const vector< vector <pair<int, double>>>& fCurrentAdj)
{
	int n = fCurrentAdj.size();

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

		for (auto [l, d] : fCurrentAdj[id])
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