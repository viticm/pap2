
#pragma once


// KSceneEditorTopViewDialog dialog
interface IEKG3DSceneSceneEditor;


class KSceneEditorTopViewDialog : public CDialog
{
	DECLARE_DYNAMIC(KSceneEditorTopViewDialog)

	IEKG3DSceneSceneEditor* m_lpSceneEditor;
	int m_nOutputWndID;
	CPoint m_LastMousePos;

	CPoint m_MouseLDown;
	D3DXVECTOR3 m_vCameraPos;

	DWORD m_dwSelectionPoint;
	//BOOL  m_bSelecting;
	//BOOL m_bAddPolyVertex;
	enum
	{
		STATE_SELECTING = 1,
		STATE_ADDPOLYVERTEX = 2,
		STATE_SELECTLOGICREGION=3,
		STATE_NONE = 1000,
	};
	DWORD m_eState;
public:
	void SetSceneEditor(IEKG3DSceneSceneEditor* pSceneEditor)
	{
		m_lpSceneEditor = pSceneEditor;
	}

	KSceneEditorTopViewDialog(CWnd* pParent = NULL);   // standard constructor
	virtual ~KSceneEditorTopViewDialog();

// Dialog Data
	enum { IDD = IDD_SCENEEDITORTOPVIEW };

    void SetSelState();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
    afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
public:
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
public:
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
public:
	afx_msg void OnSize(UINT nType, int cx, int cy);
public:
	afx_msg void OnMButtonDown(UINT nFlags, CPoint point);
protected:
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
public:
	afx_msg void OnSetFocus(CWnd* pOldWnd);
public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
public:
	afx_msg void OnTopviewGround();
public:
	afx_msg void OnUpdateTopviewGround(CCmdUI *pCmdUI);
public:
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
public:
	afx_msg void OnClose();
public:
	afx_msg void OnTopviewEntityfar();
public:
	afx_msg void OnTopviewWater();
public:
	afx_msg void OnTopviewlightmap();
public:
	afx_msg void OnTopviewGroundgird();
public:
	afx_msg void OnTopviewRegionSelection();
public:
	afx_msg void OnNewpoly();
	void MousePosToScene(float& X,float& Z,CPoint point);
	afx_msg void OnEndaddpoly();
	afx_msg void OnInitMenuPopup(CMenu* pPopupMenu, UINT nIndex, BOOL bSysMenu);
	afx_msg void OnUpdateTopviewlightmap(CCmdUI *pCmdUI);
	afx_msg void OnUpdateTopviewGroundgird(CCmdUI *pCmdUI);
	afx_msg void OnUpdateTopviewWater(CCmdUI *pCmdUI);
	afx_msg void OnUpdateTopviewEntityfar(CCmdUI *pCmdUI);
	afx_msg void OnFoceloadall();

	CButton m_Button_View;
	BOOL m_bEnableSelectZone;
	afx_msg void OnBnClickedCheckZone();
	BOOL m_bEnableSelectSection;
	BOOL m_bEnableSelectRegion;
	BOOL m_bEnableOnlyExist;
	afx_msg void OnBnClickedCheckSection();
	afx_msg void OnBnClickedCheckRegion();
	afx_msg void OnBnClickedCheckExistonly();
	afx_msg void OnBnClickedButtonBlockselect();
	afx_msg void OnBnClickedButtonBlockcreate();
	afx_msg void OnBnClickedButtonBlockdelete();
	afx_msg void OnBnClickedButtonTerraincreate();
	afx_msg void OnBnClickedButtonTerraindelete();
	afx_msg void OnEnChangeEditHeightmap();
	afx_msg void OnEnChangeEditHeightwidth();
	CString m_szHeightmapName;
	int m_nHeightMapWidth;
	float m_fHeightLowest;
	float m_fHeightTotal;
	afx_msg void OnEnChangeEditLow();
	afx_msg void OnEnChangeEditHeight();
	afx_msg void OnBnClickedButtonApplyheightmap();
	CString m_szBaseTexture;
	CString m_szHeightColor;
	afx_msg void OnBnClickedButtonApplytext();
	afx_msg void OnBnClickedButtonExportheight();
	afx_msg void OnBnClickedButtonImportheight();
	afx_msg void OnBnClickedCheckShowappointlogicalregion();

	int m_nRegionXStart;
	int m_nRegionZStart;
	int m_nCountRegionX;
	int m_nCountRegionZ;
	void UpdataAppiontLogicalRegion();
	afx_msg void OnEnChangeEditLrstartx();
	afx_msg void OnEnChangeEditLrstartz();
	afx_msg void OnEnChangeEditLrcountx();
	afx_msg void OnEnChangeEditLrcountz();
	afx_msg void OnBnClickedSelectLogicRegion();
	
	afx_msg void OnBnClickedCheckBox();
};
