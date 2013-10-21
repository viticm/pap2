#pragma once

//#include "cs3logicform.h"
#include "IEEditor.h"
#include "KListCtrl.h"
// KSceneSceneEditorDialogLogical dialog

//class KG3DSceneSceneEditor;


class KSceneEditorDoc;

class KSceneSceneEditorDialogLogical : public CDialog
{
	DECLARE_DYNAMIC(KSceneSceneEditorDialogLogical)

public:
	KSceneSceneEditorDialogLogical(CWnd* pParent = NULL);   // standard constructor
	virtual ~KSceneSceneEditorDialogLogical();

// Dialog Data
	enum { IDD = IDD_SCENEEDITOR_LOGICAL };

	void SetSceneEditor(IEKG3DSceneSceneEditor* pEditor);
	void SetSceneEditorDoc(KSceneEditorDoc* pDoc);

	void Save();
	void Load();
	void put_MapFileName(CString cstrPathName);
	void GetItem(long Index, BSTR * ItemName, long * LogicInfo, BSTR * ScriptFileName);
	long get_Count();
	void SetLogicCheck();
	void SetShowWindow(int nCurSel);
	void MoveWindowByOffset(int nX, int nY, DWORD dwWndID);
	void RefreshItemRect();
	void LoadLogicInfo();
	void RedrawColorWindow();
protected:
	IEKG3DSceneLogicalEditorBase*	m_lpSceneEditor;
	KSceneEditorDoc*				m_lpDoc;
	CButton							m_Check;
	CTabCtrl						m_TabCtrl;
	COLORREF						m_color;
	BOOL							m_bInit;
	KListCtrl						m_ListLogicData;
	BOOL							m_bShowOKCANCEL;
	BOOL							m_bRectPen;
	char							m_szFileName[MAX_PATH];
	char							m_szFilePath[MAX_PATH];
	eCellBrushShape					m_eBrushShape;
	int								m_nBrushWidth;
	int								m_nShowWidth;
	int								m_nCellHeight;
	char							m_szLogicFileName[MAX_PATH];
	//CString							m_strOldFileName;
	//CString							m_strOldFilePath;
	void SetChildWindowPos(int nCurSel, CRect rect);
	
	virtual BOOL OnInitDialog();
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//virtual void OnOK();
	//virtual void OnCancel();

	void OnTcnSelchangeList(NMHDR *pNMHDR, LRESULT *pResult);
	void OnTcnSelchangeTab(NMHDR *pNMHDR, LRESULT *pResult);
	//virtual void PostNcDestory(); 
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnSize(UINT nType, int cx, int cy);
public:
	void OnModifyDisplaySpeLogicInfo();
	
	void SetDocument(CDocument* pDoc, BOOL bSave);
private:
	//CS3LogicForm m_formobject;
	BOOL m_bObjInit;
	
	void SetEngineModifyStateByLogicInfo(BOOL bSelected, long lLogicInfo, LPCTSTR lpstrScriptFileName);
//)
public:
	afx_msg void OnCustomDrawSliderPenSize(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnSpinClickedPenSize(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnEnChangePenSize();
	afx_msg void OnCustomDrawSliderArea(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnSpinClickedArea(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnEnChangeArea();
	afx_msg void OnCustomDrawSliderCellHeight(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnSpinClickedCellHeight(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnEnChangeCellHeight();
	afx_msg void OnListItemRename(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnModifyBrushInfo();
	afx_msg void OnBnClickedBrowseScript();
	afx_msg void OnBnClickedDeleteScript();
	afx_msg void OnBnClickedClearalltrap();
	afx_msg void OnBnClickedCheck();
	afx_msg void OnBnClickedEditColor();
	afx_msg void OnBnClickedAdd();
	afx_msg void OnBnClickedDel();
	afx_msg void OnBnClickedOK();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnBnClickedLogicInfo();
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);

	//afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnDestroy();
	//afx_msg void OnPaint();
    afx_msg void OnBnClickedExportMeshData();
};

class LogicDataItem
{
protected:
	char m_szName[64];
	COLORREF m_color;
	BOOL m_bReadOnly;
	DWORD m_LogicMask;
	char m_szScriptFileName[MAX_PATH];
	char m_szScriptFilePath[MAX_PATH];
	//DWORD m_dwScriptID;
	CRect m_ClolorRect;

public:
	LogicDataItem();
	void SetName(const char* pszName);
	void GetName(char* pszNameBuf);
	void GetRect(CRect* pRect);
	void SetRect(CRect rect);
	void GetColor(COLORREF* pColor);
	void SetItemInfo(COLORREF color, BOOL ReadOnly, DWORD LogicMask, const char* pszFileName, const char* pszFilePath, CRect rect);
	void GetItemInfo(COLORREF* pColor, DWORD* pLogicMask, char* pszFileName, char* pszFilePath, CRect* pRect);
};