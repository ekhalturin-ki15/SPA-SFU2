#pragma once
#include <exception>
#include <stdexcept>
#include <string>
#include <vector>
using namespace std;

struct Expression
{
    explicit Expression(string _sToken) : sToken(_sToken)
    {
    }
    explicit Expression(string _sToken, Expression fArg)
        : sToken(_sToken), arrArgument { fArg }
    {
    }
    explicit Expression(string _sToken, Expression fLeftArg,
                        Expression fRightArg)
        : sToken(_sToken), arrArgument { fLeftArg, fRightArg }
    {
    }

    explicit Expression() = default;

    double Calculate(const Expression& fExp);

private:
    string             sToken;
    vector<Expression> arrArgument;
};

struct FormulaParser
{
    FormulaParser() = default;

    explicit FormulaParser(string _sInput, double _dSumScore,
                           int _iAmountDisc);    // Только постоянные значения

    double TakeResult(double _dLeft, double _dRight, int _iPowerComp,
                      int _iPowerDisc);    // Только изменяемые значения

    string sInput = "";

    double dLeft       = 0;    // L
    double dRight      = 0;    // R
    double dSumScore   = 0;    // A
    int    iAmountDisc = 0;    // N
    int    iPowerComp  = 0;    // K
    int    iPowerDisc  = 0;    // D

private:
    Expression Parse();

    string     ParserToken();
    Expression UnaryExp();
    Expression BinaryExp(int iMinPriority);

    int iGetPriority(const string& sOperation);

    int i = 0;    // Указатель на позицию строки

    // Expression fFormula;
};
