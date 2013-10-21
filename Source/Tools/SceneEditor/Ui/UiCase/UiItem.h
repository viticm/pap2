/*****************************************************************************************
//	界面--道具界面
//	Copyright : Kingsoft 2002
//	Author	:   Wooy(Wu yue)
//	CreateTime:	2002-9-2
------------------------------------------------------------------------------------------
*****************************************************************************************/
#pragma once

#include "../elem/WndButton.h"
#include "../elem/WndText.h"
#include "../elem/WndObjContainer.h"
#include "../Elem/WndShowAnimate.h"

struct KUiObjAtRegion;

class KUiItem : public KWndShowAnimate
{
public:
	static KUiItem* OpenWindow();				//打开窗口，返回唯一的一个类对象实例
	static KUiItem* GetIfVisible();				//如果窗口正被显示，则返回实例指针
	static void		CloseWindow(bool bDestroy);	//关闭窗口，同时可以选则是否删除对象实例
	static void		LoadScheme(const char* pScheme);//载入界面方案
	static void		OnNpcTradeMode(bool bTrue);
	void			UpdateItem(KUiObjAtRegion* pItem, int bAdd);//物品变化更新
	void			Breathe();									//活动函数
	void			UpdateStallState(bool bIsStall);			//更新进入摆摊按钮状态

private:
	KUiItem();
	~KUiItem() {}
	void	Initialize();							//初始化
	int		WndProc(unsigned int uMsg, unsigned int uParam, int nParam);//窗口函数
	void	UpdateData();
	void	OnClickItem(KUiDraggedObject* pItem, bool bDoImmed);
	void	OnRepairItem(KUiDraggedObject* pItem);
	void	OnItemPickDrop(ITEM_PICKDROP_PLACE* pPickPos, ITEM_PICKDROP_PLACE* pDropPos);
	void	OnGetMoney(int nMoney);
	void	OnMarkPrice(KUiDraggedObject* pItem);//[wxb 2003-11-18]
private:
	static KUiItem*		m_pSelf;
private:
	int					m_nMoney;
	KWndText256			m_Money;
	KWndButton			m_GetMoneyBtn;
	KWndButton			m_CloseBtn;
	KWndButton	        m_MakeAdvBtn; //摆摊:卖方输入广告
	KWndButton	        m_MakeStallBtn;//摆摊:卖方开始摆摊
	KWndObjectMatrix	m_ItemBox;
	KWndButton			m_OpenStatusPadBtn;
	KWndButton			m_MarkPriceBtn;// flying add the context from now on

	//界面状态句柄
	int					m_nStatusHandle;
};