/*****************************************************************************************
//	界面窗口体系结构--带文字按键窗口
//	Copyright : Kingsoft 2002
//	Author	:   Wooy(Wu yue)
//	CreateTime:	2002-10-4
------------------------------------------------------------------------------------------
	带文字按钮窗口。
*****************************************************************************************/
#pragma once
#include "WndButton.h"
#include "WndText.h"

//===========================================
//	简单的带文字按钮窗口。
//	文字单行，居中处理，文字中不包含控制符。
//===========================================
class KWndLabeledButton : public KWndButton
{
public:
	KWndLabeledButton();
	void			SetLabel(const char* pLabel);		//设置按钮标题文字
	int				GetLabel(char* pLabel, int nSize);
	void			SetLabelColor(unsigned int Color);	//设置按钮标题文字颜色
	virtual int		Init(KIniFile* pIniFile, const char* pSection);	//初始化
	virtual void	PaintWindow();									//窗体绘制
	void			Clone(KWndLabeledButton* pCopy);
private:
	char	m_Label[32];		//标题文字
	int		m_nLabelLen;		//标题文字串的长度，不包括结束符号
	int		m_nFontSize;		//标题文字的大小
	int		m_nLabelYOffset;	//标题文字向下偏移的量
	unsigned int m_FontColor;	//标题文字的颜色
	unsigned int m_FontBorderColor;	//标题文字的边缘颜色
	unsigned int m_OverColor;	//鼠标移动到上面时候的文字颜色
	unsigned int m_OverBorderColor;	//鼠标移动到上面时候的文字边缘颜色
};

//=====================================================
//	复杂的带文字按钮窗口。
//	文字可以为单行，也可以为多行，单行时候可以指定
//	水平以及垂直的对齐方式。文字中可以包含控制符。
//=====================================================
class KWndRichLabeledButton : public KWndButton
{
public:
	void			SetLabel(const char* pLabel, int nLen);	//设置按钮标题文字
	void			SetLabelColor(unsigned int Color);		//设置按钮标题文字颜色
	virtual int		Init(KIniFile* pIniFile, const char* pSection);	//初始化
	virtual void	PaintWindow();									//窗体绘制
private:
	KWndText256		m_Label;	//标题文字控件
};