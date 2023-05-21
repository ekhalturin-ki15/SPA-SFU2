#include "config.h"
#include "global.h"

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

FConfig fConfig;


int main()
{
    //ptrGlobal = new Global;

    fConfig.Init(fGlobal.sNameConfig, fGlobal.sNamePage);

    auto fFile = filesystem::current_path(); //Взятие пути директории расположения exe файла


   /* XLDocument doc;
    doc.open("./0903010030-21.plm.plx.xlsx");
    auto wb = doc.workbook();
    

    auto st = wb.worksheetNames();
    for (auto s : st) cout << s << " ";
    

    auto wks = wb.worksheet(st[3]);
        for (auto& row : wks.rows()) {
            for (auto& et : row.cells())
            {
                cout << et.value().get<string>() << " ";
            }
            cout << "\n";
        }
    



    cout << " YES ";*/


    //delete ptrGlobal;
}