#include "global.h"

#include "config.h"
#include "error.h"
#include "solve.h"

FGlobal::FGlobal() : sNameConfig(L"./config.xlsx"), sNamePage(L"���������")
{
	fConfig = new FConfig(this);
	fSolve = new FSolve(this);
	fError = new FError(this);

	fTranslit = 
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
		if (fTranslit.count(it)) it = fTranslit[it];
	}

	if ((sFileName != sNewName) && (bRename))
		filesystem::rename(sFileName, sNewName);

	return sNewName;
}

FGlobal::~FGlobal()
{
	delete fError;
	delete fConfig;
	delete fSolve;
}