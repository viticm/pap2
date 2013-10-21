//////////////////////////////////////////////////////////////////////////////////////
//	文件名			:	UiKillerCreateTask.h
//	创建人			:	王西贝
//	创建时间		:	2003-12-26 10:50:03
//	文件说明		:	创建通缉任务对话框
//////////////////////////////////////////////////////////////////////////////////////

#ifndef __UIKILLERCREATETASK_H__
#define __UIKILLERCREATETASK_H__

#include "../elem/WndButton.h"
#include "../elem/WndText.h"
#include "../elem/WndObjContainer.h"
#include "../Elem/WndShowAnimate.h"
#include "../Elem/WndEdit.h"

class KRoleNameEdit : public KWndEdit
{
private:
	char			m_Text[200];
public:
	KRoleNameEdit();
};


class KUiKillerCreateTask : public KWndShowAnimate
{
public:
	//----界面面板统一的接口函数----
	static KUiKillerCreateTask*	OpenWindow(unsigned int uParam);	//打开窗口，返回唯一的一个类对象实例
	static KUiKillerCreateTask*	GetIfVisible();
	static void			LoadScheme(const char* pScheme);	//载入界面方案
	static void			CloseWindow(bool bDestroy);		//关闭窗口
private:
	KUiKillerCreateTask();
	virtual ~KUiKillerCreateTask() {}
	int		Initialize();								//初始化
	int		WndProc(unsigned int uMsg, unsigned int uParam, int nParam);
	void	OnCancel();
	void	OnOk();
	void	OnModifyCount(bool bAdd);
	void	OnRewardInputChanged();
	void	OnTimeInputChanged();

private:
	static KUiKillerCreateTask*	m_pSelf;
	KRoleNameEdit		m_cKilleeName;					//被杀者角色名
	KWndEdit32			m_cReward;	
	KWndText256			m_cActiveTime;					//激活时间(单位:小时)
	int					m_nActiveTimeNum;
	KWndButton          m_BtnIncrease, m_BtnDecrease;   //增加和减少的Hold型按钮
	KWndText256			m_cCommision;					//系统佣金
	KWndButton			m_OkBtn;
	KWndButton			m_CancelBtn;
	int					m_nMinReward;
	int					m_nMoneyPerHour;
	int					m_nMaxActiveTime;
};


#endif //__UIKILLERCREATETASK_H__
