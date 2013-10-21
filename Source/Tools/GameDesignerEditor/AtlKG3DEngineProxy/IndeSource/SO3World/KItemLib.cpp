#include "stdafx.h"
#include "KItem.h"
#include "KItemLib.h"
#include "KSO3World.h"
#include "Engine/KMemory.h"

BOOL KItemLib::Init(int nVersion)
{
	BOOL bResult    = false;
	BOOL bRetCode   = false;
	char szFile[MAX_PATH];

	KGLOG_PROCESS_ERROR(nVersion >= 0 && nVersion <= CURRENT_ITEM_VERSION);

	snprintf(szFile, sizeof(szFile), "%s/%03d/%s", ITEM_SETTING_DIR, nVersion, CUST_WEAPON_FILE);
    szFile[sizeof(szFile) - 1] = '\0';

	bRetCode = m_CustomWeapon.Load(szFile);
	KGLOG_PROCESS_ERROR(bRetCode);

	snprintf(szFile, sizeof(szFile), "%s/%03d/%s", ITEM_SETTING_DIR, nVersion, CUST_ARMOR_FILE);
    szFile[sizeof(szFile) - 1] = '\0';

	bRetCode = m_CustomArmor.Load(szFile);
	KGLOG_PROCESS_ERROR(bRetCode);

	snprintf(szFile, sizeof(szFile), "%s/%03d/%s", ITEM_SETTING_DIR, nVersion, CUST_TRINKET_FILE);
    szFile[sizeof(szFile) - 1] = '\0';

	bRetCode = m_CustomTrinket.Load(szFile);
	KGLOG_PROCESS_ERROR(bRetCode);

	snprintf(szFile, sizeof(szFile), "%s/%03d/%s", ITEM_SETTING_DIR, nVersion, COMMON_ITEM_FILE);
    szFile[sizeof(szFile) - 1] = '\0';

	bRetCode = m_Other.Load(szFile);
	KGLOG_PROCESS_ERROR(bRetCode);

	snprintf(szFile, sizeof(szFile), "%s/%03d/%s", ITEM_SETTING_DIR, nVersion, ATTRIB_FILE);
    szFile[sizeof(szFile) - 1] = '\0';

	bRetCode = m_Attrib.Load(szFile);
	KGLOG_PROCESS_ERROR(bRetCode);

	snprintf(szFile, sizeof(szFile), "%s/%03d/%s", ITEM_SETTING_DIR, nVersion, SET_FILE);
    szFile[sizeof(szFile) - 1] = '\0';

	bRetCode = m_Set.Load(szFile);
	KGLOG_PROCESS_ERROR(bRetCode);

	snprintf(szFile, sizeof(szFile), "%s/%03d/%s", ITEM_SETTING_DIR, nVersion, ATTRIB_FILE);
    szFile[sizeof(szFile) - 1] = '\0';

	bRetCode = m_AttribGroup.Init(szFile);
	KGLOG_PROCESS_ERROR(bRetCode);

#ifdef _CLIENT
	bRetCode = InsertSetEquipList(&m_CustomWeapon);
	KGLOG_PROCESS_ERROR(bRetCode);

	bRetCode = InsertSetEquipList(&m_CustomArmor);
	KGLOG_PROCESS_ERROR(bRetCode);

	bRetCode = InsertSetEquipList(&m_CustomTrinket);
	KGLOG_PROCESS_ERROR(bRetCode);
#endif

	m_nVerion = nVersion;

	bResult = true;

Exit0:
	return bResult;
}

void KItemLib::UnInit()
{
    m_CustomWeapon.Clear();
    m_CustomArmor.Clear();
    m_CustomTrinket.Clear();

    m_Other.Clear();
    m_Attrib.Clear();
    m_Set.Clear();
    m_AttribGroup.UnInit();
}

BOOL KItemLib::GenerateItem(
    KItem* pItem,	  
    DWORD  dwTabType, 
    DWORD  dwIndex, 
    DWORD  dwId/* = 0*/,
    DWORD  dwRandSeed/* = 0 */,
    int    nMagicNum/* = 0 */,
    int    nQuality/* = 0 */,
    int    nMagic[]/* = NULL */,
    time_t nGenTime/* = 0*/
)
{
	BOOL          bResult       = false;
	return bResult;
}

KItemInfo* KItemLib::GetItemInfo(DWORD dwTabType, DWORD dwIndex)
{
    KItemInfo* pRetItemInfo = NULL;

	switch(dwTabType)
	{
	case ittOther:
		pRetItemInfo = m_Other.GetItemInfo(dwIndex);
        break;

	case ittCustWeapon:
		pRetItemInfo = m_CustomWeapon.GetEquipInfo(dwIndex);
        break;

	case ittCustArmor:
		pRetItemInfo = m_CustomArmor.GetEquipInfo(dwIndex);
        break;

	case ittCustTrinket:
		pRetItemInfo = m_CustomTrinket.GetEquipInfo(dwIndex);
        break;

	case ittAttrib:
	case ittSet:
	default:
		KGLOG_PROCESS_ERROR(false);
        break;
	}

Exit0:
	return pRetItemInfo;
}

KAttribInfo* KItemLib::GetAttribInfo(int nAttribIndex)
{
	return m_Attrib.GetItemInfo(nAttribIndex);	
}

KSetInfo* KItemLib::GetSetInfo(DWORD dwID)
{
	return m_Set.GetItemInfo(dwID);
}

BOOL KItemLib::GenerateCommonItem(KItem* pItem, KOtherItemInfo* pInfo, DWORD dwRandSeed)
{
	BOOL bResult = false;

	assert(pItem);
    assert(pInfo);

	pItem->m_GenParam.dwRandSeed    = dwRandSeed;
    pItem->m_GenParam.nQuality      = pInfo->nQuality;
    
    strncpy(pItem->m_Common.szItemName, pInfo->szName, ITEM_NAME_LEN);
    pItem->m_Common.szItemName[ITEM_NAME_LEN - 1] = '\0';

	pItem->m_Common.nGenre              = pInfo->nGenre;
	pItem->m_Common.nSub                = pInfo->nSub;
	pItem->m_Common.nDetail             = pInfo->nDetail;
	pItem->m_Common.nPrice              = pInfo->nPrice;
	pItem->m_Common.nLevel              = pInfo->nLevel;
	pItem->m_Common.bCanTrade           = pInfo->bCanTrade;
	pItem->m_Common.bCanDestroy         = pInfo->bCanDestroy;
    pItem->m_Common.nMaxExistAmount     = pInfo->nMaxExistAmount;
    
    pItem->m_Common.bConsumeDurabiltity = pInfo->bCanConsume;
	pItem->m_Common.nBindType           = pInfo->nBindType;
    pItem->m_Common.bStack              = pInfo->bCanStack;
	pItem->m_Common.nAbradeRate         = 0;		
    pItem->m_Common.dwSetID             = 0;
    
    pItem->m_nMaxStackNum       = pInfo->nMaxDurability;
	pItem->m_nCurrentDurability = pInfo->nMaxDurability;
    pItem->m_dwScriptID         = pInfo->dwScriptID;
	pItem->m_dwSkillID          = pInfo->dwSkillID;
	pItem->m_dwSkillLevel       = pInfo->dwSkillLevel;

#ifdef _CLIENT
	pItem->m_nUiId = pInfo->nUiId;
#endif
	
	bResult = true;
Exit0:
	return bResult;
}

BOOL KItemLib::GenerateCustomItem(KItem* pItem, KCustomEquipInfo* pInfo, DWORD dwRandSeed)
{
	BOOL bResult  = false;
	BOOL bRetCode = false;

	assert(pItem);
	assert(pInfo);
    
	if (dwRandSeed == 0)
	{
		dwRandSeed = g_GetRandomSeed();
	}
	else
	{
		g_RandomSeed(dwRandSeed);
	}

	pItem->m_GenParam.dwRandSeed    = dwRandSeed;
   	pItem->m_GenParam.nQuality      = pInfo->nQuality;
    
    memcpy(pItem->m_GenParam.nMagicIndex, pInfo->nAttribIndex, sizeof(pItem->m_GenParam.nMagicIndex));

	bRetCode = ExpandCustomItemAttrib(pItem, pInfo);
	KGLOG_PROCESS_ERROR(bRetCode);
	
	bResult = true;
Exit0:
	return bResult;
}

// 填好装备的生成参数，生成装备
BOOL KItemLib::ExpandCustomItemAttrib(KItem* pItem, KCustomEquipInfo* pInfo) 
{
	BOOL            bResult     = false;
    KAttribInfo*    pAttribInfo = NULL;
	KAttribute*     pAttribNode = NULL;
	KAttribute*     pTailAttr   = NULL;
    int             nRandMax    = 0;

	assert(pItem);
	assert(pInfo);
    
    strncpy(pItem->m_Common.szItemName, pInfo->szName, ITEM_NAME_LEN);
    pItem->m_Common.szItemName[ITEM_NAME_LEN - 1] = '\0';

	pItem->m_Common.nGenre              = pInfo->nGenre;
	pItem->m_Common.nSub                = pInfo->nSub;
	pItem->m_Common.nDetail             = pInfo->nDetail;
	pItem->m_Common.nLevel              = pInfo->nLevel;
	pItem->m_Common.nPrice              = pInfo->nPrice;
	pItem->m_Common.bCanTrade           = pInfo->bCanTrade;
	pItem->m_Common.bCanDestroy         = pInfo->bCanDestroy;
	pItem->m_Common.nMaxExistAmount     = pInfo->nMaxExistAmount;
    
    pItem->m_Common.bConsumeDurabiltity = false;
    pItem->m_Common.nBindType           = pInfo->nBindType;
    pItem->m_Common.bStack              = (pInfo->nSub == estArrow); // 暗器是可叠加的
    pItem->m_Common.nAbradeRate         = pInfo->nAbradeRate;
	pItem->m_Common.dwSetID             = pInfo->nSetID;
    
    pItem->m_nMaxDurability             = pInfo->nMaxDurability;
	pItem->m_nCurrentDurability         = pInfo->nMaxDurability;
    pItem->m_dwScriptID                 = pInfo->dwScriptID;
    pItem->m_dwSkillID                  = pInfo->dwSkillID;
	pItem->m_dwSkillLevel               = pInfo->dwSkillLevel;
    
    pItem->m_nRepresentID               = pInfo->nRepresentID;
    pItem->m_nColorID                   = pInfo->nColorID;
	
#ifdef _CLIENT
	pItem->m_nUiId = pInfo->nUiId;
#endif
	// 展开道具基本属性

	for (int i = 0; i < MAX_ITEM_BASE_COUNT; i++)
	{
		if (pInfo->BaseAttrib[i].nAttribID > 0)
		{
			pAttribNode = KMemory::New<KAttribute>();
			KGLOG_PROCESS_ERROR(pAttribNode);

			pAttribNode->pNext = NULL;

			if (pTailAttr)
			{
				pTailAttr->pNext = pAttribNode;
				pTailAttr = pAttribNode;
			}
			else
			{
				assert(pItem->m_pBaseAttr == NULL);
				pItem->m_pBaseAttr = pAttribNode;
				pTailAttr = pAttribNode;
			}

			pAttribNode->nKey = pInfo->BaseAttrib[i].nAttribID;

            nRandMax = pInfo->BaseAttrib[i].nMax - pInfo->BaseAttrib[i].nMin;
            if (nRandMax < 0)
            {
                nRandMax = 0;
            }

			pAttribNode->nValue1 = pInfo->BaseAttrib[i].nMin + g_Random(nRandMax);
			pAttribNode->nValue2 = 0;
		}
		else
		{
			break;
		}
	}

	pTailAttr = NULL;

	for (int i = 0; i < MAX_ITEM_REQUIRE_COUNT; i++)
	{
		if (pInfo->RequireAttrib[i].nAttribID > 0)
		{
			pAttribNode = KMemory::New<KAttribute>();
			KGLOG_PROCESS_ERROR(pAttribNode);

			pAttribNode->pNext = NULL;

			if (pTailAttr)
			{
				pTailAttr->pNext = pAttribNode;
				pTailAttr = pAttribNode;
			}
			else
			{
				assert(pItem->m_pRequireAttr == NULL);
				pItem->m_pRequireAttr = pAttribNode;
				pTailAttr = pAttribNode;
			}

			pAttribNode->nKey       = pInfo->RequireAttrib[i].nAttribID;
			pAttribNode->nValue1    = pInfo->RequireAttrib[i].nValue;
			pAttribNode->nValue2    = 0;
		}
		else
		{
			break;
		}
	}

	pTailAttr = NULL;

	for (int i = 0; i < MAX_ITEM_MAGIC_COUNT; i++)
	{
		if (pItem->m_GenParam.nMagicIndex[i] > 0)
		{
			pAttribInfo =	m_Attrib.GetItemInfo(pItem->m_GenParam.nMagicIndex[i]);
			KGLOG_PROCESS_ERROR(pAttribInfo);

			pAttribNode = KMemory::New<KAttribute>();
			KGLOG_PROCESS_ERROR(pAttribNode);

			pAttribNode->pNext = NULL;

			if (pTailAttr)
			{
				pTailAttr->pNext = pAttribNode;
				pTailAttr = pAttribNode;
			}
			else
			{
				assert(pItem->m_pMagicAttr == NULL);
				pItem->m_pMagicAttr = pAttribNode;
				pTailAttr = pAttribNode;
			}

			pAttribNode->nKey = pAttribInfo->nAttribID;

            nRandMax = pAttribInfo->Param[0].nMax - pAttribInfo->Param[0].nMin;
            if (nRandMax < 0)
            {
                nRandMax = 0;
            }
			pAttribNode->nValue1 = pAttribInfo->Param[0].nMin + g_Random(nRandMax);

            nRandMax = pAttribInfo->Param[1].nMax - pAttribInfo->Param[1].nMin;
            if (nRandMax < 0)
            {
                nRandMax = 0;
            }
			pAttribNode->nValue2 = pAttribInfo->Param[1].nMin + g_Random(nRandMax);			
		}
		else
		{
			break;
		}
	}

	bResult = true;
Exit0:
	return bResult;
}

#ifdef _CLIENT
BOOL KItemLib::InsertSetEquipList(KCustomEquipInfoList* pEquipInfo)
{
	BOOL	bResult     = false;
	BOOL	bAddToList  = false;

	std::map<DWORD, KCustomEquipInfo>::iterator	it = pEquipInfo->m_EquipInfoList.begin();
	while (it != pEquipInfo->m_EquipInfoList.end())
	{
		KCustomEquipInfo* pInfo = &(it->second);

		if (pInfo->nSetID != ERROR_ID)
		{
			KSetInfo* pSetInfo = m_Set.GetItemInfo(pInfo->nSetID);
			KGLOG_PROCESS_ERROR(pSetInfo);

			for (int i = 0; i < MAX_SET_PART_NUM; i++)
			{
				if (pSetInfo->pEquipList[i] == NULL)
				{
					pSetInfo->pEquipList[i] = pInfo;
					bAddToList = true;
					break;
				}
			}

			KGLOG_PROCESS_ERROR(bAddToList);
		}

		it++;
	}

	bResult = true;
Exit0:
	return bResult;
}
#endif
