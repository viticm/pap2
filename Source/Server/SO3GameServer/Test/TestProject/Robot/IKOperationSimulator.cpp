#include "stdafx.h"
#include "IKOperationSimulator.h"
#include "KOperationSimulator.h"

IKOperationSimulator *CreateSimulator(OPERATION_SIMULATOR_TYPE eType, KMonster *pOwner)
{
    int nRetCode = FALSE;
    IKOperationSimulator *piSimulator = NULL;

    ASSERT(pOwner);
    switch (eType)
    {
    case ostGeneral:
        {
            KOperationSimulator *pGeneral = new KOperationSimulator(pOwner);
            KG_PROCESS_ERROR(pGeneral);
            piSimulator = (IKOperationSimulator *)pGeneral;
        }        
        break;
    case ostTalk:
        break;
    default:
        piSimulator = NULL;
        break;
    }
Exit0:
    return piSimulator;
}

int DestroySimulator(IKOperationSimulator *piSimulator)
{
    if (piSimulator)
    {
        delete piSimulator;
        piSimulator = NULL;
    }
    return TRUE;
}