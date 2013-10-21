/*****************************************************************************************
//	界面窗口体系结构--输入窗口
//	Copyright : Kingsoft 2002
//	Author	:   Wooy(Wu yue)
//	CreateTime:	2002-7-22
------------------------------------------------------------------------------------------
*****************************************************************************************/
#pragma once
#include "WndWindow.h"
#include "WndText.h"

#define	WNDEDIT_ES_FILTER				0x00ff
#define	WNDEDIT_ES_ENGLISH_AVAILABLE	0x0001		//可以输入西文字符，符号、标点等
#define	WNDEDIT_ES_IME_AVAILABLE		0x0002		//可以输入ime字符（中文字符）等
#define	WNDEDIT_ES_CHAR_TYPE_FILTER		(WNDEDIT_ES_ENGLISH_AVAILABLE | WNDEDIT_ES_IME_AVAILABLE)
#define	WNDEDIT_ES_MASK_CHARACTER		0x0010		//显示时以特殊符号替换要显示的字符
#define	WNDEDIT_ES_MULTI_LINE			0x0020		//多行编辑
#define	WNDEDIT_ES_WIN32_WND			0x0040		//采用Win32窗口
#define	WNDEDIT_F_HAVE_FOCUS			0x0100		//为当前拥有输入焦点的窗口

class KWndEdit : public KWndWindow
{
public:
	KWndEdit();
	~KWndEdit();
	virtual int	Init(KIniFile* pIniFile, const char* pSection);//初始化
	virtual int		WndProc(unsigned int uMsg, unsigned int uParam, int nParam);//窗口函数
	virtual void	PaintWindow();							//绘制窗口
	void			SetColor(unsigned int uColor);
	void			SetText(const char* pText);				//设置文本文字
	int				GetText(char* pBuffer, int nSize, bool bExcludeSpace);//获取字符串内容
	void			SetIntText(int nNumber);				//设置文本串的内容为与表述所整数的字符串
	int				GetIntNumber();
	void			Enable(int bEnable);					//禁止或者允许使窗口被操作
	void			Clone(KWndEdit* pCopy);
	void			ClearText(bool bSafe = false);
	int				InsertString(const char* pString, int nLen);
	void			GetCaretPosition(int* nx, int* ny);
	unsigned int	GetFocusBkColor()	{return m_FocusBkColor;}
	void	SetFocusBkColor(unsigned int nC)	{m_FocusBkColor = nC;}
	void	SetLimitLen(int nLen){m_nLimitText = nLen;}

	virtual void	Show();									//显示窗口
	virtual void	Hide();									//隐藏窗口
	
	BOOL	SetType(int nType);								//设置输入框类型 0数字/1英文/2中英文 [wxb 2004-3-29]

protected:
	void			SetTextPtr(char* pText, int nMaxLen);	//设置文本缓冲区指针
	int				InsertChar(int nChar);					//
	int				InsertChar(char cLow, char cHigh);		//
	int				OnKeyDown(int nKeyCode, int nModifiers);//响应键盘按键操作
	int				OnKeyDownWin32(int nKeyCode, int nModifiers);//响应键盘按键操作

private:
	int				CharFromPos(int x, int y);				//retrieves information about the character closest to a specified point in the client area of an edit control
	void			UpdateData();
	void			FmtForShow();
	int				Paste();
	int				Copy();
private:
	unsigned int	m_Flag;
	int			m_nFontSize;				//字体大小
	char*		m_pText;					//文本缓冲区指针
	int			m_nBufferSize;				//文本缓冲区指针的长度
	int			m_nLimitText;				//字符串长度（BYTE）限制，不包括字符串结束符号。
	int			m_nTextLen;					//字符串实际长度

	int			m_nSkipAhead;				//显示时忽略前端超出的字符，前端被忽略字符的容量
	int			m_nSkipBehind;				//显示时忽略后端超出的字符，后端被忽略字符的容量

	int			m_nCaretLineIndex;			//插入符所在的行位置
	int			m_nCaretPos;				//插入符的位置
	int			m_nCaretTime;				//控制插入符显示的时钟

    int         m_nCaretX;
    int         m_nCaretY;

	unsigned int m_TextColor;				//字体颜色
	unsigned int m_TextBorderColor;			//字体边缘颜色
	unsigned int m_FocusBkColor;			//有焦点时的背景颜色

	HWND        m_hWin32Wnd;
};

//=====================================
//	文本缓冲区长度为32的文本输入窗口类
//=====================================
class KWndEdit32 : public KWndEdit
{
private:
	char			m_Text[32];
public:
	KWndEdit32();
};

//=====================================
//	文本缓冲区长度为32的文本输入窗口类
//=====================================
class KWndEdit512 : public KWndEdit
{
private:
	char			m_Text[512];
public:
	KWndEdit512();
};
