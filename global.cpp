#include "global.h"

#include "adapOutData.h"
#include "config.h"
#include "error.h"
#include "graph.h"
#include "outData.h"
#include "solve.h"

int FGlobal::iSinglControll = 0;

const double FGlobal::dNoInit = -2e4;
const double FGlobal::dINF    = 1e8;

FGlobal::FGlobal(wstring wsNameConfig)
{
    sNameFileConfig = ConwertToString(GetCurrentPath() / wsNameConfig);

    // Unit test против такого
#ifndef UNIT_TEST
    if (iSinglControll > 0)
        throw std::runtime_error("Re-creation Singleton");
#endif
    ++iSinglControll;

    mapTranslit = {
        { 'А', 'A' },  { 'а', 'а' },  { 'Б', 'B' }, { 'б', 'b' }, { 'В', 'V' },
        { 'в', 'v' },  { 'Г', 'G' },  { 'г', 'g' }, { 'Д', 'D' }, { 'д', 'd' },
        { 'Е', 'E' },  { 'е', 'e' },  { 'Ё', 'E' }, { 'ё', 'e' }, { 'Ж', 'J' },
        { 'ж', 'j' },  { 'З', 'Z' },  { 'з', 'z' }, { 'И', 'I' }, { 'и', 'i' },
        { 'Й', 'I' },  { 'й', 'i' },  { 'К', 'K' }, { 'к', 'k' }, { 'Л', 'L' },
        { 'л', 'l' },  { 'М', 'M' },  { 'м', 'm' }, { 'Н', 'N' }, { 'н', 'n' },
        { 'О', 'O' },  { 'о', 'o' },  { 'П', 'P' }, { 'п', 'p' }, { 'Р', 'R' },
        { 'р', 'r' },  { 'С', 'S' },  { 'с', 's' }, { 'Т', 'T' }, { 'т', 't' },
        { 'У', 'U' },  { 'у', 'u' },  { 'Ф', 'F' }, { 'ф', 'f' }, { 'Х', 'H' },
        { 'х', 'h' },  { 'ч', 'h' },  { 'Ц', 'С' }, { 'ц', 'c' }, { 'Ч', 'C' },
        { 'ч', 'c' },  { 'Ш', 'S' },  { 'ш', 's' }, { 'Щ', 'С' }, { 'щ', 'c' },
        { 'Ъ', '\'' }, { 'ъ', '\'' }, { 'Ы', '_' }, { 'ы', '_' }, { 'Ь', '`' },
        { 'ь', '`' },  { 'Э', '-' },  { 'э', '-' }, { 'Ю', 'U' }, { 'ю', 'u' },
        { 'Я', 'Y' },  { 'я', 'y' }
    };

    mapWTranslit = {
        { L'А', L'A' },  { L'а', L'a' }, { L'Б', L'B' }, { L'б', L'b' },
        { L'В', L'V' },  { L'в', L'v' }, { L'Г', L'G' }, { L'г', L'g' },
        { L'Д', L'D' },  { L'д', L'd' }, { L'Е', L'E' }, { L'е', L'e' },
        { L'Ё', L'E' },  { L'ё', L'e' }, { L'Ж', L'J' }, { L'ж', L'j' },
        { L'З', L'Z' },  { L'з', L'z' }, { L'И', L'I' }, { L'и', L'i' },
        { L'Й', L'I' },  { L'й', L'i' }, { L'К', L'K' }, { L'к', L'k' },
        { L'Л', L'L' },  { L'л', L'l' }, { L'М', L'M' }, { L'м', L'm' },
        { L'Н', L'N' },  { L'н', L'n' }, { L'О', L'O' }, { L'о', L'o' },
        { L'П', L'P' },  { L'п', L'p' }, { L'Р', L'R' }, { L'р', L'r' },
        { L'С', L'S' },  { L'с', L's' }, { L'Т', L'T' }, { L'т', L't' },
        { L'У', L'U' },  { L'у', L'u' }, { L'Ф', L'F' }, { L'ф', L'f' },
        { L'Х', L'H' },  { L'х', L'h' }, { L'ч', L'h' }, { L'Ц', L'C' },
        { L'ц', L'c' },  { L'Ч', L'C' }, { L'ч', L'c' }, { L'Ш', L'S' },
        { L'ш', L's' },  { L'Щ', L'C' }, { L'щ', L'c' }, { L'Ъ', L'\'' },
        { L'ъ', L'\'' }, { L'Ы', L'_' }, { L'ы', L'_' }, { L'Ь', L'`' },
        { L'ь', L'`' },  { L'Э', L'-' }, { L'э', L'-' }, { L'Ю', L'U' },
        { L'ю', L'u' },  { L'Я', L'Y' }, { L'я', L'y' }
    };
    // mapFirstUnicRU = {
    //     { 'а', -48 }, { 'б', -48 }, { 'в', -48 }, { 'г', -48 }, { 'д', -48 },
    //     { 'е', -48 }, { 'ё', -47 }, { 'ж', -48 }, { 'з', -48 }, { 'и', -48 },
    //     { 'й', -48 }, { 'к', -48 }, { 'л', -48 }, { 'м', -48 }, { 'н', -48 },
    //     { 'о', -48 }, { 'п', -48 }, { 'р', -47 }, { 'с', -47 }, { 'т', -47 },
    //     { 'у', -47 }, { 'ф', -47 }, { 'х', -47 }, { 'ц', -47 }, { 'ч', -47 },
    //     { 'ш', -47 }, { 'щ', -47 }, { 'ъ', -47 }, { 'ы', -47 }, { 'ь', -47 },
    //     { 'э', -47 }, { 'ю', -47 }, { 'я', -47 }, { 'А', -48 }, { 'Б', -48 },
    //     { 'В', -48 }, { 'Г', -48 }, { 'Д', -48 }, { 'Е', -48 }, { 'Ё', -48 },
    //     { 'Ж', -48 }, { 'З', -48 }, { 'И', -48 }, { 'Й', -48 }, { 'К', -48 },
    //     { 'Л', -48 }, { 'М', -48 }, { 'Н', -48 }, { 'О', -48 }, { 'П', -48 },
    //     { 'Р', -48 }, { 'С', -48 }, { 'Т', -48 }, { 'У', -48 }, { 'Ф', -48 },
    //     { 'Х', -48 }, { 'Ц', -48 }, { 'Ч', -48 }, { 'Ш', -48 }, { 'Щ', -48 },
    //     { 'Ъ', -48 }, { 'Ы', -48 }, { 'Ь', -48 }, { 'Э', -48 }, { 'Ю', -48 },
    //     { 'Я', -48 }
    // };
    //
    // mapLastUnicRU = {
    //     { 'а', -80 },  { 'б', -79 },  { 'в', -78 },  { 'г', -77 },
    //     { 'д', -76 },  { 'е', -75 },  { 'ё', -111 }, { 'ж', -74 },
    //     { 'з', -73 },  { 'и', -72 },  { 'й', -71 },  { 'к', -70 },
    //     { 'л', -69 },  { 'м', -68 },  { 'н', -67 },  { 'о', -66 },
    //     { 'п', -65 },  { 'р', -128 }, { 'с', -127 }, { 'т', -126 },
    //     { 'у', -125 }, { 'ф', -124 }, { 'х', -123 }, { 'ц', -122 },
    //     { 'ч', -121 }, { 'ш', -120 }, { 'щ', -119 }, { 'ъ', -118 },
    //     { 'ы', -117 }, { 'ь', -116 }, { 'э', -115 }, { 'ю', -114 },
    //     { 'я', -113 }, { 'А', -112 }, { 'Б', -111 }, { 'В', -110 },
    //     { 'Г', -109 }, { 'Д', -108 }, { 'Е', -107 }, { 'Ё', -127 },
    //     { 'Ж', -106 }, { 'З', -105 }, { 'И', -104 }, { 'Й', -103 },
    //     { 'К', -102 }, { 'Л', -101 }, { 'М', -100 }, { 'Н', -99 },
    //     { 'О', -98 },  { 'П', -97 },  { 'Р', -96 },  { 'С', -95 },
    //     { 'Т', -94 },  { 'У', -93 },  { 'Ф', -92 },  { 'Х', -91 },
    //     { 'Ц', -90 },  { 'Ч', -89 },  { 'Ш', -88 },  { 'Щ', -87 },
    //     { 'Ъ', -86 },  { 'Ы', -85 },  { 'Ь', -84 },  { 'Э', -83 },
    //     { 'Ю', -82 },  { 'Я', -81 }
    // };
    string s;
#ifdef DEBUG
    s = {
        -32, -31, -30, -29, -28, -27, -72, -26, -25, -24, -23, -22, -21, -20,
        -19, -18, -17, -16, -15, -14, -13, -12, -11, -10, -9,  -8,  -7,  -6,
        -5,  -4,  -3,  -2,  -1,  -64, -63, -62, -61, -60, -59, -88, -58, -57,
        -56, -55, -54, -53, -52, -51, -50, -49, -48, -47, -46, -45, -44, -43,
        -42, -41, -40, -39, -38, -37, -36, -35, -34, -33,
    };
#else
    s = "абвгдеёжзийклмнопрстуфхцчшщъыьэюяАБВГДЕЁЖЗИЙКЛМНОПРСТУФХЦЧШЩЪЫЬЭЮЯ";
#endif
    mapReversUnic = { { { -48, -80 }, s[0] },   { { -48, -79 }, s[1] },
                      { { -48, -78 }, s[2] },   { { -48, -77 }, s[3] },
                      { { -48, -76 }, s[4] },   { { -48, -75 }, s[5] },
                      { { -47, -111 }, s[6] },  { { -48, -74 }, s[7] },
                      { { -48, -73 }, s[8] },   { { -48, -72 }, s[9] },
                      { { -48, -71 }, s[10] },  { { -48, -70 }, s[11] },
                      { { -48, -69 }, s[12] },  { { -48, -68 }, s[13] },
                      { { -48, -67 }, s[14] },  { { -48, -66 }, s[15] },
                      { { -48, -65 }, s[16] },  { { -47, -128 }, s[17] },
                      { { -47, -127 }, s[18] }, { { -47, -126 }, s[19] },
                      { { -47, -125 }, s[20] }, { { -47, -124 }, s[21] },
                      { { -47, -123 }, s[22] }, { { -47, -122 }, s[23] },
                      { { -47, -121 }, s[24] }, { { -47, -120 }, s[25] },
                      { { -47, -119 }, s[26] }, { { -47, -118 }, s[27] },
                      { { -47, -117 }, s[28] }, { { -47, -116 }, s[29] },
                      { { -47, -115 }, s[30] }, { { -47, -114 }, s[31] },
                      { { -47, -113 }, s[32] }, { { -48, -112 }, s[33] },
                      { { -48, -111 }, s[34] }, { { -48, -110 }, s[35] },
                      { { -48, -109 }, s[36] }, { { -48, -108 }, s[37] },
                      { { -48, -107 }, s[38] }, { { -48, -127 }, s[39] },
                      { { -48, -106 }, s[40] }, { { -48, -105 }, s[41] },
                      { { -48, -104 }, s[42] }, { { -48, -103 }, s[43] },
                      { { -48, -102 }, s[44] }, { { -48, -101 }, s[45] },
                      { { -48, -100 }, s[46] }, { { -48, -99 }, s[47] },
                      { { -48, -98 }, s[48] },  { { -48, -97 }, s[49] },
                      { { -48, -96 }, s[50] },  { { -48, -95 }, s[51] },
                      { { -48, -94 }, s[52] },  { { -48, -93 }, s[53] },
                      { { -48, -92 }, s[54] },  { { -48, -91 }, s[55] },
                      { { -48, -90 }, s[56] },  { { -48, -89 }, s[57] },
                      { { -48, -88 }, s[58] },  { { -48, -87 }, s[59] },
                      { { -48, -86 }, s[60] },  { { -48, -85 }, s[61] },
                      { { -48, -84 }, s[62] },  { { -48, -83 }, s[63] },
                      { { -48, -82 }, s[64] },  { { -48, -81 }, s[65] } };

    for (auto [pr, symbol] : mapReversUnic)
    {
        mapUnic[symbol] = pr;
    }
}

bool FGlobal::Init(shared_ptr<FGlobal> _ptrThis)
{
    ptrThis        = _ptrThis;
    ptrConfig      = make_shared<FConfig>(ptrThis);
    ptrSolve       = make_shared<FSolve>(ptrThis);
    ptrError       = make_shared<FError>(ptrThis);
    ptrAdapOutData = make_shared<FAdapOutData>(ptrThis);
    ptrOutData     = make_shared<FOutData>(ptrThis);

    if (!ptrConfig->Init())
        return false;
    if (!ptrError->Init())
        return false;
    if (!ptrSolve->Init())
        return false;
    if (!ptrAdapOutData->Init())
        return false;
    if (!ptrOutData->Init())
        return false;

    return true;
}

wstring FGlobal::ConwertToWstring(string sData)
{
    try
    {
        return (fConverterToWString1.from_bytes(sData.c_str()));
    }
    catch (...)
    {
        // Значит, была Кириллица
        return (fConverterToWString1.from_bytes(ConwertUTF16RU(sData).c_str()));
    }
}

string FGlobal::ConwertToString(wstring wsData)
{
    return (fConverterToString2.to_bytes(wsData));
}

string FGlobal::ConwertUTF16RU(string sData)
{
    string sOut;
    for (const auto& it : sData)
    {
        if (mapUnic.count(it))
        {
            sOut.push_back(mapUnic[it].first);
            sOut.push_back(mapUnic[it].second);
        }
        else
        {
            sOut.push_back(it);
        }
    }
    return sOut;
}

string FGlobal::ReversUTF16RU(string sData) const
{
    string sOut;
    for (int i = 0; i < sData.size();)
    {
        if (i + 1 < sData.size())
        {
            if (mapReversUnic.count({ sData[i], sData[i + 1] }))
            {
                // Чтобы метод был константный, используем const
                sOut.push_back(
                    mapReversUnic.find({ sData[i], sData[i + 1] })->second);
                i += 2;
            }
            else
            {
                sOut.push_back(sData[i]);
                ++i;
            }
        }
        else
        {
            sOut.push_back(sData[i]);
            ++i;
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
            return cell.value().get<bool>() ? L"1" : L"0";

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
string FGlobal::ConwertPathFormat(string sFileName, bool bRename) const
{
    string sNewName = sFileName;

    for (auto& it : sNewName)
    {
        if (it == '\\')
            it = '/';
        if (mapTranslit.count(it))
            it = mapTranslit.find(it)->second;
    }

    if (bRename)
    {
        try
        {
            filesystem::rename(sFileName, sNewName);
        }
        catch (...)
        {
        }
    }

    return sNewName;
}

//+ Избавляемся от киррилицы
wstring FGlobal::ConwertPathFormat(wstring wsFileName, bool bRename) const
{
    wstring wsNewName = wsFileName;

    for (auto& it : wsNewName)
    {
        if (it == '\\')
            it = '/';
        if (mapWTranslit.count(it))
            it = mapWTranslit.find(it)->second;
    }

    if (bRename)
    {
        try
        {
            filesystem::rename(wsFileName, wsNewName);
        }
        catch (...)
        {
        }
    }

    return wsNewName;
}

FGlobal::~FGlobal()
{
    ptrError.reset();
    ptrConfig.reset();
    ptrSolve.reset();
    ptrOutData.reset();
    // ptrGraph.reset();

    --iSinglControll;
}

bool FGlobal::ReCreate()
{
    // Всё, кроме ptrError и ptrConfig нужно пересоздать
    ptrSolve.reset();
    ptrAdapOutData.reset();
    ptrOutData.reset();

    ptrSolve       = make_shared<FSolve>(ptrThis);
    ptrAdapOutData = make_shared<FAdapOutData>(ptrThis);
    ptrOutData     = make_shared<FOutData>(ptrThis);

    if (!ptrSolve->Init())
        return false;
    if (!ptrAdapOutData->Init())
        return false;
    if (!ptrOutData->Init())
        return false;

    return true;
}
