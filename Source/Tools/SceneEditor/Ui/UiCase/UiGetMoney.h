// -------------------------------------------------------------------------
//	文件名		：	UiGetMoney.h
//	创建者		：	Wooy(Wu yue)
//	创建时间	：	2003-1-7
//	功能描述	：	取钱界面
// -------------------------------------------------------------------------
#ifndef __UiGetMoney_H__
#define __UiGetMoney_H__

#include "../Elem/WndButton.h"
#include "../Elem/WndEdit.h"


class KUiGetMoney : protected KWndImage
{
public:
	//----界面面板统一的接口函数----
	static KUiGetMoney*	OpenWindow(int nMoney, int nMaxMoney, KWndWindow* pRequester,
							 unsigned int uParam, KWndWindow* pMoneyWnd);	//打开窗口，返回唯一的一个类对象实例
	static KUiGetMoney*	GetIfVisible();
	static void			LoadScheme(const char* pScheme);	//载入界面方案
	static void			CloseWindow(bool bDestroy);		//关闭窗口
private:
	KUiGetMoney();
	~KUiGetMoney() {}
	int		Initialize();								//初始化
	void	Show(KWndWindow* pMoneyWnd);
	void	Hide();
	int		WndProc(unsigned int uMsg, unsigned int uParam, int nParam);
	void	OnCancel();
	void	OnOk();
	void	OnCheckInput();
private:
	static KUiGetMoney*	m_pSelf;
	KWndEdit32			m_Money;
	KWndButton			m_OkBtn;
	KWndButton			m_CancelBtn;
	int					m_nMaxMoney;
	KWndWindow*			m_pRequester;
	unsigned int		m_uRequesterParam;
};


#endif // __UiGetMoney_H__