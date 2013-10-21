#include "stdafx.h"
#include "sceneeditor.h"
#include "KModelEditorMaterialAlphaOperatorDialog.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// KSceneModelEditorMaterialAlphaOperatorDialog dialog


KSceneModelEditorMaterialAlphaOperatorDialog::KSceneModelEditorMaterialAlphaOperatorDialog(CWnd* pParent /*=NULL*/)
	: CDialog(KSceneModelEditorMaterialAlphaOperatorDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(KSceneModelEditorMaterialAlphaOperatorDialog)
	m_bEnableAlphaBlend = FALSE;
	m_bEnableAlphaTest = FALSE;
	m_dwAlphaTestValue = 0;
	m_dwFactor_A = 0;
	m_dwFactor_B = 0;
	m_dwFactor_G = 0;
	m_dwFactor_R = 0;
	//}}AFX_DATA_INIT
	m_pMaterial = NULL;
}


void KSceneModelEditorMaterialAlphaOperatorDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(KSceneModelEditorMaterialAlphaOperatorDialog)
	DDX_Control(pDX, IDC_COMBO_SRCBLEND, m_Combo_SrcBlend);
	DDX_Control(pDX, IDC_COMBO_OPERATOR, m_Combo_Operator);
	DDX_Control(pDX, IDC_COMBO_DESTBLEND, m_Combo_DestBlend);
	DDX_Control(pDX, IDC_COMBO_ALPHATESTFUC, m_Combo_AlphaTestFunction);
	DDX_Check(pDX, IDC_CHECK_ALPHABLEND, m_bEnableAlphaBlend);
	DDX_Check(pDX, IDC_CHECK_ALPHATEST, m_bEnableAlphaTest);
	DDX_Text(pDX, IDC_EDIT_ALPHATESTVALUE, m_dwAlphaTestValue);
	DDX_Text(pDX, IDC_EDIT_FACTOR_A, m_dwFactor_A);
	DDX_Text(pDX, IDC_EDIT_FACTOR_B, m_dwFactor_B);
	DDX_Text(pDX, IDC_EDIT_FACTOR_G, m_dwFactor_G);
	DDX_Text(pDX, IDC_EDIT_FACTOR_R, m_dwFactor_R);
	//}}AFX_DATA_MAP
	DDX_Control(pDX, IDC_CHECK_ALPHATEST, m_ctlAlphaTest);
	DDX_Control(pDX, IDC_CHECK_ALPHABLEND, m_ctlAlphaBlend);
	DDX_Control(pDX, IDC_EDIT_ALPHATESTVALUE, m_ctlAlphaRef);
	DDX_Control(pDX, IDC_SPIN_ALPHABASE, m_ctlAlphaCmpBase);
	DDX_Control(pDX, IDC_EDIT_FACTOR_A, m_ctlFactorAlpha);
	DDX_Control(pDX, IDC_EDIT_FACTOR_R, m_ctlFactorRed);
	DDX_Control(pDX, IDC_EDIT_FACTOR_G, m_ctlFactorGreen);
	DDX_Control(pDX, IDC_EDIT_FACTOR_B, m_ctlFactorBlue);
}

BEGIN_MESSAGE_MAP(KSceneModelEditorMaterialAlphaOperatorDialog, CDialog)
	//{{AFX_MSG_MAP(KSceneModelEditorMaterialAlphaOperatorDialog)
	ON_WM_SHOWWINDOW()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_CHECK_ALPHATEST, OnBnClickedCheckAlphatest)
	ON_CBN_SELCHANGE(IDC_COMBO_SRCBLEND, OnCbnSelchangeComboSrcblend)
	ON_CBN_SELCHANGE(IDC_COMBO_DESTBLEND, OnCbnSelchangeComboDestblend)
	ON_BN_CLICKED(IDC_CHECK_ALPHABLEND, OnBnClickedCheckAlphablend)
	ON_CBN_SELCHANGE(IDC_COMBO_OPERATOR, OnCbnSelchangeComboOperator)
	ON_CBN_SELCHANGE(IDC_COMBO_ALPHATESTFUC, OnCbnSelchangeComboAlphatestfuc)
	ON_EN_CHANGE(IDC_EDIT_ALPHATESTVALUE, OnEnChangeEditAlphatestvalue)
	ON_EN_CHANGE(IDC_EDIT_FACTOR_A, OnEnChangeEditFactorA)
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_BUTTON_RESTORE, &KSceneModelEditorMaterialAlphaOperatorDialog::OnBnClickedButtonRestore)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_ALPHABASE, &KSceneModelEditorMaterialAlphaOperatorDialog::OnDeltaposSpinAlphabase)
	ON_BN_CLICKED(IDOK, &KSceneModelEditorMaterialAlphaOperatorDialog::OnBnClickedOk)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// KSceneModelEditorMaterialAlphaOperatorDialog message handlers

BOOL KSceneModelEditorMaterialAlphaOperatorDialog::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	m_ctlAlphaCmpBase.SetBuddy(&m_ctlAlphaRef);
	m_ctlAlphaCmpBase.SetRange(0, 255);

	ResetDialog();
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


void KSceneModelEditorMaterialAlphaOperatorDialog::OnShowWindow(BOOL bShow, UINT nStatus) 
{
	CDialog::OnShowWindow(bShow, nStatus);
	
	// TODO: Add your message handler code here
	ResetDialog();
	
}

void KSceneModelEditorMaterialAlphaOperatorDialog::OnBnClickedCheckAlphatest()
{
	KG_PROCESS_ERROR(m_pMaterial);

	for(size_t i = 0; i < m_SelList.size(); i++)
	{
		_MtlAlphaOperation* pAlphaOp = NULL;
        
        m_pMaterial->GetSubMaterialItemAlphaOp((DWORD)m_SelList[i], &pAlphaOp);
		if (!pAlphaOp)
			continue;
		
		UpdateData(TRUE);
		if (m_ctlAlphaTest.GetCheck() == BST_CHECKED)
		{
			pAlphaOp->EnableAlphaTest = TRUE;
			m_Combo_AlphaTestFunction.EnableWindow();
			m_Combo_AlphaTestFunction.SetCurSel(pAlphaOp->AlphaFunction - 1);
			m_dwAlphaTestValue = pAlphaOp->AlphaTestRef;
			m_ctlAlphaCmpBase.SetPos(static_cast<int>(pAlphaOp->AlphaTestRef));
			OnCbnSelchangeComboAlphatestfuc();
			UpdateData(FALSE);
		}
		else
		{
			pAlphaOp->EnableAlphaTest = FALSE;
			m_Combo_AlphaTestFunction.EnableWindow(FALSE);
		}
	}
Exit0:
	return;
	
}

void KSceneModelEditorMaterialAlphaOperatorDialog::OnCbnSelchangeComboSrcblend()
{
	int nCurSel = 0;
	KG_PROCESS_ERROR(m_pMaterial);

	nCurSel = m_Combo_SrcBlend.GetCurSel();
	KG_PROCESS_ERROR(nCurSel != CB_ERR);

	if (nCurSel == D3DBLEND_BLENDFACTOR - 1 ||
		nCurSel == D3DBLEND_INVBLENDFACTOR - 1)
	{
		m_ctlFactorAlpha.EnableWindow();
		m_ctlFactorRed.EnableWindow();
		m_ctlFactorGreen.EnableWindow();
		m_ctlFactorBlue.EnableWindow();
	}
	else
	{
		m_ctlFactorAlpha.EnableWindow(FALSE);
		m_ctlFactorRed.EnableWindow(FALSE);
		m_ctlFactorGreen.EnableWindow(FALSE);
		m_ctlFactorBlue.EnableWindow(FALSE);
	}

	for(size_t i = 0; i < m_SelList.size(); i++)
	{
        LPCTSTR pszName = NULL;
        m_pMaterial->GetName(&pszName);

        _MtlAlphaOperation* pAlphaOp = NULL;

        m_pMaterial->GetSubMaterialItemAlphaOp((DWORD)m_SelList[i], &pAlphaOp);

        if (!pAlphaOp)
		{
			KGLogPrintf(KGLOG_ERR, 
				"材质%s子材质%d获取Alpha选项失败.", 
				pszName, 
				m_SelList[i]
				);
			break;
		}
		pAlphaOp->SrcBlend = static_cast<DWORD>(nCurSel + 1);
	}

Exit0:
	return;
}

void KSceneModelEditorMaterialAlphaOperatorDialog::OnCbnSelchangeComboDestblend()
{
	int nCurSel = 0;
	KG_PROCESS_ERROR(m_pMaterial);

	nCurSel = m_Combo_DestBlend.GetCurSel();
	KG_PROCESS_ERROR(nCurSel != CB_ERR);

	if (nCurSel == D3DBLEND_BLENDFACTOR - 1 ||
		nCurSel == D3DBLEND_INVBLENDFACTOR - 1)
	{
		m_ctlFactorAlpha.EnableWindow();
		m_ctlFactorRed.EnableWindow();
		m_ctlFactorGreen.EnableWindow();
		m_ctlFactorBlue.EnableWindow();
	}
	else
	{
		m_ctlFactorAlpha.EnableWindow(FALSE);
		m_ctlFactorRed.EnableWindow(FALSE);
		m_ctlFactorGreen.EnableWindow(FALSE);
		m_ctlFactorBlue.EnableWindow(FALSE);
	}

	for(size_t i = 0; i < m_SelList.size(); i++)
	{
        LPCTSTR pszName = NULL;
        m_pMaterial->GetName(&pszName);

        _MtlAlphaOperation* pAlphaOp = NULL;

        m_pMaterial->GetSubMaterialItemAlphaOp((DWORD)m_SelList[i], &pAlphaOp);

        if (!pAlphaOp)
		{
			KGLogPrintf(KGLOG_ERR, 
				"材质%s子材质%d获取Alpha选项失败.", 
				pszName, 
				m_SelList[i]
				);
			break;
		}
		pAlphaOp->DestBlend = static_cast<DWORD>(nCurSel + 1);
	}

Exit0:
	return;
}

void KSceneModelEditorMaterialAlphaOperatorDialog::OnBnClickedCheckAlphablend()
{
	KG_PROCESS_ERROR(m_pMaterial);

	for(size_t i = 0; i < m_SelList.size(); i++)
	{
        _MtlAlphaOperation* pAlphaOp = NULL;

        m_pMaterial->GetSubMaterialItemAlphaOp((DWORD)m_SelList[i], &pAlphaOp);

        if (!pAlphaOp)
			continue;

		if (m_ctlAlphaBlend.GetCheck() == BST_CHECKED)
		{
			pAlphaOp->EnableAlphaBlend = TRUE;
			m_Combo_DestBlend.EnableWindow();
			m_Combo_SrcBlend.EnableWindow();
			m_Combo_Operator.EnableWindow();

			m_Combo_Operator.SetCurSel(pAlphaOp->BlendOperator - 1);
			m_Combo_DestBlend.SetCurSel(pAlphaOp->DestBlend - 1);
			m_Combo_SrcBlend.SetCurSel(pAlphaOp->SrcBlend - 1);
		}
		else
		{
			pAlphaOp->EnableAlphaBlend = FALSE;
			m_Combo_DestBlend.EnableWindow(FALSE);
			m_Combo_SrcBlend.EnableWindow(FALSE);
			m_Combo_Operator.EnableWindow(FALSE);
		}
	}
Exit0:
	return;
}

void KSceneModelEditorMaterialAlphaOperatorDialog::OnCbnSelchangeComboOperator()
{
	int nCurSel = 0;
	KG_PROCESS_ERROR(m_pMaterial);

	nCurSel = m_Combo_Operator.GetCurSel();
	KG_PROCESS_ERROR(nCurSel != CB_ERR);

	for(size_t i = 0; i < m_SelList.size(); i++)
	{
        _MtlAlphaOperation* pAlphaOp = NULL;

        m_pMaterial->GetSubMaterialItemAlphaOp((DWORD)m_SelList[i], &pAlphaOp);

        if (!pAlphaOp)
		{

            LPCTSTR pszName = NULL;
            m_pMaterial->GetName(&pszName);
			KGLogPrintf(KGLOG_ERR, 
				"材质%s子材质%d获取Alpha选项失败.", 
				pszName, 
				m_SelList[i]
				);
			break;
		}
		pAlphaOp->BlendOperator = static_cast<DWORD>(nCurSel + 1);
	}

Exit0:
	return;
}

void KSceneModelEditorMaterialAlphaOperatorDialog::OnCbnSelchangeComboAlphatestfuc()
{
	int nCurSel = 0;
	KG_PROCESS_ERROR(m_pMaterial);

	nCurSel = m_Combo_AlphaTestFunction.GetCurSel();
	KG_PROCESS_ERROR(nCurSel != CB_ERR);

	if (nCurSel == 0 || nCurSel == 7)
	{
		m_ctlAlphaRef.EnableWindow(FALSE);
		m_ctlAlphaCmpBase.EnableWindow(FALSE);
	}
	else
	{
		m_ctlAlphaRef.EnableWindow();
		m_ctlAlphaCmpBase.EnableWindow(TRUE);
	}

	for(size_t i = 0; i < m_SelList.size(); i++)
	{
        DWORD dwSelect = m_SelList[i];

		_MtlAlphaOperation* pAlphaOp = NULL;

        m_pMaterial->GetSubMaterialItemAlphaOp(dwSelect, &pAlphaOp);

		if (!pAlphaOp)
		{

            LPCTSTR pszName = NULL;
            m_pMaterial->GetName(&pszName);

			KGLogPrintf(KGLOG_ERR, 
				"材质%s子材质%d获取Alpha选项失败.", 
				pszName, 
				m_SelList[i]
			);
			break;
		}
		pAlphaOp->AlphaFunction = static_cast<DWORD>(nCurSel + 1);
	}
Exit0:
	return;
}

void KSceneModelEditorMaterialAlphaOperatorDialog::OnEnChangeEditAlphatestvalue()
{
	UpdateData(TRUE);
	m_dwAlphaTestValue = min(m_dwAlphaTestValue, 255);
	m_dwAlphaTestValue = max(0, m_dwAlphaTestValue);
	UpdateData(FALSE);
	m_ctlAlphaCmpBase.SetPos(m_dwAlphaTestValue);

	UpdateValue();
}

void KSceneModelEditorMaterialAlphaOperatorDialog::OnEnChangeEditFactorA()
{
	UpdateData(TRUE);
	m_dwFactor_A = max(m_dwFactor_A, 0);
	m_dwFactor_A = min(255, m_dwFactor_A);
	
	m_dwFactor_R = max(m_dwFactor_R, 0);
	m_dwFactor_R = min(255, m_dwFactor_R);

	m_dwFactor_G = max(m_dwFactor_G, 0);
	m_dwFactor_G = min(255, m_dwFactor_G);

	m_dwFactor_B = max(m_dwFactor_B, 0);
	m_dwFactor_B = min(255, m_dwFactor_B);

	UpdateData(FALSE);

	KG_PROCESS_ERROR(m_pMaterial);
	for (size_t i = 0; i < m_SelList.size(); i++)
	{
        _MtlAlphaOperation* pAlphaOp = NULL;

        m_pMaterial->GetSubMaterialItemAlphaOp((DWORD)m_SelList[i], &pAlphaOp);

        KG_PROCESS_ERROR(pAlphaOp);
		pAlphaOp->BlendFactor = (m_dwFactor_A<<24)|(m_dwFactor_R<<16)|(m_dwFactor_G<<8)|(m_dwFactor_B);
	}
Exit0:
	return;
}

void KSceneModelEditorMaterialAlphaOperatorDialog::OnDestroy()
{
	CDialog::OnDestroy();

	SAFE_RELEASE(m_pMaterial);
}

void KSceneModelEditorMaterialAlphaOperatorDialog::OnBnClickedButtonRestore()
{
	KG_PROCESS_ERROR(m_pMaterial);

	for (size_t i = 0; i < m_SelList.size(); i++)
	{
        _MtlAlphaOperation* pAlphaOp = NULL;

        m_pMaterial->GetSubMaterialItemAlphaOp((DWORD)m_SelList[i], &pAlphaOp);

        if(!pAlphaOp)
			return;
		
		*pAlphaOp = m_vecBackup[i];
	}
	ResetDialog();

Exit0:
	return;
}

void KSceneModelEditorMaterialAlphaOperatorDialog::OnDeltaposSpinAlphabase(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	m_dwAlphaTestValue = static_cast<DWORD>(LOWORD(m_ctlAlphaCmpBase.GetPos()));
	UpdateData(FALSE);

	UpdateValue();
	*pResult = 0;
}

void KSceneModelEditorMaterialAlphaOperatorDialog::ResetDialog()
{
	_MtlAlphaOperation* pAlphaOp = NULL;
	KG_PROCESS_ERROR(m_SelList.size());
	KG_PROCESS_ERROR(m_pMaterial);


    m_pMaterial->GetSubMaterialItemAlphaOp((DWORD)m_SelList[0], &pAlphaOp);
    KG_PROCESS_ERROR(pAlphaOp);

	if (pAlphaOp->EnableAlphaBlend)
	{
		m_ctlAlphaBlend.SetCheck(BST_CHECKED);
		m_Combo_Operator.EnableWindow();
		m_Combo_DestBlend.EnableWindow();
		m_Combo_SrcBlend.EnableWindow();

		m_Combo_Operator.SetCurSel(pAlphaOp->BlendOperator - 1);
		m_Combo_DestBlend.SetCurSel(pAlphaOp->DestBlend - 1);
		m_Combo_SrcBlend.SetCurSel(pAlphaOp->SrcBlend - 1);
	}
	else
	{
		m_Combo_Operator.EnableWindow(FALSE);
		m_Combo_DestBlend.EnableWindow(FALSE);
		m_Combo_SrcBlend.EnableWindow(FALSE);

		m_ctlAlphaBlend.SetCheck(BST_UNCHECKED);

	}
	if (pAlphaOp->EnableAlphaTest)
	{
		m_ctlAlphaTest.SetCheck(BST_CHECKED);
		m_Combo_AlphaTestFunction.EnableWindow();
		m_Combo_AlphaTestFunction.SetCurSel(pAlphaOp->AlphaFunction - 1);
		OnCbnSelchangeComboAlphatestfuc();
	}
	else
	{
		m_ctlAlphaTest.SetCheck(BST_UNCHECKED);
		m_Combo_AlphaTestFunction.EnableWindow(FALSE);
	}

	UpdateData(TRUE);

	//Read data member here
	m_dwAlphaTestValue = pAlphaOp->AlphaTestRef;
	m_dwFactor_A = pAlphaOp->BlendFactor >> 24;
	m_dwFactor_R = (pAlphaOp->BlendFactor & 0x00ff0000) >> 16;
	m_dwFactor_G = (pAlphaOp->BlendFactor & 0x0000ff00) >> 8;
	m_dwFactor_B = (pAlphaOp->BlendFactor & 0x000000ff);
	UpdateData(FALSE);
Exit0:
	return;
}

void KSceneModelEditorMaterialAlphaOperatorDialog::UpdateValue()
{
	KG_PROCESS_ERROR(m_pMaterial);

	for (size_t i = 0; i < m_SelList.size(); i++)
	{
        _MtlAlphaOperation* pAlphaOp = NULL;

        m_pMaterial->GetSubMaterialItemAlphaOp((DWORD)m_SelList[i], &pAlphaOp);

        KG_PROCESS_ERROR(pAlphaOp);

		pAlphaOp->AlphaTestRef = m_dwAlphaTestValue;
		pAlphaOp->BackAlphaTestRef = pAlphaOp->AlphaTestRef;
	}
Exit0:
	return;
}

void KSceneModelEditorMaterialAlphaOperatorDialog::OnOK()
{

}


void KSceneModelEditorMaterialAlphaOperatorDialog::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	CDialog::OnOK();
}


void KSceneModelEditorMaterialAlphaOperatorDialog::OnCancel()
{
	SAFE_RELEASE(m_pMaterial);
	CDialog::OnCancel();
}