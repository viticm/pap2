// -------------------------------------------------------------------------
//	文件名		：	UiTrade.h
//	创建者		：	Wooy(Wu yue)
//	创建时间	：	2002-12-21
//	功能描述	：	npc交易系统界面
// -------------------------------------------------------------------------
#pragma once

#include "../Elem/WndShowAnimate.h"
#include "../elem/wndbutton.h"
#include "../Elem/WndText.h"
#include "../elem/wndObjContainer.h"
#include "../../Engine/Src/LinkStruct.h"
#include "GameDataDef.h"

struct KUiObjAtContRegion;

struct KObjForBuy : public KUiObjAtContRegion
{
	int         nCount;
	KObjForBuy *next;
	KObjForBuy *previous;
};

class KUiShop : protected KWndShowAnimate
{
public:
	//----界面面板统一的接口函数----
	static KUiShop*		OpenWindow();				//打开窗口，返回唯一的一个类对象实例
	static KUiShop*		GetIfVisible();				//如果窗口正被显示，则返回实例指针
	static void			CloseWindow();				//关闭窗口，同时可以选则是否删除对象实例
	static void			LoadScheme(const char* pScheme);	//载入界面方案
	static void			CancelTrade();
	void				UpdateData();	
	int		            WndProc(unsigned int uMsg, unsigned int uParam, int nParam);//窗口函数
	void                PushBuyAttempt(KObjForBuy *pObj);   //把一次购买的意图入队
	void                ABuyFinish();                       //当买进一个物品之后，外部调用这个通知Shop界面
	void                ClearBuyList();                     //清空掉购买队列
private:
	KUiShop();
	~KUiShop() {}
	void	SetPage(int nIndex);
	void	Clear();
	void	OnClickButton(KWndButton* pWnd, int bCheck);
	void	OnBuyItem(KUiDraggedObject* pItem, bool bDoImmed);
	void	Initialize();						//初始化

	void    Buy(KObjForBuy *pObj);              //购买一个物品，并且把等待数量减一
private:
	static KUiShop*		m_pSelf;

private:
	KUiObjAtContRegion*	m_pObjsList;

	KLinkStruct<KObjForBuy>             //购买物品的队列
		                m_BuyList;

	int					m_nObjCount;
	int					m_nPageCount;
	int					m_nCurrentPage;
	KWndObjectMatrix	m_ItemsBox;		//物品栏
	KWndButton			m_BuyBtn;
	KWndButton			m_SellBtn;
	KWndButton			m_RepairBtn;
	KWndButton			m_PreBtn;
	KWndButton			m_NextBtn;
	KWndButton			m_CloseBtn;

	//界面状态句柄
	int					m_nStatusHandle;
};