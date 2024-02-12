#pragma once
#include "solve.h"
#include <map>

using namespace std;
struct FCurricula;

// Использую структуру данных Бор, так как есть Компетенция, её индекс, её
// индикатор и так далее (сейчас вложеность 3, но нет смысла ограничивать)
struct FTreeMetric
{
    explicit FTreeMetric(shared_ptr<FMetric> _ptrMetric);

    string sName = "-";

    // Получилось матрёшка

    int iAmountUsingDisc = 0;
    double dNoBalanceSum = 0.;    // Всего ЗЕ без пересечения у всех Компетенций
    double dBalanceSum = 0.;    // Всего ЗЕ с пересечением у всех Компетенций
    double dChosenSum =
        0.;    // Всего ЗЕ, которые были выбраны для вывода пользователем (В
               // зависимости от ptrConfig->bCompInterDelete)
    double dInclusionPercent = 0.;

    map<string, shared_ptr<FTreeMetric>> mapChild;
    shared_ptr<FTreeMetric>              ptrParent = nullptr;

    void Delete();
    void DeleteDFS(shared_ptr<FTreeMetric> th);
    void InitBalanceScore();
    void InitBalanceScoreDFS(shared_ptr<FTreeMetric> th);

    void InitChosenScore(const bool& bIsPercentRegAll,
                         const bool& bCompInterDelete);
    void InitChosenScoreDFS(shared_ptr<FTreeMetric> th,
                            const double&           dAllSum,
                            const bool&             bIsPercentRegAll,
                            const bool&             bCompInterDelete);

    // map<string, double> mapCompDistr; //ЗЕ у каждой компетенции по
    // отдельности map<string, double> mapBalancAmountCompDistr;//Всего ЗЕ с
    // пересечением у конкретной компетенции map<string, map<string, double>>
    // mapIndicDistr;//ЗЕ у каждого индикатора по отдельности

    shared_ptr<FMetric> ptrMetric;
};

// Класс похож на FGraph по идеологии, но в нём другие данные
struct FMetric
{
    static string sAllMetric;    // Плохо, что не статик, ну да ладно
    static const string sEmptyIndicator;

    // Инверсия зависимости
    explicit FMetric(shared_ptr<FCurricula> _ptrCurricula);
    void Init();    // Для позднего связывания, чтобы у родителя уже был создан
                    // shared_ptr

    ~FMetric();

    void InitBalanceScore();

    // У всего графа целиком тег sAllMetric, у остальных по курсам
    shared_ptr<FTreeMetric> ptrTreeMetric;

private:
    // Использовать только после вызова Read у ptrSolve, то есть, когда ptrTree
    // заполнен Вызывается в конце Init
    void Create();

    void UpdateCourseMetric(shared_ptr<FTreeMetric> ptrRootTree,
                            set<vector<string>>&    setIsTakenScore,
                            const vector<string>&   arrRectUpdate,
                            const double&           dScore);

    // Смотрим, какие компетенции мы уже учитывали ранее, а если не учитывали,
    // то на сколько ЗЕ изменить
    void UpdateMetricBranch(shared_ptr<FTreeMetric> ptrNowTree,
                            set<vector<string>>&    setIsTakenScore,
                            const string&           sName,
                            const double&           dScore);

    void AddScoreNoBalanceSum(shared_ptr<FTreeMetric> ptrNowTree,
                              set<vector<string>>&    setIsTakenScore,
                              const vector<string>&   sCurrent,
                              const double&           dScore);

    shared_ptr<FCurricula> ptrCurricula;
    vector<regex> arrRegexHeaderInd;    // Может быть несколько, поэтому вектор
    vector<regex> arrRegexHeaderComp;    // Может быть несколько, поэтому вектор

    // mapAllowDisc теперь в FSolve вместе с УП
    // map<wstring, FTreeElement*> mapAllowDisc;   // Оставляем только
    // разрешённые дисциплины (без
    // модулей) для анализа (и без тех, у кого ЗЕ = 0)
};
