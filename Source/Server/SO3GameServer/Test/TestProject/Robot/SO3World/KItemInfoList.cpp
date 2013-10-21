#include "stdafx.h"
#include "StrMap.h"
#include "KItemInfoList.h"
#include "KItemLib.h"

// 一般装备的特化
template <>
BOOL KItemInfoList<KEquipInfo>::Init(char* pszFile)
{
	BOOL bResult = FALSE;
	BOOL bRetCode = FALSE;
	ITabFile* pFile = NULL;
	std::pair<ITEM_INFO_LIST::iterator, bool> InsRet;

	KG_PROCESS_ERROR(pszFile);

	pFile = g_OpenTabFile(pszFile);
	KGLOG_PROCESS_ERROR(pFile);

	KEquipInfo	DefaultInfo;
	ZeroMemory(&DefaultInfo, sizeof(KEquipInfo));

	int nHeight = pFile->GetHeight();
	// 第一行是默认值, 所以一个有效的表至少有两行。
	KGLOG_PROCESS_ERROR(nHeight > 1);

	for (int nIndex = 1; nIndex < nHeight; nIndex++)
	{
		DWORD dwID = 0;
		KEquipInfo	EquipInfo;

		bRetCode = pFile->GetInteger(nIndex + 1, "ID", 
			DefaultInfo.dwID, (int*)&dwID);
		KGLOG_PROCESS_ERROR(bRetCode);
		// 保证ID有效
		if (nIndex != 1)	// 默认值必须为ERROR_ID
			KGLOG_PROCESS_ERROR(dwID < MAX_ITEM_COUNT);
		else
			KGLOG_PROCESS_ERROR(dwID == ERROR_ID);
		// 保证ID不能填重
		KGLOG_PROCESS_ERROR(GetItemInfo(dwID) == NULL);	

		EquipInfo.dwID = dwID;

		bRetCode = pFile->GetString(nIndex + 1, "Name",
			DefaultInfo.szName, EquipInfo.szName, ITEM_NAME_LEN);
		KGLOG_PROCESS_ERROR(bRetCode);

#ifdef _CLIENT
		bRetCode = pFile->GetInteger(nIndex + 1, "UiID",
			DefaultInfo.nUiId, &EquipInfo.nUiId);
		KGLOG_PROCESS_ERROR(bRetCode);
#endif

		bRetCode = pFile->GetInteger(nIndex + 1, "RepresentID",
			DefaultInfo.nRepresentId, &EquipInfo.nRepresentId);
		KGLOG_PROCESS_ERROR(bRetCode);

		bRetCode = pFile->GetInteger(nIndex + 1, "Genre",
			DefaultInfo.nGenre, &EquipInfo.nGenre);
		KGLOG_PROCESS_ERROR(bRetCode);

		bRetCode = pFile->GetInteger(nIndex + 1, "SubType", 
			DefaultInfo.nSub, &EquipInfo.nSub);
		KGLOG_PROCESS_ERROR(bRetCode);

		bRetCode = pFile->GetInteger(nIndex + 1, "DetailType", 
			DefaultInfo.nDetail, &EquipInfo.nDetail);
		KGLOG_PROCESS_ERROR(bRetCode);

		bRetCode = pFile->GetInteger(nIndex + 1, "Price",
			DefaultInfo.nPrice, &EquipInfo.nPrice);
		KGLOG_PROCESS_ERROR(bRetCode);

		bRetCode = pFile->GetInteger(nIndex + 1, "Level",
			DefaultInfo.nLevel, &EquipInfo.nLevel);
		KGLOG_PROCESS_ERROR(bRetCode);

		bRetCode = pFile->GetInteger(nIndex + 1, "CanTrade",
			DefaultInfo.bCanTrade, &EquipInfo.bCanTrade);
		KGLOG_PROCESS_ERROR(bRetCode);

		bRetCode = pFile->GetInteger(nIndex + 1, "CanDestory",
			DefaultInfo.bCanDestory, &EquipInfo.bCanDestory);
		KGLOG_PROCESS_ERROR(bRetCode);

		bRetCode = pFile->GetInteger(nIndex + 1, "MaxDurability",
			DefaultInfo.nMaxDurability, &EquipInfo.nMaxDurability);
		KGLOG_PROCESS_ERROR(bRetCode);

		bRetCode = pFile->GetInteger(nIndex + 1, "MaxExistTime",
			DefaultInfo.nMaxExistTime, &EquipInfo.nMaxExistTime);
		KGLOG_PROCESS_ERROR(bRetCode);

		bRetCode = pFile->GetInteger(nIndex + 1, "MaxExistAmount",
			DefaultInfo.nMaxExistAmount, &EquipInfo.nMaxExistAmount);
		KGLOG_PROCESS_ERROR(bRetCode);

		bRetCode = pFile->GetInteger(nIndex + 1, "AttrGroup", 
			DefaultInfo.nAttribGroup, &EquipInfo.nAttribGroup);
		KGLOG_PROCESS_ERROR(bRetCode);

/*		char	szScriptName[MAX_PATH];
		bRetCode = pFile->GetString(nIndex + 1, "ScriptName", 
			"", szScriptName, MAX_PATH);
		KGLOG_PROCESS_ERROR(bRetCode);*/
		// 随机装备无需脚本支持
		EquipInfo.dwScriptID = 0;

		for (int i = 0; i < MAX_ITEM_BASE_COUNT; i++)
		{
			char szKey[32];
			char szTempValue[32];

			sprintf(szKey, "Base%dType", i + 1);
			bRetCode = pFile->GetString(nIndex + 1, szKey, 
				"atInvalid",
				szTempValue, sizeof(szTempValue));
			KGLOG_PROCESS_ERROR(bRetCode);

			MAP_STRING_EXTERN(ATTRIBUTE_TYPE, szTempValue, EquipInfo.BaseAttrib[i].nAttribID);
			if (bRetCode == -1)	// 采用默认值
			{
				KGLOG_PROCESS_ERROR(nIndex > 1);	// 默认值都没填，出错
				EquipInfo.BaseAttrib[i].nAttribID = DefaultInfo.BaseAttrib[i].nAttribID;
			}

			sprintf(szKey, "Base%dMin", i + 1);
			bRetCode = pFile->GetInteger(nIndex + 1, szKey,
				DefaultInfo.BaseAttrib[i].nMin,
				&EquipInfo.BaseAttrib[i].nMin);
			KGLOG_PROCESS_ERROR(bRetCode);

			sprintf(szKey, "Base%dMax", i + 1);
			bRetCode = pFile->GetInteger(nIndex + 1, szKey,
				DefaultInfo.BaseAttrib[i].nMax,
				&EquipInfo.BaseAttrib[i].nMax);
			KGLOG_PROCESS_ERROR(bRetCode);
		}

		for (int i = 0; i < MAX_ITEM_REQUIRE_COUNT; i++)
		{
			char szKey[32];

			sprintf(szKey, "Require%dType", i + 1);
			bRetCode = pFile->GetInteger(nIndex + 1, szKey,
				DefaultInfo.RequireAttrib[i].nAttribID,
				&EquipInfo.RequireAttrib[i].nAttribID);
			KGLOG_PROCESS_ERROR(bRetCode);

			sprintf(szKey, "Require%dValue", i + 1);
			bRetCode = pFile->GetInteger(nIndex + 1, szKey,
				DefaultInfo.RequireAttrib[i].nValue,
				&EquipInfo.RequireAttrib[i].nValue);
			KGLOG_PROCESS_ERROR(bRetCode);
		}

		for (int i = 0; i < MAX_QUALITY_NUM; i++)
		{
			char szKey[32];

			sprintf(szKey, "Quality%d", i + 1);
			bRetCode = pFile->GetInteger(nIndex + 1, szKey, 
				DefaultInfo.nQualityRate[i], &EquipInfo.nQualityRate[i]);
			KGLOG_PROCESS_ERROR(bRetCode);
		}

		InsRet = m_ItemInfoList.insert(std::make_pair(dwID, EquipInfo));
		KGLOG_PROCESS_ERROR(InsRet.second);

		if (nIndex == 1)	// 读过一遍后复制默认值
		{
			memcpy(&DefaultInfo, &EquipInfo, sizeof(KEquipInfo));
		}
	}
	bResult = TRUE;
Exit0:
	KG_COM_RELEASE(pFile);
	return bResult;
}

// 普通道具的特化
template <>
BOOL KItemInfoList<KOtherItemInfo>::Init(char* pszFile)
{
	BOOL bResult = FALSE;
	BOOL bRetCode = FALSE;
	ITabFile* pFile = NULL;
	std::pair<ITEM_INFO_LIST::iterator, bool> InsRet;

	KG_PROCESS_ERROR(pszFile);
	pFile = g_OpenTabFile(pszFile);
	KG_PROCESS_ERROR(pFile);

	KOtherItemInfo	DefaultInfo;
	ZeroMemory(&DefaultInfo, sizeof(KOtherItemInfo));

	int nHeight = pFile->GetHeight();
	// 第一行是默认值, 所以一个有效的表至少有两行。
	KGLOG_PROCESS_ERROR(nHeight > 1);

	for (int nIndex = 1; nIndex < nHeight; nIndex++)
	{
		DWORD dwID = 0;
		KOtherItemInfo	EquipInfo;

		bRetCode = pFile->GetInteger(nIndex + 1, "ID", 
			DefaultInfo.dwID, (int*)&dwID);
		KGLOG_PROCESS_ERROR(bRetCode);
		// 保证ID有效
		if (nIndex != 1)	// 默认值必须为ERROR_ID
			KGLOG_PROCESS_ERROR(dwID < MAX_ITEM_COUNT);
		else
			KGLOG_PROCESS_ERROR(dwID == ERROR_ID);
		// 保证ID不能填重
		KGLOG_PROCESS_ERROR(GetItemInfo(dwID) == NULL);	

		EquipInfo.dwID = dwID;

		bRetCode = pFile->GetString(nIndex + 1, "Name",
			DefaultInfo.szName, EquipInfo.szName, ITEM_NAME_LEN);
		KGLOG_PROCESS_ERROR(bRetCode);

#ifdef _CLIENT
		bRetCode = pFile->GetInteger(nIndex + 1, "UiID",
			DefaultInfo.nUiId, &EquipInfo.nUiId);
		KGLOG_PROCESS_ERROR(bRetCode);
#endif

		bRetCode = pFile->GetInteger(nIndex + 1, "Genre",
			DefaultInfo.nGenre, &EquipInfo.nGenre);
		KGLOG_PROCESS_ERROR(bRetCode);

		bRetCode = pFile->GetInteger(nIndex + 1, "SubType", 
			DefaultInfo.nSub, &EquipInfo.nSub);
		KGLOG_PROCESS_ERROR(bRetCode);

		bRetCode = pFile->GetInteger(nIndex + 1, "DetailType", 
			DefaultInfo.nDetail, &EquipInfo.nDetail);
		KGLOG_PROCESS_ERROR(bRetCode);

		bRetCode = pFile->GetInteger(nIndex + 1, "Price",
			DefaultInfo.nPrice, &EquipInfo.nPrice);
		KGLOG_PROCESS_ERROR(bRetCode);

		bRetCode = pFile->GetInteger(nIndex + 1, "BindType",
			DefaultInfo.nBindType, &EquipInfo.nBindType);
		KGLOG_PROCESS_ERROR(bRetCode);

		bRetCode = pFile->GetInteger(nIndex + 1, "CanTrade",
			DefaultInfo.bCanTrade, &EquipInfo.bCanTrade);
		KGLOG_PROCESS_ERROR(bRetCode);

		bRetCode = pFile->GetInteger(nIndex + 1, "CanDestory",
			DefaultInfo.bCanDestory, &EquipInfo.bCanDestory);
		KGLOG_PROCESS_ERROR(bRetCode);

		bRetCode = pFile->GetInteger(nIndex + 1, "CanConsume", 
			DefaultInfo.bCanConsume, &EquipInfo.bCanConsume);
		KGLOG_PROCESS_ERROR(bRetCode);

		bRetCode = pFile->GetInteger(nIndex + 1, "CanStack",
			DefaultInfo.bCanStack, &EquipInfo.bCanStack);
		KGLOG_PROCESS_ERROR(bRetCode);

		bRetCode = pFile->GetInteger(nIndex + 1, "MaxDurability",
			DefaultInfo.nMaxDurability, &EquipInfo.nMaxDurability);
		KGLOG_PROCESS_ERROR(bRetCode);

		bRetCode = pFile->GetInteger(nIndex + 1, "MaxExistTime",
			DefaultInfo.nMaxExistTime, &EquipInfo.nMaxExistTime);
		KGLOG_PROCESS_ERROR(bRetCode);

		bRetCode = pFile->GetInteger(nIndex + 1, "MaxExistAmount",
			DefaultInfo.nMaxExistAmount, &EquipInfo.nMaxExistAmount);
		KGLOG_PROCESS_ERROR(bRetCode);

		char	szScriptName[MAX_PATH];
		bRetCode = pFile->GetString(nIndex + 1, "ScriptName", 
			"", szScriptName, MAX_PATH);
		KGLOG_PROCESS_ERROR(bRetCode);
		EquipInfo.dwScriptID = g_FileNameHash(szScriptName);

		bRetCode = pFile->GetInteger(nIndex + 1, "SkillID", 
			DefaultInfo.dwSkillID, (int *)&EquipInfo.dwSkillID);
		KGLOG_PROCESS_ERROR(bRetCode);

		bRetCode = pFile->GetInteger(nIndex + 1, "SkillLevel",
			DefaultInfo.dwSkillLevel, (int *)&EquipInfo.dwSkillLevel);
		KGLOG_PROCESS_ERROR(bRetCode);

		InsRet = m_ItemInfoList.insert(std::make_pair(dwID, EquipInfo));
		KGLOG_PROCESS_ERROR(InsRet.second);

		if (nIndex == 1)	// 读过一遍后复制默认值
		{
			memcpy(&DefaultInfo, &EquipInfo, sizeof(KOtherItemInfo));
		}
	}

	bResult = TRUE;
Exit0:
	KG_COM_RELEASE(pFile);
	return bResult;
}

// 定制装备的特化
template <>
BOOL KItemInfoList<KCustomEquipInfo>::Init(char* pszFile)
{
	BOOL bResult = FALSE;
	BOOL bRetCode = FALSE;
	ITabFile* pFile = NULL;
	std::pair<ITEM_INFO_LIST::iterator, bool> InsRet;

	KG_PROCESS_ERROR(pszFile);

	pFile = g_OpenTabFile(pszFile);
	KG_PROCESS_ERROR(pFile);

	KCustomEquipInfo	DefaultInfo;
	ZeroMemory(&DefaultInfo, sizeof(KCustomEquipInfo));

	int nHeight = pFile->GetHeight();
	// 第一行是默认值, 所以一个有效的表至少有两行。
	KGLOG_PROCESS_ERROR(nHeight > 1);

	for (int nIndex = 1; nIndex < nHeight; nIndex++)
	{
		DWORD	dwID = 0;
		KCustomEquipInfo	EquipInfo;

		bRetCode = pFile->GetInteger(nIndex + 1, "ID",
			DefaultInfo.dwID, (int*)&dwID);
		KGLOG_PROCESS_ERROR(bRetCode);
		// 保证ID有效
		if (nIndex != 1)	// 默认值必须为ERROR_ID
			KGLOG_PROCESS_ERROR(dwID < MAX_ITEM_COUNT);
		else
			KGLOG_PROCESS_ERROR(dwID == ERROR_ID);
		// 保证ID不能填重
		KGLOG_PROCESS_ERROR(GetItemInfo(dwID) == NULL);	

		EquipInfo.dwID = dwID;

		bRetCode = pFile->GetString(nIndex + 1, "Name",
			DefaultInfo.szName, EquipInfo.szName, ITEM_NAME_LEN);
		KGLOG_PROCESS_ERROR(bRetCode);

#ifdef _CLIENT
		bRetCode = pFile->GetInteger(nIndex + 1, "UiID",
			DefaultInfo.nUiId, &EquipInfo.nUiId);
		KGLOG_PROCESS_ERROR(bRetCode);
#endif

		bRetCode = pFile->GetInteger(nIndex + 1, "RepresentID",
			DefaultInfo.nRepresentId, &EquipInfo.nRepresentId);
		KGLOG_PROCESS_ERROR(bRetCode);

		bRetCode = pFile->GetInteger(nIndex + 1, "Genre",
			DefaultInfo.nGenre, &EquipInfo.nGenre);
		KGLOG_PROCESS_ERROR(bRetCode);

		bRetCode = pFile->GetInteger(nIndex + 1, "SubType", 
			DefaultInfo.nSub, &EquipInfo.nSub);
		KGLOG_PROCESS_ERROR(bRetCode);

		bRetCode = pFile->GetInteger(nIndex + 1, "DetailType", 
			DefaultInfo.nDetail, &EquipInfo.nDetail);
		KGLOG_PROCESS_ERROR(bRetCode);

		bRetCode = pFile->GetInteger(nIndex + 1, "Price",
			DefaultInfo.nPrice, &EquipInfo.nPrice);
		KGLOG_PROCESS_ERROR(bRetCode);

		bRetCode = pFile->GetInteger(nIndex + 1, "Level",
			DefaultInfo.nLevel, &EquipInfo.nLevel);
		KGLOG_PROCESS_ERROR(bRetCode);

		bRetCode = pFile->GetInteger(nIndex + 1, "BindType",
			DefaultInfo.nBindType, &EquipInfo.nBindType);
		KGLOG_PROCESS_ERROR(bRetCode);

		bRetCode = pFile->GetInteger(nIndex + 1, "Quality",
			DefaultInfo.nQuality, &EquipInfo.nQuality);
		KGLOG_PROCESS_ERROR(bRetCode);

		bRetCode = pFile->GetInteger(nIndex + 1, "CanTrade",
			DefaultInfo.bCanTrade, &EquipInfo.bCanTrade);
		KGLOG_PROCESS_ERROR(bRetCode);

		bRetCode = pFile->GetInteger(nIndex + 1, "CanDestory",
			DefaultInfo.bCanDestory, &EquipInfo.bCanDestory);
		KGLOG_PROCESS_ERROR(bRetCode);

		bRetCode = pFile->GetInteger(nIndex + 1, "MaxDurability",
			DefaultInfo.nMaxDurability, &EquipInfo.nMaxDurability);
		KGLOG_PROCESS_ERROR(bRetCode);

		bRetCode = pFile->GetInteger(nIndex + 1, "MaxExistTime",
			DefaultInfo.nMaxExistTime, &EquipInfo.nMaxExistTime);
		KGLOG_PROCESS_ERROR(bRetCode);

		bRetCode = pFile->GetInteger(nIndex + 1, "MaxExistAmount",
			DefaultInfo.nMaxExistAmount, &EquipInfo.nMaxExistAmount);
		KGLOG_PROCESS_ERROR(bRetCode);

		bRetCode = pFile->GetInteger(nIndex + 1, "SetID",
			DefaultInfo.nSetID, &EquipInfo.nSetID);
		KGLOG_PROCESS_ERROR(bRetCode);

		bRetCode = pFile->GetInteger(nIndex + 1, "AbradeRate",
			DefaultInfo.nAbradeRate, &EquipInfo.nAbradeRate);

		char	szScriptName[MAX_PATH];
		bRetCode = pFile->GetString(nIndex + 1, "ScriptName", 
			"", szScriptName, MAX_PATH);
		KGLOG_PROCESS_ERROR(bRetCode);
		EquipInfo.dwScriptID = g_FileNameHash(szScriptName);

		for (int i = 0; i < MAX_ITEM_BASE_COUNT; i++)
		{
			char szKey[32];
			char szTempValue[32];

			sprintf(szKey, "Base%dType", i + 1);
			bRetCode = pFile->GetString(nIndex + 1, szKey, 
				"atInvalid",
				szTempValue, sizeof(szTempValue));
			KGLOG_PROCESS_ERROR(bRetCode);

			MAP_STRING_EXTERN(ATTRIBUTE_TYPE, szTempValue, EquipInfo.BaseAttrib[i].nAttribID);
			if (bRetCode == -1)	// 采用默认值
			{
				KGLOG_PROCESS_ERROR(nIndex > 1);	// 默认值都没填，出错
				EquipInfo.BaseAttrib[i].nAttribID = DefaultInfo.BaseAttrib[i].nAttribID;
			}

			sprintf(szKey, "Base%dMin", i + 1);
			bRetCode = pFile->GetInteger(nIndex + 1, szKey,
				DefaultInfo.BaseAttrib[i].nMin,
				&EquipInfo.BaseAttrib[i].nMin);
			KGLOG_PROCESS_ERROR(bRetCode);

			sprintf(szKey, "Base%dMax", i + 1);
			bRetCode = pFile->GetInteger(nIndex + 1, szKey,
				DefaultInfo.BaseAttrib[i].nMax,
				&EquipInfo.BaseAttrib[i].nMax);
			KGLOG_PROCESS_ERROR(bRetCode);
		}

		for (int i = 0; i < MAX_ITEM_REQUIRE_COUNT; i++)
		{
			char szKey[32];

			sprintf(szKey, "Require%dType", i + 1);
			bRetCode = pFile->GetInteger(nIndex + 1, szKey,
				DefaultInfo.RequireAttrib[i].nAttribID,
				&EquipInfo.RequireAttrib[i].nAttribID);
			KGLOG_PROCESS_ERROR(bRetCode);

			sprintf(szKey, "Require%dValue", i + 1);
			bRetCode = pFile->GetInteger(nIndex + 1, szKey,
				DefaultInfo.RequireAttrib[i].nValue,
				&EquipInfo.RequireAttrib[i].nValue);
			KGLOG_PROCESS_ERROR(bRetCode);
		}

		for (int i = 0; i < MAX_ITEM_MAGIC_COUNT; i++)
		{
			char szKey[32];

			sprintf(szKey, "Magic%dType", i + 1);
			bRetCode = pFile->GetInteger(nIndex + 1, szKey, 
				DefaultInfo.nAttribIndex[i],
				&EquipInfo.nAttribIndex[i]);
			KGLOG_PROCESS_ERROR(bRetCode);
		}

		InsRet = m_ItemInfoList.insert(std::make_pair(dwID, EquipInfo));
		KGLOG_PROCESS_ERROR(InsRet.second);

		if (nIndex == 1)	// 读过一遍后复制默认值
		{
			memcpy(&DefaultInfo, &EquipInfo, sizeof(KCustomEquipInfo));
		}
	}

	bResult = TRUE;
Exit0:
	KG_COM_RELEASE(pFile);
	return bResult;
}

template <>
BOOL KItemInfoList<KAttribInfo>::Init(char* pszFile)
{
	BOOL bResult = FALSE;
	BOOL bRetCode = FALSE;
	ITabFile* pFile = NULL;
	std::pair<ITEM_INFO_LIST::iterator, bool> InsRet;

	KG_PROCESS_ERROR(pszFile);

	pFile = g_OpenTabFile(pszFile);
	KG_PROCESS_ERROR(pFile);

	KAttribInfo	DefaultInfo;
	ZeroMemory(&DefaultInfo, sizeof(KAttribInfo));

	int nHeight = pFile->GetHeight();
	// 第一行是默认值, 所以一个有效的表至少有两行。
	KGLOG_PROCESS_ERROR(nHeight > 1);

	for (int nIndex = 1; nIndex < nHeight; nIndex++)
	{
		DWORD	dwID = 0;
		KAttribInfo	EquipInfo;

		bRetCode = pFile->GetInteger(nIndex + 1, "ID",
			DefaultInfo.dwID, (int*)&dwID);
		KGLOG_PROCESS_ERROR(bRetCode);
		// 保证ID有效
		if (nIndex != 1)	// 默认值必须为ERROR_ID
			KGLOG_PROCESS_ERROR(dwID < MAX_ITEM_COUNT);
		else
			KGLOG_PROCESS_ERROR(dwID == ERROR_ID);
		// 保证ID不能填重
		KGLOG_PROCESS_ERROR(GetItemInfo(dwID) == NULL);	

		EquipInfo.dwID = dwID;

		bRetCode = pFile->GetInteger(nIndex + 1, "Value",
			DefaultInfo.nImportance, &EquipInfo.nImportance);
		KGLOG_PROCESS_ERROR(bRetCode);

		char szTempValue[32];
		bRetCode = pFile->GetString(nIndex + 1, "ModifyType", 
			"atInvalid", szTempValue, sizeof(szTempValue));
		KGLOG_PROCESS_ERROR(bRetCode);

		MAP_STRING_EXTERN(ATTRIBUTE_TYPE, szTempValue, EquipInfo.nAttribID);
		if (bRetCode == -1)	// 采用默认值
		{
			KGLOG_PROCESS_ERROR(nIndex > 1);	// 默认值都没填，出错
			EquipInfo.nAttribID = DefaultInfo.nAttribID;
		}

		for (int i = 0; i < MAX_ATTRIB_PARAM_NUM; i++)
		{
			char szKey[32];

			sprintf(szKey, "Param%dMin", i + 1);
			bRetCode = pFile->GetInteger(nIndex + 1, szKey,
				DefaultInfo.Param[i].nMin,
				&EquipInfo.Param[i].nMin);
			KGLOG_PROCESS_ERROR(bRetCode);

			sprintf(szKey, "Param%dMax", i + 1);
			bRetCode = pFile->GetInteger(nIndex + 1, szKey,
				DefaultInfo.Param[i].nMax,
				&EquipInfo.Param[i].nMax);
			KGLOG_PROCESS_ERROR(bRetCode);
		}

		InsRet = m_ItemInfoList.insert(std::make_pair(dwID, EquipInfo));
		KGLOG_PROCESS_ERROR(InsRet.second);

		if (nIndex == 1)	// 读过一遍后复制默认值
		{
			memcpy(&DefaultInfo, &EquipInfo, sizeof(KAttribInfo));
		}
	}

	bResult = TRUE;
Exit0:
	KG_COM_RELEASE(pFile);
	return bResult;
}

int KCustomEquipInfo::LuaGetBaseAttrib(Lua_State* L)
{
	int nRelIndex = 0;

	ASSERT(L);

	Lua_NewTable(L);
	int nTable = Lua_GetTopIndex(L);
	for(int i = 0; i < MAX_ITEM_BASE_COUNT; ++i)
	{
		if (BaseAttrib[i].nAttribID != atInvalid && 
			BaseAttrib[i].nAttribID != atMeleeWeaponDamageRand &&
			BaseAttrib[i].nAttribID != atRangeWeaponDamageRand
			)
		{
			++nRelIndex;

			Lua_PushNumber(L, nRelIndex);

			Lua_NewTable(L);
			int nAttrib = Lua_GetTopIndex(L);

			Lua_PushString(L, "nID");
			Lua_PushNumber(L, BaseAttrib[i].nAttribID);
			Lua_SetTable(L, nAttrib);

			Lua_PushString(L, "nMin");
			Lua_PushNumber(L, BaseAttrib[i].nMin);
			Lua_SetTable(L, nAttrib);

			Lua_PushString(L, "nMax");
			Lua_PushNumber(L, BaseAttrib[i].nMax);
			Lua_SetTable(L, nAttrib);

			if (BaseAttrib[i].nAttribID == atMeleeWeaponDamageBase)
			{
				for(int i = 0; i < MAX_ITEM_BASE_COUNT; ++i)
				{
					if (BaseAttrib[i].nAttribID == atMeleeWeaponDamageRand)
					{
						Lua_PushString(L, "nMin1");
						Lua_PushNumber(L, BaseAttrib[i].nMin);
						Lua_SetTable(L, nAttrib);

						Lua_PushString(L, "nMax1");
						Lua_PushNumber(L, BaseAttrib[i].nMax);
						Lua_SetTable(L, nAttrib);
					}
				}
			}

			if (BaseAttrib[i].nAttribID == atRangeWeaponDamageBase)
			{
				for(int i = 0; i < MAX_ITEM_BASE_COUNT; ++i)
				{
					if (BaseAttrib[i].nAttribID == atRangeWeaponDamageRand)
					{
						Lua_PushString(L, "nMin1");
						Lua_PushNumber(L, BaseAttrib[i].nMin);
						Lua_SetTable(L, nAttrib);

						Lua_PushString(L, "nMax1");
						Lua_PushNumber(L, BaseAttrib[i].nMax);
						Lua_SetTable(L, nAttrib);
					}
				}
			}


			Lua_SetTable(L, nTable);
		}
	}

	return 1;
}

int KCustomEquipInfo::LuaGetRequireAttrib(Lua_State* L)
{
	int nRelIndex = 0;

	ASSERT(L);

	Lua_NewTable(L);
	int nTable = Lua_GetTopIndex(L);
	for(int i = 0; i < MAX_ITEM_REQUIRE_COUNT; ++i)
	{
		if (RequireAttrib[i].nAttribID != atInvalid)
		{
			++nRelIndex;

			Lua_PushNumber(L, nRelIndex);

			Lua_NewTable(L);
			int nAttrib = Lua_GetTopIndex(L);

			Lua_PushString(L, "nID");
			Lua_PushNumber(L, RequireAttrib[i].nAttribID);
			Lua_SetTable(L, nAttrib);

			Lua_PushString(L, "nValue");
			Lua_PushNumber(L, RequireAttrib[i].nValue);
			Lua_SetTable(L, nAttrib);

			Lua_SetTable(L, nTable);
		}
	}

	return 1;
}

int KCustomEquipInfo::LuaGetMagicAttrib(Lua_State* L)
{
	int nResult = false;
	int nRelIndex = 0;

	int nVersion = CURRENT_ITEM_VERSION;
	if (Lua_GetTopIndex(L) >= 1)
		nVersion = (int)Lua_ValueToNumber(L, 1);
	KGLOG_PROCESS_ERROR(nVersion >= 0);
	KGLOG_PROCESS_ERROR(nVersion <= CURRENT_ITEM_VERSION);

	ASSERT(L);

	Lua_NewTable(L);
	int nTable = Lua_GetTopIndex(L);
	for(int i = 0; i < MAX_ITEM_MAGIC_COUNT; ++i)
	{
		KAttribInfo *pInfo = g_ItemLib.m_ItemLib[nVersion].GetAttribInfo(i);
		if (pInfo && pInfo->nAttribID != atInvalid)
		{
			++nRelIndex;

			Lua_PushNumber(L, nRelIndex);
			Lua_NewTable(L);
			int nAttrib = Lua_GetTopIndex(L);

			Lua_PushString(L, "nID");
			Lua_PushNumber(L, pInfo->nAttribID);
			Lua_SetTable(L, nAttrib);

			Lua_PushString(L, "nImportance");
			Lua_PushNumber(L, pInfo->nImportance);
			Lua_SetTable(L, nAttrib);

			for (int j = 0; j < MAX_ATTRIB_PARAM_NUM; ++j)
			{
				char szKey[16];
				sprintf(szKey, "%s%d", "Param", (j * 2));

				Lua_PushString(L, szKey);
				Lua_PushNumber(L, pInfo->Param[j].nMin);
				Lua_SetTable(L, nAttrib);

				sprintf(szKey, "%s%d", "Param", (j * 2 + 1));
				Lua_PushString(L, szKey);
				Lua_PushNumber(L, pInfo->Param[j].nMax);
				Lua_SetTable(L, nAttrib);
			}

			Lua_SetTable(L, nTable);
		}
	}

	nResult = true;
Exit0:
	if (!nResult) 
		Lua_PushNil(L);
	return 1;
}
