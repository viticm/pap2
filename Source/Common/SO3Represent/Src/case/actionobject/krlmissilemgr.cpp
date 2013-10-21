#include "stdafx.h"
#include "./krlmissile.h"
#include "./krlmissilemgr.h"
#include "./krlscene.h"
#include "./krlcharacter.h"
#include "./krldoodad.h"
#include "../gameworld/ktabletransform.h"
#include "../../common/kmathtools.h"
#include "../../SO3Represent.h"
#include "SO3World/IKGSO3WorldClient.h"
#include "../../Source/Common/SO3World/Src/KSceneObject.h"

namespace 
{
    unsigned long long ParamIDFromBulletIDAndTargetID(DWORD dwBulletID, DWORD dwTargetID)
    {
        ASSERT(sizeof(DWORD) == 4);
        return static_cast<unsigned long long>(dwBulletID) << 32 | dwTargetID;
    }

    DWORD BulletIDFromParamID(unsigned long long ullParamID)
    {
        ASSERT(sizeof(DWORD) == 4);
        return static_cast<DWORD>((ullParamID & 0xFFFFFFFF00000000) >> 32);
    }

    DWORD TargetIDFromParamID(unsigned long long ullParamID)
    {
        ASSERT(sizeof(DWORD) == 4);
        return static_cast<DWORD>(ullParamID & 0x00000000FFFFFFFF);
    }
}


KRLMissileMgr::KRLMissileMgr()
{
}

KRLMissileMgr::~KRLMissileMgr()
{
}

int KRLMissileMgr::Init()
{
    int nRetCode = false;
    int nResult = false;

    nRetCode = m_MissileAllocator.Init(sizeof(KRLMissile));
    KGLOG_PROCESS_ERROR(nRetCode);

    nResult = true;
Exit0:
	return nResult;
}

void KRLMissileMgr::Exit()
{
}

int KRLMissileMgr::Reset()
{
	int nRetCode = false;
    int nResult = false;

	Exit();

	nRetCode = Init();
	KGLOG_PROCESS_ERROR(nRetCode);

	nResult = true;
Exit0:
    if (!nResult)
    {
	    Exit();
    }
	return nResult;
}

int KRLMissileMgr::Activate(double fTime, double fTimeLast, DWORD dwGameLoop, BOOL bFrame)
{
    int nRetCode = false;
    int nResult = false;
	int nHasFinished = false;

    StartProfile(&g_pRL->m_Profile, KRLPROFILE_MISSILE);

    for (KRLMissilePtrList::iterator it = m_pRLMissiles.begin(); it != m_pRLMissiles.end(); ++it)
	{
        KRLMissile* pMissile = *it;

        if (!pMissile->Update(fTime, fTimeLast, dwGameLoop, bFrame))
		{
			KGLogPrintf(KGLOG_ERR, "KRLMissileMgr::OnActivate(%f, %f, %s)\n", fTime, fTimeLast, bFrame ? "TRUE" : "FALSE");
		}
		if (pMissile->IsFinished())
		{
			nHasFinished = true;
		}
	}

	if (nHasFinished)
	{
        // purge missiles

		for (KRLMissilePtrList::iterator it = m_pRLMissiles.begin(); it != m_pRLMissiles.end();)
		{
			KRLMissile* pMissile = *it;

            if (pMissile->IsFinished())
			{
                nRetCode = pMissile->UnInit();
                KGLOG_PROCESS_ERROR(nRetCode);

                m_MissileAllocator.Free(pMissile);
                
                it = m_pRLMissiles.erase(it);
			}
			else
			{
				++it;
			}
		}
	}

    // purge missile params

    for (KRLMissileParamMap::iterator it = m_RLMissileParams.begin(); it != m_RLMissileParams.end();)
    {
        KRLMissileParam& MissileParam = (*it).second;

        if (g_pRL->m_fTime > MissileParam.fDeadTime)
        {
            it = m_RLMissileParams.erase(it);
        }
        else
        {
            ++it;
        }
    }

	nResult = true;
Exit0:
    StopProfile(&g_pRL->m_Profile, KRLPROFILE_MISSILE);
	return nResult;
}


int KRLMissileMgr::UpdateResult(DWORD dwBulletID, KRepresentSkillEffectResult const* pResult)
{
    int nRetCode = false;
    int nResult = false;

	for (KRLMissilePtrList::iterator it = m_pRLMissiles.begin(); it != m_pRLMissiles.end(); ++it)
	{
        KRLMissile* pMissile = *it;

        if (pMissile->m_Param.Result.dwBulletID == dwBulletID)
        {
            memcpy(&pMissile->m_Param.Result, pResult, sizeof(KRepresentSkillEffectResult));

            nRetCode = pMissile->UpdateResult();
            KGLOG_PROCESS_ERROR(nRetCode);
        }
	}

    nResult = true;
Exit0:
    return nResult;
}

int KRLMissileMgr::CreateRLMissile(KRLMissile** ppMissile)
{
	int nRetCode = false;
    int nResult = false;
	KRLMissile* pMissile = NULL;
	
	ASSERT(ppMissile);
	ASSERT(*ppMissile == NULL);

    pMissile  = (KRLMissile*)m_MissileAllocator.Alloc();
	KGLOG_PROCESS_ERROR(pMissile);

    memset(pMissile, 0, sizeof(KRLMissile));

	m_pRLMissiles.push_back(pMissile);
	
    *ppMissile = pMissile;

	nResult = true;
Exit0:
    if (!nResult)
    {
        if (pMissile)
        {
            m_MissileAllocator.Free(pMissile);
            pMissile = NULL;
        }
    }
	return nResult;
}

int KRLMissileMgr::DuplicateRLMissile(KRLMissile* pMissile, KRLMissile** ppNewMissile)
{
    int nRetCode = false;
    int nResult = false;
    KRLMissile* pNewMissile = NULL;
    int nPushBack = false;

    KGLOG_PROCESS_ERROR(pMissile);
    KGLOG_PROCESS_ERROR(ppNewMissile);
    KGLOG_PROCESS_ERROR(*ppNewMissile == NULL);

    pNewMissile = (KRLMissile*)m_MissileAllocator.Alloc();
	KGLOG_PROCESS_ERROR(pNewMissile);

    pNewMissile->m_Param = pMissile->m_Param;
    pNewMissile->m_pRLScene = pMissile->m_pRLScene;
    pNewMissile->m_RepresentMissile = pMissile->m_RepresentMissile;

	m_pRLMissiles.push_back(pNewMissile);
    nPushBack = true;

    *ppNewMissile = pNewMissile;
    pNewMissile = NULL;

    nResult = true;
Exit0:
    if (!nResult)
    {
        if (pNewMissile)
        {
            if (nPushBack)
        	    m_pRLMissiles.remove(pNewMissile);

            m_MissileAllocator.Free(pNewMissile);
        }
    }
    return nResult;
}

int KRLMissileMgr::DestroyRLMissile(KRLMissile* pMissile)
{
    int nResult = false;

    KGLOG_PROCESS_ERROR(pMissile);

	m_pRLMissiles.remove(pMissile);

	nResult = true;
Exit0:
    if (pMissile)
    {
        m_MissileAllocator.Free(pMissile);
    }
	return nResult;
}

int KRLMissileMgr::InsertRLMissileParam(DWORD dwBulletID, DWORD dwTargetID, KRLMissileParam& Param)
{
    int nResult = false;
    KRLMissileParamMap::iterator it;
    unsigned long long ullParamID = 0;

    ullParamID = ParamIDFromBulletIDAndTargetID(dwBulletID, dwTargetID);

    it = m_RLMissileParams.find(ullParamID);
    KGLOG_PROCESS_ERROR(it == m_RLMissileParams.end());

    Param.fDeadTime = g_pRL->m_fTime + g_pRL->m_TableList.GetNumber(RL_NUMBER_MISSILE_LIVE_TIME);

    m_RLMissileParams.insert(std::make_pair(ullParamID, Param));

    nResult = true;
Exit0:
    return nResult;
}

int KRLMissileMgr::RemoveRLMissileParam(DWORD dwBulletID, DWORD dwTargetID)
{
    int nResult = false;
    KRLMissileParamMap::iterator it;
    unsigned long long ullParamID = 0;

    ullParamID = ParamIDFromBulletIDAndTargetID(dwBulletID, dwTargetID);

    it = m_RLMissileParams.find(ullParamID);
    KGLOG_PROCESS_ERROR(it != m_RLMissileParams.end());

    m_RLMissileParams.erase(it);

    nResult = true;
Exit0:
    return nResult;
}

KRLMissileParam* KRLMissileMgr::LookupRLMissileParam(DWORD dwBulletID, DWORD dwTargetID)
{
    KRLMissileParamMap::iterator it;
    KRLMissileParam* pRLMissileParam = NULL;
    unsigned long long ullParamID = 0;

    ullParamID = ParamIDFromBulletIDAndTargetID(dwBulletID, dwTargetID);

    it = m_RLMissileParams.find(ullParamID);
    KG_PROCESS_ERROR(it != m_RLMissileParams.end());

    pRLMissileParam = &(*it).second;

Exit0:
    return pRLMissileParam;
}

int PrepareAnimationTagMissileParam(KRLCharacter* pRLCaster, DWORD dwBulletID, DWORD dwMissileID, KRLMissileTagParam const* pTagParam)
{
    int nRetCode = false;
    int nResult = false;
    KRLScene* pRLScene = NULL;
    KRLMissileParam* pRLMissileParam = NULL;
	KRLMissileParamMap::iterator it;
    KRLMissileParamMap::iterator ie;

    KG_ASSERT_EXIT(pRLCaster);
    KG_ASSERT_EXIT(pTagParam);

    pRLScene = pRLCaster->m_RenderData.GetRLScene();
    KGLOG_PROCESS_ERROR(pRLScene);

	it = pRLScene->m_MissileMgr.m_RLMissileParams.lower_bound(ParamIDFromBulletIDAndTargetID(dwBulletID, 0));
    ie = pRLScene->m_MissileMgr.m_RLMissileParams.upper_bound(ParamIDFromBulletIDAndTargetID(dwBulletID, 0xFFFFFFFF));
    if (it == ie)
    {
        // 标签先到

        KRLMissileParam RLMissileParam;

        memset(&RLMissileParam, 0, sizeof(RLMissileParam));

        RLMissileParam.Result.dwBulletID = dwBulletID;

        RLMissileParam.dwMissileID = dwMissileID;

        memcpy(&RLMissileParam.TagParam, pTagParam, sizeof(KRLMissileTagParam));

        nRetCode = GetCharacterSkillCaster(pRLCaster, &RLMissileParam.Result.Caster);
        KGLOG_PROCESS_ERROR(nRetCode);

        nRetCode = pRLScene->m_MissileMgr.InsertRLMissileParam(dwBulletID, RL_STUB_TARGET_ID, RLMissileParam);
        KGLOG_PROCESS_ERROR(nRetCode);

        pRLMissileParam = pRLScene->m_MissileMgr.LookupRLMissileParam(dwBulletID, RL_STUB_TARGET_ID);

        // 播放子弹
        nRetCode = FireMissile(pRLCaster, pRLMissileParam);
        KGLOG_PROCESS_ERROR(nRetCode);
    }
    else
    {
        for (; it != ie; ++it)
        {
            // 伤害包先到，Caster 以伤害包为准

            KRLMissileParam& Param = it->second;
            DWORD dwTargetID = TargetIDFromParamID(it->first);

            Param.dwMissileID = dwMissileID;
            memcpy(&Param.TagParam, pTagParam, sizeof(KRLMissileTagParam));

            // 播放子弹
            nRetCode = FireMissile(pRLCaster, &Param);
            KG_PROCESS_ERROR(nRetCode);
        }
    }

    nResult = true;
Exit0:
    return nResult;
}

int PrepareEffectResultMissileParam(KRLCharacter* pRLTarget, KRepresentSkillEffectResult const* pResult)
{
    int nRetCode = false;
    int nResult = false;
    KRLScene* pRLScene = NULL;
    KRLMissileParam* pRLMissileParam = NULL;
    DWORD dwTargetID = 0;

    KG_ASSERT_EXIT(pRLTarget);
    KG_ASSERT_EXIT(pResult);

    pRLScene = pRLTarget->m_RenderData.GetRLScene();
    KGLOG_PROCESS_ERROR(pRLScene);

    dwTargetID = pRLTarget->GetObjectID();

    pRLMissileParam = pRLScene->m_MissileMgr.LookupRLMissileParam(pResult->dwBulletID, RL_STUB_TARGET_ID);
    if (pRLMissileParam)
    {
        KRLCharacter* pRLCaster = NULL;
        KRLMissileParam* pTargetRLMissileParam = NULL;

        // 标签先到
        
        // 如果只有存根子弹，那么生成新的子弹，并复制存根子弹的当前状态
        pTargetRLMissileParam = pRLScene->m_MissileMgr.LookupRLMissileParam(pResult->dwBulletID, dwTargetID);
        if (pTargetRLMissileParam == NULL)
        {
            // TODO: 这个机制是基于游戏逻辑一个子弹打多个目标的技能都是伤害包跟技能释放同时到

            memcpy(&pRLMissileParam->Result, pResult, sizeof(KRepresentSkillEffectResult));

            nRetCode = pRLScene->m_MissileMgr.InsertRLMissileParam(pResult->dwBulletID, dwTargetID, *pRLMissileParam);
            KGLOG_PROCESS_ERROR(nRetCode);

            nRetCode = pRLScene->m_MissileMgr.RemoveRLMissileParam(pResult->dwBulletID, RL_STUB_TARGET_ID);
            KGLOG_PROCESS_ERROR(nRetCode);
        }

        nRetCode = pRLScene->m_MissileMgr.UpdateResult(pResult->dwBulletID, pResult);
        KGLOG_PROCESS_ERROR(nRetCode);
    }
    else
    {
        // 伤害包先到

        KRLMissileParam RLMissileParam;

        memset(&RLMissileParam, 0, sizeof(RLMissileParam));
        memcpy(&RLMissileParam.Result, pResult, sizeof(KRepresentSkillEffectResult));

        nRetCode = pRLScene->m_MissileMgr.InsertRLMissileParam(pResult->dwBulletID, dwTargetID, RLMissileParam);
        KGLOG_PROCESS_ERROR(nRetCode);
    }
    
    nResult = true;
Exit0:
    return nResult;
}

int FireMissile(KRLCharacter* pRLCharacter, KRLMissileParam* pRLMissileParam)
{
    int nRetCode = false;
    int nResult = false;
    int nInitMissile = false;
    KRLScene* pRLScene = NULL;
    KRLMissile* pRLMissile = NULL;

    KG_ASSERT_EXIT(pRLCharacter);
    KG_ASSERT_EXIT(pRLMissileParam);

    pRLScene = pRLCharacter->m_RenderData.GetRLScene();
    KGLOG_PROCESS_ERROR(pRLScene);

    nRetCode = pRLScene->m_MissileMgr.CreateRLMissile(&pRLMissile);
    KGLOG_PROCESS_ERROR(nRetCode);

    nRetCode = pRLMissile->Init(pRLScene, pRLMissileParam);
    KGLOG_PROCESS_ERROR(nRetCode);

    nInitMissile = true;

    nRetCode = pRLMissile->Start();
    KG_PROCESS_ERROR(nRetCode);

    nResult = true;
Exit0:
    if (!nResult)
    {
        if (pRLMissile)
        {
            if (nInitMissile)
            {
                nRetCode = pRLMissile->UnInit();
                KGLOG_CHECK_ERROR(nRetCode);
            }

            nRetCode = pRLScene->m_MissileMgr.DestroyRLMissile(pRLMissile);
            KGLOG_CHECK_ERROR(nRetCode);
        }
    }
    return nResult;
}

int HitTarget(KRLMissileParam* pRLMissileParam)
{
    int nRetCode = false;
    int nResult = false;
	TARGET_TYPE eTargetType = ttInvalid;
    KRepresentSkillEffectResult* pResult = NULL;
    KCaster* pCaster = NULL;
    KTargetData* pTarget = NULL;

    KG_ASSERT_EXIT(pRLMissileParam);
    
    pResult = &pRLMissileParam->Result;
    pCaster = &pResult->Caster;
    pTarget = &pResult->Target;

	eTargetType = pTarget->m_eTargetType;

	switch (eTargetType)
	{
    case ttInvalid:
        break;
	case ttCoordination:
		{
			int nX = 0;
			int nY = 0;
			int nZ = 0;
            D3DXVECTOR3 vPosition;
            
			nRetCode = g_pRL->m_pSO3WorldClient->GetTarget(const_cast<KTargetData*>(pTarget), &nX, &nY, &nZ);
            KGLOG_PROCESS_ERROR(nRetCode);

			GameWorldCellPointToScenePoint(vPosition, nX, nY, nZ, TRUE);

            // TODO: 地表贴花？
		}
		break;
	case ttNpc:
    case ttPlayer:
		{
			KRLCharacter* pRLTarget = NULL;
            KRLScene* pRLScene = NULL;
			LPCTSTR szBoneName = NULL;
            KSceneObject* pSceneObject = NULL;

            nRetCode = g_pRL->m_pSO3WorldClient->GetTarget(pTarget, &pSceneObject);
            KGLOG_PROCESS_ERROR(nRetCode);
            KGLOG_PROCESS_ERROR(pSceneObject);
            
			pRLTarget = reinterpret_cast<KRLCharacter*>(pSceneObject->m_pRepresentObject);
			KGLOG_PROCESS_ERROR(pRLTarget);

            pRLScene = pRLTarget->m_RenderData.GetRLScene();
            KGLOG_PROCESS_ERROR(pRLScene);
            
            pRLScene->m_SkillEffectResult.InvokeResult(pCaster->dwCharacterID, pTarget->m_dwID, pResult->dwBulletID, pRLMissileParam->TagParam.fPercent);
		}
		break;
	case ttDoodad:
		{
			KRLDoodad* pRLDoodad = NULL;
            KSceneObject* pSceneObject = NULL;
            D3DXVECTOR3 vPosition;

            nRetCode = g_pRL->m_pSO3WorldClient->GetTarget(const_cast<KTargetData*>(pTarget), &pSceneObject);
            KGLOG_PROCESS_ERROR(nRetCode);
            KGLOG_PROCESS_ERROR(pSceneObject);
            
            pRLDoodad = reinterpret_cast<KRLDoodad*>(pSceneObject->m_pRepresentObject);
			KGLOG_PROCESS_ERROR(pRLDoodad);

			pRLDoodad->GetPosition(vPosition);

            // TODO: 地表贴花？
		}
		break;
	default:
		ASSERT(0);
		break;
	}

    nResult = true;
Exit0:
    return nResult;
}

KRLCharacter* GetCasterRLCharacter(KCaster const& Caster)
{
    KRLCharacter* pRLCharacter = NULL;

    switch (Caster.eTargetType)
	{
	case ttPlayer:
    case ttNpc:
        pRLCharacter = GetRLCharacter(Caster.dwCharacterID);
		break;
    case ttInvalid:
    case ttNoTarget:
    case ttCoordination:
    case ttDoodad:
    case ttItem:
    case ttItemPos:
        break;
	default:
		ASSERT(0);
		break;
	};

    return pRLCharacter;
}

