#pragma once

// KMaterialUniformToolAlphaOperation dialog
struct _MtlAlphaOperation;
class KMaterialUniformToolAlphaOperation : public CDialog
{
	DECLARE_DYNAMIC(KMaterialUniformToolAlphaOperation)

public:
	KMaterialUniformToolAlphaOperation(CWnd* pParent = NULL);   // standard constructor
	virtual ~KMaterialUniformToolAlphaOperation();
	void SetAlphaOperation(_MtlAlphaOperation* pAlphaOperation);

// Dialog Data
	enum { IDD = IDD_MATERIALUNIFORMTOOL_ALPHABLEND };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

	_MtlAlphaOperation* m_pAlphaOperation;
	CButton m_ctlAlphaTest;
	CButton m_ctlAlphaBlend;
	CEdit m_ctlAlphaRef;
	CSpinButtonCtrl m_ctlAlphaCmpBase;
	CEdit m_ctlFactorAlpha;
	CEdit m_ctlFactorRed;
	CEdit m_ctlFactorGreen;
	CEdit m_ctlFactorBlue;
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
public:
	afx_msg void OnBnClickedCheckAlphatest();
	afx_msg void OnCbnSelchangeComboAlphatestfuc();
	afx_msg void OnCbnSelchangeComboSrcblend();
	afx_msg void OnCbnSelchangeComboDestblend();
	afx_msg void OnBnClickedCheckAlphablend();
	afx_msg void OnCbnSelchangeComboOperator();
	void OnEnChangeEditAlphatestvalue();
	void OnEnChangeEditFactorA();
	void OnDeltaposSpinAlphabase(NMHDR *pNMHDR, LRESULT *pResult);
	void ResetDialog();
	void UpdateValue();
	void OnOK();
	void OnBnClickedOk();
public:
	virtual BOOL OnInitDialog();
};
