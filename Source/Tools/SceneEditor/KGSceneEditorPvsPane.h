#pragma once
#include "afxwin.h"
#include "IEEditor.h"

interface IEKG3DSceneSceneEditor;
interface IEKG3DRepresentPVS;


class KGSceneEditorPvsPane : public CDialog
{
	DECLARE_DYNAMIC(KGSceneEditorPvsPane)

public:
	KGSceneEditorPvsPane(CWnd* pParent = NULL);   // standard constructor
	virtual ~KGSceneEditorPvsPane();

// Dialog Data
	enum { IDD = IDD_SCENEEDITOR_OBJPVS };


    void SetSceneEditor(IEKG3DSceneSceneEditor* pEditor)    { m_pEditor = pEditor; }
    void UpdateUI();
    void UpdataPortalList();

private :
    IEKG3DSceneSceneEditor* m_pEditor;
    IEKG3DRepresentPVS*     m_pPvs;
    BOOL                    m_bEditPortal;
    //ISelectFilter*          m_pSelectFilterSave;//<ISelectFilter Refactor>
    CListBox                m_PortalListCtrl;
    //PvsSelectFilter         m_PvsSelectFilter;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    BOOL PreTranslateMessage(MSG* pMsg);
    void OnOK();

	DECLARE_MESSAGE_MAP()
public :
    afx_msg void OnBnClickedButtonConver();
    afx_msg void OnBnClickedButtonAddPortal();
    //afx_msg void OnBnClickedCheckEditPortal();
    afx_msg void OnLbnSelchangeListPortal();
    afx_msg void OnBnClickedButtonDelPortal();
};
