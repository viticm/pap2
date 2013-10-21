#include "afxwin.h"
#include "KGTreeCtrl.h"

#if !defined(AFX_KSCENESCENEEDITORDIALOGORANMENT_H__CDA6EA64_E05C_4619_A4B7_CB6800EB5FDB__INCLUDED_)
#define AFX_KSCENESCENEEDITORDIALOGORANMENT_H__CDA6EA64_E05C_4619_A4B7_CB6800EB5FDB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// KSceneSceneEditorDialogOranment.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// KSceneSceneEditorDialogOranment dialog

#define PLIGHT_FILE_PATH TEXT("data\\public\\SceneEditor\\plight.plight")

enum
{
	IMAGE_ERROR =		-1,
	IMAGE_DIR =			0,
	IMAGE_MESH =		1,
	IMAGE_SPEEDTREE =	2,
	IMAGE_SFX =			3,
	IMAGE_OBJECT =		4,
	IMAGE_SET =			5,
	IMAGE_GROUP =		6,
	IMAGE_SOUNDBALL =	7,
	IMAGE_MODEL =		8,	//用于检查模型
	IMAGE_WALL  =       9,
    IMAGE_PVS   =       10,
    IMAGE_PORATL =      11,
	IMAGE_POINTLIGHT =  12
};

interface IEKG3DSceneSceneEditor;//by dengzhihui Refactor 2006年11月27日
interface IEKG3DSceneObjectPlacementTool;

class KSceneSceneEditorDialogOranment : public CDialog
{
// Construction
public:
	KSceneSceneEditorDialogOranment(CWnd* pParent = NULL);   // standard constructor
	~KSceneSceneEditorDialogOranment();
// Dialog Data
	//{{AFX_DATA(KSceneSceneEditorDialogOranment)
	enum { IDD = IDD_SCENEEDITOR_ORANMENT };
	KGTreeCtrl m_Tree;//	CTreeCtrl	m_Tree;
	//}}AFX_DATA
	CImageList  m_imgList;


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(KSceneSceneEditorDialogOranment)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(KSceneSceneEditorDialogOranment)
	afx_msg void OnSelchangedTree1(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	HRESULT FillTree();

	HRESULT FillTreeWithDir(LPSTR Dir,LPSTR pFileType,HTREEITEM hStart);

	IEKG3DSceneSceneEditor*  m_lpSceneSceneEditor;
	//IEKG3DSceneObjectPlacementTool* m_lpObjPlacementTool;
	IEKG3DSceneObjectPlacementTool* GetObjectPlacementToolInterface(IEKG3DSceneSceneEditor* pEditor);
public:
	void SetSceneEditor(IEKG3DSceneSceneEditor* pSceneEdit);
public:
	CStatic m_Static_PreView;
	int m_nOutputWindowID;
private:
	afx_msg void OnBnClickedButtonRefresh();
    afx_msg void OnBnClickedButtonReload();
	afx_msg void OnSize(UINT nType, int cx, int cy);

	afx_msg void OnNMSetfocusTree1(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedCheckMesh();
	afx_msg void OnBnClickedCheckSet();
	afx_msg void OnBnClickedCheckSpeedtree();
	afx_msg void OnBnClickedCheckObject();
	afx_msg void OnBnClickedCheckSfx();
	afx_msg void OnBnClickedCheckGroup();
	afx_msg void OnBnClickedButtonCreatini();
	afx_msg void OnBnClickedCheckShownew();



	void GetItemFilePath(HTREEITEM hItem, CString& RetPath);
	void CreateDeafultInI(HTREEITEM hItem, TCHAR* strDefaultPath);
	BOOL CheckInIExist(const TCHAR *strFileName);
	HTREEITEM m_hLastItem;
	afx_msg void OnNMClickTree1(NMHDR *pNMHDR, LRESULT *pResult);

	BOOL CheckIsLODModel(std::string strFileName);
public:
    afx_msg void OnBnClickedPlacePlight();
	CString m_strSub;
	HTREEITEM m_FindNode ; // 用于记录查找出来的节点 by huangjinshou
	afx_msg void OnBnClickedButtonLookup();
	afx_msg void OnEnChangeEditSubstring();
	afx_msg void OnBnClickedCheckPvs();
	afx_msg void OnBnClickedCheckPointlight();
	afx_msg void OnBnClickedReplaceOneEntity();
	afx_msg void OnBnClickedReplaceAllEntity();
};

extern int GetItemImage(LPSTR extName);

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_KSCENESCENEEDITORDIALOGORANMENT_H__CDA6EA64_E05C_4619_A4B7_CB6800EB5FDB__INCLUDED_)
