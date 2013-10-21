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

int KItem::LuaGetPermanentEnchantAttrib(Lua_State* L)
{
	int nFirst = true;
    int nResult = 0;
	int nIndex = 1;

	KGLOG_PROCESS_ERROR(Lua_GetTopIndex(L) == 0);
	KG_PROCESS_ERROR(m_pPermanentEnchant);
    KG_PROCESS_ERROR(m_pPermanentEnchant->pAttribute);
	for (KAttribute* pAttribute = m_pPermanentEnchant->pAttribute; pAttribute; pAttribute = pAttribute->pNext)
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

int KItem::LuaGetTemporaryEnchantAttrib(Lua_State* L)
{
	int nFirst = true;
    int nResult = 0;
	int nIndex  = 1;

	KGLOG_PROCESS_ERROR(Lua_GetTopIndex(L) == 0);
	KG_PROCESS_ERROR(m_pTemporaryEnchant);
    KG_PROCESS_ERROR(m_pTemporaryEnchant->pAttribute);
	for (KAttribute* pAttribute = m_pTemporaryEnchant->pAttribute; pAttribute; pAttribute = pAttribute->pNext)
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

int	KItem::LuaGetSetAttrib(Lua_State* L)
{
	int         nResult         = 0;
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
	REGISTER_LUA_DWORD(KItem, TabType)
	REGISTER_LUA_DWORD(KItem, Index)
	REGISTER_LUA_INTEGER(KItem, MaxExistAmount)
	REGISTER_LUA_INTEGER(KItem, Quality)

	REGISTER_LUA_INTEGER(KItem, RepresentID)
	REGISTER_LUA_INTEGER_READONLY(KItem, ColorID)
#if defined(_CLIENT)
	REGISTER_LUA_INTEGER(KItem, UiId)

	REGISTER_LUA_FUNC(KItem, GetBaseAttrib)
	REGISTER_LUA_FUNC(KItem, GetRequireAttrib)
	REGISTER_LUA_FUNC(KItem, GetMagicAttrib)
	REGISTER_LUA_FUNC(KItem, GetPermanentEnchantAttrib)
	REGISTER_LUA_FUNC(KItem, GetTemporaryEnchantAttrib)
	REGISTER_LUA_FUNC(KItem, GetSetAttrib)
	REGISTER_LUA_FUNC(KItem, CanEquip)
#endif	//_CLIENT
	REGISTER_LUA_FUNC(KItem, IsRepairable)

	REGISTER_LUA_FUNC(KItem, GetUseSkill)
	REGISTER_LUA_FUNC(KItem, HasScript)
	REGISTER_LUA_FUNC(KItem, GetRequireLevel)
DEFINE_LUA_CLASS_END(KItem)
