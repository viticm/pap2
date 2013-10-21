/*****************************************************************************************
//	界面--状态界面
//	Copyright : Kingsoft 2002
//	Author	:   Wooy(Wu yue)
//	CreateTime:	2002-9-2
------------------------------------------------------------------------------------------
	共有11个装备摆放位置，武器2×4，衣服2×3，头部2×2，腰带2×1，
	手镯1×1，鞋子2×2，戒指1×1，腰坠1×2，项链1×1，马2×3。
*****************************************************************************************/
#pragma once

#include "../Elem/WndButton.h"
#include "../Elem/WndText.h"
#include "../Elem/WndShowAnimate.h"
#include "../elem/WndObjContainer.h"
#include "../../../core/src/gamedatadef.h"

enum UI_PLAYER_ATTRIBUTE;
struct KUiObjAtRegion;

#define _ITEM_COUNT 11

class KUiParadeItem : protected KWndShowAnimate
{
public:
	//----界面面板统一的接口函数----
	static KUiParadeItem*	OpenWindow(KUiPlayerItem* pDest);					//打开窗口，返回唯一的一个类对象实例
	static KUiParadeItem*	GetIfVisible();					//如果窗口正被显示，则返回实例指针
	static void			CloseWindow(bool bDestroy);		//关闭窗口，同时可以选则是否删除对象实例
	static void			LoadScheme(const char* pScheme);//载入界面方案

	void	UpdateData(KUiPlayerItem* pDest);

private:
	KUiParadeItem() {}
	~KUiParadeItem() {}
	void	Initialize();								//初始化	
	int		WndProc(unsigned int uMsg, unsigned int uParam, int nParam);	//窗口函数
	void	LoadScheme(class KIniFile* pIni);			//载入界面方案
	void	UpdateBaseData(KUiPlayerItem* pDest);							//更新基本数据（人名等不易变数据）
	void	UpdateAllEquips(KUiPlayerItem* pDest);
	void	UpdateEquip(KUiObjAtRegion* pEquip, int bAdd);	//装备变化更新
private:
	static KUiParadeItem*	m_pSelf;

private:
	KWndImage	m_Face;
	KWndText32	m_Name;
	KWndText32	m_Title;
	KWndText32  m_PKValue;
	KWndText32	m_MateName;
	KWndText32	m_WorldRank;

	KWndButton	m_Close;

	//装备物品
	KWndObjectBox	m_EquipBox[_ITEM_COUNT];

	KUiPlayerItem m_Dest;
};
