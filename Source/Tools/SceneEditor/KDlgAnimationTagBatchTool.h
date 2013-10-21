#pragma once
#include "afxwin.h"

// KDlgAnimationTagBatchTool dialog

class IEKG3DAnimationTagContainer;
interface IEKG3DSceneModelEditor;

struct MetorSelInfo 
{// add by chenganlai  用于暂时保存刀光数据
	TCHAR pSocketName[MAX_PATH];
	BOOL bIsMetorOn;
	DWORD dwNameHash;
	MetorSelInfo()
	{
		pSocketName[0] = '\0';
		bIsMetorOn = FALSE;
		dwNameHash = 0;
	}
};
class KDlgAnimationTagBatchTool : public CDialog
{
	DECLARE_DYNAMIC(KDlgAnimationTagBatchTool)

public:
	KDlgAnimationTagBatchTool(CWnd* pParent, 
		IEKG3DAnimationTagContainer *pTagContainer);   // standard constructor
	virtual ~KDlgAnimationTagBatchTool();

// Dialog Data
	enum { IDD = IDD_DIALOG_TAGPROCESS };
	enum enuFileFilterType
	{
		FILETYPE_TANI,
		FILETYPE_SOURCE,
		FILETYPE_COUNT,
	};

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
	
	
	CListBox m_ListFiles;
	CListBox m_ListFilePending;
	CString m_strKeyword;
	DWORD m_dwType;
	DWORD m_dwProcessType;
public:
	enum enuProcessType
	{
		PROCESS_GENERATELIST,
		PROCESS_CHANGESOURCE,
		PROCESS_DELETE,
		PROCESS_ADD,
		PROCESS_CHANGEPLAYTYPE,
		PROCESS_DELETEALLSOUND,
		PROCESS_INSERTMETOR,
		PROCESS_SOUNDVERSION3,
		PROCESS_COUNT,
	};

	struct ProcessTypeInfo
	{
		const TCHAR *strOld;
		const TCHAR *strNew;
		int nOldShow;
		int nNewShow;
		int nSuffix;
		int nExtraControl;
	};
	map<DWORD,MetorSelInfo> MetroSelect;
	static const TCHAR *s_ProcessOption[PROCESS_COUNT];
	static ProcessTypeInfo s_ProcessTypeInfo[PROCESS_COUNT];
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedRadioTani();
	afx_msg void OnBnClickedRadioSource();
	void OnTypeChange(BOOL bClearPendingList = TRUE);
	virtual void OnOK();
private:
	void SearchKeyword();
	CString m_strFileFullPath;
	std::set<DWORD> m_setFilePendingHash;
	std::vector<DWORD> m_CurrentList;//经过keyword筛选之后剩下的
	std::vector<DWORD> m_vecFileNameHashList;
public:
	afx_msg void OnLbnSelchangeListFiles();
	afx_msg void OnLbnDblclkListFiles();
	afx_msg void OnBnClickedButtonProcess();
private:
	CString m_strOldSource;
	CString m_strNewSource;
	CEdit m_editOld;
	CEdit m_editNew;
public:
	CComboBox m_comboProcessOption;
private:
	CString m_strStaticOld;
	CListBox& GetCurrentFileListBox();
	void MoveSoundV3Control();
public:
	CString m_strStaticNew;
	afx_msg void OnCbnSelchangeComboProcessoption();
	afx_msg void OnLbnDblclkListPending();
	CString m_strSuffix;
	CListBox m_ListFileSource;
	IEKG3DAnimationTagContainer* m_pTagContainer;
	CListBox *m_pFileLists[FILETYPE_COUNT];
	afx_msg void OnLbnSelchangeListFilesSource();
	afx_msg void OnLbnDblclkListFilesSource();
	afx_msg void OnLbnSelchangeListPending();
	afx_msg void OnBnClickedButtonClearpending();
	float m_fSoundPlayRate;
	float m_fSoundVolume;
	float m_fSFXSpeed;
	bool m_bDefaultValueSet;
	CComboBox m_comboSFXPlayType;
	afx_msg void OnBnClickedButtonExportlist();
	// add by chenganlai
protected:
	CListBox m_ListBoxSocket;//
	HRESULT  Init_Motion_Metor();
	HRESULT  FillCombSocket();//
	CComboBox m_CombSocket;//
	int m_nSoundVersion;// 
	// 
public:
	afx_msg void OnCbnSelchangeComboSocket();

protected:
	void FillMetorListBox();
	void SetMeteorSwith(BOOL);
	DWORD GetNameHash(LPCSTR);
	void MoveMetroControl();
	void FillSoundType();
public:
	/*afx_msg void OnBnClickedButtonDeleteSocket();*/
	/*afx_msg void OnBnClickedButtonSocketbindDelete();*/
	afx_msg void OnLbnSelchangeListSocket();
	afx_msg void OnBnClickedRadioCloseMetor();
	afx_msg void OnBnClickedRadioOpenMetor();
	afx_msg void OnLbnDblclkListSocket();
	//add end
protected:
	CComboBox mComSoundType;
};
