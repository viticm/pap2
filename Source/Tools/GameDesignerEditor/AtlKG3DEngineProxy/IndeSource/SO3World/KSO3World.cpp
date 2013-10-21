#include "stdafx.h"
#include <time.h>

#ifdef WIN32
#include <conio.h>
#endif

#include "KMath.h"
#include "Engine/KMemory.h"
#include "KAttrModifier.h"
#include "KPlayerServer.h"
#include "KPlayerClient.h"
#include "KRelayClient.h"
#include "KSO3World.h"
#include "KRegion.h"
#include "KScene.h"
#include "KTerrainData.h"
#include "KPlayer.h"
#include "KNpc.h"
#include "KDoodad.h"
#include "Engine/KG_Time.h"

#if !defined(_SERVER)
#include "SO3Represent/SO3RepresentInterface.h"
#include "SO3UI/SO3GameWorldUIHandler.h"
#include "Engine/KTextFilter.h"
#endif // !defined(_SERVER)

#define CIRCLE_LIST_SIZE				256

#ifdef _CLIENT
HMODULE g_hRepresentDll = NULL;
HMODULE g_hUIDll = NULL;

IKSO3GameWorldUIHandler* g_pGameWorldUIHandler = NULL;
IKSO3GameWorldRepresentHandler* g_pGameWorldRepresentHandler = NULL;

void g_SetGameWorldUIHandler(IKSO3GameWorldUIHandler *pGameWorldUIHandler)
{
	g_pGameWorldUIHandler = pGameWorldUIHandler;
}

void g_SetGameWorldRepresentHandler(IKSO3GameWorldRepresentHandler *pGameWorldRepresentHandler)
{
	g_pGameWorldRepresentHandler = pGameWorldRepresentHandler;
}

_KMAIL::_KMAIL(
    DWORD dwID, char* pszSenderName, char* pszTitle, time_t lTime, 
    BYTE bUnRead, BYTE bMoney, BYTE bText, BYTE bItem, BYTE bSystem 
)
{
    assert(dwID);
    assert(pszSenderName);
    assert(pszTitle);

    dwMailID = dwID;

    strncpy(szSenderName, pszSenderName, sizeof(szSenderName));
    szSenderName[sizeof(szSenderName) - sizeof('\0')] = '\0';

    strncpy(szTitle, pszTitle, sizeof(szTitle));
    szTitle[sizeof(szTitle) - sizeof('\0')] = '\0';

    lReceiveTime    = lTime;

    bUnReadFlag     = bUnRead;
    bMoneyFlag      = bMoney;
    bTextFlag       = bText;
    bItemFlag       = bItem;
    bSystemFlag     = bSystem;
    bGotContentFlag = false;

    nMoney          = 0;
    szText[0]       = '\0';
    dwItemID        = ERROR_ID;
}

int _KMAIL::GetLeftSecond(int nMailSurvivalTime)
{
    int     nResult         = 0;
    time_t  nNowTime        = 0;
    time_t  nLeftSeconds    = 0;

    assert(g_pSO3World);

    nNowTime = g_pSO3World->m_nCurrentTime;
    
    nLeftSeconds = lReceiveTime + nMailSurvivalTime - nNowTime;
    
    if (nLeftSeconds > 0)
    {
        nResult = (int)nLeftSeconds;
    }

    return nResult;
}

#endif

KSO3World* g_pSO3World = NULL;

KSO3World::KSO3World()
{
    m_eWorldMode    = wmInvalid;
#ifdef _CLIENT
    m_piTextFilter  = NULL;
#endif
}

KSO3World::~KSO3World(void)
{
}

BOOL KSO3World::Init(IRecorderFactory* piFactory)
{
    BOOL    bResult                     = false;
	BOOL    bRetCode                    = false;
    BOOL    bSettingsInitFlag           = false;
    BOOL    bRoadManageInitFlag         = false;

	/*
    BOOL    bTeamInitFlag               = false;
    BOOL    bRelationMgrInitFlag        = false;
    BOOL    bScriptCenterInitFlag       = false;
    BOOL    bItemManagerInitFlag        = false;
    BOOL    bShopCenterInitFlag         = false;
    BOOL    bProfessionManagerInitFlag  = false;
    BOOL    bAIManageInitFlag           = false;
    BOOL    bDropCenterInitFlag         = false;
    BOOL    bSkillManagerInitFlag       = false;
    BOOL    bBuffManagerInitFlag        = false;
    BOOL    bTextFilterInitFlag         = false;
    BOOL    bCampInfoInitFlag           = false;
    BOOL    bPQInitFlag                 = false;
    BOOL    bTransListInitFlag          = false;
    DWORD   dwRandSeed                  = 0;
    IIniFile*   piIniFile               = NULL;
    
#ifdef _CLIENT
    BOOL    bTongClientInitFlag         = false;
#endif

    m_eWorldMode    = wmNormal;
    m_nBaseTime     = 0;
    m_dwStartTime   = 0;
    m_nStartLoop    = 0;
    m_nGameLoop     = 0;
    
    assert(piFactory);
    
    m_piRecorderTools = piFactory->CreateToolsInterface();
    KGLOG_PROCESS_ERROR(m_piRecorderTools);

    m_nCurrentTime  = m_piRecorderTools->GetTime();
    m_dwStartTime   = m_piRecorderTools->GetTickCount();

#ifdef _SERVER
    m_piSceneLoader = piFactory->CreateSceneLoader();
    KGLOG_PROCESS_ERROR(m_piSceneLoader);
    
    piIniFile = g_OpenIniFile(GS_SETTINGS_FILENAME);
	KGLOG_PROCESS_ERROR(piIniFile);

    bRetCode = piIniFile->GetInteger("Global", "FastBootMode", 0, &m_bFastBootMode);
    KGLOG_PROCESS_ERROR(bRetCode);
#endif
    
    g_RandomSeed(m_dwStartTime);

#ifdef _SERVER
    //bRetCode = m_PartyCache.Init();
    bRetCode = m_TeamServer.Init();
#endif

#ifdef _CLIENT
    bRetCode = m_TeamClient.Init();
#endif
    KGLOG_PROCESS_ERROR(bRetCode);
    bTeamInitFlag = true;

    bRetCode = m_FellowshipMgr.Init();
    KGLOG_PROCESS_ERROR(bRetCode);
    bRelationMgrInitFlag = true;
	*/

	bRetCode = m_Settings.Init();
	KGLOG_PROCESS_ERROR(bRetCode);
    bSettingsInitFlag = true;

	/*
	bRetCode = m_ScriptCenter.Init();
	KGLOG_PROCESS_ERROR(bRetCode);
    bScriptCenterInitFlag = true;

	bRetCode = InitAttributeFunctions();
	KGLOG_PROCESS_ERROR(bRetCode);

	bRetCode = m_ItemManager.Init();
	KGLOG_PROCESS_ERROR(bRetCode);
    bItemManagerInitFlag = true;

    bRetCode = m_ShopCenter.Init();
    KGLOG_PROCESS_ERROR(bRetCode);
    bShopCenterInitFlag = true;

	bRetCode = m_ProfessionManager.Init();
	KGLOG_PROCESS_ERROR(bRetCode);
    bProfessionManagerInitFlag = true;
	*/

    bRetCode = m_RoadManager.Init();
    KGLOG_PROCESS_ERROR(bRetCode);
    bRoadManageInitFlag = true;

	/*
    bRetCode = m_AIManager.Init();
    KGLOG_PROCESS_ERROR(bRetCode);
    bAIManageInitFlag = true;

    bRetCode = m_BuffManager.Init();
    KGLOG_CHECK_ERROR(bRetCode);
    bBuffManagerInitFlag = true;

    bRetCode = m_SkillManager.Init();
    KGLOG_PROCESS_ERROR(bRetCode);
    bSkillManagerInitFlag = true;

#ifdef _SERVER
	bRetCode = m_DropCenter.Init();
	KGLOG_PROCESS_ERROR(bRetCode);
    bDropCenterInitFlag = true;
#endif
    
    bRetCode = m_CampInfo.Init();
    KGLOG_PROCESS_ERROR(bRetCode);
    bCampInfoInitFlag = true;

    bRetCode = m_PQManager.Init();
    KGLOG_PROCESS_ERROR(bRetCode);
    bPQInitFlag = true;

#ifdef _CLIENT
    bRetCode = m_TongClient.Init();
    KGLOG_PROCESS_ERROR(bRetCode);
    bTongClientInitFlag = true;
#endif

#ifdef _SERVER
    bRetCode = m_TransmissionList.Init();
    KGLOG_PROCESS_ERROR(bRetCode);
    bTransListInitFlag = true;
#endif
	*/

#ifdef _CLIENT
	/*
	m_PlayerSet.SetPrefix(PLAYER_ID_PREFIX);
	m_NpcSet.SetPrefix(CLIENT_ID_PREFIX);
	m_DoodadSet.SetPrefix(CLIENT_ID_PREFIX);
	m_ItemSet.SetPrefix(CLIENT_ID_PREFIX);

    m_LinkItemQue.clear();
    m_ClientMailList.clear();
    m_ClientMailList.reserve(100);

    m_piTextFilter = CreateTextFilterInterface();
    KGLOG_PROCESS_ERROR(m_piTextFilter);

    bRetCode = m_piTextFilter->LoadFilterFile(SETTING_DIR"/TextFilter.txt");
    KGLOG_PROCESS_ERROR(bRetCode);

    g_pSO3World->m_dwBattleFieldPQID            = 0;
    g_pSO3World->m_dwBattleFieldPQTemplateID    = 0;
    g_pSO3World->m_nBattleFieldPQBeginTime      = 0;
    g_pSO3World->m_nBattleFieldPQEndTime        = 0;
    memset(g_pSO3World->m_nBattleFieldObjectiveCurrent, 0, sizeof(g_pSO3World->m_nBattleFieldObjectiveCurrent));
    memset(g_pSO3World->m_nBattleFieldObjectiveTarget, 0, sizeof(g_pSO3World->m_nBattleFieldObjectiveTarget));
	*/
#endif

	/*
#ifdef _SERVER
    m_bRunFlag             = true;
    m_nIdleFrame           = 0;
#endif

#ifdef _SERVER
    m_CenterRemote.Setup();
    m_ScriptServer.Setup();
#else
    m_ScriptClient.Setup();
#endif

#ifdef _CLIENT
    m_bForwardCharacterPosition = false;
#endif
	*/

	bResult = true;
Exit0:
    if (!bResult)
    {
		/*
#ifdef _CLIENT
        KG_COM_RELEASE(m_piTextFilter);
#endif

#ifdef _CLIENT
        if (bTongClientInitFlag)
        {
            m_TongClient.UnInit();
            bTongClientInitFlag = false;
        }
#endif
        if (bPQInitFlag)
        {
            m_PQManager.UnInit();
            bPQInitFlag = false;
        }

        if (bCampInfoInitFlag)
        {
            m_CampInfo.UnInit();
            bCampInfoInitFlag = false;
        }
#ifdef _SERVER
        if (bDropCenterInitFlag)
        {
            m_DropCenter.UnInit();
            bDropCenterInitFlag = false;
        }
#endif
        if (bBuffManagerInitFlag)
        {
            m_BuffManager.UnInit();
            bBuffManagerInitFlag = false;
        }

        if (bSkillManagerInitFlag)
        {
            m_SkillManager.UnInit();
            bSkillManagerInitFlag = false;
        }

        if (bAIManageInitFlag)
        {
            m_AIManager.UnInit();
            bAIManageInitFlag = false;
        }
		*/

        if (bRoadManageInitFlag)
        {
            m_RoadManager.UnInit();
            bRoadManageInitFlag = false;
        }
        
		/*
        if (bProfessionManagerInitFlag)
        {
            m_ProfessionManager.UnInit();
            bProfessionManagerInitFlag = false;
        }

        if (bShopCenterInitFlag)
        {
            m_ShopCenter.UnInit();
            bShopCenterInitFlag = false;
        }

        if (bItemManagerInitFlag)
        {
            m_ItemManager.UnInit();
            bItemManagerInitFlag = false;
        }        
        
        if (bScriptCenterInitFlag)
        {
            m_ScriptCenter.UnInit();
            bScriptCenterInitFlag = false;
        }
		*/

        if (bSettingsInitFlag)
        {
            m_Settings.UnInit();
            bSettingsInitFlag = false;
        }

		/*
        if (bRelationMgrInitFlag)
        {
            m_FellowshipMgr.UnInit();
            bRelationMgrInitFlag = false;
        }

        if (bTeamInitFlag)
        {
#ifdef _SERVER
            m_TeamServer.UnInit();
#endif
#ifdef _CLIENT
            m_TeamClient.UnInit();
#endif
            bTeamInitFlag = false;
        }

#ifdef _SERVER
        if (bTransListInitFlag)
        {
            m_TransmissionList.UnInit();

            bTransListInitFlag = false;
        }
#endif

#ifdef _SERVER
        KG_COM_RELEASE(m_piSceneLoader);
#endif
        KG_COM_RELEASE(m_piRecorderTools);
		*/
    }

    //KG_COM_RELEASE(piIniFile);
    KGLogPrintf(KGLOG_INFO, "Load game settings ... ... [%s]", bResult ? "OK" : "Failed");
    
	return bResult;
}

void KSO3World::UnInit(void)
{
	BOOL                bRetCode         = false;
    KDeleteSceneFunc    DeleteSceneFunc;


//#ifdef _SERVER
//    m_TransmissionList.UnInit();
//#endif
//
//#ifdef _CLIENT
//    m_TongClient.UnInit();
//#endif
//
//    m_PQManager.UnInit();
//
//    m_AIManager.UnInit();
//
	m_SceneSet.Traverse(DeleteSceneFunc);
//
//#ifdef _CLIENT
//    KG_COM_RELEASE(m_piTextFilter);
//
//    while (!m_LinkItemQue.empty())
//    {
//        KItem* pDeleteItem = m_LinkItemQue.front();
//        if (pDeleteItem)
//        {
//            pDeleteItem->UnInit();
//			g_pSO3World->m_ItemSet.Unregister(pDeleteItem);
//			KMemory::Delete(pDeleteItem);
//            pDeleteItem = NULL;
//        }
//        m_LinkItemQue.pop_front();
//    }
//
//    m_ClientMailList.clear();
//    m_AuctionClient.ClearAll();
//#endif
//
//    m_SkillManager.UnInit();
//
//    m_BuffManager.UnInit();
//
//    m_CampInfo.UnInit();
//#ifdef _SERVER
//    m_DropCenter.UnInit();
//#endif
//
      m_RoadManager.UnInit();
//
//	  m_ProfessionManager.UnInit();
//    
//    m_ShopCenter.UnInit();
//
//    m_ItemManager.UnInit();
//    
//    m_ScriptCenter.UnInit();
//    
      m_Settings.UnInit();
//    
//    m_FellowshipMgr.UnInit();
//    
//#ifdef _SERVER
//    // m_PartyCache.UnInit();
//    m_TeamServer.UnInit();
//#endif
//#ifdef _CLIENT
//    m_TeamClient.UnInit();
//#endif
//    
//#ifdef _SERVER
//    KG_COM_RELEASE(m_piSceneLoader);
//#endif
//    KG_COM_RELEASE(m_piRecorderTools);

	return;
}


void KSO3World::Activate(void)
{
    KTraverseSceneFunc  TraverseSceneFunc;

	m_nGameLoop++;
    
    assert(m_piRecorderTools);

    m_nCurrentTime = m_piRecorderTools->GetTime();

#ifdef _SERVER
  	KTraversePlayerFunc TraversePlayerFunc;

	g_pSO3World->m_PlayerSet.Traverse(TraversePlayerFunc);
    
    ProcessLoadedScene();
#endif

	m_SceneSet.Traverse(TraverseSceneFunc);
 
#ifdef _SERVER
    m_ShopCenter.Activate();
#endif

    m_ScriptCenter.Activate();

	return;
}

#ifdef _CLIENT
KScene* KSO3World::NewEditorScene(int nRegionWidth, int nRegionHeight)
{
    KScene*         pResult     = NULL;
	BOOL            bRetCode    = false;
    KScene*         pScene      = NULL;
    BOOL            bInitFlag   = false;
	static DWORD    s_dwMapID   = 1;

	pScene = KMemory::New<KScene>();
	KGLOG_PROCESS_ERROR(pScene);
	
	bRetCode = pScene->Init();
	KGLOG_PROCESS_ERROR(bRetCode);
    bInitFlag = true;

	pScene->m_dwMapID       = s_dwMapID++;
	pScene->m_nCopyIndex    = 1;
	pScene->m_eSceneState   = ssNormal;
    pScene->m_bReviveInSitu = false;

	bRetCode = pScene->SetRegionWidth(nRegionWidth);
	KGLOG_PROCESS_ERROR(bRetCode);

	bRetCode = pScene->SetRegionHeight(nRegionHeight);
	KGLOG_PROCESS_ERROR(bRetCode);

	for (int nHeightIndex = 0; nHeightIndex < nRegionHeight; nHeightIndex++)
	{
		for (int nWidthIndex = 0; nWidthIndex < nRegionWidth; nWidthIndex++)
		{
			KRegion* pRegion = KMemory::New<KRegion>();
			KGLOG_PROCESS_ERROR(pRegion);

			bRetCode = pRegion->Init(pScene, nWidthIndex, nHeightIndex);
			KGLOG_PROCESS_ERROR(bRetCode);

			pScene->SetRegion(nWidthIndex, nHeightIndex, pRegion);

            pRegion->SetActive();
		}
	}

	if (g_pGameWorldRepresentHandler)
        g_pGameWorldRepresentHandler->OnNewScene(pScene, 0, 0, 0, true);

    pResult = pScene;
Exit0:
    if (pResult == NULL)
    {
        if (bInitFlag)
        {
            pScene->UnInit();
            bInitFlag = false;
        }

        if (pScene != NULL)
        {
            KMemory::Delete(pScene);
            pScene = NULL;
        }
    }

	return pResult;
}
#endif	//_CLIENT

#ifdef _CLIENT
KScene* KSO3World::LoadEditorScene(const char cszFileName[],FUNC_LOADPROGRESS pProgress)
{
    KScene*                         pResult                 = NULL;
	BOOL                            bRetCode                = false;
	BOOL                            bSuccessLoad3DScene     = false;
	BOOL							bHaveLogicalScene		= false;
	int                             nRegionWidth            = 0;
	int                             nRegionHeight           = 0;
    KScene*                         pScene                  = NULL;
    BOOL                            bSceneInitFlag          = false;
    KRegion*                        pAllocRegion            = NULL;
    BOOL                            bAllocRegionInitFlag    = false;
    BOOL                            bReviveInSitu           = false;
    KMapParams*                     pMapParam               = NULL;
    char*                           pcPos                   = NULL;
	char                            szMapName[_NAME_LEN];

	g_ExtractFileName(szMapName, cszFileName);

    pcPos = strrchr(szMapName, '.');
    KGLOG_PROCESS_ERROR(pcPos);

    *pcPos = '\0';

    pMapParam = g_pSO3World->m_Settings.m_MapListFile.GetMapParamByName(szMapName);
    KGLOG_PROCESS_ERROR(pMapParam);

	pScene = KMemory::New<KScene>();
	KGLOG_PROCESS_ERROR(pScene);

	bRetCode = pScene->Init();
	KGLOG_PROCESS_ERROR(bRetCode);
    bSceneInitFlag = true;

	pScene->m_dwMapID               = pMapParam->dwMapID;
	pScene->m_nCopyIndex            = 1;
	pScene->m_eSceneState           = ssLoading;
    pScene->m_bReviveInSitu         = bReviveInSitu;
    pScene->m_dwBanSkillMask        = pMapParam->dwBanSkillMask;
    pScene->m_dwBattleRelationMask  = pMapParam->dwBattleRelationMask;
    pScene->m_nReviveCycle          = pMapParam->nReviveCycle;
    pScene->m_bDoNotGoThroughRoof   = pMapParam->bDoNotGoThroughRoof;

	strncpy(pScene->m_szName, szMapName, sizeof(pScene->m_szName));
	pScene->m_szName[sizeof(pScene->m_szName) - 1] = 0;

	strncpy(pScene->m_szFilePath, cszFileName, sizeof(pScene->m_szFilePath));
    pScene->m_szFilePath[sizeof(pScene->m_szFilePath) - 1] = '\0';

    bRetCode = pScene->LoadBaseInfo();
	if (bRetCode)
	{
		bHaveLogicalScene = true;
        bReviveInSitu     = true;
	}
	else
	{
		pScene->SetRegionWidth(2);
		pScene->SetRegionHeight(2);
	}

	nRegionWidth = pScene->GetRegionWidth();
	nRegionHeight = pScene->GetRegionHeight();

	for (int nHeightIndex = 0; nHeightIndex < nRegionHeight; nHeightIndex++)
	{
		for (int nWidthIndex = 0; nWidthIndex < nRegionWidth; nWidthIndex++)
		{
			pAllocRegion = KMemory::New<KRegion>();
			KGLOG_PROCESS_ERROR(pAllocRegion);

			bRetCode = pAllocRegion->Init(pScene, nWidthIndex, nHeightIndex);
			KGLOG_PROCESS_ERROR(bRetCode);
            bAllocRegionInitFlag = true;

			bRetCode = pScene->SetRegion(nWidthIndex, nHeightIndex, pAllocRegion);
            KGLOG_PROCESS_ERROR(bRetCode);
            
            pAllocRegion->SetActive();

            pAllocRegion            = NULL;
            bAllocRegionInitFlag    = false;
		}
	}

    KGLOG_PROCESS_ERROR(g_pGameWorldRepresentHandler);

	bRetCode = g_pGameWorldRepresentHandler->OnNewScene(pScene, 0, 0, 0, true);
	KGLOG_PROCESS_ERROR(bRetCode);
	bSuccessLoad3DScene = true;

	if (bHaveLogicalScene)
	{
		bRetCode = pScene->EditorLoad(pProgress);
		KGLOG_PROCESS_ERROR(bRetCode);
	}

	pScene->m_eSceneState = ssNormal;

    pResult = pScene;
Exit0:
    if (!pResult)
    {
		if (pScene)
		{
			pScene->DeleteAllRegion();
		}

        if (bSuccessLoad3DScene)
        {
            if (g_pGameWorldRepresentHandler)
		        g_pGameWorldRepresentHandler->OnDeleteScene(pScene->m_dwID);
            bSuccessLoad3DScene = false;
        }

		if (bSceneInitFlag)
		{
			assert(pScene);
			pScene->UnInit();
			bSceneInitFlag = false;
		}

        if (pScene)
        {
		    KMemory::Delete(pScene);
            pScene = NULL;
        }
    }	

    if (bAllocRegionInitFlag)
    {
        pAllocRegion->UnInit();
        bAllocRegionInitFlag = false;
    }

    if (pAllocRegion)
    {
        KMemory::Delete(pAllocRegion);
        pAllocRegion = NULL;
    }

	return pResult;
}

#endif	//_CLIENT

#ifdef _SERVER
void KSO3World::ProcessLoadedScene()
{
    BOOL    bRetCode            = false;
    DWORD   dwLoadedSceneID     = 0;
    KScene* pLoadedScene        = NULL;

    dwLoadedSceneID = m_piSceneLoader->GetCompletedScene();
    KG_PROCESS_ERROR(dwLoadedSceneID != ERROR_ID);

    pLoadedScene = m_SceneSet.GetObj(dwLoadedSceneID);
    KGLOG_PROCESS_ERROR(pLoadedScene);
    
    if (pLoadedScene->m_eSceneState == ssCompleteLoading)
    {
        bRetCode = pLoadedScene->CompleteLoading();
        if (bRetCode)
        {
            pLoadedScene->m_eSceneState = ssNormal;
            pLoadedScene->CallInitScript();
            g_RelayClient.DoCreateMapRespond(pLoadedScene->m_dwMapID, pLoadedScene->m_nCopyIndex, true);
        }
        else
        {
            g_RelayClient.DoCreateMapRespond(pLoadedScene->m_dwMapID, pLoadedScene->m_nCopyIndex, false);			
            pLoadedScene->m_eSceneState = ssInvalid;
            g_pSO3World->DeleteScene(pLoadedScene);
        }
    }
    else
    {
        assert(pLoadedScene->m_eSceneState == ssFailedLoading);

        g_RelayClient.DoCreateMapRespond(pLoadedScene->m_dwMapID, pLoadedScene->m_nCopyIndex, false);			
        pLoadedScene->m_eSceneState = ssInvalid;
        g_pSO3World->DeleteScene(pLoadedScene);
    }

Exit0:
    return;
}
#endif	//_SERVER

#ifdef _CLIENT
KScene* KSO3World::NewClientScene(DWORD dwMapID, int nMapCopyIndex)
{
    KScene*     pResult     = NULL;
	BOOL        bRetCode    = false;
	KScene*     pScene      = NULL;
    BOOL        bInitFlag   = false;
    KMapParams* pMapParams  = NULL;

	pMapParams = g_pSO3World->m_Settings.m_MapListFile.GetMapParamByID(dwMapID);
	KGLOG_PROCESS_ERROR(pMapParams);

	pScene = KMemory::New<KScene>();
	KGLOG_PROCESS_ERROR(pScene);

	bRetCode = pScene->Init();
	KGLOG_PROCESS_ERROR(bRetCode);
    bInitFlag = true;

	strncpy(pScene->m_szName, pMapParams->szMapName, sizeof(pScene->m_szName));
	pScene->m_szName[sizeof(pScene->m_szName) - 1] = '\0';

    strncpy(pScene->m_szFilePath, pMapParams->szResourceFilePath, sizeof(pScene->m_szFilePath));
    pScene->m_szFilePath[sizeof(pScene->m_szFilePath) - 1] = '\0';
    
    pScene->m_bCanPK = pMapParams->bCanPK;

    bRetCode = pScene->LoadBaseInfo();
    KGLOG_PROCESS_ERROR(bRetCode);

	pScene->m_dwMapID               = dwMapID;
    pScene->m_nBroadcastRegion      = pMapParams->nBroadcast;
	pScene->m_nCopyIndex            = nMapCopyIndex;
    pScene->m_bReviveInSitu         = pMapParams->bReviveInSitu;
	pScene->m_eSceneState           = ssNormal;
    pScene->m_dwBanSkillMask        = pMapParams->dwBanSkillMask;
    pScene->m_dwBattleRelationMask  = pMapParams->dwBattleRelationMask;
    pScene->m_nReviveCycle          = pMapParams->nReviveCycle;
    pScene->m_bDoNotGoThroughRoof   = pMapParams->bDoNotGoThroughRoof;

    pResult = pScene;
Exit0:
    if (pResult == NULL)
    {
        if (bInitFlag)
        {
            pScene->UnInit();
            bInitFlag = true;
        }

        if (pScene)
        {
            KMemory::Delete(pScene);
            pScene = NULL;
        }
    }
	return pResult;
}
#endif	//_CLIENT

BOOL KSO3World::DeleteScene(KScene* pScene)
{
    BOOL    bResult  = false;
	BOOL    bRetCode = false;
    DWORD   dwID     = pScene->m_dwID;

	pScene->UnInit();

#ifdef _CLIENT
    if (g_pGameWorldRepresentHandler)
	    g_pGameWorldRepresentHandler->OnDeleteScene(dwID);
#endif

    KMemory::Delete(pScene);
    pScene = NULL;

    bResult = true;
Exit0:
	return bResult;
}


KScene* KSO3World::GetScene(DWORD dwMapID, int nMapCopyIndex)
{
	KGetSceneFunc GetSceneFunc;

	GetSceneFunc.m_dwMapID          = dwMapID;
	GetSceneFunc.m_nMapCopyIndex    = nMapCopyIndex;
    GetSceneFunc.m_pScene           = NULL;

	g_pSO3World->m_SceneSet.Traverse(GetSceneFunc);

	return GetSceneFunc.m_pScene;
}

KNpc* KSO3World::NewNpc(DWORD dwNpcID)
{
    KNpc*           pResult         = NULL;
	BOOL            bRetCode        = false;
    KNpc*           pNpc            = NULL;
	BOOL            bNpcRegFlag     = false;
    BOOL            bNpcInitFlag    = false;

    pNpc = KMemory::New<KNpc>();
    KGLOG_PROCESS_ERROR(pNpc);

	bRetCode = g_pSO3World->m_NpcSet.Register(pNpc, dwNpcID);
	KGLOG_PROCESS_ERROR(bRetCode);
	bNpcRegFlag = true;

    bRetCode = pNpc->Init();
    KGLOG_PROCESS_ERROR(bRetCode);
    bNpcInitFlag = true;

	pResult = pNpc;
Exit0:
    if (pResult == NULL)
    {
        if (bNpcInitFlag)
        {
            pNpc->UnInit();
            bNpcInitFlag = false;
        }

		if (bNpcRegFlag)
		{
			g_pSO3World->m_NpcSet.Unregister(pNpc);
			bNpcRegFlag = false;
		}

        if (pNpc)
        {
            KMemory::Delete(pNpc);
            pNpc = NULL;
        }
    }
    return pResult;
}

BOOL KSO3World::DeleteNpc(KNpc* pNpc, BOOL bKilled /* = false*/)
{
	BOOL bRetCode = false;

	assert(pNpc);    

    if (pNpc->m_pScene)
    {
#ifdef _SERVER
        pNpc->m_pScene->DeleteNpcNickname(pNpc);

        if (pNpc->m_AIData.nPatrolPathID)
            pNpc->m_pScene->m_AIPatrolGroupManager.LeaveGroup(pNpc->m_AIData.nPatrolPathID, pNpc->m_dwID);
#endif

        RemoveNpc(pNpc, bKilled);
    }

	pNpc->UnInit();

	g_pSO3World->m_NpcSet.Unregister(pNpc);

    KMemory::Delete(pNpc);
    pNpc = NULL;
    
    return true;
}

BOOL KSO3World::AddNpc(KNpc* pNpc, KScene* pScene, int nX, int nY, int nZ/* = 0*/)
{
    BOOL bResult  = false;
	BOOL bRetCode = false;

	assert(pNpc);
	assert(pScene);
    assert(pNpc->m_pTemplate);

	pNpc->m_nX = nX;
	pNpc->m_nY = nY;
	pNpc->m_nZ = nZ;
	pNpc->m_nXCell = nX / CELL_LENGTH % REGION_GRID_WIDTH;
	pNpc->m_nYCell = nY / CELL_LENGTH % REGION_GRID_HEIGHT;

	bRetCode = pScene->AddNpc(pNpc);
	KGLOG_PROCESS_ERROR(bRetCode);

#ifdef _CLIENT
    if (g_pGameWorldRepresentHandler)
        g_pGameWorldRepresentHandler->OnAddCharacter(pNpc);

    if (g_pGameWorldUIHandler)
    {
        KUIEventNpcEnterScene param = { pNpc->m_dwID };
        g_pGameWorldUIHandler->OnNpcEnterScene(param);
    }
#endif

    bResult = true;
Exit0:
    if (!bResult)
    {
	    if (pNpc && pNpc->m_pScene)
	    {
		    pNpc->m_pScene->RemoveNpc(pNpc);
	    }
    }
	return bResult;
}

BOOL KSO3World::RemoveNpc(KNpc* pNpc, BOOL bKilled /* = false    为真表示被杀并且生成尸体,不要渐隐效果*/)
{
    BOOL bResult  = false;
	BOOL bRetCode = false;

	assert(pNpc->m_pScene);

#ifdef _CLIENT
    if (g_pGameWorldUIHandler)
    {
        KUIEventNpcLeaveScene param = { pNpc->m_dwID };
        g_pGameWorldUIHandler->OnNpcLeaveScene(param);
    }

    if (g_pGameWorldRepresentHandler)
    {
        g_pGameWorldRepresentHandler->OnRemoveCharacter(pNpc, bKilled);
    }
#endif

#ifdef _SERVER
	g_PlayerServer.DoRemoveCharacter(pNpc, bKilled);
#endif

	bRetCode = pNpc->m_pScene->RemoveNpc(pNpc);
	KGLOG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
	return bResult;
}

KDoodad* KSO3World::NewDoodad(DWORD dwDoodadID)
{
    KDoodad*        pResult         = NULL;
	BOOL            bRetCode        = false;
	KDoodad*        pDoodad         = NULL;
	BOOL            bDoodadRegFlag  = false;
    BOOL            bDoodadInitFlag = false;

    pDoodad = KMemory::New<KDoodad>();
    KGLOG_PROCESS_ERROR(pDoodad);

	bRetCode = g_pSO3World->m_DoodadSet.Register(pDoodad, dwDoodadID);
	KGLOG_PROCESS_ERROR(bRetCode);
	bDoodadRegFlag = true;

    bRetCode = pDoodad->Init();
    KGLOG_PROCESS_ERROR(bRetCode);
    bDoodadInitFlag = true;
	
	pResult = pDoodad;
Exit0:
    if (pResult == NULL)
    {
        if (bDoodadInitFlag)
        {
            pDoodad->UnInit();
            bDoodadInitFlag = false;
        }

		if (bDoodadRegFlag)
		{
			g_pSO3World->m_DoodadSet.Unregister(pDoodad);
			bDoodadRegFlag = false;
		}

        if (pDoodad)
        {
            KMemory::Delete(pDoodad);
            pDoodad = NULL;
        }
    }
	return pResult;
}

BOOL KSO3World::DelDoodad(KDoodad* pDoodad)
{
    BOOL bResult  = false;
	BOOL bRetCode = false;

    assert(pDoodad);

    if (pDoodad->m_pScene)
    {
#ifdef _SERVER
        pDoodad->m_pScene->DeleteDoodadNickname(pDoodad);
#endif

        RemoveDoodad(pDoodad);
    }

	pDoodad->UnInit();

	g_pSO3World->m_DoodadSet.Unregister(pDoodad);

    KMemory::Delete(pDoodad);
    pDoodad = NULL;

    bResult = true;
Exit0:
    return bResult;
}

BOOL KSO3World::AddDoodad(KDoodad* pDoodad, KScene* pScene, int nX, int nY, int nZ)
{
    BOOL bResult  = false;
	BOOL bRetCode = false;

	pDoodad->m_nX = nX;
	pDoodad->m_nY = nY;
	pDoodad->m_nZ = nZ;
	pDoodad->m_nXCell = nX / CELL_LENGTH % REGION_GRID_WIDTH;
	pDoodad->m_nYCell = nY / CELL_LENGTH % REGION_GRID_HEIGHT;

	bRetCode = pScene->AddDoodad(pDoodad);
	KGLOG_PROCESS_ERROR(bRetCode);

#ifdef _CLIENT
    if (g_pGameWorldRepresentHandler)
        g_pGameWorldRepresentHandler->OnAddDoodad(pDoodad);

    if (g_pGameWorldUIHandler)
    {
        KUIEventDoodadEnterScene param = { pDoodad->m_dwID };
        g_pGameWorldUIHandler->OnDoodadEnterScene(param);
    }
#endif

    bResult = true;
Exit0:
    if (!bResult)
    {
	    if (pDoodad && pDoodad->m_pScene)
	    {
		    pDoodad->m_pScene->RemoveDoodad(pDoodad);
	    }
    }
	return bResult;
}

BOOL KSO3World::RemoveDoodad(KDoodad* pDoodad)
{
    BOOL bResult  = false;
	BOOL bRetCode = false;

	assert(pDoodad);
	assert(pDoodad->m_pScene);

#ifdef _CLIENT
    if (g_pGameWorldUIHandler)
    {
        KUIEventDoodadLeaveScene param = { pDoodad->m_dwID };
        g_pGameWorldUIHandler->OnDoodadLeaveScene(param);
    }

    if (g_pGameWorldRepresentHandler)
    {
        g_pGameWorldRepresentHandler->OnRemoveDoodad(pDoodad);
    }
#endif

#ifdef _SERVER
    g_PlayerServer.DoRemoveDoodad(pDoodad);
#endif

	bRetCode = pDoodad->m_pScene->RemoveDoodad(pDoodad);
	KGLOG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
	return bResult;
}

KPlayer* KSO3World::NewPlayer(DWORD dwPlayerID)
{
    KPlayer*    pResult         = NULL;
	BOOL        bRetCode        = false;
	KPlayer*    pPlayer         = NULL;
    BOOL        bRegisterFlag   = false;
    BOOL        bInitFlag       = false;

	assert(dwPlayerID != ERROR_ID);

    pPlayer = KMemory::New<KPlayer>();
    KGLOG_PROCESS_ERROR(pPlayer);

    bRetCode = g_pSO3World->m_PlayerSet.Register(pPlayer, dwPlayerID);
    KGLOG_PROCESS_ERROR(bRetCode);
    bRegisterFlag = true;

	bRetCode = pPlayer->Init();
	KGLOG_PROCESS_ERROR(bRetCode);
    bInitFlag = true;

    pResult = pPlayer;
Exit0:
    if (pResult == NULL)
    {
        if (bInitFlag)
        {
            pPlayer->UnInit();
            bInitFlag = false;
        }

        if (bRegisterFlag)
        {
            g_pSO3World->m_PlayerSet.Unregister(pPlayer);
            bRegisterFlag = false;
        }

        if (pPlayer != NULL)
        {
            KMemory::Delete(pPlayer);
            pPlayer = NULL;
        }
    }
	return pResult;
}

BOOL KSO3World::DelPlayer(KPlayer* pPlayer)
{
	BOOL    bRetCode        = false;
    int     nLuaTopIndex    = 0;

#ifdef _SERVER
	g_pSO3World->m_ScriptCenter.SafeCallBegin(&nLuaTopIndex);
	g_pSO3World->m_ScriptCenter.PushValueToStack(pPlayer);
	g_pSO3World->m_ScriptCenter.CallFunction(PLAYER_SCRIPT, "OnPlayerLeaveGS", 0);
	g_pSO3World->m_ScriptCenter.SafeCallEnd(nLuaTopIndex);

	g_RelayClient.DoPlayerLeaveGS(pPlayer->m_dwID);
#endif

    if (pPlayer->m_pScene)
    {
        RemovePlayer(pPlayer);
    }

	pPlayer->UnInit();

    assert(pPlayer->m_dwID != ERROR_ID);

    g_pSO3World->m_PlayerSet.Unregister(pPlayer);

    KMemory::Delete(pPlayer);
    pPlayer = NULL;

	return true;
}

// 向游戏世界中添加一个新玩家
BOOL KSO3World::AddPlayer(KPlayer* pPlayer, KScene* pScene, int nX, int nY, int nZ)
{
    BOOL bResult  = false;
	BOOL bRetCode = false;

	assert(pPlayer);
	assert(pScene);

	pPlayer->m_nX = nX;
	pPlayer->m_nY = nY;
	pPlayer->m_nZ = nZ;
	pPlayer->m_nXCell = nX / CELL_LENGTH % REGION_GRID_WIDTH;
	pPlayer->m_nYCell = nY / CELL_LENGTH % REGION_GRID_HEIGHT;

	bRetCode = pScene->AddPlayer(pPlayer);
	KGLOG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
	return bResult;
}

BOOL KSO3World::RemovePlayer(KPlayer* pPlayer)
{
    BOOL bResult    = false;
	BOOL bRetCode   = false;

	assert(pPlayer->m_pScene);

#ifdef _CLIENT
    if (g_pGameWorldUIHandler)
    {
        KUIEventPlayerLeaveScene param = { pPlayer->m_dwID };
        g_pGameWorldUIHandler->OnPlayerLeaveScene(param);
    }

    if (g_pGameWorldRepresentHandler)
    {
        g_pGameWorldRepresentHandler->OnRemoveCharacter(pPlayer, false);
    }
#endif

    if (pPlayer->m_pRegion)
    {
#ifdef _SERVER
        g_PlayerServer.DoRemoveCharacter(pPlayer, false);
#endif

        bRetCode = pPlayer->m_pScene->RemovePlayer(pPlayer);
        KGLOG_PROCESS_ERROR(bRetCode);
    }
    bResult = true;
Exit0:
	return bResult;
}

BOOL KGetSceneFunc::operator()(DWORD dwID, KScene* pScene)
{
    // CopyID为0表示任意Copy,只要MapID对即可
	if (pScene->m_dwMapID == m_dwMapID && (pScene->m_nCopyIndex == m_nMapCopyIndex || m_nMapCopyIndex == 0))
	{
		m_pScene = pScene;
		return false;
	}

	return true;
}

BOOL KDeleteSceneFunc::operator()(DWORD dwID, KScene* pScene)
{
	pScene->UnInit();

#ifdef _CLIENT
    if (g_pGameWorldRepresentHandler)
	    g_pGameWorldRepresentHandler->OnDeleteScene(dwID);
#endif

    KMemory::Delete(pScene);
    pScene = NULL;
    
	return true;
}

#ifdef _SERVER
BOOL KTraversePlayerFunc::operator()(DWORD dwID, KPlayer* pPlayer)
{
	BOOL bRetCode = false;

	switch (pPlayer->m_eGameStatus)
	{
	case gsWaitForDisconnect:
		break;

	case gsWaitForConnect:
		// 等待登陆倒计时
		if (pPlayer->m_nTimer++ >= LOGIN_TIMEOUT)
        {
            KGLogPrintf(
                KGLOG_INFO, "Login timeout: acc = %s, role = %s\n", 
                pPlayer->m_szAccount, pPlayer->m_szName
            );
			g_pSO3World->DelPlayer(pPlayer);
        }
        break;

	case gsWaitForPermit:
	case gsWaitForRoleData:
		// 等待登陆倒计时
		if (pPlayer->m_nTimer++ >= LOGIN_TIMEOUT)
		{
            KGLogPrintf(
                KGLOG_INFO, "Relay permission or query role data,shutdown(%s, %d)\n", 
                pPlayer->m_szName, pPlayer->m_nConnIndex
            );
			g_PlayerServer.Detach(pPlayer->m_nConnIndex);
		}
		break;

    case gsWaitForLoginLoading:
    case gsWaitForNewMapLoading:
        break;

	case gsDeleting:
		// 掉线倒计时
		if (pPlayer->m_nTimer++ >= LOGOUT_WAITING_TIME || !pPlayer->m_bFightState)
		{
            pPlayer->SavePosition();

            // 这里之所以调RemovePlayer是为了在Save之前触发Trap(如果有的话),Trap可能会涉及存盘角色数据
            g_pSO3World->RemovePlayer(pPlayer);

	        g_RelayClient.SaveRoleData(pPlayer);

            g_pSO3World->m_FellowshipMgr.UnloadPlayerFellowship(pPlayer->m_dwID);

			g_pSO3World->DelPlayer(pPlayer);
		}
		break;

    case gsPlaying:
        if ((pPlayer->m_dwID - (DWORD)g_pSO3World->m_nGameLoop) % 2 == 0)
        {
		    g_PlayerServer.DoFrameSignal(pPlayer->m_nConnIndex);
        }

        if (g_pSO3World->m_nGameLoop == pPlayer->m_nNextSaveFrame)
        {
            pPlayer->SavePosition();

            g_RelayClient.SaveRoleData(pPlayer);
        }

        break;

    default:
        break;
	}

	return true;
}

#endif // _SERVER


BOOL KTraverseSceneFunc::operator()(DWORD dwID, KScene* pScene)
{    
    if (pScene->m_eSceneState == ssNormal)
    {
        pScene->Activate();
    }

	return true;
}

