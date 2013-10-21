#pragma once
#include "ieeditor.h"
#include "afxwin.h"

// KModelEditorBspGenerateDialog dialog

class KModelEditorBspGenerateDialog : public CDialog
{
	DECLARE_DYNAMIC(KModelEditorBspGenerateDialog)

public:
	struct meshinfo
	{
		string szName;
		BOOL bBsp;
	};
	vector<meshinfo>m_vecMeshInfo;

	string m_strDirectory;
	KModelEditorBspGenerateDialog(CWnd* pParent = NULL);   // standard constructor
	virtual ~KModelEditorBspGenerateDialog();
	IEKG3DScene* m_pScene;
	IEKG3DSceneModelEditor* m_pModelEditor;
	void FillList();

	void FindFilesInDir(LPSTR pDir,BOOL bNoLodMesh);

	void SetScene(IEKG3DScene* pScene)
	{
		IEKG3DModel* pModel = NULL;
		m_pScene = pScene;
		KG_PROCESS_ERROR(pScene);

		pScene->GetSceneModelEditor(&m_pModelEditor);
		KG_PROCESS_ERROR(m_pModelEditor);

		m_pModelEditor->GetCurModel(&pModel);
		KG_PROCESS_ERROR(pModel);

		pScene->RemoveRenderEntity(SCENEENTITY_MODEL, (void*)pModel);
Exit0:
		;
	}

// Dialog Data
	enum { IDD = IDD_BSP };

	void FindFiles(BOOL bNoLodMesh);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonSetdir();
	CListBox m_List;
	afx_msg void OnLbnSelchangeList1();
	afx_msg void OnBnClickedButtonGenerate();
	afx_msg void OnBnClickedButtonSetdir3();
	afx_msg void OnBnClickedButtonGenerate2();
	CListBox m_ListReport;
	afx_msg void OnEnChangeEdit1();
	afx_msg void OnEnChangeEdit6();
	float m_fLowRation;
	float m_fMediumRation;
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
};
