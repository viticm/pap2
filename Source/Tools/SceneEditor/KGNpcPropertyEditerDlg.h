////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KGNpcPropertyEditerDlg.h
//  Version     : 1.0
//  Author      : ZhaoXinyu (Chiyer)
//  CopyRight   : Kingsoft Corporation All rights Reserved
//  History     : 2006-5-25 12:23:39
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _INCLUDE_KGNPCPROPERTYEDITERDLG_H_
#define _INCLUDE_KGNPCPROPERTYEDITERDLG_H_

#define NPC_TEMPLATE_TABLE_PATH TEXT("\\settings\\NpcTemplate.tab")

#include "KGPropertyTreeCtrl.h"
#include "KGPropertyListCtrl.h"

#include "KSO3World.h"
#include "IEKG3DScene.h"
#include "Global.h"
#include "KRegion.h"
#include "SO3GlobalDef.h "
#include "KGObjectEditerProjectListDlg.h"
#include "KSceneEditorDocLogical.h"

#define  SCTRPT_NAME_FILE "Scripts\\ScriptName.ini"

#define  CTL_BROWSE		100
#define  CTL_NUMBER     150

enum
{
    PROPERTY_EDIT_TYPE_NPC,
    PROPERTY_EDIT_TYPE_DOODAD
};

class KGListCtrlNPC : public KGListCtrl
{
public :
	int BandNpcData(KSceneEditorDocLogical::NPCInfo* pNpcData,KSceneEditorDocLogical* pDoc);
    int BandDoodadData(KSceneEditorDocLogical::DoodadInfo* pDoodadData,KSceneEditorDocLogical* pDoc);
	int BandDoodadData(list<KSceneEditorDocLogical::DoodadInfo*>& pListDoodadData,KSceneEditorDocLogical* pDoc, KSceneEditorDocLogical::DoodadInfo* pParentDoodadData);

	//by huangjinshou 2007Äê7ÔÂ18ÈÕ
	int BandNpcData(KSceneEditorDocLogical::NPCInfo* pNpcData,map<DWORD,KSceneEditorDocLogical::WayPointSet*> pListWayPointSet,KSceneEditorDocLogical* pDoc);
    int BandNpcData(list<KSceneEditorDocLogical::NPCInfo*>& pListNpcData,KSceneEditorDocLogical* pDoc, KSceneEditorDocLogical::NPCInfo* pNpcData, map<DWORD,KSceneEditorDocLogical::WayPointSet*> pmapWayPointSet);
	//
	int Clear();
	KGListCtrlNPC();
	virtual ~KGListCtrlNPC();
  
public :
	CSpinButtonCtrl m_spinCtrl;
	CComboBox		m_comboxCtrl;
	KGClassButton	m_bnBrowse;
    CEdit           m_EditText;
	int				m_nCurItem;
	int             m_nSelCount;
	KSceneEditorDocLogical::NPCInfo* m_pNpcData;
	KSceneEditorDocLogical::DoodadInfo* m_pDoodadData;
	map<DWORD,KSceneEditorDocLogical::WayPointSet*> m_mapWayPointSet;
	list<KSceneEditorDocLogical::NPCInfo*> m_listNpcData;
	list<KSceneEditorDocLogical::DoodadInfo*> m_listDoodadData;

    int             m_nType; 
    IEKG3DScene*	m_lpEditScene;
	KSceneEditorDocLogical* m_lpDocLogical;

	BOOL DlgModifyAISet(KSceneEditorDocLogical::NPCInfo*);


public :
	DECLARE_MESSAGE_MAP()
	afx_msg void OnDeltaposSpin(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBrowseClick();
	afx_msg void OnCbnSelect();
	afx_msg void OnNMClick(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnEnChangeEdit();
    afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
};


class KGNpcPropertyEditerDlg : public CDialog
{
	DECLARE_DYNAMIC(KGNpcPropertyEditerDlg)

private :
    int _HighlightItem(int nIndex);
	KSceneEditorDocLogical* m_lpDocLogical;

public :

	KGNpcPropertyEditerDlg(IEKG3DScene* lpEditScene,KSceneEditorDocLogical* pDocLogical,CWnd* pParent = NULL, int nType = PROPERTY_EDIT_TYPE_NPC);
	virtual ~KGNpcPropertyEditerDlg();
	enum { IDD = IDD_DIALOG_EDIT_NPC_PROPERTY };

	int  FillListCtrlForUniformAttribute();
	int  FillListCtrl();
	BOOL OnInitDialog();

    // --------- npc date conversion --------
    static CString KindIDToText(int nKind, int nType = PROPERTY_EDIT_TYPE_NPC);
    static DWORD   TextToKingID(CString strText, int nType = PROPERTY_EDIT_TYPE_NPC);
    static CString AITypeToText(int byAIType);
    static DWORD   TextToAIType(CString strText);

public :
	KGPropertyTreeCtrl	m_treeCtrl;
	KGListCtrlNPC	m_listCtrl;

	CImageList  m_imageListTree;
	IEKG3DScene*	m_lpEditScene;
	std::list<KSceneEditorDocLogical::NPCInfo*>    m_listNpcData;
	std::list<KSceneEditorDocLogical::DoodadInfo*> m_listDoodadData;
	void BackupNpcDoodadData();
	void ReloadBackupNpcDoodadData();
	std::list<KSceneEditorDocLogical::NPCInfo*>    m_listNpcDataBak;
	std::list<KSceneEditorDocLogical::DoodadInfo*> m_listDoodadDataBak;
	
	HTREEITEM m_hItemParent;
	KSceneEditorDocLogical::NPCInfo*	m_pParentNpcData;
	//KSceneEditorDocLogical::DoodadInfo* m_pParentDoodadData;
	KSceneEditorDocLogical::NPCInfo*    m_plogicNpcData;
	KSceneEditorDocLogical::NPCInfo*    m_plogicNpcDataBak;
    KSceneEditorDocLogical::DoodadInfo* m_plogicDoodadData;
	KSceneEditorDocLogical::DoodadInfo* m_plogicDoodadDataBak;
    int         m_nType; 
    int         m_nRenderWndID;

    IEKG3DRepresentObject* m_pHandelObj;
	
	int         m_nSelCount;
protected :
	virtual void DoDataExchange(CDataExchange* pDX);
	DECLARE_MESSAGE_MAP()

    void OnShowWindow(BOOL bShow, UINT nStatus);
    void OnOK();
    void OnCancel();

public :
    afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnTvnSelchangedTree(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedRefresh();
};


#endif //_INCLUDE_KGNPCPROPERTYEDITERDLG_H_
