/*****************************************************************************************
//	界面--消息窗口
//	Copyright : Kingsoft 2002
//	Author	:   Wooy(Wu yue)
//	CreateTime:	2002-8-14
------------------------------------------------------------------------------------------
*****************************************************************************************/
#pragma once

#include "../Elem/WndText.h"
#include "../Elem/WndPureTextBtn.h"
#include "../Elem/WndShowAnimate.h"

class KUiInformation : protected KWndShowAnimate
{
public:
	void	Initialize();							//初始化
	void	LoadScheme(const char* pScheme);		//载入界面方案
	void	Show(const char* pInformation,
				const char* pszFirstBtnText = "确定",
				const char* pszSecondBtnText = 0,
				KWndWindow* pCallerWnd = 0,
				unsigned int uParam = 0,
                int nInformationLen = -1);			//显示窗口
	void	Close();								//关闭窗口，不通知调用窗口
	KUiInformation();

private:
	void	Hide(int nBtnIndex);					//隐藏窗口
	int		WndProc(unsigned int uMsg, unsigned int uParam, int nParam);//窗口函数
private:
	KWndText256			m_Information;
	KWndPureTextBtn		m_FirstBtn;
	KWndPureTextBtn		m_SecondBtn;
	KWndWindow*			m_pCallerWnd;
	unsigned int		m_uCallerParam;
	int					m_nOrigFirstBtnXPos;
	int					m_nCentreBtnXPos;

    int                 m_nTipModeFlagFlag;
};

void UIMessageBox(const char* pMsg, KWndWindow* pCaller = 0, 
				  const char* pszFirstBtnText = "确定",
				  const char* pszSecondBtnText = 0,
				  unsigned int uParam = 0);
void UiCloseMessageBox();

extern KUiInformation	g_UiInformation;