
#ifndef _KGCONTROLCOLUMNDLG_H_
#define _KGCONTROLCOLUMNDLG_H_

#include "resource.h"

#define ANI_COUNT_MAX 100

class KG3DObjectEditor;

enum SHOW_MODEL
{
	SHOW_MODEL_FLOAT,
	SHOW_MODEL_BIND,
	SHOW_MODEL_HIDE
};

enum PLAY_STATE
{
	PLAY_STATE_PAUSE,
	PLAY_STATE_STOP,
	PLAY_STATE_PALY
};

class KGControlColumnDlg : public CDialog
{
	DECLARE_DYNAMIC(KGControlColumnDlg)

public :
	int BandData(ITabFile** ppTabFile, IIniFile** ppIniFile);
	SHOW_MODEL GetShowMode();
	int	SetShowMode(SHOW_MODEL eShowMode);
	int Update(int nCurrentRow);
	
public :
	KGControlColumnDlg(CWnd* pParent = NULL);  
	virtual ~KGControlColumnDlg();
	enum { IDD = IDD_DIALOG_CONTROL_PANEL };
	BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);

public :
	KG3DObjectEditor* m_pParentWindow;
	SHOW_MODEL        m_eShowMode;
	ITabFile**	      m_ppTabFile;			
	IIniFile**		  m_ppIniFile;

	int				  m_nCurrentRow;
	CComboBox		  m_comboxAniList;
	CComboBox		  m_comboxModList;
	CSliderCtrl		  m_sliderSpeed;
	char			  m_szAinFieldName[ANI_COUNT_MAX][64];
	char			  m_szModFieldName[ANI_COUNT_MAX][64];

	float             m_fAniSpeed;
	CString			  m_strFrameTime;

	PLAY_STATE	      m_ePlayState;

	CToolTipCtrl	  m_toolTipCtrl;

public :
	virtual void DoDataExchange(CDataExchange* pDX);    
	afx_msg void OnNcLButtonDblClk(UINT nHitTest, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg LRESULT OnNcHitTest(CPoint point);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnCbnAniSelect();
	afx_msg void OnCbnModSelect();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnOK();
	afx_msg void OnCancel();
	afx_msg void OnEdit();
	afx_msg void OnMunisAniSpeed();
	afx_msg void OnAddAniSpeed();
	afx_msg void OnAniPause();
	afx_msg void OnAniStop();
	afx_msg void OnAniPlay();
	afx_msg void OnBnClickedButtonSave();
	afx_msg void OnEnChangeFrame();

	DECLARE_MESSAGE_MAP()
};

#endif // _KGCONTROLCOLUMNDLG_H_