// Описание переменных в стиле unreal engine
//  Префикс f - сложная структура
//  Префикс s - строка (string)
//  Префикс ws - строка UTF 16 (wstring)
//  Префикс arr - массив элементов (vector)
//  Префикс map - словарь (map)
//  Префикс set - множество (set)
//  Префикс list - список
//  Префикс b - флаг (bool)
//  Префикс i - число (int)
//  Префикс d - вещественное число (double)
//  Префикс ptr - указатель

#include "adapOutData.h"
#include "config.h"
#include "error.h"
#include "global.h"
#include "outData.h"
#include "solve.h"

#pragma warning(disable:4267) // size_t лучше не использовать, 
//так как обратный цикл (с шагом -1) может стать бесконечным

using namespace std;
// using namespace OpenXLSX;

#define CONF_FILE L"config.xlsx"


bool Create(shared_ptr<FGlobal>& _ptrGlobal, wstring wsNameConfig);
bool Create(shared_ptr<FGlobal>& _ptrGlobal, wstring wsNameConfig)
{
    _ptrGlobal = make_shared<FGlobal>(wsNameConfig);
    if (!_ptrGlobal->Init(_ptrGlobal))
    {
        _ptrGlobal.reset();
        return false;
    }
    return true;
}

int main()
{
    try
    {
        shared_ptr<FGlobal> ptrGlobal;

        if (!Create(ptrGlobal, CONF_FILE))
#ifdef DEBUG
            return 4;    // Аварийное завершение
#else
            return 0;        // Аварийное завершение
#endif

        if (ptrGlobal->ptrConfig->GetSizeKeyPage() < 3)
        {
            ptrGlobal->ptrError
                ->FatalErrorFewConfigPages();    // Не хватает данных для
                                                 // парсинга УП
            // Delete();
#ifdef DEBUG
            return 3;    // Аварийное завершение
#else
            return 0;        // Аварийное завершение
#endif
        }

        filesystem::path fFile =
            filesystem::current_path();    // ptrGlobal->GetCurrentPath();
                                           // // Взятие пути директории
                                           // расположения exe файла

        for (int category = 0;
             category < ptrGlobal->ptrConfig->GetArrNameFileIn().size();
             ++category)
        {
            filesystem::path fInFile =
                fFile / ptrGlobal->ptrConfig->GetArrNameFileIn()[category];
            if (!filesystem::exists(fInFile))
            {
                ptrGlobal->ptrError->ErrorInFileNotFind(fInFile);
                continue;    // Код ошибки - нет файлов указанного формата, из
                             // которых ожидалось считывание
            }
            auto fOutFile =
                fFile / ptrGlobal->ptrConfig->GetArrNameFileOut()[category];
            if (!filesystem::exists(fOutFile))
            {
                if (ptrGlobal->ptrConfig->GetBCreateFolder())
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
                    // exit(0); //Код ошибки - не удаётся создать папку для
                    // вывода
                }
            }

            for (auto it : filesystem::directory_iterator(fInFile))
            {
                if (!it.is_directory())
                {
                    if (ptrGlobal->ptrConfig->setIgnoreСurriculum.count(
                            it.path().filename().string()))
                        continue;
                    ptrGlobal->ptrSolve->Create(it.path().string(),
                                              it.path().filename().string());
                }
            }

            ptrGlobal->ptrSolve->CreateAllGraph();
            ptrGlobal->ptrSolve
                ->CreateAllMetric();    // Метрики не связаны с графом

            // Вывод всех тех предметов, для которых не указаны теги
            // (Гуманитарная,
            // естеств. общепроф. и т.д.)
            ptrGlobal->ptrError->OutDiscWithoutTag();

            auto sTotalOutName =
                fFile /
                ptrGlobal->ptrConfig
                    ->GetArrNameFileOut()[category];    // / "TotalData.xlsx";

            ptrGlobal->ptrAdapOutData->Create();
            ptrGlobal->ptrOutData->Create(sTotalOutName.string());

            // Пересоздание
            if (!ptrGlobal->ReCreate())
#ifdef DEBUG
                return 4;    // Аварийное завершение
#else
                return 0;    // Аварийное завершение
#endif
        }

        // Delete();
    }
    catch (...)
    {
        // Delete();
#ifdef DEBUG
        return 2;    // Аварийное завершение
#else
        return 0;            // Аварийное завершение
#endif
    }
}