/*****************************************************************************************
//	界面窗口体系结构--滚动条窗口
//	Copyright : Kingsoft 2002
//	Author	:   Wooy(Wu yue)
//	CreateTime:	2002-7-22
------------------------------------------------------------------------------------------
*****************************************************************************************/
#pragma once
#include "WndWindow.h"
#include "WndButton.h"

#define WNDSCROLL_ES_VERTICAL		0x0001		//垂直型的滚动条
#define	WNDSCROLL_F_DRAGGING_SLIDE	0x0100		//滚动条的滑块正在被拖动

class KWndScrollBar : public KWndImage
{
private:
	unsigned int	m_Flag;
	int			m_nMinValue;	//最小值
	int			m_nMaxValue;	//最大值
	int			m_nCurValue;	//当前值
//	int			m_nLineSize;	//一行大小
	int			m_nPageSize;	//一页大小
	int			m_nMinPosition;	//滑动活动起始位置
	int			m_nMaxPosition;	//滑动活动终止位置
	KWndButton	m_SlideBtn;		//滑动按钮
	int			m_nImgRange;	//重复贴图时，贴图的宽度或高度

public:
	KWndScrollBar();
	virtual int		Init(KIniFile* pIniFile, const char* pSection);	//初始化
	virtual int		WndProc(unsigned int uMsg, unsigned int uParam, int nParam);//窗口函数
	virtual void	PaintWindow();							//窗体绘制
	void			SetSize(int nWidth, int nHeight);		//设置窗口大小

	int				ScrollLine(bool bPre);
	int				ScrollPage(bool bPre);
	void			SetScrollPos(int nPosition);				//设置位置
	int				GetScrollPos() { return m_nCurValue; }		//获取位置
	void			SetValueRange(int nMinValue, int nMaxValue);	//设置取值范围
	int				GetMaxValue() { return m_nMaxValue;}	//取值范围
	int				GetMinValue() { return m_nMinValue;}	//取值范围
	void			Clone(KWndScrollBar* pCopy);
	int				GetMinHeight();
private:
	void			OnLButtonDown(int x,int y);					//响应鼠标左键按下
	void			OnSlideBtnPressed();						//响应滑动按钮被按下
	void			OnDraggingSlide(int x, int y);				//正在拖动滑动按钮
	void			SetSlideBtnPos();							//设置滑动块窗口位置
};
