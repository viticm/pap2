#pragma once

#include "KSceneEditorDocLogical.h"
#include "afxwin.h"
// KGTrafficPropertyEditerDlg dialog

class KGTrafficPropertyEditerDlg : public CDialog
{
	DECLARE_DYNAMIC(KGTrafficPropertyEditerDlg)

public:
	KGTrafficPropertyEditerDlg(IEKG3DScene* lpEditScene,KSceneEditorDocLogical* pDocLogical,CWnd* pParent = NULL);   // standard constructor
	virtual ~KGTrafficPropertyEditerDlg();

// Dialog Data
	enum { IDD = IDD_DIALOG_TRAFFICPOINT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support	
	DECLARE_MESSAGE_MAP()
public:
	CEdit editDescription;
	CEdit editNodeID;
	CButton checkNeedOpen;
	CEdit editMapID;
	CEdit editCityID;
	CEdit editForceID;
	CEdit editTongID;
	CEdit editScriptFile;
	afx_msg void OnBnClickedButton3();
	BOOL OnInitDialog();
	void FillOverMapList(DWORD dwID);
private:
	KSceneEditorDocLogical* m_lpDocLogical;
	IEKG3DScene*	m_lpEditScene;
	DWORD m_dwNodeID;
public:
	afx_msg void OnBnClickedOk();
	CComboBox m_ListMapPoints;
	afx_msg void OnBnClickedNeedopne();
	afx_msg void OnBnClickedButton1();
	CButton buttonChange;
	CButton buttonIsOverMap;
	CListBox listTrack;
	CButton buttonDelete;
	CEdit editTrackID;
	CEdit editCostMoney;
	CEdit editVelocity;
	CButton buttonEdit;
	afx_msg void OnBnClickedCheckovermap();
	afx_msg void OnBnClickedButtondelete();
	afx_msg void OnBnClickedButtonedit();
	afx_msg void OnChangedListtrack();
	CEdit editByWhat;
};
