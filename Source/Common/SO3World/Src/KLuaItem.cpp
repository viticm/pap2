#include "stdafx.h"
#include "KSkill.h"
#include "KItem.h"
#include "KItemInfoList.h"
#include "KSO3World.h"
#include "KItemLib.h"
#include "KPlayer.h"

#if defined _CLIENT
int KItem::LuaGetBaseAttrib(Lua_State* L)
{
	int nRelIndex = 0;

	assert(L);

	Lua_NewTable(L);
	int nTable = Lua_GetTopIndex(L);

	for (KAttribute* pAttribute = m_pBaseAttr; pAttribute; pAttribute = pAttribute->pNext)
	{
		if (pAttribute->nKey != atInvalid)
		{
			if (pAttribute->nKey == atMeleeWeaponDamageRand || pAttribute->nKey == atRangeWeaponDamageRand)
				continue;

			++nRelIndex;

			Lua_PushNumber(L, nRelIndex);

			Lua_NewTable(L);
			int nAttrib = Lua_GetTopIndex(L);

			Lua_PushString(L, "nID");
			Lua_PushNumber(L, pAttribute->nKey);
			Lua_SetTable(L, nAttrib);

			Lua_PushString(L, "nValue1");
			Lua_PushNumber(L, pAttribute->nValue1);
			Lua_SetTable(L, nAttrib);

			int nValue2 = pAttribute->nValue2;
			if (pAttribute->nKey == atMeleeWeaponDamageBase)
			{
				nValue2 = pAttribute->nValue1;
				for (KAttribute* pAttributeRand = m_pBaseAttr; 
					pAttributeRand; pAttributeRand = pAttributeRand->pNext)
				{
					if (pAttributeRand->nKey == atMeleeWeaponDamageRand)
					{
						nValue2 += pAttributeRand->nValue1;
					}
				}
			}

            if (pAttribute->nKey == atRangeWeaponDamageBase)
            {
                nValue2 = pAttribute->nValue1;
                for (KAttribute* pAttributeRand = m_pBaseAttr; 
                    pAttributeRand; pAttributeRand = pAttributeRand->pNext)
                {
                    if (pAttributeRand->nKey == atRangeWeaponDamageRand)
                    {
                        nValue2 += pAttributeRand->nValue1;
                    }
                }
           }

			Lua_PushString(L, "nValue2");
			Lua_PushNumber(L, nValue2 );
			Lua_SetTable(L, nAttrib);

			Lua_SetTable(L, nTable);
		}
	}

	return 1;
}

int KItem::LuaGetRequireAttrib(Lua_State* L)
{
	int nRelIndex = 0;

	assert(L);

	Lua_NewTable(L);
	int nTable = Lua_GetTopIndex(L);
	for (KAttribute* pAttribute = m_pRequireAttr; pAttribute; pAttribute = pAttribute->pNext)
	{
		if (pAttribute->nKey != atInvalid)
		{
			++nRelIndex;

			Lua_PushNumber(L, nRelIndex);

			Lua_NewTable(L);
			int nAttrib = Lua_GetTopIndex(L);

			Lua_PushString(L, "nID");
			Lua_PushNumber(L, pAttribute->nKey);
			Lua_SetTable(L, nAttrib);

			Lua_PushString(L, "nValue1");
			Lua_PushNumber(L, pAttribute->nValue1);
			Lua_SetTable(L, nAttrib);

			Lua_PushString(L, "nValue2");
			Lua_PushNumber(L, pAttribute->nValue2);
			Lua_SetTable(L, nAttrib);

			Lua_SetTable(L, nTable);
		}
	}

	return 1;
}

int KItem::LuaGetMagicAttrib(Lua_State* L)
{
	int nRelIndex = 0;

	assert(L);

	Lua_NewTable(L);
	int nTable = Lua_GetTopIndex(L);
	for (KAttribute* pAttribute = m_pMagicAttr; pAttribute; pAttribute = pAttribute->pNext)
	{
		if (pAttribute->nKey != atInvalid)
		{
			++nRelIndex;

			Lua_PushNumber(L, nRelIndex);

			Lua_NewTable(L);
			int nAttrib = Lua_GetTopIndex(L);

			Lua_PushString(L, "nID");
			Lua_PushNumber(L, pAttribute->nKey);
			Lua_SetTable(L, nAttrib);

			Lua_PushString(L, "nValue1");
			Lua_PushNumber(L, pAttribute->nValue1);
			Lua_SetTable(L, nAttrib);

			Lua_PushString(L, "nValue2");
			Lua_PushNumber(L, pAttribute->nValue2);
			Lua_SetTable(L, nAttrib);

			Lua_SetTable(L, nTable);
		}
	}

	return 1;
}

int GetEnchantAttrib(Lua_State* L, DWORD dwEnchantID)
{
    int nFirst = true;
    int nResult = 0;
    int nIndex = 1;
    KENCHANT* pEnchant = NULL;

    pEnchant = g_pSO3World->m_ItemManager.m_EnchantLib.GetEnchantInfo(dwEnchantID);
    KGLOG_PROCESS_ERROR(pEnchant);

    KG_PROCESS_ERROR(pEnchant->pAttribute);
    for (KAttribute* pAttribute = pEnchant->pAttribute; pAttribute; pAttribute = pAttribute->pNext)
    {
        if (pAttribute->nKey != atInvalid)
        {
            if (nFirst)
            {
                Lua_NewTable(L);
                nFirst = false;
            }

            Lua_PushNumber(L, nIndex);
            nIndex++;
            Lua_NewTable(L);

            Lua_PushString(L, "nID");
            Lua_PushNumber(L, pAttribute->nKey);
            Lua_SetTable(L, -3);

            Lua_PushString(L, "nValue1");
            Lua_PushNumber(L, pAttribute->nValue1);
            Lua_SetTable(L, -3);

            Lua_PushString(L, "nValue2");
            Lua_PushNumber(L, pAttribute->nValue2);
            Lua_SetTable(L, -3);

            Lua_SetTable(L, -3);
        }
    }

    if (!nFirst)
        nResult = 1;
Exit0:
    return nResult;
}

int KItem::LuaGetPermanentEnchantAttrib(Lua_State* L)
{
    int nResult = 0;
	KGLOG_PROCESS_ERROR(Lua_GetTopIndex(L) == 0);
	KG_PROCESS_ERROR(m_dwEnchantID[eiPermanentEnchant]);

    nResult = GetEnchantAttrib(L, m_dwEnchantID[eiPermanentEnchant]);
Exit0:
    return nResult;
}

int KItem::LuaGetTemporaryEnchantAttrib(Lua_State* L)
{
    int nResult = 0;
    KGLOG_PROCESS_ERROR(Lua_GetTopIndex(L) == 0);
    KG_PROCESS_ERROR(m_dwEnchantID[eiTemporaryEnchant]);

    nResult = GetEnchantAttrib(L, m_dwEnchantID[eiTemporaryEnchant]);
Exit0:
    return nResult;
}

int KItem::LuaGetMountAttrib(Lua_State* L)
{
    int nResult = 0;
    int nMountIndex = 0;
    KGLOG_PROCESS_ERROR(Lua_GetTopIndex(L) == 1);
    
    nMountIndex = (int)Lua_ValueToNumber(L, 1);
    KGLOG_PROCESS_ERROR(nMountIndex >= eiMount1 && nMountIndex <= eiMount4);

    KG_PROCESS_ERROR(m_dwEnchantID[nMountIndex]);

    nResult = GetEnchantAttrib(L, m_dwEnchantID[nMountIndex]);
Exit0:
    return nResult;
}

int	KItem::LuaGetSetAttrib(Lua_State* L)
{
	int         nResult         = 0;
	int         nSetEquipCount  = 0;
	int         nIndex          = 0;
	int         nEquipedNum     = 0;
    KSetInfo*   pSetInfo        = NULL;
    KPlayer*    pPlayer         = NULL;

	KG_PROCESS_ERROR(m_Common.dwSetID > 0);

	pSetInfo = g_pSO3World->m_ItemManager.m_ItemLib.GetSetInfo(m_Common.dwSetID);
	KGLOG_PROCESS_ERROR(pSetInfo);

	pPlayer = g_pSO3World->m_PlayerSet.GetObj(g_pSO3World->m_dwClientPlayerID);
	KGLOG_PROCESS_ERROR(pPlayer);

	Lua_PushNumber(L, pSetInfo->dwUiID);

	Lua_NewTable(L);
	int nTable = Lua_GetTopIndex(L);

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
		if (g_pSO3World->m_ItemManager.GetSetAttrib(Attribute, m_Common.dwSetID, i))
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

int KItem::LuaCanEquip(Lua_State* L)
{
	assert(L);

	int nCanEquipPos = -1;
	int nCanEquip = false;

	KPlayer* pPlayer = g_pSO3World->m_PlayerSet.GetObj(g_pSO3World->m_dwClientPlayerID);
	KGLOG_PROCESS_ERROR(pPlayer);

	KG_PROCESS_ERROR(getGenre() == igEquipment);

	for (int i = 0; i < eitTotal; ++i)
	{
		if (pPlayer->m_ItemList.CanEquip(this, i) == ircSuccess)
		{
			nCanEquipPos = i;
			if (i == eitLeftRing)
				if (pPlayer->m_ItemList.GetItem(igEquipment, eitLeftRing))
					if (pPlayer->m_ItemList.CanEquip(this, eitRightRing) == ircSuccess)
						nCanEquipPos = eitRightRing;
			goto Exit1;
		}
	}
	goto Exit0;

Exit1:
	nCanEquip = true;
Exit0:
	Lua_PushBoolean(L, nCanEquip);
	Lua_PushNumber(L, nCanEquipPos);
	return 2;
}

int KItem::LuaGetMountItemUIID(Lua_State* L)
{
    DWORD dwResult = 0;
    int   nMountIndex = 0;
    KENCHANT* pEnchant = NULL;

    KGLOG_PROCESS_ERROR(Lua_GetTopIndex(L) == 1);

    nMountIndex = (int)Lua_ValueToNumber(L, 1);

    KGLOG_PROCESS_ERROR(nMountIndex >= eiMount1 && nMountIndex <= eiMount4);

    KG_PROCESS_ERROR(m_dwEnchantID[nMountIndex] > 0);

    pEnchant = g_pSO3World->m_ItemManager.m_EnchantLib.GetEnchantInfo(m_dwEnchantID[nMountIndex]);
    KGLOG_PROCESS_ERROR(pEnchant);
    
    dwResult = pEnchant->dwUIID;
Exit0:
    Lua_PushNumber(L, dwResult);
    return 1;
}

int KItem::LuaIsDestroyOnMount(Lua_State* L)
{
    BOOL bResult = false;
    KOtherItemInfo* pItemInfo = NULL;
    KENCHANT* pEnchant = NULL;
    int nTopIndex = Lua_GetTopIndex(L);
    int nMountIndex = -1;
    DWORD dwEnchantID = 0;
    
    KGLOG_PROCESS_ERROR(nTopIndex == 0 || nTopIndex == 1);
    
    if (nTopIndex == 0)
    {
        KG_PROCESS_ERROR(m_Common.nGenre == igMountItem);
        KG_PROCESS_ERROR(m_GenParam.dwTabType == ittOther);

        pItemInfo = (KOtherItemInfo*)g_pSO3World->m_ItemManager.GetItemInfo(m_GenParam.dwTabType, m_GenParam.dwIndex);
        KGLOG_PROCESS_ERROR(pItemInfo);
        KG_PROCESS_ERROR(pItemInfo->dwEnchantID);

        dwEnchantID = pItemInfo->dwEnchantID;
    }
    else
    {
        nMountIndex = (int)Lua_ValueToNumber(L, 1);
        KGLOG_PROCESS_ERROR(nMountIndex >= eiMount1 && nMountIndex <= eiMount4);
        dwEnchantID = m_dwEnchantID[nMountIndex];
    }

    pEnchant = g_pSO3World->m_ItemManager.m_EnchantLib.GetEnchantInfo(dwEnchantID);
    KGLOG_PROCESS_ERROR(pEnchant);
    KG_PROCESS_ERROR(pEnchant->dwTabType == 0 || pEnchant->dwTabIndex == 0);

    bResult = true;
Exit0:
    Lua_PushBoolean(L, bResult);
    return 1;
}
#endif //_CLIENT

int KItem::LuaIsRepairable(Lua_State* L)
{
    BOOL bResult  = false;
	BOOL bRetCode = false;
    
	KGLOG_PROCESS_ERROR(Lua_GetTopIndex(L) == 0);

	bRetCode = IsRepairable();
    KG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
	Lua_PushBoolean(L, bResult);
	return 1;
}

int KItem::LuaGetUseSkill(Lua_State* L)
{
	KSkill* pSkill = NULL;

	KG_PROCESS_ERROR(m_dwSkillID);
		
	pSkill = g_pSO3World->m_SkillManager.GetSkill_RAW(m_dwSkillID, m_dwSkillLevel);
	KG_PROCESS_ERROR(pSkill);

	return pSkill->LuaGetObj(L);
Exit0:
	return 0;
}

int KItem::LuaHasScript(Lua_State* L)
{
	BOOL bResult = false;

	bResult = (m_dwScriptID != 0);
	Lua_PushBoolean(L, bResult);
	return 1;
}

int KItem::LuaGetRequireLevel(Lua_State* L)
{
    int nResult = 0;

    nResult = GetRequireLevel();
    Lua_PushNumber(L, nResult);
    return 1;
}

int KItem::LuaGetTemporaryEnchantLeftSeconds(Lua_State* L)
{
    time_t nLeftSecond = 0;
    
    if (m_nEnchantTime > g_pSO3World->m_nCurrentTime)
    {
        nLeftSecond = m_nEnchantTime - g_pSO3World->m_nCurrentTime;
    }
    
    Lua_PushNumber(L, nLeftSecond);
    return 1;
}

int KItem::LuaGetMountIndex(Lua_State* L)
{
    int nResult = GetMountIndex();
    Lua_PushNumber(L, nResult);
    return 1;
}

int KItem::LuaGetLeftExistTime(Lua_State* L)
{
    int nLeftTime  = 0;
    int nExistTime = (int)(g_pSO3World->m_nCurrentTime - m_GenParam.nGenTime);
    
    assert(m_pItemInfo);
    KG_PROCESS_ERROR(m_pItemInfo->nExistType == ketOnLine || m_pItemInfo->nExistType == ketOnlineAndOffLine);

    if (m_pItemInfo->nMaxExistTime > nExistTime)
    {
        nLeftTime = m_pItemInfo->nMaxExistTime - nExistTime;
    }

Exit0:
    Lua_PushNumber(L, nLeftTime);
    return 1;
}

DEFINE_LUA_CLASS_BEGIN(KItem)
	REGISTER_LUA_DWORD_READONLY(KBaseObject, ID)

	REGISTER_LUA_INTEGER(KItem, CurrentDurability)
	REGISTER_LUA_BOOL(KItem, CanStack)
	REGISTER_LUA_INTEGER(KItem, StackNum)
	REGISTER_LUA_INTEGER(KItem, MaxDurability)
	REGISTER_LUA_INTEGER(KItem, MaxStackNum)

	REGISTER_LUA_BOOL(KItem, Bind)
	REGISTER_LUA_STRING_READONLY(KItem, Name)
	REGISTER_LUA_INTEGER(KItem, Level)
	REGISTER_LUA_INTEGER(KItem, Price)
	REGISTER_LUA_INTEGER(KItem, Genre)
	REGISTER_LUA_INTEGER(KItem, Sub)
	REGISTER_LUA_INTEGER(KItem, Detail)
	REGISTER_LUA_INTEGER(KItem, BindType)
	REGISTER_LUA_BOOL(KItem, CanTrade)
	REGISTER_LUA_BOOL(KItem, CanDestroy)
	REGISTER_LUA_DWORD(KItem, SetID)
	REGISTER_LUA_DWORD(KItem, TabType)
	REGISTER_LUA_DWORD(KItem, Index)
    REGISTER_LUA_TIME_READONLY(KItem, GenTime)
	REGISTER_LUA_INTEGER(KItem, MaxExistAmount)
	REGISTER_LUA_INTEGER(KItem, Quality)

	REGISTER_LUA_INTEGER(KItem, Version)

	REGISTER_LUA_INTEGER(KItem, RepresentID)
    REGISTER_LUA_INTEGER_READONLY(KItem, ColorID)
#if defined(_CLIENT)
	REGISTER_LUA_INTEGER(KItem, UiId)

	REGISTER_LUA_FUNC(KItem, GetBaseAttrib)
	REGISTER_LUA_FUNC(KItem, GetRequireAttrib)
	REGISTER_LUA_FUNC(KItem, GetMagicAttrib)
	REGISTER_LUA_FUNC(KItem, GetPermanentEnchantAttrib)
	REGISTER_LUA_FUNC(KItem, GetTemporaryEnchantAttrib)
    REGISTER_LUA_FUNC(KItem, GetMountAttrib)
	REGISTER_LUA_FUNC(KItem, GetSetAttrib)
	REGISTER_LUA_FUNC(KItem, CanEquip)
    REGISTER_LUA_FUNC(KItem, GetMountItemUIID)
    REGISTER_LUA_FUNC(KItem, IsDestroyOnMount)
#endif	//_CLIENT
    REGISTER_LUA_FUNC(KItem, IsRepairable)

	REGISTER_LUA_FUNC(KItem, GetUseSkill)
	REGISTER_LUA_FUNC(KItem, HasScript)
    REGISTER_LUA_FUNC(KItem, GetRequireLevel)
    REGISTER_LUA_FUNC(KItem, GetTemporaryEnchantLeftSeconds)
    REGISTER_LUA_FUNC(KItem, GetMountIndex)
    REGISTER_LUA_FUNC(KItem, GetLeftExistTime)
DEFINE_LUA_CLASS_END(KItem)
