////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KGPropertyEditTabDlg.h
//  Version     : 1.0
//  Author      : ZhaoXinyu (Chiyer)
//  CopyRight   : Kingsoft Corporation All rights Reserved
//  History     : 2005-11-29 10:18:24
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _KGOBJECTPROPERTYTABDLG_H_
#define _KGOBJECTPROPERTYTABDLG_H_

#include "KObjectEditorDoc.h"
//#include "KG3DEngineManager.h"
#include "KGObjectPropertyEdit.h"
#include "resource.h"
#include "KGPropertyListCtrl.h"
#include "AfxTempl.h"
#include "KGPropertyTreeCtrl.h"
#include "KGProperytEditFinderDlg.h"

// The Max Length Of The Tab File Item String
#define MAX_LENGTH 	      1224      

// The Max Size Of The BookMark Text
#define BOOKMARK_SIZE     4000

// The Max Size Of The Ini Section Name
#define SECTION_NAME_SIZE 64

// The Max Size Of The Ini Key Name
#define KEY_NAME_SIZE     64

// Define The File Path
#define QUEST_NAMEPLATE_PATH  "settings\\QuestNamePlate.ini"

#define TEMP_QUEST_PATH       "__TempQuestFile__.tab"

#define BACK_UP_PATH		  "quest_editer_backup"

#define WM_USER_UPDATE_TREE_ICON WM_USER + 100
#define WM_USER_BACKUP_TAB_FILE  WM_USER + 200

class KSceneObjectEditorPanelView;

// Define The Quest Type
enum QUEST_TYPE
{
	QUEST_MAP,
	QUEST_LNK,
	QUEST_MAK,
	QUEST_GLN,
	QUEST_KIL,
	QUEST_POS,
	QUEST_TAK,
	QUEST_OFR,
	QUEST_OTR,

	QUEST_COUNT
};

enum EDIT_STATE
{
	EDIT_STATE_CLOSE,
	EDIT_STATE_EDIT_PROPERTY,
	EDIT_STATE_EDIT_BOOKMARK,
	EDIT_STATE_EDIT_SUBSETBOOKMARK,
};

class KGObjectPropertyEditTabDlg : public CDialog
{
	DECLARE_DYNAMIC(KGObjectPropertyEditTabDlg)

public:
	KGObjectPropertyEditTabDlg(
		PROPERTY_EDIT_TYPE eType = PROPERTY_EDIT_TYPE_OBJECT,
		KSceneObjectEditorPanelView* pView = NULL, 
		CWnd* pParent = NULL
	);   
	virtual ~KGObjectPropertyEditTabDlg();
	enum { IDD = IDD_OBJECT_PROPERTY }; 

public:
	int  UpdateTaskTree();
	int  UpdateProperty();

	int  OnClickTaskTree();

	int  Init();
	int  UnInit();
	
	int UpdateGrid(int nEnbale);
	int UpdateToolBarState();

	int CreateOneQuest(QUEST_TYPE eQuestType); 

	int CheckValidate();

	void SetSceneObjectEditorDoc(KSceneObjectEditorDoc* pScene)
	{
		m_lpSceneObjectEditorDoc = pScene;
		m_listProperty.SetScene(pScene->GetScene());
	}

	int CopyIniFileSubset(HTREEITEM hTreeItem, IIniFile* pIniFile); 
	
	// Temp //////////////////////////////////////////////////////////////////////////
	int UpdateStateBar();
	int BackUp();
	static int UpdateDependData(CTreeCtrl* pTree, HTREEITEM hTreeItem);
	//
	
	static int GetImageIndex(ITabFile* pTabFile, int nRow);
	
public:
	KSceneObjectEditorPanelView* m_pView;
	KSceneObjectEditorDoc*		 m_lpSceneObjectEditorDoc;
	PROPERTY_EDIT_TYPE			 m_eEditType;	

	KGPrimaryKeyManager			 m_primaryKeyManager;
	KGPropertyListCtrl			 m_listProperty; 
	KGPropertyTreeCtrl			 m_treeObjectView; 
	CEdit						 m_editBookMarker;
	CStatic						 m_iniWinidow;  
	CMenu						 m_menuPopup;
	HACCEL						 m_hAccel;
	CComboBox					 m_comBoxType;
	CToolBar					 m_wndToolBar;
	CStatusBar					 m_wndStateBar;
	CProgressCtrl				 m_proessBar;

	int					m_nRefustCountFlag;
	int					m_nFirstResizeFlag;
	int				    m_nCurRow; 
	int					m_nSizeBarFlag;
	int					m_nLButtonFlag;
	int					m_nBeSavedFlag;

	ITabFile*			m_pTabFile;
	IIniFile*			m_pIniFile;
	IIniFile*			m_pQuestIniFile;

	HTREEITEM			m_hTreeRootItem;
	HTREEITEM			m_hTreeSelItem;

	CImageList			m_imageList;
	CImageList			m_NormalList;
	CImageList			m_HotList;
	CImageList			m_DisableList;

	CString				m_szPropertyFileName;
	CString				m_szNamePlateFileName;
	CString				m_szPropertySaveFile;
	CString				m_szCurBookMarkName;
	CString				m_szPreBookMarkName;
	CString				m_szBookMark;

	CRect				m_rectList;
	CRect				m_rectTree;
	CSize			    m_RectSave;

	char**				m_ppszClassifyFields;
	int					m_nClassifyFieldsSize;
	char*				m_pszCaptionField;
	char*				m_pszPrimaryKesField;

	EDIT_STATE			m_nEditerState;

protected:
	virtual void DoDataExchange(CDataExchange* pDX); 
	virtual BOOL PreTranslateMessage(MSG* pMsg);

    afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnTvnSelchangedObjectPropertyTree(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMDblclkObjectPropertyList(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMRclickObjectPropertyTree(NMHDR *pNMHDR, LRESULT *pResult);

	afx_msg BOOL OnToolTipText(UINT, NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnInitMenuPopup(CMenu* pPopupMenu, UINT nIndex, BOOL bSysMenu);
	afx_msg void  OnTimer(UINT_PTR nIDEvent);

	afx_msg void OnCreateMap();
	afx_msg void OnCreateLnk();
	afx_msg void OnCreateSub();
	afx_msg void OnCreateGln();
	afx_msg void OnCreateKil();
	afx_msg void OnCreatePos();
	afx_msg void OnCreateTlk();
	afx_msg void OnCreateOfr();
	afx_msg void OnCreateOtr();

	afx_msg void OnFind();
	afx_msg void OnDel(); 
	afx_msg void OnCopy();
	afx_msg void OnCut();
	afx_msg void OnPase();
	afx_msg void OnReName();

	afx_msg void OnSet();
	afx_msg void OnQuestSwitchOpen();
	afx_msg void OnQuestSwintcClose();

	afx_msg void OnOpen();
	afx_msg void OnClose();
	afx_msg void OnNew();	
	afx_msg void OnSave(); 
	afx_msg void OnSaveAs(); 

	afx_msg void OnCbnSelect();
	afx_msg void OnExpendAll();
	afx_msg void OnShrinkAll();

	afx_msg void OnQuestExportFileAll();
	afx_msg void OnQuestExportFile();
	afx_msg void OnQuestInportFileEx();
	afx_msg void OnQuestInportFile();

	afx_msg void OnHellp();

	afx_msg void OnOK();
	afx_msg void OnCancel();
	afx_msg HBRUSH  OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg LRESULT OnUpdateTreeIcon(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnBackUp(WPARAM wParam, LPARAM lParam);
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void PostNcDestroy();
	afx_msg void OnBnClickedCheckAnd();
	afx_msg void OnBnClickedCheckOr();

	DECLARE_MESSAGE_MAP() 
};

#endif // _KGOBJECTPROPERTYTABDLG_H_