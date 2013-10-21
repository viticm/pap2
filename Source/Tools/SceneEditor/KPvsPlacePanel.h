#pragma once

#include "KGLilisSlipPanel.h"
#include "KEnterEdit.h"

class IEKG3DScenePvsEditor;

class KPvsPlacePanel : public KGLilisSlipPanel<CPropertyPage>
{
	DECLARE_DYNAMIC(KPvsPlacePanel)

public :
    IEKG3DScenePvsEditor* GetPvsScene();
    BOOL OnInitDialog();

	KPvsPlacePanel();
	virtual ~KPvsPlacePanel();

// Dialog Data
	enum { IDD = IDD_KPVSPLACEPANEL };

protected :
    void SubFillTree(LPSTR Dir, LPSTR pFileType, HTREEITEM hStart);
    void FillTree();
    void GetItemFilePath(HTREEITEM hItem, CString& RetPath);
    bool Filter(LPCTSTR str);

protected :
    CTreeCtrl   m_treeObj;
    CTreeCtrl   m_treePtl;
    CImageList  m_imageListTree;
    int m_nOutputWindowID;

    BOOL        m_bFilterMesh;
    BOOL        m_bFilterSfx;
    BOOL        m_bFilterMeshSet;
    BOOL        m_bFilterPvs;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
	afx_msg void OnNMClickTree(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnSize(UINT nType, int cx, int cy);
    afx_msg void OnBnClickedButtonRf();
    afx_msg void OnDestroy();
    afx_msg void OnBnClickedCheckMesh();
    afx_msg void OnBnClickedCheckMeshset();
    afx_msg void OnBnClickedCheckSfx();
    afx_msg void OnBnClickedCheckPvs();
public:
    afx_msg void OnBnClickedCheckCull();
    afx_msg void OnBnClickedButtonPointlight();
};
