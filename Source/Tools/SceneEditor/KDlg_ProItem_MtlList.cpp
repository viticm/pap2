// KDlg_ProItem_MtlList.cpp : implementation file
//

#include "stdafx.h"
#include "sceneeditor.h"
#include "KDlg_ProItem_MtlList.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// KDlg_ProItem_MtlList dialog
namespace PropertyBase
{

KDlg_ProItem_MtlList::KDlg_ProItem_MtlList(CWnd* pParent /*=NULL*/)
	: CDialog(KDlg_ProItem_MtlList::IDD, pParent)
{
	//{{AFX_DATA_INIT(KDlg_ProItem_MtlList)
	//}}AFX_DATA_INIT
	/*m_lpModel = NULL;
	m_lpProperty = NULL;
	m_lpPropertyPage = NULL;*/
}


void KDlg_ProItem_MtlList::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(KDlg_ProItem_MtlList)
	DDX_Control(pDX, IDC_LIST1, m_List);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(KDlg_ProItem_MtlList, CDialog)
	//{{AFX_MSG_MAP(KDlg_ProItem_MtlList)
	ON_BN_CLICKED(IDC_BSELECTALL, OnBselectall)
	ON_BN_CLICKED(IDC_BSELECTINVERT, OnBselectinvert)
	ON_BN_CLICKED(IDC_BSELECTNONE, OnBselectnone)
	ON_BN_CLICKED(IDC_BUNIT, OnBunit)
	ON_BN_CLICKED(IDC_BMOVEBUTTON, OnBmovebutton)
	ON_BN_CLICKED(IDC_BMOVETOP, OnBmovetop)
	ON_NOTIFY(NM_CLICK, IDC_LIST1, OnClickList1)
	ON_BN_CLICKED(IDC_BPRT, OnBPRT)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// KDlg_ProItem_MtlList message handlers

void KDlg_ProItem_MtlList::OnBselectall() 
{
	// TODO: Add your control notification handler code here
	//if(!m_lpModel)
	//	return ;

	//for(DWORD i=0;i<m_lpModel->m_dNumMaterial;i++)
	//{
	//	KModel::LPMATERIAL pMtl = &m_lpModel->m_lpMaterial[i];
	//	
	//	pMtl->m_dOption |= MATERIAL_OPTION_SELECTED;
	//	//m_List.SetCheck(i,TRUE);
	//	m_List.SetItemState(i,LVIS_SELECTED,0xFFFFFFFF);
	//}

	//m_lpModel->PRT_LaunchSimulation();

}

void KDlg_ProItem_MtlList::OnBselectinvert() 
{
	// TODO: Add your control notification handler code here
	/*if(!m_lpModel)
		return ;

	for(DWORD i=0;i<m_lpModel->m_dNumMaterial;i++)
	{
		KModel::LPMATERIAL pMtl = &m_lpModel->m_lpMaterial[i];

		if(pMtl->m_dOption&MATERIAL_OPTION_SELECTED)
		{
			pMtl->m_dOption &= (~MATERIAL_OPTION_SELECTED);
			m_List.SetItemState(i,0,0xFFFFFFFF);
		}
		else
		{
			pMtl->m_dOption |= MATERIAL_OPTION_SELECTED;
			m_List.SetItemState(i,LVIS_SELECTED,0xFFFFFFFF);
		}
	}*/
}

void KDlg_ProItem_MtlList::OnBselectnone() 
{
	// TODO: Add your control notification handler code here
	/*if(!m_lpModel)
		return ;

	for(DWORD i=0;i<m_lpModel->m_dNumMaterial;i++)
	{
		KModel::LPMATERIAL pMtl = &m_lpModel->m_lpMaterial[i];

		pMtl->m_dOption &= (~MATERIAL_OPTION_SELECTED);
		m_List.SetItemState(i,0,0xFFFFFFFF);
	}*/
}

void KDlg_ProItem_MtlList::OnBunit() 
{
	// TODO: Add your control notification handler code here
	
}

void KDlg_ProItem_MtlList::OnBmovebutton() 
{
	// TODO: Add your control notification handler code here
	
}

void KDlg_ProItem_MtlList::OnBmovetop() 
{
	// TODO: Add your control notification handler code here
	
}

BOOL KDlg_ProItem_MtlList::OnInitDialog() 
{
	CDialog::OnInitDialog();

	m_imgList.Create( IDB_BITMAP1, 16, 1, RGB( 0, 128, 128 ) );
	m_List.SetImageList(&m_imgList,TVSIL_NORMAL);
	// TODO: Add extra initialization here
	FillList();
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

HRESULT KDlg_ProItem_MtlList::FillList()
{
	m_List.DeleteAllItems();
	///*if(m_lpProperty)
	//{
	//	m_lpModel = (LPMODEL)(m_lpProperty->Address);
	//	for(DWORD i=0;i<m_lpModel->m_dNumMaterial;i++)
	//	{
	//		KModel::LPMATERIAL pMtl = &m_lpModel->m_lpMaterial[i];

	//		TCHAR str[256];
	//		wsprintf(str,"Mtl %d",i);
	//		int index = m_List.InsertItem(i,str,0);
	//		m_List.SetItemData(index,i);
	//		
	//		if(pMtl->m_dOption & MATERIAL_OPTION_SELECTED)
	//			m_List.SetItemState(i,LVIS_SELECTED,0xFFFFFFFF);
	//		else
	//			m_List.SetItemState(i,0,0xFFFFFFFF);
	//	}
	//}*/
	return S_OK;
}




void KDlg_ProItem_MtlList::OnClickList1(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here
	/*if(!m_lpModel)
		return ;

	for(DWORD i=0;i<m_lpModel->m_dNumMaterial;i++)
	{
		KModel::LPMATERIAL pMtl = &m_lpModel->m_lpMaterial[i];
		pMtl->m_dOption &= (~MATERIAL_OPTION_SELECTED);
	}

	POSITION pos = m_List.GetFirstSelectedItemPosition();
	while (pos)
	{
		int index = m_List.GetNextSelectedItem(pos);
		int Mtl = m_List.GetItemData(index);
		if(m_lpProperty)
		{
			m_lpModel = (LPMODEL)(m_lpProperty->Address);
			KModel::LPMATERIAL pMtl = &m_lpModel->m_lpMaterial[Mtl];
			
			pMtl->m_dOption |= MATERIAL_OPTION_SELECTED;
			
		}
	}

	ReCreateEffect();*/

	*pResult = 0;
}

HRESULT KDlg_ProItem_MtlList::ReCreateEffect()
{
	//KDlg_PropertyPage::_Group* pGroup = &m_lpPropertyPage->m_vecGroup[4];
	//BOOL Ex = pGroup->m_bExtracted;
	//pGroup->Clear();

	//PropertyBase::_PropertyGroup* pProGroup = &m_lpPropertyPage->m_lpProperPage->vecGroup[4];
	//pProGroup->Clear();

	//KModel::LPMATERIAL pMtl = m_lpModel->GetFirstSelectedMtl();
	//if(pMtl)
	//{
	//	pProGroup->AddPropertyMtlEffect("Effect",m_lpModel);

	//	if(pMtl->m_lpEffect)
	//	{
	//		for(DWORD i=0;i<pMtl->m_lpEffect->InputConstant.size();i++)
	//		{
	//			KModel::SHADERCONSTANT* pCon = &pMtl->m_lpEffect->InputConstant[i];
	//			switch(pCon->dwType)
	//			{
	//			/*case MSEFFECT_INT:
	//				pProGroup->AddPropertyInt((LPSTR)pCon->scName.c_str(),&m_nTest,0,10);
	//				break;
	//			case MSEFFECT_POSITION:
	//				pProGroup->AddPropertyInt((LPSTR)pCon->scName.c_str(),&m_nTest,0,10);
	//				break;
	//			case MSEFFECT_FLOAT4:
	//				pProGroup->AddPropertyInt((LPSTR)pCon->scName.c_str(),&m_nTest,0,10);
	//				break;*/
	//			case MSEFFECT_COLORRGBA:
	//				{
	//					D3DCOLORVALUE* pValue = (D3DCOLORVALUE*)pCon->pData;
	//					pProGroup->AddPropertyColor((LPSTR)pCon->scName.c_str(),
	//						pValue);
	//					break;
	//				}
	//			case MSEFFECT_INT:
	//				{
	//					int* pValue = (int*) pCon->pData;
	//					pProGroup->AddPropertyInt((LPSTR)pCon->scName.c_str(),pValue,0,10);
	//					break;
	//				}
	//			case MSEFFECT_FLOAT:
	//				{
	//					float* pValue = (float*) pCon->pData;
	//					pProGroup->AddPropertyFloat((LPSTR)pCon->scName.c_str(),pValue,0,10);
	//					break;
	//				}
	//			}
	//		}
	//	}
	//}

	//pGroup->Create(4,pProGroup,m_lpPropertyPage);
	//pGroup->CreateItems(pProGroup,m_lpPropertyPage);
	//pGroup->SetExtracted(Ex);

	//CRect m_Rect;
	//m_lpPropertyPage->GetWindowRect(&m_Rect);
	//m_lpPropertyPage->OnMove(m_Rect.left,m_Rect.right);

	return S_OK;
}


void KDlg_ProItem_MtlList::OnBPRT() 
{
	// TODO: Add your control notification handler code here
	//m_lpModel->PRT_LaunchSimulation();
}

}
