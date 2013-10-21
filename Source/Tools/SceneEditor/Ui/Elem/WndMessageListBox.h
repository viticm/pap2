// -------------------------------------------------------------------------
//	文件名		：	ShowChatText.h
//	创建者		：	彭建波
//	创建时间	：	2002-9-16 16:22:52
//	功能描述	：	显示聊天内容
// -------------------------------------------------------------------------
#pragma once

#include "WndWindow.h"
#include "WndScrollbar.h"

struct KOneMsgInfo
{
	int			 nLines;				//这条信息占了多少行
	int			 nCharWidth;			//这条信息占了字符宽
	unsigned int uTextBKColor;			//这条信息字显示时的文字衬底的颜色,0为无
	int			 nMaxSize;				//Msg可以容纳的最大信息长度
	int			 nLen;					//信息长度
	char		 Msg[1];				//信息的内容
};

class KWndScrollBar;

class KWndMessageListBox : public KWndWindow
{
public:
	KWndMessageListBox();
	virtual ~KWndMessageListBox();
	virtual int		Init(KIniFile* pIniFile, const char* pSection);//初始化

	void	SetFirstShowLine(int nLine);	//设置第一条被显示的文字是全部文字的第几行
	int		GetFirstShowLine();				//获取第一条被显示的文字是全部文字的第几行
	void	SetScrollbar(KWndScrollBar* pScroll);

	int		SetCapability(int nNumMessage);
	int		GetCapability(){ return m_nCapability;}
	int		GetCurSel() { return m_nSelMsgIndex; }
	int		SetCurSel(int nIndex);

	int		AddOneMessage(const char* pText, int nLen, unsigned int uTextBKColor = 0);
	int		GetOneMessage(int nIndex, char* pBuffer, int nLen, bool bExcludeCtrl);
	void	RemoveAMessage(int nIndex);
	int		GetMsgCount() { return m_nNumMessage; }
	int		HitTextAtPoint(int x, int y);

	void	SetSize(int nWidth, int nHeight);//设置窗口大小
	void	Clear();
	void	Clone(KWndMessageListBox* pCopy);
	int		GetMaxShowLine() {return m_nNumMaxShowLine;}
	int		GetItemLineCount(int nIndex);

	//从窗口分离出数据
	unsigned int	SplitData();
	//给窗口捆绑数据
	unsigned int	BindData(unsigned int	hData);
	//释放窗口数据句柄
	static void		FreeData(unsigned int	hData);

	virtual int		PtInWindow(int x, int y);

	void ClearHideLine();
	void HideNextLine();
	void HideAllLine();
	
	int	GetMinHeight();
	
	void SetFontSize(int nFontSize);
	int GetFontSize()	{return m_nFontSize;}

private:
	int		WndProc(unsigned int uMsg, unsigned int uParam, int nParam);
	void	UpdateData();					//根据内容增删或者窗口尺寸变化重新作些参数计算以及滚动条容量调整
	virtual void	PaintWindow();			//绘制窗口
	void	OnMouseMove(int x, int y);
	void	OnLButtonDown(int x, int y);
	void	OnLButtonDClick(int x, int y);
	int		GetMsgAtPoint(int x, int y);

private:
	struct KMessageListData
	{
		KOneMsgInfo**	pMessages;			//信息数据
		int				nNumMessage;		//信息条数目
		int				nCapability;		//信息条最多允许数目
		int				nStartShowMsg;		//首条显示的信息
		int				nStartMsgSkipLine;	//首条显示的信息上方忽略的行数目
		int				nSelMsgIndex;		//当前选择的消息
	};
	
private:
	KWndScrollBar*	m_pScrollbar;			//滚动条
	KOneMsgInfo**	m_pMessages;			//信息数据

	int				m_nNumMessage;			//信息条数目
	int				m_nCapability;			//信息条最多允许数目

	int				m_nNumMaxShowLine;		//最多可显示的行数目
	int				m_nNumBytesPerLine;		//每一行字符的数目
	int				m_nNumVisibleTextLine;	//可以看见的有文字的行的数目
	
	int				m_nHideNumLine;		//被强迫隐藏的行数,追加/删除/选中/滚动行/改尺寸时会无效

	int				m_nFontSize;			//字体大小
	int				m_nStartShowMsg;		//首条显示的信息
	int				m_nStartMsgSkipLine;	//首条显示的信息上方忽略的行数目

	int				m_nSelMsgIndex;			//当前选择的消息
	int				m_nHLMsgIndex;			//鼠标所指消息的索引
	unsigned int	m_MsgColor;				//默认的文字颜色
	unsigned int	m_MsgBorderColor;		//默认的文字边缘颜色
	unsigned int	m_SelMsgColor;			//被选中文字的颜色
	unsigned int	m_SelMsgBorderColor;	//被选中文字的边缘颜色
		
	unsigned int	m_SelMsgBgColor;		//被选中消息的背景颜色
	unsigned int	m_HLMsgColor;			//鼠标所指消息的颜色
	unsigned int	m_HLMsgBorderColor;		//鼠标所指消息的边缘颜色
	unsigned int	m_uTextLineShadowColor;	//有文字的行拥有的背景颜色，如果为0，表示无此背景色

	BOOL m_bHitText;

	unsigned int	GetOffsetTextHeight();
};

class KScrollMessageListBox : public KWndWindow
{
public:
	KScrollMessageListBox();

	virtual int		Init(KIniFile* pIniFile, const char* pSection);//初始化
	int		WndProc(unsigned int uMsg, unsigned int uParam, int nParam);
	
	KWndMessageListBox* GetMessageListBox()
	{
		return &m_MsgList;
	}
	KWndScrollBar* GetScrollBar()
	{
		return &m_Scroll;
	}
	int		HeightToLineHeight(int nHeight);
	int		WidthToCharWidth(int nWidth);
	int		GetMinHeight();
protected:
	KWndMessageListBox	m_MsgList;
	KWndScrollBar		m_Scroll;
	int m_nLineHeight;
	int m_nMinLineCount;
	int	m_nMaxLineCount;
	int m_nCurrentLineCount;

	void	SetMsgLineCount(int nCount);
	void	InitMinMaxLineCount(int nMin, int nMax);//初始化
};

bool	MsgListBox_LoadContent(KWndMessageListBox* pBox, KIniFile* pFile, const char* pszSection);