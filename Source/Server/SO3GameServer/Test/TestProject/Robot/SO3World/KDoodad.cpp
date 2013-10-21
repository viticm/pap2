#include "stdafx.h"
#include "KObjectManager.h"
#include "KDoodad.h"
#include "KRegion.h"
#include "KScene.h"
//#include "KPlayerServer.h"
#include "KPlayer.h"
#include "KLootList.h"
#include "KSO3World.h"
//#include "KDropCenter.h"
#include "KThreatList.h"
#include "KWorldSettings.h"
#include "KItemLib.h"
//#include "KCraft.h"
//#include "KProfessionManager.h"
#include "KScriptCenter.h"
#include "KMath.h"
//#include "SO3Represent/SO3GameWorldUIHandler.h"
//#include "SO3Represent/SO3GameWorldRepresentHandler.h"

KDoodad::KDoodad()
{
	m_dwTemplateID      = 0;
    m_szName[0]         = '\0';
	m_eKind             = dkInvalid;
	m_nMaxLife          = 1000;
	m_nOpenFrames       = 0;
	m_nDisappearFrames  = INT_MAX;
	m_nCurrentLife      = 0;
	m_dwScriptID        = 0;

	memset(&m_DoodadInfo, 0, sizeof(m_DoodadInfo));
	//m_pLootList = NULL;
}

KDoodad::~KDoodad()
{
}

BOOL KDoodad::Init(void)
{
	BOOL bRetCode = FALSE;

	bRetCode = KSceneObject::Init();
	KGLOG_PROCESS_ERROR(bRetCode);

	m_dwTemplateID          = 0;
	m_eState                = dsIdle;
    m_szName[0]             = '\0';
	m_eKind                 = dkInvalid;
	m_nMaxLife              = 1000;
	m_nOpenFrames           = 0;
	m_nDisappearFrames      = INT_MAX;
	m_dwRepresentID		    = 0;
	m_dwNpcTemplateID		= 0;
	m_nCurrentLife          = 0;
	m_dwScriptID            = 0;
	m_bLooted				= FALSE;
	//m_pLootList             = NULL;

	memset(&m_DoodadInfo, 0, sizeof(m_DoodadInfo));
	//memset(m_PObstacleCell, 0, sizeof(m_PObstacleCell));

	return TRUE;
Exit0:
	return FALSE;
}

BOOL KDoodad::UnInit(void)
{
	//if (m_pLootList)
	//{
	//	m_pLootList->UnInit();
	//	g_ObjectManager.m_LootListSet.Delete(m_pLootList);
	//}

	//m_pLootList = NULL;

	return KSceneObject::UnInit();
}

BOOL KDoodad::Activate(void)
{
//	BOOL bRetCode = FALSE;
//
//	bRetCode = KSceneObject::CheckGameLoop();
//	if (!bRetCode)
//		return TRUE;
//
//#ifdef _SERVER
//
//    if (((DWORD)g_SO3World.m_nGameLoop - m_dwID) % GAME_FPS == 0)
//    {
//        // TODO: 这个广播过几天去掉,原来有这个广播是因为没有Doodad进入同步范围的同步,偷懒这样做的:)
//        g_PlayerServer.DoSyncDoodadState(this);
//    }
//
//	//LootList检查Item的保护时间
//	if (m_pLootList)
//	{
//		m_pLootList->CheckGameLoop();
//	}
//
//	switch (m_eKind)
//	{
//	case dkCorpse:
//	case dkNormal:
//	case dkQuest:	
//	case dkTreasure:
//	case dkOrnament:
//	case dkClientOnly:
//		if (m_nDisappearFrames <= g_SO3World.m_nGameLoop)
//			Remove();
//		break;
//	default:
//		break;
//	}
//#endif //_SERVER

	return TRUE;
}

//应用物件上的特殊效果
void KDoodad::ApplyEffect(void)
{
	KGLOG_PROCESS_ERROR(m_pCell);

	if (m_DoodadInfo.m_bSetBlockCharacter)
	{
		BOOL bTemp = m_pCell->m_BaseInfo.dwBlockCharacter;
		m_pCell->m_BaseInfo.dwBlockCharacter = m_DoodadInfo.m_bBlockCharacter;
		m_DoodadInfo.m_bBlockCharacter = bTemp;
	}

	if (m_DoodadInfo.m_bSetPutObj)
	{
		BOOL bTemp = m_pCell->m_BaseInfo.dwPutObj;
		m_pCell->m_BaseInfo.dwPutObj = m_DoodadInfo.m_bPutObj;
		m_DoodadInfo.m_bPutObj = bTemp;
	}

	if (m_DoodadInfo.m_bSetStall)
	{
		BOOL bTemp = m_pCell->m_BaseInfo.dwStall;
		m_pCell->m_BaseInfo.dwStall = m_DoodadInfo.m_bStall;
		m_DoodadInfo.m_bStall = bTemp;
	}

	if (m_DoodadInfo.m_bSetSafe)
	{
		BOOL bTemp = m_pCell->m_BaseInfo.dwRest;
		m_pCell->m_BaseInfo.dwRest = m_DoodadInfo.m_bSafe;
		m_DoodadInfo.m_bSafe = bTemp;
	}

	if (m_DoodadInfo.m_bSetCellType)
	{
		int nTemp = m_pCell->m_BaseInfo.dwCellType;
		m_pCell->m_BaseInfo.dwCellType = m_DoodadInfo.m_CellType;
		m_DoodadInfo.m_CellType = nTemp;
	}

	if (m_DoodadInfo.m_bSetPassWidth)
	{
		int nTemp = m_pCell->m_BaseInfo.dwPassWidth;
		m_pCell->m_BaseInfo.dwPassWidth = m_DoodadInfo.m_PassWidth;
		m_DoodadInfo.m_PassWidth = nTemp;
	}

Exit0:

	return;
}

//反应用物件上的特殊效果
void KDoodad::UnApplyEffect(void)
{
	KGLOG_PROCESS_ERROR(m_pCell);

	if (m_DoodadInfo.m_bSetBlockCharacter)
	{
		BOOL bTemp = m_pCell->m_BaseInfo.dwBlockCharacter;
		m_pCell->m_BaseInfo.dwBlockCharacter = m_DoodadInfo.m_bBlockCharacter;
		m_DoodadInfo.m_bBlockCharacter = bTemp;
	}

	if (m_DoodadInfo.m_bSetPutObj)
	{
		BOOL bTemp = m_pCell->m_BaseInfo.dwPutObj;
		m_pCell->m_BaseInfo.dwPutObj = m_DoodadInfo.m_bPutObj;
		m_DoodadInfo.m_bPutObj = bTemp;
	}

	if (m_DoodadInfo.m_bSetStall)
	{
		BOOL bTemp = m_pCell->m_BaseInfo.dwStall;
		m_pCell->m_BaseInfo.dwStall = m_DoodadInfo.m_bStall;
		m_DoodadInfo.m_bStall = bTemp;
	}

	if (m_DoodadInfo.m_bSetSafe)
	{
		BOOL bTemp = m_pCell->m_BaseInfo.dwRest;
		m_pCell->m_BaseInfo.dwRest = m_DoodadInfo.m_bSafe;
		m_DoodadInfo.m_bSafe = bTemp;
	}

	if (m_DoodadInfo.m_bSetCellType)
	{
		int nTemp = m_pCell->m_BaseInfo.dwCellType;
		m_pCell->m_BaseInfo.dwCellType = m_DoodadInfo.m_CellType;
		m_DoodadInfo.m_CellType = nTemp;
	}

	if (m_DoodadInfo.m_bSetPassWidth)
	{
		int nTemp = m_pCell->m_BaseInfo.dwPassWidth;
		m_pCell->m_BaseInfo.dwPassWidth = m_DoodadInfo.m_PassWidth;
		m_DoodadInfo.m_PassWidth = nTemp;
	}

Exit0:

	return;
}

// 应用Doodad的障碍
BOOL KDoodad::ApplyObstacle()
{
	
	return FALSE;
}

// 将Doodad的障碍信息从地图上移除
BOOL KDoodad::RemoveObstacle()
{
	BOOL bRetCode = FALSE;
	bRetCode = TRUE;
Exit0:
	return bRetCode;
}

BOOL KDoodad::UpdateObstacle()
{
	BOOL bRetCode = FALSE;

	bRetCode = RemoveObstacle();
	KGLOG_CHECK_ERROR(bRetCode);

	bRetCode = ApplyObstacle();
	KGLOG_PROCESS_ERROR(bRetCode);

	bRetCode = TRUE;
Exit0:
	return bRetCode;
}

void KDoodad::DoPick(KCharacter* pCharacter)
{
	return;
}

void KDoodad::ChangeState(DOODAD_STATE eState)
{
	BOOL bRetCode = FALSE;

	switch(eState)
	{
	case dsIdle:
		DoIdle();
		break;
	case dsBeHit:
		DoBeHit();
		break;
	case dsAfterHit:
		DoAfterHit();
		break;
	case dsRemove:
//		bRetCode = g_SO3World.RemoveDoodad(this);
//		KGLOG_CHECK_ERROR(bRetCode);
//#ifdef _CLIENT
//		bRetCode = g_SO3World.DelDoodad(this);
//		KGLOG_CHECK_ERROR(bRetCode);
//#endif //_CLIENT
		break;
	default:
		KGLOG_PROCESS_ERROR(FALSE);
	}
	return ;
Exit0:
	return ;
}

void KDoodad::DoIdle()
{
	KG_PROCESS_ERROR(m_eState != dsRemove);

	m_eState = dsIdle;

Exit0:
	return;
}

void KDoodad::DoBeHit()
{
	KG_PROCESS_ERROR(m_eState != dsRemove);

	m_eState = dsBeHit;

Exit0:
	return;
}

void KDoodad::DoAfterHit()
{
	KG_PROCESS_ERROR(m_eState != dsRemove);

	m_eState = dsAfterHit;

Exit0:
	return;
}

