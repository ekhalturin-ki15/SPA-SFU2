#pragma once
#include <string>
#include <vector>
#include <stdexcept>
#include <exception>
using namespace std;

struct Expression
{
	explicit Expression(string _sToken) : sToken(_sToken) {}
	explicit Expression(string _sToken, Expression fArg) : sToken(_sToken), arrArgument{ fArg } {}
	explicit Expression(string _sToken, Expression fLeftArg, Expression fRightArg) : sToken(_sToken),
		arrArgument{ fLeftArg, fRightArg } {}

	double Calculate(const Expression& fExp);

private:
	string sToken;
	vector<Expression> arrArgument;
};

struct FormulaParser
{
	explicit FormulaParser(string _sInput, double _dSumScore, int _iAmountDisc); //Только постоянные значения
	
	double TakeResult(double _dLeft, double _dRight, int _iPowerComp);//Только изменяемые значения

	string sInput = "";

	double dLeft = 0; // L
	double dRight = 0; // R
	double dSumScore = 0; // A
	int iAmountDisc = 0; // N
	int iPowerComp = 0; // K

private:

	Expression Parse();

	string ParserToken();
	Expression UnaryExp();
	Expression BinaryExp(int iMinPriority);

	int iGetPriority(const string& sOperation);

	int i = 0; //Указатель на позицию строки
};
