#include "error.h"
#include "config.h"

void FError::ErrorInFileNotFind(wstring wsName)
{
	ofstream out(fConfig->wsNameLogFile);
	out << "Не обнаружен каталог с входными данным " + fGlobal->ConwertToString(wsName);
	out << END;
	out.close();
}

void FError::ErrorOutFileNotFind(wstring wsName)
{
	ofstream out(fConfig->wsNameLogFile);
	out << "Не обнаружен каталог для вывода данных" + fGlobal->ConwertToString(wsName);
	out << END;
	out.close();
}