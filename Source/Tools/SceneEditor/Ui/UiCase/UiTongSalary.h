/*******************************************************************************
File        : UiTongSalary.h
Creator     : Fyt(Fan Zhanpeng)
create data : 02-24-2004(mm-dd-yyyy)
Description : 帮会发钱界面
********************************************************************************/

#pragma once
#include "..\elem\wndlabeledbutton.h"
//#include "..\elem\wndimage.h"
#include "..\elem\wndtext.h"
#include "..\elem\wndedit.h"

class KUiTongSalary : public KWndImage
{
public:
	KUiTongSalary(void);
	~KUiTongSalary(void);

public: //类自身的特有数据类型
	enum enumSALARY_TYPE
	{
		enumST_WHOLE,
		enumST_WHOLE_DIRECTOR,
		enumST_WHOLE_MANAGER,
		enumST_WHOLE_MEMBER,
		enumST_PER_DIRECTOR,
		enumST_PER_MANAGER,
		enumST_PER_MEMBER,
		enumST_NUM,
	};

	enum enumINPUT_SALARY
	{
		enumIS_DIRECTOR,
		enumIS_MANAGER,
		enumIS_MEMBER,
		enumIS_NUM,
	};

public: //界面标准的接口
	static			KUiTongSalary* OpenWindow();		//打开窗口
	static			KUiTongSalary* GetIfVisible();		//如果窗口正被显示，则返回实例指针
	static void		CloseWindow(bool bDestory = TRUE);	//关闭窗口，同时可以选则是否删除对象实例
	static void		LoadScheme(const char* pScheme);	//载入界面方案

private://界面标准的内部处理方法
	void			Initialize();						//初始化界面

	//处理界面消息的处理方法
	virtual int		WndProc(unsigned int uMsg, unsigned int uParam, int nParam);

private: //标准界面数据定义
	static KUiTongSalary*	ms_pSelf;

public:  //接口
	//清理人数
	void			ClearCount();

	//外部数据进来的处理，简单的统计在线人数就ok
	void			InjectData(KTongMemberItem *pItem, int nCount, int nType);

	/**
	 *@brief 为薪水发放命令的书写安排文案数据(就是告诉我有多少资金等等啦)
	 */
	void			ArrangeDecree(KTongInfo sInfo);

	/**
	 *@brief 更新界面显示
	 */
	void			UpdateView();

private: //内部处理方法
	void			Reset();							///重置计数数值

	/**
	 *@brief 导入部分数据完成的提醒
	 */
	void			InjectNotify();

	/**
	 *@brief 根据人数计算各个薪水数字
	 */
	void			WorkoutSalary();

	/**
	 *@brief 响应字符编辑框的改动
	 */
	void			OnEditChange(KWndWindow *pWnd);

	/**
	 *@brief 响应三个总数/单人切换按钮的点击
	 */
	void			OnIsTotalButtonCheck();

	/**
	 *@brief 响应确定按钮按下
	 */
	void			OnConfirm();

private: //数据资源部
	//在线人数计数
	int						m_aryCount[enumIS_NUM];

	//记录各种金额的数组
	int						m_arySalary[enumST_NUM];

	//记录玩家输入数字的数组
	int						m_aryInputSalary[enumIS_NUM];

	//输入的金额是总数吗？
	BOOL					m_arybIsTotal[enumIS_NUM];

	//要进行操作的帮会的数据
	KTongInfo				m_TongInfo;

	//字符串数据，帮会资金不足
	char					m_szNotEnoughMoney[128];
	//字符串数据，人数、金额提示字符串
	char					m_szDescript[128];

private: //空间资源部
	KWndEdit32				m_editSalary[enumIS_NUM];
	KWndButton				m_btnIsTotal[enumIS_NUM];
	KWndText256				m_textDescription;

	KWndButton				m_btnConfirm;
	KWndButton				m_btnCancel;
};
