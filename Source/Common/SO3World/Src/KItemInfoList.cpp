#include "stdafx.h"
#include "StrMap.h"
#include "KItemInfoList.h"
#include "KSO3World.h"
#include "KItemLib.h"
#include "KPlayer.h"

BOOL KOtherItemInfoList::Load(char* pszFile)
{
    BOOL            bResult         = false;
	BOOL            bRetCode        = false;
	ITabFile*       piTabFile       = NULL;
    int             nHeight         = 0;
	KOtherItemInfo	DefaultInfo;
	std::pair<ITEM_INFO_LIST::iterator, bool> InsRet;

	piTabFile = g_OpenTabFile(pszFile);
	KGLOG_PROCESS_ERROR(piTabFile);

	ZeroMemory(&DefaultInfo, sizeof(KOtherItemInfo));

	nHeight = piTabFile->GetHeight();
	KGLOG_PROCESS_ERROR(nHeight > 1);
    
    bRetCode = LoadLine(piTabFile, 2, &DefaultInfo, DefaultInfo);
    KGLOG_PROCESS_ERROR(bRetCode);

	for (int nIndex = 3; nIndex <= nHeight; nIndex++)
	{
        KOtherItemInfo	EquipInfo;

        bRetCode = LoadLine(piTabFile, nIndex, &EquipInfo, DefaultInfo);
        KGLOG_PROCESS_ERROR(bRetCode);

		InsRet = m_ItemInfoList.insert(std::make_pair(EquipInfo.dwID, EquipInfo));
	    KGLOG_PROCESS_ERROR(InsRet.second);
	}

	bResult = true;
Exit0:
	KG_COM_RELEASE(piTabFile);
	return bResult;
}

void KOtherItemInfoList::Clear()
{
	m_ItemInfoList.clear();
}

KOtherItemInfo*	KOtherItemInfoList::GetItemInfo(DWORD dwID)
{
    KOtherItemInfo*             pResult = NULL;
    ITEM_INFO_LIST::iterator    it;

	KG_PROCESS_ERROR(dwID < MAX_ITEM_COUNT);

	it = m_ItemInfoList.find(dwID);
	KG_PROCESS_ERROR(it != m_ItemInfoList.end());

    pResult = &it->second;
Exit0:
	return pResult;
}

BOOL KOtherItemInfoList::LoadLine(ITabFile* piTabFile, int nLine, KOtherItemInfo* pItemInfo, const KOtherItemInfo& crDefaultInfo)
{
    BOOL            bResult         = false;
    BOOL            bRetCode        = false;
    DWORD           dwID            = 0;
    BOOL            bDefaultData    = false;
    KOtherItemInfo	EquipInfo;
	char	        szScriptName[MAX_PATH];

    assert(piTabFile);
    assert(pItemInfo);

	bRetCode = piTabFile->GetInteger(nLine, "ID", crDefaultInfo.dwID, (int*)&dwID);
	KGLOG_PROCESS_ERROR(bRetCode);
    
    KGLOG_PROCESS_ERROR((nLine == 2 && dwID == ERROR_ID) || (nLine > 2 && dwID < MAX_ITEM_COUNT));

	EquipInfo.dwID = dwID;

	bRetCode = piTabFile->GetString(nLine, "Name", crDefaultInfo.szName, EquipInfo.szName, ITEM_NAME_LEN);
	KGLOG_PROCESS_ERROR(bRetCode);

#ifdef _CLIENT
	bRetCode = piTabFile->GetInteger(nLine, "UiID", crDefaultInfo.nUiId, &EquipInfo.nUiId);
	KGLOG_PROCESS_ERROR(bRetCode);
#endif

	bRetCode = piTabFile->GetInteger(nLine, "Genre", crDefaultInfo.nGenre, &EquipInfo.nGenre);
	KGLOG_PROCESS_ERROR(bRetCode);

    KGLOG_PROCESS_ERROR(EquipInfo.nGenre >= igEquipment && EquipInfo.nGenre < igTotal);

	bRetCode = piTabFile->GetInteger(nLine, "SubType", crDefaultInfo.nSub, &EquipInfo.nSub);
	KGLOG_PROCESS_ERROR(bRetCode);

	bRetCode = piTabFile->GetInteger(nLine, "DetailType", crDefaultInfo.nDetail, &EquipInfo.nDetail);
	KGLOG_PROCESS_ERROR(bRetCode);

	bRetCode = piTabFile->GetInteger(nLine, "Price", crDefaultInfo.nPrice, &EquipInfo.nPrice);
	KGLOG_PROCESS_ERROR(bRetCode);

	bRetCode = piTabFile->GetInteger(nLine, "BindType", crDefaultInfo.nBindType, &EquipInfo.nBindType);
	KGLOG_PROCESS_ERROR(bRetCode);
    KGLOG_PROCESS_ERROR(EquipInfo.nBindType > ibtInvalid && EquipInfo.nBindType < ibtTotal);

	bRetCode = piTabFile->GetInteger(nLine, "Quality", crDefaultInfo.nQuality, &EquipInfo.nQuality);
	KGLOG_PROCESS_ERROR(bRetCode);

	bRetCode = piTabFile->GetInteger(nLine, "CanTrade", crDefaultInfo.bCanTrade, &EquipInfo.bCanTrade);
	KGLOG_PROCESS_ERROR(bRetCode);

	bRetCode = piTabFile->GetInteger(nLine, "CanDestroy", crDefaultInfo.bCanDestroy, &EquipInfo.bCanDestroy);
	KGLOG_PROCESS_ERROR(bRetCode);

	bRetCode = piTabFile->GetInteger(nLine, "CanConsume", crDefaultInfo.bCanConsume, &EquipInfo.bCanConsume);
	KGLOG_PROCESS_ERROR(bRetCode);

	bRetCode = piTabFile->GetInteger(nLine, "CanStack", crDefaultInfo.bCanStack, &EquipInfo.bCanStack);
	KGLOG_PROCESS_ERROR(bRetCode);

	bRetCode = piTabFile->GetInteger(nLine, "MaxDurability", crDefaultInfo.nMaxDurability, &EquipInfo.nMaxDurability);
	KGLOG_PROCESS_ERROR(bRetCode);

    bRetCode = piTabFile->GetInteger(nLine, "ExistType", crDefaultInfo.nExistType, &EquipInfo.nExistType);
    KGLOG_PROCESS_ERROR(bRetCode);

	bRetCode = piTabFile->GetInteger(nLine, "MaxExistTime", crDefaultInfo.nMaxExistTime, &EquipInfo.nMaxExistTime);
	KGLOG_PROCESS_ERROR(bRetCode);

	bRetCode = piTabFile->GetInteger(nLine, "MaxExistAmount", crDefaultInfo.nMaxExistAmount, &EquipInfo.nMaxExistAmount);
	KGLOG_PROCESS_ERROR(bRetCode);

	bRetCode = piTabFile->GetString(nLine, "ScriptName", "", szScriptName, MAX_PATH);
	KGLOG_PROCESS_ERROR(bRetCode);
    
    EquipInfo.dwScriptID = 0;
	if (szScriptName[0] != '\0')
    {
        EquipInfo.dwScriptID = g_FileNameHash(szScriptName);
    }

	bRetCode = piTabFile->GetInteger(nLine, "SkillID", crDefaultInfo.dwSkillID, (int*)&EquipInfo.dwSkillID);
	KGLOG_PROCESS_ERROR(bRetCode);

	bRetCode = piTabFile->GetInteger(nLine, "SkillLevel", crDefaultInfo.dwSkillLevel, (int*)&EquipInfo.dwSkillLevel);
	KGLOG_PROCESS_ERROR(bRetCode);
    
    bRetCode = piTabFile->GetInteger(nLine, "CoolDownID", crDefaultInfo.dwCoolDownID, (int*)&EquipInfo.dwCoolDownID);
    KGLOG_PROCESS_ERROR(bRetCode);
    
    bRetCode = piTabFile->GetInteger(nLine, "RequireLevel", crDefaultInfo.nRequireLevel, &EquipInfo.nRequireLevel);
    KGLOG_PROCESS_ERROR(bRetCode);
    
    bRetCode = piTabFile->GetInteger(nLine, "RequireProfessionID", crDefaultInfo.dwRequireProfessionID, (int*)&EquipInfo.dwRequireProfessionID);
    KGLOG_PROCESS_ERROR(bRetCode);
    
    // 0表示对分支没有限制
    bRetCode = piTabFile->GetInteger(nLine, "RequireProfessionBranch", crDefaultInfo.dwRequireProfessionBranch, (int*)&EquipInfo.dwRequireProfessionBranch);
    KGLOG_PROCESS_ERROR(bRetCode);
    
    bRetCode = piTabFile->GetInteger(nLine, "RequireProfessionLevel", crDefaultInfo.nRequireProfessionLevel, &EquipInfo.nRequireProfessionLevel);
    KGLOG_PROCESS_ERROR(bRetCode);
    
    // 0表示对性别没有要求，1表示男性，2表示女性
    bRetCode = piTabFile->GetInteger(nLine, "RequireGender", crDefaultInfo.nRequireGender, &EquipInfo.nRequireGender);
    KGLOG_PROCESS_ERROR(bRetCode);
    assert(EquipInfo.nRequireGender == 0 || EquipInfo.nRequireGender == 1 || EquipInfo.nRequireGender == 2);

    bRetCode = piTabFile->GetInteger(nLine, "CanUseOnHorse", crDefaultInfo.bCanUseOnHorse, (int*)&EquipInfo.bCanUseOnHorse);
    KGLOG_PROCESS_ERROR(bRetCode);

    bRetCode = piTabFile->GetInteger(nLine, "CanUseInFight", crDefaultInfo.bCanUseInFight, (int*)&EquipInfo.bCanUseInFight);
    KGLOG_PROCESS_ERROR(bRetCode);
    
    bRetCode = piTabFile->GetInteger(nLine, "RequireCamp", crDefaultInfo.nRequireCamp, &EquipInfo.nRequireCamp);
    KGLOG_PROCESS_ERROR(bRetCode);

    bRetCode = piTabFile->GetInteger(nLine, "AucGenre", crDefaultInfo.nAucGenre, &EquipInfo.nAucGenre);
    KGLOG_PROCESS_ERROR(bRetCode);

    bRetCode = piTabFile->GetInteger(nLine, "AucSubType", crDefaultInfo.nAucSub, &EquipInfo.nAucSub);
    KGLOG_PROCESS_ERROR(bRetCode);

    bRetCode = piTabFile->GetInteger(nLine, "RequireForce", crDefaultInfo.dwRequireForceID, (int*)&EquipInfo.dwRequireForceID);
    KGLOG_PROCESS_ERROR(bRetCode);

    bRetCode = piTabFile->GetInteger(nLine, "Prefix", crDefaultInfo.nPrefix, &EquipInfo.nPrefix);
    KGLOG_PROCESS_ERROR(bRetCode);

    bRetCode = piTabFile->GetInteger(nLine, "Postfix", crDefaultInfo.nPostfix, &EquipInfo.nPostfix);
    KGLOG_PROCESS_ERROR(bRetCode);
    
    bRetCode = piTabFile->GetInteger(nLine, "EnchantID", crDefaultInfo.dwEnchantID, (int*)&EquipInfo.dwEnchantID);
    KGLOG_PROCESS_ERROR(bRetCode);

    bRetCode = piTabFile->GetInteger(nLine, "BoxID", crDefaultInfo.dwBoxTemplateID, (int*)&EquipInfo.dwBoxTemplateID);
    KGLOG_PROCESS_ERROR(bRetCode);

    if (EquipInfo.dwBoxTemplateID)
    {
        // 箱子必须不是限时道具且不能叠加
        KGLOG_PROCESS_ERROR(EquipInfo.nExistType == ketPermanent);
        KGLOG_PROCESS_ERROR(!EquipInfo.bCanStack);
    }

    *pItemInfo = EquipInfo;

    bResult = true;
Exit0:
    return bResult;
}
//////////////////////////////////////////////////////////////////////////

BOOL KAttribInfoList::Load(char* pszFile)
{
    BOOL                bResult         = false;
	BOOL                bRetCode        = false;
	ITabFile*           piTabFile       = NULL;
    int                 nHeight         = 0;
	KAttribInfo	        DefaultInfo;
	std::pair<ITEM_INFO_LIST::iterator, bool> InsRet;

	piTabFile = g_OpenTabFile(pszFile);
	KGLOG_PROCESS_ERROR(piTabFile);

	ZeroMemory(&DefaultInfo, sizeof(KAttribInfo));

	nHeight = piTabFile->GetHeight();
	KGLOG_PROCESS_ERROR(nHeight > 1);

	for (int nIndex = 1; nIndex < nHeight; nIndex++)
	{
		DWORD	    dwID        = 0;
		KAttribInfo	EquipInfo;
		char        szTempValue[KATTRIBUTE_STRING_LEN];

		bRetCode = piTabFile->GetInteger(nIndex + 1, "ID", DefaultInfo.dwID, (int*)&dwID);
		KGLOG_PROCESS_ERROR(bRetCode);

		if (nIndex != 1)
			KGLOG_PROCESS_ERROR(dwID < MAX_ITEM_COUNT);
		else
			KGLOG_PROCESS_ERROR(dwID == ERROR_ID);

		KGLOG_PROCESS_ERROR(GetItemInfo(dwID) == NULL);	

		EquipInfo.dwID = dwID;

		bRetCode = piTabFile->GetInteger(nIndex + 1, "Value", DefaultInfo.nImportance, &EquipInfo.nImportance);
		KGLOG_PROCESS_ERROR(bRetCode);

		bRetCode = piTabFile->GetString(nIndex + 1, "ModifyType", "atInvalid", szTempValue, sizeof(szTempValue));
		KGLOG_PROCESS_ERROR(bRetCode);

		MAP_STRING_EXTERN(ATTRIBUTE_TYPE, szTempValue, EquipInfo.nAttribID);
		if (bRetCode == -1)
		{
			KGLOG_PROCESS_ERROR(nIndex > 1);
			EquipInfo.nAttribID = DefaultInfo.nAttribID;
		}

		for (int i = 0; i < MAX_ATTRIB_PARAM_NUM; i++)
		{
			char szKey[32];

			sprintf(szKey, "Param%dMin", i + 1);
			bRetCode = piTabFile->GetInteger(nIndex + 1, szKey, DefaultInfo.Param[i].nMin, &EquipInfo.Param[i].nMin);
			KGLOG_PROCESS_ERROR(bRetCode);

			sprintf(szKey, "Param%dMax", i + 1);
			bRetCode = piTabFile->GetInteger(nIndex + 1, szKey, DefaultInfo.Param[i].nMax, &EquipInfo.Param[i].nMax);
			KGLOG_PROCESS_ERROR(bRetCode);
		}

		InsRet = m_ItemInfoList.insert(std::make_pair(dwID, EquipInfo));
		KGLOG_PROCESS_ERROR(InsRet.second);

		if (nIndex == 1)
		{
			memcpy(&DefaultInfo, &EquipInfo, sizeof(KAttribInfo));
		}
	}

	bResult = true;
Exit0:
	KG_COM_RELEASE(piTabFile);
	return bResult;
}

void KAttribInfoList::Clear()
{
    m_ItemInfoList.clear();
}

KAttribInfo* KAttribInfoList::GetItemInfo(DWORD dwID)
{
    KAttribInfo*                pResult = NULL;
    ITEM_INFO_LIST::iterator    it;

	KG_PROCESS_ERROR(dwID < MAX_ITEM_COUNT);

	it = m_ItemInfoList.find(dwID);
	KG_PROCESS_ERROR(it != m_ItemInfoList.end());

    pResult = &it->second;
Exit0:
	return pResult;
}

//////////////////////////////////////////////////////////////////////////

BOOL KSetInfoList::Load(char* pszFile)
{
    BOOL        bResult     = false;
	BOOL        bRetCode    = false;
	ITabFile*   piTabFile   = NULL;
    int         nHeight     = 0;
	KSetInfo	DefaultInfo;
	std::pair<ITEM_INFO_LIST::iterator, bool> InsRet;

	piTabFile = g_OpenTabFile(pszFile);
	KG_PROCESS_ERROR(piTabFile);

	ZeroMemory(&DefaultInfo, sizeof(KSetInfo));

	nHeight = piTabFile->GetHeight();
	KGLOG_PROCESS_ERROR(nHeight > 1);

	for (int nIndex = 1; nIndex < nHeight; nIndex++)
	{
		DWORD		dwID        = 0;
		KSetInfo	EquipInfo;

		ZeroMemory(&EquipInfo, sizeof(KSetInfo));

		bRetCode = piTabFile->GetInteger(nIndex + 1, "ID", DefaultInfo.dwID, (int*)&dwID);
		KGLOG_PROCESS_ERROR(bRetCode);

		if (nIndex != 1)
			KGLOG_PROCESS_ERROR(dwID < MAX_ITEM_COUNT);
		else
			KGLOG_PROCESS_ERROR(dwID == ERROR_ID);

		KGLOG_PROCESS_ERROR(GetItemInfo(dwID) == NULL);	

		EquipInfo.dwID = dwID;

		bRetCode = piTabFile->GetString(nIndex + 1, "Name", DefaultInfo.szName, EquipInfo.szName, sizeof(EquipInfo.szName));
		KGLOG_PROCESS_ERROR(bRetCode);

#ifdef _CLIENT
		bRetCode = piTabFile->GetInteger(nIndex + 1, "UiID", DefaultInfo.dwUiID, (int *)&EquipInfo.dwUiID);
		KGLOG_PROCESS_ERROR(bRetCode);
#endif

		for (int i = 0; i < MAX_SET_ATTR_NUM; i++)
		{
			char szKey[32];

			sprintf(szKey, "%d", i + 2);
			bRetCode = piTabFile->GetInteger(nIndex + 1, szKey, DefaultInfo.dwAttribID[i], (int*)&EquipInfo.dwAttribID[i]);
			KGLOG_PROCESS_ERROR(bRetCode);
		}

		InsRet = m_ItemInfoList.insert(std::make_pair(dwID, EquipInfo));
		KGLOG_PROCESS_ERROR(InsRet.second);

		if (nIndex == 1)	// 读过一遍后复制默认值
		{
			memcpy(&DefaultInfo, &EquipInfo, sizeof(KSetInfo));
		}
	}

	bResult = true;
Exit0:
	KG_COM_RELEASE(piTabFile);
	return bResult;
}

void KSetInfoList::Clear()
{
    m_ItemInfoList.clear();
}

KSetInfo* KSetInfoList::GetItemInfo(DWORD dwID)
{
    KSetInfo*                   pResult = NULL;
    ITEM_INFO_LIST::iterator    it;

	KG_PROCESS_ERROR(dwID < MAX_ITEM_COUNT);

	it = m_ItemInfoList.find(dwID);
	KG_PROCESS_ERROR(it != m_ItemInfoList.end());

    pResult = &it->second;
Exit0:
	return pResult;
}

BOOL KBoxInfoList::Load(const char cszFileName[])
{
    BOOL        bResult     = false;
    BOOL        bRetCode    = false;
    ITabFile*   piTabFile   = NULL;
    int         nHeight     = 0;
    KBOX_ITEM*  pBoxItem    = NULL;
    DWORD       dwID        = 0;
    pair<ITEM_BOX_INFO_LIST::iterator, bool> ItRes;
    char        szColName[_NAME_LEN];
    KBOX_INFO   BoxInfo;

    assert(cszFileName);

    piTabFile = g_OpenTabFile(cszFileName);
    KGLOG_PROCESS_ERROR(piTabFile);
    
    nHeight = piTabFile->GetHeight();
    if (nHeight == 1)
        goto Exit1;

    KGLOG_PROCESS_ERROR(nHeight >= 2);
    
    for (int nIndex = 2; nIndex <= nHeight; nIndex++)
    {
        ZeroMemory(&BoxInfo, sizeof(BoxInfo));

        bRetCode = piTabFile->GetInteger(nIndex, "ID", 0, (int*)&dwID);
        KGLOG_PROCESS_ERROR(bRetCode);
    
        for (int i = 0; i < BOX_ITEM_RATE_TYPE_NUM; ++i)
        {
            snprintf(szColName, sizeof(szColName), "RateType%d", i + 1);
            szColName[sizeof(szColName) - 1] = '\0';

            bRetCode = piTabFile->GetInteger(nIndex, szColName, ebrtInvalid, (int*)&BoxInfo.nRateType[i]);
            KGLOG_PROCESS_ERROR(bRetCode);
        }
        
        for (int i = 0; i < MAX_BOX_ITEM_NUM; ++i)
        {
            pBoxItem = &BoxInfo.BoxItem[i];

            snprintf(szColName, sizeof(szColName), "TabType%d", i + 1);
            szColName[sizeof(szColName) - 1] = '\0';

            bRetCode = piTabFile->GetInteger(nIndex, szColName, 0, (int*)&pBoxItem->dwTabType);
            KGLOG_PROCESS_ERROR(bRetCode);
        }
        
        for (int i = 0; i < MAX_BOX_ITEM_NUM; ++i)
        {
            pBoxItem = &BoxInfo.BoxItem[i];

            snprintf(szColName, sizeof(szColName), "TabIndex%d", i + 1);
            szColName[sizeof(szColName) - 1] = '\0';

            bRetCode = piTabFile->GetInteger(nIndex, szColName, 0, (int*)&pBoxItem->dwTabIndex);
            KGLOG_PROCESS_ERROR(bRetCode);
        }

        for (int i = 0; i < MAX_BOX_ITEM_NUM; ++i)
        {
            pBoxItem = &BoxInfo.BoxItem[i];

            snprintf(szColName, sizeof(szColName), "StackNum%d", i + 1);
            szColName[sizeof(szColName) - 1] = '\0';

            bRetCode = piTabFile->GetInteger(nIndex, szColName, 0, (int*)&pBoxItem->nStackNum);
            KGLOG_PROCESS_ERROR(bRetCode);
        }

        for (int i = 0; i < MAX_BOX_ITEM_NUM; ++i)
        {
            pBoxItem = &BoxInfo.BoxItem[i];

            snprintf(szColName, sizeof(szColName), "Rate%d", i + 1);
            szColName[sizeof(szColName) - 1] = '\0';

            bRetCode = piTabFile->GetInteger(nIndex, szColName, 0, &pBoxItem->nRate);
            KGLOG_PROCESS_ERROR(bRetCode);
        }
        
        for (int i = 0; i < BOX_ITEM_RATE_TYPE_NUM; ++i)
        {
            if (BoxInfo.nRateType[i] == ebrtUniform)
            {
                int nCurrentDropRate = -1;
                for (int j = i * BOX_ITEM_GROUP_NUM; j < (i + 1) * BOX_ITEM_GROUP_NUM; j++)
                {
                    nCurrentDropRate += BoxInfo.BoxItem[j].nRate;
                    BoxInfo.BoxItem[j].nRate = nCurrentDropRate;
                }
                KGLOG_PROCESS_ERROR(nCurrentDropRate < MILLION_NUM);
            }
        }

        ItRes = m_BoxInfoList.insert(make_pair<DWORD, KBOX_INFO>(dwID, BoxInfo));
        KGLOG_PROCESS_ERROR(ItRes.second);
    }

Exit1:
    bResult = true;
Exit0:
    KG_COM_RELEASE(piTabFile);
    return bResult;
}   

KBOX_INFO* KBoxInfoList::GetBoxInfo(DWORD dwID)
{
    KBOX_INFO* pResult = NULL;
    ITEM_BOX_INFO_LIST::iterator It;

    It = m_BoxInfoList.find(dwID);
    KG_PROCESS_ERROR(It != m_BoxInfoList.end());

    pResult = &(It->second);
Exit0:
    return pResult;
}
