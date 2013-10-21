////////////////////////////////////////////////////////////////////////////////
//
//  FileName  : KGObjectManagerSheet.h
//  Author    : ZhaoXinyu 
//  CopyRight : Kingsoft Corporation All rights Reserved
//  History   : 2005.9.21 19:31 Create
//  Comment   : Objece Manager Sheet
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _H_KGOBJECTMANAGERSHEET_H_
#define _H_KGOBJECTMANAGERSHEET_H_

#pragma once
#include "KGObjectPropertyEdit.h"
//#include "KG3DEngineManager.h"
#include "KGListCtrl.h"
#define OBJECT_PATH ".\\Îï¼þ\\"                     // Default Object Directory

class KSceneObjectEditorDoc;

enum ENUM_GETDIR
{
    GD_FIRST,                                        
    GD_OTHER                      
};

class KGObjectManagerSheet : public CPropertyPage
{
	DECLARE_DYNAMIC(KGObjectManagerSheet)
public:
    int GetChildPoint(HTREEITEM hParent, CString szPath, ENUM_GETDIR nType);
    int GetRootPoint(CString szPath);

    //int UpdateDataObjectProperty(NPC_PROPERTY* lpNpc);
    //int UpdateDataObjectProperty(OBJECT_PROPERTY* lpObj);

    void SetSceneObjectEditorDoc(KSceneObjectEditorDoc* pScene)
    {
        m_lpSceneObjectEditorDoc = pScene;
    }

public:
	KGObjectManagerSheet();
	virtual ~KGObjectManagerSheet();

public:
	enum { IDD = IDD_OBJECTMANAGE_SHEET };

public:
    CTabCtrl	   m_tabObjectManageSheet;  
    CTreeCtrl      m_treeMeshView;          
    KGListCtrl     m_listProperty;          
    HTREEITEM      m_treeRoot;              
    int            m_nCurIndex;
    CString        m_szCurSheet;

    KSceneObjectEditorDoc*  m_lpSceneObjectEditorDoc;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    

	DECLARE_MESSAGE_MAP()
public:
    afx_msg void OnTcnSelchangeObjectManagePage(NMHDR *pNMHDR, LRESULT *pResult);
    virtual BOOL OnInitDialog();
    afx_msg void OnTvnSelchangedObjectPropertyTree(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnLvnItemchangedObjectPropertyList(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnHdnItemclickObjectPropertyList(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnLvnColumnclickObjectPropertyList(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnNMRclickObjectPropertyTree(NMHDR *pNMHDR, LRESULT *pResult);  
    afx_msg void OnNMDblclkObjectPropertyList(NMHDR *pNMHDR, LRESULT *pResult);
};

#endif // _H_KGOBJECTMANAGERSHEET_H_