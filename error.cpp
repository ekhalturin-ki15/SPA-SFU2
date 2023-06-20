#include "error.h"
#include "config.h"
#include "solve.h"

const string FError::sBadTree = "Bad Tree";
const string FError::sDontHaveIndex = "Dont have index";
const string FError::sNotFoundKeyCol = "Key column not found";
const string FError::sNotEqualSum = "Lack of links between disciplines"; //Не хватает связей между дисциплинами
const string FError::sNotInitConfig = "Not init Config pointer";//Нужны данные с config, а он ещё не создан (например, такая ошибка может быть в FGlobal::HeightPage)

FError::FError(FGlobal* _ptrGlobal) : ptrGlobal(_ptrGlobal)
{
}

void FError::Init()
{
	if (ptrGlobal->ptrConfig->bReloadLogFile)
	{
		ofstream out(ptrGlobal->ptrConfig->wsNameLogFile);
		out << "";
		out.close();
	}
	else
	{
		ofstream out(ptrGlobal->ptrConfig->wsNameLogFile, std::ios::app);
		out << "--------------------------------";
		out << END;
		out.close();
	}
}

ofstream FError::OutHeader()
{
	ofstream out(ptrGlobal->ptrConfig->wsNameLogFile, std::ios::app);
	int& iCurrentPage = ptrGlobal->ptrSolve->iCurrentPage;
	int& iCurrentRow = ptrGlobal->ptrSolve->iCurrentRow;
	ptrGlobal->ptrSolve->bIsCorrectParsing = false;
	if (iCurrentPage < ptrGlobal->ptrConfig->arrKeyPage.size())
		out << ptrGlobal->ConwertToString(ptrGlobal->ptrConfig->arrKeyPage[iCurrentPage].wsName);
	else
		out << "! Страница не определена";

	out << " [строка " << iCurrentRow + 1 << "] : ";
	return out;
}

void FError::FatalErrorFewConfigPages()
{
	ofstream out(ptrGlobal->ptrConfig->wsNameLogFile, std::ios::app);
	out << "!! Не хватает указаний страниц для парсинга УП (их 3, обычно их названия: Компетенции(2), Компетенции, ПланСвод";
	out << END;
	out.close();
}
void FError::ErrorInFileNotFind(string sPathName)
{
	ofstream out(ptrGlobal->ptrConfig->wsNameLogFile, std::ios::app);
	out << "! Не обнаружен каталог с входными данным " + sPathName;
	out << END;
	out.close();
}

void FError::ErrorInFileNotFind(wstring wsPathName)
{
	ErrorInFileNotFind(ptrGlobal->ConwertToString(wsPathName));
}

void FError::ErrorOutFileCreate(string sPathName)
{
	ofstream out(ptrGlobal->ptrConfig->wsNameLogFile, std::ios::app);
	out << "! Не удаётся создать каталог " + sPathName;
	out << END;
	out.close();
}

void FError::ErrorOutFileCreate(wstring wsPathName)
{
	ErrorOutFileCreate(ptrGlobal->ConwertToString(wsPathName));
}

void FError::ErrorOutFileNotFind(string sPathName)
{
	ofstream out(ptrGlobal->ptrConfig->wsNameLogFile, std::ios::app);
	out << "Не обнаружен каталог для вывода данных " + sPathName;
	out << END;
	out.close();
}

void FError::ErrorOutFileNotFind(wstring wsPathName)
{
	ErrorOutFileNotFind(ptrGlobal->ConwertToString(wsPathName));
}

void FError::ErrorUncorrectExtension()
{
	ofstream out(ptrGlobal->ptrConfig->wsNameLogFile, std::ios::app);
	out << "! Программа работает только с расширение XLSX, другое у файла " + ptrGlobal->ptrSolve->sInPath;
	out << END;
	out.close();
}

void FError::ErrorBadTree()
{
	ofstream out = OutHeader();

	out << "! В учебном плане " + ptrGlobal->ptrSolve->sInPath + " неправильное дерево дисциплин";
	out << END;
	out.close();
}

void FError::ErrorNotFoundKeyCol()
{
	ofstream out = OutHeader();

	out << "! В учебном плане " + ptrGlobal->ptrSolve->sInPath + " не найдён ключевой столбец на странице";
	out << END;
	out.close();
}

void FError::ErrorNotEqualSum()
{
	ofstream out = OutHeader();

	out << "! В учебном плане " + ptrGlobal->ptrSolve->sInPath + " не совпали ЗЕ дисциплин в общем, и по семестрам";
	out << END;
	out.close();
}

void FError::ErrorToMuchColums()
{
	ofstream out = OutHeader();

	out << "В учебном плане " + ptrGlobal->ptrSolve->sInPath + " слишком много столбцов для определения индикаторов компетенций";
	out << END;
	out.close();
}

void FError::ErrorBadParser()
{
	ofstream out = OutHeader();

	out << "В учебном плане " + ptrGlobal->ptrSolve->sInPath + " у одной из дисциплин не указан индекс";
	out << END;
	out.close();
}

void FError::ErrorEmptyLine()
{
	ofstream out = OutHeader();

	out << "В учебном плане " + ptrGlobal->ptrSolve->sInPath + " присутствует пустая строка";
	out << END;
	out.close();
}

void FError::ErrorBadParserName(wstring wsIndexName)
{
	ErrorBadParserName(ptrGlobal->ConwertToString(wsIndexName));
}

void FError::ErrorBadParserName(string sIndexName)
{
	ofstream out = OutHeader();

	out << " В учебном плане " + ptrGlobal->ptrSolve->sInPath + " нет имени у дисциплины с индексом " + sIndexName;
	out << END;
	out.close();
}

void FError::ErrorBadParserComp(wstring wsIndexName)
{
	ErrorBadParserComp(ptrGlobal->ConwertToString(wsIndexName));
}

void FError::ErrorBadParserComp(string sIndexName)
{
	ofstream out = OutHeader();

	out << "В учебном плане " + ptrGlobal->ptrSolve->sInPath + " не указаны компетенции у дисциплины с индексом " + sIndexName;
	out << END;
	out.close();
}

void FError::ErrorBadIndicatorBind(wstring wsIndexName, wstring wsIndicator)
{
	ErrorBadIndicatorBind(ptrGlobal->ConwertToString(wsIndexName),
		ptrGlobal->ConwertToString(wsIndicator));
}

void FError::ErrorBadIndicatorBind(string sIndexName, string sIndicator)
{
	ofstream&& out = OutHeader();

	out << "В учебном плане " + ptrGlobal->ptrSolve->sInPath + " для дисциплины " + sIndexName + " не получается связать с индикатором " + sIndicator;
	out << END;
	out.close();
}

void FError::OKParsing()
{
	ofstream out(ptrGlobal->ptrConfig->wsNameLogFile, std::ios::app);

	out << "+ Учебный план " + ptrGlobal->ptrSolve->sInPath + " успешно обработан";
	out << END;
	out << END;
	out.close();
}


void FError::WAParsing()
{
	ofstream out(ptrGlobal->ptrConfig->wsNameLogFile, std::ios::app);

	out << "- Учебный план " + ptrGlobal->ptrSolve->sInPath + " обработан с недочётами";
	out << END;
	out << END;
	out.close();
}