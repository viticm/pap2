// -------------------------------------------------------------------------
//	文件名		：	UiTrade.h
//	创建者		：	吕桂华, Wooy(Wu yue)
//	创建时间	：	2002-9-16 11:57:03
//	功能描述	：	玩家交易系统界面
// -------------------------------------------------------------------------
#pragma once

#include "../Elem/WndShowAnimate.h"
#include "../elem/wndbutton.h"
#include "../Elem/WndText.h"
#include "../Elem/WndEdit.h"
#include "../elem/wndObjContainer.h"
#include "../../../core/src/gamedatadef.h"

struct KUiObjAtRegion;

class KUiTrade : protected KWndShowAnimate
{
public:
	//----界面面板统一的接口函数----
	static KUiTrade*	OpenWindow(KUiPlayerItem* pOther);//打开窗口，返回唯一的一个类对象实例
	static KUiTrade*	GetIfVisible();					//如果窗口正被显示，则返回实例指针
	static void			CloseWindow();					//关闭窗口，同时可以选则是否删除对象实例
	static void			LoadScheme(const char* pScheme);//载入界面方案

	void				OnChangedTakewithItem(KUiObjAtRegion* pObj, int bAdd);	//对方变更随身物品
	void				OnOppositeChangedItem(KUiObjAtRegion* pObj, int bAdd);	//对方变更交易物品
	void				OnSelfChangedItem(KUiObjAtRegion* pObj, int bAdd);		//自己变更交易物品
	void				UpdateOperData();

	void				UpdateTradeEnd(int bTraded);	//交易结束

	virtual void		PaintDesc();					//描述绘制
	virtual void		Paint();						//绘制

private:
	KUiTrade();
	~KUiTrade();
	void	Initialize();						//初始化
	void	LoadScheme(KIniFile* pIni);			//载入界面方案
	int		WndProc(unsigned int uMsg, unsigned int uParam, int nParam);//窗口函数

//	void	Clear();						//清除内容
	void	Breathe();
	void	OnTrade();						//响应点击开始交易
	void	OnOk(int bChecked);				//响应点击有交易意向
	void	OnCancelTrade();
	void	OnAdjustMoney(bool bAdd);
	void	OnAdjustedMoney();
	void	OnModififyMoney();
	void	OnItemPickDrop(ITEM_PICKDROP_PLACE* pPickPos, ITEM_PICKDROP_PLACE* pDropPos);
	void	UpdateTakewithItems();

private:
	static KUiTrade*	m_pSelf;
private:
	KWndText32			m_TakewithMoney;	//随身的金钱
	KWndObjectMatrix	m_TakewithItemsBox;	//随身的物品

	KWndObjectMatrix	m_SelfItemsBox;		//物品栏
	KWndEdit32			m_SelfMoney;		//钱币
	KWndButton			m_Ok;				//确认
	KWndButton			m_Cancel;			//取消
	KWndButton			m_Trade;			//执行交易
	KWndButton			m_AddMoney;			//增加钱
	KWndButton			m_ReduceMoney;		//减少钱
	int					m_nSelfTradeMoney;

	int					m_bAdjustingMoney;
	int					m_nTakewithMoney;

	KWndText32			m_OtherName;		//对方的姓名
	KWndObjectMatrix	m_OtherItemsBox;	//物品栏
	KWndText32			m_OtherMoney;		//钱币
	KWndText32			m_Info;				//对方状态讯息
	KUiPlayerItem		m_OtherData;

	unsigned int		m_uWaitTradeMsgColor;
	unsigned int		m_uOtherLockMsgColor;
	unsigned int		m_uOtherNotLockMsgColor;

	int					m_nDescX, m_nDescY, m_nDescLen;
	DWORD				m_dwLastShowDescTime;
	DWORD				m_dwLastPickTime;
	char				m_szDesc[1024];

	int					m_nMoneyDescX, m_nMoneyDescY, m_nMoneyDescLen;
	DWORD				m_dwLastShowMoneyDescTime;
	char				m_szMoneyDesc[256];

	int					m_nLockDescX, m_nLockDescY, m_nLockDescLen;
	DWORD				m_dwLastShowLockDescTime;
	char				m_szLockDesc[256];

	int					m_nLastOthersMoney;
	BOOL				m_bOtherLock;

	//界面状态句柄
	int					m_nStatusHandle;
};
