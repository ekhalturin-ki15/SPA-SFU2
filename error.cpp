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
	out << "�� ��������� ������� � �������� ������ " + ptrGlobal->ConwertToString(wsName);
	out << END;
	out.close();
}

void FError::ErrorOutFileNotFind(wstring wsName)
{
	ofstream out(ptrGlobal->ptrConfig->wsNameLogFile, std::ios::app);
	out << "�� ��������� ������� ��� ������ ������ " + ptrGlobal->ConwertToString(wsName);
	out << END;
	out.close();
}

void FError::ErrorBadTree(string sName)
{
	ofstream out(ptrGlobal->ptrConfig->wsNameLogFile, std::ios::app);
	out << "� ������� ����� " + sName + " ������������ ������ ���������";
	out << END;
	out.close();
}