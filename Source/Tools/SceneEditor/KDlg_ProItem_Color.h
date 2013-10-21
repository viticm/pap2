#if !defined(AFX_KDLG_PROITEM_COLOR_H__1435ED50_64BB_458C_B0F4_1A68D7ADD650__INCLUDED_)
#define AFX_KDLG_PROITEM_COLOR_H__1435ED50_64BB_458C_B0F4_1A68D7ADD650__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// KDlg_ProItem_Color.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// KDlg_ProItem_Color dialog
#include "KColorButton.h"
namespace PropertyBase
{
class KDlg_ProItem_Color : public CDialog
{
// Construction
public:
	//PropertyBase::_VarProperty* m_lpProperty;

	//void SetStore(PropertyBase::_VarProperty* pProperty)
	//{
	//	m_lpProperty = pProperty;
	//}

	//void RefreshData()
	//{
	//	if(m_lpProperty)
	//	{
	//		//m_lpStore->pStr = m_Value;
	//		/*if(m_lpProperty->Type==PropertyBase::_VarProperty::VAR_STRING)
	//		{
	//			LPSTR pStr = (LPSTR) m_lpProperty->Address;
	//			wsprintf(pStr,"%s",m_Value);
	//		}
	//		else if(m_lpProperty->Type==PropertyBase::_VarProperty::VAR_STLSTRING)
	//		{
	//			string* pStr = (string*) m_lpProperty->Address;
	//			(*pStr) = m_Value;
	//		}*/
	//	}
	//}

	void SetColor(D3DCOLOR* pColor)
	{
		DWORD A = (*pColor&0xFF000000)>>24;
		DWORD R = (*pColor&0x00FF0000)>>16;
		DWORD G = (*pColor&0x0000FF00)>>8;
		DWORD B = (*pColor&0x000000FF);

		m_dwColor = RGB(R,G,B);
	}

	void SetColor(D3DCOLORVALUE* pColor)
	{
		DWORD A = (DWORD)(pColor->a*255);
		DWORD R = (DWORD)(pColor->r*255);
		DWORD G = (DWORD)(pColor->g*255);
		DWORD B = (DWORD)(pColor->b*255);

		m_dwColor = RGB(R,G,B);
	}

	D3DCOLOR GetColor()
	{
		DWORD B = (m_dwColor&0x00FF0000)>>16;
		DWORD G = (m_dwColor&0x0000FF00)>>8;
		DWORD R = (m_dwColor&0x000000FF);

		D3DCOLOR Color = D3DCOLOR_ARGB(255,R,G,B);
		return Color;
	}

	D3DCOLORVALUE GetColor(D3DCOLORVALUE* p)//*p 只是用来区别两个函数
	{
		DWORD B = (m_dwColor&0x00FF0000)>>16;
		DWORD G = (m_dwColor&0x0000FF00)>>8;
		DWORD R = (m_dwColor&0x000000FF);

		D3DCOLORVALUE Color;
		Color.a = 1.0f;
		Color.r = R/255.0f;
		Color.g = G/255.0f;
		Color.b = B/255.0f;

		return Color;
	}

	KDlg_ProItem_Color(CWnd* pParent = NULL);   // standard constructor

	
// Dialog Data
	//{{AFX_DATA(KDlg_ProItem_Color)
	enum { IDD = IDD_PROITEM_COLOR };
	KColorButton	m_Color;
	CString	m_Name;
	//}}AFX_DATA

	DWORD m_dwColor;
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(KDlg_ProItem_Color)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(KDlg_ProItem_Color)
	virtual BOOL OnInitDialog();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnButton1();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
};
//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_KDLG_PROITEM_COLOR_H__1435ED50_64BB_458C_B0F4_1A68D7ADD650__INCLUDED_)
