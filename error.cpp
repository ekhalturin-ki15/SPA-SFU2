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