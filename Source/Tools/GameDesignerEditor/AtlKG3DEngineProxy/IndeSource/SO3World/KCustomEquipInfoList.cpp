////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KCustomEquipInfoList.cpp 
//  Version     : 1.0
//  Creator     : Zhao Chunfeng
//  Create Date : 2008-07-22 11:24:42
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "StrMap.h"
#include "KItemInfoList.h"
#include "KSO3World.h"
#include "KItemLib.h"
#include "KCustomEquipInfoList.h"

BOOL KCustomEquipInfoList::Load(const char cszFileName[])
{
	BOOL                    bResult         = false;
	BOOL                    bRetCode        = false;
	ITabFile*               piTabFile           = NULL;
    int                     nHeight         = 0;
	KCustomEquipInfo	    DefaultInfo;
	std::pair<KCUSTOM_EQUIP_INFO_LIST::iterator, bool> InsRet;

	piTabFile = g_OpenTabFile(cszFileName);
	KG_PROCESS_ERROR(piTabFile);

	ZeroMemory(&DefaultInfo, sizeof(KCustomEquipInfo));

	nHeight = piTabFile->GetHeight();
	KGLOG_PROCESS_ERROR(nHeight > 1);
    
    bRetCode = LoadLine(piTabFile, 2, &DefaultInfo, DefaultInfo);
    KGLOG_PROCESS_ERROR(bRetCode);

	for (int nIndex = 3; nIndex <= nHeight; nIndex++)
	{
        KCustomEquipInfo    EquipInfo;

	    bRetCode = LoadLine(piTabFile, nIndex, &EquipInfo, DefaultInfo);
        KGLOG_PROCESS_ERROR(bRetCode);
	
		InsRet = m_EquipInfoList.insert(std::make_pair(EquipInfo.dwID, EquipInfo));
		KGLOG_PROCESS_ERROR(InsRet.second);
	}

	bResult = true;
Exit0:
	KG_COM_RELEASE(piTabFile);
	return bResult;
}

void KCustomEquipInfoList::Clear()
{
    m_EquipInfoList.clear();
}

KCustomEquipInfo* KCustomEquipInfoList::GetEquipInfo(DWORD dwID)
{
    KCUSTOM_EQUIP_INFO_LIST::iterator it = m_EquipInfoList.find(dwID);
    if (it != m_EquipInfoList.end())
    {
        return &it->second;
    }
    return NULL;
}

BOOL KCustomEquipInfoList::LoadLine(
    ITabFile* piTabFile, int nLine, KCustomEquipInfo* pItemInfo, const KCustomEquipInfo& crDefaultInfo
)
{
    BOOL                bResult     = false;
	BOOL                bRetCode    = false;
    DWORD	            dwID        = 0;
	char	            szScriptName[MAX_PATH];

    assert(pItemInfo);

	bRetCode = piTabFile->GetInteger(nLine, "ID", crDefaultInfo.dwID, (int*)&dwID);
	KGLOG_PROCESS_ERROR(bRetCode);

    pItemInfo->dwID = dwID;
    
    KGLOG_PROCESS_ERROR((nLine == 2 && dwID == ERROR_ID) || (nLine > 2 && dwID < MAX_ITEM_COUNT));
    
	bRetCode = piTabFile->GetString(nLine, "Name", crDefaultInfo.szName, pItemInfo->szName, ITEM_NAME_LEN);
	KGLOG_PROCESS_ERROR(bRetCode);

#ifdef _CLIENT
	bRetCode = piTabFile->GetInteger(nLine, "UiID", crDefaultInfo.nUiId, &pItemInfo->nUiId);
	KGLOG_PROCESS_ERROR(bRetCode);
#endif

	bRetCode = piTabFile->GetInteger(nLine, "RepresentID", crDefaultInfo.nRepresentID, &pItemInfo->nRepresentID);
	KGLOG_PROCESS_ERROR(bRetCode);
    
    bRetCode = piTabFile->GetInteger(nLine, "ColorID", crDefaultInfo.nColorID, &pItemInfo->nColorID);
	KGLOG_PROCESS_ERROR(bRetCode);

    bRetCode = piTabFile->GetInteger(nLine, "CanSetColor", crDefaultInfo.bCanSetColor, &pItemInfo->bCanSetColor);
    KGLOG_PROCESS_ERROR(bRetCode);

	bRetCode = piTabFile->GetInteger(nLine, "Genre", crDefaultInfo.nGenre, &pItemInfo->nGenre);
	KGLOG_PROCESS_ERROR(bRetCode);

    KGLOG_PROCESS_ERROR(pItemInfo->nGenre >= igEquipment && pItemInfo->nGenre < igTotal);

	bRetCode = piTabFile->GetInteger(nLine, "SubType", crDefaultInfo.nSub, &pItemInfo->nSub);
	KGLOG_PROCESS_ERROR(bRetCode);

	bRetCode = piTabFile->GetInteger(nLine, "DetailType", crDefaultInfo.nDetail, &pItemInfo->nDetail);
	KGLOG_PROCESS_ERROR(bRetCode);

	bRetCode = piTabFile->GetInteger(nLine, "Price", crDefaultInfo.nPrice, &pItemInfo->nPrice);
	KGLOG_PROCESS_ERROR(bRetCode);

	bRetCode = piTabFile->GetInteger(nLine, "Level", crDefaultInfo.nLevel, &pItemInfo->nLevel);
	KGLOG_PROCESS_ERROR(bRetCode);

	bRetCode = piTabFile->GetInteger(nLine, "BindType", crDefaultInfo.nBindType, &pItemInfo->nBindType);
	KGLOG_PROCESS_ERROR(bRetCode);
    KGLOG_PROCESS_ERROR(pItemInfo->nBindType > ibtInvalid && pItemInfo->nBindType < ibtTotal);

	bRetCode = piTabFile->GetInteger(nLine, "Quality", crDefaultInfo.nQuality, &pItemInfo->nQuality);
	KGLOG_PROCESS_ERROR(bRetCode);

	bRetCode = piTabFile->GetInteger(nLine, "CanTrade", crDefaultInfo.bCanTrade, &pItemInfo->bCanTrade);
	KGLOG_PROCESS_ERROR(bRetCode);

	bRetCode = piTabFile->GetInteger(nLine, "CanDestroy", crDefaultInfo.bCanDestroy, &pItemInfo->bCanDestroy);
	KGLOG_PROCESS_ERROR(bRetCode);

	bRetCode = piTabFile->GetInteger(nLine, "MaxDurability", crDefaultInfo.nMaxDurability, &pItemInfo->nMaxDurability);
	KGLOG_PROCESS_ERROR(bRetCode);

	bRetCode = piTabFile->GetInteger(nLine, "MaxExistTime", crDefaultInfo.nMaxExistTime, &pItemInfo->nMaxExistTime);
	KGLOG_PROCESS_ERROR(bRetCode);

	bRetCode = piTabFile->GetInteger(nLine, "MaxExistAmount", crDefaultInfo.nMaxExistAmount, &pItemInfo->nMaxExistAmount);
	KGLOG_PROCESS_ERROR(bRetCode);

	bRetCode = piTabFile->GetInteger(nLine, "SetID", crDefaultInfo.nSetID, &pItemInfo->nSetID);
	KGLOG_PROCESS_ERROR(bRetCode);

	bRetCode = piTabFile->GetInteger(nLine, "AbradeRate", crDefaultInfo.nAbradeRate, &pItemInfo->nAbradeRate);
	KGLOG_PROCESS_ERROR(bRetCode);

	bRetCode = piTabFile->GetInteger(nLine, "SkillID", crDefaultInfo.dwSkillID, (int*)&pItemInfo->dwSkillID);
	KGLOG_PROCESS_ERROR(bRetCode);

	bRetCode = piTabFile->GetInteger(nLine, "SkillLevel", crDefaultInfo.dwSkillLevel, (int*)&pItemInfo->dwSkillLevel);
	KGLOG_PROCESS_ERROR(bRetCode);

	bRetCode = piTabFile->GetString(nLine, "ScriptName", "", szScriptName, MAX_PATH);
	KGLOG_PROCESS_ERROR(bRetCode);

    bRetCode = piTabFile->GetInteger(nLine, "AucGenre", crDefaultInfo.nAucGenre, &pItemInfo->nAucGenre);
    KGLOG_PROCESS_ERROR(bRetCode);

    bRetCode = piTabFile->GetInteger(nLine, "AucSubType", crDefaultInfo.nAucSub, &pItemInfo->nAucSub);
    KGLOG_PROCESS_ERROR(bRetCode);

	if (szScriptName[0] != '\0')
		pItemInfo->dwScriptID = g_FileNameHash(szScriptName);
	else
		pItemInfo->dwScriptID = 0;

	for (int i = 0; i < MAX_ITEM_BASE_COUNT; i++)
	{
		char szKey[32];
		char szTempValue[KATTRIBUTE_STRING_LEN];

		sprintf(szKey, "Base%dType", i + 1);
		bRetCode = piTabFile->GetString(nLine, szKey, "atInvalid", szTempValue, sizeof(szTempValue));
		KGLOG_PROCESS_ERROR(bRetCode);

		MAP_STRING_EXTERN(ATTRIBUTE_TYPE, szTempValue, pItemInfo->BaseAttrib[i].nAttribID);
		if (bRetCode == -1)	
		{
			KGLOG_PROCESS_ERROR(nLine > 1);
			pItemInfo->BaseAttrib[i].nAttribID = crDefaultInfo.BaseAttrib[i].nAttribID;
		}

		sprintf(szKey, "Base%dMin", i + 1);
		bRetCode = piTabFile->GetInteger(nLine, szKey, crDefaultInfo.BaseAttrib[i].nMin, &pItemInfo->BaseAttrib[i].nMin);
		KGLOG_PROCESS_ERROR(bRetCode);

		sprintf(szKey, "Base%dMax", i + 1);
		bRetCode = piTabFile->GetInteger(nLine, szKey, crDefaultInfo.BaseAttrib[i].nMax, &pItemInfo->BaseAttrib[i].nMax);
		KGLOG_PROCESS_ERROR(bRetCode);
	}

	for (int i = 0; i < MAX_ITEM_REQUIRE_COUNT; i++)
	{
		char szKey[32];

		sprintf(szKey, "Require%dType", i + 1);
		bRetCode = piTabFile->GetInteger(nLine, szKey, crDefaultInfo.RequireAttrib[i].nAttribID, &pItemInfo->RequireAttrib[i].nAttribID);
		KGLOG_PROCESS_ERROR(bRetCode);

		sprintf(szKey, "Require%dValue", i + 1);
		bRetCode = piTabFile->GetInteger(nLine, szKey, crDefaultInfo.RequireAttrib[i].nValue, &pItemInfo->RequireAttrib[i].nValue);
		KGLOG_PROCESS_ERROR(bRetCode);
	}

	for (int i = 0; i < MAX_ITEM_MAGIC_COUNT; i++)
	{
		char szKey[32];

		sprintf(szKey, "Magic%dType", i + 1);
		bRetCode = piTabFile->GetInteger(nLine, szKey, crDefaultInfo.nAttribIndex[i], &pItemInfo->nAttribIndex[i]);
		KGLOG_PROCESS_ERROR(bRetCode);
	}
    
    bRetCode = piTabFile->GetInteger(nLine, "CoolDownID", crDefaultInfo.dwCoolDownID, (int*)&pItemInfo->dwCoolDownID);
    KGLOG_PROCESS_ERROR(bRetCode);

    bRetCode = piTabFile->GetInteger(nLine, "RequireCamp", crDefaultInfo.nRequireCamp, (int*)&pItemInfo->nRequireCamp);
    KGLOG_PROCESS_ERROR(bRetCode);
    
    bRetCode = piTabFile->GetInteger(nLine, "RequireProfessionID", crDefaultInfo.dwRequireProfessionID, (int*)&pItemInfo->dwRequireProfessionID);
    KGLOG_PROCESS_ERROR(bRetCode);
    
    // 0表示对分支没有限制
    bRetCode = piTabFile->GetInteger(nLine, "RequireProfessionBranch", crDefaultInfo.dwRequireProfessionBranch, (int*)&pItemInfo->dwRequireProfessionBranch);
    KGLOG_PROCESS_ERROR(bRetCode);
    
    bRetCode = piTabFile->GetInteger(nLine, "RequireProfessionLevel", crDefaultInfo.nRequireProfessionLevel, &pItemInfo->nRequireProfessionLevel);
    KGLOG_PROCESS_ERROR(bRetCode);
    
    bRetCode = piTabFile->GetInteger(nLine, "PackageGenerType", crDefaultInfo.nPackageGenerType, &pItemInfo->nPackageGenerType);
    KGLOG_PROCESS_ERROR(bRetCode);
    
    bRetCode = piTabFile->GetInteger(nLine, "PackageSubType", crDefaultInfo.nPackageSubType, &pItemInfo->nPackageSubType);
    KGLOG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
	return bResult;
}

