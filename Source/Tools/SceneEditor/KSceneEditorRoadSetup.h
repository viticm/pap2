#pragma once
#include "afxwin.h"
#include "afxcmn.h"



// KSceneEditorRoadSetup dialog
class IEKG3DSceneSceneEditor;
//class IEKG3DTerrain;
class IEKG3DSceneOutDoorSpaceMgr;
class IEKG3DTerrainRoadMgr;
class KSceneEditorRoadSetup : public CDialog ,public IEKG3DSimpleSink

{
	DECLARE_DYNAMIC(KSceneEditorRoadSetup)

public:
	KSceneEditorRoadSetup(CWnd* pParent = NULL);   // standard constructor
	virtual ~KSceneEditorRoadSetup();

	IEKG3DSceneSceneEditor* m_lpEditor;
	//IEKG3DTerrain* m_lpTerrain;
	IEKG3DSceneOutDoorSpaceMgr* m_lpOutDoorSpaceMgr;
	IEKG3DTerrainRoadMgr* m_lpRoadMgr;
	//void SetEditor(IEKG3DSceneSceneEditor* pEditor,IEKG3DTerrain* pTerrain)
	//{
	//	m_lpEditor = pEditor;
	//	m_lpTerrain = pTerrain;
	//}
	void SetEditor(IEKG3DSceneSceneEditor* pEditor,IEKG3DSceneOutDoorSpaceMgr* pOutDoorMgr);
	
// Dialog Data
	enum { IDD = IDD_SCENEEDITOR_ROADSETUP };

protected:
	void FillList();
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual void SinkFunc(INT nSinkID){}
	virtual void SinkFunc2(INT nSinkID,INT nsubSinkID,DWORD dwValue,PVOID pPoint);
	virtual BOOL OnInitDialog();

	CListBox m_List;
	float m_fRoadWidth;
	float m_fLastRoadWidth;
	int m_nSegmentLength;
	int m_nLastSegmentLength;
	int m_nModulusK;
	int m_nLastModulusK;
	int m_nEdge;
	int m_nLastEdge;
	float m_fTransWidth;
	float m_fLastTransWidth;
	int m_nTexDensity;
	int m_nLastTexDensity;
	CSliderCtrl m_SliderSL;
	CSliderCtrl m_SliderNode;
	CString m_CStringSL;
	int m_nID;
	BOOL m_bShowNode;
	BOOL m_bShowLine;
	int m_nNodeSize;
	CSliderCtrl m_SliderK;
	CString m_CStringK;
	CString m_strName;
	CString m_strLastName;
	CString m_CStringEdge;
	CString m_CStringNodeSize;
	CEdit m_EditReName;
	float m_fViewDistance;
	

	afx_msg void OnBnClickedNewroad();
	afx_msg void OnBnClickedNewnode();
	afx_msg void OnLbnSelchangeList1();
	afx_msg void OnBnClickedRefresh();
	afx_msg void OnBnClickedFlatroad();
	afx_msg void OnBnClickedHeightTerraintoroad();
	afx_msg void OnBnClickedDeleteroad();

	afx_msg void OnBnClickedBunRefresh();
	afx_msg void OnBnClickedCheckShowline();
	afx_msg void OnBnClickedButtonTexture();
	afx_msg void OnEnKillfocusEditRename();
	afx_msg void OnBnClickedButtonPre();
	afx_msg void OnBnClickedButtonNext();
	
	afx_msg void OnEnKillfocusEditTexDensity();
	afx_msg void OnEnKillfocusEditViewDistance();
	afx_msg void OnEnKillfocusEditTransWidth();
	afx_msg void OnEnKillfocusEdit1();
	
	afx_msg void OnNMCustomdrawSliderEdge(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMCustomdrawSliderK(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMCustomdrawSliderNodesize(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedCheckShownode();
	afx_msg void OnNMReleasedcaptureSliderK(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMReleasedcaptureSliderEdge(NMHDR *pNMHDR, LRESULT *pResult);	
	afx_msg void OnNMReleasedcaptureSliderSl(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMCustomdrawSliderSl(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnEnKillfocusEditId();
	afx_msg void OnBnClickedRefreshList();
};
