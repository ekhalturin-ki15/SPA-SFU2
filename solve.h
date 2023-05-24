#pragma once

#include "global.h"

struct FGlobal;


struct FTreeElement
{
	FTreeElement();

	bool bAllow; //��������� �� ��� �������� �������� ������ (��)
	int iSumScore; // ���������� �������� ������ (��)
	wstring wsName;
	wstring wsIndexName;

	FTreeElement* ptrPerent;
	vector<FTreeElement*> arrChild;  //���������� ������ ������
	vector<string> fComp;

};

struct FTreeDisc
{
	FTreeDisc();
	~FTreeDisc();

	void DeleteDFS(FTreeElement* ptrThis); // ����� � ������� ��� ������� ������

	FTreeElement* ptrRoot;
	map<wstring, FTreeElement*> mapDisc; //����� ��������� �� ���������� �� � �������

	set< string > fAllComp; //��������� ���� �����������, �������������� � ������� ����� (��)
};

struct FSolve
{
	FSolve(FGlobal* _ptrGlobal);
	~FSolve();

	void ClearTreeDisc();

	void Read(string sInPath, string sOutPath);

	void CreateDiscTree(OpenXLSX::XLWorkbook& fBook, int iKeyPageNumber);

	void AddCompIndicator(OpenXLSX::XLWorkbook& fBook, int iKeyPageNumber);

	vector<FTreeDisc*> arrDisc; // ��������� �� ��� ��, ������� ������� (��� ��� ������������ �������� � ������)

	regex fRegexComp;

	string sInPath; // ��� �������
	string sOutPath;

	FGlobal* ptrGlobal; //��������
};


