#include "metric.h"
#include "config.h"
#include "error.h"
#include "global.h"

#include "formulaParser.h"

string       FMetric::sAllMetric      = "All";
const string FMetric::sEmptyIndicator = "-";

void FTreeMetric::DeleteDFS(FTreeMetric* th)
{
    for (auto& [key, val] : th->mapChild) DeleteDFS(val);
    delete th;
}

void FTreeMetric::InitBalanceScoreDFS(FTreeMetric* th)
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

void FTreeMetric::Delete() { DeleteDFS(this); }

void FTreeMetric::InitBalanceScore() { InitBalanceScoreDFS(this); }

FMetric::~FMetric() { ptrTreeMetric->Delete(); }

void FMetric::InitBalanceScore() { ptrTreeMetric->InitBalanceScore(); }

// Инверсия зависимости
FMetric::FMetric(FTreeDisc* _ptrTree) : ptrTree(_ptrTree)
{
    // sAllMetric = _ptrTree->ptrGlobal->ConwertToString(
    //     _ptrTree->ptrGlobal->ptrConfig->wsOutPrefAllCourse);

    sAllMetric = _ptrTree->ptrGlobal->ConwertToString(
        _ptrTree->ptrGlobal->ptrConfig
            ->mapArrOutParams
                [L"Предлог перед выводом статистики по всем курсам"]
            .at(0));

    try
    {
        fRegexHeaderComp = _ptrTree->ptrGlobal->ptrConfig->sRegexHeaderComp;
    }
    catch (...)
    {
        _ptrTree->ptrGlobal->ptrError->ErrorBadRegex(
            "Регулярное выражение поиска заголовка компетенции");
    }

    try
    {
        fRegexHeaderInd = _ptrTree->ptrGlobal->ptrConfig->sRegexHeaderIndicator;
    }
    catch (...)
    {
        _ptrTree->ptrGlobal->ptrError->ErrorBadRegex(
            "Регулярное выражение поиска заголовка индикатора");
    }
    mapAllowDisc = _ptrTree->GewMapAllowDisc(true, true);

    ptrTreeMetric            = new FTreeMetric;
    ptrTreeMetric->ptrParent = nullptr;
    ptrTreeMetric->sName     = "Root";

    ptrTreeMetric->mapChild[sAllMetric]            = new FTreeMetric;
    ptrTreeMetric->mapChild[sAllMetric]->ptrParent = ptrTreeMetric;
    ptrTreeMetric->mapChild[sAllMetric]->sName     = sAllMetric;

    for (int iCourse = 1; iCourse <= _ptrTree->iAmountCourse;
         ++iCourse)    // Делаем 1 нумерацию
    {
        ptrTreeMetric->mapChild[to_string(iCourse)] = new FTreeMetric;
        ptrTreeMetric->mapChild[to_string(iCourse)]->ptrParent = ptrTreeMetric;
        ptrTreeMetric->mapChild[to_string(iCourse)]->sName = to_string(iCourse);
    }
}

void FMetric::UpdateMetricBranch(FTreeMetric*         ptrNowTree,
                                 set<vector<string>>& setIsTakenScore,
                                 const string&        sName,
                                 const double&        dScore)
{
    if (!ptrNowTree->mapChild.count(sName))
    {
        ptrNowTree->mapChild[sName]            = new FTreeMetric;
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

    if (!setIsTakenScore.count({ arrCurrent }))
    {
        ptrNowTree->mapChild[sName]->dNoBalanceSum += dScore;
        setIsTakenScore.insert({ arrCurrent });
    }
}

void FMetric::UpdateCourseMetric(FTreeMetric*          ptrRootTree,
                                 set<vector<string>>&  setIsTakenScore,
                                 const vector<string>& arrRectUpdate,
                                 const double&         dScore)
{
    auto ptrNowTree = ptrRootTree;
    for (const auto& sCurName : arrRectUpdate)
    {
        UpdateMetricBranch(ptrNowTree, setIsTakenScore, sCurName, dScore);
        ptrNowTree = ptrNowTree->mapChild[sCurName];
    }
    if (!setIsTakenScore.count({ ptrRootTree->sName }))
    {
        ptrRootTree->dNoBalanceSum += dScore;
        setIsTakenScore.insert({ ptrRootTree->sName });
    }
}
void FMetric::Create()
{
    // Сначало для общего, потом для курсов по отдельности
    int iL = -1;
    for (auto& [key, it] : mapAllowDisc)
    {
        set<vector<string>>
            mapIsTakenScore;    // Достаточно set, так как идёт разветвление
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
                vector<smatch> matchesInd { sregex_iterator { ALL(ind),
                                                              fRegexHeaderInd },
                                            sregex_iterator {} };

                string sCompName;
                string sCompNumber;
                string sIndicatorNumber;
                if (matchesInd.size() > 0)
                {
                    for (auto sData : matchesInd)
                    {
                        sCompName        = sData[1].str();
                        sCompNumber      = sData[2].str();
                        sIndicatorNumber = sData[3].str();
                    }
                }
                else
                {
                    vector<smatch> matchesComp {
                        sregex_iterator { ALL(comp), fRegexHeaderComp },
                        sregex_iterator {}
                    };
                    for (auto sData : matchesComp)
                    {
                        sCompName        = sData[1].str();
                        sCompNumber      = sData[2].str();
                        sIndicatorNumber = sEmptyIndicator;
                    }
                }

                {
                    double dNormalizeScore = it->dSumScore;
                    if (ptrTree->ptrGlobal->ptrConfig->bIsNormalizeScoreComp)
                    {
                        // Делим всё на кол-во индикаторов, так как считаем, что
                        // все ЗЕ равномерно делятся на изучение каждого
                        // индикатора (причём, не параллельно)
                        dNormalizeScore /= dAmountInd;
                    }

                    UpdateCourseMetric(
                        ptrTreeMetric->mapChild[sAllMetric],
                        mapIsTakenScore,
                        { sCompName, sCompNumber, sIndicatorNumber },
                        dNormalizeScore);
                }

                for (auto [iCourse, dScore] : it->mapCourseScore)
                {
                    double dNormalizeScore = dScore;
                    if (ptrTree->ptrGlobal->ptrConfig->bIsNormalizeScoreComp)
                    {
                        dNormalizeScore /= dAmountInd;
                    }

                    string sCourse = to_string(iCourse + 1);
                    UpdateCourseMetric(
                        ptrTreeMetric->mapChild[sCourse],
                        mapIsTakenScore,
                        { sCompName, sCompNumber, sIndicatorNumber },
                        dNormalizeScore);
                }
            }
        }

        // Восходящая инициализация (с листьев) для высчитывания iBalanceSum
        InitBalanceScore();
    }
}
