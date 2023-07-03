#pragma once
#include "solve.h"
#include <map>

using namespace std;
struct FTreeDisc;

// Использую структуру данных Бор, так как есть Компетенция, её индекс, её индикатор и так далее (сейчас вложеность 3, но нет смысла
// ограничивать)
struct FTreeMetric
{
    string sName = "-";

    // Получилось матрёшка
    double                    dNoBalanceSum = 0.;    // Всего ЗЕ с пересечением у всех Компетенций
    double                    dBalanceSum   = 0.;    // Всего ЗЕ с пересечением у всех Компетенций
    map<string, FTreeMetric*> mapChild;
    FTreeMetric*              ptrParent = nullptr;    // Инверсия зависимости

    void Delete();
    void DeleteDFS(FTreeMetric* th);
    void InitBalanceScore();
    void InitBalanceScoreDFS(FTreeMetric* th);

    // map<string, double> mapCompDistr; //ЗЕ у каждой компетенции по отдельности
    // map<string, double> mapBalancAmountCompDistr;//Всего ЗЕ с пересечением у конкретной компетенции
    // map<string, map<string, double>> mapIndicDistr;//ЗЕ у каждого индикатора по отдельности
};

// Класс похож на FGraph по идеологии, но в нём другие данные
struct FMetric
{
    static const string sAllMetric;
    static const string sEmptyIndicator;
    // Инверсия зависимости
    explicit FMetric(FTreeDisc* _ptrTree);
    ~FMetric();
    // Использовать только после вызова Read у ptrSolve, то есть, когда ptrTree заполнен
    void Create();
    void InitBalanceScore();

    // У всего графа целиком тег sAllMetric, у остальных по курсам
    FTreeMetric* ptrTreeMetric;

private:
    void UpdateCourseMetric(FTreeMetric*          ptrRootTree,
                            set<vector<string>>&  setIsTakenScore,
                            const vector<string>& arrRectUpdate,
                            const double&         dScore);

    // Смотрим, какие компетенции мы уже учитывали ранее, а если не учитывали, то на сколько ЗЕ изменить
    void UpdateMetricBranch(FTreeMetric* ptrNowTree, set<vector<string>>& setIsTakenScore, const string& sName, const double& dScore);

    FTreeDisc* ptrTree;
    regex      fRegexHeaderIndicator;

    map<wstring, FTreeElement*> mapAllowDisc;    // Оставляем только разрешённые дисциплины (без модулей) для анализа
};
