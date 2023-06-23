#pragma once
#include "global.h"

struct FGlobal;

struct FСorridor
{
	double dMin = 0;
	string sMin = "";
	double dMax = 0;
	string sMax = "";
};

struct FOutData
{
	explicit FOutData(FGlobal* _ptrGlobal);

	void Out(string sOutPath); // Каталог, где будут файлы

	bool Init();

private:
	OpenXLSX::XLWorksheet CreateAndTake(string sName, string sPath);

	FGlobal* ptrGlobal; //Синглтон

	map< int, FСorridor > mapSaveData;
	void RetakeMinMax(FСorridor& fSaveData, const double& dNewData, const string& sNewData);
	void OutData(int& x, const int& y, double dDate, string sDate, OpenXLSX::XLWorksheet& wks, string sOutData);
};