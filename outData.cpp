#include "outData.h"
#include "config.h"
#include "error.h"
#include "global.h"
#include "graph.h"
#include "metric.h"
#include "adapOutData.h"
#include "solve.h"

#include <iomanip>
#include <sstream>
int FOutData::iSinglControll = 0;

FOutData::FOutData(shared_ptr<FGlobal> _ptrGlobal)
    : ptrGlobal(_ptrGlobal)
{
    // Unit test против такого
#ifndef UNIT_TEST
    if (iSinglControll > 0)
        throw std::runtime_error("Re-creation Singleton");
#endif
    ++iSinglControll;
}

void FOutData::Create(string sOutPath)
{

}

bool FOutData::Init()
{
    return true;
}




FOutData::~FOutData()
{
    --iSinglControll;
}