#if !defined(AFX_KSCENEMODELEDITORSFXSCENESFXDIALOG_H__58552567_8BDB_4E5E_B571_534821A21743__INCLUDED_)
#define AFX_KSCENEMODELEDITORSFXSCENESFXDIALOG_H__58552567_8BDB_4E5E_B571_534821A21743__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// KSceneModelEditorSFXSceneSFXDialog.h : header file
//

/*
struct BBHeader
{
	int m_iWhichBB;
	DWORD	m_dwMaxParticle;
	int m_iMaxWidth,m_iMinWidth;
	int m_iMaxHeight,m_iMinHeight; 
	D3DXVECTOR3 m_fInitSpeed,m_v3AccSpeed;
	D3DXVECTOR3 m_v3InitMinPos,m_v3InitMaxPos;
};*/


/////////////////////////////////////////////////////////////////////////////
// KSceneModelEditorSFXSceneSFXDialog dialog
class KModelSceneSFX;
struct BBHeader;

class KSceneModelEditorSFXSceneSFXDialog : public CDialog
{
// Construction
public:
	KSceneModelEditorSFXSceneSFXDialog(CWnd* pParent = NULL);   // standard constructor
	/*void SetSceneSFX(KModelSceneSFX* pSFX)
	{
		m_lpSceneSFX = pSFX;
	}*/

// Dialog Data
	//{{AFX_DATA(KSceneModelEditorSFXSceneSFXDialog)
	enum { IDD = IDD_SFX_SCENESFX };
	
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(KSceneModelEditorSFXSceneSFXDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//KModelSceneSFX* m_lpSceneSFX;

	CDataExchange* m_pde;

	// Generated message map functions
	//{{AFX_MSG(KSceneModelEditorSFXSceneSFXDialog)
	afx_msg void OnButtonSetmaxpartcle();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:

	DWORD m_dwMaxParticle;
	float m_fYAccSpeed;
	float m_fYSpeed;
	float m_fAlphaBase;
	afx_msg void SetAccSpeed();
	int m_iWhichBB;
	afx_msg void OnEnChangeEdit2();
	afx_msg void OnSetWichBB();
	float m_fXAccSpeed;
//	afx_msg void SetXAccSpeed();
	int m_iWidth,m_iMaxWidth,m_iMinWidth;
	int m_iHeight,m_iMaxHeight,m_iMinHeight;
//	afx_msg void SetBBWidth();
//	afx_msg void SetBBHeight();

	afx_msg void SetBBWidth();
//	afx_msg void SetBBMinHeight();
//	afx_msg void SetBBMaxWidth();
//	afx_msg void SetBBMaxHeight();
	afx_msg void SetBBHeight();
//	afx_msg void SetBBMaxHeight();
	float m_fZAccSpeed;
	afx_msg void OnEnChangeEdit9();

	D3DXVECTOR3 m_fInitSpeed;
	D3DXVECTOR3 m_v3InitMinPos,m_v3InitMaxPos;
	afx_msg void SetInitSpeed();
	afx_msg void SetInitMinPos();
	afx_msg void SetInitMaxPos();
	afx_msg void LoadSSFXFile();
	afx_msg void SaveSSFXFile();

	void SetData(const BBHeader &bbheader);
	void InPutdata(BBHeader &bbheader);
	afx_msg void SetAlphaBase();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_KSCENEMODELEDITORSFXSCENESFXDIALOG_H__58552567_8BDB_4E5E_B571_534821A21743__INCLUDED_)
