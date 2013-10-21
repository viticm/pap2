// KDlg_ProItem_MtlOption.cpp : implementation file
//

#include "stdafx.h"
#include "sceneeditor.h"
#include "KDlg_ProItem_MtlOption.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// KDlg_ProItem_MtlOption dialog

namespace PropertyBase
{
KDlg_ProItem_MtlOption::KDlg_ProItem_MtlOption(CWnd* pParent /*=NULL*/)
	: CDialog(KDlg_ProItem_MtlOption::IDD, pParent)
{
	//{{AFX_DATA_INIT(KDlg_ProItem_MtlOption)
	m_2Face = FALSE;
	m_Wire = FALSE;
	m_Z = FALSE;
	//}}AFX_DATA_INIT
	//m_lpModel = NULL;
	//m_lpProperty = NULL;
	//m_lpCurMaterial = NULL;
}


void KDlg_ProItem_MtlOption::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(KDlg_ProItem_MtlOption)
	DDX_Check(pDX, IDC_2FACE, m_2Face);
	DDX_Check(pDX, IDC_CWIRE, m_Wire);
	DDX_Check(pDX, IDC_ZBUFFER, m_Z);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(KDlg_ProItem_MtlOption, CDialog)
	//{{AFX_MSG_MAP(KDlg_ProItem_MtlOption)
	ON_WM_MOUSEMOVE()
	ON_BN_CLICKED(IDC_ZBUFFER, OnZbuffer)
	ON_BN_CLICKED(IDC_CWIRE, OnCwire)
	ON_BN_CLICKED(IDC_2FACE, On2face)
	ON_BN_CLICKED(IDC_BAPPLY, OnBapply)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// KDlg_ProItem_MtlOption message handlers

BOOL KDlg_ProItem_MtlOption::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	//if(m_lpProperty)
	//{
	//	m_lpModel = (LPMODEL)(m_lpProperty->Address);
	//}

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void KDlg_ProItem_MtlOption::Update(BOOL Dir)
{
	/*if(!m_lpModel)
		return;
	KModel::LPMATERIAL pMat = m_lpModel->GetFirstSelectedMtl();
	if(!pMat)
		return;
	if(!Dir)
	{
		m_Z = pMat->m_dOption & MATERIAL_OPTION_ZBUFFER_TRUE;
		m_Wire = pMat->m_dOption & MATERIAL_OPTION_FILL_WIREFRAME;
		m_2Face = pMat->m_dOption & MATERIAL_OPTION_CULL_NONE;

		UpdateData(FALSE);
	}
	else
	{
		UpdateData(TRUE);
		if(m_Z)
		{
			pMat->m_dOption |= MATERIAL_OPTION_ZBUFFER_TRUE;
			pMat->m_dOption &= ~MATERIAL_OPTION_ZBUFFER_FALSE;
		}
		else
		{
			pMat->m_dOption &= ~MATERIAL_OPTION_ZBUFFER_TRUE;
			pMat->m_dOption |= MATERIAL_OPTION_ZBUFFER_FALSE;
		}

		if(m_Wire)
		{
			pMat->m_dOption |= MATERIAL_OPTION_FILL_WIREFRAME;
			pMat->m_dOption &= ~MATERIAL_OPTION_FILL_SOLID;
		}
		else
		{
			pMat->m_dOption |= MATERIAL_OPTION_FILL_SOLID;
			pMat->m_dOption &= ~MATERIAL_OPTION_FILL_WIREFRAME;
		}

		if(m_2Face)
		{
			pMat->m_dOption |= MATERIAL_OPTION_CULL_NONE;
			pMat->m_dOption &= ~MATERIAL_OPTION_CULL_CW;
		}
		else
		{
			pMat->m_dOption &= ~MATERIAL_OPTION_CULL_NONE;
			pMat->m_dOption |= MATERIAL_OPTION_CULL_CW;
		}
		UpdateData(FALSE);
	}*/

}

void KDlg_ProItem_MtlOption::OnMouseMove(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	/*if(!m_lpModel)
		return;
	if(m_lpCurMaterial!=m_lpModel->GetFirstSelectedMtl())
	{
		m_lpCurMaterial = m_lpModel->GetFirstSelectedMtl();
		Update(FALSE);
	}*/
	CDialog::OnMouseMove(nFlags, point);
}



void KDlg_ProItem_MtlOption::OnZbuffer() 
{
	// TODO: Add your control notification handler code here
	Update(TRUE);
}

void KDlg_ProItem_MtlOption::OnCwire() 
{
	// TODO: Add your control notification handler code here
	Update(TRUE);
}

void KDlg_ProItem_MtlOption::On2face() 
{
	// TODO: Add your control notification handler code here
	Update(TRUE);
}

void KDlg_ProItem_MtlOption::OnBapply() 
{
	// TODO: Add your control notification handler code here
	/*if(!m_lpModel)
		return;
	m_lpCurMaterial = m_lpModel->GetFirstSelectedMtl();
	if(!m_lpCurMaterial)
		return;

	for(DWORD i=0;i<m_lpModel->m_dNumMaterial;i++)
	{
		KModel::LPMATERIAL pMtl = &m_lpModel->m_lpMaterial[i];
		if(pMtl->m_dOption&MATERIAL_OPTION_SELECTED)
		{
			pMtl->m_dOption =
				m_lpCurMaterial->m_dOption;
		}
	}*/
}

}
