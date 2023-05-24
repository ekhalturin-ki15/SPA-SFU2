#include "error.h"
#include "config.h"

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

void FError::ErrorInFileNotFind(wstring wsName)
{
	ErrorInFileNotFind(ptrGlobal->ConwertToString(wsName));
}

void FError::ErrorInFileNotFind(string sName)
{
	ofstream out(ptrGlobal->ptrConfig->wsNameLogFile, std::ios::app);
	out << "Не обнаружен каталог с входными данным " +sName;
	out << END;
	out.close();
}

void FError::ErrorOutFileNotFind(wstring wsName)
{
	ErrorOutFileNotFind(ptrGlobal->ConwertToString(wsName));
}

void FError::ErrorOutFileNotFind(string sName)
{
	ofstream out(ptrGlobal->ptrConfig->wsNameLogFile, std::ios::app);
	out << "Не обнаружен каталог для вывода данных " + sName;
	out << END;
	out.close();
}

void FError::ErrorUncorrectExtension(wstring wsName)
{
	ErrorUncorrectExtension(ptrGlobal->ConwertToString(wsName));
}

void FError::ErrorUncorrectExtension(string sName)
{
	ofstream out(ptrGlobal->ptrConfig->wsNameLogFile, std::ios::app);
	out << "Программа работает только с расширение XLSX, другое у файла " + sName;
	out << END;
	out.close();
}

void FError::ErrorBadTree(wstring wsName)
{
	ErrorBadTree(ptrGlobal->ConwertToString(wsName));
}

void FError::ErrorBadTree(string sName)
{
	ofstream out(ptrGlobal->ptrConfig->wsNameLogFile, std::ios::app);
	out << "В учебном плане " + sName + " неправильное дерево дисциплин";
	out << END;
	out.close();
}

void FError::ErrorToMuchColums(wstring wsName)
{
	ErrorToMuchColums(ptrGlobal->ConwertToString(wsName));
}

void FError::ErrorToMuchColums(string sName)
{
	ofstream out(ptrGlobal->ptrConfig->wsNameLogFile, std::ios::app);
	out << "В учебном плане " + sName + " слишком много столбцов для определения индикаторов компетенций";
	out << END;
	out.close();
}

void FError::ErrorBadParser(wstring wsName)
{
	ErrorBadParser(ptrGlobal->ConwertToString(wsName));
}

void FError::ErrorBadParser(string sName)
{
	ofstream out(ptrGlobal->ptrConfig->wsNameLogFile, std::ios::app);
	out << "В учебном плане " + sName + " у одной из дисциплин не указан индекс";
	out << END;
	out.close();
}

void FError::ErrorBadParserName(wstring wsName, wstring wsIndexName)
{
	ErrorBadParserName(ptrGlobal->ConwertToString(wsName), ptrGlobal->ConwertToString(wsIndexName));
}

void FError::ErrorBadParserName(string sName, string sIndexName)
{
	ofstream out(ptrGlobal->ptrConfig->wsNameLogFile, std::ios::app);
	out << "В учебном плане " + sName + " нет имени у дисциплины с индексом " + sIndexName;
	out << END;
	out.close();
}

void FError::ErrorBadParserComp(wstring wsName, wstring wsIndexName)
{
	ErrorBadParserComp(ptrGlobal->ConwertToString(wsName), ptrGlobal->ConwertToString(wsIndexName));
}

void FError::ErrorBadParserComp(string sName, string sIndexName)
{
	ofstream out(ptrGlobal->ptrConfig->wsNameLogFile, std::ios::app);
	out << "В учебном плане " + sName + " не указаны компетенции у дисциплины с индексом " + sIndexName;
	out << END;
	out.close();
}


void FError::OKParsing(wstring wsName)
{
	OKParsing(ptrGlobal->ConwertToString(wsName));
}

void FError::OKParsing(string sName)
{
	ofstream out(ptrGlobal->ptrConfig->wsNameLogFile, std::ios::app);
	out << "+ Учебный план " + sName + " успешно обработан";
	out << END;
	out.close();
}