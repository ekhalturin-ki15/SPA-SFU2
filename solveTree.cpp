#include "solve.h"
#include "error.h"
#include "config.h"
#include "global.h"

FTreeElement::FTreeElement() : dSumScore(0.), wsName(L""), wsIndexName(L""), ptrPerent(nullptr), bAllow(true)
{
}

FTreeDisc::FTreeDisc() : iAmountCourse(0), dAllSumScore(0.), iAmountDisc(0)
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

void FTreeDisc::dFindAllScore(double& outDSum, int& outIAmountDisc)
{
	outDSum = 0;
	outIAmountDisc = 0;
	for (const auto& [key, it] : mapDisc)
	{
		if (it->arrChild.size() == 0)
		{
			outDSum += it->dSumScore;
			outIAmountDisc++;
		}
	}
	return;
}

void FTreeDisc::dFindAllScore(double& outDSum)
{
	outDSum = 0;
	for (const auto& [key, it] : mapDisc)
	{
		if (it->arrChild.size() == 0)
		{
			outDSum += it->dSumScore;
		}
	}
	return;
}

void FTreeDisc::CountDisc()
{
	this->iAmountDisc = 0;
	for (const auto& [key, it] : mapDisc)
	{
		if ((it->arrChild.size() == 0) && (it->bAllow))
		{
			this->iAmountDisc++;
		}
	}
	return;
}