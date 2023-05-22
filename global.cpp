#include "global.h"

#include "config.h"
#include "error.h"
#include "solve.h"

FGlobal::FGlobal() : sNameConfig(L"./config.xlsx"), sNamePage(L"���������")
{
	ptrConfig = new FConfig(this);
	ptrConfig->Init(sNameConfig, sNamePage);

	ptrSolve = new FSolve(this);
	//ptrSolve->Init();

	ptrError = new FError(this);
	ptrError->Init();

	mapTranslit =
	{
		{'�' , 'A'},
		{'�' , '�'},
		{'�' , 'B'},
		{'�' , 'b'},
		{'�' , 'V'},
		{'�' , 'v'},
		{'�' , 'G'},
		{'�' , 'g'},
		{'�' , 'D'},
		{'�' , 'd'},
		{'�' , 'E'},
		{'�' , 'e'},
		{'�' , 'E'},
		{'�' , 'e'},
		{'�' , 'J'},
		{'�' , 'j'},
		{'�' , 'Z'},
		{'�' , 'z'},
		{'�' , 'I'},
		{'�' , 'i'},
		{'�' , 'I'},
		{'�' , 'i'},
		{'�' , 'K'},
		{'�' , 'k'},
		{'�' , 'L'},
		{'�' , 'l'},
		{'�' , 'M'},
		{'�' , 'm'},
		{'�' , 'N'},
		{'�' , 'n'},
		{'�' , 'O'},
		{'�' , 'o'},
		{'�' , 'P'},
		{'�' , 'p'},
		{'�' , 'R'},
		{'�' , 'r'},
		{'�' , 'S'},
		{'�' , 's'},
		{'�' , 'T'},
		{'�' , 't'},
		{'�' , 'U'},
		{'�' , 'u'},
		{'�' , 'F'},
		{'�' , 'f'},
		{'�' , 'H'},
		{'�' , 'h'},
		{'�' , 'h'},
		{'�' , '�'},
		{'�' , 'c'},
		{'�' , 'C'},
		{'�' , 'c'},
		{'�' , 'S'},
		{'�' , 's'},
		{'�' , '�'},
		{'�' , 'c'},
		{'�' , '\''},
		{'�' , '\''},
		{'�' , '_'},
		{'�' , '_'},
		{'�' , '`'},
		{'�' , '`'},
		{'�' , '-'},
		{'�' , '-'},
		{'�' , 'U'},
		{'�' , 'u'},
		{'�' , 'Y'},
		{'�' , 'y'}
	};

	mapWTranslit =
	{
		{L'�' , L'A'},
		{L'�' , L'�'},
		{L'�' , L'B'},
		{L'�' , L'b'},
		{L'�' , L'V'},
		{L'�' , L'v'},
		{L'�' , L'G'},
		{L'�' , L'g'},
		{L'�' , L'D'},
		{L'�' , L'd'},
		{L'�' , L'E'},
		{L'�' , L'e'},
		{L'�' , L'E'},
		{L'�' , L'e'},
		{L'�' , L'J'},
		{L'�' , L'j'},
		{L'�' , L'Z'},
		{L'�' , L'z'},
		{L'�' , L'I'},
		{L'�' , L'i'},
		{L'�' , L'I'},
		{L'�' , L'i'},
		{L'�' , L'K'},
		{L'�' , L'k'},
		{L'�' , L'L'},
		{L'�' , L'l'},
		{L'�' , L'M'},
		{L'�' , L'm'},
		{L'�' , L'N'},
		{L'�' , L'n'},
		{L'�' , L'O'},
		{L'�' , L'o'},
		{L'�' , L'P'},
		{L'�' , L'p'},
		{L'�' , L'R'},
		{L'�' , L'r'},
		{L'�' , L'S'},
		{L'�' , L's'},
		{L'�' , L'T'},
		{L'�' , L't'},
		{L'�' , L'U'},
		{L'�' , L'u'},
		{L'�' , L'F'},
		{L'�' , L'f'},
		{L'�' , L'H'},
		{L'�' , L'h'},
		{L'�' , L'h'},
		{L'�' , L'�'},
		{L'�' , L'c'},
		{L'�' , L'C'},
		{L'�' , L'c'},
		{L'�' , L'S'},
		{L'�' , L's'},
		{L'�' , L'�'},
		{L'�' , L'c'},
		{L'�' , L'\''},
		{L'�' , L'\''},
		{L'�' , L'_'},
		{L'�' , L'_'},
		{L'�' , L'`'},
		{L'�' , L'`'},
		{L'�' , L'-'},
		{L'�' , L'-'},
		{L'�' , L'U'},
		{L'�' , L'u'},
		{L'�' , L'Y'},
		{L'�' , L'y'}
	};

	/*auto fLowTranslit = fTranslit; // ����������� ������� ���
	for (auto [ch, nch] : fLowTranslit)
	{
		fTranslit[tolower(ch)] = nch;
	}*/
}

wstring FGlobal::ConwertToWstring(string sData)
{
	std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
	return(converter.from_bytes(sData.c_str()));

}

string FGlobal::ConwertToString(wstring wsData)
{
	std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> converterX;
	return converterX.to_bytes(wsData);
}

wstring FGlobal::GetValue(OpenXLSX::XLCell cell)
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

//+ ����������� �� ���������
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

//+ ����������� �� ���������
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
}