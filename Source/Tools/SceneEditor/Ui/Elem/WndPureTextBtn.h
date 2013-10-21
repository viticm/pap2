/*****************************************************************************************
//	界面窗口体系结构-纯文字按钮
//	Copyright : Kingsoft 2003
//	Author	:   Wooy(Wu yue)
//	CreateTime:	2003-1-19
------------------------------------------------------------------------------------------
*****************************************************************************************/
#pragma once
#include "WndWindow.h"

#define	WNDPTB_MAX_TEXT_LEN	64

#define	WNDPTB_S_CENTRE_ALIGN	0x00008000

#define	WNDPTB_F_BE_PRESSEDDOWN	0x00000001
#define	WNDPTB_F_OVER			0x00000002
#define	WNDPTB_F_CHECK			0x00000004

class KWndPureTextBtn : public KWndWindow
{
public:
	KWndPureTextBtn();
	virtual int		Init(KIniFile* pIniFile, const char* pSection);	//初始化
	virtual void	PaintWindow();									//绘制窗口
	void			SetText(const char* pText, int nLen = -1);		//设置文本文字
	virtual int		WndProc(unsigned int uMsg, unsigned int uParam, int nParam);//窗口函数
	void			CheckButton(int bChecked);
	void			Clone(KWndPureTextBtn* pCopy);
	int             GetFontSize();
private:
	void			AdjustPosition();
private:
	unsigned int	m_Flag;
	char			m_sText[WNDPTB_MAX_TEXT_LEN];
	int				m_nTextLen;			//字符串的存储长度
	unsigned int	m_NormalColor;
	unsigned int	m_OverColor;
	unsigned int	m_PressedColor;
	unsigned int	m_NormalBorderColor;
	unsigned int	m_OverBorderColor;
	unsigned int	m_PressedBorderColor;
	int				m_nFontSize;
};
