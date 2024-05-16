#include "solve.h"
#include "config.h"
#include "error.h"
#include "global.h"
#include "graph.h"
#include "metric.h"

int FSolve::iSinglControll = 0;

FSolve::FSolve(shared_ptr<FGlobal> _ptrGlobal)
    : ptrGlobal(_ptrGlobal),
      bIsCorrectParsing(true),
      iCurrentPage(0),
      iCurrentRow(0),
      iMaxCourse(0)
{
    // Unit test против такого
#ifndef UNIT_TEST
    if (iSinglControll > 0)
        throw std::runtime_error("Re-creation Singleton");
#endif
    ++iSinglControll;

    ptrSolveSecondPage = make_unique<FSolveSecondPage>(_ptrGlobal);

    ptrGraphAllData = make_unique<FGraphAllData>(_ptrGlobal);
}

bool FSolve::Init()
{
    try
    {
        // fRegexComp = ptrGlobal->ptrConfig->GetSRegexComp();
        arrRegexComp.resize(ptrGlobal->ptrConfig->arrRegexComp.size());
        for (int i = 0; i < ptrGlobal->ptrConfig->arrRegexComp.size(); ++i)
            arrRegexComp[i] = ptrGlobal->ptrConfig->arrRegexComp[i];
    }
    catch (...)
    {
        ptrGlobal->ptrError->ErrorBadRegex(
            "Регулярное выражение разбивки строки ([Компетенции(2)] "
            "Формируемые компетенции)");
    }

    try
    {
        arrRegexCodeUGSN.resize(ptrGlobal->ptrConfig->arrRegexCodeUGSN.size());
        for (int i = 0; i < ptrGlobal->ptrConfig->arrRegexCodeUGSN.size(); ++i)
            arrRegexCodeUGSN[i] = ptrGlobal->ptrConfig->arrRegexCodeUGSN[i];
    }
    catch (...)
    {
        ptrGlobal->ptrError->ErrorBadRegex(
            "Регулярное выражение разбивки кода направления подготовки");
    }

    try
    {
        // fRegexHeaderComp = ptrGlobal->ptrConfig->sRegexHeaderComp;
        arrRegexHeaderComp.resize(
            ptrGlobal->ptrConfig->arrRegexHeaderComp.size());
        for (int i = 0; i < ptrGlobal->ptrConfig->arrRegexHeaderComp.size();
             ++i)
            arrRegexHeaderComp[i] = ptrGlobal->ptrConfig->arrRegexHeaderComp[i];
    }
    catch (...)
    {
        ptrGlobal->ptrError->ErrorBadRegex(
            "Регулярное выражение поиска заголовка компетенции");
    }

    try
    {
        // fRegexHeaderInd = ptrGlobal->ptrConfig->sRegexHeaderIndicator;
        arrRegexHeaderInd.resize(
            ptrGlobal->ptrConfig->arrRegexHeaderInd.size());
        for (int i = 0; i < ptrGlobal->ptrConfig->arrRegexHeaderInd.size(); ++i)
            arrRegexHeaderInd[i] = ptrGlobal->ptrConfig->arrRegexHeaderInd[i];
    }
    catch (...)
    {
        ptrGlobal->ptrError->ErrorBadRegex(
            "Регулярное выражение поиска заголовка индикатора");
    }
    return true;
}

void FSolve::ClearCurricula()
{
    arrDisc.clear();
}

bool FSolve::Create(string _sInPath, string sNamePlan)
{
    // ClearCurricula();

    sInPath = ptrGlobal->ConwertPathFormat(_sInPath, true);

    OpenXLSX::XLDocument fDoc;
    OpenXLSX::XLWorkbook fBook;
    iCurrentPage      = 0;
    bIsCorrectParsing = true;

    try
    {
        shared_ptr<FCurricula> ptrCurricula =
            make_shared<FCurricula>(ptrGlobal);
        arrDisc.push_back(ptrCurricula);
        arrDisc.back()->sNamePlan = sNamePlan;
        arrDisc.back()->sShortNamePlan =
            sNamePlan.substr(0, sNamePlan.find('.'));

        arrDisc.back()->sCurName = (ptrGlobal->ptrConfig->GetBOutShortNameCur())
                                       ? arrDisc.back()->sShortNamePlan
                                       : arrDisc.back()->sNamePlan;

        fDoc.open(sInPath);
        fBook = fDoc.workbook();
        ZeroPageCreateDiscTree(
            fBook.worksheet(
                ptrGlobal->ptrConfig->GetKeyPage(iCurrentPage).sName),
            iCurrentPage);
        ++iCurrentPage;

        FirstPageAddCompIndicator(
            fBook.worksheet(
                ptrGlobal->ptrConfig->GetKeyPage(iCurrentPage).sName),
            iCurrentPage);
        ++iCurrentPage;

        ptrSolveSecondPage->AddDiscScore(
            fBook.worksheet(
                ptrGlobal->ptrConfig->GetKeyPage(iCurrentPage).sName),
            iCurrentPage);
        ++iCurrentPage;

        ThirdPageFindTypePlan(
            fBook.worksheet(
                ptrGlobal->ptrConfig->GetKeyPage(iCurrentPage).sName),
            iCurrentPage);
        ++iCurrentPage;

        arrDisc.back()->CountDisc();

        if (ptrSolveSecondPage->DFSCountingScore(arrDisc.back()->ptrRoot) !=
            arrDisc.back()->arrETMAllSumScore[ETM_NoIgnore])
        {
            throw std::logic_error(FError::sNotEqualSum);
        }

        arrDisc.back()->mapNoIgnoreDisc =
            arrDisc.back()->GetMapNoIgnoreDisc(true, true);

        ++iCurrentPage;
    }
    catch (logic_error eError)
    {
        arrDisc.back().reset();
        arrDisc.pop_back();
        /*if (FError::sDontHaveIndex== eError.what())
        {
                ptrGlobal->ptrError->ErrorBadParser(sInPath);
        }
        else*/
        if (FError::sBadTree == eError.what())
        {
            ptrGlobal->ptrError->ErrorBadTree();
        }
        else if (FError::sNotFoundKeyCol == eError.what())
        {
            ptrGlobal->ptrError->ErrorNotFoundKeyCol();
        }
        else if (FError::sNotEqualSum == eError.what())
        {
            ptrGlobal->ptrError->ErrorNotEqualSum();
        }

        fDoc.close();
        return false;    // Но продолжаем работать с другими файлами
    }
    catch (...)
    {
        if (sInPath.find(XLSX) == string::npos)
        {
            ptrGlobal->ptrError->ErrorUncorrectExtension();
        }
        else
        {
            ptrGlobal->ptrError->ErrorInFileNotFind(sInPath);
        }
        return false;    // Но продолжаем работать с другими файлами
    }

    if (this->bIsCorrectParsing)
        ptrGlobal->ptrError->OKParsing();
    else
        ptrGlobal->ptrError->WAParsing();
    fDoc.close();

    return bIsCorrectParsing;
}

void FSolve::CreateAllGraph()
{
    for (auto& it : arrDisc)
    {
        it->ptrGraph = make_shared<FGraph>(it);
        it->ptrGraph->Create();    // А теперь, построй граф на основе УП
    }
    //Добавились данные о всех Графах (генеральной совокупности)
    for (auto& it : arrDisc)
    {
        //it->ptrGraph->CreateAfter();    // После подсчитанных общих метрик,
                                       // определить ещё дополнительные (на
                                       // основе генеральной совокупности)
    }
}

void FSolve::CreateAllMetric()
{
    for (auto& it : arrDisc)
    {
        it->ptrMetric = make_shared<FMetric>(it);
        it->ptrMetric->Init();
        // it->ptrMetric->Create();    // А теперь, посчитай метрики УП
    }
}

FSolve::~FSolve()
{
    for (auto& it : arrDisc)
        it.reset();

    ptrSolveSecondPage.reset();
    --iSinglControll;
}
