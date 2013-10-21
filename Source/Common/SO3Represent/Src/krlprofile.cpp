#include "stdafx.h"
#include "./krlprofile.h"
#include "./common/kmathtools.h"

int InitProfile(KRLProfile* pRLProfile)
{
    int nResult = false;

    KGLOG_PROCESS_ERROR(pRLProfile);

    memset(pRLProfile, 0, sizeof(KRLProfile));

    pRLProfile->Items[KRLPROFILE_MAIN].szName =                 _T("main               ");
    pRLProfile->Items[KRLPROFILE_SKILL_EFFECT_RESULT].szName =  _T("skill effect result");
    pRLProfile->Items[KRLPROFILE_MISSILE].szName =              _T("missile            ");
    pRLProfile->Items[KRLPROFILE_CHARACTER].szName =            _T("character          ");
    pRLProfile->Items[KRLPROFILE_RIDES].szName =                _T("rides              ");
    pRLProfile->Items[KRLPROFILE_DOODAD].szName =               _T("doodad             ");
    pRLProfile->Items[KRLPROFILE_CHARACTER_GC].szName =         _T("character gc       ");
    pRLProfile->Items[KRLPROFILE_RIDES_GC].szName =             _T("rides gc           ");
    pRLProfile->Items[KRLPROFILE_DOODAD_GC].szName =            _T("doodad gc          ");
    pRLProfile->Items[KRLPROFILE_CAMERA].szName =               _T("camera             ");
    pRLProfile->Items[KRLPROFILE_CHARACTER_CAMERA].szName =     _T("  chracter camera  ");
    pRLProfile->Items[KRLPROFILE_CHARACTER_ALPHA].szName =      _T("  chracter alpha   ");
    pRLProfile->Items[KRLPROFILE_TRACK_CAMERA].szName =         _T("  track camera     ");
    pRLProfile->Items[KRLPROFILE_CURSOR].szName =               _T("cursor             ");
    pRLProfile->Items[KRLPROFILE_TARGET].szName =               _T("target             ");
    pRLProfile->Items[KRLPROFILE_MODEL_GC].szName =             _T("model gc           ");
    pRLProfile->Items[KRLPROFILE_REGION_INFO].szName =          _T("region info        ");
    pRLProfile->Items[KRLPROFILE_OPTIMIZATION].szName =         _T("optimization       ");
    pRLProfile->Items[KRLPROFILE_RESOURCE_LIMIT].szName =       _T("resource limit     ");

    nResult = true;
Exit0:
    return nResult;
}

int UnInitProfile(KRLProfile* pRLProfile)
{
    return true;
}

int ResetProfile(KRLProfile* pRLProfile)
{
    int nRetCode = false;
    int nResult = false;
    KRLProfileItem* pItem = NULL;

    KGLOG_PROCESS_ERROR(pRLProfile);

    for (int nIndex = 0; nIndex < KRLPROFILE_COUNT; ++nIndex)
    {
        pItem = pRLProfile->Items + nIndex;

        pItem->nCount = 0;
        pItem->fTotalTime = 0.0;
    }

    nResult = true;
Exit0:
    return nResult;
}

int PrintProfile(KRLProfile* pRLProfile)
{
    int nRetCode = false;
    int nResult = false;
    KRLProfileItem* pItem = NULL;

    KGLOG_PROCESS_ERROR(pRLProfile);

    KG_PROCESS_SUCCESS(!pRLProfile->bEnable);

    KG_PROCESS_SUCCESS((pRLProfile->dwPrintCount++ & 0x7F) != 0);

    printf("[RL] Profile:\n");

    for (int nIndex = 0; nIndex < KRLPROFILE_COUNT; ++nIndex)
    {
        pItem = pRLProfile->Items + nIndex;

        printf("\t%s\t%f, %d\n", pItem->szName, 
            pItem->nCount != 0 ? pItem->fTotalTime / pItem->nCount : 0.0, 
            pItem->nCount);
    }

    printf("\n");

Exit1:
    nResult = true;
Exit0:
    return nResult;
}

int StartProfile(KRLProfile* pRLProfile, KRLPROFILE nIndex)
{
    int nRetCode = false;
    int nResult = false;
    KRLProfileItem* pItem = NULL;

    KGLOG_PROCESS_ERROR(pRLProfile);

    KG_PROCESS_SUCCESS(!pRLProfile->bEnable);

    pItem = pRLProfile->Items + nIndex;

    pItem->fStartTime = GetPerformanceTickCount();

Exit1:
    nResult = true;
Exit0:
    return nResult;
}

int StopProfile(KRLProfile* pRLProfile, KRLPROFILE nIndex)
{
    int nRetCode = false;
    int nResult = false;
    KRLProfileItem* pItem = NULL;

    KGLOG_PROCESS_ERROR(pRLProfile);

    KG_PROCESS_SUCCESS(!pRLProfile->bEnable);

    pItem = pRLProfile->Items + nIndex;

    pItem->fTotalTime += GetPerformanceTickCount() - pItem->fStartTime;

    ++pItem->nCount;

Exit1:
    nResult = true;
Exit0:
    return nResult;
}
