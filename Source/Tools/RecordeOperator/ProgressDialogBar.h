#pragma once


// CProgressDialogBar dialog

class CProgressDialogBar : public CDialogBar
{
	DECLARE_DYNAMIC(CProgressDialogBar)

public:
	CProgressDialogBar();   // standard constructor
	virtual ~CProgressDialogBar();
	void SetPorgressRange(int nMin,int nMax,BOOL bRedraw = FALSE);
	void SetProgressPos(int nPos);
	//virtual CSize CalcDynamicLayout(int nLength, DWORD nMode);
	CButton m_buttonPlay;
	CButton m_buttonStop;
	CButton m_buttonFreeCamera;
	
// Dialog Data
	enum { IDD = IDD_PROGRESS_DIALOGBAR };


	CSliderCtrl m_sliderProgress;
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	afx_msg LONG OnInitDialog ( UINT, LONG ); 
	afx_msg void OnHScroll(UINT nSBCode,UINT nPos,CScrollBar* pScrollBar);
	DECLARE_MESSAGE_MAP()
};
