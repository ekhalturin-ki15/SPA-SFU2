#include "metric.h"
#include "config.h"
#include "error.h"
#include "global.h"

#include "formulaParser.h"

string       FMetric::sAllMetric      = "All";
const string FMetric::sEmptyIndicator = "-";

FTreeMetric::FTreeMetric(shared_ptr<FMetric> _ptrMetric) : ptrMetric(_ptrMetric)
{
}

void FTreeMetric::DeleteDFS(shared_ptr<FTreeMetric> th)
{
    for (auto& [key, val] : th->mapChild)
        DeleteDFS(val);
    th.reset();
}

void FTreeMetric::InitBalanceScoreDFS(shared_ptr<FTreeMetric> th)
{
    if (th->mapChild.size() == 0)
    {
        th->dBalanceSum = th->dNoBalanceSum;    // У листьев совпадает
        //(так как внутри нет разделений, а значит и нет кругов Эйлера
        //(пересечений))
        return;
    }

    double dScore = 0.;
    for (auto& [key, val] : th->mapChild)
    {
        InitBalanceScoreDFS(val);
        dScore += val->dBalanceSum;
    }
    th->dBalanceSum = dScore;
}

void FTreeMetric::InitChosenScoreDFS(shared_ptr<FTreeMetric> th,
                                     const double&           dAllSum,
                                     const bool&             bIsPercentRegAll,
                                     const bool&             bCompInterDelete)
{
    th->dChosenSum = (bCompInterDelete) ? th->dBalanceSum : th->dNoBalanceSum;
    if (bIsPercentRegAll)
    {
        th->dInclusionPercent = (bCompInterDelete)
                                    ? th->dBalanceSum / dAllSum
                                    : th->dNoBalanceSum / dAllSum;
    }
    else
    {
        th->dInclusionPercent =
            (bCompInterDelete)
                ? th->dBalanceSum / th->ptrParent->dBalanceSum
                : th->dNoBalanceSum / th->ptrParent->dNoBalanceSum;
    }
    // if (th->mapChild.size() == 0) return;
    for (auto& [key, val] : th->mapChild)
    {
        InitChosenScoreDFS(val, dAllSum, bIsPercentRegAll, bCompInterDelete);
    }
}

void FTreeMetric::Delete()
{
    DeleteDFS(this->ptrMetric->ptrTreeMetric);
}

void FTreeMetric::InitBalanceScore()
{
    InitBalanceScoreDFS(this->ptrMetric->ptrTreeMetric);
}

void FTreeMetric::InitChosenScore(const bool& bIsPercentRegAll,
                                  const bool& bCompInterDelete)
{
    for (const auto& [key, val] : this->mapChild)
    {
        double dAllScore =
            (bCompInterDelete) ? val->dBalanceSum : val->dNoBalanceSum;

        InitChosenScoreDFS(val, dAllScore, bIsPercentRegAll, bCompInterDelete);
    }
}

FMetric::~FMetric()
{
    ptrTreeMetric->Delete();
}

void FMetric::InitBalanceScore()
{
    ptrTreeMetric->InitBalanceScore();
}

// Инверсия зависимости
FMetric::FMetric(shared_ptr<FCurricula> _ptrCurricula)
    : ptrCurricula(_ptrCurricula)
{
}

void FMetric::Init()
{
    // sAllMetric = _ptrTree->ptrGlobal->ConwertToString(
    //     _ptrTree->ptrGlobal->ptrConfig->wsOutPrefAllCourse);

    sAllMetric = ptrCurricula->ptrGlobal->ConwertToString(
        ptrCurricula->ptrGlobal->ptrConfig
            ->mapArrOutParams
                [L"Предлог перед выводом статистики по всем курсам"]
            .GetName());

    try
    {
        // fRegexHeaderComp = _ptrTree->ptrGlobal->ptrConfig->sRegexHeaderComp;
        arrRegexHeaderComp.resize(
            ptrCurricula->ptrGlobal->ptrConfig->GetArrRegexHeaderComp().size());
        for (int i = 0;
             i <
             ptrCurricula->ptrGlobal->ptrConfig->GetArrRegexHeaderComp().size();
             ++i)
            arrRegexHeaderComp[i] =
                ptrCurricula->ptrGlobal->ptrConfig->GetArrRegexHeaderComp()[i];
    }
    catch (...)
    {
        ptrCurricula->ptrGlobal->ptrError->ErrorBadRegex(
            "Регулярное выражение поиска заголовка компетенции");
    }

    try
    {
        // fRegexHeaderInd =
        // _ptrTree->ptrGlobal->ptrConfig->sRegexHeaderIndicator;
        arrRegexHeaderInd.resize(
            ptrCurricula->ptrGlobal->ptrConfig->GetArrRegexHeaderInd().size());
        for (int i = 0;
             i <
             ptrCurricula->ptrGlobal->ptrConfig->GetArrRegexHeaderInd().size();
             ++i)
            arrRegexHeaderInd[i] =
                ptrCurricula->ptrGlobal->ptrConfig->GetArrRegexHeaderInd()[i];
    }
    catch (...)
    {
        ptrCurricula->ptrGlobal->ptrError->ErrorBadRegex(
            "Регулярное выражение поиска заголовка индикатора");
    }
    // mapAllowDisc = _ptrTree->GewMapAllowDisc(true, true);

    ptrTreeMetric = make_shared<FTreeMetric>(ptrCurricula->ptrMetric);
    ptrTreeMetric->ptrParent = nullptr;
    ptrTreeMetric->sName     = "Root";

    ptrTreeMetric->mapChild[sAllMetric] =
        make_shared<FTreeMetric>(ptrCurricula->ptrMetric);

    ptrTreeMetric->mapChild[sAllMetric]->ptrParent = ptrTreeMetric;
    ptrTreeMetric->mapChild[sAllMetric]->sName     = sAllMetric;

    for (int iCourse = 1; iCourse <= ptrCurricula->iAmountCourse;
         ++iCourse)    // Делаем 1 нумерацию
    {
        ptrTreeMetric->mapChild[to_string(iCourse)] =
            make_shared<FTreeMetric>(ptrCurricula->ptrMetric);
        ptrTreeMetric->mapChild[to_string(iCourse)]->ptrParent = ptrTreeMetric;
        ptrTreeMetric->mapChild[to_string(iCourse)]->sName = to_string(iCourse);
    }

    Create();
}

void FMetric::UpdateMetricBranch(shared_ptr<FTreeMetric> ptrNowTree,
                                 set<vector<string>>&    setIsTakenScore,
                                 const string&           sName,
                                 const double&           dScore)
{
    if (!ptrNowTree->mapChild.count(sName))
    {
        ptrNowTree->mapChild[sName] =
            make_shared<FTreeMetric>(ptrCurricula->ptrMetric);
        ptrNowTree->mapChild[sName]->ptrParent = ptrNowTree;
        ptrNowTree->mapChild[sName]->sName     = sName;
    }
    auto ptrRec = ptrNowTree->mapChild[sName];

    vector<string> arrCurrent;

    while (ptrRec->ptrParent)
    {
        arrCurrent.push_back(ptrRec->sName);
        ptrRec = ptrRec->ptrParent;
    }
    AddScoreNoBalanceSum(ptrNowTree->mapChild[sName], setIsTakenScore,
                         arrCurrent, dScore);

    // if (!setIsTakenScore.count( arrCurrent ))
    //{
    //     ptrNowTree->mapChild[sName]->dNoBalanceSum += dScore;
    //     ptrNowTree->mapChild[sName]->iAmountUsingDisc += 1; // Подсчитываем
    //     кол-во дисциплин, которые участвовали в формировании компетенции
    //     setIsTakenScore.insert( arrCurrent );
    // }
}

void FMetric::AddScoreNoBalanceSum(shared_ptr<FTreeMetric> ptrNowTree,
                                   set<vector<string>>&    setIsTakenScore,
                                   const vector<string>&   sCurrent,
                                   const double&           dScore)
{
    if (!setIsTakenScore.count({ sCurrent }))
    {
        ptrNowTree->dNoBalanceSum += dScore;
        ptrNowTree->iAmountUsingDisc +=
            1;    // Подсчитываем кол-во дисциплин, которые участвовали в
                  // формировании компетенции
        setIsTakenScore.insert({ sCurrent });
    }
}

void FMetric::UpdateCourseMetric(shared_ptr<FTreeMetric> ptrRootTree,
                                 set<vector<string>>&    setIsTakenScore,
                                 const vector<string>&   arrRectUpdate,
                                 const double&           dScore)
{
    auto ptrNowTree = ptrRootTree;
    for (const auto& sCurName : arrRectUpdate)
    {
        UpdateMetricBranch(ptrNowTree, setIsTakenScore, sCurName, dScore);
        ptrNowTree = ptrNowTree->mapChild[sCurName];
    }
    AddScoreNoBalanceSum(ptrRootTree, setIsTakenScore, { ptrRootTree->sName },
                         dScore);

    /*if (!setIsTakenScore.count({ ptrRootTree->sName }))
    {
        ptrRootTree->dNoBalanceSum += dScore;
        setIsTakenScore.insert({ ptrRootTree->sName });
    }*/
}

void FMetric::Create()
{
    // Сначало для общего, потом для курсов по отдельности
    int iL = -1;
    for (auto& [key, it] : ptrCurricula->mapNoIgnoreDisc)
    {
        set<vector<string>>
            setIsTakenScore;    // Достаточно set, так как идёт разветвление
                                // также и по типу анализа
        //(целиком, или только по определённому курсу)

        double dAmountInd = 0;    // Если установлен флаг bIsNormalizeScoreComp,
                                  // то делим ЗЕ на кол-во индикаторов
        for (auto& [comp, arrInd] : it->mapComp)
        {
            dAmountInd += 1 > arrInd.size() ? 1 : arrInd.size();
        }
        dAmountInd = 1 > dAmountInd ? 1 : dAmountInd;    // Но 0 делить нельзя

        ++iL;
        for (auto& [comp, arrInd] : it->mapComp)
        {
            for (const auto& ind : arrInd)
            {
                string sCompName;
                string sCompNumber;
                string sIndicatorNumber;
                int iTrueMatchInd = 0;    // Не самое гибкое решение
                vector<smatch> matchesInd;

                int i = 0;
                for (const auto& HeaderInd : arrRegexHeaderInd)
                {
                    ++i;
                    vector<smatch> matchesBuf { sregex_iterator { ALL(ind),
                                                                  HeaderInd },
                                                sregex_iterator {} };
                    if (matchesBuf.size() > 0)
                    {
                        matchesInd    = matchesBuf;
                        iTrueMatchInd = i;
                        break;
                    }
                }

                if (iTrueMatchInd)
                {
                    for (const auto& sData : matchesInd)
                    {
                        const auto& arrIndexGroup =
                            ptrCurricula->ptrGlobal->ptrConfig
                                ->arrRegexIndexGroup[iTrueMatchInd - 1];

                        sCompName        = sData[arrIndexGroup[0]].str();
                        sCompNumber      = sData[arrIndexGroup[1]].str();
                        sIndicatorNumber = sData[arrIndexGroup[2]].str();
                    }
                }
                else
                {
                    bool           bIsTrueMatchComp = false;
                    vector<smatch> matchesComp;

                    for (const auto& HeaderComp : arrRegexHeaderComp)
                    {
                        vector<smatch> matchesBuf { sregex_iterator {
                                                        ALL(comp), HeaderComp },
                                                    sregex_iterator {} };
                        if (matchesBuf.size() > 0)
                        {
                            matchesComp      = matchesBuf;
                            bIsTrueMatchComp = true;
                            break;
                        }
                    }

                    for (const auto& sData : matchesComp)
                    {
                        sCompName        = sData[1].str();
                        sCompNumber      = sData[2].str();
                        sIndicatorNumber = sEmptyIndicator;
                    }
                }

                {
                    double dNormalizeScore = it->dSumScore;
                    if (ptrCurricula->ptrGlobal->ptrConfig
                            ->GetBIsNormalizeScoreComp())
                    {
                        // Делим всё на кол-во индикаторов, так как считаем, что
                        // все ЗЕ равномерно делятся на изучение каждого
                        // индикатора (причём, не параллельно)
                        dNormalizeScore /= dAmountInd;
                    }

                    UpdateCourseMetric(
                        ptrTreeMetric->mapChild[sAllMetric],
                        setIsTakenScore,
                        { sCompName, sCompNumber, sIndicatorNumber },
                        dNormalizeScore);

                    // В холостую пройдёмся по дереву, чтобы создать недостающие
                    // нулевые узлы
                    if (ptrCurricula->ptrGlobal->ptrConfig->GetBOutEmptyComp())
                    {
                        for (int iCourse = 1;
                             iCourse <= ptrCurricula->iAmountCourse;
                             ++iCourse)    // Делаем 1 нумерацию
                        {
                            auto ptrNowTree =
                                ptrTreeMetric->mapChild[to_string(iCourse)];
                            for (const auto& sCurName :
                                 { sCompName, sCompNumber, sIndicatorNumber })
                            {
                                if (!ptrNowTree->mapChild.count(sCurName))
                                {
                                    ptrNowTree->mapChild[sCurName] =
                                        make_shared<FTreeMetric>(
                                            ptrCurricula->ptrMetric);
                                    ptrNowTree->mapChild[sCurName]->ptrParent =
                                        ptrNowTree;
                                    ptrNowTree->mapChild[sCurName]->sName =
                                        sCurName;
                                }
                                ptrNowTree = ptrNowTree->mapChild[sCurName];
                            }
                        }
                    }
                }

                // Только для определённого курса
                for (const auto& [iCourse, dScore] : it->mapCourseScore)
                {
                    double dNormalizeScore = dScore;
                    if (ptrCurricula->ptrGlobal->ptrConfig
                            ->GetBIsNormalizeScoreComp())
                    {
                        dNormalizeScore /= dAmountInd;
                    }

                    string sCourse = to_string(iCourse + 1);
                    UpdateCourseMetric(
                        ptrTreeMetric->mapChild[sCourse],
                        setIsTakenScore,
                        { sCompName, sCompNumber, sIndicatorNumber },
                        dNormalizeScore);
                }
            }
        }

        // Восходящая инициализация (с листьев) для высчитывания iBalanceSum
        // А также для расчёта dChosenSum и dInclusionPercent
        InitBalanceScore();
    }

    ptrTreeMetric->InitChosenScore(
        ptrCurricula->ptrGlobal->ptrConfig->GetBIsPercentRegAll(),
        ptrCurricula->ptrGlobal->ptrConfig->GetBCompInterDelete());
}
