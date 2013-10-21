#if !defined(AFX_KSCENEMODELEDITORSFXBLADEDIALOG_H__67AEDB62_28FB_42C7_9873_704CD4C49BC5__INCLUDED_)
#define AFX_KSCENEMODELEDITORSFXBLADEDIALOG_H__67AEDB62_28FB_42C7_9873_704CD4C49BC5__INCLUDED_

//#include "MeshBlade.h"

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// KSceneModelEditorSFXBladeDialog.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// KSceneModelEditorSFXBladeDialog dialog

class KSceneModelEditorSFXBladeDialog : public CDialog
{
// Construction
public:
	KSceneModelEditorSFXBladeDialog(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(KSceneModelEditorSFXBladeDialog)
	enum { IDD = IDD_SFX_BLADE };
	CButton	m_Check_SelectFrame;
	CSliderCtrl	m_Slider_Time;
	CListBox	m_ListBox_Ani;
	CComboBox	m_Combo_Socket;
	CListBox	m_ListBox_Blade;
	float	m_fTotalLength;
	float	m_fSampleLength;
	float	m_A_X;
	float	m_A_Y;
	float	m_A_Z;
	float	m_B_X;
	float	m_B_Y;
	float	m_B_Z;
	float	m_fPlaySpeed;
	BOOL	m_bSelectFrame;
	float	m_fSetSelectLength;
	CString	m_CString_CurFrame;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(KSceneModelEditorSFXBladeDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
public:
	/*void SetBlade(KSceneModelEditor* pModelEditor,ModelSFX::KModelSFXBlade* pBlade)
	{
		m_lpModelEditor = pModelEditor;
		m_lpBlade = pBlade;
	}*/
protected:
	int                       m_nSelectStart;
	//KSceneModelEditor*        m_lpModelEditor;
	//ModelSFX::KModelSFXBlade* m_lpBlade;
	//LPMODEL                   m_lpModelWeapon;


	// Generated message map functions
	//{{AFX_MSG(KSceneModelEditorSFXBladeDialog)
	virtual BOOL OnInitDialog();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnPaint();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	void FillListBoxBlade();
	void FillListBoxAni();
	//void FillComboSocket(LPMODEL pModel);

public:
	afx_msg void OnLbnSelchangeList1();
	afx_msg void OnLbnSelchangeListBlade();
	afx_msg void OnEnChangeEditSamplelength();
	afx_msg void OnEnChangeEditTotallength();
	afx_msg void OnEnChangeEditAX();
	afx_msg void OnEnChangeEditAY();
	afx_msg void OnEnChangeEditAZ();
	afx_msg void OnEnChangeEditBX();
	afx_msg void OnEnChangeEditBY();
	afx_msg void OnEnChangeEditBZ();
	afx_msg void OnBnClickedButtonBladenew();
	afx_msg void OnBnClickedButtonBladesave();
	afx_msg void OnBnClickedButtonBladeload();
	afx_msg void OnBnClickedButton1();
	afx_msg void OnCbnSelchangeComboSocket();
	afx_msg void OnBnClickedButtonAutoani();
	afx_msg void OnLbnSelchangeListAni();
	afx_msg void OnBnClickedButtonLoadani();
	afx_msg void OnBnClickedButtonAniplay();
	afx_msg void OnNMCustomdrawSlider4(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnEnChangeEditPlayspeed();
	afx_msg void OnNMCustomdrawSliderTime(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedCheckFrameselect();
	afx_msg void OnBnClickedButtonHideselect();
	afx_msg void OnBnClickedButtonShowselect();
	afx_msg void OnBnClickedButtonSetselectlength();
	afx_msg void OnBnClickedButtonRefresh();
	afx_msg void OnBnClickedButtonAnilaod();
	afx_msg void OnBnClickedButtonAnisave();
	afx_msg void OnBnClickedButtonAninew();
	afx_msg void OnBnClickedButtonAnidelete();
	afx_msg void OnBnClickedButtonLoadweapon();
	afx_msg void OnBnClickedButtonAutoani2();
	afx_msg void OnEnChangeEditSelectlength();
	afx_msg void OnBnClickedButtonHideselect2();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_KSCENEMODELEDITORSFXBLADEDIALOG_H__67AEDB62_28FB_42C7_9873_704CD4C49BC5__INCLUDED_)
