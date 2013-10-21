#pragma once
#include "afxwin.h"
#include "IEEditor.h"


// KSceneSceneEditorDialogTerrianCopy dialog

class KSceneSceneEditorDialogTerrianCopy : public CDialog
{
	DECLARE_DYNAMIC(KSceneSceneEditorDialogTerrianCopy)

public:
	KSceneSceneEditorDialogTerrianCopy(CWnd* pParent = NULL);   // standard constructor
	virtual ~KSceneSceneEditorDialogTerrianCopy();

// Dialog Data
	enum { IDD = IDD_SCENEEDITOR_TERRIANCOPY };

public:
	void SetSceneEditor(IEKG3DSceneSceneEditor* pSceneEdit)
	{
		m_lpSceneSceneEditor = pSceneEdit;	
	}
	//void SetSceneEditorView(KSceneEditorView *lpSceneEditorView)
	//{
	//	m_lpSceneEditorView = lpSceneEditorView;
	//}

protected:
	IEKG3DSceneSceneEditor*  m_lpSceneSceneEditor;
	IEKG3DRepresentTerrainBlock* m_pTerrainBlock;
	BOOL m_bIsOn;
	//KSceneEditorView *m_lpSceneEditorView;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedSelect();
	CButton m_TerrianSelect;
	CEdit editName;
	void InitBlockList();

	afx_msg void OnBnClickedButton4();
	afx_msg void OnBnClickedButton2();
	afx_msg void OnBnClickedButton3();
	CListBox TerrainBlockList;
	afx_msg void OnLbnDblclkList1();
	CEdit editMixValue;
	CButton buttonIsPasteTerrain;
	CButton buttonIsPasteBlend;
	CButton buttonIsPasteObject;
	CButton buttonIsMax;
	int m_nMixtype;
	CEdit editWidth;
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
};
