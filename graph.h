#pragma once
#include "global.h"

struct FGlobal;

struct FGraph
{
	explicit FGraph(FGlobal* _ptrGlobal);

	void Init();

private:
	FGlobal* ptrGlobal; //Синглтон
};

struct Expression 
{
	explicit Expression(string _sToken) : sToken(_sToken) {}
	explicit Expression(string _sToken, Expression fArg) : sToken(_sToken), arrArgument{ fArg } {}
	explicit Expression(string _sToken, Expression fLeftArg, Expression fRightArg) : sToken(_sToken),
		arrArgument{ fLeftArg, fRightArg } {}

	double dSolve(const Expression& fExp);

private:
	string sToken;
	vector<Expression> arrArgument;
};

struct Parser 
{
	explicit Parser(string _sInput, double _dLeft, double _dRight, int _iAmountDisc, int _iPowerComp, double _dSumScore);
	
	Expression parse();

	string sParserToken();
	Expression UnaryExp();
	Expression BinaryExp(int iMinPriority);

private:
	string sInput;
	int i = 0;

	double dLeft = 0; // L
	double dRight = 0; // R
	double dSumScore = 0; // A
	int iAmountDisc = 0; // N
	int iPowerComp = 0; // K

};

