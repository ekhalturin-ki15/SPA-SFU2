#pragma once
#include "solve.h"
#include <map>

using namespace std;
struct FTreeDisc;

// Использую структуру данных Бор, так как есть Компетенция, её индекс, её
// индикатор и так далее (сейчас вложеность 3, но нет смысла ограничивать)
struct FTreeMetric
{
    string sName = "-";

    // Получилось матрёшка

    int iAmountUsingDisc = 0;
    double dNoBalanceSum = 0.;    // Всего ЗЕ без пересечения у всех Компетенций
    double dBalanceSum = 0.;    // Всего ЗЕ с пересечением у всех Компетенций
    double dChosenSum =
        0.;    // Всего ЗЕ, которые были выбраны для вывода пользователем (В
               // зависимости от ptrConfig->bCompInterDelete)
    double dInclusionPercent = 0.;

    map<string, FTreeMetric*> mapChild;
    FTreeMetric* ptrParent = nullptr;    // Инверсия зависимости

    void Delete();
    void DeleteDFS(FTreeMetric* th);
    void InitBalanceScore();
    void InitBalanceScoreDFS(FTreeMetric* th);

    void InitChosenScore(const bool& bIsPercentRegAll,
                         const bool& bCompInterDelete);
    void InitChosenScoreDFS(FTreeMetric*  th,
                            const double& dAllSum,
                            const bool&   bIsPercentRegAll,
                            const bool&   bCompInterDelete);

    // map<string, double> mapCompDistr; //ЗЕ у каждой компетенции по
    // отдельности map<string, double> mapBalancAmountCompDistr;//Всего ЗЕ с
    // пересечением у конкретной компетенции map<string, map<string, double>>
    // mapIndicDistr;//ЗЕ у каждого индикатора по отдельности
};

// Класс похож на FGraph по идеологии, но в нём другие данные
struct FMetric
{
    static string sAllMetric;    // Плохо, что не статик, ну да ладно
    static const string sEmptyIndicator;
    // Инверсия зависимости
    explicit FMetric(FTreeDisc* _ptrTree);
    ~FMetric();
    // Использовать только после вызова Read у ptrSolve, то есть, когда ptrTree
    // заполнен
    void Create();
    void InitBalanceScore();

    // У всего графа целиком тег sAllMetric, у остальных по курсам
    FTreeMetric* ptrTreeMetric;

private:
    void UpdateCourseMetric(FTreeMetric*          ptrRootTree,
                            set<vector<string>>&  setIsTakenScore,
                            const vector<string>& arrRectUpdate,
                            const double&         dScore);

    // Смотрим, какие компетенции мы уже учитывали ранее, а если не учитывали,
    // то на сколько ЗЕ изменить
    void UpdateMetricBranch(FTreeMetric*         ptrNowTree,
                            set<vector<string>>& setIsTakenScore,
                            const string&        sName,
                            const double&        dScore);

    void AddScoreNoBalanceSum(FTreeMetric*          ptrNowTree,
                              set<vector<string>>&  setIsTakenScore,
                              const vector<string>& sCurrent,
                              const double&         dScore);

    FTreeDisc*    ptrTree;
    vector<regex> arrRegexHeaderInd; // Может быть несколько, поэтому вектор
    vector<regex> arrRegexHeaderComp;// Может быть несколько, поэтому вектор

    // mapAllowDisc теперь в FSolve вместе с УП
    // map<wstring, FTreeElement*> mapAllowDisc;   // Оставляем только
    // разрешённые дисциплины (без
    // модулей) для анализа (и без тех, у кого ЗЕ = 0)
};
