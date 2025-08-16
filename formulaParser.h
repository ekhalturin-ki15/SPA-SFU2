#pragma once
#include <array>
#include <cctype>
#include <cmath>
#include <stdexcept>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

enum class TokenType
{
    Num,
    Var_L,
    Var_R,
    Var_A,
    Var_N,
    Var_K,
    Var_D,
    Add,
    Sub,
    Mul,
    Div,
    Mod,
    Pow,
    Abs,
    Sin,
    Cos,
    Log,
    LParen,
    RParen
};

struct Instr
{
    TokenType type;
    double    value = 0.0;    // только для чисел
};

class FormulaParser
{
public:
    FormulaParser(std::string sInput, int _A, int _N)
        : input(std::move(sInput)), A(_A), N(_N)
    {
        tokenize();
        toPostfix();
    }

    double TakeResult(double L,
                      double R,
                      double K,
                      double D)
        const
    {
        std::vector<double> stack;
        stack.reserve(postfix.size());
        for (auto& ins : postfix)
        {
            switch (ins.type)
            {
                case TokenType::Num:
                    stack.push_back(ins.value);
                    break;
                case TokenType::Var_L:
                    stack.push_back(L);
                    break;
                case TokenType::Var_R:
                    stack.push_back(R);
                    break;
                case TokenType::Var_A:
                    stack.push_back(A);
                    break;
                case TokenType::Var_N:
                    stack.push_back(N);
                    break;
                case TokenType::Var_K:
                    stack.push_back(K);
                    break;
                case TokenType::Var_D:
                    stack.push_back(D);
                    break;

                case TokenType::Add:
                    binOp(stack,
                          [](double a, double b)
                          {
                              return a + b;
                          });
                    break;
                case TokenType::Sub:
                    binOp(stack,
                          [](double a, double b)
                          {
                              return a - b;
                          });
                    break;
                case TokenType::Mul:
                    binOp(stack,
                          [](double a, double b)
                          {
                              return a * b;
                          });
                    break;
                case TokenType::Div:
                    binOp(stack,
                          [](double a, double b)
                          {
                              return a / b;
                          });
                    break;
                case TokenType::Pow:
                    binOp(stack,
                          [](double a, double b)
                          {
                              return std::pow(a, b);
                          });
                    break;
                case TokenType::Mod:
                    binOp(stack,
                          [](double a, double b)
                          {
                              return (long long)a % (long long)b;
                          });
                    break;

                case TokenType::Abs:
                    unOp(stack,
                         [](double a)
                         {
                             return std::abs(a);
                         });
                    break;
                case TokenType::Sin:
                    unOp(stack,
                         [](double a)
                         {
                             return std::sin(a);
                         });
                    break;
                case TokenType::Cos:
                    unOp(stack,
                         [](double a)
                         {
                             return std::cos(a);
                         });
                    break;
                case TokenType::Log:
                    unOp(stack,
                         [](double a)
                         {
                             return std::log(a);
                         });
                    break;

                default:
                    throw std::runtime_error("Unexpected token in execution");
            }
        }
        if (stack.empty())
            throw std::runtime_error("Empty expression");
        return stack.back();
    }

private:
    std::string        input;
    int        A;
    int        N;
    std::vector<Instr> tokens;
    std::vector<Instr> postfix;

    static int priority(TokenType t)
    {
        switch (t)
        {
            case TokenType::Add:
            case TokenType::Sub:
                return 1;
            case TokenType::Mul:
            case TokenType::Div:
            case TokenType::Mod:
                return 2;
            case TokenType::Pow:
                return 3;
            default:
                return 0;
        }
    }

    template<typename F>
    static void binOp(std::vector<double>& st, F func)
    {
        double b = st.back();
        st.pop_back();
        double a = st.back();
        st.pop_back();
        st.push_back(func(a, b));
    }

    template<typename F>
    static void unOp(std::vector<double>& st, F func)
    {
        double a = st.back();
        st.pop_back();
        st.push_back(func(a));
    }

    void tokenize()
    {
        size_t i = 0;
        while (i < input.size())
        {
            if (std::isspace((unsigned char)input[i]))
            {
                ++i;
                continue;
            }

            if (std::isdigit((unsigned char)input[i]) || input[i] == '.')
            {
                size_t start = i;
                while (
                    i < input.size() &&
                    (std::isdigit((unsigned char)input[i]) || input[i] == '.'))
                    ++i;
                tokens.push_back({ TokenType::Num,
                                   std::stod(input.substr(start, i - start)) });
                continue;
            }

            static const std::unordered_map<std::string, TokenType> keywords = {
                { "+", TokenType::Add },    { "-", TokenType::Sub },
                { "*", TokenType::Mul },    { "/", TokenType::Div },
                { "^", TokenType::Pow },    { "mod", TokenType::Mod },
                { "abs", TokenType::Abs },  { "sin", TokenType::Sin },
                { "cos", TokenType::Cos },  { "log", TokenType::Log },
                { "L", TokenType::Var_L },  { "R", TokenType::Var_R },
                { "A", TokenType::Var_A },  { "N", TokenType::Var_N },
                { "K", TokenType::Var_K },  { "D", TokenType::Var_D },
                { "(", TokenType::LParen }, { ")", TokenType::RParen }
            };

            bool matched = false;
            for (auto& [kw, t] : keywords)
            {
                if (input.compare(i, kw.size(), kw) == 0)
                {
                    tokens.push_back({ t, 0.0 });
                    i += kw.size();
                    matched = true;
                    break;
                }
            }
            if (!matched)
                throw std::runtime_error("Unknown token at pos " +
                                         std::to_string(i));
        }
    }

    void toPostfix()
    {
        std::vector<Instr> opStack;
        for (auto& tok : tokens)
        {
            if (tok.type == TokenType::Num || tok.type == TokenType::Var_L ||
                tok.type == TokenType::Var_R || tok.type == TokenType::Var_A ||
                tok.type == TokenType::Var_N || tok.type == TokenType::Var_K ||
                tok.type == TokenType::Var_D)
            {
                postfix.push_back(tok);
            }
            else if (tok.type == TokenType::Abs || tok.type == TokenType::Sin ||
                     tok.type == TokenType::Cos || tok.type == TokenType::Log)
            {
                opStack.push_back(tok);
            }
            else if (tok.type == TokenType::Add || tok.type == TokenType::Sub ||
                     tok.type == TokenType::Mul || tok.type == TokenType::Div ||
                     tok.type == TokenType::Mod || tok.type == TokenType::Pow)
            {
                while (!opStack.empty() &&
                       priority(opStack.back().type) >= priority(tok.type))
                {
                    postfix.push_back(opStack.back());
                    opStack.pop_back();
                }
                opStack.push_back(tok);
            }
            else if (tok.type == TokenType::LParen)
            {
                opStack.push_back(tok);
            }
            else if (tok.type == TokenType::RParen)
            {
                while (!opStack.empty() &&
                       opStack.back().type != TokenType::LParen)
                {
                    postfix.push_back(opStack.back());
                    opStack.pop_back();
                }
                if (opStack.empty())
                    throw std::runtime_error("Bad formula");
                opStack.pop_back();
                if (!opStack.empty() &&
                    (opStack.back().type == TokenType::Abs ||
                     opStack.back().type == TokenType::Sin ||
                     opStack.back().type == TokenType::Cos ||
                     opStack.back().type == TokenType::Log))
                {
                    postfix.push_back(opStack.back());
                    opStack.pop_back();
                }
            }
        }
        while (!opStack.empty())
        {
            if (opStack.back().type == TokenType::LParen)
                throw std::runtime_error("Bad formula");
            postfix.push_back(opStack.back());
            opStack.pop_back();
        }
    }
};
