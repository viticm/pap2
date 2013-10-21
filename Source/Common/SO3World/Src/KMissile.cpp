#include "stdafx.h"
#include "KMissile.h"

#include "KMath.h"
#include "KRegion.h"
#include "KSO3World.h"

//所有函数的返回值，若非特殊说明，都遵循：TRUE－子弹不消亡；FALSE－子弹消亡，
#define KG_CHECK_VANISH(Condition)	if (!(Condition)) goto ProcessVanish;

KMissile::KMissile(void)
{
	//质量为零的物体不受重力加速度的影响
	m_nMass = 0;
}

KMissile::~KMissile(void)
{
	return;
}

BOOL KMissile::Init(void)
{
	BOOL bRetCode = FALSE;

	bRetCode = KSceneObject::Init();
	KGLOG_PROCESS_ERROR(bRetCode);

	m_nExistingFrame = 0;

	m_nCollidedTimes = 0;
	m_nCollideInterval = 0;		
	m_nCollideFrame = 0;
	m_bCollideFriend = FALSE;

	m_nDamageRadius = 0;

	m_pLauncher = NULL;
	m_dwLauncherID = 0;

	m_pSkill = NULL;
	m_pSubSkill = NULL;
	m_nFlyEventInterval = 0;
	m_nFlyEventFrame = 0;
	m_bCollideEvent = FALSE;
	m_bVanishEvent = FALSE;

	return TRUE;
Exit0:

	return FALSE;
}

BOOL KMissile::UnInit(void)
{
	BOOL bRetCode = FALSE;

	//删除子弹上的属性列表
	KAttribute* pAttribute = m_pAttribute;
	while (pAttribute)
	{
		KAttribute* pNext = pAttribute->m_pNext;

		bRetCode = g_SO3World.m_ObjectManager.m_AttributeSet.Delete(pAttribute);
		if (!bRetCode)
			KGLogPrintf(KGLOG_ERR, "Remove attribute from missile failed.");

		pAttribute = pNext;
	}

	m_pAttribute = NULL;

	return KSceneObject::UnInit();
}

//关于碰撞的规则：
//1	每次游戏循环，至少碰撞一次
//2 每次游戏循环，每个格子至多碰撞一次
BOOL KMissile::Activate(void)
{
	BOOL bRetCode = FALSE;

	//调用父类同名函数
	bRetCode = KSceneObject::CheckGameLoop();
	if (!bRetCode)
		return TRUE;

	//是否需要在子弹生命周期中调用周期性子技能
	if (m_nFlyEventInterval && m_nFlyEventFrame >= m_nExistingFrame)
	{
		bRetCode = OnEvent();
		KG_CHECK_VANISH(bRetCode);
		m_nFlyEventFrame -= m_nFlyEventInterval;
	}

	//检测剩下的格子的碰撞，顺便实现了每个游戏循环至少检测一次碰撞的规则：）
	if (m_nCollideInterval == 0)
	{
		//没有间隔，每帧都检测碰撞
		bRetCode = EnumDamageRange();
		KG_CHECK_VANISH(bRetCode);
	}
	else
	{
		//周期性检测碰撞
		if (m_nCollideFrame >= m_nExistingFrame)
		{
			bRetCode = EnumDamageRange();
			KG_CHECK_VANISH(bRetCode);
			m_nCollideFrame -= m_nCollideInterval;
		}
	}

	//如果帧数为零，则消亡
	m_nExistingFrame--;
	KG_CHECK_VANISH(m_nExistingFrame > 0);

	return TRUE;

ProcessVanish:
	OnVanish();

	return 	TRUE;
}

//
BOOL KMissile::EnumDamageRange()
{
	BOOL bResult = FALSE;
	KRegion* pRegion = NULL;

	ASSERT(m_nDamageRadius > 0);

	bResult |= EnumObject(m_pRegion);

	for (int nIndex = 0; nIndex < ALL_DIRECTION; nIndex++)
	{
		pRegion = m_pRegion->m_pConnectRegion[nIndex];
		bResult |= EnumObject(pRegion);
	}

	return bResult;
}

BOOL KMissile::InDamageRange(KSceneObject *pSceneObject)
{
	BOOL bResult = FALSE;
	int nDirection = g_GetDirection(m_nX, m_nY, pSceneObject->m_nX, pSceneObject->m_nY);
	int nDistance2 = g_GetDistance2(m_nX, m_nY, pSceneObject->m_nX, pSceneObject->m_nY);

	if (nDistance2 > (m_nDamageRadius * m_nDamageRadius))
		goto Exit0;

	if (nDirection < m_nStartAngle)
		goto Exit0;

	if (nDirection > m_nEndAngle)
		goto Exit0;

	bResult = TRUE;
Exit0:
	return bResult;
}

BOOL KMissile::EnumObject(KRegion *pRegion)
{
	BOOL bRetCode = FALSE;
	KSceneObjNode* pNode = NULL;
	KPlayer* pPlayer = NULL;
	KNpc* pNpc = NULL;
	KDoodad* pDoodad = NULL;
		
	//检测Player
	pNode = (KSceneObjNode*)pRegion->m_PlayerList.GetHead();
	while (pNode != NULL)
	{
		pPlayer = (KPlayer*)pNode->m_pSceneObject;
		KG_PROCESS_ERROR(pPlayer);

		bRetCode = InDamageRange(pNode->m_pSceneObject);
		pNode = (KSceneObjNode*)pNode->GetNext();
		if (!bRetCode)
			continue;

		bRetCode = pPlayer->OnCollided(this);
		if (bRetCode)
		{
			m_nCollidedTimes--;
			KG_CHECK_VANISH(m_nCollidedTimes > 0);
		}
	}

	//检测Npc
	pNode = (KSceneObjNode*)pRegion->m_NpcList.GetHead();
	while (pNode != NULL)
	{
		pNpc = (KNpc*)pNode->m_pSceneObject;
		KG_PROCESS_ERROR(pNpc);

		bRetCode = InDamageRange(pNode->m_pSceneObject);
		pNode = (KSceneObjNode*)pNode->GetNext();
		if (!bRetCode)
			continue;

		bRetCode = pNpc->OnCollided(this);
		if (bRetCode)
		{
			m_nCollidedTimes--;
			KG_CHECK_VANISH(m_nCollidedTimes > 0);
		}
	}

	//检测Doodad
	pNode = (KSceneObjNode*)pRegion->m_DoodadList.GetHead();
	while (pNode != NULL)
	{
		pDoodad = (KDoodad*)pNode->m_pSceneObject;
		KG_PROCESS_ERROR(pDoodad);

		bRetCode = InDamageRange(pNode->m_pSceneObject);
		pNode = (KSceneObjNode*)pNode->GetNext();
		if (!bRetCode)
			continue;

		bRetCode = pDoodad->OnCollided(this);
		if (bRetCode)
		{
			m_nCollidedTimes--;
			KG_CHECK_VANISH(m_nCollidedTimes > 0);
		}
	}

Exit0:
ProcessVanish:
	return TRUE;
}


//消亡处理
void KMissile::OnVanish(void)
{
	BOOL bRetCode = FALSE;

	//检测是否需要调用子技能
	if (m_bVanishEvent)
		OnEvent();

	//删除Region中的子弹
	KGLOG_PROCESS_ERROR(m_pRegion);

	bRetCode = m_pRegion->RemoveMissile(this);
	KGLOG_PROCESS_ERROR(bRetCode);

	//删除子弹自身
	bRetCode = UnInit();
	KGLOG_PROCESS_ERROR(bRetCode);

	g_SO3World.m_ObjectManager.m_MissileSet.Delete(this);

Exit0:

	return;  
}

//子技能事件
//返回FALSE表示子弹消失
BOOL KMissile::OnEvent(void)
{

	return TRUE;
}

BOOL KMissile::OnCollided(KMissile* pMissile)
{

	return FALSE;
}

