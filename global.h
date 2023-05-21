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

struct Global
{
    std::string sNoIdentity;

    const std::string sNameConfig = "./config.xlsx"; // "���������"
    const std::string sNamePage = "Параметры"; //�������, ��� ��� ����� �����

    wstring GetValue(OpenXLSX::XLCell cell);

    wstring ConwertToWstring(string sData);
};

static Global fGlobal; //��������