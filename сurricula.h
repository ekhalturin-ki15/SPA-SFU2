#pragma once
#include "global.h"

struct FGraph;
struct FMetric;

enum ETagDisc;

// Тип дисциплины (основная, по выбору, факультатив)
enum EClassicTypeDisc : int
{
    ETD_Total = 0,    // Заглушка, чтобы считать общую статистику в том числе
    ETD_Common,
    ETD_Chosen,
    ETD_Elective,
    ETD_Add,
    ETD_Size
};

//using ETypeDisc = int; // Больше типов данных

struct FTreeElement
{
    explicit FTreeElement();

    bool bAllow;    // Учитывать ли при подсчёте зачётных единиц (ЗЕ)
    bool bNoIgnore;    // Если предмет входит в перечень игнорируемых

    double dSumScore;    // Количество зачётных единиц (ЗЕ)
    map<int, double>
        mapCourseScore;    // В каком семестре (ключ) сколько ЗЕ (значение)

    // wstring wsName;
    string  sName;
    wstring wsIndexName;

    shared_ptr<FTreeElement> ptrParent;
    vector<shared_ptr<FTreeElement>> arrChild;    // Дисциплины внутри модуля
    // Указаны компетенции с индикаторами для дополнительной валидации УП (что у
    // страницы Компетенции (2) есть весь перечень компетенций)
    map<string, set<string>>
        mapComp;    // Компетенции, у каждой из которых перечень индикаторов
                    // Для обратного графа перечислены дисциплины

    //ETypeDisc eTypeDisc = ETypeDisc::ETD_Common;
    int eTypeDisc = EClassicTypeDisc::ETD_Common;
    // Перечисляются теги дисциплины (Гуманитарная, естеств., общепроф.)
    // Может быть несколько тегов у одной дисциплины
    set<ETagDisc> setTagDisc;

    // vector<string> arrIndicator; // Индикаторы (из индикатора можно извлечь
    // компетенцию)
};

struct FDiscParams
{
    int    iAmount;
    double dCredits;
};

// Один конкретный УП
struct FCurricula
{
    friend struct FSolve;

    explicit FCurricula(shared_ptr<FGlobal> _ptrGlobal);
    ~FCurricula();

    void CountDisc();

    shared_ptr<FTreeElement> ptrRoot;
    map<wstring, shared_ptr<FTreeElement>>
        mapAllDisc;    // Поиск указателя на дисциплину по её индексу

    // Сквозная индексация (совпадает с mapAllDisc)
    map<wstring, shared_ptr<FTreeElement>>
        mapNoIgnoreDisc;    // Оставляем только разрешённые дисциплины (без
                            // модулей) для анализа (и без тех, у кого ЗЕ = 0)

    map<wstring, shared_ptr<FTreeElement>>
        mapNoIgnoreComp;    // Аналогично mapNoIgnoreDisc, но для компетенций
                            // (пригодится для ETG_Reverse графа)

    map<wstring, wstring> mapNameToIndexDisc;

    string sNamePlan;
    string sShortNamePlan;    // Имя УП без расширения
    string sCurName;          // Имя УП без расширения
    string sTypePlan;    // Указывается: бакалавр, магистр, специалист или
                         // абитуриент

    int iYearStart;    // Год начала обучения
    int iCodeUGSN;     // Год начала обучения

    int iAmountCourse;    // Количество курсов (именно курсов, не семестров)

    // double dAllSumScore;    // Общее кол-во ЗЕ курса (только дисциплин, и
    // только
    //  тех, что учитываются)
    // int iAmountDisc;    // Количество учитываемых дисциплин (без игнорируемых
    // в списке) int iExtendedAmountDisc;    // Количество всех дисциплин (не
    // модулей)

    // vector<double> arrETMAllSumScore;
    // vector<int>    arrETMAmountDisc;
    vector<map<int, FDiscParams>>
        mapETMTypeDisc;    // Количество и ЗЕ дисциплин по типу (Все, основные,
                           // по выбору, факультативы), Учитываются в том числе
                           // и те, что не считаются в Плане
    vector<map<ETagDisc, FDiscParams>>
        mapETMTagDisc;    // Количество дисциплин по теги
                          // (гуманитарные,
    // технические, естествонаучные)

    set<string> setAllComp;    // Множество всех компетенций, присутствующих в
                             // учебном плане (УП)

    // Не нужно, так как эти значения хранятся в iAmountDisc iExtendedAmountDisc
    // и mapAmountTypeDisc void FindAllScore(double& outDSum, int&
    // outIAmountDisc);    // Вывод через параметры void FindAllScore(double&
    // outDSum);

    shared_ptr<FGraph> ptrGraph;    // У каждого УП свой объект класса FGraph
    shared_ptr<FMetric> ptrMetric;    // У каждого УП свой объект класса FMetric

    // Добавил, чтобы можно было обращаться к Config
    shared_ptr<FGlobal> ptrGlobal;    // Синглтон
private:
    void DeleteDFS(shared_ptr<FTreeElement>
                       ptrThis);    // Поиск в глубину для очистки памяти

    // Получить только дисциплины (без модулей)
    // Вывод результата через параметр
    void GetMapNoIgnoreDisc(
        map<wstring, shared_ptr<FTreeElement>>& mapDiscReturn,
        bool                                    IsNecessaryAllow,
        bool                                    IsNecessaryNotIgnore);

    void GetMapNoIgnoreComp(
        map<wstring, shared_ptr<FTreeElement>>&       mapCompReturn,
        const map<wstring, shared_ptr<FTreeElement>>& mapNoIgnoreDisc);
};