#include "solve.h"
#include "config.h"
#include "error.h"
#include "global.h"
#include "graph.h"
#include "metric.h"

int FSolve::iSinglControll = 0;

FSolve::FSolve(FGlobal* _ptrGlobal)
    : ptrGlobal(_ptrGlobal),
      bIsCorrectParsing(true),
      iCurrentPage(0),
      iCurrentRow(0)
{
    if (iSinglControll > 0) throw std::runtime_error("Re-creation Singleton");
    ++iSinglControll;

    ptrSolveSecondPage = new FSolveSecondPage(_ptrGlobal);
    // fSolveSecondPage.ptrGlobal = _ptrGlobal;
}

bool FSolve::Init()
{
    try
    {
        fRegexComp = ptrGlobal->ptrConfig->sRegexComp;
    }
    catch (...)
    {
        ptrGlobal->ptrError->ErrorBadRegex(
            "Регулярное выражение разбивки строки ([Компетенции(2)] "
            "Формируемые компетенции)");
    }

    try
    {
        fRegexHeaderComp = ptrGlobal->ptrConfig->sRegexHeaderComp;
    }
    catch (...)
    {
        ptrGlobal->ptrError->ErrorBadRegex(
            "Регулярное выражение поиска заголовка компетенции");
    }

    try
    {
        fRegexHeaderInd = ptrGlobal->ptrConfig->sRegexHeaderIndicator;
    }
    catch (...)
    {
        ptrGlobal->ptrError->ErrorBadRegex(
            "Регулярное выражение поиска заголовка индикатора");
    }
    return true;
}

void FSolve::ClearTreeDisc() { arrDisc.clear(); }

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
        fDoc.open(sInPath);
        fBook = fDoc.workbook();
        CreateDiscTree(
            fBook.worksheet(ptrGlobal->ConwertToString(
                ptrGlobal->ptrConfig->arrKeyPage[iCurrentPage].wsName)),
            iCurrentPage);
        ++iCurrentPage;

        AddCompIndicator(
            fBook.worksheet(ptrGlobal->ConwertToString(
                ptrGlobal->ptrConfig->arrKeyPage[iCurrentPage].wsName)),
            iCurrentPage);
        ++iCurrentPage;

        ptrSolveSecondPage->AddDiscScore(
            fBook.worksheet(ptrGlobal->ConwertToString(
                ptrGlobal->ptrConfig->arrKeyPage[iCurrentPage].wsName)),
            iCurrentPage);
        arrDisc.back()->CountDisc();

        if (ptrSolveSecondPage->DFSCountingScore(arrDisc.back()->ptrRoot) !=
            arrDisc.back()->dAllSumScore)
        {
            throw std::logic_error(FError::sNotEqualSum);
        }
        arrDisc.back()->sNamePlan = sNamePlan;
        arrDisc.back()->sShortNamePlan =
            sNamePlan.substr(0, sNamePlan.find('.'));
        ++iCurrentPage;
    }
    catch (logic_error eError)
    {
        delete arrDisc.back();
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
        it->ptrGraph = new FGraph(it);
        it->ptrGraph->Create();    // А теперь, построй граф на основе УП
    }
}

void FSolve::CreateAllMetric()
{
    for (auto& it : arrDisc)
    {
        it->ptrMetric = new FMetric(it);
        it->ptrMetric->Create();    // А теперь, посчитай метрики УП
    }
}

FSolve::~FSolve()
{
    for (auto& it : arrDisc) delete it;

    delete ptrSolveSecondPage;
    --iSinglControll;
}
