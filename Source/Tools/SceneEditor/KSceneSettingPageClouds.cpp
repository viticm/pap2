// KSceneSettingPageClouds.cpp : implementation file
//

#include "stdafx.h"
#include "SceneEditor.h"
#include "KSceneSettingPageClouds.h"
#include "IEKG3DScene.h"
//#include "IEOther.h"
#include "IEKG3DSkySystem.h"
#include "IEKG3DCloud.h"
//#include "KsceneTemplateHelpers.h"
//#include "KSceneInterfaceGetters.h"
#include <algorithm>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNAMIC(KSceneSettingPageClouds, CPropertyPage)

KSceneSettingPageClouds::KSceneSettingPageClouds()
	: KSceneSettingPageBase(KSceneSettingPageClouds::IDD)
{
 
	ZeroMemory(m_tszColorTexBuffer, sizeof(m_tszColorTexBuffer));
	ZeroMemory(m_tszMaskTexBuffer, sizeof(m_tszMaskTexBuffer));
	m_DlgStruct.m_tszColorMapPath = m_tszColorTexBuffer;
	m_DlgStruct.m_tszMaskMapPath = m_tszMaskTexBuffer;
}

KSceneSettingPageClouds::~KSceneSettingPageClouds()
{
}

void KSceneSettingPageClouds::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_SS_TARGET_TREE, m_tree);
    DDX_Text(pDX, IDC_TEXTURE_CLOUD, m_strText);
    DDX_Text(pDX, IDC_TEXTURE_MASK,  m_strMask);
    DDX_Text(pDX, IDC_HEIGHT,  m_fHeight);
    DDX_Text(pDX, IDC_SPEED,   m_fSpeed);
    DDX_Text(pDX, IDC_CLOUD_SIZE, m_fSize);
    DDX_Slider(pDX, IDC_ANGEL, m_nAngel);
    DDX_Text(pDX, IDC_CLOUD_UV, m_nUVRepeat);
	DDX_Text(pDX, IDC_CLOUD_VISIBILITY, m_fVisibility);
    DDX_Text(pDX, IDC_CLOUD_ALPHAREF, m_fAlphaRef);
    DDX_Text(pDX, IDC_CLOUD_TEXFACTOR, m_fTexFactor);
	DDX_Control(pDX, IDC_COLOR1, m_colorButtonDiffuse);
	DDX_Control(pDX, IDC_COLOR2, m_colorButtonAmbient);
}


BEGIN_MESSAGE_MAP(KSceneSettingPageClouds, CPropertyPage)
    ON_NOTIFY(TVN_SELCHANGED, IDC_SS_TARGET_TREE, &KSceneSettingPageClouds::OnTvnSelchangedTree)
    ON_WM_TIMER()
    ON_BN_CLICKED(IDC_SS_LIST_BN_DEL, &KSceneSettingPageClouds::OnBnClickedDel)
    ON_BN_CLICKED(IDC_SS_LIST_BN_ADD, &KSceneSettingPageClouds::OnBnClickedAdd)
    ON_BN_CLICKED(IDC_CLOUD, &KSceneSettingPageClouds::OnBnClickedCloud)
    ON_BN_CLICKED(IDC_MASK, &KSceneSettingPageClouds::OnBnClickedMask)
	ON_BN_CLICKED(IDC_CHECK1, &KSceneSettingPageClouds::OnBnClickedEnableColorControl)
END_MESSAGE_MAP()


BOOL KSceneSettingPageClouds::OnSetActive()
{
	BOOL bRet = CPropertyPage::OnSetActive();
	KG_PROCESS_ERROR(bRet);

    IEKG3DScene*	lpCurScene = g_pGraphicsTool->GetScene();
    KG_PROCESS_ERROR(lpCurScene);

    ((CSliderCtrl*)(GetDlgItem(IDC_ANGEL)))->SetRange(0, 359);

    UpdateTree();
    SetTimer(0, 50, NULL);
Exit0:
    return bRet;
}
BOOL KSceneSettingPageClouds::OnKillActive()
{
	this->KillTimer(0);
	return CPropertyPage::OnKillActive();
}
BOOL KSceneSettingPageClouds::UpdateTree()
{
    //vector<KG3DCloud*>::iterator i;
    //IEKG3DScene*	lpCurScene   = NULL;
    HTREEITEM   hHead = NULL;	
    HTREEITEM   hTreeItem;
	HRESULT	hr;
    char szCloud[64];
    
   // lpCurScene   = g_pGraphicsTool->GetScene();
    //KG_PROCESS_ERROR(lpCurScene);

   
    //DeleteAllItems会触发OnTVNSelChange，所以先把所有Item的Data置空
    for (HTREEITEM hIt = m_tree.GetRootItem(); NULL != hIt; hIt = m_tree.GetNextSiblingItem(hIt))
    {
        m_tree.SetItemData(hIt, NULL);
    }
    m_tree.DeleteAllItems();

    int nCloudVecSize = 0;
	IEKG3DSkySystem* pSkySystem = KSH::GetEngineInterface<KG3DTYPE_ENVEFF_SKYSYSTEM_SYSTEM>();
	//lpCurScene->GetSkySystem(&pSkySystem);
    KG_PROCESS_ERROR(pSkySystem);
	hr = pSkySystem->GetCloudVecSize(&nCloudVecSize);
	IEKG3DCloud* pCloud;
	KG_COM_PROCESS_ERROR(hr);
	for (int i = 0; i < nCloudVecSize; i++)
	{
		if (SUCCEEDED(pSkySystem->GetCloud(i, &pCloud)))
		{
			_ASSERTE(pCloud);
			sprintf_s(szCloud, "Cloud%d", i);
			hTreeItem = m_tree.InsertItem(szCloud);
			if (NULL != hTreeItem)
			{
				m_tree.SetItemData(hTreeItem, (DWORD_PTR)pCloud);
			}	
		}
		
	}
    hHead = m_tree.GetRootItem();
    if (hHead)
        m_tree.SelectItem(hHead);

Exit0:
    return TRUE;
}

void KSceneSettingPageClouds::OnTimer(UINT_PTR uIDEvent)
{ 
	if(uIDEvent == 0)
	{
		//UpdateData();
		
		CDataExchange dx(this, TRUE);
		DDX_Slider(&dx, IDC_ANGEL, m_nAngel);

		CRect rect;
		GetDlgItem(IDC_VIEW)->GetWindowRect(&rect);
		ScreenToClient(&rect);
		//CDC* pDC = GetDlgItem(IDC_VIEW)->GetDC();
		CClientDC dc(this);
		//if (pDC)
		{
			CPen pen(PS_SOLID, 5, RGB(255, 0, 0));
			dc.SelectObject(&pen);
			POINT center;
			POINT lineto;
			center.x = rect.left + rect.Width() / 2;
			center.y = rect.top + rect.Height() / 2;

			lineto.x = (LONG)(center.x - 60 * cosf(D3DXToRadian(m_nAngel)));
			lineto.y = (LONG)(center.y + 60 * sinf(D3DXToRadian(m_nAngel)));

			dc.FillRect(&rect, &CBrush(RGB(100, 100, 100)));

			dc.MoveTo(center);
			dc.LineTo(lineto);
		}
	}

//Exit0:
    CPropertyPage::OnTimer(uIDEvent);
}

void KSceneSettingPageClouds::OnTvnSelchangedTree(NMHDR *pNMHDR, LRESULT *pResult)
{
    LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
    IEKG3DCloud* pCloud_N = NULL;
    IEKG3DCloud* pCloud_O = NULL;

    UpdateData(TRUE);

    if (pNMTreeView->itemOld.hItem)
        pCloud_O = (IEKG3DCloud*)m_tree.GetItemData(pNMTreeView->itemOld.hItem);
    if (pCloud_O)
    {
       /*
		 SynFromInterfaceToInner();
				pCloud_O->RequestModification(&m_DlgStruct);*/
		
    }

    if (pNMTreeView->itemNew.hItem)
        pCloud_N = (IEKG3DCloud*)m_tree.GetItemData(pNMTreeView->itemNew.hItem);
    if (pCloud_N)
    {
		//pCloud_N->GetStruct(&m_DlgStruct);
		SynFromInnerToInterface();
    }
    
//Exit0:
}

void KSceneSettingPageClouds::OnBnClickedDel()
{
    //IEKG3DScene*	lpCurScene = g_pGraphicsTool->GetScene();
   // KG_PROCESS_ERROR(lpCurScene);
    IEKG3DCloud* pCloud = GetValidCurCloudPointer(); 
    IEKG3DSkySystem *pSkySystem = NULL;
    if (pCloud)
    {
        pSkySystem = KSH::GetEngineInterface<KG3DTYPE_ENVEFF_SKYSYSTEM_SYSTEM>();
        KG_COM_PROCESS_ERROR(pSkySystem->DelCloud(pCloud));
        UpdateTree();
    }

	return;
Exit0:
    return;
}

void KSceneSettingPageClouds::OnBnClickedAdd()
{
//    char szCloud[64];  
    //IEKG3DScene*	lpCurScene = g_pGraphicsTool->GetScene();
	//KG_PROCESS_ERROR(lpCurScene);
	IEKG3DSkySystem* pSkySystem = KSH::GetEngineInterface<KG3DTYPE_ENVEFF_SKYSYSTEM_SYSTEM>();

   // lpCurScene->GetSkySystem(&pSkySystem);
	KG_PROCESS_ERROR(pSkySystem);
	if (SUCCEEDED(pSkySystem->AddCloud()))
	{
		UpdateTree();
	}
    return;
Exit0:
	return;
}
void KSceneSettingPageClouds::OnBnClickedCloud()
{
    CFileDialog dlg(TRUE);
    if (dlg.DoModal() == IDOK) 
    {
        CString strText = dlg.GetPathName();
		IEKG3DCloud* pCloud = GetValidCurCloudPointer();
		if (NULL != pCloud)
		{
			TCHAR tszTemp[MAX_PATH];
			BOOL bRet = g_GetFilePathFromFullPath(tszTemp,(LPCTSTR)strText);///这个函数记得检查返回
			KG_PROCESS_ERROR(bRet);
			{
				m_strText = tszTemp;
				UpdateData(FALSE);
				KG_COM_PROCESS_ERROR(UpdateByStruct(KG3DCLOUD_MDO_COLORTEX));
			}
		}
    }
	return;
Exit0:
	return;
}

void KSceneSettingPageClouds::OnBnClickedMask()
{
    CFileDialog dlg(TRUE);
    if (dlg.DoModal() == IDOK)
    {
		IEKG3DCloud* pCloud = GetValidCurCloudPointer();
		if (NULL != pCloud)
		{
			TCHAR tszTemp[MAX_PATH];
			CString strMask = dlg.GetPathName();
			BOOL bRet = g_GetFilePathFromFullPath(tszTemp, (LPCTSTR)strMask);///这个函数记得检查返回
			KG_PROCESS_ERROR(bRet);
			m_strMask = tszTemp;
			UpdateData(FALSE);
			KG_COM_PROCESS_ERROR(UpdateByStruct(KG3DCLOUD_MDO_MASKTEX));
		}   
    }
	return;
Exit0:
	return;
}

HRESULT KSceneSettingPageClouds::SynFromInnerToInterface(DWORD_PTR)
{
	IEKG3DCloud* pCloud = NULL;
	HTREEITEM hItem = m_tree.GetSelectedItem();
	HRESULT hr = E_FAIL;
	KG_PROCESS_ERROR(hItem);
	pCloud =  (IEKG3DCloud*)m_tree.GetItemData(hItem);
	KG_PROCESS_ERROR(pCloud);
	hr = pCloud->GetStruct(&m_DlgStruct);
	KG_COM_PROCESS_ERROR(hr);
	m_strText = m_DlgStruct.m_tszColorMapPath;///对CString对象使用=号实际是进行了拷贝
	m_strMask = m_DlgStruct.m_tszMaskMapPath;
	m_fHeight = m_DlgStruct.m_fHeight;
	m_fSpeed = m_DlgStruct.m_fSpeed;
	m_nUVRepeat = m_DlgStruct.m_nUVRepeat;
	m_nAngel = (int)m_DlgStruct.m_fAngel;
	m_fSize = m_DlgStruct.m_fSize;
	m_fVisibility = m_DlgStruct.m_fVisibility;
    m_fAlphaRef   = m_DlgStruct.m_fAlphaRef;
    m_fTexFactor  = m_DlgStruct.m_fTexFector;
	m_colorButtonDiffuse.SetBKColor(m_DlgStruct.m_diffuseColor);
	m_colorButtonAmbient.SetBKColor(m_DlgStruct.m_ambientColor);
	this->CheckDlgButton(IDC_CHECK1, m_DlgStruct.m_bEnableControlColor ? BST_CHECKED : BST_UNCHECKED);
	UpdateData(FALSE);
	return S_OK;
Exit0:
	return E_FAIL;
}
HRESULT KSceneSettingPageClouds::SynFromInterfaceToInner(DWORD_PTR)
{
	UpdateData(TRUE);
	IEKG3DCloud* pCloud = NULL;
	HTREEITEM hItem = m_tree.GetSelectedItem();
	KG_PROCESS_ERROR(hItem);
	pCloud =  (IEKG3DCloud*)m_tree.GetItemData(hItem);
	KG_PROCESS_ERROR(NULL != pCloud);

	m_DlgStruct.m_dwMidificationOption = 0;
	if ( m_strText.GetLength() < MAX_PATH && 0 != _tcscmp(m_DlgStruct.m_tszColorMapPath, (LPCTSTR)m_strText))
	{
		_tcscpy_s(m_DlgStruct.m_tszColorMapPath, MAX_PATH, (LPCTSTR)m_strText);
		m_DlgStruct.m_dwMidificationOption |= KG3DCLOUD_MDO_COLORTEX; 
	}
	if ( m_strMask.GetLength() < MAX_PATH && 0 != _tcscmp(m_DlgStruct.m_tszMaskMapPath, (LPCTSTR)m_strMask))
	{
		_tcscpy_s(m_DlgStruct.m_tszMaskMapPath, MAX_PATH, (LPCTSTR)m_strMask);
		m_DlgStruct.m_dwMidificationOption |= KG3DCLOUD_MDO_MASKTEX; 
	}
	if (m_fHeight != m_DlgStruct.m_fHeight)
	{
		m_DlgStruct.m_fHeight = m_fHeight;
		m_DlgStruct.m_dwMidificationOption |= KG3DCLOUD_MDO_MESH;
	}
	if (m_fSize != m_DlgStruct.m_fSize)
	{
		m_DlgStruct.m_fSize = m_fSize;
		m_DlgStruct.m_dwMidificationOption |= KG3DCLOUD_MDO_MESH;
	}
	if (m_nUVRepeat != m_DlgStruct.m_nUVRepeat)
	{
		m_DlgStruct.m_nUVRepeat = m_nUVRepeat;
		m_DlgStruct.m_dwMidificationOption |= KG3DCLOUD_MDO_MESH;
	}
	if (m_nAngel != (int)m_DlgStruct.m_fAngel)
	{
		m_DlgStruct.m_fAngel = (float)m_nAngel;
		m_DlgStruct.m_dwMidificationOption |= KG3DCLOUD_MDO_MOVEMENT;
	}
	if (m_fSpeed != m_DlgStruct.m_fSpeed)
	{
		m_DlgStruct.m_fSpeed = m_fSpeed;
		m_DlgStruct.m_dwMidificationOption |= KG3DCLOUD_MDO_MOVEMENT;
	}
	if (m_fVisibility != m_DlgStruct.m_fVisibility)
	{
		m_DlgStruct.m_fVisibility = m_fVisibility;
		m_DlgStruct.m_dwMidificationOption |= KG3DCLOUD_MDO_VISIBILITY;
	}

    if (m_fAlphaRef != m_DlgStruct.m_fAlphaRef)
    {
        m_DlgStruct.m_fAlphaRef = m_fAlphaRef;
        m_DlgStruct.m_dwMidificationOption |= KG3DCLOUD_MDO_ALPHAREF;
    }

    if (MT_FLOAT_UNEQUAL(m_DlgStruct.m_fTexFector, m_fTexFactor))
    {
        m_DlgStruct.m_fTexFector = m_fTexFactor;
        m_DlgStruct.m_dwMidificationOption |= KG3DCLOUD_MDO_TEXFACTOR;
    }

	m_DlgStruct.m_diffuseColor = m_colorButtonDiffuse.GetBkColor();
	m_DlgStruct.m_ambientColor = m_colorButtonAmbient.GetBkColor();
	m_DlgStruct.m_dwMidificationOption |= KG3DCLOUD_MDO_COLOR;

	m_DlgStruct.m_bEnableControlColor = this->IsDlgButtonChecked(IDC_CHECK1);
	m_DlgStruct.m_dwMidificationOption |= KG3DCLOUD_MDO_ENABLE_CONTRL_COLOR;
	
	return pCloud->RequestModification(&m_DlgStruct);	
	
Exit0:
	return E_FAIL;
}
IEKG3DCloud* KSceneSettingPageClouds::GetValidCurCloudPointer()
{
	//IEKG3DScene* lpCurScene = g_pGraphicsTool->GetScene();
	//KG_PROCESS_ERROR(lpCurScene);
	IEKG3DSkySystem* pSkySystem = KSH::GetEngineInterface<KG3DTYPE_ENVEFF_SKYSYSTEM_SYSTEM>();
   // lpCurScene->GetSkySystem(&pSkySystem);
	BOOL bIsValid = FALSE;
	KG_PROCESS_ERROR(pSkySystem);
	HTREEITEM hSelItem = m_tree.GetSelectedItem();
	IEKG3DCloud* pCloud = NULL;
	if (hSelItem)
		pCloud = (IEKG3DCloud*)m_tree.GetItemData(hSelItem);
	pSkySystem->IsCloudPointerValid(pCloud, &bIsValid);
	if (TRUE == bIsValid)
	{
		_ASSERTE(pCloud);return pCloud;
	}
Exit0:
	return NULL;
}

HRESULT KSceneSettingPageClouds::UpdateByStruct(DWORD dwOption /*NULL*/)
{
	/*
	IEKG3DCloud* pCloud = GetValidCurCloudPointer(); 
		KG_PROCESS_ERROR(NULL != pCloud);
		SynFromInterfaceToInner();
		if (dwOption != NULL)
		{
			m_DlgStruct.m_dwMidificationOption = dwOption;
		}
		HRESULT hr = pCloud->RequestModification(&m_DlgStruct);
		pCloud->GetStruct(&m_DlgStruct);
		CollectStructToInterface();
		KG_COM_PROCESS_ERROR(hr);
		return S_OK;
	Exit0:
		return E_FAIL;*/
	HRESULT hr = SynFromInterfaceToInner();
	SynFromInnerToInterface();
	return hr;
}

void KSceneSettingPageClouds::OnBnClickedEnableColorControl()
{
	
	SynFromInterfaceToInner();
	SynFromInnerToInterface();

	return;
}


/*
void KSceneSettingPageClouds::OnBnClickedSsCloudBnApplymod()
{
	// TODO: Add your control notification handler code here
	UpdateByStruct(NULL);
}*/

