#if !defined(AFX_NSCHARTCTRL_H__54B8AB33_DC07_11D8_9FE8_000021F2B5E0__INCLUDED_)
#define AFX_NSCHARTCTRL_H__54B8AB33_DC07_11D8_9FE8_000021F2B5E0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// NSChartCtrl.h : header file
//
// Written by Ivan Cachicatari (ivancp@viabcp.com)
// Copyright (c) 2004.
//
// This code may be used in compiled form in any way you desire. This
// file may be redistributed unmodified by any means PROVIDING it is 
// not sold for profit without the authors written consent, and 
// providing that this notice and the authors name is included. If 
// the source code in  this file is used in any commercial application 
// then a simple email would be nice.
//
// This file is provided "as is" with no expressed or implied warranty.
// The author accepts no liability if it causes any damage whatsoever.
// It's free - so you get what you pay for.

#include <afxtempl.h>

/////////////////////////////////////////////////////////////////////////////
// CNSChartCtrl window


// NSChart styles
#define NSCS_BAR	0x001 
#define NSCS_PIE	0x002 

#define MAXCHARLABEL	60
//#define NSCS_LINE	0x003 


// Notify codes
#define NSCS_SELECTEDITEM	0x001



typedef struct _NMCHARTCTRL
{
    NMHDR		hdr;
    INT			iItem;
	CHAR		sLabel[MAXCHARLABEL];
	DOUBLE		dValue;
	_NMCHARTCTRL():iItem(-1),dValue(0.0){}
} NMCHARTCTRL, FAR* LPNMCHARTCTRL;

class CNSChartValue : public CObject  
{
public:
	DOUBLE m_dValue;
	CString m_sLabel;

	CNSChartValue():m_dValue(0),m_sLabel(_T("")){};
	virtual ~CNSChartValue(){};

};

typedef CTypedPtrArray <CObArray, CNSChartValue*> CChartValueArray;

class CNSChartCtrl : public CWnd
{
// Construction 

public:
	CNSChartCtrl();
// Attributes
	enum DefaultColors
	{
		EMPY,
		GrayScale,
		SimpleColors
	};
public:
// Operations

public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CNSChartCtrl)
	public:
	//}}AFX_VIRTUAL

// Implementation 
public:	
	bool DeleteItem(INT iItem);
	bool ModifyItem(INT iItem,DOUBLE dValue,CString &str);
	void ResetChart();
	BOOL AddValue(DOUBLE dValue,LPCTSTR strLabel,BOOL bRepaint = FALSE);
	BOOL AddBrush(CBrush *br);
	void ResetColors();
	void SetChartStyle(DWORD dStyle);
	BOOL AddSolidBrush(COLORREF cr);
	void PrepareColors(DefaultColors defaultColor);
	virtual ~CNSChartCtrl();

	// Generated message map functions
protected:
	CChartValueArray m_ChartValues;
	CArray<CBrush*,CBrush*> m_BrushArray;
	CArray<COLORREF,COLORREF> m_BrushCrArray;

	BOOL Notify(UINT nCode, INT iItem);
	BOOL Notify(LPNMCHARTCTRL nm);
	int HitTest(CPoint &pt);
	CFont m_boldFont;
	int m_iCurSel;
	static BOOL RegisterWndClass(HINSTANCE hInstance);
	CFont m_titleFont;
	DOUBLE m_dMax;
	void DrawBarChart(CDC *pDC);
	CFont m_txtFont;
	DOUBLE m_dTotal;
	void DrawPieChart(CDC* pDC);
	DWORD m_dwStyle;
	//{{AFX_MSG(CNSChartCtrl)
	afx_msg void OnPaint();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	CBitmap m_bmpScreen;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_NSCHARTCTRL_H__54B8AB33_DC07_11D8_9FE8_000021F2B5E0__INCLUDED_)
