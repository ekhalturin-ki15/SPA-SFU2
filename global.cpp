#include "global.h"

#include "config.h"
#include "error.h"
#include "solve.h"

FGlobal::FGlobal() : sNameConfig(L"./config.xlsx"), sNamePage(L"Параметры")
{
	fConfig = new FConfig(this);
	fSolve = new FSolve(this);
	fError = new FError(this);

	fTranslit = 
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

	/*auto fLowTranslit = fTranslit; // Нормального решения нет
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

//+ Избавляемся от киррилицы
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