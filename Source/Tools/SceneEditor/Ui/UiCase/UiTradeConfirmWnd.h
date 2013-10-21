// -------------------------------------------------------------------------
//	文件名		：	交易确认框
//	创建者		：	Wooy(Wu yue)
//	创建时间	：	2003-5-28
// -------------------------------------------------------------------------
#ifndef __UiTradeConfirm_H__
#define __UiTradeConfirm_H__

#include "../Elem/WndButton.h"
#include "../Elem/WndText.h"
#include "../Elem/wndedit.h"
#include "../../../core/src/gamedatadef.h"

enum TRADE_CONFIRM_ACTION
{
	TCA_BUY,
	TCA_SALE,
	TCA_REPAIR,
};

class KUiTradeConfirm : protected KWndImage
{
public:
	//----界面面板统一的接口函数----
	static KUiTradeConfirm*	OpenWindow(KUiObjAtContRegion* pObj,
			KUiItemBuySelInfo* pPriceInfo, TRADE_CONFIRM_ACTION eAction);	//打开窗口，返回唯一的一个类对象实例
	static KUiTradeConfirm*	GetIfVisible();				//如果窗口正被显示，则返回实例指针
	static void			CloseWindow(bool bDestroy);		//关闭窗口
private:
	KUiTradeConfirm();
	~KUiTradeConfirm() {}
	int		Initialize();								//初始化
	void	Show();
	void	Hide();
	void	LoadScheme(const char* pScheme);			//载入界面方案
	int		WndProc(unsigned int uMsg, unsigned int uParam, int nParam);
	void	OnCancel();
	void	OnBuy();
	void	OnSale();
	void	OnRepair();
	void	OnModifyCount(bool bAdd);
	void	OnCountInputChanged();
private:
	static KUiTradeConfirm*	m_pSelf;
	KWndText32			m_ItemName;
	KWndText32			m_Money;
	KWndEdit32          m_BuyCount;
	KWndImage			m_BuyImg, m_SaleImg, m_RepairImg;
	KWndButton			m_OkBtn;
	KWndButton			m_CancelBtn;
	KWndButton          m_BtnIncrease, m_BtnDecrease;   //增加和减少的Hold型按钮
	KUiObjAtContRegion	m_ItemInfo;
	KUiItemBuySelInfo	m_PriceInfo;
	unsigned int		m_uNormalPriceColor;
	unsigned int		m_uNotEnoughMoneyPriceColor;
};


#endif // __UiTradeConfirm_H__