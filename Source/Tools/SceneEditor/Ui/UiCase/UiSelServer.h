/*****************************************************************************************
//	界面--选择游戏服务器窗口
//	Copyright : Kingsoft 2002
//	Author	:   Wooy(Wu yue)
//	CreateTime:	2002-8-12
------------------------------------------------------------------------------------------
*****************************************************************************************/
#pragma once
#include "../Elem/WndList.h"
#include "../Elem/WndScrollBar.h"
#include "../Elem/WndShowAnimate.h"

class KUiSelServer : protected KWndShowAnimate
{
public:
	static KUiSelServer* OpenWindow();				//打开窗口，返回唯一的一个类对象实例
	static void			 CloseWindow(bool bDestroy);//关闭窗口
private:
	KUiSelServer();
	~KUiSelServer();
	void	Initialize();						//初始化
	void	LoadScheme(const char* pScheme);	//载入界面方案
	int		WndProc(unsigned int uMsg, unsigned int uParam, int nParam);	//窗口函数
	void	OnLogin();
	void	OnCancel();
	int		OnKeyDown(unsigned int uKey);
	void	OnClickButton(KWndButton* pBtn);
	void	GetList();

private:
	static KUiSelServer* m_pSelf;
private:
	struct KLoginServer* m_pServList;

	KWndList		m_List;
	KWndScrollBar	m_ScrollBar;
	KWndButton		m_Login;
	KWndButton		m_Cancel;
	char			m_szLoginBg[32];

	bool			m_bSelRegion;
	int				m_nRegionIndex;
};
