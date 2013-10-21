// -------------------------------------------------------------------------
//	文件名		：	UiSelPlayer.h
//	创建者		：	吕桂华
//	创建时间	：	2002-9-10 14:24:08
//	功能描述	：	
//	---> 角色选择界面,包含以下界面元素:
//		1) 三个可选择的角色
//		2) 四个按钮：确定，新建，删除，取消
// -------------------------------------------------------------------------
#pragma once 

#include "../elem/WndShowAnimate.h"
#include "../elem/wndtext.h"
#include "../elem/wndbutton.h"

#ifndef MAX_PLAYER_PER_ACCOUNT
	#define	MAX_PLAYER_PER_ACCOUNT	3
#endif
// -------------------------------------------------------------------------

class KUiSelPlayer : protected KWndShowAnimate
{
public:
	//----界面面板统一的接口函数----
	static KUiSelPlayer* OpenWindow();					//打开窗口，返回唯一的一个类对象实例
	static void			 CloseWindow();					//关闭窗口

	static void	GetRoleImageName(char* pszName, const char* pszPrefix, unsigned char bGender, unsigned char bAttribute, int nIndex);

private:
	KUiSelPlayer();
	~KUiSelPlayer() {}
	void	Initialize();						//初始化
	void	LoadScheme(const char* pScheme);	//载入界面方案
	void	PaintWindow();						//窗体绘制
	void	UpdateData();						//更新

	void	Hide();
	int		WndProc(unsigned int uMsg, unsigned int uParam, int nParam);//消息处理
	void	OnClickButton(KWndButton* pWnd, bool bDoubleClick);	//响应点击按钮
	void	OnNew();
	void	OnDel();
	void	OnCancel();
	void	OnSelectPlayer(int nSel, bool bPlaySound = true);
	void	OnEnterGame();
	int		OnKeyDown(unsigned int uKey);
	KWndButton*	GetActiveBtn();
private:
	static KUiSelPlayer* m_pSelf;
private:
	KWndText80	m_LifeTimeText;
	// 界面元素
	KWndText32	m_Name[MAX_PLAYER_PER_ACCOUNT];
	KWndText32	m_Level[MAX_PLAYER_PER_ACCOUNT];
	KWndButton	m_player[MAX_PLAYER_PER_ACCOUNT];	// 三个角色
	KWndImage	m_PlayerInfoBg[MAX_PLAYER_PER_ACCOUNT];
	KWndButton	m_btnOk;		// 确定
	KWndButton	m_btnCancel;	// 取消
	KWndButton	m_btnNew;		// 新建角色
	KWndButton	m_btnDel;		// 删除角色

	unsigned char	m_Gender[MAX_PLAYER_PER_ACCOUNT];
	unsigned char	m_Attribute[MAX_PLAYER_PER_ACCOUNT];
	char			m_szSelPlayerSound[MAX_PLAYER_PER_ACCOUNT][128];

	KWndImage	m_ButterflyWnd;

	char		m_szLoginBg[32];

	char		m_szPlayerImgPrefix[128];

	int			m_ChildPos[4];
	int			m_ChildWndXOffset[4];
	int			m_ChildWndYOffset;

private:
	int			m_bJustClicked;
	short		m_nNumPlayer;
	short		m_nSelPlayer;	// 当前选中的是第几个角色
						//  -1	-- 无选中；    0	-- 选中第一个；
						//  1	-- 选中第二个；2	-- 选中第三个；
};
