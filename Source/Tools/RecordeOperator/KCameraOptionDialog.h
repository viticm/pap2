#pragma once
#include "IRecPlayer.h"
#include "afxwin.h"
// KCameraOptionDialog dialog

class KCameraOptionDialog : public CDialog
{
	DECLARE_DYNAMIC(KCameraOptionDialog)

public:
	KCameraOptionDialog(IERecPlayer* pPlayer,CWnd* pParent = NULL);   // standard constructor
	virtual ~KCameraOptionDialog();
	
// Dialog Data
	enum { IDD = IDD_DIALOG_CAMERAOPTION };

protected:
	IERecPlayer* m_pPlayer;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	
	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedOk();
	int m_nCameraType;
	CEdit m_editBindID;
	CEdit m_editAngle;
	int m_nFreeCamera;
};
