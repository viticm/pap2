/************************************************************************/
/* 场景装饰物模板列表					                                */
/* Copyright : Kingsoft 2004										    */
/* Author	 : Zhu Jianqiu												*/
/* History	 :															*/
/*		2005.04.25	Create												*/
/* Comment	 :															*/
/*		封装了对场景装饰物模板配置文件的读写和Cache						*/
/************************************************************************/
#ifndef _KDOODAD_TEMPLATE_H_
#define _KDOODAD_TEMPLATE_H_

#include "Luna.h"
#include "Global.h"

#include <map>

class KDoodad;

struct KDoodadTemplate
{
	DWORD						m_dwTemplateID;
	char						m_szName[_NAME_LEN];	
	char						m_szMapName[_NAME_LEN];	        // 所属地图
	BOOL						m_bSelectable;				    // 可否被角色点选
	int							m_nRemoveDelay;
	int							m_nReviveDelay;
	char						m_szDropName[MAX_DROP_PER_DOODAD][MAX_PATH];
	DWORD						m_dwDropCount[MAX_DROP_PER_DOODAD];
	DWORD						m_dwOpenPrepareFrame;
	DOODAD_KIND					m_eKind;						// 类型
	DWORD						m_dwClassID;				    // Doodad掉落分类,比如宝箱,草药,矿脉
	int							m_nLevel;					    // Doodad等级
	BOOL						m_bCanOperateEach;			    // 能否同时被多人操作
	BOOL						m_bCanPick;					    // 打开后是否进拾取流程
	DWORD						m_dwCraftID;				    // 能够操作的技艺,只有dkCraftTarget类型的Doodad可用
	DWORD						m_dwRepresentID;
    char						szScript[MAX_SCIRPT_NAME_LEN];
    int                         m_nOverLootFrame;
    BOOL                        m_bDynamicObstacle;             // 是否生成动态障碍

#if defined(_SERVER)
	int							m_nMinMoney;
	int							m_nMaxMoney;
	int							m_nMoneyRate;
#endif //_SERVER

#if defined(_CLIENT)
	char						m_szBarText[_NAME_LEN];
#endif //_CLIENT

#ifdef _SERVER
    int                         m_nProgressID;
#endif

	DECLARE_LUA_CLASS(KDoodadTemplate);

	DECLARE_LUA_DWORD(TemplateID);
	DECLARE_LUA_STRING(Name, _NAME_LEN);
	DECLARE_LUA_BOOL(Selectable);
	DECLARE_LUA_INTEGER(ReviveDelay);
	DECLARE_LUA_DWORD(OpenPrepareFrame);
	DECLARE_LUA_ENUM(Kind);
	DECLARE_LUA_DWORD(ClassID);
	DECLARE_LUA_INTEGER(Level);
	DECLARE_LUA_BOOL(CanOperateEach);
	DECLARE_LUA_DWORD(CraftID);
	DECLARE_LUA_DWORD(RepresentID);
#if defined(_CLIENT)
	DECLARE_LUA_STRING(BarText, _NAME_LEN);
#endif 
};

class KDoodadTemplateList
{
public:
	KDoodadTemplateList();
	~KDoodadTemplateList();

	BOOL Init();
	void UnInit();

	BOOL LoadFromTemplate(DWORD dwTemplateID, KDoodad* pDoodad);	

	KDoodadTemplate* GetTemplate(DWORD dwTemplateID);
	KDoodadTemplate* GetTemplateByIndex(int nIndex);

	int GetDoodadTemplateCount();

private:
	typedef std::map<DWORD, KDoodadTemplate*> KDOODAD_TEMPLATE_LIST;
	KDOODAD_TEMPLATE_LIST	m_mapDoodadTemplateList;
	KDoodadTemplate			m_DefaultDoodadTemplate;
	KDoodadTemplate*		m_pDoodadTemplateList;
	int						m_nDoodadTemplateCount;

	BOOL LoadDoodadTemplate(int nIndex, ITabFile* piTabFile, KDoodadTemplate& fDoodadTemplate);
};

#endif	//_KDOODAD_TEMPLATE_H_
