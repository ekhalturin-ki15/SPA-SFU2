// #include "pch.h"
#include "../adapOutData.h"
#include "../config.h"
#include "../error.h"
#include "../global.h"
#include "../graph.h"
#include "../outData.h"
#include "../solve.h"
#include "CppUnitTest.h"

#include "../formulaParser.h"

#include <filesystem>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace std;
// using namespace OpenXLSX;

bool Create(shared_ptr<FGlobal>& _ptrGlobal, const wstring& _wsNameConfig)
{
    _ptrGlobal = make_shared<FGlobal>(_wsNameConfig);
    if (!_ptrGlobal->Init(_ptrGlobal))
    {
        _ptrGlobal.reset();
        return false;
    }
    return true;
}


bool isSameExcel();

namespace TestFSolve
{
    const wstring wsNameConfig = L"test_config.xlsx";

    TEST_CLASS(TestFSolveOnly)
    {
        shared_ptr<FGlobal> ptrGlobal;

    public:
        TestFSolveOnly()
        {
            if (!Create(ptrGlobal, wsNameConfig))
            {
                Assert::Fail(L"�� ������� ���������������� ������ ������");
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
                        if (ptrGlobal->ptrConfig->setIgnore�urriculum.count(
                                it.path().filename().string()))
                            continue;

                        ptrGlobal->ptrSolve->Read(
                            it.path().string(),
                            it.path().filename().string());
                    }
                }
            }
        }

        TEST_METHOD(IsCorrectParsing)
        {
            Assert::IsTrue(ptrGlobal->ptrSolve->bIsCorrectParsing);
        }

        TEST_METHOD(AllFindComp)
        {
            // 6 ���� = �� �� ��� ��� ���� ���
            Assert::AreEqual(6ull, ptrGlobal->ptrSolve->setHeaderComp.size());
        }

        TEST_METHOD(MaxCourse)
        {
            Assert::AreEqual(4, ptrGlobal->ptrSolve->iMaxCourse);
        }
    };

    //! �� �������� ������� ���� config.xlsx � ���� �������� �����
    //! �� �������� ������� ���� config.xlsx � ���� �������� �����
    //! �� �������� ������� ���� config.xlsx � ���� �������� �����
    TEST_CLASS(TestFCurricula)
    {
        shared_ptr<FGlobal>    ptrGlobal;
        shared_ptr<FCurricula> ptrFirst;

    public:
        TestFCurricula()
        {
            if (!Create(ptrGlobal, wsNameConfig))
            {
                Assert::Fail(L"�� ������� ���������������� ������ ������");
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
                        if (ptrGlobal->ptrConfig->setIgnore�urriculum.count(
                                it.path().filename().string()))
                            continue;

                        ptrGlobal->ptrSolve->Read(
                            it.path().string(),
                            it.path().filename().string());
                    }
                }
            }
            // ptrGlobal->ptrSolve->CreateAllGraph();
            // ptrGlobal->ptrSolve
            //     ->CreateAllMetric();    // ������� �� ������� � ������

            ptrFirst = ptrGlobal->ptrSolve->arrDisc.front();
        }

        TEST_METHOD(ItsNotEmpty)
        {
            try
            {
                Assert::AreEqual(3ull, ptrGlobal->ptrSolve->arrDisc.size());
            }
            catch (const std::exception& ex)
            {
                string str = ex.what();
                Assert::Fail(wstring(ALL(str)).c_str());
            }
        }

        TEST_METHOD(CheckTypeDisc)
        {
            string sActual;
            int    i = -1;
            for (const auto& [key, val] : ptrFirst->mapAmountTypeDisc)
            {
                ++i;
                if (i)
                    sActual += ' ';
                sActual += to_string(key);
                sActual += '=';
                sActual += to_string(val);
            }

            Assert::AreEqual(string("0=6 1=2 2=2"), sActual);
        }

        TEST_METHOD(CheckTagDisc)
        {
            string sActual;
            int    i = -1;
            for (const auto& [key, val] : ptrFirst->mapAmountTagDisc)
            {
                ++i;
                if (i)
                    sActual += ' ';
                sActual += to_string(key);
                sActual += '=';
                sActual += to_string(val);
            }

            Assert::AreEqual(string("0=4 1=5 2=4 3=1"), sActual);
        }

        TEST_METHOD(AllSumCredits)
        {
            Assert::AreEqual(25., ptrFirst->dAllSumScore);
        }

        TEST_METHOD(CountAllowDisc)
        {
            Assert::AreEqual(7, ptrFirst->iAmountDisc);
        }

        TEST_METHOD(CountDiscWhithNotAllow)
        {
            Assert::AreEqual(10, ptrFirst->iExtendedAmountDisc);
        }

        TEST_METHOD(AllCompIsFind)
        {
            // 12 ���� = ��-1 ��-2 ��-3 ��-1 ��-2 ���-1 ���-2 ���-3 ���-4 ���-1
            // ����-1 ���-1
            Assert::AreEqual(12ull, ptrFirst->fAllComp.size());
        }

        TEST_METHOD(Year)
        {
            Assert::AreEqual(2030, ptrFirst->iYearStart);
        }

        TEST_METHOD(UGNS)
        {
            Assert::AreEqual(77, ptrFirst->iCodeUGSN);
        }
    };

    TEST_CLASS(TestFGraph)
    {
        shared_ptr<FGlobal>    ptrGlobal;
        shared_ptr<FCurricula> ptrFirst;
        shared_ptr<FCurricula> ptrSecond;
        shared_ptr<FCurricula> ptrThird;
        int                    iCommon;

    public:
        TestFGraph()
        {
            if (!Create(ptrGlobal, wsNameConfig))
            {
                Assert::Fail(L"�� ������� ���������������� ������ ������");
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
                        if (ptrGlobal->ptrConfig->setIgnore�urriculum.count(
                                it.path().filename().string()))
                            continue;

                        ptrGlobal->ptrSolve->Read(
                            it.path().string(),
                            it.path().filename().string());
                    }
                }
            }
            ptrGlobal->ptrSolve->CreateAllGraph();
            ptrFirst  = ptrGlobal->ptrSolve->arrDisc.front();
            ptrSecond = ptrGlobal->ptrSolve->arrDisc.at(1);
            ptrThird = ptrGlobal->ptrSolve->arrDisc.at(2);
            iCommon   = ptrFirst->ptrGraph->iCommon;
        }

        TEST_METHOD(InitGraph)
        {
            Assert::IsTrue(ptrFirst->ptrGraph != nullptr);
        }

        TEST_METHOD(InitAllTypeGraph)
        {
            Assert::AreEqual(4ull + 3ull, ptrFirst->ptrGraph->mapGraph.size());
        }

         TEST_METHOD(FirstAmountDisc1Course)
        {
            // �� ����������� ���������� "���������������
            // (��������-���������������) ��������" ��� ��� ��� � �����������
            // ����� config.xlsx "������������ ��������"
            Assert::AreEqual(
                2, ptrThird->ptrGraph->mapGraph[0].iGraphAmountDisc);
        }

        TEST_METHOD(ThirdAmountDiscCommon)
        {
            // �� ����������� ���������� "��������������� (��������-���������������) ��������"
            // ��� ��� ��� � ����������� ����� config.xlsx "������������ ��������"
            Assert::AreEqual(
                9, ptrThird->ptrGraph->mapGraph[iCommon].iGraphAmountDisc);
        }

        TEST_METHOD(FirstDenseCommon)
        {
            // ��������� � Gephi
            Assert::AreEqual(0.238,
                             ptrFirst->ptrGraph->mapGraph[iCommon].dDense, 0.01);
        }

        TEST_METHOD(SecondDenseCommon)
        {
            // ������������ ����
            Assert::AreEqual(1., ptrSecond->ptrGraph->mapGraph[iCommon].dDense);
        }

        TEST_METHOD(ThirdDenseCommon)
        {
            // ��������� � Gephi
            Assert::AreEqual(
                0.222, ptrThird->ptrGraph->mapGraph[iCommon].dDense, 0.01);
        }

       


        TEST_METHOD(FirstDiametrStepCommon)
        {
            // 3 ������������ ����
            Assert::AreEqual(
                2., ptrFirst->ptrGraph->mapGraph[iCommon].dDiametrStep);
        }

        TEST_METHOD(ThirdDiametrStepCommon)
        {
            // ������������ ����
            Assert::AreEqual(
                2., ptrThird->ptrGraph->mapGraph[iCommon].dDiametrStep);
        }

        TEST_METHOD(FirstComponentCommon)
        {
            Assert::AreEqual(3,
                             ptrFirst->ptrGraph->mapGraph[iCommon].iComponent);
        }

        TEST_METHOD(ThirdComponentCommon)
        {
            Assert::AreEqual(
                4, ptrThird->ptrGraph->mapGraph[iCommon].iComponent);
        }
    };

}    // namespace TestFSolve

namespace AnomalTestFSolve
{
    const wstring wsNameConfigAnomal = L"anomal_test_config.xlsx";

    TEST_CLASS(TestFSolveOnly)
    {
        shared_ptr<FGlobal> ptrGlobal;

    public:
        TestFSolveOnly()
        {
            if (!Create(ptrGlobal, wsNameConfigAnomal))
            {
                Assert::Fail(L"�� ������� ���������������� ������ ������");
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
                        if (ptrGlobal->ptrConfig->setIgnore�urriculum.count(
                                it.path().filename().string()))
                            continue;

                        ptrGlobal->ptrSolve->Read(
                            it.path().string(),
                            it.path().filename().string());
                    }
                }
            }
        }

        TEST_METHOD(IsCorrectParsing)
        {
            Assert::IsTrue(ptrGlobal->ptrSolve->bIsCorrectParsing);
        }

        TEST_METHOD(AllFindComp)
        {
            // 2 ���� = �� ��
            Assert::AreEqual(2ull, ptrGlobal->ptrSolve->setHeaderComp.size());
        }

        TEST_METHOD(MaxCourse)
        {
            Assert::AreEqual(4, ptrGlobal->ptrSolve->iMaxCourse);
        }
    };

    //! �� �������� ������� ���� config.xlsx � ���� �������� �����
    //! �� �������� ������� ���� config.xlsx � ���� �������� �����
    //! �� �������� ������� ���� config.xlsx � ���� �������� �����
    TEST_CLASS(TestFCurricula)
    {
        shared_ptr<FGlobal>    ptrGlobal;
        shared_ptr<FCurricula> ptrFirst;

    public:
        TestFCurricula()
        {
            if (!Create(ptrGlobal, wsNameConfigAnomal))
            {
                Assert::Fail(L"�� ������� ���������������� ������ ������");
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
                        if (ptrGlobal->ptrConfig->setIgnore�urriculum.count(
                                it.path().filename().string()))
                            continue;

                        ptrGlobal->ptrSolve->Read(
                            it.path().string(),
                            it.path().filename().string());
                    }
                }
            }
            // ptrGlobal->ptrSolve->CreateAllGraph();
            // ptrGlobal->ptrSolve
            //     ->CreateAllMetric();    // ������� �� ������� � ������

            ptrFirst = ptrGlobal->ptrSolve->arrDisc.front();
        }

        TEST_METHOD(ItsNotEmpty)
        {
            try
            {
                Assert::AreEqual(2ull, ptrGlobal->ptrSolve->arrDisc.size());
            }
            catch (const std::exception& ex)
            {
                string str = ex.what();
                Assert::Fail(wstring(ALL(str)).c_str());
            }
        }

        TEST_METHOD(CheckTypeDisc)
        {
            string sActual;
            int    i = -1;
            for (const auto& [key, val] : ptrFirst->mapAmountTypeDisc)
            {
                ++i;
                if (i)
                    sActual += ' ';
                sActual += to_string(key);
                sActual += '=';
                sActual += to_string(val);
            }

            Assert::AreEqual(string("0=4"), sActual);
        }

        TEST_METHOD(CheckTagDisc)
        {
            string sActual;
            int    i = -1;
            for (const auto& [key, val] : ptrFirst->mapAmountTagDisc)
            {
                ++i;
                if (i)
                    sActual += ' ';
                sActual += to_string(key);
                sActual += '=';
                sActual += to_string(val);
            }

            Assert::AreEqual(string("1=1 2=1 3=2"), sActual);
        }

        TEST_METHOD(AllSumCredits)
        {
            Assert::AreEqual(10., ptrFirst->dAllSumScore);
        }

        TEST_METHOD(CountAllowDisc)
        {
            Assert::AreEqual(3, ptrFirst->iAmountDisc);
        }

        TEST_METHOD(CountDiscWhithNotAllow)
        {
            Assert::AreEqual(4, ptrFirst->iExtendedAmountDisc);
        }

        TEST_METHOD(AllCompIsFind)
        {
            // 12 ���� = ��-1 ��-2 ��-3 
            // ����������� ��-3.1 ��-3.2 ��������������� � ����� ��������� ��� ��-3
            // ��� �������� ���������, ��� ��� � ��������� �� �� �������� �����������(2) ����������� ����������
            Assert::AreEqual(3ull, ptrFirst->fAllComp.size());
        }

        TEST_METHOD(Year)
        {
            Assert::AreEqual(2025, ptrFirst->iYearStart);
        }

        TEST_METHOD(UGNS)
        {
            Assert::AreEqual(44, ptrFirst->iCodeUGSN);
        }
    };

    TEST_CLASS(TestFGraph)
    {
        shared_ptr<FGlobal>    ptrGlobal;
        shared_ptr<FCurricula> ptrFirst;

    public:
        TestFGraph()
        {
            if (!Create(ptrGlobal, wsNameConfigAnomal))
            {
                Assert::Fail(L"�� ������� ���������������� ������ ������");
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
                        if (ptrGlobal->ptrConfig->setIgnore�urriculum.count(
                                it.path().filename().string()))
                            continue;

                        ptrGlobal->ptrSolve->Read(
                            it.path().string(),
                            it.path().filename().string());
                    }
                }
            }
            ptrGlobal->ptrSolve->CreateAllGraph();
            ptrFirst = ptrGlobal->ptrSolve->arrDisc.front();
        }

        TEST_METHOD(InitGraph)
        {
            Assert::IsTrue(ptrFirst->ptrGraph != nullptr);
        }
    };

}    // namespace AnomalTestFSolve

namespace TestFormulaParser
{
    TEST_CLASS(TestFormulaParser)
    {
    public:
        TestFormulaParser() : res1(0.), res2(0.)
        {
            // �������, �����
            // A - ��������� ���-�� ��, �����
            // N - ��������� ���-�� ���������
            // K - �������� ����������� �� ������������
            // D - �������� ����������� �� �����������

            fTestFormula.push_back(FormulaParser("((L + R) / 2) * K", 244.,
                                                 17));    // A = 244 N = 17

            fTestFormula.push_back(FormulaParser("(L / 2 + R / 2) * K",
                                                 244.,
                                                 17));    // A = 244 N = 17
            fTestFormula.push_back(FormulaParser("((L + R) / (2 * A)) * N",
                                                 245.,
                                                 49));    // A = 245 N = 49
            fTestFormula.push_back(FormulaParser("((L * R) ^ (1 / 2)) * K",
                                                 245.,
                                                 49));    // A = 245 N = 49
            fTestFormula.push_back(
                FormulaParser("abs(L)", 245., 49));    // A = 245 N = 49
            fTestFormula.push_back(
                FormulaParser("R mod N", 245., 3));    // A = 245 N = 3
            fTestFormula.push_back(
                FormulaParser("log(R)", 245., 3));    // A = 245 N = 3

            fTestFormula.push_back(
                FormulaParser("((L + R) / 2) * D", 244., 17));
        }

        vector<FormulaParser> fTestFormula;

        double res1, res2;

        TEST_METHOD(Test1)
        {
            // ������ ����
            // TakeResult ��������� L R K � D
            //  ((L + R) / 2) * K
            //  L = 5
            //  R = 11
            //  K = 3
            res1 = fTestFormula[0].TakeResult(5., 11., 3, 0);
            Assert::AreEqual(((5. + 11.) / 2) * 3, res1);
        }

        TEST_METHOD(Test2)
        {
            // ������ ����
            // (L / 2 + R / 2) * K
            //  L = 6
            //  R = 12.5
            //  K = 11
            res2 = fTestFormula[1].TakeResult(6., 12.5, 11, 0);
            Assert::AreEqual((6. / 2 + 12.5 / 2) * 11, res2);
        }

        TEST_METHOD(Test2Add)
        {
            // ������ ���� +
            res1 = fTestFormula[0].TakeResult(6., 12.5, 11, 0);
            res2 = fTestFormula[1].TakeResult(6., 12.5, 11, 0);
            Assert::AreEqual(res1, res2);
        }

        TEST_METHOD(Test3)
        {
            // ������ ����
            res1 = fTestFormula[2].TakeResult(6., 7., 2, 0);
            Assert::AreEqual(1.3, res1);
        }

        TEST_METHOD(Test4)
        {
            res1 = fTestFormula[3].TakeResult(5., 5., 6, 0);
            Assert::AreEqual(30., res1);
        }

        TEST_METHOD(Test5)
        {
            res1 = fTestFormula[4].TakeResult(-5., 5., 6, 0);
            Assert::AreEqual(5., res1);
        }

        TEST_METHOD(Test6)
        {
            res1 = fTestFormula[5].TakeResult(-5., 50., 6, 0);
            Assert::AreEqual(2., res1);
        }

        TEST_METHOD(Test7)
        {
            res1 = fTestFormula[6].TakeResult(-5., 50., 6, 0);
            Assert::AreNotEqual(
                0.,
                res1);    // �������� �� ��, ��� �� ������ �����������
        }

        TEST_METHOD(Test8)
        {
            // ������ ����
            // TakeResult ��������� L R K � D
            //  ((L + R) / 2) * D
            //  L = 5
            //  R = 11
            //  D = 3
            res1 = fTestFormula[7].TakeResult(5., 11., 0, 3);
            Assert::AreEqual(((5. + 11.) / 2) * 3, res1);
        }
    };
}    // namespace TestFormulaParser
