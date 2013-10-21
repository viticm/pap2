#pragma once
#include "KColorDialog.h"
interface IEKG3DSceneSceneEditor;//by dengzhihui Refactor 2006Äê11ÔÂ27ÈÕ
interface IEKG3DEnvironment;
interface IEKG3DEnvEffFog;

// KSceneSceneEditorDialogMisc dialog

class KSceneSceneEditorDialogMisc : public CDialog
{
	DECLARE_DYNAMIC(KSceneSceneEditorDialogMisc)

	IEKG3DSceneSceneEditor*  m_lpSceneSceneEditor;
public:
	void SetSceneEditor(IEKG3DSceneSceneEditor* pSceneEdit)
	{
		m_lpSceneSceneEditor = pSceneEdit;
	}
public:
	KSceneSceneEditorDialogMisc(CWnd* pParent = NULL);   // standard constructor
	virtual ~KSceneSceneEditorDialogMisc();

// Dialog Data
	enum { IDD = IDD_SCENEEDITOR_MISC };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void OnOK();

	DECLARE_MESSAGE_MAP()
	float m_fLod;
	float m_fFogNear;
	float m_fFogFar;
	float m_fFogDes;
	DWORD m_dwFogModificationOption;
public:
	afx_msg void OnBnClickedCheckMipmap();
	afx_msg void OnBnClickedCheckMagmap();
	afx_msg void OnBnClickedCheckMinmap();
	afx_msg void OnEnChangeEditLod();
	afx_msg void OnEnChangeEditFogNear();
	afx_msg void OnEnChangeEditFogFar();
	afx_msg void OnEnChangeEditFogDes();
	afx_msg void OnBnClickedFogEnable();

	afx_msg void OnBnClickedButtonFogcolor();
public:
	KColorPickerButton m_bnFogColorPicker;
public:
	virtual BOOL OnInitDialog();
private:
	IEKG3DEnvEffFog* GetFog(IEKG3DScene* pScene);
	void UpdataLogicalSize();

public:
	afx_msg void OnBnClickedBnFogUpdateFogParam();
	//afx_msg void OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized);
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);

	void UpdateFogUI();
	afx_msg void OnBnClickedCheckShowlogicalsize();
	int m_nLogicalXStart;
	int m_nLogicalZStart;
	int m_nLogicalWidth;
	int m_nLogicalHeight;
	afx_msg void OnEnChangeEditLogicalx();
	afx_msg void OnEnChangeEditLogicalz();
	afx_msg void OnEnChangeEditLogicalwidth();
	afx_msg void OnEnChangeEditLogicalheight();

	BOOL m_bCheckModel;
	BOOL m_bCheckModelST;
	BOOL m_bCheckSFX;
	BOOL m_bCheckStree;
	BOOL m_bCheckLogicObj;
	BOOL m_bCheckObjSet;
	BOOL m_bCheckUnSmallObj;
	BOOL m_bCheckUnBigOBj;
	DWORD m_dwLimit;
	afx_msg void OnBnClickedCheckModel();
	afx_msg void OnBnClickedCheckModelst();
	afx_msg void OnBnClickedCheckSfx();
	afx_msg void OnBnClickedCheckStree();
	afx_msg void OnBnClickedCheckLogicobj();
	afx_msg void OnBnClickedCheckGrounp();
	void UpdateSelectOption(BOOL bSaveAndValidate = TRUE);
	
	afx_msg void OnBnClickedCheckUnsmallobj();
	afx_msg void OnBnClickedCheckUnbigobj();
	afx_msg void OnEnChangeEditLimit();	
};
