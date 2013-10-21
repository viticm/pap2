#pragma once
#include "afxcmn.h"
#include "afxwin.h"
#include "IETools.h"


// KSceneSceneEditorDialogGroup dialog
interface IEKG3DSceneSceneEditor;
class KSceneEditorDocLogical;


class KSceneSceneEditorDialogGroup : 
	public CDialog,
	public IEKG3DSimpleSink
{
	DECLARE_DYNAMIC(KSceneSceneEditorDialogGroup)

public:
	KSceneSceneEditorDialogGroup(CWnd* pParent = NULL);   // standard constructor
	virtual ~KSceneSceneEditorDialogGroup();
	void SetSceneEditor(IEKG3DSceneSceneEditor* pSceneEdit)
	{
		m_lpSceneSceneEditor = pSceneEdit;
	}
	void SetDocLogical(KSceneEditorDocLogical* pLogical)
	{
		m_lpDocLogical = pLogical;
	}
// Dialog Data
	enum { IDD = IDD_SCENEEDITOR_GROUP };

protected:
	void FillCombo();

	virtual void SinkFunc(INT nSinkID){}
	virtual void SinkFunc2(INT nSinkID,INT nsubSinkID,DWORD dwValue,PVOID pPoint);

	HTREEITEM m_hDoodadRoot;
	HTREEITEM m_hNPCRoot;
	HTREEITEM m_hAIRoot;
	HTREEITEM m_hNPCRandomRoot;
	HTREEITEM m_hNPCAIParameterRoot;
	HTREEITEM m_hProgressRoot;

	map<DWORD,HTREEITEM>m_mapDoodadReliveGroupTreeItem;
	map<DWORD,HTREEITEM>m_mapNpcReliveGroupTreeItem;
	map<DWORD,HTREEITEM>m_mapNpcRandomGroupTreeItem;
	map<DWORD,HTREEITEM>m_mapNpcAIParameterTreeItem;
	map<int,HTREEITEM> m_mapProgressGroupTreeItem;

	IEKG3DSceneSceneEditor*  m_lpSceneSceneEditor;
	KSceneEditorDocLogical* m_lpDocLogical;

	void BuildOrFindProgressTreeItems(int nProgressID, HTREEITEM* hItemNpc, HTREEITEM* hItemDoodad);
	void FillTree();

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CTreeCtrl m_Tree;
public:
	afx_msg void OnBnClickedButtonAdddoodadgroup();
public:
	afx_msg void OnBnClickedButtonAddnpcgroup();
public:
	afx_msg void OnBnClickedButtonRefresh();
public:
	afx_msg void OnBnClickedButtonAdd2();
public:
	afx_msg void OnTvnSelchangedTreeNpc(NMHDR *pNMHDR, LRESULT *pResult);
public:
	afx_msg void OnBnClickedButtonAddselectedentity();
public:
	afx_msg void OnEnChangeEditDirection();
public:
	CEdit m_Edit_Direction;
public:
	CButton m_Button_AddPoint;
public:
	CButton m_Button_AddSelectedEntity;
public:
	CEdit m_Edit_SetName;
public:
	CButton m_Button_RemoveSelectedEntity;
public:
	virtual BOOL OnInitDialog();
public:
	afx_msg void OnEnChangeEditSetname();
public:
	int m_nDirection;
public:
	afx_msg void OnBnClickedButtonAddselectedentity2();
public:
	afx_msg void OnNMDblclkTreeNpc(NMHDR *pNMHDR, LRESULT *pResult);
public:
	afx_msg void OnBnClickedButtonDelete();
public:
	afx_msg void OnBnClickedButtonAddaigroup();
public:
	afx_msg void OnBnClickedCheckKing();
public:
	CButton m_Check_King;
public:
	afx_msg void OnCbnSelchangeComboWaypointse();
public:
	CComboBox m_Combo_Way;
public:
	afx_msg void OnBnClickedButtonBrown();
public:
	CEdit m_Edit_ArrayFile;
public:
	afx_msg void OnBnClickedButtonClear();
public:
	afx_msg void OnEnChangeEditMatrixfile();
public:
	CButton m_Button_ArrayBrown;
public:
	CButton m_Button_ArrayClear;
public:
	afx_msg void OnBnClickedButtonFitgroup();
public:
	afx_msg void OnBnClickedButtonFitarray();
public:
	CButton m_Button_FitGroup;
public:
	CButton m_Button_FitArray;
public:
	afx_msg void OnEnChangeEditArrayid();
public:
	CEdit m_Edit_AIArrayID;
public:
	int m_nAIArrayID;
};
