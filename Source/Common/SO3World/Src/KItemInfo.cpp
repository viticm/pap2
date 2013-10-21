////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KItemInfo.cpp
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
#include "KPlayer.h"

#if defined _CLIENT
int KCustomEquipInfo::LuaGetBaseAttrib(Lua_State* L)
{
	int nRelIndex   = 0;
    int nTable      = 0;

	assert(L);

	Lua_NewTable(L);

	nTable = Lua_GetTopIndex(L);

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
				for(int j = 0; j < MAX_ITEM_BASE_COUNT; ++j)
				{
					if (BaseAttrib[j].nAttribID == atMeleeWeaponDamageRand)
					{
						Lua_PushString(L, "nMin1");
						Lua_PushNumber(L, BaseAttrib[j].nMin);
						Lua_SetTable(L, nAttrib);

						Lua_PushString(L, "nMax1");
						Lua_PushNumber(L, BaseAttrib[j].nMax);
						Lua_SetTable(L, nAttrib);
					}
				}
			}

			if (BaseAttrib[i].nAttribID == atRangeWeaponDamageBase)
			{
				for(int j = 0; j < MAX_ITEM_BASE_COUNT; ++j)
				{
					if (BaseAttrib[j].nAttribID == atRangeWeaponDamageRand)
					{
						Lua_PushString(L, "nMin1");
						Lua_PushNumber(L, BaseAttrib[j].nMin);
						Lua_SetTable(L, nAttrib);

						Lua_PushString(L, "nMax1");
						Lua_PushNumber(L, BaseAttrib[j].nMax);
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
	int nRelIndex   = 0;
    int nTable      = 0;

	assert(L);

	Lua_NewTable(L);

	nTable = Lua_GetTopIndex(L);

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
	int nResult     = false;
	int nRelIndex   = 0;
    int nTable      = 0;

	Lua_NewTable(L);

	nTable = Lua_GetTopIndex(L);

	for(int i = 0; i < MAX_ITEM_MAGIC_COUNT; ++i)
	{
		KAttribInfo* pInfo = g_pSO3World->m_ItemManager.m_ItemLib.GetAttribInfo(nAttribIndex[i]);
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

int KCustomEquipInfo::LuaGetSetAttrib(Lua_State* L)
{
	int         nResult         = 0;
	int         nSetEquipCount  = 0;
	int         nIndex          = 0;
	int         nEquipedNum     = 0;
    KSetInfo*   pSetInfo        = NULL;
    KPlayer*    pPlayer         = NULL;
    int         nTable          = 0;

	KG_PROCESS_ERROR(nSetID > 0);

	pSetInfo = g_pSO3World->m_ItemManager.m_ItemLib.GetSetInfo(nSetID);
	KGLOG_PROCESS_ERROR(pSetInfo);

	pPlayer = g_pSO3World->m_PlayerSet.GetObj(g_pSO3World->m_dwClientPlayerID);
	KGLOG_PROCESS_ERROR(pPlayer);

	Lua_PushNumber(L, pSetInfo->dwUiID);

	Lua_NewTable(L);

	nTable = Lua_GetTopIndex(L);

	for (int i = 0; i < MAX_SET_PART_NUM; ++i)
	{
		KCustomEquipInfo *pInfo = pSetInfo->pEquipList[i];
		if (pInfo)
		{
			++nSetEquipCount;
			Lua_PushNumber(L, nSetEquipCount);

			Lua_NewTable(L);
			int nAttrib = Lua_GetTopIndex(L);

			Lua_PushString(L, "nUiId");
			Lua_PushNumber(L, pInfo->nUiId);
			Lua_SetTable(L, nAttrib);

			Lua_PushString(L, "bEquiped");
			int nEquiped = pPlayer->IsEquiped(pInfo);
			if (nEquiped)
				++nEquipedNum;
			Lua_PushBoolean(L, nEquiped);
			Lua_SetTable(L, nAttrib);

			Lua_SetTable(L, nTable);
		}
	}

	Lua_PushNumber(L, nSetEquipCount);
	Lua_PushNumber(L, nEquipedNum);

	Lua_NewTable(L);
	nTable = Lua_GetTopIndex(L);
	nIndex = 0;

	for (int i = 1; i <= nSetEquipCount; ++i)
	{
		KAttribute Attribute;
		if (g_pSO3World->m_ItemManager.GetSetAttrib(Attribute, nUiId, i))
		{
			if (Attribute.nKey != atInvalid)
			{
				++nIndex;
				Lua_PushNumber(L, nIndex);

				Lua_NewTable(L);
				int nAttrib = Lua_GetTopIndex(L);

				Lua_PushString(L, "nID");
				Lua_PushNumber(L, Attribute.nKey);
				Lua_SetTable(L, nAttrib);

				Lua_PushString(L, "nCount");
				Lua_PushNumber(L, i);
				Lua_SetTable(L, nAttrib);

				Lua_PushString(L, "nValue1");
				Lua_PushNumber(L, Attribute.nValue1);
				Lua_SetTable(L, nAttrib);

				Lua_PushString(L, "nValue2");
				Lua_PushNumber(L, Attribute.nValue2);
				Lua_SetTable(L, nAttrib);

				Lua_PushString(L, "bEquiped");
				int nHave = false;
				if (nEquipedNum >= i)
					nHave = true;
				Lua_PushBoolean(L, nHave);
				Lua_SetTable(L, nAttrib);

				Lua_SetTable(L, nTable);
			}
		}
	}

	nResult = 5;
Exit0:
	return nResult;
}

int	KItemInfo::LuaGetCoolDown(Lua_State* L)
{
	int     nCoolDown   = 0;
	KSkill* pSkill      = NULL;

	assert(L);

	pSkill = g_pSO3World->m_SkillManager.GetSkill_RAW(dwSkillID, dwSkillLevel);
	if (pSkill)
	{
		for (int i = 0; i < MAX_SKILL_COOL_DOWN_TIMER; i++)
		{
			DWORD dwID      = pSkill->m_dwCoolDownID[i];
			if (dwID == 0)
				continue;
			int nFrame = g_pSO3World->m_Settings.m_CoolDownList.GetCoolDownValue(dwID);
			if (nFrame > nCoolDown)
				nCoolDown = nFrame;
		}
	}

	int nFrame = g_pSO3World->m_Settings.m_CoolDownList.GetCoolDownValue(dwCoolDownID);
	if (nFrame > nCoolDown)
		nCoolDown = nFrame;

	Lua_PushNumber(L, nCoolDown);
	return 1;
}

#endif //_CLIENT

DEFINE_LUA_CLASS_BEGIN(KCustomEquipInfo)
	
	REGISTER_LUA_DWORD(KItemInfo, ID)
	REGISTER_LUA_INTEGER(KItemInfo, Genre)
	REGISTER_LUA_INTEGER(KItemInfo, Sub)
	REGISTER_LUA_INTEGER(KItemInfo, Detail)
	REGISTER_LUA_INTEGER(KItemInfo, Level)
	REGISTER_LUA_INTEGER(KItemInfo, Price)
	REGISTER_LUA_DWORD(KItemInfo, ScriptID)
	REGISTER_LUA_INTEGER(KItemInfo, MaxDurability)
	REGISTER_LUA_INTEGER(KItemInfo, MaxExistAmount)
    REGISTER_LUA_INTEGER(KItemInfo, MaxExistTime)
	REGISTER_LUA_BOOL(KItemInfo, CanTrade)
    REGISTER_LUA_BOOL(KItemInfo, CanDestroy)
    REGISTER_LUA_INTEGER(KItemInfo, Quality)
    REGISTER_LUA_STRING(KItemInfo, Name)
	REGISTER_LUA_DWORD(KItemInfo, CoolDownID)

#ifdef _CLIENT
	REGISTER_LUA_INTEGER(KItemInfo, UiId)
#endif
    
    REGISTER_LUA_INTEGER(KCustomEquipInfo, RepresentID)
	REGISTER_LUA_INTEGER(KCustomEquipInfo, BindType)
	REGISTER_LUA_INTEGER(KCustomEquipInfo, SetID)
    REGISTER_LUA_INTEGER(KCustomEquipInfo, AbradeRate)
    REGISTER_LUA_BOOL(KCustomEquipInfo, CanSetColor)
    REGISTER_LUA_INTEGER(KCustomEquipInfo, PackageGenerType)
    REGISTER_LUA_INTEGER(KCustomEquipInfo, PackageSubType)

#ifdef _CLIENT
	REGISTER_LUA_FUNC(KCustomEquipInfo, GetBaseAttrib)
	REGISTER_LUA_FUNC(KCustomEquipInfo, GetRequireAttrib)
	REGISTER_LUA_FUNC(KCustomEquipInfo, GetMagicAttrib)
	REGISTER_LUA_FUNC(KCustomEquipInfo, GetSetAttrib)
    REGISTER_LUA_FUNC(KItemInfo, GetCoolDown)
#endif

    REGISTER_LUA_BOOL_READONLY(KItemInfo, CanGoodCampUse)
    REGISTER_LUA_BOOL_READONLY(KItemInfo, CanEvilCampUse)
    REGISTER_LUA_BOOL_READONLY(KItemInfo, CanNeutralCampUse)

    REGISTER_LUA_DWORD(KItemInfo, RequireProfessionID)
    REGISTER_LUA_DWORD(KItemInfo, RequireProfessionBranch)
    REGISTER_LUA_INTEGER(KItemInfo,RequireProfessionLevel)
    REGISTER_LUA_INTEGER(KItemInfo, ExistType)

DEFINE_LUA_CLASS_END(KCustomEquipInfo)

DEFINE_LUA_CLASS_BEGIN(KOtherItemInfo)
	
	REGISTER_LUA_DWORD(KItemInfo, ID)
	REGISTER_LUA_INTEGER(KItemInfo, Genre)
	REGISTER_LUA_INTEGER(KItemInfo, Sub)
	REGISTER_LUA_INTEGER(KItemInfo, Detail)
	REGISTER_LUA_INTEGER(KItemInfo, Level)
	REGISTER_LUA_INTEGER(KItemInfo, Price)
	REGISTER_LUA_DWORD(KItemInfo, ScriptID)
	REGISTER_LUA_INTEGER(KItemInfo, MaxDurability)
	REGISTER_LUA_INTEGER(KItemInfo, MaxExistAmount)
    REGISTER_LUA_INTEGER(KItemInfo, MaxExistTime)
	REGISTER_LUA_BOOL(KItemInfo, CanTrade)
    REGISTER_LUA_BOOL(KItemInfo, CanDestroy)
    REGISTER_LUA_INTEGER(KItemInfo, Quality)
    REGISTER_LUA_STRING(KItemInfo, Name)
	REGISTER_LUA_DWORD(KItemInfo, CoolDownID)
#ifdef _CLIENT
	REGISTER_LUA_INTEGER(KItemInfo, UiId)
#endif
    
    REGISTER_LUA_INTEGER(KOtherItemInfo, BindType)
    REGISTER_LUA_BOOL(KOtherItemInfo, CanConsume)
    REGISTER_LUA_BOOL(KOtherItemInfo, CanStack)
    REGISTER_LUA_INTEGER(KOtherItemInfo, RequireLevel)
    REGISTER_LUA_INTEGER(KOtherItemInfo,RequireGender)
    REGISTER_LUA_BOOL(KOtherItemInfo, CanUseOnHorse)
    REGISTER_LUA_BOOL(KOtherItemInfo, CanUseInFight)
    REGISTER_LUA_DWORD(KOtherItemInfo, RequireForceID)

    REGISTER_LUA_BOOL_READONLY(KItemInfo, CanGoodCampUse)
    REGISTER_LUA_BOOL_READONLY(KItemInfo, CanEvilCampUse)
    REGISTER_LUA_BOOL_READONLY(KItemInfo, CanNeutralCampUse)
    
#ifdef _CLIENT
    REGISTER_LUA_FUNC(KItemInfo, GetCoolDown)
#endif

    REGISTER_LUA_DWORD(KItemInfo, RequireProfessionID)
    REGISTER_LUA_DWORD(KItemInfo, RequireProfessionBranch)
    REGISTER_LUA_INTEGER(KItemInfo, RequireProfessionLevel)
    REGISTER_LUA_INTEGER(KItemInfo, UseTargetType)
    REGISTER_LUA_INTEGER(KItemInfo, ExistType)
    REGISTER_LUA_INTEGER(KOtherItemInfo, Prefix)
    REGISTER_LUA_INTEGER(KOtherItemInfo, Postfix)
    REGISTER_LUA_DWORD(KOtherItemInfo, EnchantID)
    REGISTER_LUA_DWORD(KOtherItemInfo, BoxTemplateID)

DEFINE_LUA_CLASS_END(KOtherItemInfo)
