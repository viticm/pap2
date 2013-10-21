/*****************************************************************************************
//	阴影窗口
//	Copyright : Kingsoft 2002
//	Author	:   Wooy(Wu yue)
//	CreateTime:	2003-4-5
------------------------------------------------------------------------------------------
*****************************************************************************************/
#pragma once
#include "WndWindow.h"

class KWndShadow : public KWndWindow
{
public:
	virtual int	Init(KIniFile* pIniFile, const char* pSection);//初始化
	virtual int		PtInWindow(int x, int y);	//判断一个点是否在窗口范围内,传入的是绝对坐标
	void	SetColor(unsigned int uColor);
	void	Clone(KWndShadow* pCopy);
	//设置是否如平常窗口一样判断点是否在窗口内
	void	SetExactWnd(BOOL bExactWnd) { m_bExactWnd = bExactWnd; }
	KWndShadow() { m_uColor = 0; m_bExactWnd = FALSE; }
protected:
	void	PaintWindow();
private:
	unsigned int	m_uColor;
	BOOL			m_bExactWnd;	//是否如平常窗口一样判断点是否在窗口内,
									//如果不是，则此窗口完全穿透
};
