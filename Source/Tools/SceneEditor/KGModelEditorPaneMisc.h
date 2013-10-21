

#ifndef _KGMODELEDITORPANEMISC_H_
#define _KGMODELEDITORPANEMISC_H_

#include "IEEditor.h"
#include "afxcmn.h"

class IEKG3DBillBoardCloudCreater;

class KGModelEditorPaneMisc : public CPropertyPage
{
	DECLARE_DYNAMIC(KGModelEditorPaneMisc)

public :
	KGModelEditorPaneMisc();
	virtual ~KGModelEditorPaneMisc();

	enum { IDD = IDD_MODELEDITOR_MISC };

    void UpdateInfo();
    void SetCurModel(IEKG3DModel* pModel)    { m_pModel = pModel; UpdateInfo(); }
    void FillTree(CString strPath, HTREEITEM hItem);
    void BuildBillboardCloud();


public :
    IEKG3DBillBoardCloudCreater* m_lpBillBoardCreater;
    IEKG3DSceneModelEditor*      m_pModelEditor;
    IEKG3DModel*                 m_pModel;
    float                        m_fPrecision;
    CString                      m_strMeshInfo;
    BOOL                         m_bTwitterPlane;
    CString                      m_strFileName;
    CString                      m_szCount;
    CTreeCtrl                    m_MeshTree;
    CProgressCtrl                m_Progress;
    BOOL                         m_bBatch;
    BOOL                         m_bPause;

    std::vector<CString>         m_vecModelNames;

    int                          m_nCurIndex;

    BOOL                         m_bBuildTrunk;


protected :
	virtual void DoDataExchange(CDataExchange* pDX);  

	virtual	BOOL OnSetActive();

	DECLARE_MESSAGE_MAP()
public :
    afx_msg void OnBnClickedButtonCreate();
    afx_msg void OnTimer(UINT_PTR nIDEvent);
    afx_msg void OnEnChangeEditK();
    afx_msg void OnBnClickedCheck();
    afx_msg void OnBnClickedButtonSave();
    afx_msg void OnBnClickedButtonBat();
    afx_msg void OnBnClickedButtonAdd();
    afx_msg void OnTvnSelchangedTree(NMHDR *pNMHDR, LRESULT *pResult);

    
    afx_msg void OnBnClickedButtonStp();
    afx_msg void OnBnClickedButtonAdd2();
	afx_msg void OnBnClickedButtonOption();
	afx_msg void OnBnClickedButtonShaderUpdate();
	afx_msg void OnBnClickedButtonProcessShow();
	afx_msg void OnBnClickedButtonStep();
	afx_msg void OnBnEnableNormalCheck();
	afx_msg void OnEnChangeEdit1();
	afx_msg void OnBnClickedButtonComputesize();
	afx_msg void OnBnClickedButtonSetsize();
	int m_nTextureWidth;
	afx_msg void OnBnClickedButtonCreate2();
};

#endif