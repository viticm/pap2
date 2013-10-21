/*****************************************************************************************
//	界面--login过程背景窗口
//	Copyright : Kingsoft 2002
//	Author	:   Wooy(Wu yue)
//	CreateTime:	2002-12-16
------------------------------------------------------------------------------------------
*****************************************************************************************/

#pragma once
#include "../Elem/WndImage.h"
#include "../Elem/WndText.h"

class KUiLoginBackGround : protected KWndImage
{
public:
	static KUiLoginBackGround*	OpenWindow(const char* pszConfig);	//打开窗口，返回唯一的一个类对象实例
	static void					CloseWindow(bool bDestroy);			//关闭窗口
	static void					SetConfig(const char* pszConfig);	//设置图形
private:
	KUiLoginBackGround() {}
	~KUiLoginBackGround() {}
	void	Initialize();					//初始化
	void	LoadScheme(KIniFile* pIni);		//载入界面方案
	void	Breathe();						//活动函数
private:
	static 	KUiLoginBackGround* m_pSelf;
#define	MAX_NUM_BUTTERFLY	3
	KWndImage		m_Butterflys[MAX_NUM_BUTTERFLY];
	unsigned int	m_uAdviceIntervalMin[MAX_NUM_BUTTERFLY];
	unsigned int	m_uAdviceIntervalMax[MAX_NUM_BUTTERFLY];
	unsigned int	m_uInterval[MAX_NUM_BUTTERFLY];
	unsigned int	m_uLastSleepTime[MAX_NUM_BUTTERFLY];
	char			m_szConfig[32];
	KWndText80		m_VersionWnd;
};
