#include "global.h"

#include "config.h"
#include "error.h"
#include "solve.h"
#include "outData.h"
#include "graph.h"

int FGlobal::iSinglControll = 0;

FGlobal::FGlobal()
{
	if (iSinglControll > 0)
		throw std::runtime_error("Re-creation Singleton");
	++iSinglControll;

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

	mapFirstUnicRU = { { 'а', -48 }, { 'б', -48 }, { 'в', -48 }, { 'г', -48 }, { 'д', -48 }, { 'е', -48 }, { 'ё', -47 },
								   { 'ж', -48 }, { 'з', -48 }, { 'и', -48 }, { 'й', -48 }, { 'к', -48 }, { 'л', -48 }, { 'м', -48 },
								   { 'н', -48 }, { 'о', -48 }, { 'п', -48 }, { 'р', -47 }, { 'с', -47 }, { 'т', -47 }, { 'у', -47 },
								   { 'ф', -47 }, { 'х', -47 }, { 'ц', -47 }, { 'ч', -47 }, { 'ш', -47 }, { 'щ', -47 }, { 'ъ', -47 },
								   { 'ы', -47 }, { 'ь', -47 }, { 'э', -47 }, { 'ю', -47 }, { 'я', -47 }, { 'А', -48 }, { 'Б', -48 },
								   { 'В', -48 }, { 'Г', -48 }, { 'Д', -48 }, { 'Е', -48 }, { 'Ё', -48 }, { 'Ж', -48 }, { 'З', -48 },
								   { 'И', -48 }, { 'Й', -48 }, { 'К', -48 }, { 'Л', -48 }, { 'М', -48 }, { 'Н', -48 }, { 'О', -48 },
								   { 'П', -48 }, { 'Р', -48 }, { 'С', -48 }, { 'Т', -48 }, { 'У', -48 }, { 'Ф', -48 }, { 'Х', -48 },
								   { 'Ц', -48 }, { 'Ч', -48 }, { 'Ш', -48 }, { 'Щ', -48 }, { 'Ъ', -48 }, { 'Ы', -48 }, { 'Ь', -48 },
								   { 'Э', -48 }, { 'Ю', -48 }, { 'Я', -48 } };

	mapLastUnicRU = { { 'а', -80 },  { 'б', -79 },  { 'в', -78 },  { 'г', -77 },  { 'д', -76 },  { 'е', -75 },  { 'ё', -111 },
								  { 'ж', -74 },  { 'з', -73 },  { 'и', -72 },  { 'й', -71 },  { 'к', -70 },  { 'л', -69 },  { 'м', -68 },
								  { 'н', -67 },  { 'о', -66 },  { 'п', -65 },  { 'р', -128 }, { 'с', -127 }, { 'т', -126 }, { 'у', -125 },
								  { 'ф', -124 }, { 'х', -123 }, { 'ц', -122 }, { 'ч', -121 }, { 'ш', -120 }, { 'щ', -119 }, { 'ъ', -118 },
								  { 'ы', -117 }, { 'ь', -116 }, { 'э', -115 }, { 'ю', -114 }, { 'я', -113 }, { 'А', -112 }, { 'Б', -111 },
								  { 'В', -110 }, { 'Г', -109 }, { 'Д', -108 }, { 'Е', -107 }, { 'Ё', -127 }, { 'Ж', -106 }, { 'З', -105 },
								  { 'И', -104 }, { 'Й', -103 }, { 'К', -102 }, { 'Л', -101 }, { 'М', -100 }, { 'Н', -99 },  { 'О', -98 },
								  { 'П', -97 },  { 'Р', -96 },  { 'С', -95 },  { 'Т', -94 },  { 'У', -93 },  { 'Ф', -92 },  { 'Х', -91 },
								  { 'Ц', -90 },  { 'Ч', -89 },  { 'Ш', -88 },  { 'Щ', -87 },  { 'Ъ', -86 },  { 'Ы', -85 },  { 'Ь', -84 },
								  { 'Э', -83 },  { 'Ю', -82 },  { 'Я', -81 } };
}

bool FGlobal::Init()
{
	ptrConfig = new FConfig(this);
	ptrSolve = new FSolve(this);
	ptrError = new FError(this);
	ptrOutData = new FOutData(this);

	if (!ptrConfig->Init()) return false;
	if (!ptrError->Init()) return false;
	if (!ptrSolve->Init()) return false;
	if (!ptrOutData->Init()) return false;
	
	return true;
}

wstring FGlobal::ConwertToWstring(string sData)
{
	return(fConverterToWString1.from_bytes(sData.c_str()));
}

string FGlobal::ConwertToString(wstring wsData)
{
	return fConverterToString2.to_bytes(wsData);
}

string FGlobal::ConwertUTF16RU(string sData)
{
	string sOut;
	for (const auto& it : sData)
	{
		if (mapFirstUnicRU.count(it))
		{
			sOut.push_back(mapFirstUnicRU[it]);
			sOut.push_back(mapLastUnicRU[it]);
		}
		else
		{
			sOut.push_back(it);
		}
	}
	return sOut;
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
	--iSinglControll;
	//delete ptrGraph;
}