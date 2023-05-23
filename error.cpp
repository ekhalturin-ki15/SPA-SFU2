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
	ofstream out(ptrGlobal->ptrConfig->wsNameLogFile, std::ios::app);
	out << "Не обнаружен каталог с входными данным " + ptrGlobal->ConwertToString(wsName);
	out << END;
	out.close();
}

void FError::ErrorOutFileNotFind(wstring wsName)
{
	ofstream out(ptrGlobal->ptrConfig->wsNameLogFile, std::ios::app);
	out << "Не обнаружен каталог для вывода данных " + ptrGlobal->ConwertToString(wsName);
	out << END;
	out.close();
}

void FError::ErrorBadTree(string sName)
{
	ofstream out(ptrGlobal->ptrConfig->wsNameLogFile, std::ios::app);
	out << "В учебном плане " + sName + " неправильное дерево дисциплин";
	out << END;
	out.close();
}