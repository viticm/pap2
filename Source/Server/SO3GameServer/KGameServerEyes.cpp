
/************************************************************************/
/* Copyright : Kingsoft 2008										    */
/* Author	 : Zhao chunfeng, Xia yong								    */
/* FSEyes м╗педё©И                                                      */
/************************************************************************/

#include "stdafx.h"
#include "SO3GlobalDef.h"
#include "KGameServerEyes.h"
#include "KSO3World.h"
#include "KSO3GameServer.h"
#include "FSEyes/fseye_protocol.h"
#include "KRelayClient.h"
#include "KPlayerServer.h"
#include "KScene.h"

#define REGISTER_GSEYES_FUNCTION(id, func)                      \
    do                                                          \
    {                                                           \
        m_ProtocolFunctions[id] = &KGameServerEyes::func;       \
    } while(false)

#define TEMP_DATA_SIZE 1024 * 1024

BOOL GetSelfName(char* pszBuffer, size_t uBufferSize)
{
    BOOL    bResult  = false;
    int     nRetCode = false;

#ifdef _WIN32
    nRetCode = (int)GetModuleFileName(NULL, pszBuffer, (DWORD)uBufferSize);
    KGLOG_PROCESS_ERROR(nRetCode);
#else
    nRetCode = readlink("/proc/self/exe", pszBuffer, uBufferSize);
    KGLOG_PROCESS_ERROR(nRetCode > 0 && nRetCode < (int)uBufferSize);
    pszBuffer[nRetCode] = '\0';
#endif

    bResult = true;
Exit0:
    return bResult;
}

KGameServerEyes::KGameServerEyes():m_pbyTempData(NULL){}

BOOL KGameServerEyes::Init()
{
    BOOL         bResult            = false;
    BOOL         bRetCode           = false;
    BOOL         bStreamInitFlag    = false;
    int          nGardIndex         = 0;
    int          nEyesIndex         = 0;
    IIniFile*	 piIniFile          = NULL;

    bRetCode = GetSelfName(m_szSelfName, sizeof(m_szSelfName));
    KGLOG_PROCESS_ERROR(bRetCode);

    piIniFile = g_OpenIniFile(GS_SETTINGS_FILENAME);
	KGLOG_PROCESS_ERROR(piIniFile);
    
    bRetCode = piIniFile->GetInteger("Eyes", "EyesIndex", 0, &nEyesIndex);
    KGLOG_PROCESS_ERROR(bRetCode);
    KGLOG_PROCESS_ERROR(nEyesIndex >= 0 && nEyesIndex < MAX_GS_COUNT);

    nGardIndex = emKGUARDSERVER_FIRSTID + emKGUARDSERVER_GAMESERVER01 + nEyesIndex;

    bRetCode = m_Stream.Initialize(nGardIndex, 200 * 1024);
    KGLOG_PROCESS_ERROR(bRetCode);
    bStreamInitFlag = true;

    memset(m_ProtocolFunctions, 0, sizeof(m_ProtocolFunctions));

    REGISTER_GSEYES_FUNCTION(e2l_getpid_def, OnE2LGetPid);
    REGISTER_GSEYES_FUNCTION(e2l_exit_def, OnE2LStop);
    REGISTER_GSEYES_FUNCTION(g2l_header_def, OnG2LProtocol);

    m_nNextPingTime             = 0;
    m_nNextSendPlayerTime       = 0;
    m_nNextSendCustomInfoTime   = 0;
    m_nNextSendMapInfoTime      = 0;
    m_fLastPerformance          = 0.0f;
    m_fLastClientFlux           = 0.0f;

    m_pbyTempData = new BYTE[TEMP_DATA_SIZE];
    KGLOG_PROCESS_ERROR(m_pbyTempData);

    bResult = true;
Exit0:
    if (!bResult)
    {
        if (m_pbyTempData)
        {
            delete[] m_pbyTempData;
            m_pbyTempData = NULL;
        }

        if (bStreamInitFlag)
        {
            m_Stream.UnInitialize();
            bStreamInitFlag = false;
        }
    }
    KG_COM_RELEASE(piIniFile);
    return bResult;
}

void KGameServerEyes::UnInit()
{
    if (m_pbyTempData)
    {
        delete[] m_pbyTempData;
        m_pbyTempData = NULL;
    }

    m_Stream.UnInitialize();
}

void KGameServerEyes::Activate()
{
    size_t              nDataLen            = 0;
	char*               pData               = NULL;
    BYTE                byProtocol          = 0;
    PROTOCOL_FUNCTION   pfnProtocolFunction = NULL;
    
    while (true)
    {
        pData = m_Stream.GetPack(nDataLen);

        if (pData == NULL)
        {
            break;
        }
        
        assert(nDataLen > 0);
        
        byProtocol = *(BYTE*)pData;
        
        pfnProtocolFunction = m_ProtocolFunctions[byProtocol];
        if (pfnProtocolFunction == NULL)
        {
            m_Stream.PopPack();
            continue;
        }

        (this->*pfnProtocolFunction)((BYTE*)pData, nDataLen);
        m_Stream.PopPack();
    }
    
    assert(g_pSO3World);
    if (g_pSO3World->m_nCurrentTime >= m_nNextPingTime)
    {
        Ping();
        m_nNextPingTime = g_pSO3World->m_nCurrentTime + 20;
    }

    if (g_pSO3World->m_nCurrentTime >= m_nNextSendPlayerTime)
    {
        SendPlayerCount();
        m_nNextSendPlayerTime = g_pSO3World->m_nCurrentTime + 3;
    }

    if (g_pSO3World->m_nCurrentTime >= m_nNextSendCustomInfoTime)
    {
        SendCustomInfo();
        m_nNextSendCustomInfoTime = g_pSO3World->m_nCurrentTime + 2;
    }

    if (g_pSO3World->m_nCurrentTime >= m_nNextSendMapInfoTime)
    {
        SendMapInfo();
        m_nNextSendMapInfoTime = g_pSO3World->m_nCurrentTime + 30;
    }
}

void KGameServerEyes::OnE2LGetPid(BYTE* pbyData, size_t uDataLen)
{
    l2e_getpid  sMsg;

	sMsg.wProtocol = l2e_getpid_def;
	sMsg.wServer   = 0;
	sMsg.nPid	   = getpid();
	
    m_Stream.PutPack(&sMsg, sizeof(sMsg));
    
    return;
}

void KGameServerEyes::OnE2LStop(BYTE* pbyData, size_t uDataLen)
{
    KGLogPrintf(KGLOG_INFO, "Service stoped by kingeyes !\n");
    g_RelayClient.PrepareToQuit();
}

void KGameServerEyes::OnG2LProtocol(BYTE* pbyData, size_t uDataLen)
{
	KSubProtocolHead*   pHead   = (KSubProtocolHead*)pbyData;

	if (pHead->wSubProtocol == emKPROTOCOL_G2L_PIDREQ)
	{
        KPROTOCOL_L2G_PIDRET sRet;
        
        KGLOG_PROCESS_ERROR(uDataLen == sizeof(KSubProtocolHead));
		
        sRet.wProtocol		= l2g_header_def;
		sRet.wSubProtocol	= emKPROTOCOL_L2G_PIDRET;
		sRet.nPid			= getpid();
		sRet.wServer		= 0;
		m_Stream.PutPack(&sRet, sizeof(sRet));
	}

Exit0:
    return;
}

void KGameServerEyes::Ping()
{
	KSubProtocolHead    sPing;

	sPing.wProtocol		= l2e_header_def;
	sPing.wSubProtocol	= l2e_ping_def;
	sPing.wServer		= 0;
	
    m_Stream.PutPack(&sPing, sizeof(sPing));

    return;
}

void KGameServerEyes::SendPlayerCount()
{
	char                szBuff[sizeof(l2e_PlayerCount) + sizeof(int) * emKGUARD_PLAYERCOUNT_COUNT];
	l2e_PlayerCount*    pProtocol   = (l2e_PlayerCount*)szBuff;
    int*                pnCount     = NULL;

    KGLOG_PROCESS_ERROR(g_pSO3World);

	pProtocol->wProtocol	= l2e_header_def;
	pProtocol->wServer		= 0;
	pProtocol->wSubProtocol = l2e_PlayerCount_def;
	pProtocol->uCount		= emKGUARD_PLAYERCOUNT_COUNT;

	pnCount = (int*)(pProtocol + 1);

	for (int i = 0; i < emKGUARD_PLAYERCOUNT_COUNT; i++)
	{
		pnCount[i] = -1;
	}

    pnCount[emKGUARD_PLAYERCOUNT_TOTOAL] = (int)g_pSO3World->m_PlayerSet.GetObjCount();

	m_Stream.PutPack(szBuff, sizeof(szBuff));

Exit0:
    return;
}

/*
------------------GS%.2d-----------------------
Performance ... ... ... %.1f
Memory ... .... ... ... %.3f MB
Player ... .... ... ... %d 
Connection ... .... ... %d
Flux ... ... ... .. ... %.3f MB/s
-----------------------------------------------
IMG: /root/jx3server/gs1/SO3GameServerD[16354]
*/

void KGameServerEyes::SendCustomInfo()
{
    l2e_update_custom_info* pInfo               = NULL;
    size_t                  uStrLen             = 0;
    int                     nWorldIndex         = 0;
    size_t                  uMemory             = 0;
    int                     nPlayerCount        = 0;
    int                     nConnectionCount    = 0;
    float                   fClientFlux             = 0.0f;
    int                     nPID                = 0;
    float                   fUpTraffic          = 0.0f;
    float                   fDownTraffic        = 0.0f;

    assert(g_pSO3World);

    m_fLastPerformance  = m_fLastPerformance * 0.8f + g_pSO3World->m_nIdleFrame * 0.2f;
    nWorldIndex         = g_RelayClient.GetWorldIndex();
    uMemory             = KMemory::GetUsedSize();
    nPlayerCount        = (int)g_pSO3World->m_PlayerSet.GetObjCount();
    nConnectionCount    = g_PlayerServer.GetConnectionCount();
    fClientFlux         = (float)g_PlayerServer.GetNetworkFlux() / 1024.0f;
    nPID                = (int)getpid();
    fUpTraffic          = g_RelayClient.GetUpTraffic() / 1024.0f;
    fDownTraffic        = g_RelayClient.GetDownTraffic() / 1024.0f;

    m_fLastClientFlux = m_fLastClientFlux * 0.9f + (float)fClientFlux * 0.1f;
    
    pInfo = (l2e_update_custom_info*)m_pbyTempData;
    KGLOG_PROCESS_ERROR(pInfo);

    pInfo->wProtocol	= l2e_header_def;
    pInfo->wServer		= 0;
    pInfo->wSubProtocol = l2e_update_custom_info_def;

    uStrLen = snprintf(
        (char*)pInfo->byData, TEMP_DATA_SIZE - sizeof(l2e_update_custom_info),
        "------------------GS%.2d-----------------------\n"
        "Performance ... ... ... %.1f\n"
        "Memory ... .... ... ... %.3f MB\n"
        "Player ... .... ... ... %d\n"
        "Connection ... .... ... %d\n"
        "Client flux ... ... ... %.1f KB/s\n"
        "Center up traffic .. .. %.1f KB/s\n"
        "Center down traffic ... %.1f KB/s\n"
        "-----------------------------------------------\n"
        "IMG: %s[%d]\n",
        nWorldIndex,
        m_fLastPerformance,
        uMemory / 1024.0f / 1024.0f,
        nPlayerCount,
        nConnectionCount,
        m_fLastClientFlux,
        fUpTraffic,
        fDownTraffic,
        m_szSelfName,
        nPID
    );
    KGLOG_PROCESS_ERROR(uStrLen > 0);
    KGLOG_PROCESS_ERROR(uStrLen < TEMP_DATA_SIZE - sizeof(l2e_update_custom_info));

    pInfo->uDataLen = uStrLen;

    m_Stream.PutPack(pInfo, sizeof(l2e_update_custom_info) + uStrLen);

Exit0:
    return;
}

struct KGetRegionInfoFunc
{
    KGetRegionInfoFunc() : m_nActiveRegionCount(0), m_nNpcCount(0), m_nPlayerCount(0) { };

    BOOL operator () (KRegion* pRegion)
    {
        if (pRegion->IsActive())
        {
            ++m_nActiveRegionCount;
        }

        m_nNpcCount += pRegion->m_NpcList.GetNodeCount();
        m_nPlayerCount += pRegion->m_PlayerList.GetNodeCount();

        return true;
    }

    int m_nActiveRegionCount;
    int m_nNpcCount;
    int m_nPlayerCount;
};

struct KGetMapInfoFunc
{
	BOOL operator()(DWORD dwID, KScene* pScene);
    l2e_update_map_list* m_pMapList;
};

#define MAX_MAP_DESC_COUNT  ((TEMP_DATA_SIZE - sizeof(l2e_update_map_list)) / sizeof(l2e_update_map_list::KMapDesc))

BOOL KGetMapInfoFunc::operator()(DWORD dwID, KScene* pScene)
{   
    int                             nRegionHeight   = 0;
    int                             nRegionWidth    = 0;
    l2e_update_map_list::KMapDesc*  pMapDesc        = NULL;
    KGetRegionInfoFunc              GetRegionInfoFunc;
    
    if (pScene->m_eSceneState != ssNormal)
        return true;

    pMapDesc = &(m_pMapList->MapDesc[m_pMapList->nCount++]);
    
    pScene->TraverseRegion(GetRegionInfoFunc);
    
    nRegionHeight = pScene->GetRegionHeight();
    nRegionWidth  = pScene->GetRegionWidth();

    pMapDesc->dwMapID       = pScene->m_dwMapID;
    pMapDesc->nCopyIndex    = pScene->m_nCopyIndex;
    pMapDesc->nPlayerCount  = GetRegionInfoFunc.m_nPlayerCount;
    pMapDesc->nNpcCount     = GetRegionInfoFunc.m_nNpcCount;
    pMapDesc->nTotalRegion  = nRegionWidth * nRegionHeight;
    pMapDesc->nRegionActive = GetRegionInfoFunc.m_nActiveRegionCount;

    strncpy(pMapDesc->szName, pScene->m_szName, sizeof(pMapDesc->szName));
	pMapDesc->szName[sizeof(pMapDesc->szName) - 1] = '\0';

    if (m_pMapList->nCount >= MAX_MAP_DESC_COUNT)
        return false;

	return true;
}

void KGameServerEyes::SendMapInfo()
{
    l2e_update_map_list*    pMapList        = (l2e_update_map_list*)m_pbyTempData;
    KGetMapInfoFunc         GetMapInfoFunc;

    assert(m_pbyTempData);
    assert(g_pSO3World);
    
    pMapList->wProtocol	    = l2e_header_def;
    pMapList->wServer		= 0;
    pMapList->wSubProtocol  = l2e_update_map_list_def;
    pMapList->nCount        = 0;

    GetMapInfoFunc.m_pMapList = pMapList;
    g_pSO3World->m_SceneSet.Traverse(GetMapInfoFunc);
    
    m_Stream.PutPack(pMapList, sizeof(l2e_update_map_list) + pMapList->nCount * sizeof(l2e_update_map_list::KMapDesc));

    return;
}
