#pragma once

#include "KGLilisSlipPanel.h"
#include "KEnterEdit.h"
#include "IEKG3DSceneSFXEditor.h"
#include "KSceneSceneEditorWaterPanel.h"

class IEKG3DScenePvsEditor;

class KPvsAttriPanel : public KGLilisSlipPanel<CPropertyPage>, public IPvsEditorEventCallback
{
	DECLARE_DYNAMIC(KPvsAttriPanel)

public :
    IEKG3DScenePvsEditor* GetPvsScene();
    BOOL OnInitDialog();
    BOOL OnSetActive();
	void UpdateAttri();

	KPvsAttriPanel();
	virtual ~KPvsAttriPanel();

    virtual void OnAddObject(IEKG3DRepresentObject* obj);
    virtual void OnDelObject(IEKG3DRepresentObject* obj);
    virtual void OnObjModify(IEKG3DRepresentObject* obj);
	virtual void OnSelectedChanged();

	enum { IDD = IDD_KPVSATTRIPANEL };

protected :
	HTREEITEM m_hModelRoot;
	HTREEITEM m_hSFXRoot;
	HTREEITEM m_hLightRoot;

    CString m_strOW;
    CString m_strIW;

    CTreeCtrl m_treeSub;
    bool      m_bRegEvent;

	CColorWnd m_ColorWndPLight;
	D3DCOLORVALUE m_PointLightColor;

protected:

    void UpdateSubTree();
	void OnDelObjectForNext(IEKG3DRepresentObject* obj,HTREEITEM hItemRoot);

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
    afx_msg void OnBnClickedButtonIw();
    afx_msg void OnBnClickedButtonOw();

	afx_msg LRESULT OnColorChanged(WPARAM wParam, LPARAM lParam);
    afx_msg void OnNMClickTreeSubObj(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnTvnSelchangedTreeSubObj(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnSize(UINT nType, int cx, int cy);
    afx_msg void OnBnClickedCheckW();
    afx_msg void OnBnClickedCheckN();
    afx_msg void OnBnClickedButtonDelW();
    afx_msg void OnBnClickedButtonDelN();
	afx_msg void OnStnClickedColorPlight();
	afx_msg void OnCbnSelchangeComboLightmapsize();
	afx_msg void OnBnClickedCheckLightmap();
	afx_msg void OnBnClickedButtonAutoLightMapsize();
	afx_msg void OnBnClickedButtonComputeLightmap();
	afx_msg void OnBnClickedButtonComputeselected();
	afx_msg void OnBnClickedButtonCall();
	afx_msg void OnBnClickedButtonCall2();
	afx_msg void OnBnClickedCheck1();
	afx_msg void OnBnClickedCheck33();
};
