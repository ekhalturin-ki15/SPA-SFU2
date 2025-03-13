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

#define CONF_FILE1 L"1_test_config.xlsx"
#define CONF_FILE2 L"2_anomal_test_config.xlsx"
#define CONF_FILE3 L"3_real.xlsx"
#define CONF_FILE4 L"4_real.xlsx"

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

// bool isSameExcel();

namespace TestFSolve
{
    const wstring wsNameConfig = CONF_FILE1;

    TEST_CLASS(TestFSolveOnly)
    {
        shared_ptr<FGlobal> ptrGlobal;

    public:
        TestFSolveOnly()
        {
            if (!Create(ptrGlobal, wsNameConfig))
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

                        ptrGlobal->ptrSolve->Create(
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
            // 6 Штук = УК ПУ ОПК ДПК ДОПК СПК
            Assert::AreEqual(6ull, ptrGlobal->ptrSolve->setHeaderComp.size());
        }

        TEST_METHOD(MaxCourse)
        {
            Assert::AreEqual(4, ptrGlobal->ptrSolve->iMaxCourse);
        }
    };

    //! Не забудьте закрыть файл config.xlsx и файл учебного плана
    //! Не забудьте закрыть файл config.xlsx и файл учебного плана
    //! Не забудьте закрыть файл config.xlsx и файл учебного плана
    TEST_CLASS(TestFCurricula)
    {
        shared_ptr<FGlobal>    ptrGlobal;
        shared_ptr<FCurricula> ptrFirst;

    public:
        TestFCurricula()
        {
            if (!Create(ptrGlobal, wsNameConfig))
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

                        ptrGlobal->ptrSolve->Create(
                            it.path().string(),
                            it.path().filename().string());
                    }
                }
            }
            // ptrGlobal->ptrSolve->CreateAllGraph();
            // ptrGlobal->ptrSolve
            //     ->CreateAllMetric();    // Метрики не связаны с графом

            ptrFirst = ptrGlobal->ptrSolve->GetCurricula(0);
        }

        TEST_METHOD(ItsNotEmpty)
        {
            try
            {
                Assert::AreEqual(6, ptrGlobal->ptrSolve->N);
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
            for (const auto& [key, val] :
                 ptrFirst->mapETMTypeDisc[ETM_Extended])
            {
                ++i;
                if (i)
                    sActual += ' ';
                sActual += to_string(key);
                sActual += '=';
                sActual += to_string(val.iAmount);
            }

            // Факультативы Основные По выбору
            Assert::AreEqual(string("0=10 1=6 2=2 3=2"), sActual);
        }

        TEST_METHOD(CheckTagDisc)
        {
            string sActual;
            int    i = -1;
            for (const auto& [key, val] : ptrFirst->mapETMTagDisc[ETM_Extended])
            {
                ++i;
                if (i)
                    sActual += ' ';
                sActual += to_string(key);
                sActual += '=';
                sActual += to_string(val.iAmount);
            }

            Assert::AreEqual(string("0=4 1=5 2=4 3=1"), sActual);
        }

        TEST_METHOD(AllSumCredits)
        {
            Assert::AreEqual(
                25.,
                ptrFirst->mapETMTypeDisc[ETM_NoExtended][ETypeDisc::ETD_Total]
                    .dCredits);
        }

        TEST_METHOD(CountAllowDisc)
        {
            Assert::AreEqual(
                7,
                ptrFirst->mapETMTypeDisc[ETM_NoExtended][ETypeDisc::ETD_Total]
                    .iAmount);
        }

        TEST_METHOD(CountDiscWhithNotAllow)
        {
            Assert::AreEqual(
                10,
                ptrFirst->mapETMTypeDisc[ETM_Extended][ETypeDisc::ETD_Total]
                    .iAmount);
        }

        TEST_METHOD(AllCompIsFind)
        {
            // 12 Штук = УК-1 УК-2 УК-3 ПК-1 ПК-2 ОПК-1 ОПК-2 ОПК-3 ОПК-4 ДПК-1
            // ДОПК-1 СПК-1
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

        shared_ptr<FCurricula> ptrFourth;
        shared_ptr<FCurricula> ptrFifth;
        shared_ptr<FCurricula> ptrSixth;

        ETypeGraph eCommon;

    public:
        TestFGraph()
        {
            if (!Create(ptrGlobal, wsNameConfig))
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

                        ptrGlobal->ptrSolve->Create(
                            it.path().string(),
                            it.path().filename().string());
                    }
                }
            }
            ptrGlobal->ptrSolve->CreateAllGraph();
            int iNumCur = 0;
            ptrFirst    = ptrGlobal->ptrSolve->GetCurricula(iNumCur++);
            ptrSecond   = ptrGlobal->ptrSolve->GetCurricula(iNumCur++);
            ptrThird    = ptrGlobal->ptrSolve->GetCurricula(iNumCur++);
            ptrFourth   = ptrGlobal->ptrSolve->GetCurricula(iNumCur++);
            ptrFifth    = ptrGlobal->ptrSolve->GetCurricula(iNumCur++);
            ptrSixth    = ptrGlobal->ptrSolve->GetCurricula(iNumCur++);

            eCommon = ETypeGraph::ETG_Common;
        }

        TEST_METHOD(InitGraph)
        {
            auto& ptrThis = ptrFirst;
            Assert::IsTrue(ptrThis->ptrGraph != nullptr);
        }

        TEST_METHOD(InitAllTypeGraph)
        {
            auto& ptrThis = ptrFirst;
            Assert::AreEqual(4ull + 3ull, ptrThis->ptrGraph->mapGraph.size());
        }

        TEST_METHOD(FirstAmountDiscCourse)
        {
            // Не учитывается дисциплина "Технологическая
            // (проектно-технологическая) практика" Так как она в исключениях
            // файла config.xlsx "Игнорируемые предметы"

            auto&  ptrThis = ptrFirst;
            string sActual;
            for (int i = 0; i < ptrThis->iAmountCourse; ++i)
            {
                if (i)
                    sActual += " ";
                sActual +=
                    to_string(ptrThis->ptrGraph->mapGraph[ETypeGraph(i)]
                                  .mapGraphDataTypeDisc[ETypeDisc::ETD_Total]
                                  .iAmount);
            }
            Assert::AreEqual("2 2 3 0", sActual.c_str());
        }

        TEST_METHOD(SecondAmountDiscCourse)
        {
            auto&  ptrThis = ptrSecond;
            string sActual;
            for (int i = 0; i < ptrThis->iAmountCourse; ++i)
            {
                if (i)
                    sActual += " ";
                sActual +=
                    to_string(ptrThis->ptrGraph->mapGraph[ETypeGraph(i)]
                                  .mapGraphDataTypeDisc[ETypeDisc::ETD_Total]
                                  .iAmount);
            }
            Assert::AreEqual("2 2 2 2", sActual.c_str());
        }

        TEST_METHOD(ThirdAmountDiscCourse)
        {
            auto&  ptrThis = ptrThird;
            string sActual;
            for (int i = 0; i < ptrThis->iAmountCourse; ++i)
            {
                if (i)
                    sActual += " ";
                sActual +=
                    to_string(ptrThis->ptrGraph->mapGraph[ETypeGraph(i)]
                                  .mapGraphDataTypeDisc[ETypeDisc::ETD_Total]
                                  .iAmount);
            }
            Assert::AreEqual("2 3 4 0", sActual.c_str());
        }

        TEST_METHOD(ThirdCreditsDiscTotal)
        {
            // Не учитывается дисциплина "Технологическая
            // (проектно-технологическая) практика" Так как она в исключениях
            // файла config.xlsx "Игнорируемые предметы"
            auto& ptrThis = ptrThird;
            Assert::AreEqual(30.,
                             ptrThis->ptrGraph->mapGraph[eCommon]
                                 .mapGraphDataTypeDisc[ETypeDisc::ETD_Total]
                                 .dCredits);
        }

        TEST_METHOD(ThirdAmountDiscTotal)
        {
            // Не учитывается дисциплина "Технологическая
            // (проектно-технологическая) практика" Так как она в исключениях
            // файла config.xlsx "Игнорируемые предметы"
            auto& ptrThis = ptrThird;
            Assert::AreEqual(9, ptrThis->ptrGraph->mapGraph[eCommon]
                                    .mapGraphDataTypeDisc[ETypeDisc::ETD_Total]
                                    .iAmount);
        }

        TEST_METHOD(ThirdCreditsDiscCommon)
        {
            // Не учитывается дисциплина "Технологическая
            // (проектно-технологическая) практика" Так как она в исключениях
            // файла config.xlsx "Игнорируемые предметы"
            auto& ptrThis = ptrThird;
            Assert::AreEqual(16.,
                             ptrThis->ptrGraph->mapGraph[eCommon]
                                 .mapGraphDataTypeDisc[ETypeDisc::ETD_Common]
                                 .dCredits);
        }

        TEST_METHOD(ThirdAmountDiscCommon)
        {
            // Не учитывается дисциплина "Технологическая
            // (проектно-технологическая) практика" Так как она в исключениях
            // файла config.xlsx "Игнорируемые предметы"
            auto& ptrThis = ptrThird;
            Assert::AreEqual(5, ptrThis->ptrGraph->mapGraph[eCommon]
                                    .mapGraphDataTypeDisc[ETypeDisc::ETD_Common]
                                    .iAmount);
        }

        TEST_METHOD(FirstDenseCommon)
        {
            auto& ptrThis = ptrFirst;
            // Проверено в Gephi
            Assert::AreEqual(0.238, ptrThis->ptrGraph->mapGraph[eCommon].dDense,
                             0.01);
        }

        TEST_METHOD(SecondDenseCommon)
        {
            auto& ptrThis = ptrSecond;
            // Полносвязный граф
            Assert::AreEqual(1., ptrThis->ptrGraph->mapGraph[eCommon].dDense);
        }

        TEST_METHOD(ThirdDenseCommon)
        {
            auto& ptrThis = ptrThird;
            // Проверено в Gephi
            Assert::AreEqual(0.222, ptrThis->ptrGraph->mapGraph[eCommon].dDense,
                             0.01);
        }

        TEST_METHOD(FirstDiametrStepCommon)
        {
            auto& ptrThis = ptrFirst;
            // 3 компонентный граф
            Assert::AreEqual(2.,
                             ptrThis->ptrGraph->mapGraph[eCommon].dDiametrStep);
        }

        TEST_METHOD(ThirdDiametrStepCommon)
        {
            auto& ptrThis = ptrThird;
            // Полносвязный граф
            Assert::AreEqual(2.,
                             ptrThis->ptrGraph->mapGraph[eCommon].dDiametrStep);
        }

        TEST_METHOD(FirstComponentCommon)
        {
            auto& ptrThis = ptrFirst;
            Assert::AreEqual(3,
                             ptrThis->ptrGraph->mapGraph[eCommon].iComponent);
        }

        TEST_METHOD(ThirdComponentCommon)
        {
            auto& ptrThis = ptrThird;
            Assert::AreEqual(4,
                             ptrThis->ptrGraph->mapGraph[eCommon].iComponent);
        }

        TEST_METHOD(FirstClusterFull)
        {
            auto& ptrThis = ptrFirst;
            Assert::AreEqual(
                (3.0 / 5.0),
                ptrThis->ptrGraph->mapGraph[eCommon].dGlobalСluster, 1e-2);
        }

        TEST_METHOD(FourthClusterFull)
        {
            auto& ptrThis = ptrFourth;
            Assert::AreEqual(
                1.0, ptrThis->ptrGraph->mapGraph[eCommon].dGlobalСluster, 1e-2);
        }

        TEST_METHOD(FifthClusterNotFull)
        {
            auto& ptrThis = ptrFifth;
            Assert::AreEqual(
                (0.75), ptrThis->ptrGraph->mapGraph[eCommon].dGlobalСluster,
                1e-2);
        }

        TEST_METHOD(SixthlusterNotFull)
        {
            auto& ptrThis = ptrSixth;
            Assert::AreEqual(
                (3.0 / 5.0),
                ptrThis->ptrGraph->mapGraph[eCommon].dGlobalСluster, 1e-2);
        }
    };

    TEST_CLASS(TestCompTree)    // Проверка рекурсивного дерева
    {
        shared_ptr<FGlobal>    ptrGlobal;
        shared_ptr<FCurricula> ptrFirst;

    public:
        TestCompTree()
        {
            if (!Create(ptrGlobal, wsNameConfig))
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

                        ptrGlobal->ptrSolve->Create(
                            it.path().string(),
                            it.path().filename().string());
                    }
                }

                ptrGlobal->ptrSolve->CreateAllGraph();
                ptrGlobal->ptrSolve
                    ->CreateAllMetric();    // Метрики не связаны с графом

                ptrGlobal->ptrAdapOutData
                    ->Create();    // Будет производится проверка адаптера
            }

            ptrFirst = ptrGlobal->ptrSolve->GetCurricula(0);
        }

        TEST_METHOD(ItsNotEmpty)
        {
            try
            {
                Assert::AreEqual(6, ptrGlobal->ptrSolve->N);
            }
            catch (const std::exception& ex)
            {
                string str = ex.what();
                Assert::Fail(wstring(ALL(str)).c_str());
            }
        }
    };

}    // namespace TestFSolve

namespace AnomalTestFSolve
{
    const wstring wsNameConfigAnomal = CONF_FILE2;

    TEST_CLASS(TestFSolveOnly)
    {
        shared_ptr<FGlobal> ptrGlobal;

    public:
        TestFSolveOnly()
        {
            if (!Create(ptrGlobal, wsNameConfigAnomal))
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

                        ptrGlobal->ptrSolve->Create(
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
            // 2 Штук = УК ПК
            Assert::AreEqual(2ull, ptrGlobal->ptrSolve->setHeaderComp.size());
        }

        TEST_METHOD(MaxCourse)
        {
            Assert::AreEqual(4, ptrGlobal->ptrSolve->iMaxCourse);
        }
    };

    //! Не забудьте закрыть файл config.xlsx и файл учебного плана
    //! Не забудьте закрыть файл config.xlsx и файл учебного плана
    //! Не забудьте закрыть файл config.xlsx и файл учебного плана
    TEST_CLASS(TestFCurricula)
    {
        shared_ptr<FGlobal>    ptrGlobal;
        shared_ptr<FCurricula> ptrFirst;

    public:
        TestFCurricula()
        {
            if (!Create(ptrGlobal, wsNameConfigAnomal))
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

                        ptrGlobal->ptrSolve->Create(
                            it.path().string(),
                            it.path().filename().string());
                    }
                }
            }
            // ptrGlobal->ptrSolve->CreateAllGraph();
            // ptrGlobal->ptrSolve
            //     ->CreateAllMetric();    // Метрики не связаны с графом

            ptrFirst = ptrGlobal->ptrSolve->GetCurricula(0);
        }

        TEST_METHOD(ItsNotEmpty)
        {
            try
            {
                Assert::AreEqual(2, ptrGlobal->ptrSolve->N);
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
            for (const auto& [key, val] :
                 ptrFirst->mapETMTypeDisc[ETM_Extended])
            {
                ++i;
                if (i)
                    sActual += ' ';
                sActual += to_string(key);
                sActual += '=';
                sActual += to_string(val.iAmount);
            }

            // Все только основные
            Assert::AreEqual(string("0=4 1=3 2=1"), sActual);
        }

        TEST_METHOD(CheckTagDisc)
        {
            string sActual;
            int    i = -1;
            for (const auto& [key, val] : ptrFirst->mapETMTagDisc[ETM_Extended])
            {
                ++i;
                if (i)
                    sActual += ' ';
                sActual += to_string(key);
                sActual += '=';
                sActual += to_string(val.iAmount);
            }

            Assert::AreEqual(string("1=1 2=1 3=2"), sActual);
        }

        TEST_METHOD(AllSumCredits)
        {
            Assert::AreEqual(
                10.,
                ptrFirst->mapETMTypeDisc[ETM_NoExtended][ETypeDisc::ETD_Total]
                    .dCredits);
        }

        TEST_METHOD(CountAllowDisc)
        {
            Assert::AreEqual(
                3,
                ptrFirst->mapETMTypeDisc[ETM_NoExtended][ETypeDisc::ETD_Total]
                    .iAmount);
        }

        TEST_METHOD(CountDiscWhithNotAllow)
        {
            Assert::AreEqual(
                4,
                ptrFirst->mapETMTypeDisc[ETM_Extended][ETypeDisc::ETD_Total]
                    .iAmount);
        }

        TEST_METHOD(AllCompIsFind)
        {
            // 12 Штук = УК-1 УК-2 УК-3
            // Компетенции УК-3.1 УК-3.2 проигнорируются и будут считаться как
            // УК-3 Это неплохой компромис, так как в некоторых УП на странице
            // Компетенции(2) указываются индикаторы
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

                        ptrGlobal->ptrSolve->Create(
                            it.path().string(),
                            it.path().filename().string());
                    }
                }
            }
            ptrGlobal->ptrSolve->CreateAllGraph();
            ptrFirst = ptrGlobal->ptrSolve->GetCurricula(0);
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
            // Формула, потом
            // A - суммарное кол-во ЗЕ, потом
            // N - суммарное кол-во дисциплин
            // K - мощность пересечения по компетенциям
            // D - мощность пересечения по дисциплинам

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
            // Первый тест
            // TakeResult принимает L R K и D
            //  ((L + R) / 2) * K
            //  L = 5
            //  R = 11
            //  K = 3
            res1 = fTestFormula[0].TakeResult(5., 11., 3, 0);
            Assert::AreEqual(((5. + 11.) / 2) * 3, res1);
        }

        TEST_METHOD(Test2)
        {
            // Второй тест
            // (L / 2 + R / 2) * K
            //  L = 6
            //  R = 12.5
            //  K = 11
            res2 = fTestFormula[1].TakeResult(6., 12.5, 11, 0);
            Assert::AreEqual((6. / 2 + 12.5 / 2) * 11, res2);
        }

        TEST_METHOD(Test2Add)
        {
            // Второй тест +
            res1 = fTestFormula[0].TakeResult(6., 12.5, 11, 0);
            res2 = fTestFormula[1].TakeResult(6., 12.5, 11, 0);
            Assert::AreEqual(res1, res2);
        }

        TEST_METHOD(Test3)
        {
            // Третий тест
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
                res1);    // Проверка на то, что всё удачно выполнилось
        }

        TEST_METHOD(Test8)
        {
            // Первый тест
            // TakeResult принимает L R K и D
            //  ((L + R) / 2) * D
            //  L = 5
            //  R = 11
            //  D = 3
            res1 = fTestFormula[7].TakeResult(5., 11., 0, 3);
            Assert::AreEqual(((5. + 11.) / 2) * 3, res1);
        }
    };
}    // namespace TestFormulaParser
