#include "error.h"
#include "config.h"

void FError::ErrorInFileNotFind(wstring wsName)
{
	ofstream out(fConfig->wsNameLogFile);
	out << "�� ��������� ������� � �������� ������ " + fGlobal->ConwertToString(wsName);
	out << END;
	out.close();
}

void FError::ErrorOutFileNotFind(wstring wsName)
{
	ofstream out(fConfig->wsNameLogFile);
	out << "�� ��������� ������� ��� ������ ������" + fGlobal->ConwertToString(wsName);
	out << END;
	out.close();
}