#ifndef _KITEM_INFO_LIST_H_
#define _KITEM_INFO_LIST_H_

#include "Global.h"
#include "Luna.h"

#define MAX_QUALITY_NUM		9
#define	MAX_ATTRIB_PARAM_NUM		2

struct KRange
{
	int					nMin;
	int					nMax;
};
struct KAttribRange : KRange
{
	int					nAttribID;
};

struct KAttribKey
{
	int					nAttribID;
	int					nValue;
};

struct KAttribInfo
{
	DWORD				dwID;
	int					nAttribID;
	int					nImportance;
	KRange				Param[MAX_ATTRIB_PARAM_NUM];
};

struct KItemInfo
{
	DWORD				dwID;
	int					nGenre;
	int					nSub;
	int					nDetail;
	int					nLevel;
	int					nPrice;
	DWORD				dwScriptID;
	int					nMaxDurability;
	int					nMaxExistAmount;
	int					nMaxExistTime;
	BOOL				bCanTrade;
	BOOL				bCanDestory;
	char				szName[ITEM_NAME_LEN];
#ifdef _CLIENT
	int					nUiId;
#endif
};

struct KEquipInfo : KItemInfo
{
	int					nAttribGroup;
	int					nRepresentId;
//	int					nAbradeRate;		// TODO:磨损机率，等具体随机装备制作时再搞吧
	int					nQualityRate[MAX_QUALITY_NUM];
	KAttribRange		BaseAttrib[MAX_ITEM_BASE_COUNT];
	KAttribKey			RequireAttrib[MAX_ITEM_REQUIRE_COUNT];
};

struct KCustomEquipInfo : KItemInfo
{
	int					nRepresentId;
	int					nQuality;
	int					nBindType;
	int					nSetID;
	int					nAbradeRate;
	KAttribRange		BaseAttrib[MAX_ITEM_BASE_COUNT];
	KAttribKey			RequireAttrib[MAX_ITEM_REQUIRE_COUNT];
	int					nAttribIndex[MAX_ITEM_MAGIC_COUNT];

	int					LuaGetBaseAttrib(Lua_State* L);
	int					LuaGetRequireAttrib(Lua_State* L);
	int					LuaGetMagicAttrib(Lua_State* L);

	DECLARE_LUA_CLASS(KCustomEquipInfo);

	DECLARE_LUA_STRUCT_INTEGER(RepresentID, nRepresentId);
	DECLARE_LUA_STRUCT_INTEGER(Quality, nQuality);
	DECLARE_LUA_STRUCT_INTEGER(BindType, nBindType);
	DECLARE_LUA_STRUCT_INTEGER(SetID, nSetID);

	DECLARE_LUA_STRUCT_DWORD(ID, dwID);
	DECLARE_LUA_STRUCT_INTEGER(Genre, nGenre);
	DECLARE_LUA_STRUCT_INTEGER(Sub, nSub);
	DECLARE_LUA_STRUCT_INTEGER(Detail, nDetail);
	DECLARE_LUA_STRUCT_INTEGER(Level, nLevel);
	DECLARE_LUA_STRUCT_INTEGER(Price, nPrice);
	DECLARE_LUA_STRUCT_DWORD(ScriptID, dwScriptID);
	DECLARE_LUA_STRUCT_INTEGER(MaxDurability, nMaxDurability);
	DECLARE_LUA_STRUCT_BOOL(CanTrade, bCanTrade);
	DECLARE_LUA_STRUCT_BOOL(CanDestory, bCanDestory);
	DECLARE_LUA_STRUCT_INTEGER(AbradeRate, nAbradeRate);
	DECLARE_LUA_STRUCT_STRING(Name, ITEM_NAME_LEN, szName);
#ifdef _CLIENT
	DECLARE_LUA_STRUCT_INTEGER(UiId, nUiId);
#endif


};

struct KOtherItemInfo : KItemInfo
{
	int					nBindType;
	DWORD				dwSkillID;
	DWORD				dwSkillLevel;
	BOOL				bCanConsume;
	BOOL				bCanStack;
};

template <class T>
class KItemInfoList
{
public:
	BOOL	Init(char* pszFile);
	BOOL	UnInit();
	T*		GetItemInfo(DWORD dwID);
	DWORD	GetItemCount();
private:
	typedef std::map<DWORD, T>	ITEM_INFO_LIST;
	ITEM_INFO_LIST				m_ItemInfoList;
};

template <class T>
BOOL KItemInfoList<T>::Init(char* pszFile)
{
	return TRUE;
}

template <class T>
BOOL KItemInfoList<T>::UnInit()
{
	m_ItemInfoList.clear();
Exit0:
	return TRUE;
}

template <class T>
T* KItemInfoList<T>::GetItemInfo(DWORD dwID)
{
	ITEM_INFO_LIST::iterator it;

	KG_PROCESS_ERROR(dwID < MAX_ITEM_COUNT);
	it = m_ItemInfoList.find(dwID);
	KG_PROCESS_ERROR(it != m_ItemInfoList.end());

	return &(it->second);
Exit0:
	return NULL;
}

template <class T>
DWORD KItemInfoList<T>::GetItemCount()
{
	return m_ItemInfoList.size();
}

#endif	//_KITEM_INFO_LIST_H_