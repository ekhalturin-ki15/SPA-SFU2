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
    // if (iSinglControll > 0) throw std::runtime_error("Re-creation
    // Singleton");
    ++iSinglControll;

    ptrSolveSecondPage = make_unique<FSolveSecondPage>(_ptrGlobal);
}

bool FSolve::Init()
{
    try
    {
        fRegexComp = ptrGlobal->ptrConfig->GetSRegexComp();
    }
    catch (...)
    {
        ptrGlobal->ptrError->ErrorBadRegex(
            "Регулярное выражение разбивки строки ([Компетенции(2)] "
            "Формируемые компетенции)");
    }

    try
    {
        // fRegexHeaderComp = ptrGlobal->ptrConfig->sRegexHeaderComp;
        arrRegexHeaderComp.resize(
            ptrGlobal->ptrConfig->arrRegexHeaderComp.size());
        for (int i = 0; i < ptrGlobal->ptrConfig->arrRegexHeaderComp.size();
             ++i)
            arrRegexHeaderComp[i] = ptrGlobal->ptrConfig->arrRegexHeaderComp[i];

        arrRegexCodeUGSN.resize(ptrGlobal->ptrConfig->arrRegexCodeUGSN.size());
        for (int i = 0; i < ptrGlobal->ptrConfig->arrRegexCodeUGSN.size(); ++i)
            arrRegexCodeUGSN[i] = ptrGlobal->ptrConfig->arrRegexCodeUGSN[i];
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

void FSolve::ClearTreeDisc()
{
    arrDisc.clear();
}

bool FSolve::Read(string _sInPath, string sNamePlan)
{
    // ClearTreeDisc();

    sInPath = ptrGlobal->ConwertPathFormat(_sInPath, true);

    OpenXLSX::XLDocument fDoc;
    OpenXLSX::XLWorkbook fBook;
    iCurrentPage      = 0;
    bIsCorrectParsing = true;

    try
    {
        shared_ptr<FTreeDisc> ptrTree = make_shared<FTreeDisc>(ptrGlobal);
        arrDisc.push_back(ptrTree);
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
            arrDisc.back()->dAllSumScore)
        {
            throw std::logic_error(FError::sNotEqualSum);
        }

        arrDisc.back()->mapAllowDisc =
            arrDisc.back()->GewMapAllowDisc(true, true);

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
        if (sInPath.find(".xlsx") == string::npos)
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
