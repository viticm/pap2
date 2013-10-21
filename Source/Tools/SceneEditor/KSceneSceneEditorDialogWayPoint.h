#pragma once
#include "afxcmn.h"
#include "afxwin.h"
#include "IETools.h"

// KSceneSceneEditorDialogWayPoint dialog
interface IEKG3DSceneSceneEditor;
class KSceneEditorDocLogical;

class KSceneSceneEditorDialogWayPoint : 
	public CDialog,
	public IEKG3DSimpleSink
{
	DECLARE_DYNAMIC(KSceneSceneEditorDialogWayPoint)

	enum ImageType
	{
		image_WayPointSet,
		image_WayPoint,
		image_Npc,
	};
public:
	KSceneSceneEditorDialogWayPoint(CWnd* pParent = NULL);   // standard constructor
	virtual ~KSceneSceneEditorDialogWayPoint();
	void SetSceneEditor(IEKG3DSceneSceneEditor* pSceneEdit)
	{
		m_lpSceneSceneEditor = pSceneEdit;
	}
	void SetDocLogical(KSceneEditorDocLogical* pLogical)
	{
		m_lpDocLogical = pLogical;
	}// Dialog Data
	enum { IDD = IDD_SCENEEDITOR_WAYPOINT };

protected:
	virtual void SinkFunc(INT nSinkID){}
	virtual void SinkFunc2(INT nSinkID,INT nsubSinkID,DWORD dwValue,PVOID pPoint);

	void FillTree();

	IEKG3DSceneSceneEditor*  m_lpSceneSceneEditor;
	KSceneEditorDocLogical* m_lpDocLogical;

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonAdd();
public:
	CTreeCtrl m_Tree;
public:
	afx_msg void OnTvnSelchangedTreeNpc(NMHDR *pNMHDR, LRESULT *pResult);
public:
	afx_msg void OnBnClickedButtonRefresh();
public:
	afx_msg void OnBnClickedButtonAdd2();
public:
	afx_msg void OnEnChangeEditSetname();
	afx_msg void OnEnChangeEditOrderid();
public:
	CEdit m_Edit_SetName;
	CEdit m_Edit_OrderID;
protected:
	virtual void OnOK();
public:
	CEdit m_Edit_StayFrame;
public:
	CEdit m_Edit_StayDirection;
public:
	CEdit m_Edit_StayAni;
public:
	CButton m_Check_AniLoop;
public:
	CButton m_Check_IsRun;
public:
	CButton m_Button_BrownScript;
public:
	CButton m_Button_ClearScript;
public:
	CEdit m_Edit_ScripName;
public:
	CEdit m_Edit_PointInfo;
public:
	afx_msg void OnEnChangeEditStayframe();
public:
	int m_nStayFrame;
public:
	int m_nStayDirection;
public:
	int m_nStayAni;
public:
	BOOL m_bStayAniLoop;
public:
	BOOL m_bIsRun;
public:
	afx_msg void OnEnChangeEditStaydirection();
public:
	afx_msg void OnEnChangeEditStayani();
public:
	afx_msg void OnBnClickedCheckAniloop();
public:
	afx_msg void OnBnClickedCheckIsRun();
public:
	afx_msg void OnEnChangeEditInfo();
public:
	afx_msg void OnEnChangeEditScript();
public:
	afx_msg void OnBnClickedButtonBrownscript();
public:
	afx_msg void OnBnClickedButtonClearscript();
public:
	afx_msg void OnNMDblclkTreeNpc(NMHDR *pNMHDR, LRESULT *pResult);
public:
	virtual BOOL OnInitDialog();
public:
	afx_msg void OnBnClickedButtonDeleteset();
public:
	CButton m_Button_DeleteSet;
public:
	CButton m_Button_AddPoint;
public:
	//int m_nOrderID;
	int m_nPatrolPathOrderID;
	int m_nPatrolPathWalkSpeed;
	afx_msg void OnEnChangeEditOrderid2();
	CComboBox m_Combo_PathOrder;
	afx_msg void OnBnClickedButtonAutoindex();
	afx_msg void OnCbnSelchangeComboPathorder();

	void FillPathOrderCombo(int nID,LPSTR pName);

	afx_msg void OnBnClickedButtonClone();
};
#pragma once

