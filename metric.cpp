#include "metric.h"
#include "config.h"
#include "error.h"
#include "global.h"

#include "formulaParser.h"

// string       FMetric::sAllMetric      = "All";
// const string FMetric::sEmptyIndicator = "-";

FTreeMetric::FTreeMetric(shared_ptr<FMetric> _ptrMetric,
                         shared_ptr<FCurricula>
                             _ptrCurricula)
    : ptrMetric(_ptrMetric), ptrCurricula(_ptrCurricula)
{
    sName = ptrCurricula->ptrGlobal->ptrConfig->GetSNoInitData();
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
    const auto& sAllCourses =
        ptrCurricula->ptrGlobal->ptrConfig->GetSAllCourses();
    try
    {
        // fRegexHeaderComp = _ptrTree->ptrGlobal->ptrConfig->sRegexHeaderComp;
        arrRegexHeaderComp.resize(
            ptrCurricula->ptrGlobal->ptrConfig->GetArrRegexHeaderComp().size());
        for (int i = 0;
             i <
             ptrCurricula->ptrGlobal->ptrConfig->GetArrRegexHeaderComp().size();
             ++i)
            arrRegexHeaderComp[i] =ptrCurricula->ptrGlobal->ReversUTF16RU(
                ptrCurricula->ptrGlobal->ptrConfig->GetArrRegexHeaderComp()[i]);
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
            arrRegexHeaderInd[i] =ptrCurricula->ptrGlobal->ReversUTF16RU(
                ptrCurricula->ptrGlobal->ptrConfig->GetArrRegexHeaderInd()[i]);
    }
    catch (...)
    {
        ptrCurricula->ptrGlobal->ptrError->ErrorBadRegex(
            "Регулярное выражение поиска заголовка индикатора");
    }
    // mapAllowDisc = _ptrTree->GewMapAllowDisc(true, true);

    ptrTreeMetric =
        make_shared<FTreeMetric>(ptrCurricula->ptrMetric, ptrCurricula);
    ptrTreeMetric->ptrParent = nullptr;
    ptrTreeMetric->sName     = "Root";

    ptrTreeMetric->mapChild[sAllCourses] =
        make_shared<FTreeMetric>(ptrCurricula->ptrMetric, ptrCurricula);

    ptrTreeMetric->mapChild[sAllCourses]->ptrParent = ptrTreeMetric;
    ptrTreeMetric->mapChild[sAllCourses]->sName     = sAllCourses;

    for (int iCourse = 1; iCourse <= ptrCurricula->iAmountCourse;
         ++iCourse)    // Делаем 1 нумерацию
    {
        ptrTreeMetric->mapChild[to_string(iCourse)] =
            make_shared<FTreeMetric>(ptrCurricula->ptrMetric, ptrCurricula);
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
            make_shared<FTreeMetric>(ptrCurricula->ptrMetric, ptrCurricula);
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

        double dAmountList =
            0;    // Если установлен флаг bIsNormalizeScoreComp,
                  // то делим ЗЕ:
                  // Если фоаг bIsIgnoreTreeInd на кол-во компетенций
                  // Иначе на кол-во индикаторов

        if (ptrCurricula->ptrGlobal->ptrConfig->GetBIsIgnoreTreeInd())
        {
            dAmountList = it->mapComp.size();
        }
        else
        {
            for (auto& [comp, arrInd] : it->mapComp)
            {
                dAmountList += 1 > arrInd.size() ? 1 : arrInd.size();
            }
        }

        dAmountList =
            1 > dAmountList ? 1 : dAmountList;    // Но 0 делить нельзя

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
                        sIndicatorNumber = ptrCurricula->ptrGlobal->ptrConfig
                                               ->GetSNoInitData();
                    }
                }

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
                                        ptrCurricula->ptrMetric,
                                        ptrCurricula);
                                ptrNowTree->mapChild[sCurName]->ptrParent =
                                    ptrNowTree;
                                ptrNowTree->mapChild[sCurName]->sName =
                                    sCurName;
                            }
                            ptrNowTree = ptrNowTree->mapChild[sCurName];
                        }
                    }
                }

                vector<pair<string, double>> arrTreeRootBrach;
                arrTreeRootBrach.push_back(
                    { ptrCurricula->ptrGlobal->ptrConfig->GetSAllCourses(),
                      it->dSumScore });
                for (const auto& [iCourse, dScore] : it->mapCourseScore)
                {
                    arrTreeRootBrach.push_back(
                        { to_string(iCourse + 1), dScore });
                }

                // Инициализация для всех курсов
                for (const auto& [sTag, dScore] : arrTreeRootBrach)
                {
                    double dNormalizeScore = dScore;
                    if (ptrCurricula->ptrGlobal->ptrConfig
                            ->GetBIsNormalizeScoreComp())
                    {
                        dNormalizeScore /= dAmountList;
                    }

                    vector<string> arrRectUpdate({ sCompName, sCompNumber });
                    if (!ptrCurricula->ptrGlobal->ptrConfig
                             ->GetBIsIgnoreTreeInd())
                    {
                        arrRectUpdate.push_back(sIndicatorNumber);
                    }

                    UpdateCourseMetric(ptrTreeMetric->mapChild[sTag],
                                       setIsTakenScore, arrRectUpdate,
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
