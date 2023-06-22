#include "metric.h"
#include "global.h"
#include "error.h"
#include "config.h"

#include "formulaParser.h"

//Инверсия зависимости
FMetric::FMetric(FTreeDisc* _ptrTree) : ptrTree(_ptrTree)
{
	if (_ptrTree->ptrGlobal->ptrConfig)
		fRegexHeaderComp = _ptrTree->ptrGlobal->ptrConfig->sRegexComp;

	mapAllowDisc = _ptrTree->GewMapAllowDisc(true, true);
}

void FMetric::Create()
{

}
