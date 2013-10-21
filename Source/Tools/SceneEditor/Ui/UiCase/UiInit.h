// -------------------------------------------------------------------------
//	文件名		：	UiInit.h
//	创建者		：	彭建波
//	创建时间	：	2002-9-10 11:25:36
//	功能描述	：	初始界面	
//
// -------------------------------------------------------------------------
#ifndef __UIINIT_H__
#define __UIINIT_H__

#include "../Elem/WndButton.h"
#include "../Elem/WndShowAnimate.h"

class KUiInit : protected KWndShowAnimate
{
public:
	//----界面面板统一的接口函数----
	static KUiInit*		OpenWindow(bool bStartMusic = true, bool bJustLaunched = false);	//打开窗口，返回唯一的一个类对象实例
	static void			CloseWindow();							//关闭窗口
	static void			PlayTitleMusic();
	static void			StopTitleMusic();

private:
	KUiInit() {}
	~KUiInit() {}
    void    ShowCompleted();
	void	Initialize();						//初始化
	void	LoadScheme(const char* pScheme);	//载入界面方案
private:
	int		WndProc(unsigned int uMsg, unsigned int uParam, int nParam);
	void	OnClickButton(KWndButton* pWnd);	//响应点击按钮
	KWndButton*	GetActiveBtn();
	void	OnAutoLogin();
	int		OnKeyDown(unsigned int uKey);
	void	PlayStartMovie();
private:
	static KUiInit* m_pSelf;
private:
	char		m_szLoginBg[32];
	KWndButton m_EnterGame;					// 进入游戏
	KWndButton m_GameConfig;				// 游戏设置
	KWndButton m_DesignerList;				// 制作人员名单
//	KWndButton m_AutoLogin;					//自动登陆
	KWndButton m_ExitGame;					// 退出游戏

	int		   m_nCurrentMovieIndex;		//当前播放的动画的编号
	
	char       m_szConfirm[16];             //确认的字串
};

#endif // __UIINIT_H__