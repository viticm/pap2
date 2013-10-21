#include "stdafx.h"
#include "KNpc.h"
#include "KSO3World.h"
#include "KObjectManager.h"
#include "KRegion.h"
#include "KScene.h"
#include "KWorldSettings.h"
#include "KMath.h"
#include "KPlayerClient.h"
#include "KThreatList.h"
#include "KScriptCenter.h"
#include "KAttrModifier.h"
//#include "KShopCenter.h"
#include "KMonster.h"

DEFINE_LUA_CLASS_BEGIN(KNpc)
DEFINE_LUA_CLASS_END(KNpc)

#define NPC_ACTION_RANGE	(REGION_GRID_WIDTH * CELL_LENGTH * 3 / 4)

KNpc::KNpc(void)
{
}

KNpc::~KNpc(void)
{
}

BOOL KNpc::Init()
{
    BOOL bResult  = false;
	BOOL bRetCode = false;

	bRetCode = KCharacter::Init();
	KGLOG_PROCESS_ERROR(bRetCode);

	m_eKind             = ckInvalid;
	m_nMaxLife          = 1024;
	m_nMaxMana          = 1024;
	m_nCurrentLife      = 1024;
	m_nCurrentMana      = 1024;

	m_dwTemplateID      = 0;
    m_nAlarmRange       = 100;
    m_nReviveTime       = 100;
    m_eSpecies          = ensInvalid;
	m_dwRepresentID		= 0;
	m_nDisappearFrames	= 0; 
    
//    m_pShop             = NULL;

    bResult = true;

Exit0:

	return bResult;
}

BOOL KNpc::UnInit()
{
    //if (m_pShop)
    //{
    //    m_pShop = NULL;
    //}

	return KCharacter::UnInit();
}

BOOL KNpc::Activate(void)
{
	BOOL    bResult  = false;
	BOOL    bRetCode = false;

	bRetCode = KSceneObject::CheckGameLoop();
	KG_PROCESS_SUCCESS(!bRetCode);

	KGLOG_PROCESS_ERROR(m_pRegion);

	//调用父类同名函数
	KCharacter::Activate();
    // m_pScene指针为空表示自己被删除了
    KG_PROCESS_ERROR(m_pScene);

Exit1:
	bResult = true;
Exit0:
	return bResult;
}


#ifdef _CLIENT
void KNpc::SetDisplayData(DWORD dwTemplateID, int nLevel, DWORD dwRepresentID)
{
    BOOL                            bRetCode         = false;


	m_nLevel		  = nLevel;
	m_dwRepresentID	  = dwRepresentID;
Exit0:
    return;
}
#endif


BOOL KNpc::Save(KNPC_DATA* pNpcData)
{
	memset(pNpcData, 0, sizeof(KNPC_DATA));

	pNpcData->dwID				= m_dwID;
	pNpcData->dwTemplateID		= m_dwTemplateID;
	pNpcData->nX 				= m_nX;
	pNpcData->nY 				= m_nY;
	pNpcData->nZ 				= m_pCell->m_wHighLayer * POINT_PER_ALTITUDE + m_nZ;
	pNpcData->byLevel			= (BYTE)m_nLevel;
	pNpcData->nKind				= m_eKind;
	pNpcData->dwScriptID		= m_dwScriptID;
	pNpcData->byFaceDirection	= (BYTE)m_nFaceDirection;
//	pNpcData->byAIType			= (BYTE)m_AIController.GetAIType();
//	pNpcData->byAIMainState		= (BYTE)m_AIController.GetMainState();
	pNpcData->dwRepresentID		= m_dwRepresentID;

	strncpy(pNpcData->szName, m_szName, sizeof(pNpcData->szName));
	pNpcData->szName[sizeof(pNpcData->szName) - 1] = 0;

	return true;
}

BOOL KNpc::Load(const KNPC_DATA& crNpcData)
{
	BOOL bResult	= false;
	BOOL bRetCode	= false;

//	bRetCode = g_WorldSettings.m_NpcTemplateList.LoadFromTemplate(crNpcData.dwTemplateID, this);
//	KGLOG_PROCESS_ERROR(bRetCode);

	m_nX					= crNpcData.nX;
	m_nY					= crNpcData.nY;
	m_nZ					= crNpcData.nZ;
	m_dwTemplateID			= crNpcData.dwTemplateID;
	m_nLevel				= crNpcData.byLevel;
	m_eKind					= (CHARACTER_KIND)crNpcData.nKind;
	m_dwScriptID			= crNpcData.dwScriptID;
	m_nFaceDirection		= crNpcData.byFaceDirection;

	if (crNpcData.dwRepresentID)
	{
		m_dwRepresentID	= crNpcData.dwRepresentID;
	}
	strncpy(m_szName, crNpcData.szName, sizeof(m_szName));
	m_szName[sizeof(m_szName) - 1] = 0;
	//bRetCode = m_AIController.SetAIType((AI_TYPE)crNpcData.byAIType);
	//KGLOG_PROCESS_ERROR(bRetCode);

	//m_AIController.SetOriginPoint(crNpcData.nX, crNpcData.nY, crNpcData.nZ, crNpcData.byFaceDirection);
	//m_AIController.SetMainState((KAI_STATE)crNpcData.byAIMainState);

	bResult = true;
Exit0:
	return bResult;
}




#if defined(_CLIENT)
BOOL KNpc::GetCommand(void)
{
	return TRUE;
}
#endif	//_CLIENT

BOOL KNpc::ChangeRegion(KRegion* pRegion)
{
	BOOL bRetCode = false;


	//m_RegionObjNode.Remove();


	m_pRegion = pRegion;

	return true;
}

BOOL KNpc::ChangeCell(KCell* pCell)
{
	ASSERT(pCell);

	m_pCell = pCell;

	return TRUE;
}

int KNpc::GetNpcRelation(KNpc* pTarget)
{
	int nRelation = 0;
	ASSERT(!g_IsPlayer(pTarget->m_dwID));
	int nValue = g_WorldSettings.m_RelationForceList.GetRelation(m_dwForceID, pTarget->m_dwForceID);
	switch(nValue)
	{
	case -1:
		nRelation |= sortEnemy;
		break;
	case 1:
		nRelation |= sortAlly;
		break;
	default:
		break;
	}

	if (m_ThreatList.IsInThreatList(pTarget->m_dwID))
	{
		nRelation |= sortEnemy;
		nRelation &= ~sortAlly;
	}

	if (this == pTarget)
	{
		nRelation |= sortSelf;
		nRelation &= ~sortEnemy;
	}	

	return nRelation;
}


int KNpc::GetPlayerRelation(KPlayer* pTarget)
{
	int nRelation = 0;
	//ASSERT(g_IsPlayer(pTarget->m_dwID));
	//int nValue = pTarget->m_ReputeList.GetReputation(m_dwForceID);

	//if (nValue < ENEMY_REPUTE)
	//	nRelation |= sortEnemy;
	//else if (nValue > ALLY_REPUTE)
	//	nRelation |= sortAlly;
	//if (m_ThreatList.IsInThreatList(pTarget->m_dwID))
	//{
	//	nRelation |= sortEnemy;
	//	nRelation &= ~sortAlly;
	//}
	return nRelation;
}

BOOL KNpc::CallDeathScript(KCharacter* pKiller)
{
    BOOL            bResult     = false;
    BOOL            bRetCode    = false;
	KLuaScriptEx*   pScript     = NULL;
    int             nTopIndex   = 0;
    
    pScript = g_ScriptCenter.GetScript(m_dwScriptID);
    KG_PROCESS_SUCCESS(!pScript);

    bRetCode = pScript->IsFuncExist(SCRIPT_ON_NPC_DEATH);
    KG_PROCESS_SUCCESS(!bRetCode);

	pScript->SafeCallBegin(&nTopIndex);

    if (pKiller)
    {
	    if (pKiller->m_eKind == ckPlayer)
		    ((KPlayer*)pKiller)->LuaGetObj(pScript->GetLuaState());
	    else
		    ((KNpc*)pKiller)->LuaGetObj(pScript->GetLuaState());
    }
    else
    {
        lua_State* L = pScript->GetLuaState();

        KGLOG_PROCESS_ERROR(L);

        Lua_PushNil(L);
    }

	this->LuaGetObj(pScript->GetLuaState());

	pScript->CallFunction(SCRIPT_ON_NPC_DEATH, 0, "uu");

	pScript->SafeCallEnd(nTopIndex);

Exit1:
    bResult = true;
Exit0:
    return bResult;
}
