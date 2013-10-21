/* 
 * File:     UiStallMarkPrice.h
 * Desc:     摆摊标价
 * Author:   flying
 * Creation: 2003/10/23
 */
#if !defined _UISTALLMARKPRICE_HEADER
#define _UISTALLMARKPRICE_HEADER

#include "../Elem/WndButton.h"
#include "../Elem/WndEdit.h"

class KUiStallMarkPrice : public KWndImage
{
public:
	//----界面面板统一的接口函数----
	static KUiStallMarkPrice*	OpenWindow(int nMoney, int nMaxMoney, int nObjIdx,
										   unsigned int uParam, KWndWindow* pMoneyWnd);	//打开窗口，返回唯一的一个类对象实例
	static KUiStallMarkPrice*	GetIfVisible();
	static void			LoadScheme(const char* pScheme);	//载入界面方案
	static void			CloseWindow(bool bDestroy);		//关闭窗口
	static void   SetObjectName(char *pObjectName);
private:
	KUiStallMarkPrice();
	virtual ~KUiStallMarkPrice() {}
	int		Initialize();								//初始化
	void	Show(KWndWindow* pMoneyWnd);
	void	Hide();
	int		WndProc(unsigned int uMsg, unsigned int uParam, int nParam);
	void	OnCancel();
	void	OnOk();
	void	CheckInput();

private:
	static KUiStallMarkPrice*	m_pSelf;
	KWndEdit32			m_cPrice;
	KWndButton			m_OkBtn;
	KWndButton			m_CancelBtn;
	int					m_nMaxMoney;
	int					m_nObjIndex;
	static	KWndText256   m_ObjectName;
};

#endif
