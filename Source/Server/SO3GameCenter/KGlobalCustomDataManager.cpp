#include "stdafx.h"
#include "KSO3GameCenter.h"
#include "KGlobalCustomDataManager.h"
#include "Engine/KLuaPackage.h"

KGlobalCustomDataManager::KGlobalCustomDataManager()
{
    m_nNextSaveTime         = 0;
}

BOOL KGlobalCustomDataManager::Init()
{
    return true;
}

void KGlobalCustomDataManager::UnInit()
{
    for (KCUSTOM_DATA_TABLE::iterator it = m_DataTable.begin(); it != m_DataTable.end(); ++it)
    {
        KMEMORY_FREE(it->second);
    }
    m_DataTable.clear();
}

void KGlobalCustomDataManager::Activate()
{
    if (g_pSO3GameCenter->m_nTimeNow > m_nNextSaveTime)
    {
        SaveAll();
        m_nNextSaveTime = g_pSO3GameCenter->m_nTimeNow + 8;
    }
}

BOOL KGlobalCustomDataManager::Add(const char cszName[], BYTE* byData, size_t uDataLen, BOOL bSaved)
{
    BOOL                                                    bResult      = false;
    BOOL                                                    bRetCode     = false;
    KCUSTOM_DATA*                                           pCustomData  = NULL;
    std::pair<KCUSTOM_DATA_TABLE::iterator, bool>           InsRet;

    pCustomData = (KCUSTOM_DATA*)KMemory::Alloc(sizeof(KCUSTOM_DATA) + uDataLen);
    KGLOG_PROCESS_ERROR(pCustomData);

    pCustomData->bSaved     = bSaved;
    pCustomData->uDataLen   = uDataLen;

    memcpy(pCustomData->byData, byData, uDataLen);

    InsRet = m_DataTable.insert(std::make_pair(cszName, pCustomData));
    KGLOG_PROCESS_ERROR(InsRet.second);
   
    bResult = true;
Exit0:
    if (!bResult)
    {
        KMEMORY_FREE(pCustomData);
    }
    return bResult;
}

void KGlobalCustomDataManager::SaveAll()
{
    BOOL        bResult = false;

    for (KCUSTOM_DATA_TABLE::iterator it = m_DataTable.begin(); it != m_DataTable.end(); ++it)
    {
        KCUSTOM_DATA* pCustomData = it->second;

        assert(pCustomData);

        if (!pCustomData->bSaved)
        {
            g_pSO3GameCenter->m_MiscDB.DoSaveGlobalCustomData(
                it->first.c_str(), 
                pCustomData->byData, 
                pCustomData->uDataLen
            );
            pCustomData->bSaved = true;
        }
    }

    return;
}

int KGlobalCustomDataManager::LuaSet(Lua_State* L)
{
    BOOL                            bRetCode    = false;
    int                             nParamCount = 0;
    const char*                     pszName     = NULL;
    size_t                          uNameLen    = 0;
    BYTE*                           pbyEnd      = NULL;
    KCUSTOM_DATA_TABLE::iterator    it;

    nParamCount = Lua_GetTopIndex(L);
    KGLOG_PROCESS_ERROR(nParamCount == 2);

    pszName = Lua_ValueToString(L, 1);
    KGLOG_PROCESS_ERROR(pszName);

    uNameLen = strlen(pszName);
    KGLOG_PROCESS_ERROR(uNameLen < _NAME_LEN);

    it = m_DataTable.find(pszName);
    if (it != m_DataTable.end())
    {
        KMEMORY_FREE(it->second);
        m_DataTable.erase(it);
    }

    if (lua_isnil(L, 2))
    {   
        g_pSO3GameCenter->m_MiscDB.DoDeleteGlobalCustomData(pszName);
        goto Exit0;
    }

    pbyEnd = LuaPackup(L, 2, m_byOriginalBuffer, sizeof(m_byOriginalBuffer));
    KGLOG_PROCESS_ERROR(pbyEnd);

    bRetCode = Add(pszName, m_byOriginalBuffer, (size_t)(pbyEnd - m_byOriginalBuffer), false);
    KGLOG_PROCESS_ERROR(bRetCode);

Exit0:
    return 0;
}

int KGlobalCustomDataManager::LuaGet(Lua_State* L)
{
    int                             nResult     = 0;
    int                             nParamCount = 0;
    const char*                     pszName     = NULL;
    BYTE*                           pbyEnd      = NULL;
    KCUSTOM_DATA_TABLE::iterator    it;

    nParamCount = Lua_GetTopIndex(L);
    KGLOG_PROCESS_ERROR(nParamCount == 1);

    pszName =  Lua_ValueToString(L, 1);
    KGLOG_PROCESS_ERROR(pszName);

    it = m_DataTable.find(pszName);
    KG_PROCESS_ERROR(it != m_DataTable.end());

    pbyEnd = LuaUnpack(L, it->second->byData, it->second->uDataLen);
    KGLOG_PROCESS_ERROR(pbyEnd);

    nResult = 1;
Exit0:
    return nResult;
}

DEFINE_LUA_CLASS_BEGIN(KGlobalCustomDataManager)
    REGISTER_LUA_FUNC(KGlobalCustomDataManager, Set)
    REGISTER_LUA_FUNC(KGlobalCustomDataManager, Get)
DEFINE_LUA_CLASS_END(KGlobalCustomDataManager)
