#pragma once
//#include "KTimeLine.h"
//
//#define GETRED(SRC)   ((SRC&0x00ff0000)>>16)
//#define D3DCOLOR_GETGREEN(SRC) ((SRC&0x0000ff00)>> 8)
//#define D3DCOLOR_GETBLUE(SRC)        (SRC&0x000000ff)
//
//class KTimeLineColorEditBar: public CWnd
//{
//	DECLARE_DYNAMIC(KTimeLineColorEditBar)
//	enum enumDrawStyle
//	{
//		ENUM_DRAW_STYLE_BEGIN = 0,
//		ENUM_DRAW_STYLE_VERTICAL,
//		ENUM_DRAW_STYLE_HORIZONTAL,
//		ENUM_DRAW_STYLE_COUNT,
//		ENUM_DRAW_STYLE_FOCE_DWORD = 0xffffffff,
//	};
//
//	enum enumChannel
//	{
//		ENUM_CHANNEL_BEGIN = 0,
//		ENUM_CHANNEL_BLUE = ENUM_CHANNEL_BEGIN,
//		ENUM_CHANNEL_GREEN,
//		ENUM_CHANNEL_RED,
//		ENUM_CHANNEL_ALPHA,
//		ENUM_CHANNEL_COUNT,
//		ENUM_CHANNEL_FORCE_DWORD = 0xffffffff,
//	};
//public:
//	KTimeLineColorEditBar(KTimeLine<float, DWORD>* pTimeLine, CWnd* pParent, const RECT& rectWindow, const RECT& rectBar, const RECT* prectDrop = NULL);
//	~KTimeLineColorEditBar();
//	void DrawColorBar(CDC* pdc);
//
//	void SetWindowRect(const RECT& rectWindow) { m_rectWindow = rectWindow; }
//	RECT GetWindowRect() { return m_rectWindow;}
//
//	inline void SetColorBarRect(const RECT& rectBarRegion);
//	RECT GetColorBarRect() const { return m_rectBarRegion; }
//
//	void SetDropRegion(const RECT& rectDropRegion) { m_rectDropRegion.left = 0; 
//	m_rectDropRegion.top = 0;
//	m_rectDropRegion.bottom = abs(rectDropRegion.bottom - rectDropRegion.top);
//	m_rectDropRegion.right = abs(rectDropRegion.right - rectDropRegion.left);}
//	RECT GetDropRegion() { return m_rectDropRegion; }
//
//private:
//
//	BOOL Create(CWnd* pParent, const RECT& rectWindow, const RECT& rectBar, const RECT* prectDrop /*= NULL*/);//创建窗口
//
//	//////////////////////////////////////////////////////////////////////////
//	//辅助函数
//	float CoordinateToTime(int pt);//屏幕位置到时间的转换
//	int TimeToCoordinate(float fTime);//时间到屏幕位置的转换
//	int IsBounceRegion(POINT& pt);//鼠标是否选中了游标
//	BOOL OnInit();//初始化控件
//	//////////////////////////////////////////////////////////////////////////
//
//	//////////////////////////////////////////////////////////////////////////
//	//绘图相关
//	virtual void DrawButton(CDC* pdc, POINT& pt, DWORD crColor);//渲染游标
//	void GetSystemColorFormat() { };
//	void RenderColorBar(CDC* pdc, enumDrawStyle DrawStyle);//渲染颜色条
//	void ClearRegion(CDC* pDc, RECT& rect);//清除区域
//	//////////////////////////////////////////////////////////////////////////
//
//	//////////////////////////////////////////////////////////////////////////
//	//数据操作
//	int AddKeyColor(float fTime, DWORD dwColor);//在fTime制定位置添加帧，颜色为dwColor
//	int RemoveSelectedKeyColor();//删除当前选定帧
//	void ChangeColor();//改变当前帧的颜色
//	//////////////////////////////////////////////////////////////////////////
//
//
//	//////////////////////////////////////////////////////////////////////////
//	//控件
//	CEdit m_ctlEditChannel[4];
//	CSliderCtrl m_ctlSliderChannel[4];
//	CStatic m_ctlStaticChannel[4];
//	//////////////////////////////////////////////////////////////////////////
//
//	KTimeLine<float, DWORD>* m_pTimeLine;//TimeLine的数据
//	vector<KTimeLine<float, DWORD>::FrameData> m_vecKeys;//保存ＴｉｍｅＬｉｎｅ对应的数据
//
//
//
//	//////////////////////////////////////////////////////////////////////////
//	//布局参数
//	const static int m_nGridSize;//背景的格子大小
//	const static RECT m_rectButtonRgn;//按钮的相应范围
//	const static int m_nSpan;//按钮和彩色条间的距离
//	RECT m_rectClearRgn;//游标的区域
//	RECT m_rectWindow;//窗口的区域
//	RECT m_rectDropRegion;//游标的删除区域
//	RECT m_rectBarRegion;//颜色条的区域
//	RECT m_rectInsertRgn;//可以加入新关键点的区域
//	enumDrawStyle m_DrawStyle;//颜色条的样式
//	//////////////////////////////////////////////////////////////////////////
//
//	//////////////////////////////////////////////////////////////////////////
//	//辅助参数
//	CDC* m_pBackBuffer;//为了防止闪烁，所以先画在这个DC上， 然后bitblt到ＣＰａｉｎｔＤＣ
//	CBitmap* m_pBmpBar;//颜色条的位图Buffer
//	DWORD* m_pColorBarBuffer;//颜色条的Ｂｕｆｆｅｒ
//	CBitmap* m_bmpBackBuffer;
//
//	BOOL m_bDirty;//标示是否有进行改动
//	BOOL m_bVerneirsMove;//标示是否有游标移动需要重画
//	int m_nSelectButton;//当前选中的游标，－１表示没有选中的
//	//////////////////////////////////////////////////////////////////////////
//
//public:
//	DECLARE_MESSAGE_MAP()
//	afx_msg void OnPaint();
//	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
//	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
//	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
//protected:
//	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
//	virtual void PostNcDestroy();
//};
//
//class KColorBarDialog:public CDialog
//{
//public:
//	KColorBarDialog(KTimeLine<float, DWORD>* pTimeLine, CWnd* pParent = NULL);
//	~KColorBarDialog();
//	virtual BOOL OnInitDialog();
//	DECLARE_MESSAGE_MAP()
//protected:
//	virtual void OnCancel();
//	virtual void OnOK();
//private:
//	KTimeLine<float, DWORD>* m_pTimeLine;
//	KTimeLineColorEditBar* m_pColorBar;
//protected:
//	virtual void PostNcDestroy();
//};
