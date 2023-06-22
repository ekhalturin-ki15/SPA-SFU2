#include "global.h"

#include "config.h"
#include "error.h"
#include "solve.h"
#include "outData.h"
#include "graph.h"

FGlobal::FGlobal()
{
	ptrConfig = new FConfig(this);
	ptrSolve = new FSolve(this);
	ptrError = new FError(this);
	ptrOutData = new FOutData(this);
	//ptrGraph = new FGraph(this);

	ptrConfig->Init();
	ptrError->Init();
	ptrSolve->Init();
	ptrOutData->Init();
	//ptrGraph->Init();

	mapTranslit =
	{
		{'А' , 'A'},
		{'а' , 'а'},
		{'Б' , 'B'},
		{'б' , 'b'},
		{'В' , 'V'},
		{'в' , 'v'},
		{'Г' , 'G'},
		{'г' , 'g'},
		{'Д' , 'D'},
		{'д' , 'd'},
		{'Е' , 'E'},
		{'е' , 'e'},
		{'Ё' , 'E'},
		{'ё' , 'e'},
		{'Ж' , 'J'},
		{'ж' , 'j'},
		{'З' , 'Z'},
		{'з' , 'z'},
		{'И' , 'I'},
		{'и' , 'i'},
		{'Й' , 'I'},
		{'й' , 'i'},
		{'К' , 'K'},
		{'к' , 'k'},
		{'Л' , 'L'},
		{'л' , 'l'},
		{'М' , 'M'},
		{'м' , 'm'},
		{'Н' , 'N'},
		{'н' , 'n'},
		{'О' , 'O'},
		{'о' , 'o'},
		{'П' , 'P'},
		{'п' , 'p'},
		{'Р' , 'R'},
		{'р' , 'r'},
		{'С' , 'S'},
		{'с' , 's'},
		{'Т' , 'T'},
		{'т' , 't'},
		{'У' , 'U'},
		{'у' , 'u'},
		{'Ф' , 'F'},
		{'ф' , 'f'},
		{'Х' , 'H'},
		{'х' , 'h'},
		{'ч' , 'h'},
		{'Ц' , 'С'},
		{'ц' , 'c'},
		{'Ч' , 'C'},
		{'ч' , 'c'},
		{'Ш' , 'S'},
		{'ш' , 's'},
		{'Щ' , 'С'},
		{'щ' , 'c'},
		{'Ъ' , '\''},
		{'ъ' , '\''},
		{'Ы' , '_'},
		{'ы' , '_'},
		{'Ь' , '`'},
		{'ь' , '`'},
		{'Э' , '-'},
		{'э' , '-'},
		{'Ю' , 'U'},
		{'ю' , 'u'},
		{'Я' , 'Y'},
		{'я' , 'y'}
	};

	mapWTranslit =
	{
		{L'А' , L'A'},
		{L'а' , L'а'},
		{L'Б' , L'B'},
		{L'б' , L'b'},
		{L'В' , L'V'},
		{L'в' , L'v'},
		{L'Г' , L'G'},
		{L'г' , L'g'},
		{L'Д' , L'D'},
		{L'д' , L'd'},
		{L'Е' , L'E'},
		{L'е' , L'e'},
		{L'Ё' , L'E'},
		{L'ё' , L'e'},
		{L'Ж' , L'J'},
		{L'ж' , L'j'},
		{L'З' , L'Z'},
		{L'з' , L'z'},
		{L'И' , L'I'},
		{L'и' , L'i'},
		{L'Й' , L'I'},
		{L'й' , L'i'},
		{L'К' , L'K'},
		{L'к' , L'k'},
		{L'Л' , L'L'},
		{L'л' , L'l'},
		{L'М' , L'M'},
		{L'м' , L'm'},
		{L'Н' , L'N'},
		{L'н' , L'n'},
		{L'О' , L'O'},
		{L'о' , L'o'},
		{L'П' , L'P'},
		{L'п' , L'p'},
		{L'Р' , L'R'},
		{L'р' , L'r'},
		{L'С' , L'S'},
		{L'с' , L's'},
		{L'Т' , L'T'},
		{L'т' , L't'},
		{L'У' , L'U'},
		{L'у' , L'u'},
		{L'Ф' , L'F'},
		{L'ф' , L'f'},
		{L'Х' , L'H'},
		{L'х' , L'h'},
		{L'ч' , L'h'},
		{L'Ц' , L'С'},
		{L'ц' , L'c'},
		{L'Ч' , L'C'},
		{L'ч' , L'c'},
		{L'Ш' , L'S'},
		{L'ш' , L's'},
		{L'Щ' , L'С'},
		{L'щ' , L'c'},
		{L'Ъ' , L'\''},
		{L'ъ' , L'\''},
		{L'Ы' , L'_'},
		{L'ы' , L'_'},
		{L'Ь' , L'`'},
		{L'ь' , L'`'},
		{L'Э' , L'-'},
		{L'э' , L'-'},
		{L'Ю' , L'U'},
		{L'ю' , L'u'},
		{L'Я' , L'Y'},
		{L'я' , L'y'}
	};

	/*auto fLowTranslit = fTranslit; // Нормального решения нет
	for (auto [ch, nch] : fLowTranslit)
	{
		fTranslit[tolower(ch)] = nch;
	}*/
}

wstring FGlobal::ConwertToWstring(string sData)
{
	return(fConverterToWString1.from_bytes(sData.c_str()));
}

string FGlobal::ConwertToString(wstring wsData)
{
	return fConverterToString2.to_bytes(wsData);
}

wstring FGlobal::GetValue(const OpenXLSX::XLCell& cell)
{
	auto sType = cell.value().type();


	switch (sType)
	{
	case OpenXLSX::XLValueType::Empty:
		return L"";

	case OpenXLSX::XLValueType::Boolean:
		return cell.value().get<bool>()?L"1":L"0";

	case OpenXLSX::XLValueType::Integer:
		return to_wstring(cell.value().get<int>());

	case OpenXLSX::XLValueType::Float:
		return to_wstring(cell.value().get<double>());

	case OpenXLSX::XLValueType::Error:
		return L"";

	case OpenXLSX::XLValueType::String:
		return ConwertToWstring(cell.value().get<string>());

	default:
		return L"";
	}

	

	return L"";
}

//+ Избавляемся от киррилицы
string FGlobal::ConwertPathFormat(string sFileName, bool bRename)
{
	string sNewName = sFileName;

	for (auto& it : sNewName)
	{
		if (it == '\\') it = '/';
		if (mapTranslit.count(it)) it = mapTranslit[it];
	}

	if (bRename)
	{
		try
		{
			filesystem::rename(sFileName, sNewName);
		}
		catch (...) {}
	}

	return sNewName;
}

//+ Избавляемся от киррилицы
wstring FGlobal::ConwertPathFormat(wstring wsFileName, bool bRename)
{
	wstring wsNewName = wsFileName;

	for (auto& it : wsNewName)
	{
		if (it == '\\') it = '/';
		if (mapWTranslit.count(it)) it = mapWTranslit[it];
	}

	if (bRename)
	{
		try
		{
			filesystem::rename(wsFileName, wsNewName);
		}
		catch (...) {}
	}

	return wsNewName;
}

FGlobal::~FGlobal()
{
	delete ptrError;
	delete ptrConfig;
	delete ptrSolve;
	delete ptrOutData;

	//delete ptrGraph;
}