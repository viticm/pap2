#pragma once
#include "afxcmn.h"
#include "afxwin.h"
#include "..\include\\KG3DEngine\KG3DTypes.h"


struct IKG3DSoundShell;

class KSceneEditorSoundBallDialog : public CDialog
{
	DECLARE_DYNAMIC(KSceneEditorSoundBallDialog)

public:
	
	HRESULT FillTree();
	HRESULT FillTreeWithDir(LPCSTR Dir,HTREEITEM hStart);
	KSceneEditorSoundBallDialog(CWnd* pParent = NULL);   // standard constructor
	virtual ~KSceneEditorSoundBallDialog();

// Dialog Data
	enum { IDD = IDD_SCENEEDITOR_SOUNDBALL };
	SoundBallData m_SoundBallData;
protected:
	HRESULT GetSelectedItemFullName(TCHAR FullName[]);
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	string m_strDirectory;
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnTvnSelchangedTree1(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedButtonSetdir();
	CTreeCtrl m_Tree;
	afx_msg void OnNMDblclkTree1(NMHDR *pNMHDR, LRESULT *pResult);
	CListBox m_List;
	BOOL m_bTimeEnable;
	afx_msg void OnBnClickedCheckTimeenable();
	afx_msg void OnBnClickedButtonLoad();
	afx_msg void OnBnClickedButtonSave();
	afx_msg void OnBnClickedCheckLoop();
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	std::string m_strDefaultOpen;
private:
	void UpdateFromUI();
	void UpdateUI();
	void PlaySound(LPCSTR strFileName);
	CButton m_buttonLoop;
	CButton m_buttonEnableTime;
	int m_nSpan;
	int m_nStart;
	int m_nEnd;
	float m_fRation;
	float m_fRadius;
	CString m_strPathName;
	DWORD m_dwPreviewSound;
	const static DWORD s_dwInvalidateSoundID = 0xffffffff;
	afx_msg void OnBnClickedButtonPlay();
	afx_msg void OnBnClickedButtonSaveas();
public:
	afx_msg void OnClose();
};
