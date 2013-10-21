// -------------------------------------------------------------------------
//	文件名		：	UiESCDlg.h
//	创建者		：	彭建波
//	创建时间	：	2002-9-16 10:32:22
//	功能描述	：	
//
// -------------------------------------------------------------------------
#ifndef __UIESCDLG_H__
#define __UIESCDLG_H__

#include "../Elem/WndButton.h"
#include "../Elem/WndShowAnimate.h"

class KUiESCDlg : protected KWndShowAnimate
{
public:
	//----界面面板统一的接口函数----
	static KUiESCDlg*	OpenWindow();		//打开窗口，返回唯一的一个类对象实例
	static KUiESCDlg*	GetIfVisible();
	static void			CloseWindow(bool bDestroy);		//关闭窗口
private:
	KUiESCDlg() {}
	~KUiESCDlg();
	void	Show();
	void	Hide();
	int		Initialize();						//初始化
	void	LoadScheme(const char* pScheme);	//载入界面方案
	int		WndProc(unsigned int uMsg, unsigned int uParam, int nParam);//窗口函数
	void	OnClickButton(KWndWindow* pWnd);	//响应点击按钮
	int		OnKeyDown(unsigned int uKey);
	KWndButton*	GetActiveBtn();
private:
	static KUiESCDlg*	m_pSelf;
	KWndButton	m_ExitGameBtn;			// 退出游戏
	//KWndButton	m_ExitBtn;				// 退出程序
	KWndButton	m_HelpBtn;
	//KWndButton	m_TaskBtn;
	KWndButton	m_OptionsBtn;
	KWndButton	m_ContinueGameBtn;		// 继续游戏
};

#endif // __UIESCDLG_H__