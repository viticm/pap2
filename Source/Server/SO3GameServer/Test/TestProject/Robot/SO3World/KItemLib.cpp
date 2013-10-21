#include "stdafx.h"
#include "SO3ProtocolBasic.h"
#include "KItem.h"
#include "KObjectManager.h"
#include "KItemLib.h"

KItemManager	g_ItemLib;

KItemLib::KItemLib()
{
	m_nVerion = 0;
}

KItemLib::~KItemLib()
{
}

BOOL KItemLib::Init(int nVersion)
{
	BOOL bResult = FALSE;
	
	char	szFile[MAX_PATH];
	KGLOG_PROCESS_ERROR( nVersion >= 0 && nVersion <= CURRENT_ITEM_VERSION);

	sprintf(szFile, "%s/%03d/%s", ITEM_SETTING_DIR, nVersion, 
		NORMAL_WEAPON_FILE);
	m_Weapon.Init(szFile);

	sprintf(szFile, "%s/%03d/%s", ITEM_SETTING_DIR, nVersion, 
		NORMAL_ARMOR_FILE);
	m_Armor.Init(szFile);

	sprintf(szFile, "%s/%03d/%s", ITEM_SETTING_DIR, nVersion, 
		NORMAL_TRINKET_FILE);
	m_Trinket.Init(szFile);

	sprintf(szFile, "%s/%03d/%s", ITEM_SETTING_DIR, nVersion, 
		CUST_WEAPON_FILE);
	m_CustomWeapon.Init(szFile);

	sprintf(szFile, "%s/%03d/%s", ITEM_SETTING_DIR, nVersion, 
		CUST_ARMOR_FILE);
	m_CustomArmor.Init(szFile);

	sprintf(szFile, "%s/%03d/%s", ITEM_SETTING_DIR, nVersion, 
		CUST_TRINKET_FILE);
	m_CustomTrinket.Init(szFile);

	sprintf(szFile, "%s/%03d/%s", ITEM_SETTING_DIR, nVersion, 
		COMMON_ITEM_FILE);
	m_Other.Init(szFile);

	sprintf(szFile, "%s/%03d/%s", ITEM_SETTING_DIR, nVersion, 
		ATTRIB_FILE);
	m_Attrib.Init(szFile);

	sprintf(szFile, "%s/%03d/%s", ITEM_SETTING_DIR, nVersion, 
		ATTRIB_FILE);
	m_AttribGroup.Init(szFile);

	m_nVerion = nVersion;

	bResult = TRUE;

Exit0:
	return bResult;
}

BOOL KItemManager::GenerateItemTo(KItem* pDestItem, 
                                    int nVersion, 
                                    DWORD dwTabType, 
                                    DWORD dwTabIndex, 
                                    DWORD dwId /* = ERROR_ID */, 
                                    DWORD dwRandSeed /* = 0 */, 
                                    int nMagicNum /* = 0 */, 
                                    int nQuality /* = 0 */, 
                                    int nMagic[] /* = NULL */, 
                                    time_t nGenTime /* = 0 */)
{
    BOOL bResult    = FALSE;
	BOOL bRetCode   = FALSE;
    BOOL bInitFlag  = FALSE;

	KGLOG_PROCESS_ERROR(pDestItem);
	KGLOG_PROCESS_ERROR(nVersion <= CURRENT_ITEM_VERSION);

	bRetCode = pDestItem->Init();
	KGLOG_PROCESS_ERROR(bRetCode);
    bInitFlag = TRUE;

	bRetCode = m_ItemLib[nVersion].GenerateItem(pDestItem, dwTabType, dwTabIndex, dwId, dwRandSeed,
		nMagicNum, nQuality, nMagic, nGenTime);
	KGLOG_PROCESS_ERROR(bRetCode);

	bResult = TRUE;
Exit0:
    if (!bResult)
    {
        if (bInitFlag)
        {
            pDestItem->UnInit();
            bInitFlag = FALSE;
        }
    }
	return bResult;
}

KItem* KItemManager::GenerateItem(int nVersion,
								  DWORD dwTabType, 
								  DWORD dwTabIndex, 
								  DWORD dwId/* = 0*/,
								  DWORD dwRandSeed/* = 0 */,
								  int nMagicNum/* = 0 */,
								  int nQuality/* = 0 */,
								  int nMagic[]/* = NULL */,
								  time_t nGenTime/* = 0*/)
{
	BOOL bRetCode = FALSE;

	KItem* pItem = g_ObjectManager.m_ItemSet.New(dwId);
	KGLOG_PROCESS_ERROR(pItem);
	KGLOG_PROCESS_ERROR(nVersion <= CURRENT_ITEM_VERSION);

    bRetCode = GenerateItemTo(pItem, nVersion, dwTabType, dwTabIndex, dwId, dwRandSeed, nMagicNum, nQuality, nMagic, nGenTime);
	KGLOG_PROCESS_ERROR(bRetCode);

	return pItem;
Exit0:
	if (pItem)
	{
    	bRetCode = g_ObjectManager.m_ItemSet.Delete(pItem);
		ASSERT(bRetCode);
        pItem = NULL;
	}
	return NULL;
}

KItemInfo* KItemLib::GetItemInfo(DWORD dwTabType, DWORD dwIndex, int &nType)
{
	switch(dwTabType)
	{
	case ittWeapon:
		nType = iitEquipInfo;
		return m_Weapon.GetItemInfo(dwIndex);
	case ittArmor:
		nType = iitEquipInfo;
		return m_Armor.GetItemInfo(dwIndex);
	case ittTrinket:
		nType = iitEquipInfo;
		return m_Trinket.GetItemInfo(dwIndex);
	case ittOther:
		nType = iitOtherInfo;
		return m_Other.GetItemInfo(dwIndex);
	case ittCustWeapon:
		nType = iitCustEquipInfo;
		return m_CustomWeapon.GetItemInfo(dwIndex);
	case ittCustArmor:
		nType = iitCustEquipInfo;
		return m_CustomArmor.GetItemInfo(dwIndex);
	case ittCustTrinket:
		nType = iitCustEquipInfo;
		return m_CustomTrinket.GetItemInfo(dwIndex);
	case ittAttrib:
	case ittSet:
	default:
		KGLOG_PROCESS_ERROR(FALSE);
	}

Exit0:

	return NULL;
}

BOOL KItemLib::GenerateItem(KItem* pItem,	  
							  DWORD dwTabType, 
							  DWORD dwIndex, 
							  DWORD dwId/* = 0*/,
							  DWORD dwRandSeed/* = 0 */,
							  int nMagicNum/* = 0 */,
							  int nQuality/* = 0 */,
							  int nMagic[]/* = NULL */,
							  time_t nGenTime/* = 0*/)
{
	BOOL bResult = FALSE;
	BOOL bRetCode = FALSE;
	DWORD dwOldRandSeed = 0;
	KGLOG_PROCESS_ERROR(dwTabType > ittInvalid && dwTabType < ittTotal);

	if (!nQuality)
	{
		nQuality = GenerateQuality(dwTabType, dwIndex);
	}

	if (!nGenTime)
	{
		nGenTime = time(NULL);
	}

	pItem->m_GenParam.nVersion = m_nVerion;
	pItem->m_GenParam.dwTabType = dwTabType;
	pItem->m_GenParam.dwIndex = dwIndex;

	pItem->m_GenParam.nGenTime = nGenTime;
	
	if (dwRandSeed)
	{
		dwOldRandSeed = g_GetRandomSeed();
	}
	else
	{
		// 改变随机种子,保证反复调用生成道具具有不一致性
		g_Random(128);
	}

	switch (dwTabType)
	{
	case ittWeapon:
		bRetCode = GenerateRandItem(
			pItem, m_Weapon.GetItemInfo(dwIndex), nMagicNum, nQuality, 
			dwRandSeed, nMagic);
		KG_PROCESS_ERROR(bRetCode);
		break;
	case ittArmor:
		bRetCode = GenerateRandItem(
			pItem, m_Armor.GetItemInfo(dwIndex), nMagicNum, nQuality, 
			dwRandSeed, nMagic);
		KG_PROCESS_ERROR(bRetCode);
	case ittTrinket:
		bRetCode = GenerateRandItem(
			pItem, m_Trinket.GetItemInfo(dwIndex), nMagicNum, nQuality, 
			dwRandSeed, nMagic);
		KG_PROCESS_ERROR(bRetCode);
		break;
	case ittOther:
		bRetCode = GenerateCommonItem(
			pItem, m_Other.GetItemInfo(dwIndex), dwRandSeed);
		KG_PROCESS_ERROR(bRetCode);
		break;
	case ittCustWeapon:
		bRetCode = GenerateCustomItem(
			pItem, m_CustomWeapon.GetItemInfo(dwIndex), dwRandSeed);
		KG_PROCESS_ERROR(bRetCode);
		break;
	case ittCustArmor:
		bRetCode = GenerateCustomItem(
			pItem, m_CustomArmor.GetItemInfo(dwIndex), dwRandSeed);
		KG_PROCESS_ERROR(bRetCode);
		break;
	case ittCustTrinket:
		bRetCode = GenerateCustomItem(
			pItem, m_CustomTrinket.GetItemInfo(dwIndex), dwRandSeed);
		KG_PROCESS_ERROR(bRetCode);
		break;
	default:
		KG_PROCESS_ERROR(0);
		break;
	}
	bResult = TRUE;

	if (!pItem->m_dwScriptID && !pItem->m_dwSkillID)
	{
		pItem->m_eUseType = uistCannotUse;
	}
	else
	{
		if (pItem->m_Common.nGenre == igEquipment)
			pItem->m_eUseType = uistUseWhenEquipped;
		else
			pItem->m_eUseType = uistUseInPackage;
	}
Exit0:
	if (dwOldRandSeed)
	{
		g_RandomSeed(dwOldRandSeed);
	}
	return bResult;
}

BOOL KItemLib::GenerateRandItem(KItem* pItem, 
								KEquipInfo* pInfo, 
								int nMagicNum, 
								int nQuality,
								DWORD dwRandSeed,
								int nMagic[])
{
	BOOL	bResult = FALSE;
	BOOL	bRetCode = FALSE;
	
	KGLOG_PROCESS_ERROR(pItem);
	KGLOG_PROCESS_ERROR(pInfo);
	KGLOG_PROCESS_ERROR(nMagicNum >= 0);
	KGLOG_PROCESS_ERROR(nMagicNum <= MAX_ITEM_MAGIC_COUNT);

	pItem->m_GenParam.nQuality = nQuality;

	// 生成魔法属性
	if (!nMagic)
	{
		bRetCode = GenerateMagicAttrib(pItem, pInfo->nAttribGroup, 
			nMagicNum);
		KGLOG_PROCESS_ERROR(bRetCode);
	}
	else
	{
		for (int i = 0; i < nMagicNum; i++)
		{
			pItem->m_GenParam.nMagicIndex[i] = nMagic[i];
		}
	}

	// 生成随机种子
	if (dwRandSeed == 0)	// 生成时随机
	{
		dwRandSeed = g_GetRandomSeed();
	}
	else
	{
		g_RandomSeed(dwRandSeed);
	}

	pItem->m_GenParam.dwRandSeed = dwRandSeed;

	bRetCode = ExpandItemAttrib(pItem, pInfo);
	KGLOG_PROCESS_ERROR(bRetCode);
	
	bResult = TRUE;

Exit0:
	return bResult;
}

BOOL KItemLib::GenerateCommonItem(KItem* pItem, 
								  KOtherItemInfo* pInfo,
								  DWORD dwRandSeed)
{
	BOOL bResult = FALSE;

	KGLOG_PROCESS_ERROR(pItem && pInfo);

	pItem->m_GenParam.dwRandSeed = dwRandSeed;
	pItem->m_Common.nGenre = pInfo->nGenre;
	pItem->m_Common.nBindType = pInfo->nBindType;
	pItem->m_Common.nSub = pInfo->nSub;
	pItem->m_Common.nDetail = pInfo->nDetail;
	pItem->m_Common.nPrice = pInfo->nPrice;
	pItem->m_Common.nLevel = pInfo->nLevel;
	strcpy(pItem->m_Common.szItemName, pInfo->szName);
	pItem->m_nMaxStackNum = pInfo->nMaxDurability;
	pItem->m_nCurrentDurability = pInfo->nMaxDurability;
	pItem->m_Common.bCanTrade = pInfo->bCanTrade;
	pItem->m_Common.bCanDestory = pInfo->bCanDestory;
	pItem->m_Common.bConsumeDurabiltity = pInfo->bCanConsume;
	pItem->m_Common.bStack = pInfo->bCanStack;
	pItem->m_Common.nAbradeRate = 0;							// 一般道具不存在磨损问题
	pItem->m_dwScriptID = pInfo->dwScriptID;
	pItem->m_dwSkillID = pInfo->dwSkillID;
	pItem->m_dwSkillLevel = pInfo->dwSkillLevel;
#ifdef _CLIENT
	pItem->m_nUiId = pInfo->nUiId;
#endif
	
	bResult = TRUE;
Exit0:
	return bResult;
}

BOOL KItemLib::GenerateCustomItem(KItem* pItem, 
								  KCustomEquipInfo* pInfo,
								  DWORD dwRandSeed)
{
	BOOL bResult = FALSE;
	BOOL bRetCode = FALSE;

	KGLOG_PROCESS_ERROR(pItem);
	KGLOG_PROCESS_ERROR(pInfo);

	pItem->m_GenParam.nQuality = pInfo->nQuality;
	for (int i = 0; i < MAX_ITEM_MAGIC_COUNT; i++)
	{
		pItem->m_GenParam.nMagicIndex[i] = pInfo->nAttribIndex[i];
	}

	if (dwRandSeed == 0)
	{
		dwRandSeed = g_GetRandomSeed();
	}
	else
	{
		g_RandomSeed(dwRandSeed);
	}

	pItem->m_GenParam.dwRandSeed = dwRandSeed;

	bRetCode = ExpandCustomItemAttrib(pItem, pInfo);
	KGLOG_PROCESS_ERROR(bRetCode);
	
	bResult = TRUE;
Exit0:
	return bResult;
}

BOOL KItemLib::GenerateMagicAttrib(KItem* pItem, int nAttrGroup, int nMagicNum)
{
	BOOL bResult = FALSE;
	int nAttribIndex = 0;
	int	nIndex = 0;
	int nMagic[MAX_ITEM_MAGIC_COUNT];
	KAttribInfo* pInfo = NULL;

	KGLOG_PROCESS_ERROR(pItem);
	KGLOG_PROCESS_ERROR(nAttrGroup > ERROR_ID);
	KGLOG_PROCESS_ERROR(nAttrGroup < MAX_ATTRIB_GROUP_NUM);
	KGLOG_PROCESS_ERROR(nMagicNum <= MAX_ITEM_MAGIC_COUNT);

	ZeroMemory(nMagic, sizeof(nMagic));

	for (int i = 0; i < nMagicNum; i++)
	{
		nAttribIndex = m_AttribGroup.GetAttribIndex(nAttrGroup);
		if (nAttribIndex == ERROR_ID)
		{
			continue;
		}

		pInfo = m_Attrib.GetItemInfo(nAttribIndex);
		KGLOG_PROCESS_ERROR(pInfo);

		nMagic[nIndex] = pInfo->nAttribID;

		for (int j = 0; j < nIndex; j++)
		{
			if (nMagic[j] == nMagic[nIndex])
			{
				nMagic[nIndex] = 0;
				break;
			}
		}
		
		if (nMagic[nIndex])
		{
			pItem->m_GenParam.nMagicIndex[nIndex] = nAttribIndex;
			nIndex++;
		}
	}

	for (int i = nMagicNum; i < MAX_ITEM_MAGIC_COUNT; i++)
	{
		pItem->m_GenParam.nMagicIndex[i] = 0;
	}
	bResult = TRUE;

Exit0:
	return bResult;
}

// 填好装备的生成参数，生成装备
BOOL KItemLib::ExpandItemAttrib(KItem* pItem, KEquipInfo* pInfo) 
{
	BOOL bResult = FALSE;
	KAttribInfo* pAttr = NULL;

	KGLOG_PROCESS_ERROR(pItem);
	KGLOG_PROCESS_ERROR(pInfo);

	pItem->m_Common.nGenre = pInfo->nGenre;
	pItem->m_Common.nSub = pInfo->nSub;
	pItem->m_Common.nDetail = pInfo->nDetail;
	pItem->m_Common.nLevel = pInfo->nLevel;
	pItem->m_Common.nPrice = pInfo->nPrice;
	pItem->m_Common.nAbradeRate = 0;// pInfo->nAbradeRate;// TODO:磨损机率，等具体随机装备制作时再搞吧
	strcpy(pItem->m_Common.szItemName, pInfo->szName);
#ifdef _CLIENT
	pItem->m_nUiId = pInfo->nUiId;
#endif
	pItem->m_nRepresentId = pInfo->nRepresentId;
	pItem->m_Common.nBindType = ibtBindOnEquipped;	// 随机装备都是装备绑定
	pItem->m_Common.bStack = FALSE;					// 随机装备都是不能叠放的

	// 展开道具基本属性
	KAttribute* pTailAttr = NULL;

	for (int i = 0; i < MAX_ITEM_BASE_COUNT; i++)
	{
		if (pInfo->BaseAttrib[i].nAttribID > 0)
		{
			KAttribute* pAttrib = g_ObjectManager.m_AttributeSet.New();
			KGLOG_PROCESS_ERROR(pAttrib);
			pAttrib->pNext = NULL;

			if (pTailAttr)
			{
				pTailAttr->pNext = pAttrib;
				pTailAttr = pAttrib;
			}
			else
			{
				ASSERT(NULL == pItem->m_pBaseAttr);
				pItem->m_pBaseAttr = pAttrib;
				pTailAttr = pAttrib;
			}
			pAttrib->nKey = pInfo->BaseAttrib[i].nAttribID;
			pAttrib->nValue1 = pInfo->BaseAttrib[i].nMin +
				g_Random(pInfo->BaseAttrib[i].nMax - pInfo->BaseAttrib[i].nMin);
			// 道具基本属性第二参数无效。
			pAttrib->nValue2 = 0;
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
			KAttribute* pAttrib = g_ObjectManager.m_AttributeSet.New();
			KGLOG_PROCESS_ERROR(pAttrib);
			pAttrib->pNext = NULL;

			if (pTailAttr)
			{
				pTailAttr->pNext = pAttrib;
				pTailAttr = pAttrib;
			}
			else
			{
				ASSERT(NULL == pItem->m_pRequireAttr);
				pItem->m_pRequireAttr = pAttrib;
				pTailAttr = pAttrib;
			}
			pAttrib->nKey = pInfo->RequireAttrib[i].nAttribID;
			pAttrib->nValue1 = pInfo->RequireAttrib[i].nValue;
			pAttrib->nValue2 = 0;
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
			pAttr =	m_Attrib.GetItemInfo(pItem->m_GenParam.nMagicIndex[i]);
			KGLOG_PROCESS_ERROR(pAttr);

			KAttribute* pAttrib = g_ObjectManager.m_AttributeSet.New();
			KGLOG_PROCESS_ERROR(pAttrib);
			pAttrib->pNext = NULL;

			if (pTailAttr)
			{
				pTailAttr->pNext = pAttrib;
				pTailAttr = pAttrib;
			}
			else
			{
				ASSERT(NULL == pItem->m_pMagicAttr);
				pItem->m_pMagicAttr = pAttrib;
				pTailAttr = pAttrib;
			}

			pAttrib->nKey = pAttr->nAttribID;
			pAttrib->nValue1 = pAttr->Param[0].nMin + 
				g_Random(pAttr->Param[0].nMax - pAttr->Param[0].nMin);
			pAttrib->nValue2 = pAttr->Param[1].nMin + 
				g_Random(pAttr->Param[1].nMax - pAttr->Param[1].nMin);			
		}
		else
		{
			break;
		}
	}

	bResult = TRUE;

Exit0:
	return bResult;
}

// 填好装备的生成参数，生成装备
BOOL KItemLib::ExpandCustomItemAttrib(KItem* pItem, KCustomEquipInfo* pInfo) 
{
	BOOL bResult = FALSE;
	KAttribInfo* pAttr = NULL;

	KGLOG_PROCESS_ERROR(pItem);
	KGLOG_PROCESS_ERROR(pInfo);

	pItem->m_Common.nBindType = pInfo->nBindType;

	pItem->m_Common.nGenre = pInfo->nGenre;
	pItem->m_Common.nSub = pInfo->nSub;
	pItem->m_Common.nDetail = pInfo->nDetail;
	pItem->m_Common.nLevel = pInfo->nLevel;
	pItem->m_Common.nPrice = pInfo->nPrice;
	pItem->m_nRepresentId = pInfo->nRepresentId;
	pItem->m_nMaxDurability = pInfo->nMaxDurability;
	pItem->m_nCurrentDurability = pInfo->nMaxDurability;
	pItem->m_Common.bCanTrade = pInfo->bCanTrade;
	pItem->m_Common.bCanDestory = pInfo->bCanDestory;
	pItem->m_Common.nAbradeRate = pInfo->nAbradeRate;
	if (pInfo->nSub != estArrow)	//暗器可叠加
		pItem->m_Common.bStack = FALSE;
	else
		pItem->m_Common.bStack = TRUE;
	pItem->m_Common.bConsumeDurabiltity = FALSE;
	pItem->m_dwScriptID = pInfo->dwScriptID;
	strcpy(pItem->m_Common.szItemName, pInfo->szName);
#ifdef _CLIENT
	pItem->m_nUiId = pInfo->nUiId;
#endif


	// 展开道具基本属性
	KAttribute* pTailAttr = NULL;

	for (int i = 0; i < MAX_ITEM_BASE_COUNT; i++)
	{
		if (pInfo->BaseAttrib[i].nAttribID > 0)
		{
			KAttribute* pAttrib = g_ObjectManager.m_AttributeSet.New();
			KGLOG_PROCESS_ERROR(pAttrib);
			pAttrib->pNext = NULL;

			if (pTailAttr)
			{
				pTailAttr->pNext = pAttrib;
				pTailAttr = pAttrib;
			}
			else
			{
				ASSERT(NULL == pItem->m_pBaseAttr);
				pItem->m_pBaseAttr = pAttrib;
				pTailAttr = pAttrib;
			}

			pAttrib->nKey = pInfo->BaseAttrib[i].nAttribID;
			pAttrib->nValue1 = pInfo->BaseAttrib[i].nMin +
				g_Random(pInfo->BaseAttrib[i].nMax - pInfo->BaseAttrib[i].nMin);
			// 道具基本属性第二参数无效。
			pAttrib->nValue2 = 0;
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
			KAttribute* pAttrib = g_ObjectManager.m_AttributeSet.New();
			KGLOG_PROCESS_ERROR(pAttrib);
			pAttrib->pNext = NULL;

			if (pTailAttr)
			{
				pTailAttr->pNext = pAttrib;
				pTailAttr = pAttrib;
			}
			else
			{
				ASSERT(NULL == pItem->m_pRequireAttr);
				pItem->m_pRequireAttr = pAttrib;
				pTailAttr = pAttrib;
			}
			pAttrib->nKey = pInfo->RequireAttrib[i].nAttribID;
			pAttrib->nValue1 = pInfo->RequireAttrib[i].nValue;
			pAttrib->nValue2 = 0;
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
			pAttr =	m_Attrib.GetItemInfo(pItem->m_GenParam.nMagicIndex[i]);
			KGLOG_PROCESS_ERROR(pAttr);

			KAttribute* pAttrib = g_ObjectManager.m_AttributeSet.New();
			KGLOG_PROCESS_ERROR(pAttrib);
			pAttrib->pNext = NULL;

			if (pTailAttr)
			{
				pTailAttr->pNext = pAttrib;
				pTailAttr = pAttrib;
			}
			else
			{
				ASSERT(NULL == pItem->m_pMagicAttr);
				pItem->m_pMagicAttr = pAttrib;
				pTailAttr = pAttrib;
			}

			pAttrib->nKey = pAttr->nAttribID;
			pAttrib->nValue1 = pAttr->Param[0].nMin + 
				g_Random(pAttr->Param[0].nMax - pAttr->Param[0].nMin);
			pAttrib->nValue2 = pAttr->Param[1].nMin + 
				g_Random(pAttr->Param[1].nMax - pAttr->Param[1].nMin);			
		}
		else
		{
			break;
		}
	}

	bResult = TRUE;

Exit0:
	return bResult;
}

BOOL KItemManager::FreeItem(KItem* pItem)
{
	BOOL bResult = FALSE;

	KG_PROCESS_ERROR(pItem);

	pItem->UnInit();

	bResult = g_ObjectManager.m_ItemSet.Delete(pItem);

Exit0:
	return bResult;
}

BOOL KItemManager::GetItemTransferData(KItem* pItem, void* pBuffer, size_t& uSize,
								   int& nType)
{
	BOOL bResult = FALSE;

	KGLOG_PROCESS_ERROR(pItem);
	KGLOG_PROCESS_ERROR(pBuffer);

	switch(pItem->m_GenParam.dwTabType)
	{
	case ittCustArmor:
	case ittCustWeapon:
	case ittCustTrinket:
		{
			KGLOG_PROCESS_ERROR(uSize >= sizeof(CUSTOM_EQUIP_SYNC));
			nType = isst_CustomEquip;
            uSize = sizeof(CUSTOM_EQUIP_SYNC);
			CUSTOM_EQUIP_SYNC* pCes = (CUSTOM_EQUIP_SYNC *)pBuffer;
            pCes->byVersion  = (BYTE)pItem->m_GenParam.nVersion;
			pCes->dwRandSeed = pItem->m_GenParam.dwRandSeed;
			pCes->byTabFile = (BYTE)pItem->m_GenParam.dwTabType;
			pCes->wTabIndex = (WORD)pItem->m_GenParam.dwIndex;
			pCes->dwItemId = pItem->m_dwID;
            pCes->wDurability = (WORD)pItem->m_nCurrentDurability;
			// 附魔待定
			pCes->wExtendMagic = 0;
			pCes->wTemporaryMagic = 0;
		}
		break;
	case ittArmor:
	case ittWeapon:
	case ittTrinket:
		{
			KGLOG_PROCESS_ERROR(uSize >= sizeof(RAND_EQUIP_SYNC));
			nType = isst_Equip;
            uSize = sizeof(RAND_EQUIP_SYNC);
			RAND_EQUIP_SYNC* pEs = (RAND_EQUIP_SYNC *)pBuffer;
            pEs->byVersion  = (BYTE)pItem->m_GenParam.nVersion;
			pEs->dwRandSeed = pItem->m_GenParam.dwRandSeed;
			pEs->byTabFile = (BYTE)pItem->m_GenParam.dwTabType;
			pEs->wTabIndex = (WORD)pItem->m_GenParam.dwIndex;
			pEs->dwItemId = pItem->m_dwID;
			pEs->byQuality = (BYTE)pItem->m_GenParam.nQuality;
			// 附魔待定
            pEs->wDurability = (WORD)pItem->m_nCurrentDurability;
			for (int i = 0; i < MAX_ITEM_MAGIC_COUNT; i++)
			{
				pEs->wMagic[i] = (WORD)pItem->m_GenParam.nMagicIndex[i];
			}
			pEs->wExtendMagic = 0;
			pEs->wTemporaryMagic = 0;
		}
		break;
	case ittOther:
		{
			KGLOG_PROCESS_ERROR(uSize >= sizeof(COMMON_ITEM_SYNC));
			nType = isst_Common;
            uSize = sizeof(COMMON_ITEM_SYNC);
			COMMON_ITEM_SYNC* pCis = (COMMON_ITEM_SYNC *)pBuffer;
            pCis->byVersion = (BYTE)pItem->m_GenParam.nVersion;
			pCis->byTabFile = (BYTE)pItem->m_GenParam.dwTabType;
			pCis->wTabIndex = (WORD)pItem->m_GenParam.dwIndex;
			pCis->dwItemId = pItem->m_dwID;
		}
		break;
	default:
		KGLOG_PROCESS_ERROR(FALSE);
		break;
	}
	bResult = TRUE;
Exit0:
	return bResult;
}

BOOL KItemManager::GenerateItemFromTransferDataTo(
    KItem* pDestItem, 
    int nType, void* pBuffer, size_t uBufferSize
)
{
	BOOL	bResult     = FALSE;
    BOOL    bRetCode    = FALSE;

    KGLOG_PROCESS_ERROR(pDestItem);

	switch(nType)
	{
	case isst_Common:
		{
			KGLOG_PROCESS_ERROR(uBufferSize == sizeof(COMMON_ITEM_SYNC));
			COMMON_ITEM_SYNC* pCis = (COMMON_ITEM_SYNC *)pBuffer;
			bRetCode = GenerateItemTo(pDestItem, pCis->byVersion, pCis->byTabFile, pCis->wTabIndex, pCis->dwItemId);
            KGLOG_PROCESS_ERROR(bRetCode);
		}
		break;
	case isst_CustomEquip:
		{
			KGLOG_PROCESS_ERROR(uBufferSize == sizeof(CUSTOM_EQUIP_SYNC));
			CUSTOM_EQUIP_SYNC* pCes = (CUSTOM_EQUIP_SYNC *)pBuffer;
			bRetCode = GenerateItemTo(pDestItem, pCes->byVersion, pCes->byTabFile, pCes->wTabIndex, pCes->dwItemId,
				pCes->dwRandSeed);
            KGLOG_PROCESS_ERROR(bRetCode);
		}
		break;
	case isst_Equip:
		{
			KGLOG_PROCESS_ERROR(uBufferSize == sizeof(RAND_EQUIP_SYNC));

			RAND_EQUIP_SYNC* pEs = (RAND_EQUIP_SYNC *)pBuffer;

			int nMagicNum = 0;
			int arnMagic[MAX_ITEM_MAGIC_COUNT];

			ZeroMemory(arnMagic, sizeof(arnMagic));

			for (int i = 0; i < MAX_ITEM_MAGIC_COUNT; i++)
			{
				if (pEs->wMagic[i] == ERROR_ID)
				{
					break;
				}
				nMagicNum++;
				arnMagic[i] = pEs->wMagic[i];
			}

			bRetCode = GenerateItemTo(pDestItem, pEs->byVersion, pEs->byTabFile, pEs->wTabIndex, pEs->dwItemId,
				pEs->dwRandSeed, nMagicNum, pEs->byQuality, arnMagic);
            KGLOG_PROCESS_ERROR(bRetCode);
		}
		break;
	default:
		break;
	}

    bResult = TRUE;
Exit0:
	return bResult;
}

KItem* KItemManager::GenerateItemFromTransferData(int nType, void* pBuffer, 
												  size_t uBufferSize)
{
	KItem*	pItem = NULL;
	DWORD	dwItemId = ERROR_ID;

	switch(nType)
	{
	case isst_Common:
		{
			KGLOG_PROCESS_ERROR(uBufferSize == sizeof(COMMON_ITEM_SYNC));
			COMMON_ITEM_SYNC* pCis = (COMMON_ITEM_SYNC *)pBuffer;
			pItem = GenerateItem(pCis->byVersion, pCis->byTabFile, pCis->wTabIndex, pCis->dwItemId);
		}
		break;
	case isst_CustomEquip:
		{
			KGLOG_PROCESS_ERROR(uBufferSize == sizeof(CUSTOM_EQUIP_SYNC));
			CUSTOM_EQUIP_SYNC* pCes = (CUSTOM_EQUIP_SYNC *)pBuffer;
			pItem = GenerateItem(pCes->byVersion, pCes->byTabFile, pCes->wTabIndex, pCes->dwItemId,
				pCes->dwRandSeed);
		}
		break;
	case isst_Equip:
		{
			KGLOG_PROCESS_ERROR(uBufferSize == sizeof(RAND_EQUIP_SYNC));

			RAND_EQUIP_SYNC* pEs = (RAND_EQUIP_SYNC *)pBuffer;

			int nMagicNum = 0;
			int arnMagic[MAX_ITEM_MAGIC_COUNT];

			ZeroMemory(arnMagic, sizeof(arnMagic));

			for (int i = 0; i < MAX_ITEM_MAGIC_COUNT; i++)
			{
				if (pEs->wMagic[i] == ERROR_ID)
				{
					break;
				}
				nMagicNum++;
				arnMagic[i] = pEs->wMagic[i];
			}

			pItem = GenerateItem(pEs->byVersion, pEs->byTabFile, pEs->wTabIndex, pEs->dwItemId,
				pEs->dwRandSeed, nMagicNum, pEs->byQuality, arnMagic);
		}
		break;
	default:
		break;
	}

Exit0:
	return pItem;
}

int KItemLib::GenerateQuality(int nType, int nIndex)
{
	KGLOG_PROCESS_ERROR(nType < ittTotal);
	//TODO: 需要增加品质生成代码
	
Exit0:

	return 0;
}

KAttribInfo* KItemLib::GetAttribInfo(int nAttribIndex)
{
	return m_Attrib.GetItemInfo(nAttribIndex);	
}

KItem* KItemManager::CloneItem(KItem* pSrcItem)
{
	BOOL bRetCode = FALSE;
	KItem* pItem = NULL;

	KGLOG_PROCESS_ERROR(pSrcItem);

	pItem = g_ObjectManager.m_ItemSet.New();
	KGLOG_PROCESS_ERROR(pItem);

	bRetCode = pItem->Init();
	KGLOG_PROCESS_ERROR(bRetCode);

	DWORD dwItemID = pItem->m_dwID;
	*pItem = *pSrcItem;
	pItem->m_dwID = dwItemID;
	pItem->m_GenParam.nGenTime = time(NULL);

	return pItem;

Exit0:
	if (pItem)
	{
		pItem->UnInit();
		g_ObjectManager.m_ItemSet.Delete(pItem);
	}

	return NULL;
}


KItemManager::KItemManager()
{
	m_ItemLib = NULL;
}

KItemManager::~KItemManager()
{
	if (m_ItemLib)
	{
		delete [] m_ItemLib;
		m_ItemLib = NULL;
	}
}

BOOL KItemManager::Init()
{
	BOOL bResult = FALSE;
	BOOL bRetCode = FALSE;

	m_ItemLib = new KItemLib[CURRENT_ITEM_VERSION + 1];
	KGLOG_PROCESS_ERROR(m_ItemLib);

	for (int i = 0; i < CURRENT_ITEM_VERSION + 1; i++)
	{
		bRetCode = m_ItemLib[i].Init(i);
		KGLOG_PROCESS_ERROR(bRetCode);
	}

	bResult = TRUE;
Exit0:
	return bResult;
}

KItemInfo* KItemManager::GetItemInfo(int nVersion, DWORD dwTabType, DWORD dwIndex, int &nType)
{
	KGLOG_PROCESS_ERROR(nVersion >= 0);
	KGLOG_PROCESS_ERROR(nVersion <= CURRENT_ITEM_VERSION);

	return m_ItemLib[nVersion].GetItemInfo(dwTabType, dwIndex, nType);

Exit0:

	return NULL;
}