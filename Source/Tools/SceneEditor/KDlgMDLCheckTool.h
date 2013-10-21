#pragma once
#include "afxwin.h"


// KDlgMDLCheckTool dialog
class IEKG3DSceneModelEditor;
class IEKG3DModel;
class KDlgMDLCheckTool : public CDialog
{
	DECLARE_DYNAMIC(KDlgMDLCheckTool)
	enum enuModelType
	{
		Model_Mesh,
		Model_MDL,
		Model_Ani,
	};
public:
	KDlgMDLCheckTool(CWnd* pParent = NULL);   // standard constructor
	virtual ~KDlgMDLCheckTool();
	struct FilePath
	{
		CString* strMDLPath;
		CString* strItemPath;
		CString* strAccPath;
	};
	const static int WM_FILEPATH = WM_USER + 1;
// Dialog Data
	enum { IDD = IDD_DIALOG_CHECKTOOL };
	void SetModelEditor(IEKG3DSceneModelEditor *pEditor) { m_pModelEditor = pEditor; }
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
	
	BOOL OnInitDialog();
private:
	void LoadConfig();
	void SaveConfig();
	afx_msg void OnBnClickedButtonSetdir();
	afx_msg void OnBnClickedButtonRefresh();
	afx_msg void OnLbnSelchangeListMdl();
	afx_msg void OnLbnSelchangeListSocket();
	afx_msg void OnLbnSelchangeListItem();
	afx_msg void OnLbnSelchangeListAnimation();
	LRESULT GetPath(WPARAM wParam, LPARAM lParam);
	void FindAnimationFile(LPCSTR strMdlFilePath,
		LPCSTR strSelect = NULL);
	void FindModelInDirectory(LPCSTR strStartDir, 
		BOOL bIncludeSubDir,
		enuModelType Type,
		std::vector<std::string>& vecStore);
	CListBox m_ctlMDLList;
	CListBox m_ctlSocketList;
	CListBox m_ctlItemList;
	CListBox m_ctlAnimationList;
	CListBox m_ctlAccessory;
	CListBox m_ctlAccAnimation;
	CEdit m_ctlAccAniFrameLength;
	std::vector<std::string> m_vecMdlFilePath;
	std::vector<std::string> m_vecItemFilePath;
	std::vector<std::string> m_vecAnimationFilePath;
	std::vector<std::string> m_vecFaceFilePath;
	std::vector<std::string> m_vecAccFilePath;
	std::vector<std::string> m_vecAccAniFilePath;
	std::vector<BOOL>        m_vecAccAnimationChanged;
	std::vector<BOOL>        m_vecAnimationChanged;
	IEKG3DSceneModelEditor *m_pModelEditor;
	TCHAR m_strMDLPath[MAX_PATH];
	TCHAR m_strItemPath[MAX_PATH];
	TCHAR m_strAccPath[MAX_PATH];
	CComboBox m_ctlFaceList;
	afx_msg void OnCbnSelchangeComboFace();
	void FindMDLFacePart();
	CString m_strMDLFilePath;
	CString m_strItemFilePath;
	CString m_strAnimationFilePath;
	CString m_strFaceFilePath;
	CString m_strAccFilePath;
	afx_msg void OnBnClickedButtonClearbind();
	void NotifyAnimationChange();
	float m_fFrameLength;
	BOOL m_bFrameLengthChanged;
	BOOL m_bAccFrameLengthChanged;
	virtual void OnOK();
	virtual void OnCancel();
	afx_msg void OnBnClickedButtonAccessories();
	BOOL m_bFullSized;
	float m_fAccAniFrameLength;
	int m_nFullLength;
	afx_msg void OnLbnSelchangeListAccessories();
	afx_msg void OnLbnSelchangeListAccanimation();
	IEKG3DModel* m_pAccModel;
	afx_msg void OnBnClickedButtonPlay();
public:
	afx_msg void OnBnClickedButtonSaveas();
};
