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
#include <codecvt>
#include <locale>

#include <OpenXLSX.hpp>

using namespace std;

typedef long long ll;
#define ALL(v) v.begin(), v.end()
#define END "\n"

struct FError;
struct FConfig;
struct FSolve;
struct FOutData;

//Паттерн фасад
struct FGlobal
{

    explicit FGlobal();
    ~FGlobal();

    bool Init();

    wstring GetValue(const OpenXLSX::XLCell& cell);

    wstring ConwertToWstring(string sData); //Придётся постоянно конвертировать, так как мы точно не знаем, 
    //какие данные в ячейках, и всё переводим в наиболее общий тип данных wstring
    string ConwertToString(wstring wsData);

    string ConwertPathFormat(string sFileName, bool bRename = false);
    wstring ConwertPathFormat(wstring wsFileName, bool bRename = false);

    void TakeData(bool& outBData, const OpenXLSX::XLRow& row); // Возвращение результата через параметры (значение со втрой ячейки)
    void TakeData(vector<wstring>& outArrData, const OpenXLSX::XLRow& row); // Возвращение результата через параметры (значение со втрой ячейки)
    void TakeData(wstring& outWsData, const OpenXLSX::XLRow& row); // Возвращение результата через параметры (значение со втрой ячейки)
    
    //Можно объединить следующие 3 функции через шаблон, но тогда придётся реализовывать в .h
    void TakeData(string& outWsData, const OpenXLSX::XLRow& row); // Возвращение результата через параметры (значение со втрой ячейки)
    void TakeData(int& outIData, const OpenXLSX::XLRow& row); // Возвращение результата через параметры (значение со втрой ячейки)
    void TakeData(double& outIData, const OpenXLSX::XLRow& row); // Возвращение результата через параметры (значение со втрой ячейки)

    vector<pair<wstring, wstring>> SetMapParams(const OpenXLSX::XLWorksheet& fPage);

    //Возвращает количество строк на странице Excel файла (не включительно, то есть, на 1 больше)
    int HeightPage(const OpenXLSX::XLWorksheet& fSheet);

    void DeleteSpechChars(string& sData); //Удаляем спецсимволы из строки, взятые с Config

    FError* ptrError; //Синглтон
    FConfig* ptrConfig; //Синглтон
    FSolve* ptrSolve; //Синглтон
    FOutData* ptrOutData; //Синглтон
    //FGraph* ptrGraph; //У каждого УП свой Граф

private:

    static int iSinglControll;

    std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> fConverterToWString1;
    std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> fConverterToString2;

    map<char, char> mapTranslit;
    map<wchar_t, wchar_t> mapWTranslit;

};