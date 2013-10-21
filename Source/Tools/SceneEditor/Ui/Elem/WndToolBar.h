// -------------------------------------------------------------------------
//	文件名		：	WndToolBar.h
//	创建者		：	万里
//	创建时间	：	2003-7-23 21:55:37
//	功能描述	：	
//
// -------------------------------------------------------------------------
#ifndef __WNDTOOLBAR_H__
#define __WNDTOOLBAR_H__

#pragma once
#include "WndWindow.h"
#include "WndButton.h"

struct KButtonInfo
{
	KWndWindow* m_pButton;
	char szTitle[32];
};

class KWndToolBar : public KWndImage
{
public:
	KWndToolBar();
	~KWndToolBar();
	virtual int		Init(KIniFile* pIniFile, const char* pSection);	//初始化
	virtual int		WndProc(unsigned int uMsg, unsigned int uParam, int nParam);//窗口函数
	virtual void	PaintWindow();							//窗体绘制

	void			Clone(KWndToolBar* pCopy);
	int GetButtonCount() { return m_nButtonNum;	}
	virtual void UpdateData();
protected:
	void FreeAllWindow();
	void AddOneWindow(KIniFile* pIniFile, const char* pSection);
	int m_nButtonNum;
	KButtonInfo* m_pButtons;
};

#endif // __WNDTOOLBAR_H__