////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KGObjectPropertyEditDlg.h
//  Version     : 1.0
//  Author      : ZhaoXinyu (Chiyer)
//  CopyRight   : Kingsoft Corporation All rights Reserved
//  History     : 2005-11-6 20:08:42
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _KGOBJECTPROPERTYDLG_H_
#define _KGOBJECTPROPERTYDLG_H_

#include "KObjectEditorDoc.h"
//#include "KG3DEngineManager.h"
#include "KGObjectPropertyEdit.h"
#include "resource.h"
#include "KGPropertyListCtrl.h"
#include "AfxTempl.h"

/*
#define OBJECT_OBJECT_PATH	"物件\\物件\\"                      
#define OBJECT_NPC_PATH		"物件\\NPC\\"     
#define OBJECT_PROP_PATH	"物件\\道具\\"   
#define OBJECT_MAGIC_PATH	"物件\\魔法\\"   
#define OBJECT_EFFECT_PATH	"物件\\特效\\"   
#define OBJECT_SKILL_PATH	"物件\\技能\\"   
*/

#define OBJECT_NPC_AIN_DIRECTORY    "NPC动作配置表\\"   
#define NPC_ANI_INI_NAMEPLATE_FILE  "NPC动作配置表\\nameplate.ini"   
#define NPC_ANI_INI_TEMPLATE_FILE   "NPC动作配置表\\template.ini" 

class KSceneObjectEditorPanelView;

// The Call Type Of The Function : GetChildPoint(...)
enum ENUM_CALL_TYPE
{
	TYPE_FIRST,                                     
	TYPE_OTHER                        
};

// The Property List Item Buffer (Array) Store The Section And Key Name
class PropertyListItem
{
public:
	CString m_szSectionName;
	CString m_szKeyName;
	PropertyListItem()
	{
	}
	PropertyListItem(CString szSectionName, CString szKeyName)
	{
		m_szSectionName = szSectionName;
		m_szKeyName     = szKeyName;
	}
};

class KGObjectPropertyEditDlg : public CDialog
{
	DECLARE_DYNAMIC(KGObjectPropertyEditDlg)

public:
	KGObjectPropertyEditDlg(
		PROPERTY_EDIT_TYPE eType = PROPERTY_EDIT_TYPE_OBJECT,
		KSceneObjectEditorPanelView* pView = NULL,
		CWnd* pParent = NULL
	);   
	virtual ~KGObjectPropertyEditDlg();
	enum { IDD = IDD_OBJECT_PROPERTY };

public:
	int GetChildPoint(HTREEITEM hParent, CString szPath, ENUM_CALL_TYPE nType);
	int GetRootPoint(CString szPath);
	int UpdateDataTree();
	//int UpdateDataObjectProperty(NPC_PROPERTY* lpNpc);
	//int UpdateDataObjectProperty(OBJECT_PROPERTY* lpObj);
	int ShowPropertyTable(
		CString szNamePlate, CString szTemplate, CString szIniFile
	);
	ITEM_VILUE_TYPE GetKeyType(CString szKeyType);
	
	int  LoadConfig(const TCHAR pcszAbsName[]);
	int  CheckIniValidity();
	int  TrimIniFile();
	int  Init();
	int  UnInit();
	
	void SetSceneObjectEditorDoc(KSceneObjectEditorDoc* pScene)
	{
		m_lpSceneObjectEditorDoc = pScene;
		m_listProperty.SetScene(pScene->GetScene());
	}

public:
	KSceneObjectEditorPanelView* m_pView;
	KSceneObjectEditorDoc*		 m_lpSceneObjectEditorDoc;
	PROPERTY_EDIT_TYPE			 m_eEditType;
	
	CString    m_szCurIniFileName;
	KGPropertyListCtrl m_listProperty; 
	CTreeCtrl  m_treeObjectView;  
	HTREEITEM  m_treeRoot; 
    int        m_nCurIndex;

	BOOL       m_bModify;

	IIniFile*  m_pIniFile;

	CButton    m_bnSave;
	CButton    m_bnOK;
	CArray<PropertyListItem, PropertyListItem&> m_arrayPropertyListItem;

	CString    m_szDirectory;
	CString    m_szNamePlate;
	CString    m_szTemplate;

	CString    m_szWindowTextt;

protected:
	virtual void DoDataExchange(CDataExchange* pDX); 
    virtual BOOL OnInitDialog();
    afx_msg void OnSize(UINT nType, int cx, int cy);

	DECLARE_MESSAGE_MAP()
	afx_msg void OnTvnSelchangedObjectPropertyTree(
		NMHDR *pNMHDR, LRESULT *pResult
	);
    afx_msg void OnNMDblclkObjectPropertyList(NMHDR *pNMHDR, LRESULT *pResult);
public:
	afx_msg void OnBnClickedSave();
	afx_msg void OnBnClickedOk();
	afx_msg void OnSetting();
	afx_msg void OnBnClickedCancel();	
	afx_msg void OnNMRclickObjectPropertyList(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMRclickObjectPropertyTree(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnTvnKeydownTree(NMHDR *pNMHDR, LRESULT *pResult);
};

#endif // _KGOBJECTPROPERTYDLG_H_