#pragma once

class IEKG3DSceneSceneEditor;
class IEKG3DSceneOutDoorSpaceMgr;
class IEKG3DTerrainRiverMgr;
// KSceneEditorRiver dialog

class KSceneEditorRiver : public CDialog  ,public IEKG3DSimpleSink
{
	DECLARE_DYNAMIC(KSceneEditorRiver)

public:
	KSceneEditorRiver(CWnd* pParent = NULL);   // standard constructor
	virtual ~KSceneEditorRiver();
	
	IEKG3DSceneSceneEditor* m_lpEditor;
	IEKG3DSceneOutDoorSpaceMgr* m_lpOutDoorSpaceMgr;
	IEKG3DTerrainRiverMgr* m_lpRiverMgr;
	void SetEditor(IEKG3DSceneSceneEditor* pEditor,IEKG3DSceneOutDoorSpaceMgr* pOutDoorMgr);
// Dialog Data
	enum { IDD = IDD_SCENEEDITOR_RIVER };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual void SinkFunc(INT nSinkID){}
	virtual void SinkFunc2(INT nSinkID,INT nsubSinkID,DWORD dwValue,PVOID pPoint);
	virtual BOOL OnInitDialog();
public:
	CListBox m_List;
	CString m_strName;
	CString m_strLastName;
	int m_nRiverWidth;
	int m_nLastRiverWidth;
	int m_nSegmentLength; 
	int m_nLastSegmentLength;
	int m_nModulusK;
	int m_nLastModulusK;
	int m_nRiverDepth;
	int m_nLastRiverDepth;
	CSliderCtrl m_SliderSL;
	CString m_CStringSL;
	CString m_CStringK;
	CSliderCtrl m_SliderK;
	BOOL m_bShowSelectMark;
	BOOL m_bShowLineEx;
	void FillList();
	afx_msg void OnBnClickedNewriver();
	afx_msg void OnLbnSelchangeList1();
	afx_msg void OnBnClickedAddNode();
	afx_msg void OnBnClickedMakeRiver();
	afx_msg void OnBnClickedDelriver();
	afx_msg void OnNMCustomdrawSliderK(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMCustomdrawSliderSl(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMReleasedcaptureSliderK(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMReleasedcaptureSliderSl(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnEnKillfocusEditDepth();
	afx_msg void OnEnKillfocusEditWidth();
	afx_msg void OnEnKillfocusEditRename();
	afx_msg void OnBnClickedCheckShowline();
	afx_msg void OnBnClickedUpdateTerrainheight();
	afx_msg void OnBnClickedCheckShowlineex();
	afx_msg void OnBnClickedComputeAlpha();
	afx_msg void OnBnClickedRefreshList();
	afx_msg void OnBnClickedRefreshShader();
};
