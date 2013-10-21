#include "stdafx.h"
#include "SO3World.h"
#include "KSO3World.h"
#include "SO3World.h"
#include "IKGSO3WorldClient.h"
#include "SO3RepresentInterface.h"
#include "SO3UI/ISO3UI.h"
#include "IEOther.h"
#include "IEEditor.h"
#include "AtlBase.h"
#include "ProgressForm.h"
#include "TopViewForm.h"
#include "Stolen/KG3DSceneLogicalEditorBase.h"
#include "Stolen/KSceneEditorDocLogical.h"
#include "KG3DRTTIType.h"
#include "LoadLogicalObj.h"
#include "Stolen/KLogicObstacleGenerator.h"
#include "SaveLogicalObj.h"
#include "KPatrolPath.h"

extern TCHAR g_szDefWorkDirectory[MAX_PATH];
extern IKSO3RepresentHandler* g_pRepresentHandler;

void ExchangeLogicalData(LPCTSTR pFileName, KScene* lpLogicalScene, KSceneEditorDocLogical* pLogicalData)
{
	HRESULT hrRetCode = NULL;

	BeforeChange(pLogicalData);

	TCHAR path_buffer[256];
	TCHAR drive[256];
	TCHAR dir[256];
	TCHAR fname[256];
	TCHAR ext[256];

	wsprintf(path_buffer,"%s",pFileName);
	_splitpath( path_buffer, drive, dir, fname, ext );

	TCHAR Name[MAX_PATH];
	wsprintf(Name,"map\\%s\\",fname);

	char szFileName[MAX_PATH];
	g_ExtractFileName(szFileName, fname);

	//////////////////////////////////////////////////////////////////////////
	TCHAR szMapDir[MAX_PATH];
	TCHAR szDoodadReliveDir[MAX_PATH];
	TCHAR szNpcReliveDir[MAX_PATH];
	TCHAR szPatrolPathDir[MAX_PATH];
	TCHAR szNpcTeamDir[MAX_PATH];
	TCHAR szRoadDir[MAX_PATH];
	TCHAR szAIParameterDir[MAX_PATH];
	wsprintf(szMapDir,"%s%s\\map\\%s\\",g_szDefWorkDirectory,SETTING_DIR,fname);
	wsprintf(szDoodadReliveDir,"%s%s\\map\\%s\\DoodadRelive\\",g_szDefWorkDirectory,SETTING_DIR,fname);
	wsprintf(szNpcReliveDir,"%s%s\\map\\%s\\NpcRelive\\",g_szDefWorkDirectory,SETTING_DIR,fname);
	wsprintf(szPatrolPathDir,"%s%s\\map\\%s\\PatrolPath\\",g_szDefWorkDirectory,SETTING_DIR,fname);
	wsprintf(szNpcTeamDir,"%s%s\\map\\%s\\NpcTeam\\",g_szDefWorkDirectory,SETTING_DIR,fname);
	wsprintf(szRoadDir,"%s%s\\Road\\",g_szDefWorkDirectory,SETTING_DIR);
	wsprintf(szAIParameterDir,"%s%s\\map\\%s\\NpcAIParameter\\",g_szDefWorkDirectory,SETTING_DIR,fname);
	mkdir(szMapDir);
	mkdir(szDoodadReliveDir);
	mkdir(szNpcReliveDir);
	mkdir(szPatrolPathDir);
	mkdir(szNpcTeamDir);
	mkdir(szRoadDir);
	mkdir(szAIParameterDir);

	lpLogicalScene->DeleteAllObjects();

	ExchangeNpc(lpLogicalScene, pLogicalData, pFileName);

	ExchangeDoodad(lpLogicalScene, pLogicalData);

	wsprintf(Name,"map\\%s\\NpcRelive\\",fname);
	ExchaneNpcRefreshPointSet(Name,fname, pLogicalData);

	wsprintf(Name,"map\\%s\\DoodadRelive\\",fname);
	ExchaneDoodadRefreshPointSet(Name,fname, pLogicalData);

	wsprintf(Name,"map\\%s\\NpcTeam\\",fname);
	ExchangeAIGroup(Name,fname, pLogicalData);

	wsprintf(Name,"map\\%s\\PatrolPath\\",fname);
	ExchangeWayPointSet(Name,fname, pLogicalData);

	ExchangeTrafficPointSet(szRoadDir, pLogicalData);
	SetCurrentDirectory(g_szDefWorkDirectory);
	lpLogicalScene->Save(szFileName,&ShowProgress);
	//////////////////////////////////////////////////////////////////////////
	TCHAR szNpcReliveFile[MAX_PATH];
	wsprintf(szNpcReliveFile,"%s%s\\map\\%s\\NpcRelive\\MapReviveList.tab",g_szDefWorkDirectory,SETTING_DIR,fname);
	ExchangeNpcReliveSetting(szNpcReliveFile, pLogicalData);


	TCHAR szDoodadReliveFile[MAX_PATH];
	wsprintf(szDoodadReliveFile,"%s%s\\map\\%s\\DoodadRelive\\DoodadReviveList.tab",g_szDefWorkDirectory,SETTING_DIR,fname);
	ExchangeDoodadReliveSetting(szDoodadReliveFile, pLogicalData);

	TCHAR szNpcRandomFile[MAX_PATH];
	wsprintf(szNpcRandomFile,"%s%s\\map\\%s\\NpcRelive\\NpcRandomList.tab",g_szDefWorkDirectory,SETTING_DIR,fname);
	ExchangeNpcRandomSetting(szNpcRandomFile,szNpcReliveDir, pLogicalData);

	TCHAR szNpcAISETFile[MAX_PATH];
	wsprintf(szNpcAISETFile,"%s%s\\map\\%s\\NpcAIParameter\\NpcAIParameterList.tab",g_szDefWorkDirectory,SETTING_DIR,fname);
	pLogicalData->ExchangeNpcAISet(lpLogicalScene->m_dwMapID, szNpcAISETFile);

	;
Exit0:
	;
}

void BeforeChange(KSceneEditorDocLogical* m_lpLogicalData)
{
	//////////////////////////////////////////////////////////////////////////
	{
		//DWORD dwCount = 0;
		//list<KSceneEditorDocLogical::WayPointSet*>::iterator i = m_lpLogicalData->m_listWayPointSet.begin();
		//while (i!=m_lpLogicalData->m_listWayPointSet.end())
		//{
		//	KSceneEditorDocLogical::WayPointSet* pPath = *i;

		//	pPath->nIndex = dwCount+1;

		//	i++;
		//	dwCount++;
		//}
	}
	//////////////////////////////////////////////////////////////////////////
	{
		list<KSceneEditorDocLogical::NPCInfo*>::iterator i = m_lpLogicalData->m_listNPCInfo.begin();
		while (i!=m_lpLogicalData->m_listNPCInfo.end())
		{
			KSceneEditorDocLogical::NPCInfo* pNpc = *i;

			i++;
		}
	}
	//////////////////////////////////////////////////////////////////////////
	//{
	//	DWORD dwSetCount = 0;
	//	list<KSceneEditorDocLogical::NPCRefreshPointSet*>::iterator i = m_lpLogicalData->m_listNPCRefreshPointSet.begin();
	//	while (i!=m_lpLogicalData->m_listNPCRefreshPointSet.end())
	//	{
	//		KSceneEditorDocLogical::NPCRefreshPointSet* pSet = *i;


	//		list<KSceneEditorDocLogical::NPCInfo*>::iterator j = pSet->listNPCInfo.begin();
	//		while (j!=pSet->listNPCInfo.end())
	//		{
	//			KSceneEditorDocLogical::NPCInfo* pNpc = *j;

	//			pNpc->dwNpcReliveID = dwSetCount + 1;
	//			j++;
	//		}

	//		i++;
	//		dwSetCount++;
	//	}
	//}
	////////////////////////////////////////////////////////////////////////////
	//{
	//	DWORD dwSetCount = 0;
	//	list<KSceneEditorDocLogical::DoodadRefreshPointSet*>::iterator i = m_lpLogicalData->m_listDoodadRefreshPointSet.begin();
	//	while (i!=m_lpLogicalData->m_listDoodadRefreshPointSet.end())
	//	{
	//		KSceneEditorDocLogical::DoodadRefreshPointSet* pSet = *i;


	//		list<KSceneEditorDocLogical::DoodadInfo*>::iterator j = pSet->listDoodadInfo.begin();
	//		while (j!=pSet->listDoodadInfo.end())
	//		{
	//			KSceneEditorDocLogical::DoodadInfo* pDoodad = *j;

	//			pDoodad->dwDoodadReliveID = dwSetCount + 1;
	//			j++;
	//		}

	//		i++;
	//		dwSetCount++;
	//	}
	//}
	//////////////////////////////////////////////////////////////////////////
	{
		DWORD dwCount = 0;
		list<KSceneEditorDocLogical::AIGroup*>::iterator i = m_lpLogicalData->m_listAIGroup.begin();
		while (i!=m_lpLogicalData->m_listAIGroup.end())
		{
			KSceneEditorDocLogical::AIGroup* pAI = *i;

			for (size_t n = 0;n<pAI->vecNpcInfo.size();n++)
			{
				KSceneEditorDocLogical::NPCInfo* pNpcInfo = pAI->vecNpcInfo[n];
				pNpcInfo->dwNpcTeamID = dwCount + 1;
				pNpcInfo->dwNpcTeamIndex = (DWORD) n;
			}

			i++;
			dwCount++;
		}
	}
}

void ExchangeNpc(KScene *pLogicalScene, KSceneEditorDocLogical* pLogicalData, LPCTSTR pFileName)
{
	BOOL bRetCode = FALSE;
	TCHAR strDriver[MAX_PATH];
	TCHAR strPath[MAX_PATH];
	TCHAR strFile[MAX_PATH];
	TCHAR strPreloadNpcFilePath[MAX_PATH];
	std::set<unsigned int> setUseNpcModels;

	_splitpath_s(pFileName, 
		strDriver,
		MAX_PATH,
		strPath,
		MAX_PATH,
		strFile,
		MAX_PATH,
		NULL,
		0);

	sprintf_s(strPreloadNpcFilePath, 
		MAX_PATH,
		"%s%s%spreload.dat",
		strDriver,
		strPath,
		strFile);

	FILE *fpPreLoadNpc = fopen(strPreloadNpcFilePath, "wb");
	KG_PROCESS_ERROR(fpPreLoadNpc);

	INpcFile* piNpcFile = NULL;
	KNPC_DATA npcData;

	KGLOG_PROCESS_ERROR(pLogicalScene);

	piNpcFile = ::CreateNpcFileInterface();
	KGLOG_PROCESS_ERROR(piNpcFile);

	{
		int nTotalCount = (int)pLogicalData->m_listNPCInfo.size();
		int nCount = 0;

		list<KSceneEditorDocLogical::NPCInfo*>::iterator it = pLogicalData->m_listNPCInfo.begin();
		while (it != pLogicalData->m_listNPCInfo.end())
		{
			//KRepresentNPCEquipResource NPCRes;
			KSceneEditorDocLogical::NPCInfo* pNpcInfo = *it;

			pLogicalData->UpdateNPC(pNpcInfo);

			memset(&npcData, 0, sizeof(npcData));

			npcData.dwTemplateID        = (DWORD)pNpcInfo->nTempleteID;
			npcData.nX                  = pNpcInfo->nX;
			npcData.nY                  = pNpcInfo->nY;
			npcData.nZ                  = pNpcInfo->nZ;
			npcData.byFaceDirection     = (BYTE)pNpcInfo->nFaceDirection;
			npcData.byKind              = (BYTE)pNpcInfo->nKind;
			npcData.dwScriptID          = pNpcInfo->dwScriptID;
			npcData.dwReliveID          = pNpcInfo->dwNpcReliveID;
			npcData.dwRandomID          = pNpcInfo->dwNpcRandomID;
			npcData.dwThreatLinkID      = pNpcInfo->dwNpcTeamID;
			npcData.nOrderIndex         = (int)pNpcInfo->dwNpcTeamIndex;
			npcData.nProgressID         = pNpcInfo->nProgressID;

			if(pNpcInfo->pWayPointSet)
				npcData.nPatrolPathID   = pNpcInfo->pWayPointSet->nPatrolPathOrderID;
			else
				npcData.nOrderIndex     = 0;

			if(pNpcInfo->dwNpcAISet)
			{
				npcData.dwAIType        = (pLogicalScene->m_dwMapID<<24) | (pNpcInfo->dwNpcAISet);
			}
			else
				npcData.dwAIType        = 0;

			npcData.dwRepresentID       = pNpcInfo->dwRepresentID;

			// 指定表现ID后，随机NPC的模型就不会变了。
			if (pNpcInfo->dwNpcRandomID != 0)
			{
				npcData.dwRepresentID = 0;
			}

			strncpy(npcData.szNickName, pNpcInfo->szNickName.c_str(), sizeof(npcData.szNickName));
			npcData.szNickName[sizeof(npcData.szNickName) - 1] = '\0';

			piNpcFile->Push(npcData);

			nCount++;

			/*g_pRepresentHandler->GetNPCEquipResource(pNpcInfo->dwRepresentID, &NPCRes);
			unsigned int uHash = g_FileNameHash(NPCRes.szMDLFilePath);
			if (!setUseNpcModels.count(uHash))
			{
				setUseNpcModels.insert(uHash);
				TCHAR strOutput[MAX_PATH];
				strcpy_s(strOutput, MAX_PATH, NPCRes.szMDLFilePath);
				fwrite(strOutput, MAX_PATH, 1, fpPreLoadNpc);
			}*/

			if (nCount % 16 == 0)
			{
				ShowProgress("Exchange NPC",nCount*1.0F/nTotalCount);
			}

			++it;
		}
	}

	piNpcFile->Save(pLogicalScene->m_szName);

	if (fpPreLoadNpc)
	{
		fclose(fpPreLoadNpc);
		fpPreLoadNpc = NULL;
	}
	KG_COM_RELEASE(piNpcFile);
	return ;

Exit0:
	if (fpPreLoadNpc)
	{
		fclose(fpPreLoadNpc);
		fpPreLoadNpc = NULL;
	}
	KG_COM_RELEASE(piNpcFile);
	return ;
}

void ExchangeDoodad(KScene *pLogicalScene, KSceneEditorDocLogical* m_lpLogicalData)
{
	BOOL bRetCode = FALSE;
	IDoodadFile* piDoodadFile = NULL;
	KDOODAD_DATA DoodadData;

	ASSERT(pLogicalScene);

	piDoodadFile = ::CreateDoodadFileInterface();
	KGLOG_PROCESS_ERROR(piDoodadFile);

	{
		int nTotalCount = (int)m_lpLogicalData->m_listDoodadInfo.size();
		int nCount = 0;
		WORD nObstacleDoodadCount = 0;
		list<int> lsDoodadExceeded;
#define MAX_DOODAD_OBSTACLE 4000

		list<KSceneEditorDocLogical::DoodadInfo*>::iterator it = m_lpLogicalData->m_listDoodadInfo.begin();
		while (it != m_lpLogicalData->m_listDoodadInfo.end())
		{
			KSceneEditorDocLogical::DoodadInfo* pDoodadInfo = *it;

			m_lpLogicalData->UpdateDoodad(pDoodadInfo);

			memset(&DoodadData, 0, sizeof(DoodadData));

			DoodadData.dwTemplateID     = (DWORD)pDoodadInfo->nTempleteID;
			DoodadData.nX               = pDoodadInfo->nX;
			DoodadData.nY               = pDoodadInfo->nY;
			DoodadData.nZ               = pDoodadInfo->nZ;
			DoodadData.byFaceDirection  = (BYTE)pDoodadInfo->nFaceDirection;
			DoodadData.byLevel          = 0;
			DoodadData.dwDoodadReliveID = pDoodadInfo->dwDoodadReliveID;
			DoodadData.dwScriptID       = pDoodadInfo->dwScriptID;
			DoodadData.nProgressID      = INVALID_SCENE_PROGRESS_ID;
			// doodad加nickname
			strncpy(DoodadData.szNickName, pDoodadInfo->szNickName.c_str(), sizeof(DoodadData.szNickName));
			DoodadData.szNickName[sizeof(DoodadData.szNickName) - 1] = '\0';
			// 动态障碍
			if (pDoodadInfo->bUseDefaultObstacle)
			{
				const KDoodadTemplate* pcDoodadTemplate = g_pSO3World->m_Settings.m_DoodadTemplateList.GetTemplate(pDoodadInfo->nTempleteID);
				if (pcDoodadTemplate && pcDoodadTemplate->m_dwTemplateID != 0 && pcDoodadTemplate->m_bDynamicObstacle)
					DoodadData.wObstacleGroup = ++nObstacleDoodadCount;
				else
					DoodadData.wObstacleGroup = 0;
			}
			else
			{
				DoodadData.wObstacleGroup = pDoodadInfo->bGenerateObstacle ? ++nObstacleDoodadCount : 0;
			}
			if (DoodadData.wObstacleGroup > MAX_DOODAD_OBSTACLE - 1)
			{
				DoodadData.wObstacleGroup = 0;
				lsDoodadExceeded.push_back(pDoodadInfo->nIndex);
			}

			piDoodadFile->Push(DoodadData);

			nCount++;
			if (nCount % 8 == 0)
			{
				ShowProgress("Exchange Doodad",nCount*1.0F/nTotalCount);
			}

			++it;
		}

		// 如果有超过4000个有障碍的doodad，要msgbox一下
		if (lsDoodadExceeded.size() > 0)
		{
			TCHAR msg[4196];
			TCHAR buf[16];
			msg[0] = '\0';
			buf[0] = '\0';
			sprintf(msg, "以下nIndex的doodad无法保存动态障碍，已经超过本场景上限%d。\r\n", MAX_DOODAD_OBSTACLE);
			for (list<int>::iterator iter = lsDoodadExceeded.begin(); iter != lsDoodadExceeded.end(); iter++)
			{
				itoa(*iter, buf, 10);
				strcat(msg, buf);
				strcat(msg, " ");
			}

			MessageBox(NULL, msg, "未保存全部", 0);
			lsDoodadExceeded.clear();
		}
	}

	piDoodadFile->Save(pLogicalScene->m_szName);

	KG_COM_RELEASE(piDoodadFile);
	return ;

Exit0:
	KG_COM_RELEASE(piDoodadFile);
	return ;
}

void ExchaneNpcRefreshPointSet(LPCTSTR pFileName,LPCTSTR pMapName,KSceneEditorDocLogical* pLogicalData)
{
}

void ExchaneDoodadRefreshPointSet(LPCTSTR pFileName,LPCTSTR pMapName,KSceneEditorDocLogical* pLogicalData)
{
}

void ExchangeAIGroup(LPCTSTR pFileName,LPCTSTR pMapName,KSceneEditorDocLogical* pLogicalData)
{
	int nTotalCount = (int)pLogicalData->m_listAIGroup.size();
	int nCount = 0;

	DWORD dwCount = 0;
	list<KSceneEditorDocLogical::AIGroup*>::iterator i = pLogicalData->m_listAIGroup.begin();
	while (i!=pLogicalData->m_listAIGroup.end())
	{
		KSceneEditorDocLogical::AIGroup* pAI = *i;

		nCount++;
		ShowProgress("Exchange AIGroup",nCount*1.0F/nTotalCount);

		TCHAR Name[MAX_PATH];
		wsprintf(Name,"%sNPCTeam%d.Tab",pFileName,dwCount);

		KNpcTeam* pTeam = NULL;
		g_pSO3World->m_Settings.m_NpcTeamList.AddNpcTeam((DWORD)pLogicalData->m_nSceneID,dwCount+1,pAI->nArrayIndex);
		//pTeam = g_pSO3World->m_Settings.m_NpcTeamList.GetNpcTeam((DWORD)pLogicalData->m_nSceneID,dwCount+1);

		/*for (size_t n = 0;n<pAI->vecNpcInfo.size();n++)
		{
		KSceneEditorDocLogical::NPCInfo* pNpcInfo = pAI->vecNpcInfo[n];
		if(pNpcInfo->pLogicalNpc)
		{
		pTeam->AddMember(pNpcInfo->pLogicalNpc,(int)n);
		}
		}

		pTeam->SetLeader(0);*/
		//pTeam->SetKing((DWORD)pAI->nKing);

		i++;
		dwCount++;
	}
	g_pSO3World->m_Settings.m_NpcTeamList.Save((DWORD)pLogicalData->m_nSceneID,pMapName);
}

void ExchangeWayPointSet(LPCTSTR pFileName,LPCTSTR pMapName,KSceneEditorDocLogical* m_lpLogicalData)
{
	int nTotalCount = (int)m_lpLogicalData->m_mapWayPointSet.size();
	int nCount = 0;
	DWORD dwCount = 0;

	//////////////////////////////////////////////////////////////////////////
	m_lpLogicalData->SortWayPointSet();
	//////////////////////////////////////////////////////////////////////////
	g_pSO3World->m_Settings.m_PatrolPathList.UnInit();
	map<DWORD,KSceneEditorDocLogical::WayPointSet*>::iterator i = m_lpLogicalData->m_mapWayPointSet.begin();
	while (i!=m_lpLogicalData->m_mapWayPointSet.end())
	{
		KSceneEditorDocLogical::WayPointSet* pSet = i->second;

		nCount++;
		ShowProgress("Exchange WayPointSet",nCount*1.0F/nTotalCount);

		TCHAR Name[MAX_PATH];
		wsprintf(Name,"%sPatrolPathList%d.Tab",pFileName,pSet->nIndex-1);

		KPatrolPath* pPath = g_pSO3World->m_Settings.m_PatrolPathList.AddPatrolPath((DWORD)m_lpLogicalData->m_nSceneID, pSet->nIndex, Name);
		pPath->m_nOrderID = pSet->nPatrolPathOrderID;
		pPath->m_nWalkSpeed = pSet->nPatrolPathWalkSpeed;

		for (size_t s=0;s<pSet->vectorWayPoint.size();s++)
		{
			KSceneEditorDocLogical::WayPoint* pPoint = pSet->vectorWayPoint[s];

			KPatrolNode Node;
			Node.nPatrolPointX = pPoint->nX;
			Node.nPatrolPointY = pPoint->nY;
			Node.nRestFrame = pPoint->nStayFrame;
			Node.nFaceTo = pPoint->nStayDirection;
			Node.nAnimationID = pPoint->nStayAnimation;
			Node.nAniLoop = pPoint->bStayAniLoop;
			Node.bIsRun = pPoint->bIsRun;
			wsprintf(Node.szScriptName,"%s",pPoint->szScriptName.c_str());

			pPath->AddPoint(Node);
		}
		pPath->Save();

		i++;
		dwCount++;

	}
	g_pSO3World->m_Settings.m_PatrolPathList.Save((DWORD)m_lpLogicalData->m_nSceneID,pMapName);

}

void ExchangeTrafficPointSet(LPCTSTR pFileName,KSceneEditorDocLogical* pLogicalData)
{
	int nTotalCount = (int)pLogicalData->m_mapTrafficPoint.size();
	int nCount = 0;
	DWORD dwCount = 0;
	map<DWORD,KSceneEditorDocLogical::TrafficPoint>::iterator iter = pLogicalData->m_mapTrafficPoint.begin();
	while (iter != pLogicalData->m_mapTrafficPoint.end())
	{
		nCount++;
		ShowProgress("Exchange TrafficPoint",nCount*1.0F/nTotalCount);
		KROUTE_NODE* pNode = g_pSO3World->m_RoadManager.GetNodeInfo(iter->first);
		if (pNode == NULL)
		{
			dwCount++;
			iter++;
			continue;
		}
		pNode->dwMapID = (iter->second).m_dwMapID;
		pNode->dwCityID = (iter->second).m_dwCityID;
		pNode->eCamp = *((KCAMP*)(&((iter->second).m_dwForceID)));
		pNode->dwTongID = (iter->second).m_dwTongID;
		pNode->bIsNeedOpen = (iter->second).m_bNeedOpen;
		pLogicalData->SceneToLogical(pNode->nX,pNode->nY,pNode->nZ,(iter->second).vPosition,0);
		wsprintf(pNode->szDescription,"%s",(iter->second).m_strDescription.c_str());
		wsprintf(pNode->szScriptFile,"%s",(iter->second).m_strScriptFile.c_str());
		dwCount++;
		iter++;
	}

	nTotalCount = (int)pLogicalData->m_listTrafficPointSet.size();
	nCount = 0;
	dwCount = 0;
	list<KSceneEditorDocLogical::TrafficPointSet*>::iterator i = pLogicalData->m_listTrafficPointSet.begin();
	while (i != pLogicalData->m_listTrafficPointSet.end())
	{
		KSceneEditorDocLogical::TrafficPointSet* pSet = *i;
		nCount++;
		ShowProgress("Exchange TrafficPointSet",nCount*1.0F/nTotalCount);  
		int nTrackID = 0;
		KTRACK* pKTrack = NULL;

		pKTrack = g_pSO3World->m_RoadManager.GetTrackInfo(pSet->m_dwFromID,pSet->m_dwToID);
		if (pKTrack == NULL)
		{
			dwCount++;
			i++;
			continue;
		}
		pKTrack->nVelocity = pSet->m_dwVelocity;
		pKTrack->nCostMoney = pSet->m_dwCostMoney;

		pKTrack->vPoint.clear();
		KTRACK_POINT TrackPoint;
		DWORD dwFromID = pSet->m_dwFromID;
		for (size_t s=0;s<pSet->vecTrafficPoint.size();s++)
		{	
			pLogicalData->SceneToLogical(TrackPoint.nX,TrackPoint.nY,TrackPoint.nZ,(pSet->vecTrafficPoint[s])->vPosition,0);
			pKTrack->vPoint.push_back(TrackPoint);
		}	
		dwCount++;
		i++;
	}

	nTotalCount = (int)pLogicalData->m_listOverMapPointSet.size();
	nCount = 0;
	dwCount = 0;
	i = pLogicalData->m_listOverMapPointSet.begin();
	while (i != pLogicalData->m_listOverMapPointSet.end())
	{
		KSceneEditorDocLogical::TrafficPointSet* pSet = *i;
		nCount++;
		ShowProgress("Exchange OverMapTrafficPointSet",nCount*1.0F/nTotalCount);  
		int nTrackID = 0;
		KTRACK* pKTrack = NULL;

		pKTrack = g_pSO3World->m_RoadManager.GetTrackInfo(pSet->m_dwID);
		if (pKTrack == NULL)
		{
			dwCount++;
			i++;
			continue;
		}
		pKTrack->nVelocity = pSet->m_dwVelocity;
		pKTrack->nCostMoney = pSet->m_dwCostMoney;
		pKTrack->vPoint.clear();;
		dwCount++;
		i++;
	}
	g_pSO3World->m_RoadManager.ExportTable();
}

HRESULT ExchangeNpcReliveSetting(LPTSTR pFileName,KSceneEditorDocLogical* pLogicalData)
{
	ITabFile* pFile = NULL;
	KGLOG_PROCESS_ERROR(pFileName);
	pFile = g_OpenTabFile(pFileName,true,true);
	KGLOG_PROCESS_ERROR(pFile);

	pFile->InsertNewCol("ReviveID");
	pFile->InsertNewCol("Min");
	pFile->InsertNewCol("Max");
	pFile->InsertNewCol("IsRandom");

	pLogicalData->ExchangeNpcReviveGroups(pFile);


	pFile->Save(pFileName);

	return S_OK;
Exit0:
	SAFE_RELEASE(pFile);
	return E_FAIL;
}

HRESULT ExchangeDoodadReliveSetting(LPTSTR pFileName,KSceneEditorDocLogical* pLogicalData)
{
	ITabFile* pFile = NULL;
	KGLOG_PROCESS_ERROR(pFileName);
	pFile = g_OpenTabFile(pFileName,true,true);
	KGLOG_PROCESS_ERROR(pFile);

	pFile->InsertNewCol("ReviveID");
	pFile->InsertNewCol("Min");
	pFile->InsertNewCol("Max");
	pFile->InsertNewCol("IsRandom");

	pLogicalData->ExchangeDoodadReviveGroups(pFile);

	pFile->Save(pFileName);

	return S_OK;
Exit0:
	SAFE_RELEASE(pFile);
	return E_FAIL;
}

HRESULT ExchangeNpcRandomSetting(LPTSTR pFileName,LPTSTR pDir,KSceneEditorDocLogical* pLogicalData)
{
	ITabFile* pFile = NULL;
	KGLOG_PROCESS_ERROR(pFileName);
	pFile = g_OpenTabFile(pFileName,true,true);
	KGLOG_PROCESS_ERROR(pFile);

	pFile->InsertNewCol("RandomID");
	pFile->InsertNewCol("FileName");

	pLogicalData->ExchangeNpcRandomGroups(pFile,pDir);

	pFile->Save(pFileName);

	return S_OK;
Exit0:
	SAFE_RELEASE(pFile);
	return E_FAIL;
}