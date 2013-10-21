// KMaterialUniformToolAlphaOperation.cpp : implementation file
//

#include "stdafx.h"
#include "SceneEditor.h"
#include "KMaterialUniformToolAlphaOperation.h"
#include "IEKG3DMaterial.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

////////////////////////////////////////////////////////////////////////////////

// KMaterialUniformToolAlphaOperation dialog

IMPLEMENT_DYNAMIC(KMaterialUniformToolAlphaOperation, CDialog)

KMaterialUniformToolAlphaOperation::KMaterialUniformToolAlphaOperation(CWnd* pParent /*=NULL*/)
	: CDialog(KMaterialUniformToolAlphaOperation::IDD, pParent)
{

}

KMaterialUniformToolAlphaOperation::~KMaterialUniformToolAlphaOperation()
{
}

void KMaterialUniformToolAlphaOperation::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

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
	DDX_Control(pDX, IDC_CHECK_ALPHATEST, m_ctlAlphaTest);
	DDX_Control(pDX, IDC_CHECK_ALPHABLEND, m_ctlAlphaBlend);
	DDX_Control(pDX, IDC_EDIT_ALPHATESTVALUE, m_ctlAlphaRef);
	DDX_Control(pDX, IDC_SPIN_ALPHABASE, m_ctlAlphaCmpBase);
	DDX_Control(pDX, IDC_EDIT_FACTOR_A, m_ctlFactorAlpha);
	DDX_Control(pDX, IDC_EDIT_FACTOR_R, m_ctlFactorRed);
	DDX_Control(pDX, IDC_EDIT_FACTOR_G, m_ctlFactorGreen);
	DDX_Control(pDX, IDC_EDIT_FACTOR_B, m_ctlFactorBlue);
}


BEGIN_MESSAGE_MAP(KMaterialUniformToolAlphaOperation, CDialog)
	ON_BN_CLICKED(IDC_CHECK_ALPHATEST, &KMaterialUniformToolAlphaOperation::OnBnClickedCheckAlphatest)
	ON_CBN_SELCHANGE(IDC_COMBO_SRCBLEND, OnCbnSelchangeComboSrcblend)
	ON_CBN_SELCHANGE(IDC_COMBO_DESTBLEND, OnCbnSelchangeComboDestblend)
	ON_BN_CLICKED(IDC_CHECK_ALPHABLEND, OnBnClickedCheckAlphablend)
	ON_CBN_SELCHANGE(IDC_COMBO_OPERATOR, OnCbnSelchangeComboOperator)
	ON_CBN_SELCHANGE(IDC_COMBO_ALPHATESTFUC, OnCbnSelchangeComboAlphatestfuc)
	ON_EN_CHANGE(IDC_EDIT_ALPHATESTVALUE, OnEnChangeEditAlphatestvalue)
	ON_EN_CHANGE(IDC_EDIT_FACTOR_A, OnEnChangeEditFactorA)
	ON_WM_DESTROY()
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_ALPHABASE, &KMaterialUniformToolAlphaOperation::OnDeltaposSpinAlphabase)
	ON_BN_CLICKED(IDOK, &KMaterialUniformToolAlphaOperation::OnBnClickedOk)
END_MESSAGE_MAP()

// KMaterialUniformToolAlphaOperation message handlers
void KMaterialUniformToolAlphaOperation::SetAlphaOperation(_MtlAlphaOperation* pAlphaOperation)
{
	m_pAlphaOperation = pAlphaOperation;
}

void KMaterialUniformToolAlphaOperation::OnBnClickedCheckAlphatest()
{
	KG_PROCESS_ERROR(m_pAlphaOperation);

	UpdateData(TRUE);
	if (m_ctlAlphaTest.GetCheck() == BST_CHECKED)
	{
		m_pAlphaOperation->EnableAlphaTest = TRUE;
		m_Combo_AlphaTestFunction.EnableWindow();
		m_Combo_AlphaTestFunction.SetCurSel(m_pAlphaOperation->AlphaFunction - 1);
		m_dwAlphaTestValue = m_pAlphaOperation->AlphaTestRef;
		m_ctlAlphaCmpBase.SetPos(static_cast<int>(m_pAlphaOperation->AlphaTestRef));
		OnCbnSelchangeComboAlphatestfuc();
		UpdateData(FALSE);
	}
	else
	{
		m_pAlphaOperation->EnableAlphaTest = FALSE;
		m_Combo_AlphaTestFunction.EnableWindow(FALSE);
	}
Exit0:
	return;
}

void KMaterialUniformToolAlphaOperation::OnCbnSelchangeComboAlphatestfuc()
{
	int nCurSel = 0;
	KG_PROCESS_ERROR(m_pAlphaOperation);

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

	m_pAlphaOperation->AlphaFunction = static_cast<DWORD>(nCurSel + 1);
Exit0:
	return;
}


void KMaterialUniformToolAlphaOperation::OnCbnSelchangeComboSrcblend()
{
	int nCurSel = 0;
	KG_PROCESS_ERROR(m_pAlphaOperation);

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

	m_pAlphaOperation->SrcBlend = static_cast<DWORD>(nCurSel + 1);


Exit0:
	return;
}

void KMaterialUniformToolAlphaOperation::OnCbnSelchangeComboDestblend()
{
	int nCurSel = 0;
	KG_PROCESS_ERROR(m_pAlphaOperation);

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

	m_pAlphaOperation->DestBlend = static_cast<DWORD>(nCurSel + 1);

Exit0:
	return;
}

void KMaterialUniformToolAlphaOperation::OnBnClickedCheckAlphablend()
{
	KG_PROCESS_ERROR(m_pAlphaOperation);

	if (m_ctlAlphaBlend.GetCheck() == BST_CHECKED)
	{
		m_pAlphaOperation->EnableAlphaBlend = TRUE;
		m_Combo_DestBlend.EnableWindow();
		m_Combo_SrcBlend.EnableWindow();
		m_Combo_Operator.EnableWindow();

		m_Combo_Operator.SetCurSel(m_pAlphaOperation->BlendOperator - 1);
		m_Combo_DestBlend.SetCurSel(m_pAlphaOperation->DestBlend - 1);
		m_Combo_SrcBlend.SetCurSel(m_pAlphaOperation->SrcBlend - 1);
	}
	else
	{
		m_pAlphaOperation->EnableAlphaBlend = FALSE;
		m_Combo_DestBlend.EnableWindow(FALSE);
		m_Combo_SrcBlend.EnableWindow(FALSE);
		m_Combo_Operator.EnableWindow(FALSE);
	}

Exit0:
	return;
}

void KMaterialUniformToolAlphaOperation::OnCbnSelchangeComboOperator()
{
	int nCurSel = 0;
	KG_PROCESS_ERROR(m_pAlphaOperation);

	nCurSel = m_Combo_Operator.GetCurSel();
	KG_PROCESS_ERROR(nCurSel != CB_ERR);

	m_pAlphaOperation->BlendOperator = static_cast<DWORD>(nCurSel + 1);

Exit0:
	return;
}

void KMaterialUniformToolAlphaOperation::OnEnChangeEditAlphatestvalue()
{
	UpdateData(TRUE);
	m_dwAlphaTestValue = min(m_dwAlphaTestValue, 255);
	m_dwAlphaTestValue = max(0, m_dwAlphaTestValue);
	UpdateData(FALSE);
	m_ctlAlphaCmpBase.SetPos(m_dwAlphaTestValue);

	UpdateValue();
}

void KMaterialUniformToolAlphaOperation::OnEnChangeEditFactorA()
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

	KG_PROCESS_ERROR(m_pAlphaOperation);

	m_pAlphaOperation->BlendFactor = (m_dwFactor_A<<24)|(m_dwFactor_R<<16)|(m_dwFactor_G<<8)|(m_dwFactor_B);
Exit0:
	return;
}


void KMaterialUniformToolAlphaOperation::OnDeltaposSpinAlphabase(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	m_dwAlphaTestValue = static_cast<DWORD>(LOWORD(m_ctlAlphaCmpBase.GetPos()));
	UpdateData(FALSE);

	UpdateValue();
	*pResult = 0;
}

void KMaterialUniformToolAlphaOperation::ResetDialog()
{
	_MtlAlphaOperation* pAlphaOp = NULL;
	KG_PROCESS_ERROR(m_pAlphaOperation);



	if (m_pAlphaOperation->EnableAlphaBlend)
	{
		m_ctlAlphaBlend.SetCheck(BST_CHECKED);
		m_Combo_Operator.EnableWindow();
		m_Combo_DestBlend.EnableWindow();
		m_Combo_SrcBlend.EnableWindow();

		m_Combo_Operator.SetCurSel(m_pAlphaOperation->BlendOperator - 1);
		m_Combo_DestBlend.SetCurSel(m_pAlphaOperation->DestBlend - 1);
		m_Combo_SrcBlend.SetCurSel(m_pAlphaOperation->SrcBlend - 1);
	}
	else
	{
		m_Combo_Operator.EnableWindow(FALSE);
		m_Combo_DestBlend.EnableWindow(FALSE);
		m_Combo_SrcBlend.EnableWindow(FALSE);

		m_ctlAlphaBlend.SetCheck(BST_UNCHECKED);

	}
	if (m_pAlphaOperation->EnableAlphaTest)
	{
		m_ctlAlphaTest.SetCheck(BST_CHECKED);
		m_Combo_AlphaTestFunction.EnableWindow();
		m_Combo_AlphaTestFunction.SetCurSel(m_pAlphaOperation->AlphaFunction - 1);
		OnCbnSelchangeComboAlphatestfuc();
	}
	else
	{
		m_ctlAlphaTest.SetCheck(BST_UNCHECKED);
		m_Combo_AlphaTestFunction.EnableWindow(FALSE);
	}

	UpdateData(TRUE);

	//Read data member here
	m_dwAlphaTestValue = m_pAlphaOperation->AlphaTestRef;
	m_dwFactor_A = m_pAlphaOperation->BlendFactor >> 24;
	m_dwFactor_R = (m_pAlphaOperation->BlendFactor & 0x00ff0000) >> 16;
	m_dwFactor_G = (m_pAlphaOperation->BlendFactor & 0x0000ff00) >> 8;
	m_dwFactor_B = (m_pAlphaOperation->BlendFactor & 0x000000ff);
	UpdateData(FALSE);
Exit0:
	return;
}

void KMaterialUniformToolAlphaOperation::UpdateValue()
{
	KG_PROCESS_ERROR(m_pAlphaOperation);

	m_pAlphaOperation->AlphaTestRef = m_dwAlphaTestValue;
Exit0:
	return;
}

void KMaterialUniformToolAlphaOperation::OnOK()
{

}


void KMaterialUniformToolAlphaOperation::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	CDialog::OnOK();
}

BOOL KMaterialUniformToolAlphaOperation::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  Add extra initialization here
	m_ctlAlphaCmpBase.SetBuddy(&m_ctlAlphaRef);
	m_ctlAlphaCmpBase.SetRange(0, 255);

	ResetDialog();


	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}
