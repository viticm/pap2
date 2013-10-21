/*****************************************************************************************
//	界面--组队管理界面
//	Copyright : Kingsoft 2002-2003
//	Author	:   Wooy(Wu yue)
//	CreateTime:	2002-9-2
------------------------------------------------------------------------------------------
*****************************************************************************************/
#pragma once

#include "../Elem/WndShowAnimate.h"
#include "../Elem/WndText.h"
#include "../elem/wndbutton.h"
#include "../elem/WndList.h"
#include "../Elem/WndScrollBar.h"
#include "../../../core/src/gamedatadef.h"

class KUiTeamManage : public KWndShowAnimate
{
public:
	static KUiTeamManage* OpenWindow();		//打开窗口，返回唯一的一个类对象实例
	static KUiTeamManage* GetIfVisible();	//如果窗口正被显示，则返回实例指针
	static void			  CloseWindow();	//关闭窗口
	static void			  LoadScheme(const char* pScheme);		//载入界面方案
//	static void			  UpdateNearbyTeams(KUiTeamItem* pList, int nCount); //更新周围队伍的列表
	void				  UpdateData(KUiPlayerTeam* pInfo);

private:
	KUiTeamManage();
	~KUiTeamManage() {}
	void	Initialize();							//初始化
	void	LoadScheme(class KIniFile* pSetting);	//载入界面方案
protected:
	int		WndProc(unsigned int uMsg, unsigned int uParam, int nParam);//窗口函数
	//void	OnApplyJoin();					//申请加入
	void	OnKick();						//踢除队员
	void	OnAppoint();					//任命队长
	void	OnInvite();						//邀请加入
	void	OnLeave();						//离开队伍
	void	OnDismiss();					//解散队伍
	void	OnRefresh();					//刷新
	void	OnSelTeamMember(int nItem);
	void	OnSelNearby(int nItem);
	void	Clear();
	//void	SwitchNearbyMode(bool bNearbyPlayer);
private:
	static KUiTeamManage* m_pSelf;

	KWndText32		m_LeaderAbility;		// 统帅力	
	KWndList		m_TeamMember;			// 队员列表
	KWndList		m_NearbyList;			// 申请加入列表
	KWndScrollBar	m_MemberScroll;			// 成员列表滚动条
	KWndScrollBar	m_NearbyScroll;			// 申请加入列表滚动条

//	KWndButton		m_NearbyPlayerTab;		// 列出周围玩家的按钮
//	KWndButton		m_NearbyTeamTab;		// 列出周围玩家的按钮

	KWndButton		m_btnInvite;			// 邀请
	KWndButton		m_btnKick;				// 踢出
//	KWndButton		m_btnApplyJoin;			// 申请加入

	KWndButton		m_btnAppoint;			// 任命
	KWndButton		m_btnLeave, m_btnDismiss;// 离队、解散
	KWndButton		m_btnCloseTeam;			// 关闭组队
	KWndButton		m_btnRefresh;			// 刷新
	KWndButton		m_btnCancel;			// cancel button

	KUiPlayerTeam	m_Info;
	KUiPlayerItem*	m_pPlayersList;
	KUiPlayerItem*	m_pNearbyPlayersList;
//	KUiTeamItem*	m_pNearbyTeamsList;
};
