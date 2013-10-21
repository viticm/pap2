#pragma once
#include "afxwin.h"
#include "IEKG3DAnimationTag.h"

// KDlgSfxTag dialog

class KSceneModelEditorAnimationPanel;
class KDlgSfxTag : public CDialog
{
	DECLARE_DYNAMIC(KDlgSfxTag)

public:
	KDlgSfxTag(IEKG3DAnimationSFXTag* pSfxTag,CWnd* pParent = NULL);   // standard constructor
	virtual ~KDlgSfxTag();

	// Dialog Data
	enum { IDD = IDD_MODELEDITOR_SFX };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
	// 是否要根据动作播放的速度来修改实际播放的速度
	BOOL m_bAdaptSpeedToMotion;
	// 特效是否会被下一个特效取消
	BOOL m_bInterruptable;
	// 是否通知表现逻辑
	BOOL m_bNotify;
protected:
	// 动作完成时的处理
	BOOL m_bMotionFinishEvent;
	// 播放速度 
	float m_fSpeed;
	// 世界坐标
	BOOL m_bSetWorldPosition;
	IEKG3DAnimationSFXTag *m_pSfxTag;
	KSceneModelEditorAnimationPanel *m_pAnimationPanel;
	UINT m_uFrame;
public:
	virtual BOOL OnInitDialog();
protected:
	void FillComFinishState();
	void FillControlData();
	void FillComTagName();
	void SetComTagNameCurSel(int nCurSel);
	void SaveState();
	CComboBox m_ComTagName;
	CString m_csTagName;
	CString m_csSfxName;
	CComboBox m_ComFinishState;
	static  UINT m_uFrameCopy;
	int m_LastCursel;
public:
	afx_msg void OnBnClickedButtonNew();
	afx_msg void OnBnClickedButtonDelete();
	afx_msg void OnBnClickedButtonSfxselect();
	void SetFrame(UINT uFrame);
	void SetAnimationPanel(KSceneModelEditorAnimationPanel *pAnimationPanel);
	afx_msg void OnCbnDropdownComboTagname();
	afx_msg void OnCbnSelchangeComboTagname();
	afx_msg void OnClose();
	afx_msg void OnBnClickedButtonCopy();
	afx_msg void OnBnClickedButtonPaste();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnEnChangeEditTagname();
};
