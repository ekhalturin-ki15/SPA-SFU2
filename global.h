#pragma once
#include <string>
#include <iostream>
#include <vector>
#include <filesystem>
#include <algorithm>
#include <map>
#include <set>
#include <string>
#include <locale>
#include <codecvt>
#include <regex>
#include <exception>

#include <OpenXLSX.hpp>

using namespace std;

typedef long long ll;
#define ALL(v) v.begin(), v.end()
#define END "\n"

struct FError;
struct FConfig;
struct FSolve;

struct FGlobal
{
    map<char, char> mapTranslit;
    map<wchar_t, wchar_t> mapWTranslit;


    const std::wstring sNameConfig; // "Параметры"
    const std::wstring sNamePage; //Хардкод, так как точка опоры

    FGlobal();
    ~FGlobal();

    wstring GetValue(OpenXLSX::XLCell cell);

    wstring ConwertToWstring(string sData);

    string ConwertToString(wstring wsData);

    string ConwertPathFormat(string sFileName, bool bRename = false);
    wstring ConwertPathFormat(wstring wsFileName, bool bRename = false);

    FError* ptrError; //Синглтон
    FConfig* ptrConfig; //Синглтон
    FSolve* ptrSolve; //Синглтон
};