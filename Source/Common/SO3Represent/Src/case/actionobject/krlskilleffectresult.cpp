#include "stdafx.h"
#include "SO3UI/SO3GameWorldUIHandler.h"
#include "SO3World/IKGSO3WorldClient.h"
#include "../actionobject/krlscene.h"
#include "../actionobject/krlremotecharacter.h"
#include "../../SO3Represent.h"

namespace 
{
    unsigned long long CacheIDFromCasterIDAndTargetID(DWORD dwCasterID, DWORD dwTargetID)
    {
        ASSERT(sizeof(DWORD) == 4);
        return static_cast<unsigned long long>(dwCasterID) << 32 | dwTargetID;
    }

    DWORD CasterIDFromCacheID(unsigned long long ullCacheID)
    {
        ASSERT(sizeof(DWORD) == 4);
        return static_cast<DWORD>((ullCacheID & 0xFFFFFFFF00000000) >> 32);
    }

    DWORD TargetIDFromCacheID(unsigned long long ullCacheID)
    {
        ASSERT(sizeof(DWORD) == 4);
        return static_cast<DWORD>(ullCacheID & 0x00000000FFFFFFFF);
    }
}

KRLSkillEffectResult::KSkillEffectResultData::KSkillEffectResultData()
: m_fTime(0.0)
, m_fPercent(1.0f)
{
	memset(m_aValue, 0, sizeof(m_aValue));
	memset(&m_Result, 0, sizeof(m_Result));
}

BOOL KRLSkillEffectResult::KSkillEffectResultData::IsFinished(double fTime) const
{
    if (fTime - m_fTime >= g_pRL->m_TableList.GetNumber(RL_NUMBER_SKILL_EFFECT_RESULT_TIMEOUT))
		return TRUE;
	return FALSE;
}

void KRLSkillEffectResult::KSkillEffectResultData::InvokeResult(DWORD dwCasterID, DWORD dwTargetID, float fPercent)
{
    int nRetCode = false;
    IKSO3GameWorldUIHandler* pGameWorldUIHandler = NULL;
    
    pGameWorldUIHandler = g_pRL->m_pGameWorldUIHandler;
    KGLOG_PROCESS_ERROR(pGameWorldUIHandler);

	if (fPercent > m_fPercent)
		fPercent = m_fPercent;

	m_fPercent -= fPercent;

	if (dwTargetID != RL_STUB_TARGET_ID)
	{
		KRLCharacter* pRLCharacter = GetRLCharacter(dwTargetID);
		if (pRLCharacter)
        {
			nRetCode = PlaySkillEffectResult(pRLCharacter, &m_Result);
            KGLOG_CHECK_ERROR(nRetCode);
        }

		BOOL bLastPercent = abs(m_fPercent) <= FLT_EPSILON;

		for (int nType = 0; nType < serTotal; ++nType)
		{
			if (m_Result.aSkillResultValue[nType] == 0)
				continue;

			int nValue = bLastPercent ? m_aValue[nType] : (int)((float)m_Result.aSkillResultValue[nType] * fPercent);

			m_aValue[nType] -= nValue;

			ASSERT(m_aValue[nType] >= 0);

			if (nValue != 0)
			{
				KUIEventSkillEffectText SkillEffectText;

				SkillEffectText.dwCasterID = dwCasterID;
				SkillEffectText.dwTargetID = dwTargetID;
                SkillEffectText.dwSkillID = m_Result.dwSkillID;
                SkillEffectText.dwSkillLevel = m_Result.dwSkillLevel;
				SkillEffectText.nType = nType;
				SkillEffectText.bCriticalStrike = m_Result.bCriticalStrike;
				SkillEffectText.nValue = nValue;

				pGameWorldUIHandler->OnSkillEffectText(SkillEffectText);
			}
		}
	}

Exit0:
    return;
}

KRLSkillEffectResult::KRLSkillEffectResult()
{
} 

KRLSkillEffectResult::~KRLSkillEffectResult()
{
	m_aData.clear();
}


BOOL KRLSkillEffectResult::Activate(double fTime, double fTimeLast, DWORD dwGameLoop, BOOL bFrame)
{   
    StartProfile(&g_pRL->m_Profile, KRLPROFILE_SKILL_EFFECT_RESULT);

    KSkillEffectResultDataMap::iterator it = m_aData.begin();
    for (; it != m_aData.end();)
    {
		KSkillEffectResultData& Data = it->second;

        if (Data.IsFinished(fTime))
            m_aData.erase(it++);
        else
            ++it;
    }

    StopProfile(&g_pRL->m_Profile, KRLPROFILE_SKILL_EFFECT_RESULT);
    return TRUE;
}


HRESULT KRLSkillEffectResult::Append(DWORD dwCasterID, DWORD dwTargetID, KRepresentSkillEffectResult const* pResult)
{   
	ASSERT(pResult);

	BOOL bExist = FALSE;

	unsigned long long ullCacheID = CacheIDFromCasterIDAndTargetID(dwCasterID, dwTargetID);

	KSkillEffectResultDataMapEqualRangeResult result = m_aData.equal_range(ullCacheID);
	if (result.first != result.second)
	{
		for (KSkillEffectResultDataMap::iterator it = result.first; it != result.second; ++it)
		{
			KSkillEffectResultData& Data = it->second;

			if (Data.m_Result.dwBulletID == pResult->dwBulletID)
			{
				bExist = TRUE;
				break;
			}
		}
	}

	if (!bExist)
	{
		// 创建新的伤害包

		KSkillEffectResultData Data;

		Data.m_Result = *pResult;

		Data.m_fPercent = 1.0f;
		Data.m_fTime = g_pRL->m_fTime;

		if (dwTargetID != RL_STUB_TARGET_ID)
		{
			ASSERT(sizeof(Data.m_aValue) == sizeof(pResult->aSkillResultValue));

			memcpy(Data.m_aValue, pResult->aSkillResultValue, sizeof(Data.m_aValue));
		}

		// 检查是否动作标签先到，伤害包后到的情况

		unsigned long long ullStubCacheID = CacheIDFromCasterIDAndTargetID(dwCasterID, RL_STUB_TARGET_ID);
		result = m_aData.equal_range(ullStubCacheID);

		for (KSkillEffectResultDataMap::iterator it = result.first; it != result.second; ++it)
		{
			// 动作标签先播，把存根百分比一次播放

			KSkillEffectResultData& StubData = it->second;
		
			if (StubData.m_Result.dwBulletID == Data.m_Result.dwBulletID)
			{
				Data.InvokeResult(dwCasterID, dwTargetID, 1.0f - StubData.m_fPercent);

				break;
			}
		}

		m_aData.insert(make_pair(ullCacheID, Data));
	}
    return S_OK;
}

void KRLSkillEffectResult::InvokeResult(DWORD dwCasterID, DWORD dwBulletID, float fPercent)
{
	BOOL bExist = FALSE;

	KSkillEffectResultDataMap::iterator it = m_aData.lower_bound(CacheIDFromCasterIDAndTargetID(dwCasterID, 0));
	KSkillEffectResultDataMap::iterator ie = m_aData.upper_bound(CacheIDFromCasterIDAndTargetID(dwCasterID, 0xFFFFFFFF));

	for (; it != ie; ++it)
	{
		// 伤害包已到，正常播放

		KSkillEffectResultData& Data = it->second;
		DWORD dwTargetID = TargetIDFromCacheID(it->first);

		if (Data.m_Result.dwBulletID == dwBulletID)
		{
			Data.InvokeResult(dwCasterID, dwTargetID, fPercent);

			bExist = TRUE;
		}
	}
	
	if (!bExist)
	{
		// 标签先到，检查是否已有存根

		BOOL bExistStub = FALSE;

		unsigned long long ullStubCacheID = CacheIDFromCasterIDAndTargetID(dwCasterID, RL_STUB_TARGET_ID);
		KSkillEffectResultDataMapEqualRangeResult result = m_aData.equal_range(ullStubCacheID);

		for (it = result.first; it != result.second; ++it)
		{
			KSkillEffectResultData& Data = it->second;

			if (Data.m_Result.dwBulletID == dwBulletID)
			{
				// 已有存根，表示相同子弹多次触发标签，而伤害包还没到

				Data.InvokeResult(dwCasterID, RL_STUB_TARGET_ID, fPercent);

				bExistStub = TRUE;
			}
		}

		if (!bExistStub)
		{
			// 存根不存在，新建一个存根

			KRepresentSkillEffectResult Result;
			memset(&Result, 0, sizeof(Result));

			Result.dwBulletID = dwBulletID;

			Append(dwCasterID, RL_STUB_TARGET_ID, &Result);

			// 触发本次标签
			result = m_aData.equal_range(ullStubCacheID);

			for (it = result.first; it != result.second; ++it)
			{
				KSkillEffectResultData& Data = it->second;

				if (Data.m_Result.dwBulletID == dwBulletID)
				{
					Data.InvokeResult(dwCasterID, RL_STUB_TARGET_ID, fPercent);
				}
			}
		}
	}
}

void KRLSkillEffectResult::InvokeResult(DWORD dwCasterID, DWORD dwTargetID, DWORD dwBulletID, float fPercent)
{
	BOOL bExist = FALSE;

    KSkillEffectResultDataMapEqualRangeResult result = m_aData.equal_range(CacheIDFromCasterIDAndTargetID(dwCasterID, dwTargetID));

    for (KSkillEffectResultDataMap::iterator it = result.first; it != result.second; ++it)
	{
		// 伤害包已到，正常播放

		KSkillEffectResultData& Data = it->second;

		ASSERT(dwTargetID == TargetIDFromCacheID(it->first));

		if (Data.m_Result.dwBulletID == dwBulletID)
		{
			Data.InvokeResult(dwCasterID, dwTargetID, fPercent);

			bExist = TRUE;
		}
	}
	
	if (!bExist)
	{
		// 标签先到，检查是否已有存根

		BOOL bExistStub = FALSE;

		unsigned long long ullStubCacheID = CacheIDFromCasterIDAndTargetID(dwCasterID, RL_STUB_TARGET_ID);
		result = m_aData.equal_range(ullStubCacheID);

		for (KSkillEffectResultDataMap::iterator it = result.first; it != result.second; ++it)
		{
			KSkillEffectResultData& Data = it->second;

			if (Data.m_Result.dwBulletID == dwBulletID)
			{
				// 已有存根，表示相同子弹多次触发标签，而伤害包还没到

				Data.InvokeResult(dwCasterID, RL_STUB_TARGET_ID, fPercent);

				bExistStub = TRUE;
			}
		}

		if (!bExistStub)
		{
			// 存根不存在，新建一个存根

			KRepresentSkillEffectResult Result;
			memset(&Result, 0, sizeof(Result));

			Result.dwBulletID = dwBulletID;

			Append(dwCasterID, RL_STUB_TARGET_ID, &Result);

			// 触发本次标签
			result = m_aData.equal_range(ullStubCacheID);

			for (KSkillEffectResultDataMap::iterator it = result.first; it != result.second; ++it)
			{
				KSkillEffectResultData& Data = it->second;

				if (Data.m_Result.dwBulletID == dwBulletID)
				{
					Data.InvokeResult(dwCasterID, RL_STUB_TARGET_ID, fPercent);
				}
			}
		}
	}
}

BOOL KRLSkillEffectResult::GetTargetID(KTargetData const& target, DWORD& dwTargetID)
{
    KCharacter* pCharacter = NULL;

    switch (target.m_eTargetType)
    {
    case ttInvalid:
    case ttNoTarget:
    case ttCoordination:
        break;
    case ttNpc:
    case ttPlayer:
        g_pRL->m_pSO3WorldClient->GetTarget(const_cast<KTargetData*>(&target), &pCharacter);
        break;
    case ttDoodad:
    case ttItem:
    case ttItemPos:
    default:
        ASSERT(0);
        break;
    }

    if (pCharacter == NULL)
        return FALSE;

    dwTargetID = pCharacter->m_dwID;
    return TRUE;
}

int RegisterSkillEffectResult(KRLCharacter* pRLTarget, KRepresentSkillEffectResult const* pResult)
{
    int nRetCode = false;
    int nResult = false;
    HRESULT hr = E_FAIL;
    KRLScene* pRLScene = NULL;
    DWORD dwCasterID = 0;
    DWORD dwTargetID = 0;

    KG_ASSERT_EXIT(pRLTarget);
    KG_ASSERT_EXIT(pResult);

    ASSERT(!pResult->bImmediately);

    pRLScene = pRLTarget->m_RenderData.GetRLScene();
    KGLOG_PROCESS_ERROR(pRLScene);

    dwCasterID = pResult->Caster.dwCharacterID;
    dwTargetID = pRLTarget->GetObjectID();

    hr = pRLScene->m_SkillEffectResult.Append(dwCasterID, dwTargetID, pResult);
    KGLOG_COM_PROCESS_ERROR(hr);

    nRetCode = PrepareEffectResultMissileParam(pRLTarget, pResult);
    KGLOG_PROCESS_ERROR(nRetCode);

    nResult = true;
Exit0:
    if (!nResult)
    {
    }
    return nResult;
}

static int GetEffectPlayType(int nEffectPlayType, BOOL* pbAnimation, BOOL* pbSFX)
{
    int nResult = false;

    KGLOG_PROCESS_ERROR(pbAnimation);
    KGLOG_PROCESS_ERROR(pbSFX);

    switch (nEffectPlayType)
    {
    case septInvalid:
        *pbAnimation = FALSE;
        *pbSFX = FALSE;

        KG_ASSERT_EXIT(0);
        break;
    case septAniOnly:
        *pbAnimation = TRUE;
        *pbSFX = FALSE;
        break;
    case septSfxOnly:
        *pbAnimation = FALSE;
        *pbSFX = TRUE;
        break;
    case septAniAndSfx:
        *pbAnimation = TRUE;
        *pbSFX = TRUE;
        break;
    case septNone:
        *pbAnimation = FALSE;
        *pbSFX = FALSE;
        break;
    default:
        *pbAnimation = FALSE;
        *pbSFX = FALSE;

        KG_ASSERT_EXIT(0);
    }

    nResult = true;
Exit0:
    return nResult;
}

int PlaySkillEffectResult(KRLCharacter* pRLTarget, KRepresentSkillEffectResult* pResult)
{
    int nRetCode = false;
    int nResult = false;
    HRESULT hr = E_FAIL;
    RL_ANIMATION_UP nAnimationUp = RL_ANIMATION_UP_UNKNOWN;
    RL_ANIMATION_TABLE nAnimationTable = RL_ANIMATION_TABLE_NONE;
    int nCount = 0;
    int nEffectIndices[CHARACTER_SFX_SKILL_EFFECT_COUNT];
    DWORD dwEffectIDs[CHARACTER_SFX_SKILL_EFFECT_COUNT];
    BOOL bPlayAnimation = FALSE;
    BOOL bPlaySFX = FALSE;

    KG_ASSERT_EXIT(pRLTarget);
    KG_ASSERT_EXIT(pResult);

    KG_PROCESS_SUCCESS(pResult->nEffectPlayType == septNone);

    nRetCode = GetEffectPlayType(pResult->nEffectPlayType, &bPlayAnimation, &bPlaySFX);
    KGLOG_PROCESS_ERROR(nRetCode);

    for (int nIndex = 0; nIndex < CHARACTER_SFX_SKILL_EFFECT_COUNT; ++nIndex)
        nEffectIndices[nIndex] = -1;

    if (pResult->bHitTarget)
    {
        //  处理各种被击情况
        if (pResult->bSkillDodge)
        { 
            // 闪避

            nEffectIndices[nCount++] = REPRESENT_SKILL_EFFECT_RESULT_DODGE;
            nAnimationUp = RL_ANIMATION_UP_DODGE;
            nAnimationTable = RL_ANIMATION_TABLE_DODGE;
        }
        else if (pResult->bParry)
        {
            // 格挡

            nEffectIndices[nCount++] = REPRESENT_SKILL_EFFECT_RESULT_PARRY;
            nAnimationUp = RL_ANIMATION_UP_DODGE;
            nAnimationTable = RL_ANIMATION_TABLE_PARRY;
        }
        else
        {
            // 重击

            if (nCount < CHARACTER_SFX_SKILL_EFFECT_COUNT && pResult->bCriticalStrike)
                nEffectIndices[nCount++] = REPRESENT_SKILL_EFFECT_RESULT_CRITICAL_STRIKE;

            // 各种免疫或者伤害
            for (int nIndex = serPhysicsDamage; nIndex <= serPoisonDamage && nCount < CHARACTER_SFX_SKILL_EFFECT_COUNT; ++nIndex)
            {
                ASSERT(!pResult->aSkillBlockFlag[nIndex] || !pResult->aSkillResultFlag[nIndex]);

                if (pResult->aSkillBlockFlag[nIndex])
                    nEffectIndices[nCount++] = REPRESENT_SKILL_EFFECT_RESULT_BLOCK_BEGIN + nIndex - serPhysicsDamage;
                else if (pResult->aSkillResultFlag[nIndex])
                    nEffectIndices[nCount++] = REPRESENT_SKILL_EFFECT_RESULT_DAMAGE_BEGIN + nIndex - serPhysicsDamage;
            }

            // 特殊

            for (int nIndex = serReflectiedDamage; nIndex <= serShieldDamage && nCount < CHARACTER_SFX_SKILL_EFFECT_COUNT; ++nIndex)
            {
                if (pResult->aSkillResultFlag[nIndex])
                {
                    nEffectIndices[nCount++] = REPRESENT_SKILL_EFFECT_RESULT_SPECIAL_BEGIN + nIndex - serReflectiedDamage;
                }
            }

            if (nEffectIndices[0] != -1)
            {
                nAnimationUp = RL_ANIMATION_UP_BEHIT;
                nAnimationTable = (RL_ANIMATION_TABLE)(nEffectIndices[0] + RL_ANIMATION_TABLE_PHYSICS_BLOCK);
            }
            else
            {
                // 默认无任何属性的被击特效

                nEffectIndices[0] = REPRESENT_SKILL_EFFECT_RESULT_HIT;
                nAnimationUp = RL_ANIMATION_UP_BEHIT;
                nAnimationTable = RL_ANIMATION_TABLE_NONE;
            }
        }
    }
    else
    { 
        // Miss

        nEffectIndices[nCount++] = REPRESENT_SKILL_EFFECT_RESULT_MISS;
    }

    if (bPlaySFX && GetSkillEffectID(dwEffectIDs, nEffectIndices, CHARACTER_SFX_SKILL_EFFECT_COUNT, pResult->Caster, pResult->dwSkillID, pResult->dwSkillLevel))
    {
        nRetCode = pRLTarget->PlaySkillEffectResultSFX(dwEffectIDs, pResult->Caster.dwCharacterID);
        KGLOG_PROCESS_ERROR(nRetCode);
    }

	// 播放被击动作
	if (bPlayAnimation && nAnimationTable != RL_ANIMATION_TABLE_NONE)
	{
		KSkillCasterModel const* pSkillCasterModel = NULL;
        
        pSkillCasterModel = GetCharacterSkillCasterModel(pResult->Caster, pResult->dwSkillID, pResult->dwSkillLevel);

        nRetCode = pRLTarget->PlaySkillEffectResultAnimation(nAnimationUp, nAnimationTable, pSkillCasterModel);
        KGLOG_PROCESS_ERROR(nRetCode);
	}

Exit1:
    nResult = true;
Exit0:
    return nResult;
}

