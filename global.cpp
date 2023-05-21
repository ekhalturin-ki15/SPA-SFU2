#include "global.h"

wstring Global::ConwertToWstring(string sData)
{
	std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
	return(converter.from_bytes(sData.c_str()));

}


wstring Global::GetValue(OpenXLSX::XLCell cell)
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