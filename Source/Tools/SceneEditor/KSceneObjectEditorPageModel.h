#if !defined(AFX_KSCENEOBJECTEDITORPAGEMODEL_H__BE05C084_23C8_4AAE_83C3_E26D2ADECCB5__INCLUDED_)
#define AFX_KSCENEOBJECTEDITORPAGEMODEL_H__BE05C084_23C8_4AAE_83C3_E26D2ADECCB5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// KSceneObjectEditorPageModel.h : header file
//
class KSceneObjectEditorDoc;
/////////////////////////////////////////////////////////////////////////////
// KSceneObjectEditorPageModel : Property page dialog

class KSceneObjectEditorPageModel : public CPropertyPage
{
	DECLARE_DYNCREATE(KSceneObjectEditorPageModel)

// Constructors
public:
	DWORD m_dwBarrier;

	KSceneObjectEditorPageModel();

	KSceneObjectEditorDoc*  m_lpSceneObjectEditorDoc;

	void SetSceneObjectEditorDoc(KSceneObjectEditorDoc* pScene)
	{
		m_lpSceneObjectEditorDoc = pScene;
	}
// Dialog Data
	//{{AFX_DATA(KSceneObjectEditorPageModel)
	enum { IDD = IDD_OBJECTEDITOR_MODEL };
	CButton	m_Button_Scale;
	CButton	m_Button_Rotate;
	CButton	m_Button_Move;
	CEdit	m_Edit_HeightDown;
	CButton	m_Button_HeightDown_Down;
	CButton	m_Button_HeightDown_Set;
	CButton	m_Button_HeightDown_Up;
	CButton	m_Static_HeightDown;
	CButton	m_Static_Barrier;
	CButton	m_Button_Barrierset;
	CButton	m_Button_BarrierRemove;
	CButton	m_Button_BarrierAdd;
	CButton	m_Button_Bullet;
	CButton	m_Button_Jump;
	CButton	m_Button_Place;
	CButton	m_Button_Walk;
	CListBox	m_ListBoxModelName;
	BOOL	m_bShowBarrier;
	BOOL	m_bShowHeightUp;
	BOOL	m_bShowHeightDown;
	BOOL	m_bShowObject;
	BOOL	m_bBarrierWalk;
	BOOL	m_bBarrierJump;
	BOOL	m_bBarrierPlaceObject;
	BOOL	m_bBarrierPassType;
	BOOL	m_bBarrierGroundTexture;
	BOOL	m_bBarrierBullet;
	BOOL	m_bBarrierSet;
	BOOL	m_bBarrierAdd;
	BOOL	m_bBarrierRemove;
	float	m_fHeightValue;
	BOOL	m_bScrabble_Snap2Grid;
	BOOL	m_bScrabble_Water;
	BOOL	m_bScrabble_Vertical;
	BOOL	m_bScrabble_RunTime;
	BOOL	m_bScrabble_PitchOnly;
	BOOL	m_bScrabble_NoOnBarrier;
	BOOL	m_bScrabble_ModifySelf;
	BOOL	m_bScrabble_ModifyGround;
	BOOL	m_bScaleX;
	BOOL	m_bScaleZ;
	//}}AFX_DATA

// Implementation
protected:
	void  UnCheckOperatorState();
	void  EnableBarrierInfo(BOOL Value);
	void  EnableHeightDown(BOOL Value);
	virtual void DoDataExchange(CDataExchange* pDX);        // DDX/DDV support
	void GetCurBarrier();
// Message maps
protected:
	//{{AFX_MSG(KSceneObjectEditorPageModel)
	afx_msg void OnButtonAddmodel();
	afx_msg void OnButtonDeletemodel();
	afx_msg void OnCheckMove();
	afx_msg void OnCheckRotate();
	afx_msg void OnCheckScale();
	afx_msg void OnSelchangeList1();
	afx_msg void OnButtonCellinfo();
	afx_msg void OnButtonBuildcell();
	afx_msg void OnShowHeightDown();
	afx_msg void OnShowHeightUp();
	afx_msg void OnShowBarrier();
	afx_msg void OnShowobject();
	virtual BOOL OnInitDialog();
	afx_msg void OnCwalk();
	afx_msg void OnCbullet();
	afx_msg void OnCjump();
	afx_msg void OnCplace();
	afx_msg void OnCbarrieradd();
	afx_msg void OnCbarrierremove();
	afx_msg void OnCbarrierset();
	afx_msg void OnCheckHeightdownDown();
	afx_msg void OnCheckHeightdownSet();
	afx_msg void OnCheckHeightdownUp();
	afx_msg void OnChangeEditHeightdown();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnScrabbleSnap2grid();
	afx_msg void OnScrabbleVertical();
	afx_msg void OnScrabblePitch();
	afx_msg void OnScrabbleModifyground();
	afx_msg void OnScrabbleModifyself();
	afx_msg void OnScrabbleNoonbarrier();
	afx_msg void OnScrabbleRuntime();
	afx_msg void OnScrabbleWater();
	afx_msg void OnButtonSnapx();
	afx_msg void OnButtonSnapz();
	afx_msg void OnCheckScalex();
	afx_msg void OnCheckScalez();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	void  FillListBox();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_KSCENEOBJECTEDITORPAGEMODEL_H__BE05C084_23C8_4AAE_83C3_E26D2ADECCB5__INCLUDED_)
