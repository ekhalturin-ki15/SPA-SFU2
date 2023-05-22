#pragma once
#include "global.h"

struct FGlobal;

struct FError
{

    FError(FGlobal* _ptrGlobal);
    void Init();
        
    void ErrorInFileNotFind(wstring wsName);
    void ErrorOutFileNotFind(wstring wsName);


    FGlobal* ptrGlobal; //Синглтон
};