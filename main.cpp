//Описание переменных в стиле unreal engine
// Префикс f - сложная структура
// Префикс s - строка (string)
// Префикс ws - строка UTF 16 (wstring)
// Префикс arr - массив элементов (vector)
// Префикс map - словарь (map)
// Префикс set - множество (set)
// Префикс list - список
// Префикс b - флаг (bool)
// Префикс i - число (int)
// Префикс d - вещественное число (double)
// Префикс ptr - указатель

#include "config.h"
#include "global.h"
#include "error.h"
#include "solve.h"
#include "outData.h"

#ifdef DEBUG
    #define _CRTDBG_MAP_ALLOC
    #include <stdlib.h>
    #include <crtdbg.h>
#endif

using namespace std;
//using namespace OpenXLSX;

//ofstream logFile;// ("logFile.txt"); //Сообщаем об ошибках пользователя
//ofstream debugFile("debugFile.txt"); //Для отладки



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

FGlobal* ptrGlobal; //Синглтон
bool Create();

void Delete()
{
    delete ptrGlobal;
}

bool Create()
{
    ptrGlobal = new FGlobal;
    if (!ptrGlobal->Init())
    {
        Delete();
        return false;
    }
    return true;
}

int main()
{
    try
    {
        if (!Create()) 
#ifdef DEBUG
            return 4; //Аварийное завершение
#else
            return 0; //Аварийное завершение
#endif

        if (ptrGlobal->ptrConfig->arrKeyPage.size() < 3)
        {
            ptrGlobal->ptrError->FatalErrorFewConfigPages(); // Не хватает данных для парсинга УП
            Delete();
#ifdef DEBUG
            return 3; //Аварийное завершение
#else
            return 0; //Аварийное завершение
#endif
        }

        auto fFile = filesystem::current_path(); //Взятие пути директории расположения exe файла

        for (int category = 0; category < ptrGlobal->ptrConfig->arrNameFileIn.size(); ++category)
        {
            auto fInFile = fFile / ptrGlobal->ptrConfig->arrNameFileIn[category];
            if (!filesystem::exists(fInFile))
            {
                ptrGlobal->ptrError->ErrorInFileNotFind(fInFile);
                continue; //Код ошибки - нет файлов указанного формата, из которых ожидалось считывание 
            }
            auto fOutFile = fFile / ptrGlobal->ptrConfig->arrNameFileOut[category];
            if (!filesystem::exists(fOutFile))
            {
                if (ptrGlobal->ptrConfig->bCreateFolder)
                {
                    try
                    {
                        filesystem::create_directories(fOutFile);
                    }
                    catch (...)
                    {
                        ptrGlobal->ptrError->ErrorOutFileCreate(fOutFile);
                    }
                }
                else
                {
                    ptrGlobal->ptrError->ErrorOutFileNotFind(fOutFile);
                    continue;
                    //exit(0); //Код ошибки - не удаётся создать папку для вывода
                }
            }

            for (auto it : filesystem::directory_iterator(fInFile))
            {
                if (!it.is_directory())
                {
                    if (ptrGlobal->ptrConfig->setIgnoreСurriculum.count(it.path().filename().string())) continue;
                    auto sOutName = fOutFile / it.path().filename();
                    ptrGlobal->ptrSolve->Read(it.path().string(), it.path().filename().string());                
                }
            }

            ptrGlobal->ptrSolve->CreateAllGraph();
            ptrGlobal->ptrSolve->CreateAllMetric(); // Метрики не связаны с графом

            auto sTotalOutName = fFile / ptrGlobal->ptrConfig->arrNameFileOut[category];// / "TotalData.xlsx";
            ptrGlobal->ptrOutData->Out(sTotalOutName.string());

        }

        Delete();
    }
    catch (...)
    {
        Delete();
#ifdef DEBUG
        return 2; //Аварийное завершение
#else
        return 0; //Аварийное завершение
#endif
    }


#ifdef DEBUG
    //Будет ругаться на Статические поля в error.h
    _CrtDumpMemoryLeaks();
#endif // DEBUG
}