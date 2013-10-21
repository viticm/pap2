#pragma once
#include "afxcmn.h"
#include "afxwin.h"


// KSceneSceneEditorDialogTrafficPoint dialog
interface IEKG3DSceneSceneEditor;
class KSceneEditorDocLogical;
class KSceneSceneEditorDialogTrafficPoint : 
	public CDialog,
	public IEKG3DSimpleSink
{
	DECLARE_DYNAMIC(KSceneSceneEditorDialogTrafficPoint)

public:
	KSceneSceneEditorDialogTrafficPoint(CWnd* pParent = NULL);   // standard constructor
	virtual ~KSceneSceneEditorDialogTrafficPoint();
	
	void SetSceneEditor(IEKG3DSceneSceneEditor* pSceneEdit)
	{
		m_lpSceneSceneEditor = pSceneEdit;
	}
	void SetDocLogical(KSceneEditorDocLogical* pLogical)
	{
		m_lpDocLogical = pLogical;
	}// Dialog Data
	virtual void SinkFunc(INT nSinkID){}
	virtual void SinkFunc2(INT nSinkID,INT nsubSinkID,DWORD dwValue,PVOID pPoint);
// Dialog Data
	enum { IDD = IDD_SCENEEDITOR_TRAFFICPOINT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	IEKG3DSceneSceneEditor*  m_lpSceneSceneEditor;
	KSceneEditorDocLogical* m_lpDocLogical;
	void FillTree();
public:
	virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonAddTrafficpoint();
	afx_msg void OnBnClickedButton1();
	CTreeCtrl m_Tree;
	afx_msg void OnBnClickedButtonRefresh();
	afx_msg void OnBnClickedButtonAddtrafficpointset();
	afx_msg void OnBnClickedButtonDeletetrafficpoint();
	CEdit m_editCostMoney;
	CEdit m_editVelocity;
	CEdit m_editStep;
	//afx_msg void OnEnKillfocusBigr();
	//afx_msg void OnEnKillfocusSmallr();
	afx_msg void OnTvnSelchangedTreeNpc(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnTvnSelchangingTreeNpc(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMDblclkTreeNpc(NMHDR *pNMHDR, LRESULT *pResult);
	CEdit editName;
	CEdit editMapID;
	CEdit editForceID;
	CEdit editCityID;
	CEdit editTongID;
	afx_msg void OnBnClickedButton7();
	CButton buttonUpdate;
	afx_msg void OnEnChangeEdit4();
	afx_msg void OnEnChangeEdit1();
	afx_msg void OnEnChangeEditVelocity();
	afx_msg void OnEnChangeEditStep();
	afx_msg void OnBnClickedButtonaddlittlepoint();
};
