#include "formulaParser.h"

FormulaParser::FormulaParser(string _sInput, double _dSumScore, int _iAmountDisc) 
	: sInput(_sInput), dSumScore(_dSumScore), iAmountDisc(_iAmountDisc)
{
}

std::string FormulaParser::ParserToken() {
	while (true)
	{
		if (sInput.size() <= i) break;
		if (!isspace(sInput[i])) break;
		++i;
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
	{ "+", "-", "^", "*", "/", "mod", "abs", "sin", "cos", "log", "(", ")", "L", "R", "A", "N", "K" };

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

Expression FormulaParser::UnaryExp()
{
	string token = ParserToken();
	if (token.empty()) throw std::runtime_error("Invalid input");

	if (token == "(") {
		auto result = Parse();
		if (ParserToken() != ")") throw std::runtime_error("Expected ')'");
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

int FormulaParser::iGetPriority(const string& sOperation)
{
	if (sOperation == "+") return 1;
	if (sOperation == "-") return 1;
	if (sOperation == "*") return 2;
	if (sOperation == "/") return 2;
	if (sOperation == "mod") return 2;
	if (sOperation == "^") return 3;
	return 0;
}

Expression FormulaParser::BinaryExp(int iMinPriority)
{
	auto left_expr = UnaryExp();

	while (true)
	{
		string op = ParserToken();
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

Expression FormulaParser::Parse() {
	return BinaryExp(0);
}

double Expression::Calculate(const Expression& fExp)
{
	double dLeft = 0., dRight = 0.;
	switch (fExp.arrArgument.size())
	{
	case 0:
		return atof(fExp.sToken.c_str());

	case 1:
	{
		dLeft = Calculate(fExp.arrArgument[0]);
		if (fExp.sToken == "+") return +dLeft;
		if (fExp.sToken == "-") return -dLeft;
		if (fExp.sToken == "abs") return abs(dLeft);
		if (fExp.sToken == "sin") return sin(dLeft);
		if (fExp.sToken == "log") return log(dLeft);
		if (fExp.sToken == "cos") return cos(dLeft);
		throw std::runtime_error("Unknown unary operator");
	}
	case 2:
	{
		dLeft = Calculate(fExp.arrArgument[0]);
		dRight = Calculate(fExp.arrArgument[1]);
		if (fExp.sToken == "+") return dLeft + dRight;
		if (fExp.sToken == "-") return dLeft - dRight;
		if (fExp.sToken == "*") return dLeft * dRight;
		if (fExp.sToken == "/") return dLeft / dRight;
		if (fExp.sToken == "^") return pow(dLeft, dRight);
		if (fExp.sToken == "mod") return (long long)(dLeft) % (long long)(dRight);
		throw std::runtime_error("Unknown binary operator");
	}

	throw std::runtime_error("Unknown expression type");
	}

	return 0.;
}

double FormulaParser::TakeResult(double _dLeft, double _dRight, int _iPowerComp)
{
	//sInput = _sInput;
	i = 0;
	dLeft = _dLeft;
	dRight = _dRight;
	//dSumScore = _dSumScore;
	//iAmountDisc = _iAmountDisc;
	iPowerComp = _iPowerComp;

	Expression ex = Parse();
	return ex.Calculate(ex);
}
