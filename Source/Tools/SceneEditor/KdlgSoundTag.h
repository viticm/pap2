#pragma once

#include "afxwin.h"
#include "MeshEditorDoc.h"
#include "IEEditor.h"
#include "IEKG3DAnimationTag.h"
#include "KSceneModelEditorFrameWnd.h"
// KdlgSoundTag dialog

#undef MAX_PATH
#define  MAX_PATH 260
class KdlgSoundTag : public CDialog
{
	DECLARE_DYNAMIC(KdlgSoundTag)

public:
	KdlgSoundTag(IEKG3DAnimationSoundTag* pTag,CWnd* pParent = NULL);   // standard constructor
	virtual ~KdlgSoundTag();

	// Dialog Data
	enum { IDD = IDD_MODELEDITOR_SOUND };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonVersion2();
	afx_msg void OnBnClickedButtonToversion3();
	afx_msg void OnBnClickedButtonFilelist();
	afx_msg void OnBnClickedButtonAdd();
	afx_msg void OnBnClickedButtondelete();
	afx_msg void OnBnClickedButtonDeletetag();
	afx_msg void OnBnClickedButtonPlay();
	virtual BOOL OnInitDialog();
	typedef void (KdlgSoundTag::*pfFillControl)();
	static pfFillControl s_FillControl[2];
	typedef void (KdlgSoundTag::*pfPlay)();
	static pfPlay s_Play[2];
	typedef void (KdlgSoundTag::*pfDelete)();
	static pfDelete s_Delete[2];
	typedef void (KdlgSoundTag::*pfSave)();
	static pfSave s_Save[2];
	void SetAnimationPanel(KSceneModelEditorAnimationPanel *pAnimationPanel)
	{
		m_pAnimationPanel = pAnimationPanel;
	}
	void GetAnimationPanel(KSceneModelEditorAnimationPanel *pAnimationPanel)
	{
		pAnimationPanel = m_pAnimationPanel;
	}
	HRESULT SetFrame(UINT uFrame);
protected:
	IEKG3DAnimationSoundTag *m_pSoundTag;
	KSceneModelEditorAnimationPanel *m_pAnimationPanel;
	UINT m_uFrame;
	UINT m_uVersion;
	int m_nLastIndex;
	TCHAR m_szFromTreeCtl[MAX_PATH];
	static UINT m_uCopyFrame;
	void FillVersion2();
	void FillVersion3();
	void PlayVersion2();
	void PlayVersion3();
	void FillFileList();
	void SaveStateV2();
	void SaveStateV3();
	void DeleteVersion2();
	void DeleteVersion3();
	void FillVersion2Detail();
	HRESULT GetKeyFrameData(UINT uFrame,SoundDataInfoVersion2 *pRetSoundTagData);
	HRESULT GetKeyFrameData(UINT uFrame,SoundDataInfoVersion3 *pRetSoundTagData);
	void ChangeVersion();
	void FillSoundType();
	void FillVersion3Detail();
	void Clear();
	//
	HRESULT MoveControl();
	CComboBox m_ComFileList;
public:
	afx_msg void OnCbnSelchangeComboSoundfile();
protected:
	float m_fPlayRate;
	float m_fFar;
	float m_fNear;
	int m_Volume_V2;
	CComboBox m_ComSoundType;
	int m_Volume_V3;
	void Copy();
	void Paste();
public:
	afx_msg void OnCbnSelchangeComboSoundtype();
public:
	afx_msg void OnBnClickedCheck1();
public:
	afx_msg void OnBnClickedCheck2();
public:
	afx_msg void OnBnClickedCheck3();
	afx_msg void OnClose();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
};
