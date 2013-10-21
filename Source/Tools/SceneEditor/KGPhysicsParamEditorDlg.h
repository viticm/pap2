#pragma once

#include "KG3DTypes.h"

#define PHYSISC_PRARM_FILE_NAME "data\\public\\physics.ini"

class KGPhysicsParamEditorDlg : public CDialog
{
	DECLARE_DYNAMIC(KGPhysicsParamEditorDlg)

public:
	KGPhysicsParamEditorDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~KGPhysicsParamEditorDlg();

// Dialog Data
	enum { IDD = IDD_PHYSICS_PARAM_EDITOR };

    BOOL OnInitDialog();

    HRESULT SetSceneEditor(IEKG3DScene* pEditor)
    {
        m_lpSceneEditor = pEditor;
        return S_OK;
    }

protected:
    virtual void DoDataExchange(CDataExchange* pDX);   

    DECLARE_MESSAGE_MAP()

    afx_msg void OnBnClickedApply();
    afx_msg void OnBnClickedOk();
    afx_msg void OnBnClickedCancel();
    afx_msg void OnTvnSelchangedTree(NMHDR *pNMHDR, LRESULT *pResult);

private:
    void    _UpdateToMap(HTREEITEM hItem);
    void    _UpdateToUi(HTREEITEM hItem);
    void    _UpdateUi();

private:
    KG3DPhysiscParam m_ParamCur;
    std::map<int, KG3DPhysiscParam> m_mapParams;

    CTreeCtrl m_Tree;

    IEKG3DScene* m_lpSceneEditor;
};
