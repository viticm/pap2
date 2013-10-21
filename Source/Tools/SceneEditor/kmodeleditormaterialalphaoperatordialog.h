#if !defined(AFX_KMODELEDITORMATERIALALPHAOPERATORDIALOG_H__B02B2B73_DBD1_4432_88DF_ECF3E9651929__INCLUDED_)
#define AFX_KMODELEDITORMATERIALALPHAOPERATORDIALOG_H__B02B2B73_DBD1_4432_88DF_ECF3E9651929__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// KSceneModelEditorMaterialAlphaOperatorDialog.h : header file
//
#include "KGlog.h"
#include <vector>
#include "afxwin.h"
#include "afxcmn.h"
#include "IEKG3DMaterial.h"

/////////////////////////////////////////////////////////////////////////////
// KSceneModelEditorMaterialAlphaOperatorDialog dialog

class KSceneModelEditorMaterialAlphaOperatorDialog : public CDialog
{
// Construction
public:
	KSceneModelEditorMaterialAlphaOperatorDialog(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(KSceneModelEditorMaterialAlphaOperatorDialog)
	enum { IDD = IDD_MODELEDITOR_MATERIAL_ALPHABLEND };
	CComboBox	m_Combo_SrcBlend;
	CComboBox	m_Combo_Operator;
	CComboBox	m_Combo_DestBlend;
	CComboBox	m_Combo_AlphaTestFunction;
	BOOL	m_bEnableAlphaBlend;
	BOOL	m_bEnableAlphaTest;
	DWORD	m_dwAlphaTestValue;
	DWORD	m_dwFactor_A;
	DWORD	m_dwFactor_B;
	DWORD	m_dwFactor_G;
	DWORD	m_dwFactor_R;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(KSceneModelEditorMaterialAlphaOperatorDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
public:
	void SetMaterial(IEKG3DMaterial* pMaterial, const std::vector<int>& SelList)
	{
		m_pMaterial = pMaterial;
		if (m_pMaterial)
		{
			m_pMaterial->AddRef();
			m_vecBackup.clear();
			
			m_SelList = SelList;
			for (size_t i = 0; i < m_SelList.size(); i++)
			{
                _MtlAlphaOperation* pAlphaOp = NULL;
                m_pMaterial->GetSubMaterialItemAlphaOp((DWORD)m_SelList[i], &pAlphaOp);

                if (!pAlphaOp)
				{
					KGLogPrintf(KGLOG_ERR, "获取Alpha选项失败.");
					m_vecBackup.clear();
					break;
				}
				m_vecBackup.push_back(*pAlphaOp);
			}
		}
		
	}

protected:
	IEKG3DMaterial* m_pMaterial;
	std::vector<int> m_SelList;           
	std::vector<_MtlAlphaOperation> m_vecBackup;
	void GetSetting();
	
	// Generated message map functions
	//{{AFX_MSG(KSceneModelEditorMaterialAlphaOperatorDialog)
	virtual BOOL OnInitDialog();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	afx_msg void OnBnClickedCheckAlphatest();
	afx_msg void OnCbnSelchangeComboSrcblend();
	afx_msg void OnCbnSelchangeComboDestblend();
	afx_msg void OnBnClickedCheckAlphablend();
	afx_msg void OnCbnSelchangeComboOperator();
	afx_msg void OnCbnSelchangeComboAlphatestfuc();
	afx_msg void OnEnChangeEditAlphatestvalue();
	afx_msg void OnEnChangeEditFactorA();
	afx_msg void OnDestroy();
	CButton m_ctlAlphaTest;
	CButton m_ctlAlphaBlend;
	afx_msg void OnBnClickedButtonRestore();
	CEdit m_ctlAlphaRef;
	CSpinButtonCtrl m_ctlAlphaCmpBase;
	afx_msg void OnDeltaposSpinAlphabase(NMHDR *pNMHDR, LRESULT *pResult);
	void UpdateValue();
	void ResetDialog();
	CEdit m_ctlFactorAlpha;
	CEdit m_ctlFactorRed;
	CEdit m_ctlFactorGreen;
	CEdit m_ctlFactorBlue;
	afx_msg void OnBnClickedOk();
	void OnCancel();
	void OnOK();

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_KMODELEDITORMATERIALALPHAOPERATORDIALOG_H__B02B2B73_DBD1_4432_88DF_ECF3E9651929__INCLUDED_)
