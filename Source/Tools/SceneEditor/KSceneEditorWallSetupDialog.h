#pragma once
#include "afxcmn.h"
#include "afxwin.h"

class IEKG3DSceneSceneEditor;
class IEKG3DWallDefine;
#include "IEEditor.h"

// KSceneEditorWallSetupDialog 对话框

class KSceneEditorWallSetupDialog : public CDialog
{
	DECLARE_DYNAMIC(KSceneEditorWallSetupDialog)

public:
	KSceneEditorWallSetupDialog(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~KSceneEditorWallSetupDialog();

	IEKG3DWallDefine* m_pWallDefine;
	IEKG3DSceneSceneEditor* m_lpSceneEditor;
	IEKG3DWall* m_lpWall;
	HRESULT SetWallDefine(IEKG3DWallDefine* pDefine,IEKG3DSceneSceneEditor* pEditor,IEKG3DWall* pWall);

// 对话框数据
	enum { IDD = IDD_SCENEEDITOR_WALL };

protected:
	map<KG3DWALLELEMENTTYPE,int>m_mapComboIndex;
	int m_nCurrentSelectedElement;
	int m_nCurrentSelectedWall;
	int m_nOutputWindowID;
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()

	HRESULT FillElementList();
	HRESULT GetTypeName(LPSTR szName,KG3DWALLELEMENTTYPE eType);
	void UpdateElementInfo(BOOL bToUI,BOOL bComputeFromMesh);
	IEKG3DSceneObjectPlacementTool* GetObjectPlacementToolInterface(IEKG3DSceneSceneEditor* pEditor);
	void FillWallList();

public:
	BOOL OnInitDialog();

	void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnEnChangeEditModelfile();
	afx_msg void OnLvnItemchangedList2(NMHDR *pNMHDR, LRESULT *pResult);
	CListCtrl m_ListDefine;
	afx_msg void OnBnClickedButtonAddEle();
	afx_msg void OnBnClickedButtonDeleteEle();
	afx_msg void OnBnClickedButtonLoadele();
	afx_msg void OnBnClickedButtonSaveele();
	CComboBox m_Combo_ElementType;
	CString m_szModelName;
	afx_msg void OnBnClickedButtonBrower();
	float m_fAx;
	float m_fAy;
	float m_fAz;
	float m_fBx;
	float m_fBy;
	float m_fBz;
	CButton m_Button_Preview;
	afx_msg void OnBnClickedButtonPreview();
	afx_msg void OnCbnSelchangeComboEletype();
	afx_msg void OnEnChangeEditAX();
	afx_msg void OnEnChangeEditAY();
	afx_msg void OnEnChangeEditAZ();
	afx_msg void OnEnChangeEditBX();
	afx_msg void OnEnChangeEditBY();
	afx_msg void OnEnChangeEditBZ();
	afx_msg void OnLbnSelchangeListKeypoint();
	CListBox m_listWall;
	afx_msg void OnLbnSelchangeListWall();
	afx_msg void OnBnClickedButtonAddwall();
	afx_msg void OnBnClickedButtonDeletewall();
	afx_msg void OnBnClickedButtonAddpoint();
};
