﻿#include "graph.h"

Parser::Parser(string _sInput, double _dLeft, double _dRight, int _iAmountDisc, int _iPowerComp, double _dSumScore) :
	sInput(_sInput), dLeft(_dLeft), dRight(_dRight), dSumScore(_dSumScore), iAmountDisc(_iAmountDisc), iPowerComp(_iPowerComp)
{
}

std::string Parser::sParserToken() {
	while (true)
	{
		if (sInput.size() <= i) break;
		if (!isspace(sInput[i])) break;
	}

	if (isdigit(sInput[i])) {
		string number;
		while (isdigit(sInput[i]) || sInput[i] == '.') // Вытаскиваем вещественное число
		{
			number.push_back(sInput[i++]);
		}
		return number;
	}

	vector< string > arrToken =
	{ "+", "-", "**", "*", "/", "mod", "abs", "sin", "cos", "(", ")", "L", "R", "A", "N", "K" };

	for (auto& s : arrToken) {
		bool bIsEqual = true;
		for (int j = 0; j < s.size(); ++j) // Проверка на равенство Токену
		{
			if ((i + j >= sInput.size()) || (sInput[i + j] != s[j]))
			{
				bIsEqual = false;
				break;
			}
		}
		if (bIsEqual)
		{
			i += s.size();
			return s;
		}
	}

	return "";
}

Expression Parser::UnaryExp()
{
	string token = sParserToken();
	if (token.empty()) throw std::runtime_error("Invalid input");

	if (token == "(") {
		auto result = parse();
		if (sParserToken() != ")") throw std::runtime_error("Expected ')'");
		return result;
	}

	if (std::isdigit(token[0]))
		return Expression(token);

	vector< string > arrToken =
	{ "L", "R", "A", "N", "K" };

	int i = 0;
	for (; i < arrToken.size(); ++i)
	{
		if (arrToken[i] == token) break;
	}

	switch (i)
	{
	case 0:
		return Expression(to_string(dLeft));
		break;
	case 1:
		return Expression(to_string(dRight));
		break;
	case 2:
		return Expression(to_string(dSumScore));
		break;
	case 3:
		return Expression(to_string(iAmountDisc));
		break;
	case 4:
		return Expression(to_string(iPowerComp));
		break;
	}



	return Expression(token, UnaryExp());
}

int iGetPriority(const string& sOperation)
{
	if (sOperation == "+") return 1;
	if (sOperation == "-") return 1;
	if (sOperation == "*") return 2;
	if (sOperation == "/") return 2;
	if (sOperation == "mod") return 2;
	if (sOperation == "**") return 3;
	return 0;
}

Expression Parser::BinaryExp(int iMinPriority)
{
	auto left_expr = UnaryExp();

	while (true)
	{
		string op = sParserToken();
		int priority = iGetPriority(op);
		if (priority <= iMinPriority)
		{
			i -= op.size();
			return left_expr;
		}

		auto right_expr = BinaryExp(priority);
		left_expr = Expression(op, left_expr, right_expr);
	}
}

Expression Parser::parse() {
	return BinaryExp(0);
}

double Expression::dSolve(const Expression& fExp)
{
	double dLeft = 0., dRight = 0.;
	switch (fExp.arrArgument.size())
	{
	case 0:
		return atof(fExp.sToken.c_str());

	case 1:
	{
		dLeft = dSolve(fExp.arrArgument[0]);
		if (fExp.sToken == "+") return +dLeft;
		if (fExp.sToken == "-") return -dLeft;
		if (fExp.sToken == "abs") return abs(dLeft);
		if (fExp.sToken == "sin") return sin(dLeft);
		if (fExp.sToken == "cos") return cos(dLeft);
		throw std::runtime_error("Unknown unary operator");
	}
	case 2:
	{
		dLeft = dSolve(fExp.arrArgument[0]);
		dRight = dSolve(fExp.arrArgument[1]);
		if (fExp.sToken == "+") return dLeft + dRight;
		if (fExp.sToken == "-") return dLeft - dRight;
		if (fExp.sToken == "*") return dLeft * dRight;
		if (fExp.sToken == "/") return dLeft / dRight;
		if (fExp.sToken == "**") return pow(dLeft, dRight);
		if (fExp.sToken == "mod") return (long long)(dLeft) % (long long)(dRight);
		throw std::runtime_error("Unknown binary operator");
	}

	throw std::runtime_error("Unknown expression type");
	}

	return 0.;
}