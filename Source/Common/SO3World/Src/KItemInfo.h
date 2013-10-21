////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KItemInfo.h
//  Version     : 1.0
//  Creator     : Zhao Chunfeng
//  Create Date : 2008-07-22 11:24:42
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _KITEM_INFO_H_
#define _KITEM_INFO_H_

#include "Global.h"
#include "Luna.h"

#define MAX_QUALITY_NUM			9
#define	MAX_ATTRIB_PARAM_NUM	2
#define MAX_SET_PART_NUM		13	
#define MAX_SET_ATTR_NUM		(MAX_SET_PART_NUM - 1)
#define MAX_BOX_ITEM_NUM        8
#define BOX_ITEM_GROUP_NUM      4
#define BOX_ITEM_RATE_TYPE_NUM  (MAX_BOX_ITEM_NUM / BOX_ITEM_GROUP_NUM)

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
    int                 nExistType;
	int					nMaxExistTime;  // 单位：秒
	BOOL				bCanTrade;
	BOOL				bCanDestroy;
	int					nQuality;
	DWORD				dwSkillID;
	DWORD				dwSkillLevel;
	char				szName[ITEM_NAME_LEN];
    DWORD				dwCoolDownID;
#ifdef _CLIENT
	int					nUiId;
#endif
    int                 nAucGenre; // 寄卖行的分类
    int                 nAucSub;
    int                 nRequireCamp;

    DWORD               dwRequireProfessionID;
    DWORD               dwRequireProfessionBranch;
    int                 nRequireProfessionLevel;
    int                 nUseTargetType;

    int getCanNeutralCampUse(){return nRequireCamp & (0x01 << cNeutral);}
    int getCanGoodCampUse(){return nRequireCamp & (0x01 << cGood);}
    int getCanEvilCampUse(){return nRequireCamp & (0x01 << cEvil);}

    DECLARE_LUA_STRUCT_DWORD(ID, dwID);
	DECLARE_LUA_STRUCT_INTEGER(Genre, nGenre);
	DECLARE_LUA_STRUCT_INTEGER(Sub, nSub);
	DECLARE_LUA_STRUCT_INTEGER(Detail, nDetail);
	DECLARE_LUA_STRUCT_INTEGER(Level, nLevel);
	DECLARE_LUA_STRUCT_INTEGER(Price, nPrice);
	DECLARE_LUA_STRUCT_DWORD(ScriptID, dwScriptID);
	DECLARE_LUA_STRUCT_INTEGER(MaxDurability, nMaxDurability);
	DECLARE_LUA_STRUCT_INTEGER(MaxExistAmount, nMaxExistAmount);
    DECLARE_LUA_STRUCT_INTEGER(MaxExistTime, nMaxExistTime);
	DECLARE_LUA_STRUCT_BOOL(CanTrade, bCanTrade);
	DECLARE_LUA_STRUCT_BOOL(CanDestroy, bCanDestroy);
    DECLARE_LUA_STRUCT_INTEGER(Quality, nQuality);
	DECLARE_LUA_STRUCT_STRING(Name, ITEM_NAME_LEN, szName);
    DECLARE_LUA_STRUCT_DWORD(CoolDownID, dwCoolDownID);
    
    DECLARE_LUA_STRUCT_DWORD(RequireProfessionID, dwRequireProfessionID);
    DECLARE_LUA_STRUCT_DWORD(RequireProfessionBranch, dwRequireProfessionBranch);
    DECLARE_LUA_STRUCT_INTEGER(RequireProfessionLevel, nRequireProfessionLevel);
    DECLARE_LUA_STRUCT_INTEGER(UseTargetType, nUseTargetType);
    DECLARE_LUA_STRUCT_INTEGER(ExistType, nExistType);

#ifdef _CLIENT
	DECLARE_LUA_STRUCT_INTEGER(UiId, nUiId);
#endif

#ifdef _CLIENT	
	int					LuaGetCoolDown(Lua_State* L);
#endif
};

struct KCustomEquipInfo : KItemInfo
{
	int					nRepresentID;
	int					nBindType;
	int					nSetID;
	int					nAbradeRate;
    int                 nColorID;
    BOOL                bCanSetColor;
    int                 nPackageGenerType; // 背包能装的道具的大类
    int                 nPackageSubType;   // 背包能装的道具的小类
	KAttribRange		BaseAttrib[MAX_ITEM_BASE_COUNT];
	KAttribKey			RequireAttrib[MAX_ITEM_REQUIRE_COUNT];
	int					nAttribIndex[MAX_ITEM_MAGIC_COUNT];

	DECLARE_LUA_CLASS(KCustomEquipInfo);

	DECLARE_LUA_STRUCT_INTEGER(RepresentID, nRepresentID);
	DECLARE_LUA_STRUCT_INTEGER(BindType, nBindType);
	DECLARE_LUA_STRUCT_INTEGER(SetID, nSetID);
   	DECLARE_LUA_STRUCT_INTEGER(AbradeRate, nAbradeRate);
    DECLARE_LUA_STRUCT_BOOL(CanSetColor, bCanSetColor);
    DECLARE_LUA_STRUCT_INTEGER(PackageGenerType, nPackageGenerType);
    DECLARE_LUA_STRUCT_INTEGER(PackageSubType, nPackageSubType);
#ifdef _CLIENT	
	int					LuaGetBaseAttrib(Lua_State* L);
	int					LuaGetRequireAttrib(Lua_State* L);
	int					LuaGetMagicAttrib(Lua_State* L);
	int					LuaGetSetAttrib(Lua_State* L);
	int					LuaGetCoolDown(Lua_State* L);
#endif
};

#ifndef	_SERVER		// 加载定制装备时增加一些信息，帮助界面表现
typedef struct tagSetItemList
{
	KCustomEquipInfo*	SetItemInfo[MAX_SET_PART_NUM];
} SetItemList;

typedef std::map<DWORD, SetItemList>	SET_INFO_LIST;
#endif

struct KOtherItemInfo : KItemInfo
{
	int					nBindType;
	BOOL				bCanConsume;
	BOOL				bCanStack;
    int                 nRequireLevel;
    int                 nRequireGender;
    BOOL                bCanUseOnHorse;
    BOOL                bCanUseInFight;
    DWORD               dwRequireForceID; // 需求门派
    int                 nPrefix;          // 称号前缀
    int                 nPostfix;         // 称号后缀
    DWORD               dwEnchantID;      // 附魔ID(包括道具镶嵌)
    DWORD               dwBoxTemplateID;  // 箱子的模板ID

    DECLARE_LUA_CLASS(KOtherItemInfo);

	DECLARE_LUA_STRUCT_INTEGER(BindType, nBindType);
    DECLARE_LUA_STRUCT_BOOL(CanConsume, bCanConsume);
    DECLARE_LUA_STRUCT_BOOL(CanStack, bCanStack);
    DECLARE_LUA_STRUCT_INTEGER(RequireLevel, nRequireLevel);
    DECLARE_LUA_STRUCT_INTEGER(RequireGender, nRequireGender);
    DECLARE_LUA_STRUCT_BOOL(CanUseOnHorse, bCanUseOnHorse);
    DECLARE_LUA_STRUCT_BOOL(CanUseInFight, bCanUseInFight);
    DECLARE_LUA_STRUCT_DWORD(RequireForceID, dwRequireForceID);
    DECLARE_LUA_STRUCT_INTEGER(Prefix, nPrefix);
    DECLARE_LUA_STRUCT_INTEGER(Postfix, nPostfix);
    DECLARE_LUA_STRUCT_DWORD(EnchantID, dwEnchantID);
    DECLARE_LUA_STRUCT_DWORD(BoxTemplateID, dwBoxTemplateID);
};

struct KSetInfo
{
	DWORD				dwID;
	DWORD				dwAttribID[MAX_SET_ATTR_NUM];
#ifdef _CLIENT
	DWORD				dwUiID;
	KCustomEquipInfo*	pEquipList[MAX_SET_PART_NUM];
#endif
	char				szName[ITEM_NAME_LEN];
};

enum KBOX_RATE_TYPE
{
    ebrtInvalid = 0,

    ebrtUniform,    // 统一概率
    ebrtIndependent // 独立概率
};

struct KBOX_ITEM 
{
    DWORD dwTabType;
    DWORD dwTabIndex;
    int   nStackNum;
    int   nRate;
};

struct KBOX_INFO 
{
    KBOX_ITEM   BoxItem[MAX_BOX_ITEM_NUM];
    int         nRateType[BOX_ITEM_RATE_TYPE_NUM];
};

#endif
