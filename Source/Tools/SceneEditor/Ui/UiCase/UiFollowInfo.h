// UiFollowInfo.h: interface for the KUiFollowInfo class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_UIFOLLOWINFO_H__EB7070F3_C3E6_4CA0_8B33_2BE697BF3D4C__INCLUDED_)
#define AFX_UIFOLLOWINFO_H__EB7070F3_C3E6_4CA0_8B33_2BE697BF3D4C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "../Elem/WndText.h"
#include "../elem/wndshadow.h"
#include "../Elem/WndPureTextBtn.h"
#include "../elem/wndbutton.h"

class KUiFollowInfo : public KWndShadow
{
public:
	void	LoadScheme(const char* pScheme);		//载入界面方案
	void	Show(const char* pInformation,
				const char* pszFirstBtnText = "确定",
				const char* pszSecondBtnText = 0,
				const char* pszCancelBtnText = 0,
				KWndWindow* pCallerWnd = 0,
				unsigned int uParam = 0,
                int nInformationLen = -1);			//显示窗口
	void	Close();								//关闭窗口，不通知调用窗口
	void	Initialize();							//初始化
	KUiFollowInfo();

private:
	void	Hide(int nBtnIndex);					//隐藏窗口
	int		WndProc(unsigned int uMsg, unsigned int uParam, int nParam);
private:
	KWndText256			m_Information;
	KWndPureTextBtn		m_FirstBtn;
	KWndPureTextBtn		m_SecondBtn;
	KWndPureTextBtn		m_BtnClose;
	KWndWindow*			m_pCallerWnd;
	unsigned int		m_uCallerParam;
	int					m_nOrigFirstBtnXPos;
	int					m_nCentreBtnXPos;

    int                 m_nTipModeFlagFlag;
};

void UIMessageBox3(const char* pMsg, KWndWindow* pCaller = 0, 
				  const char* pszFirstBtnText = "确定",
				  const char* pszSecondBtnText = 0,
				  const char* pszCancelBtnText = 0,
				  unsigned int uParam = 0);
void UiCloseMessageBox3();

extern KUiFollowInfo	g_UiFollowInfo;

#endif // !defined(AFX_UIFOLLOWINFO_H__EB7070F3_C3E6_4CA0_8B33_2BE697BF3D4C__INCLUDED_)
