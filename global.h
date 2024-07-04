//v 1.25.7 b

#pragma once
#include <algorithm>
#include <codecvt>
#include <exception>
#include <filesystem>
#include <iostream>
#include <locale>
#include <map>
#include <regex>
#include <set>
#include <string>
#include <vector>

#include <OpenXLSX.hpp>

using namespace std;

typedef long long ll;
#define ALL(v) v.begin(), v.end()
#define END "\n"

struct FError;
struct FConfig;
struct FSolve;
struct FAdapOutData;
struct FOutData;

// Паттерн фасад
struct FGlobal
{
    explicit FGlobal(wstring wsNameConfig);
    ~FGlobal();

    bool ReCreate();
    bool Init(shared_ptr<FGlobal> _ptrThis);

    wstring GetValue(const OpenXLSX::XLCell& cell);

    wstring
        ConwertToWstring(string sData);    // Придётся постоянно конвертировать,
                                           // так как мы точно не знаем,
    // какие данные в ячейках, и всё переводим в наиболее общий тип данных
    // wstring
    string ConwertToString(wstring wsData);

    string ConwertUTF16RU(string sData);    // Перевод в UTF16 RU вручную (через
                                            // mapFirstUnicRU и mapLastUnicRU)
    string ReversUTF16RU(
        string sData) const;    // Перевод из UTF16 RU обратно в строку вручную
                                // (через mapReversUnic)

    string  ConwertPathFormat(string sFileName, bool bRename = false) const;
    wstring ConwertPathFormat(wstring wsFileName, bool bRename = false) const;

    // В Юнит-тестах неправильно определялся текущий путь, поэтому было решено
    // сделать разделяемый через Препроцессорные переменные метод
    filesystem::path GetCurrentPath() const;

    void TakeData(
        bool& outBData,
        const OpenXLSX::XLRow& row);    // Возвращение результата через
                                        // параметры (значение со втрой ячейки)
    void TakeData(vector<wstring>&       outArrData,
                  const OpenXLSX::XLRow& row,
                  int iSize);    // Возвращение результата через параметры
                                 // (значение со втрой ячейки)

    void TakeData(vector<bool>&          outArrData,
                  const OpenXLSX::XLRow& row,
                  int                    iSize);

    void TakeData(vector<string>&        outArrData,
                  const OpenXLSX::XLRow& row,
                  int iSize);    // Возвращение результата через параметры
                                 // (значение со втрой ячейки)
    void TakeData(
        wstring& outWsData,
        const OpenXLSX::XLRow& row);    // Возвращение результата через
                                        // параметры (значение со втрой ячейки)

    // Можно объединить следующие 3 функции через шаблон, но тогда придётся
    // реализовывать в .h
    void TakeData(
        string& outWsData,
        const OpenXLSX::XLRow& row);    // Возвращение результата через
                                        // параметры (значение со втрой ячейки)
    void TakeData(
        int& outIData,
        const OpenXLSX::XLRow& row);    // Возвращение результата через
                                        // параметры (значение со втрой ячейки)
    void TakeData(
        double& outIData,
        const OpenXLSX::XLRow& row);    // Возвращение результата через
                                        // параметры (значение со втрой ячейки)

    vector<pair<wstring, vector<wstring>>>
        SetMapParams(const OpenXLSX::XLWorksheet& fPage, int iSize);
    vector<pair<wstring, vector<wstring>>>
        SetMapParams(const OpenXLSX::XLWorksheet& fPage, set<int> setUsedSize);

    // Возвращает количество строк на странице Excel файла (не включительно, то
    // есть, на 1 больше)
    int HeightPage(const OpenXLSX::XLWorksheet& fSheet);

    void DeleteSpechChars(string& sData)
        const;    // Удаляем спецсимволы из строки, взятые с Config

    string DoubletWithPrecision(const double& dNum) const;

    bool IsThatIsTrue(wstring wsData);    // Не const, так как использует
                                          // преобразование wstring в string
    bool IsThatIsTrue(string sData) const;

    shared_ptr<FError>       ptrError;          // Синглтон
    shared_ptr<FConfig>      ptrConfig;         // Синглтон
    shared_ptr<FSolve>       ptrSolve;          // Синглтон
    shared_ptr<FOutData>     ptrOutData;        // Синглтон
    shared_ptr<FAdapOutData> ptrAdapOutData;    // Синглтон
    // shared_ptr<FGraph> ptrGraph; //У каждого УП свой Граф

    const string& GetSNameFileConfig() const
    {
        return sNameFileConfig;
    }

private:
    static int iSinglControll;

    std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> fConverterToWString1;
    std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t>
        fConverterToString2;

    map<char, char>       mapTranslit;
    map<wchar_t, wchar_t> mapWTranslit;

    map<char, char>             mapFirstUnicRU;
    map<char, char>             mapLastUnicRU;
    map<pair<char, char>, char> mapReversUnic;

    shared_ptr<FGlobal> ptrThis;

    string sNameFileConfig;
};