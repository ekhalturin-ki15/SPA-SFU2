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

#include <OpenXLSX.hpp>

using namespace std;

typedef long long ll;
#define all(v) v.begin(), v.end()
#define END "\n"

struct FError;
struct FConfig;
struct FSolve;

struct FGlobal
{
    map<char, char> fTranslit;


    const std::wstring sNameConfig; // "Параметры"
    const std::wstring sNamePage; //Хардкод, так как точка опоры

    FGlobal();
    ~FGlobal();

    wstring GetValue(OpenXLSX::XLCell cell);

    wstring ConwertToWstring(string sData);

    string ConwertToString(wstring wsData);

    string ConwertPathFormat(string sFileName, bool bRename = false);

    FError* fError; //Синглтон
    FConfig* fConfig; //Синглтон
    FSolve* fSolve; //Синглтон
};