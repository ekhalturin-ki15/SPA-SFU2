#pragma once
#include "global.h"

struct FGlobal;

struct FGraph
{
	FGraph(FGlobal* _ptrGlobal);

	void Init();

	FGlobal* ptrGlobal; //Синглтон

};

struct Expression 
{
	Expression(string _sToken) : sToken(_sToken) {}
	Expression(string _sToken, Expression fArg) : sToken(_sToken), arrArgument{ fArg } {}
	Expression(string _sToken, Expression fLeftArg, Expression fRightArg) : sToken(_sToken), 
		arrArgument{ fLeftArg, fRightArg } {}

	double dSolve(const Expression& fExp);

	string sToken;
	vector<Expression> arrArgument;
};

struct Parser 
{
	Parser(string _sInput, double _dLeft, double _dRight, int _iAmountDisc, int _iPowerComp, double _dSumScore) :
		sInput(_sInput) , dLeft(_dLeft), dRight(_dRight), dSumScore(_dSumScore), iAmountDisc(_iAmountDisc), iPowerComp(_iPowerComp)
		{}
	
	Expression parse();

	string sParserToken();
	Expression UnaryExp();
	Expression BinaryExp(int iMinPriority);

	string sInput;
	int i = 0;

	double dLeft = 0; // L
	double dRight = 0; // R
	double dSumScore = 0; // A
	int iAmountDisc = 0; // N
	int iPowerComp = 0; // K

};

