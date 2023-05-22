#include "config.h"
#include "global.h"
#include "error.h"
#include "solve.h"

using namespace std;
//using namespace OpenXLSX;


ofstream logFile;// ("logFile.txt"); //Сообщаем об ошибках пользователя
ofstream debugFile("debugFile.txt"); //Для отладки



/// <summary>
/// Данная структура описывает учебный модуль 
/// (или дисциплину, если нет наследников)
/// 
/// В структуре перечислено:
/// id модуля, как в УП
/// название модуля
/// Компетенции и индексы компетенций
/// Входящие в модуль подмодули
/// ЗЕ по каждому семестру в отдельности и суммарно
/// </summary>
struct Disc
{
    string sId;
    string sName;

    map<string, set<int>> mapCompet;
    vector<Disc*> arrChild;

    int iSumZU;
    map<int, int> mapAllZU;

    Disc(string _sNoIdentity) :iSumZU(0), sName(_sNoIdentity),
        sId(_sNoIdentity) {}
};

FGlobal* fGlobal; //Синглтон

void Create()
{
    fGlobal = new FGlobal;
    //fError = new FError;
    //fConfig = new FConfig;
    //fSolve = new FSolve;
}

void Delete()
{
    delete fGlobal;
    //delete fError;
	//delete fConfig;
	//delete fSolve;
}

int main()
{
    Create();

    fGlobal->fConfig->Init(fGlobal->sNameConfig, fGlobal->sNamePage);
    fGlobal->fError->Init(); // Чтобы очистить лог файл

    auto fFile = filesystem::current_path(); //Взятие пути директории расположения exe файла

    for (int category = 0; category < fGlobal->fConfig->arrNameFileIn.size(); ++category)
    {
        auto fInFile = fFile / fGlobal->fConfig->arrNameFileIn[category];
        if (!filesystem::exists(fInFile))
        { 
            fGlobal->fError->ErrorInFileNotFind(fInFile);
            continue; //Код ошибки - нет файлов указанного формата, из которых ожидалось считывание 
        }
        auto fOutFile = fFile / fGlobal->fConfig->arrNameFileOut[category];
        if (!filesystem::exists(fOutFile))
        {
            if (fGlobal->fConfig->bCreateFolder)
            {
                filesystem::create_directory(fOutFile);
            }
            else
            {
                fGlobal->fError->ErrorOutFileNotFind(fOutFile);
                continue;
                //exit(0); //Код ошибки - не удаётся создать папку для вывода
            }
        }

        for (auto it : filesystem::directory_iterator(fInFile))
        {
            if (!it.is_directory())
            {
                auto sOutName = fOutFile / it.path().filename() / "";
                fGlobal->fSolve->Read(it.path().string(), sOutName.string());
            }
        }


    }


    Delete();
}