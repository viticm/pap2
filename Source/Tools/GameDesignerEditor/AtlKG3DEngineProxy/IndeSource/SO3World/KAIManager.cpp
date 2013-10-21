////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KAIManager.cpp 
//  Version     : 1.0
//  Creator     : Chen Jie, zhaochunfeng
//  Comment     : 管理各种AI类型数据
//
////////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "KAIAction.h"
#include "KAIManager.h"
#include "KSO3World.h"
#include "KAILogic.h"

BOOL KAIManager::Init()
{
    BOOL bResult  = false;
    BOOL bRetCode = false;

    RegisterActionFunctions();

#ifdef _SERVER
    KGLogPrintf(KGLOG_DEBUG, "[AI] loading ... ...");

    bRetCode = LoadAITabFile();
    KGLOG_PROCESS_ERROR(bRetCode);

    KGLogPrintf(KGLOG_DEBUG, "[AI] %u AI loaded !", m_AITable.size());
#endif

    bResult = true;
Exit0:
    return bResult;
}

void KAIManager::UnInit()
{
    for (KAI_TABLE::iterator it = m_AITable.begin(); it != m_AITable.end(); ++it)
    {
        if (it->second.pLogic)
        {
            KMemory::Delete(it->second.pLogic);
            it->second.pLogic = NULL;
        }
    }

    m_AITable.clear();
}

BOOL KAIManager::LoadAITabFile()
{
    BOOL        bResult                 = false;
    int         nRetCode                = false;
    ITabFile*   piAITabFile             = NULL;
    char        szFilePath[]            = SETTING_DIR"/AIType.tab";
    int         nHeight                 = 0;
    std::pair<KAI_TABLE::iterator, bool> RetPair;

    piAITabFile = g_OpenTabFile(szFilePath);
    if (!piAITabFile)
    {
        KGLogPrintf(KGLOG_ERR, "[AI] Unable to open table file \"%s\"\n", szFilePath);
        goto Exit0;
    }

    nHeight = piAITabFile->GetHeight();
    KGLOG_PROCESS_ERROR(nHeight > 1);

    for (int nRow = 2; nRow <= nHeight; nRow++)
    {
        int                 nAIType         = 0;
        DWORD               dwScriptID      = 0;
        char                szScriptPath[MAX_PATH];
        KAIInfo             AIInfo;

        nRetCode = piAITabFile->GetInteger(nRow, "AIType", 0, &nAIType);
        KGLOG_PROCESS_ERROR(nRetCode);
        KGLOG_PROCESS_ERROR(nAIType >= 0);

        nRetCode = piAITabFile->GetString(nRow, "ScriptFile", "", szScriptPath, sizeof(szScriptPath));
        KGLOG_PROCESS_ERROR(nRetCode);

        dwScriptID = g_FileNameHash(szScriptPath);
        KGLOG_PROCESS_ERROR(dwScriptID);

        AIInfo.dwScriptID = dwScriptID;
        AIInfo.pLogic     = NULL;

        m_AITable[nAIType] = AIInfo;
    }

#ifdef _SERVER
    if (!g_pSO3World->m_bFastBootMode)
    {
        KAI_TABLE::iterator it = m_AITable.begin();
        int nCount = (int)m_AITable.size();
        int nIndex = 1;

        for (;it != m_AITable.end(); ++it, ++nIndex)
        {
            #if (defined(_MSC_VER) || defined(__ICL))   //WIN32
            cprintf("******************************>: Setup AI scripts : %d/%d\r", nIndex, nCount);
            #endif
            it->second.pLogic   = CreateAI(it->first, it->second.dwScriptID);
        }
        
        #if (defined(_MSC_VER) || defined(__ICL))   //WIN32
        cprintf("\n");
        #endif
    }
#endif

    bResult = true;
Exit0:
    if (!bResult)
    {
        m_AITable.clear();
    }
    KG_COM_RELEASE(piAITabFile);
    return bResult;
}

BOOL KAIManager::ReloadAILogic(int nAIType)
{
    BOOL                bResult                 = false;
    int                 nRetCode                = false;
    KAILogic*           pLogic                  = NULL;
    ITabFile*           piAITabFile             = NULL;
    char                szFilePath[]            = SETTING_DIR"/AIType.tab";
    int                 nHeight                 = 0;
    DWORD               dwScriptID              = 0;
    int                 nRow                    = 0;
    int                 nReadAIType             = 0;
    KAI_TABLE::iterator it;
    char                szScriptPath[MAX_PATH];

    piAITabFile = g_OpenTabFile(szFilePath);
    KGLOG_PROCESS_ERROR(piAITabFile);

    nHeight = piAITabFile->GetHeight();
    KGLOG_PROCESS_ERROR(nHeight > 1);

    for (nRow = 2; nRow <= nHeight; nRow++)
    {
        nRetCode = piAITabFile->GetInteger(nRow, "AIType", 0, &nReadAIType);
        KGLOG_PROCESS_ERROR(nRetCode == 1);

        if (nReadAIType != nAIType)
            continue;

        nRetCode = piAITabFile->GetString(nRow, "ScriptFile", "", szScriptPath, sizeof(szScriptPath));
        KGLOG_PROCESS_ERROR(nRetCode == 1);

        break;
    }

    KGLOG_PROCESS_ERROR(nRow <= nHeight);

    nRetCode = g_pSO3World->m_ScriptCenter.ReloadScript(szScriptPath);
    KGLOG_PROCESS_ERROR(nRetCode);

    dwScriptID = g_FileNameHash(szScriptPath);
    KGLOG_PROCESS_ERROR(dwScriptID);

    it = m_AITable.find(nAIType);
    if (it == m_AITable.end() || it->second.pLogic == NULL)
    {
        m_AITable[nAIType].dwScriptID   = dwScriptID;
        m_AITable[nAIType].pLogic       = CreateAI(nAIType, dwScriptID);
    }
    else
    {
        it->second.dwScriptID = dwScriptID;

        nRetCode = it->second.pLogic->Setup(nAIType, dwScriptID);
        KGLOG_PROCESS_ERROR(nRetCode);
    }

    bResult = true;
Exit0:
    return bResult;
}

KAILogic* KAIManager::GetAILogic(int nAIType)
{
    KAILogic*           pLogic  = NULL;
    KAIInfo*            pInfo   = NULL;
    KAI_TABLE::iterator it      = m_AITable.find(nAIType);

    KGLOG_PROCESS_ERROR(it != m_AITable.end());

    pInfo = &it->second;

    if (!pInfo->pLogic)
    {
        pInfo->pLogic = CreateAI(nAIType, pInfo->dwScriptID);
    }

    pLogic = pInfo->pLogic;
Exit0:
    return pLogic;
}

KAI_ACTION_FUNC KAIManager::GetActionFunction(int nKey)
{
    if (nKey > eakInvalid && nKey < eakTotal)
    {
        return m_ActionFunctionTable[nKey];
    }

    return NULL;
}

KAILogic* KAIManager::CreateAI(int nType, DWORD dwScriptID)
{
    KAILogic* pResult       = NULL;
    BOOL      bRetCode      = false;
    KAILogic* pAI           = NULL;

    pAI = KMemory::New<KAILogic>();
    KGLOG_PROCESS_ERROR(pAI);

    bRetCode = pAI->Setup(nType, dwScriptID);
    KGLOG_PROCESS_ERROR(bRetCode);

    pResult = pAI;
Exit0:
    if (!pResult)
    {
        if (pAI)
        {
            KMemory::Delete(pAI);
            pAI = NULL;
        }

        KGLogPrintf(KGLOG_ERR, "[AI] Setup AI failed, AIType: %d", nType);
    }
    return pResult;
}

