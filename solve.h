#pragma once

#include "global.h"

struct FGlobal;


struct FTreeElement
{
	FTreeElement();

	bool bAllow; //��������� �� ��� �������� �������� ������ (��)
	int iSumScore; // ���������� �������� ������ (��)
	wstring wsName;

	FTreeElement* ptrPerent;
	vector<FTreeElement*> arrChild;
	vector<string> fComp;
};

struct FTreeDisc
{
	FTreeDisc();
	~FTreeDisc();

	FTreeElement* ptrRoot;
};

struct FSolve
{
	FSolve(FGlobal* _ptrGlobal);

	void ClearTreeDisc();

	void Read(string sInPath, string sOutPath);

	void CreateDiscTree(OpenXLSX::XLWorkbook& fBook, wstring wsNamePage);

	vector<FTreeDisc> arrDisc;

	regex fRegexComp;

	string sInPath; // ��� �������
	string sOutPath;

	FGlobal* ptrGlobal; //��������
};


