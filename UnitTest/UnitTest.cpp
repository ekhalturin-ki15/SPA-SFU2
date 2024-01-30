// #include "pch.h"
#include "../adapOutData.h"
#include "../config.h"
#include "../error.h"
#include "../global.h"
#include "../outData.h"
#include "../solve.h"
#include "CppUnitTest.h"

#include "../formulaParser.h"

#include <filesystem>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace std;
// using namespace OpenXLSX;

namespace TestFSolve
{
    bool Create(shared_ptr<FGlobal>& _ptrGlobal)
    {
        _ptrGlobal = make_shared<FGlobal>();
        if (!_ptrGlobal->Init(_ptrGlobal))
        {
            _ptrGlobal.reset();
            return false;
        }
        return true;
    }

    TEST_CLASS(SolveTestValue)
    {
        shared_ptr<FGlobal> ptrGlobal;
        shared_ptr<FTreeDisc> ptrBack;

    public:
        SolveTestValue()
        {
            if (!Create(ptrGlobal))
            {
                Assert::Fail(L"Не удалось инициплизировать объект Глобал");
                return;
            }

            auto fFile = ptrGlobal->GetCurrentPath();   
            for (int category = 0;
                 category < ptrGlobal->ptrConfig->GetArrNameFileIn().size();
                 ++category)
            {
                auto fInFile =
                    fFile / ptrGlobal->ptrConfig->GetArrNameFileIn()[category];

                for (auto it : filesystem::directory_iterator(fInFile))
                {
                    if (!it.is_directory())
                    {
                        if (ptrGlobal->ptrConfig->setIgnoreСurriculum.count(
                                it.path().filename().string()))
                            continue;

                        ptrGlobal->ptrSolve->Read(
                            it.path().string(),
                            it.path().filename().string());
                    }
                }
            }

            ptrGlobal->ptrSolve->CreateAllGraph();
            ptrGlobal->ptrSolve
                ->CreateAllMetric();    // Метрики не связаны с графом

            ptrBack = ptrGlobal->ptrSolve->arrDisc.back();
        }

        TEST_METHOD(ItsNotEmpty)
        {
            try
            {
                Assert::AreEqual(1ull,
                                 ptrGlobal->ptrSolve->arrDisc.size());
            }
            catch (const std::exception& ex)
            {
                string str = ex.what();
                Assert::Fail(wstring(ALL(str)).c_str());
            }
        }
        TEST_METHOD(AllSumCredits) 
        { 
            Assert::AreEqual(25., ptrBack->dAllSumScore);
        }

        TEST_METHOD(CountAllowDisc)
        {
            Assert::AreEqual(7, ptrBack->iAmountDisc);
        }

        TEST_METHOD(CountDiscWhithNotAllow)
        {
            Assert::AreEqual(10, ptrBack->iExtendedAmountDisc);
        }

        TEST_METHOD(AllCompIsFind)
        { 
            Assert::AreEqual(12ull, ptrBack->fAllComp.size());
        }
    };
}

namespace TestFormulaParser
{
    TEST_CLASS(TestFormulaParser)
    {
    public:
        TestFormulaParser() : res1(0.), res2(0.)
        {
            fTestFormula.push_back(FormulaParser("((L + R) / 2) * K", 244., 17));
            fTestFormula.push_back(FormulaParser("(L / 2 + R / 2) * K", 244., 17));
            fTestFormula.push_back(FormulaParser("((L + R) / (2 * A)) * N", 245., 49));
            fTestFormula.push_back(FormulaParser("((L * R) ^ (1 / 2)) * K", 245., 49));
            fTestFormula.push_back(FormulaParser("abs(L)", 245., 49));
            fTestFormula.push_back(FormulaParser("R mod N", 245., 3));
            fTestFormula.push_back(FormulaParser("log(R)", 245., 3));
        }

        vector<FormulaParser> fTestFormula;

        double res1, res2;

        TEST_METHOD(Test1)
        {
            // Первый тест
            res1 = fTestFormula[0].TakeResult(5., 11., 3);
            Assert::AreEqual(24., res1);
        }

        TEST_METHOD(Test2)
        {
            // Второй тест
            res2 = fTestFormula[1].TakeResult(6., 12.5, 11);
            Assert::AreEqual(101.75, res2);
        }

        TEST_METHOD(Test2Add)
        {
            // Второй тест +
            res1 = fTestFormula[0].TakeResult(6., 12.5, 11);
            res2 = fTestFormula[1].TakeResult(6., 12.5, 11);
            Assert::AreEqual(res1, res2);
        }

        TEST_METHOD(Test3)
        {
            // Третий тест
            res1 = fTestFormula[2].TakeResult(6., 7., 2);
            Assert::AreEqual(1.3, res1);
        }

        TEST_METHOD(Test4)
        {
            res1 = fTestFormula[3].TakeResult(5., 5., 6);
            Assert::AreEqual(30., res1);
        }

        TEST_METHOD(Test5)
        {
            res1 = fTestFormula[4].TakeResult(-5., 5., 6);
            Assert::AreEqual(5., res1);
        }

        TEST_METHOD(Test6)
        {
            res1 = fTestFormula[5].TakeResult(-5., 50., 6);
            Assert::AreEqual(2., res1);
        }

        TEST_METHOD(Test7)
        {
            res1 = fTestFormula[6].TakeResult(-5., 50., 6);
            Assert::AreNotEqual(
                0.,
                res1);    // Проверка на то, что всё удачно выполнилось
        }
    };
}    // namespace TestFormulaParser
