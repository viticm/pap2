// KDlg_ProItem_MtlEffect.cpp : implementation file
//

#include "stdafx.h"
#include "sceneeditor.h"
#include "KDlg_ProItem_MtlEffect.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// KDlg_ProItem_MtlEffect dialog
namespace PropertyBase
{

KDlg_ProItem_MtlEffect::KDlg_ProItem_MtlEffect(CWnd* pParent /*=NULL*/)
	: CDialog(KDlg_ProItem_MtlEffect::IDD, pParent)
{
	//{{AFX_DATA_INIT(KDlg_ProItem_MtlEffect)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	/*m_lpModel = NULL;
	m_lpProperty = NULL;*/
}


void KDlg_ProItem_MtlEffect::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(KDlg_ProItem_MtlEffect)
	DDX_Control(pDX, IDC_COMBO1, m_Tec);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(KDlg_ProItem_MtlEffect, CDialog)
	//{{AFX_MSG_MAP(KDlg_ProItem_MtlEffect)
	ON_BN_CLICKED(IDC_BLOAD, OnBload)
	ON_CBN_SELCHANGE(IDC_COMBO1, OnSelchangeCombo1)
	ON_BN_CLICKED(IDC_BAPPLY, OnBapply)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()



/////////////////////////////////////////////////////////////////////////////
// KDlg_ProItem_MtlEffect message handlers

void KDlg_ProItem_MtlEffect::OnBload() 
{
}


BOOL KDlg_ProItem_MtlEffect::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	/*for(int i=100;i>=0;i--)
		m_Tec.DeleteString(i);

	if(m_lpModel)
	{
		KModel::LPMATERIAL pMat = m_lpModel->GetFirstSelectedMtl();
		
		if(pMat)
		{
			if(pMat->m_lpEffect)
			{
				D3DXEFFECT_DESC Desc;
				pMat->m_lpEffect->m_HEffect->pEffect->GetDesc(&Desc);

				for(UINT K = 0;K<Desc.Techniques;K++)
				{
					D3DXHANDLE Handle = pMat->m_lpEffect->m_HEffect->pEffect->GetTechnique(K);
					if(Handle)
					{
						D3DXTECHNIQUE_DESC Tec_Desc;
						pMat->m_lpEffect->m_HEffect->pEffect->GetTechniqueDesc(Handle,&Tec_Desc);

						TCHAR str[256];
						wsprintf(str,"%s",Tec_Desc.Name);

						m_Tec.AddString(str);
					}
				}

				if (pMat->m_lpEffect->m_nCurTechnique==-1)
					m_Tec.SetCurSel(0);
				else
					m_Tec.SetCurSel(pMat->m_lpEffect->m_nCurTechnique);

			}
		}
	}*/
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void KDlg_ProItem_MtlEffect::OnSelchangeCombo1() 
{
	// TODO: Add your control notification handler code here
	/*if(m_lpModel)
	{
		KModel::LPMATERIAL pMat = m_lpModel->GetFirstSelectedMtl();

		if(pMat)
		{
			if(pMat->m_lpEffect)
			{
				pMat->m_lpEffect->m_nCurTechnique = m_Tec.GetCurSel();
			}
		}
	}*/
}

void KDlg_ProItem_MtlEffect::OnBapply() 
{
	// TODO: Add your control notification handler code here
	//if(!m_lpModel)
	//	return ;

	//KModel::LPMATERIAL pMat = m_lpModel->GetFirstSelectedMtl();

	//if(!pMat)
	//	return ;
	//
	//for(DWORD i=0;i<m_lpModel->m_dNumMaterial;i++)
	//{
	//	KModel::LPMATERIAL pDestMtl = &m_lpModel->m_lpMaterial[i];
	//	if((pDestMtl!=pMat)&&(pDestMtl->m_dOption&MATERIAL_OPTION_SELECTED))
	//	{
	//		if (!pDestMtl->m_lpEffect)
	//		{
	//			pDestMtl->m_lpEffect = new KModel::MTLMSEFFECT;
	//		}

	//		g_EffectMgr.GetResource(&pDestMtl->m_lpEffect->m_HEffect,
	//			pMat->m_lpEffect->m_HEffect->m_szFileName.c_str());
	//		
	//		pDestMtl->m_lpEffect->OnLoad();

	//		pDestMtl->m_lpEffect->m_nCurTechnique = pMat->m_lpEffect->m_nCurTechnique;
	//		//pDestMtl->m_lpEffect->InputConstant = pMat->m_lpEffect->InputConstant;
	//	}
	//}

	//if(pMat->m_lpEffect)
	//{
	//	pMat->m_lpEffect->m_nCurTechnique = m_Tec.GetCurSel();
	//}

}

}
