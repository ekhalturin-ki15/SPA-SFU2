#include "error.h"
#include "config.h"

FError::FError(FGlobal* _fGlobal) : fGlobal(_fGlobal)
{
}

void FError::Init()
{
	if (fGlobal->fConfig->bReloadLogFile)
	{
		ofstream out(fGlobal->fConfig->wsNameLogFile);
		out << "";
		out.close();
	}
	else
	{
		ofstream out(fGlobal->fConfig->wsNameLogFile, std::ios::app);
		out << "--------------------------------";
		out << END;
		out.close();
	}
}

void FError::ErrorInFileNotFind(wstring wsName)
{
	ofstream out(fGlobal->fConfig->wsNameLogFile, std::ios::app);
	out << "Не обнаружен каталог с входными данным " + fGlobal->ConwertToString(wsName);
	out << END;
	out.close();
}

void FError::ErrorOutFileNotFind(wstring wsName)
{
	ofstream out(fGlobal->fConfig->wsNameLogFile, std::ios::app);
	out << "Не обнаружен каталог для вывода данных " + fGlobal->ConwertToString(wsName);
	out << END;
	out.close();
}