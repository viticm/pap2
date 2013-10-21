#include "afxwin.h"
#if !defined(AFX_KSCENESCENEEDITORDIALOGCAMERA_H__09916E03_1AB2_408C_A53C_4273F1C7BE9E__INCLUDED_)
#define AFX_KSCENESCENEEDITORDIALOGCAMERA_H__09916E03_1AB2_408C_A53C_4273F1C7BE9E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

interface IEKG3DSceneSceneEditor;//by dengzhihui Refactor 2006年11月27日
/////////////////////////////////////////////////////////////////////////////
// KSceneSceneEditorDialogCamera dialog

class KSceneSceneEditorDialogCamera : public CDialog
{
// Construction
public:
	KSceneSceneEditorDialogCamera(CWnd* pParent = NULL);   // standard constructor

	enum { IDD = IDD_SCENEEDITOR_CAMERA };
	
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void OnOK();

	DECLARE_MESSAGE_MAP()
protected:
	IEKG3DSceneSceneEditor*  m_lpSceneSceneEditor;
public:
	virtual BOOL DestroyWindow();
	virtual BOOL OnInitDialog();

	void SetSceneEditor(IEKG3DSceneSceneEditor* pSceneEdit){m_lpSceneSceneEditor = pSceneEdit;}
	
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnEnChangeEditZfar();
	
	afx_msg void OnBnClickedCheckEditkeyframe();
	afx_msg void OnBnClickedButtonClear();

	afx_msg void OnBnClickedButtonPlaySelAni();
	afx_msg void OnBnClickedButtonStopSelAni();

	afx_msg void OnBnClickedButton1();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	
	afx_msg void OnEnChangeEdPlayspeed();
	afx_msg void OnEnChangeEdPlayAngle();

	afx_msg void OnBnClickedButtonNewAni();
	afx_msg void OnBnClickedButtonRenameSelAni();
	afx_msg void OnCbnSelchangeComboCamaniList();
	afx_msg void OnBnClickedButtonClearSelAni();
	afx_msg void OnBnClickedButtonClearAllAni();
	afx_msg void OnBnClickedButtonAniUp();
	afx_msg void OnBnClickedButtonClearAniDown();
	afx_msg void OnBnClickedButtonClearRecentKey();
	afx_msg void OnBnClickedButtonAddKey();
	afx_msg void OnBnClickedButtonAutotanget();
	afx_msg void OnBnClickedButtonAddKeyToHead();
	afx_msg void OnBnClickedButtonCamaniPause();
	afx_msg void OnBnClickedButtonCircle();
private:	
	HRESULT SynWithCamAniList();	///同步内部的CameraMovement中的CamAni的列表和Dialog中的Combo
	HRESULT SynWithCheckEditKeyFrame();		///同步当前的CamAni和CheckBox中的允许编辑关键帧
	BOOL LockControlObjectsOnPlay( BOOL bDoLock = TRUE );	///检查是否在播放的时候，并似乎Scene的状态锁定某些按钮 
	HRESULT SynFromInnerToInterface(DWORD_PTR dwOption = 0);
	HRESULT SynFromInterfaceToInner(DWORD_PTR dwOption = 0);
	
	HRESULT RegisterCallbacks(BOOL bRegisterOrUnRegister);
	static KSceneSceneEditorDialogCamera* m_stpSSEDialogCamera;
	static VOID CALLBACK OnSelKeyChange(DWORD dwKeyFrameTime);
private:
	bool m_bCheckEditKeyFrame;
	CButton m_ckEnableEditKeyFrame;
	float m_fPlaySpeed;
	float m_fAngle;
	float m_fZFar;
	CComboBox m_ctlComboCamAniList;
	CButton m_BnNewAni;
	int m_nSelIndex;		///记录选择的Index。
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.
BOOL CALLBACK MyEnableWindow( HWND hWnd, LPARAM BOOL_LPARAM ); ///自定义回调函数，用于控件遍历，禁止控件

#endif // !defined(AFX_KSCENESCENEEDITORDIALOGCAMERA_H__09916E03_1AB2_408C_A53C_4273F1C7BE9E__INCLUDED_)
