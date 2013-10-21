#pragma once
#include "afxwin.h"
#include "MeshEditorDoc.h"
#include "IEEditor.h"
#include "IEKG3DAnimationTag.h"
#include "KSceneModelEditorFrameWnd.h"
#define GET_MODEL_EDITOR() \
	CMDIFrameWnd *pMainFrame = (CMDIFrameWnd*)AfxGetMainWnd();\
	KSceneModelEditorFrameWnd *pModelFrame = (KSceneModelEditorFrameWnd*)pMainFrame->GetActiveFrame();\
	KSceneModelEditorDoc* pDoc = dynamic_cast<KSceneModelEditorDoc*>(pModelFrame->GetActiveDocument());\
	if (!pDoc)\
	return;\
	IEKG3DScene* pScene = pDoc->GetScene();\
	if(!pScene) return;\
	IEKG3DSceneModelEditor* pEditor = NULL;\
	pScene->GetSceneModelEditor(&pEditor); \
	if(!pEditor)	\
	return;

// KDlgMotionTag dialog
class KSceneModelEditorAnimationPanel;
class KDlgMotionTag : public CDialog
{
	DECLARE_DYNAMIC(KDlgMotionTag)

public:
	KDlgMotionTag(IEKG3DAnimationMotionTag* pTag, 
		CWnd* pParent = NULL);   // standard constructor
	virtual ~KDlgMotionTag();

// Dialog Data
	enum { IDD = IDD_DIALOG_MOTIONTAG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
    void FillComboSocket();
	void FillListSocket(MotionTagBlockMeteor *pData);
	void FillListBinded();
    BOOL OnInitDialog();
    UINT m_uFrame;    
    KSceneModelEditorAnimationPanel *m_pAnimationPanel;

private:
    CComboBox m_ComboExtraType;
    CComboBox m_ComboType;
public:
    afx_msg void OnBnClickedOk();
private:
    MotionTagDataRuntime m_Data;
public:
    afx_msg void OnClose();
	
private:
    CString m_strTagName;
public:
    afx_msg void OnBnClickedButtonDelete();
	afx_msg void OnBnClickedMeteorBegin();
	afx_msg void OnBnClickedMeteorEnd();
	CComboBox m_Combo_Socket;
	afx_msg void OnCbnSelchangeComboSocket();
	afx_msg void OnLbnSelchangeListSocket();
	CListBox m_ListSocket;
	afx_msg void OnBnClickedButtonLoad();
	CListBox m_listBinded;
	afx_msg void OnLbnSelchangeListBinded();
	afx_msg void OnBnClickedButtonAdd();
	afx_msg void OnBnClickedButtonDel();
private:
	void FillSubList();
	CComboBox m_comboSubTag;
	IEKG3DAnimationMotionTag* m_pMotionTag;
	CComboBox m_comboAddTag;
	void UpdateDetail();
public:
	void FillMetero(void *pData);
	void FillFaceMotion(void *pData);
	void FillDamage(void *pData);
	void FillMissile(void *pData);
	void FillEquipSfx(void *pData);
	void FillGroundEffect(void *pData);  // add by chenganlai
	afx_msg void OnCbnSelchangeComboSubtag();
private:
	void SetMeteorSwith(BOOL bStart);
	void* GetCurrentSubBlock();
	void SaveLastSel();
	typedef void (KDlgMotionTag::*pfFillUserInterface)(void *);
	typedef void (KDlgMotionTag::*pfUpdateTagData)();
	static pfFillUserInterface s_FillUserInterface[MTBT_COUNT];
	static pfUpdateTagData     s_UpdateTagData[MTBT_COUNT];

	void FillFaceMotionType();

	void UpdateDamage();
	void UpdateMetero();
	void UpdateFaceMotion();
	void UpdateMissile();
	void UpdateEquipSfx();
	void UpdateGroundEffect(); // add by chegnanlai

	float m_fDamage;
	float m_fMissileDamage;
	DWORD m_dwMissileID;
	CComboBox m_comboMotionType;
	float m_fPlayRate;
	int m_nLastSel;
	float m_fPlaySpeed;
	CString m_strFaceMotionDir;
	CString m_strFaceMotionSpecifiedSuffix;//指定的表情的后缀
public:
	CString m_strCurrentFacePath;
	afx_msg void OnCbnSelchangeComboFacemotiontype();
	afx_msg void OnBnClickedButtonSelani();
private:
	CButton m_buttonOnce;
	CButton m_buttonLoop;
	//CButton m_buttonSwitch;
	//CButton m_buttonKeep;
	CButton m_buttonStart;
	CButton m_buttonEnd;
	CButton m_buttonNone;
public:
	afx_msg void OnBnClickedRadioStart();
	afx_msg void OnBnClickedRadioEnd();
	afx_msg void OnBnClickedRadioNone();
	afx_msg void OnBnClickedRadioOnce();
private:
	HRESULT LoadPlayerEquipRepresentConfig();
	CButton m_ctlKeepMotion;
	void SaveState();
	void Update();
public:
	afx_msg void OnBnClickedRadioLoop();
	DWORD m_dwEquipSfxID;
	CComboBox m_comboEquipSfxType;
	afx_msg void OnBnClickedButtonCopy();
	afx_msg void OnBnClickedButtonPaste();
protected:
	static UINT m_uFrameCopy;
public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	void SetcomboSubTagCursel(int nCursel);
	afx_msg void OnEnChangeEditTagname();
protected:
	CComboBox m_ComSFXType;
};