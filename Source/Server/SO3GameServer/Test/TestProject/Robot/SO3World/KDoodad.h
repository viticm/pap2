/************************************************************************/
/* 场景装饰物	                                                        */
/* Copyright : Kingsoft 2005										    */
/* Author	 : Zhu Jianqiu												*/
/* History	 :															*/
/*		2004.12.31	Create												*/
/* Comment	 :															*/
/*		轻量级的场景物体对象											*/
/************************************************************************/
#ifndef _KDOODAD_H_
#define	_KDOODAD_H_

#include "Luna.h"
#include "KSceneObject.h"
#include "KDoodad.h"
//#include "KDoodadTemplateList.h"

#define MAX_DOODAD_OBSTACLE 64

class KLootList;

enum DOODAD_STATE
{
	dsInvalid = 0,				//无效状态

	//受控状态
	dsIdle,						//正常状态
	dsBeHit,					//被操作或被击
	dsAfterHit,					//打开后或被击死亡

	//死亡状态
	dsRemove,

	dsTotal
};

class KCharacter;

class KDoodad : public KSceneObject
{
//场景装饰物的属性
public:
	DWORD					m_dwTemplateID;					//模板编号

	char					m_szName[_NAME_LEN];			//名称
	DOODAD_KIND				m_eKind;						//类型
	int						m_nMaxLife;						//最大生命值
	int						m_nOpenFrames;					//打开帧数
	int						m_nDisappearFrames;				//消失帧数
	int						m_nCurrentLife;					//当前生命值
	DOODAD_STATE			m_eState;						//状态
	DWORD					m_dwNpcTemplateID;
	DWORD					m_dwRepresentID;				//Doodad的表现ID
	BOOL					m_bLooted;						//尸体是否被拾取完的标记

	struct KDoodadInfo
	{
		unsigned		m_bSetBlockCharacter:	1;
		unsigned		m_bBlockCharacter	:	1;			//障碍信息
		unsigned		m_bSetPutObj		:	1;
		unsigned		m_bPutObj			:	1;			//可摆放
		unsigned		m_bSetSafe			:	1;
		unsigned		m_bSafe				:	1;			//是否是安全区
		unsigned		m_bSetStall			:	1;
		unsigned		m_bStall			:	1;			//是否可摆摊
		unsigned		m_bSetCellType		:	1;
		unsigned		m_CellType			:	4;			//地表类型
		unsigned		m_bSetPassWidth		:	1;
		unsigned		m_PassWidth			:	3;			//通过物限制
		unsigned		m_Reserved			:	15;			//未用
	}m_DoodadInfo;

	DWORD					m_dwScriptID;					//绑定脚本ID
	//KLootList*				m_pLootList;					//拾取列表

	KCell*					m_PObstacleCell[MAX_DOODAD_OBSTACLE];	//保存相关的Cell障碍,这个指针不是指向障碍cell，而是指向障碍的前一个cell

public:
	KDoodad();
	~KDoodad();

	BOOL Init(void);
	BOOL UnInit(void);

	BOOL Activate(void);

	BOOL Save(KDOODAD_DATA* pDoodadData);
	BOOL Load(const KDOODAD_DATA& crDoodadData);

	//----- Doodad带障碍 ---------------------------------------->
	BOOL ApplyObstacle();
	BOOL RemoveObstacle();
	BOOL UpdateObstacle();
	//<-----------------------------------------------------------

	//----- Doodad状态 ------------------------------------------>
	void ChangeState(DOODAD_STATE eState);
private:
	void DoIdle();
	void DoBeHit();
	void DoAfterHit();

public:
	//<-----------------------------------------------------------

	//应用物件上的特殊效果，每个Cell上只有第一个Doodad可以起作用
	void ApplyEffect(void);
	//反应用物件上的特殊效果
	void UnApplyEffect(void);

	void DoPick(KCharacter* pCharacter);

public:
	//Lua脚本接口
	DECLARE_LUA_CLASS(KDoodad);

	int LuaGetLootMode(Lua_State* L);

	int LuaGetLootItem(Lua_State* L);
	int LuaGetLootMoney(Lua_State* L);
	int LuaCanDialog(Lua_State* L);
	int LuaIsSelectable(Lua_State* L);


	int LuaLeaderDistributeItem(Lua_State* L);

	int LuaGetLooterList(Lua_State* L);


	DECLARE_LUA_DWORD(TemplateID);
	DECLARE_LUA_STRING(Name, _NAME_LEN);
	DECLARE_LUA_ENUM(Kind);
	DECLARE_LUA_INTEGER(MaxLife);
	DECLARE_LUA_INTEGER(OpenFrames);
	DECLARE_LUA_INTEGER(DisappearFrames);
	DECLARE_LUA_DWORD(RepresentID);
	DECLARE_LUA_DWORD(NpcTemplateID);

	DECLARE_LUA_INTEGER(CurrentLife);
	//DECLARE_LUA_DWORD(ClassID);

	DECLARE_LUA_DWORD(ScriptID);

	DECLARE_LUA_STRUCT_INTEGER(IsSetBlockCharacter, m_DoodadInfo.m_bSetBlockCharacter);
	DECLARE_LUA_STRUCT_INTEGER(IsBlockCharacter,	m_DoodadInfo.m_bBlockCharacter);
	DECLARE_LUA_STRUCT_INTEGER(IsSetPutObj,			m_DoodadInfo.m_bSetPutObj);
	DECLARE_LUA_STRUCT_INTEGER(IsPutObj,			m_DoodadInfo.m_bPutObj);
	DECLARE_LUA_STRUCT_INTEGER(IsSetSafe,			m_DoodadInfo.m_bSetSafe);
	DECLARE_LUA_STRUCT_INTEGER(IsSafe,				m_DoodadInfo.m_bSafe);
	DECLARE_LUA_STRUCT_INTEGER(IsSetStall,			m_DoodadInfo.m_bSetStall);
	DECLARE_LUA_STRUCT_INTEGER(IsStall,				m_DoodadInfo.m_bStall);
	DECLARE_LUA_STRUCT_INTEGER(IsSetCellType,		m_DoodadInfo.m_bSetCellType);
	DECLARE_LUA_STRUCT_INTEGER(CellType,			m_DoodadInfo.m_CellType);
	DECLARE_LUA_STRUCT_INTEGER(IsSetPassWidth,		m_DoodadInfo.m_bSetPassWidth);
	DECLARE_LUA_STRUCT_INTEGER(PassWidth,			m_DoodadInfo.m_PassWidth);


public:
	static struct KUpdateAllDoodadFunc UpdateAllDoodadFunc;
};

struct KUpdateAllDoodadFunc
{
	KUpdateAllDoodadFunc(){}

	BOOL operator()(KDoodad* pDoodad)
	{
		BOOL bRetCode = FALSE;

		bRetCode = pDoodad->UpdateObstacle();
		KGLOG_CHECK_ERROR(bRetCode);

		return TRUE;
	}
};

#endif