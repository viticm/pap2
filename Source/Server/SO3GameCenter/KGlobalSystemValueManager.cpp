#include "stdafx.h"
#include "KSO3GameCenter.h"
#include "Relay_GS_Protocol.h"

#define SECTION_GLOBAL_SYSTEM_VALUE "GlobalSystemValue"

#define SAVE_GLOBAL_SYSTEM_VALUE(name) \
    do                          \
    {                           \
        if (m_##name.bUpdate)   \
        {                       \
            g_pSO3GameCenter->m_MiscDB.DoSaveGlobalSystemValue(#name, m_##name.nValue); \
            m_##name.bUpdate = false;                                                   \
        }                                                                               \
    } while (false)

KGlobalSystemValueManager::KGlobalSystemValueManager()
{
    m_NextSaveTime      = 0;
    m_bNeedSyncToServer = false;
}

KGlobalSystemValueManager::~KGlobalSystemValueManager()
{
}

BOOL KGlobalSystemValueManager::Load()
{
    BOOL        bResult     = false;
    int         nRetCode    = false;	
    IIniFile*	piIniFile   = NULL;

    piIniFile = g_OpenIniFile(CENTER_CONST_FILENAME);
    KGLOG_PROCESS_ERROR(piIniFile);

    nRetCode = piIniFile->GetInteger(SECTION_GLOBAL_SYSTEM_VALUE, "MaxPlayerLevel", 0, &(m_MaxPlayerLevel.nValue));
    KGLOG_PROCESS_ERROR(nRetCode);

    bResult = true;
Exit0:
    KG_COM_RELEASE(piIniFile);
    return bResult;
}

void KGlobalSystemValueManager::Activate()
{
    if (g_pSO3GameCenter->m_nTimeNow >= m_NextSaveTime)
    {
        SaveAll();
        m_NextSaveTime = g_pSO3GameCenter->m_nTimeNow + 5;
    }

    if (m_bNeedSyncToServer)
    {
        g_pSO3GameCenter->m_GameServer.DoSyncGlobalSystemValue(-1);
        m_bNeedSyncToServer = false;
    }
}

void KGlobalSystemValueManager::SaveAll()
{
    SAVE_GLOBAL_SYSTEM_VALUE(GameWorldStartTime);
    SAVE_GLOBAL_SYSTEM_VALUE(MaxPlayerLevel);
    SAVE_GLOBAL_SYSTEM_VALUE(GameWorldChargeTime);
}

DEFINE_LUA_CLASS_BEGIN(KGlobalSystemValueManager)

    REGISTER_LUA_INTEGER(KGlobalSystemValueManager, GameWorldStartTime)
    REGISTER_LUA_INTEGER(KGlobalSystemValueManager, MaxPlayerLevel)
    REGISTER_LUA_INTEGER(KGlobalSystemValueManager, GameWorldChargeTime)

DEFINE_LUA_CLASS_END(KGlobalSystemValueManager)

