#pragma once

class IEKG3DAnimationTagContainer;
// KModelEditorTagExport dialog

class KModelEditorTagExport : public CDialog
{
	DECLARE_DYNAMIC(KModelEditorTagExport)

public:
	KModelEditorTagExport(CWnd* pParent = NULL);   // standard constructor
	virtual ~KModelEditorTagExport();

// Dialog Data
	enum { IDD = IDD_TAG_EXPORT };

protected:
	virtual BOOL OnInitDialog();
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	
	DECLARE_MESSAGE_MAP()
public:
	CListBox m_ListBox_Animation;
	IEKG3DAnimationTagContainer* m_pContainer;
	vector<CString> m_vecAinName;
	CString m_strTaniName;
	CString m_strPath;
	
	CListBox  m_ListBox_Source;
	CListBox  m_ListBox_Target;
	CComboBox m_comboSource;
	CComboBox m_comboTarget;
	CString   m_strTarget;
	CString   m_strSource;
	enum TAGEXPORT
	{
		TAG_Player,
		TAG_NPC,
	}m_eTagState; 
public:
	void Init();
	void SetAnimationTagContainer(IEKG3DAnimationTagContainer* pContainer);
	void SetTaniName(CString strTaniName);
	afx_msg void OnBnClickedButtonOpenfile();
	afx_msg void OnBnClickedButtonExportTani();
	void RefreshDirectory();
	afx_msg void OnBnClickedRadio1();
	afx_msg void OnBnClickedRadio2();
	afx_msg void OnCbnSelchangeCombo1();
	afx_msg void OnBnClickedBatchExport();
	afx_msg void OnCbnSelchangeCombo2();
};
