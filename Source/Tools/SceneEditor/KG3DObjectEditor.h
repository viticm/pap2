////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KG3DObjectEditor.h
//  Version     : 1.0
//  Author      : ZhaoXinyu (Chiyer)
//  CopyRight   : Kingsoft Corporation All rights Reserved
//  History     : 2006-3-20 13:27:49
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#ifndef  _KG3DOBJECTEDITOR_H_
#define  _KG3DOBJECTEDITOR_H_

#include "KGObjectEditerProjectListDlg.h"
#include "KGPropertyListCtrl.h"
#include "KGObjectEditerTreeCtrl.h"
#include "KGControlColumnDlg.h"
//#include "KG3DSceneObjectEditor.h"
#include "IEEditor.h"//by dengzhihui Refactor 2006年11月15日

class KSceneObjectEditorPanelView;

// Defind The Object Property Editer Configure Ini File
#define  SETTING_INI_FILE_NAME    _T("settings\\ObjectEditer_Setting.ini")

#define  INIT_RECT_TABCTRL_RIGHT  220		// Initiatory Right Side Of TabCtrl
#define  INIT_LIST_CTRL_WIDTH     314		// Initiatory Width Of ListCtrl
#define  RENDER_DELAY_TIME		  10		// The Delay Of The Render Timer

class KGSubEditer : public CObject
{
public :
	char		m_szTabFilePath_Represent[MAX_PATH];
	char		m_szIniFilePath_Represent[MAX_PATH];
	char		m_szTabFilePath_GameLogic[MAX_PATH]; 
	char        m_szIniFilePath_GameLogic[MAX_PATH];

	char		m_szClassifyFilePath[MAX_PATH];

	char		m_szSubEditerName[64];

	ITabFile*	m_pRepresentTabFile;
	IIniFile*	m_pRepresentIniFile;
	ITabFile*	m_pGameLogicTabFile;
	IIniFile*	m_pGameLogicIniFile;

	IIniFile*	m_pClassifyFile;

	KGPrimaryKeyManager m_primaryKeyManager_L;
	KGPrimaryKeyManager m_primaryKeyManager_R;

public :
	int UniteTabFile();
	int IsValidate();
	int Save(const char pcszFileName[] = NULL);
	int CheckConsistency();

public :
	KGSubEditer();
	virtual ~KGSubEditer();
};


class KG3DViewWindow : public CWnd
{
	DECLARE_DYNAMIC(KG3DViewWindow)
public :
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
public :
	KG3DViewWindow(){};
	virtual ~KG3DViewWindow(){};
	DECLARE_MESSAGE_MAP()
};


class KG3DObjectEditor : public CDialog
{
	DECLARE_DYNAMIC(KG3DObjectEditor)

public :
	int OpenDocument(const char pcszConfigFileName[]);

	int InitInterface();			
	int UpdateInterface();
	int UnInitInterface();
	
	int InitSubEditer();
	int UpdateSubEditer();
	int CheckSubEditerConsistency();
	int UnInitSubEditer();

	int InitScene();
	int UpdateScene();	
	int UnInitScene();

	int BuildOneObject(const char pcszObjectName[]);
	int Save();

public :
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual BOOL OnInitDialog();

public :
	KG3DObjectEditor(CWnd* pParent = NULL);  
	virtual ~KG3DObjectEditor();
	enum { IDD = IDD_OBJECT_EDITER };

public :
	KSceneObjectEditorPanelView*  m_pParentWindow;		
	KGObjectEditerProjectListDlg* m_pPropertyList;		
	KGControlColumnDlg*			  m_pControlPanel;		

	//
	//	Interface Pertinent
	//

	KGObjectEditerTreeCtrl		  m_treeCtrlObjectList;
	CTabCtrl					  m_tabCtrlClassify;
	KG3DViewWindow				  m_wndObjectView;
	CStatusBar					  m_wndStateBar;
	CToolBar					  m_wndToolBar;

	CRect						  m_rectListCtrl;
	CRect						  m_rectStateBar;
	CRect						  m_rectToolBar;
	CRect						  m_rectTabCtrl;
	CRect						  m_rectObjView;
	CRect						  m_rectControl;

	int							  m_nLSizeBarFlag;
	int							  m_nRSizeBarFlag;
	int							  m_nLButtonFlag;
	int							  m_nHaveShowFlag;

	CImageList                    m_imageListTree;
	CMenu						  m_menuPopup;

	//
	//	Editer Pertinent
	//

	IEKG3DSceneObjectEditor*		  m_pObjectScene;//by dengzhihui Refactor 2006年11月15日
	CArray<KGSubEditer*>		  m_arraySubEditer;
	int							  m_nCurSubEditerIndex;
	int							  m_nTabFilePos;

					
protected :
	virtual void	DoDataExchange(CDataExchange* pDX);   
	afx_msg void	OnTvnSelchangedObjectPropertyTree(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void	OnNMRclickObjectPropertyTree(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void	OnInitMenuPopup(CMenu* pPopupMenu, UINT nIndex, BOOL bSysMenu);

	afx_msg HBRUSH	OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg LRESULT OnListItemUpdate(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnUpdateScene(WPARAM wParam, LPARAM lParam);
	afx_msg BOOL	OnToolTipText(UINT, NMHDR* pNMHDR, LRESULT* pResult);
	
	afx_msg void	OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void	OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void	OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void	OnMouseMove(UINT nFlags, CPoint point);
    afx_msg void	OnSize(UINT nType, int cx, int cy);	
	afx_msg void	OnMove(int x, int y);
	
	afx_msg void	OnNewDirSlib();
	afx_msg void	OnNewDirChld();
	afx_msg void	OnNewObject();
	
	afx_msg void	OnSave();
	afx_msg void	OnDel(); 
	afx_msg void	OnCopy();
	afx_msg void	OnCut();
	afx_msg void	OnPase();
	afx_msg void	OnReName();
	
	afx_msg void	OnTimer(UINT nIDEvent);
	afx_msg void	PostNcDestroy();
	afx_msg void	OnOK();
	afx_msg void	OnCancel();
	afx_msg void	OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized);
	afx_msg void	OnCommandShowListWindow();
	afx_msg void	OnCommandShowControlColumn();
	afx_msg void	OnTcnSelchangeTab(NMHDR *pNMHDR, LRESULT *pResult);

	DECLARE_MESSAGE_MAP()
};

class KGInitDlg : public CDialog
{
public :
	CProgressCtrl m_progressCtrl;
	CStatic		  m_staticCtrl;
public :
	KGInitDlg(CWnd* pParent = NULL){};  
	virtual ~KGInitDlg(){};
	enum { IDD = IDD_INIT_DLG };
protected :
	virtual void DoDataExchange(CDataExchange* pDX);  
};

extern TCHAR g_strWeaponDirectory[MAX_PATH];
extern TCHAR g_strItemDirectory[MAX_PATH];

#endif //_KG3DOBJECTEDITOR_H_ 