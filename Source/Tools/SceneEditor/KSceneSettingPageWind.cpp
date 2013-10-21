////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KSceneSettingPageFog.cpp
//  Version     : 1.0
//  Author      : LiKan
//  CopyRight   : Kingsoft Corporation All rights Reserved
//  History     : 2009-12-2
//  Comment     :
//
////////////////////////////////////////////////////////////////////////////////


#include "stdafx.h"
#include "SceneEditor.h"
#include "KSceneSettingPageWind.h"
#include "../EditInterface/IEKG3DEnvEffWind.h"

// KSceneSettingPageWind dialog

IMPLEMENT_DYNAMIC(KSceneSettingPageWind, CPropertyPage)

KSceneSettingPageWind::KSceneSettingPageWind(CWnd* pParent /*=NULL*/)
: KSceneSettingPageBase(KSceneSettingPageWind::IDD)
, mWindDirX(0)
, mWindDirY(0)
, mWindDirZ(0)
, mWindAmplitude1(0)
, mWindAmplitude2(0)
, mWindWaveLen1(0)
, mWindWaveLen2(0)
, mWindSpeed1(0)
, mWindSpeed2(0)
, mWindWaveScaleUp1(0)
, mWindWaveScaleUp2(0)
, mWindWaveScaleDown1(0)
, mWindWaveScaleDown2(0)
, mWindWaveOffset1(0)
, mWindWaveOffset2(0)
, mbEnableWindWave1(FALSE)
, mbEnableWindWave2(FALSE)
, mbEnableDirWind(FALSE)
, mWindDegree(0)
, mCurrentWindid(0)
{

}

KSceneSettingPageWind::~KSceneSettingPageWind()
{
}

void KSceneSettingPageWind::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_WINDDIR_X, mWindDirX);
	DDX_Text(pDX, IDC_EDIT_WINDDIR_Y, mWindDirY);
	DDX_Text(pDX, IDC_EDIT_WINDDIR_Z, mWindDirZ);
	DDX_Text(pDX, IDC_AMPLITUDE_1, mWindAmplitude1);
	DDX_Text(pDX, IDC_AMPLITUDE_2, mWindAmplitude2);
	DDX_Text(pDX, IDC_WAVE_LEN_1, mWindWaveLen1);
	DDX_Text(pDX, IDC_WAVE_LEN_2, mWindWaveLen2);
	DDX_Text(pDX, IDC_SPEED_1, mWindSpeed1);
	DDX_Text(pDX, IDC_SPEED_2, mWindSpeed2);
	DDX_Text(pDX, IDC_WAVESCALEUP_1, mWindWaveScaleUp1);
	DDX_Text(pDX, IDC_WAVESCALEUP_2, mWindWaveScaleUp2);
	DDX_Text(pDX, IDC_WAVESCALEDOWN_1, mWindWaveScaleDown1);
	DDX_Text(pDX, IDC_WAVESCALEDOWN_2, mWindWaveScaleDown2);
	DDX_Text(pDX, IDC_WAVE_OFFSET_1, mWindWaveOffset1);
	DDX_Text(pDX, IDC_WAVE_OFFSET_2, mWindWaveOffset2);
	DDX_Check(pDX, IDC_WIND_WAVE1_ENABLE_BTN, mbEnableWindWave1);
	DDX_Check(pDX, IDC_WIND_WAVE2_ENABLE_BTN, mbEnableWindWave2);
	DDX_Slider(pDX, IDC_SLIDER_WIND, mWindDegree);

	DDX_Control(pDX, IDC_WIND_TREE, m_Tree);
}


BEGIN_MESSAGE_MAP(KSceneSettingPageWind, CDialog)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_UNIFORM_WINDDIR, &KSceneSettingPageWind::OnBnClickedUniformWinddir)
	ON_BN_CLICKED(IDC_FREE_WINDDIR, &KSceneSettingPageWind::OnBnClickedFreeWinddir)
	ON_BN_CLICKED(IDC_WIND_WAVE2_ENABLE_BTN, &KSceneSettingPageWind::OnBnClickedWindWave2EnableBtn)
	ON_BN_CLICKED(IDC_WIND_WAVE1_ENABLE_BTN, &KSceneSettingPageWind::OnBnClickedWindWave1EnableBtn)
	ON_BN_CLICKED(IDC_WIND_SWAY_U, &KSceneSettingPageWind::OnBnClickedWindSwayU)
	ON_BN_CLICKED(IDC_WIND_SWAY_V, &KSceneSettingPageWind::OnBnClickedWindSwayV)
	ON_BN_CLICKED(IDC_WIND_SWAY_W, &KSceneSettingPageWind::OnBnClickedWindSwayW)
	ON_NOTIFY(TVN_SELCHANGED, IDC_WIND_TREE, &KSceneSettingPageWind::OnTvnSelchangedWindTree)
	ON_BN_CLICKED(IDC_ADD_WIND, &KSceneSettingPageWind::OnBnClickedAddWind)
	ON_BN_CLICKED(IDC_DEL_WIND, &KSceneSettingPageWind::OnBnClickedDelWind)
END_MESSAGE_MAP()


// KSceneSettingPageWind message handlers



HRESULT KSceneSettingPageWind::SynFromInnerToInterface( DWORD_PTR dwOption )
{
	IEKG3DEnvEffWind* pWind = KSH::GetEngineInterfaceEx<IEKG3DEnvEffWind>();
	PERROR(NULL != pWind);
	{
		KG3DWIND_DESCSTRUCT wind;
		wind.m_id = mCurrentWindid;
		pWind->GetStruct(&wind);
		mWindDirX = wind.m_WindDirX;
		mWindDirY = wind.m_WindDirY;
		mWindDirZ = wind.m_WindDirZ;
		m_Sway = wind.m_Sway;
		mWindAmplitude1 = wind.m_WindAmplitude1;
		mWindAmplitude2 = wind.m_WindAmplitude2;
		mWindWaveLen1 = wind.m_WindWaveLen1;
		mWindWaveLen2 = wind.m_WindWaveLen2;
		mWindSpeed1 = wind.m_WindSpeed1;
		mWindSpeed2 = wind.m_WindSpeed2;
		mWindWaveScaleUp1 = wind.m_WindWaveScaleUp1;
		mWindWaveScaleUp2 = wind.m_WindWaveScaleUp2;
		mWindWaveScaleDown1 = wind.m_WindWaveScaleDown1;
		mWindWaveScaleDown2 = wind.m_WindWaveScaleDown2;
		mWindWaveOffset1 = wind.m_WindWaveOffset1;
		mWindWaveOffset2 = wind.m_WindWaveOffset2;
		mbEnableWindWave1 = wind.m_bEnableWave1;
		mbEnableWindWave2 = wind.m_bEnableWave2;
		mbEnableDirWind = wind.m_bEnableDirWind;

		D3DXVECTOR2 v1(mWindDirX, mWindDirZ);
		D3DXVec2Normalize(&v1, &v1);
		D3DXVECTOR2 v2(1.0f, 0);
		mWindDegree = (int)D3DXToDegree(acosf(D3DXVec2Dot(&v1, &v2)));
		if(mWindDirZ < 0)
			mWindDegree  = 360 - mWindDegree;


		UpdateData(FALSE);
		if(mbEnableDirWind)
		{
			((CButton *)GetDlgItem(IDC_FREE_WINDDIR))->SetCheck(false);
			((CButton *)GetDlgItem(IDC_UNIFORM_WINDDIR))->SetCheck(true);
		}
		else
		{
			((CButton *)GetDlgItem(IDC_FREE_WINDDIR))->SetCheck(true);
			((CButton *)GetDlgItem(IDC_UNIFORM_WINDDIR))->SetCheck(false);
		}

		((CButton *)GetDlgItem(IDC_WIND_SWAY_U))->SetCheck(false);
		((CButton *)GetDlgItem(IDC_WIND_SWAY_V))->SetCheck(false);
		((CButton *)GetDlgItem(IDC_WIND_SWAY_W))->SetCheck(false);

		if(m_Sway == 0)
		{
			((CButton *)GetDlgItem(IDC_WIND_SWAY_U))->SetCheck(true);
		}
		else if(m_Sway == 1)
		{
			((CButton *)GetDlgItem(IDC_WIND_SWAY_V))->SetCheck(true);
		}
		else
		{
			((CButton *)GetDlgItem(IDC_WIND_SWAY_W))->SetCheck(true);
		}


	}
	return S_OK;
Exit0:
	return E_FAIL;
}


BOOL KSceneSettingPageWind::UpdateTree()
{

	m_Tree.DeleteAllItems();
	int nCloudVecSize = 0;
	IEKG3DEnvEffWind* pWind = KSH::GetEngineInterfaceEx<IEKG3DEnvEffWind>();
	if(!pWind)
		return E_FAIL;

	struct WindVistor: public IEKG3DEnvWindVistor
	{
		CTreeCtrl *tree;
		void vist(KG3DWIND_DESCSTRUCT * wind)
		{
			HTREEITEM   hHead = NULL;	
			HTREEITEM   hTreeItem;

			TCHAR buf[128];
			sprintf_s(buf, "Wind_%d", wind->m_id);
			hTreeItem = tree->InsertItem(buf);
			if (NULL != hTreeItem)
		 {
			 tree->SetItemData(hTreeItem, (DWORD_PTR)wind);
			}	
		}
	};
	WindVistor vistor;
	vistor.tree = &m_Tree;
	pWind->DoVist(vistor);

	return  S_OK;
}

HRESULT KSceneSettingPageWind::SynFromInterfaceToInner( DWORD_PTR dwOption )
{
	IEKG3DEnvEffWind* pWind = KSH::GetEngineInterfaceEx<IEKG3DEnvEffWind>();
	PERROR(NULL != pWind);
	{
		UpdateData(TRUE);
		KG3DWIND_DESCSTRUCT wind;
		wind.m_id = mCurrentWindid;
		wind.m_dwMidificationOption = (DWORD)KG3DWIND_MDO_ALL;
		wind.m_WindDirX = mWindDirX;
		wind.m_WindDirY = mWindDirY;
		wind.m_WindDirZ = mWindDirZ;
		wind.m_Sway = m_Sway;
		wind.m_WindAmplitude1 = mWindAmplitude1;
		wind.m_WindAmplitude2 = mWindAmplitude2;
		wind.m_WindWaveLen1 = mWindWaveLen1;
		wind.m_WindWaveLen2 = mWindWaveLen2;
		wind.m_WindSpeed1 = mWindSpeed1;
		wind.m_WindSpeed2 = mWindSpeed2;
		wind.m_WindWaveScaleUp1 = mWindWaveScaleUp1;
		wind.m_WindWaveScaleUp2 = mWindWaveScaleUp2;
		wind.m_WindWaveScaleDown1 = mWindWaveScaleDown1;
		wind.m_WindWaveScaleDown2 = mWindWaveScaleDown2;
		wind.m_WindWaveOffset1 = mWindWaveOffset1;
		wind.m_WindWaveOffset2 = mWindWaveOffset2;
		wind.m_bEnableWave1 = mbEnableWindWave1 ? true : false;
		wind.m_bEnableWave2 = mbEnableWindWave2 ? true : false;
		wind.m_bEnableDirWind = mbEnableDirWind ? true : false;
		pWind->RequestModification(&wind);
	}
	return S_OK;
Exit0:

	return E_FAIL;
}

void KSceneSettingPageWind::OnBnClickedUniformWinddir()
{
	mbEnableDirWind = true;
	((CButton *)GetDlgItem(IDC_FREE_WINDDIR))->SetCheck(false);	
}

void KSceneSettingPageWind::OnBnClickedFreeWinddir()
{
	mbEnableDirWind = false;
	((CButton *)GetDlgItem(IDC_UNIFORM_WINDDIR))->SetCheck(false);
}

void KSceneSettingPageWind::OnBnClickedWindWave2EnableBtn()
{
	// TODO: Add your control notification handler code here
}

void KSceneSettingPageWind::OnBnClickedWindWave1EnableBtn()
{
	// TODO: Add your control notification handler code here
}

BOOL KSceneSettingPageWind::OnSetActive()
{
	SynFromInnerToInterface(0);
	((CSliderCtrl*)(GetDlgItem(IDC_SLIDER_WIND)))->SetRange(0, 359);

	UpdateTree();

	SetTimer(0, 50, NULL);
	return KSceneSettingPageBase::OnSetActive();
}

BOOL KSceneSettingPageWind::OnKillActive()
{
	KillTimer(0);
	return CPropertyPage::OnKillActive();
}


void KSceneSettingPageWind::OnTimer(UINT_PTR uIDEvent)
{
	if(uIDEvent == 0)
	{
		//UpdateData();
		CDataExchange dx(this, TRUE);
		DDX_Slider(&dx, IDC_SLIDER_WIND, mWindDegree);

		CRect rect;
		GetDlgItem(IDC_VIEW_WIND)->GetWindowRect(&rect);
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

			lineto.x = (LONG)(center.x - 60 * cosf(D3DXToRadian(mWindDegree)));
			lineto.y = (LONG)(center.y + 60 * sinf(D3DXToRadian(mWindDegree)));

			dc.FillRect(&rect, &CBrush(RGB(100, 100, 100)));

			dc.MoveTo(center);
			dc.LineTo(lineto);

			mWindDirX = cosf(D3DXToRadian(mWindDegree));
			mWindDirZ = sinf(D3DXToRadian(mWindDegree));

			CString x;
			x.Format("%f",mWindDirX);
			CString z;
			z.Format("%f",mWindDirZ);
			((CEdit *)GetDlgItem(IDC_EDIT_WINDDIR_X))->SetWindowText(x);
			((CEdit *)GetDlgItem(IDC_EDIT_WINDDIR_Z))->SetWindowText(z);

		}
	}
	//Exit0:

	CPropertyPage::OnTimer(uIDEvent);
}

void KSceneSettingPageWind::OnBnClickedWindSwayU()
{
	// TODO: Add your control notification handler code here
	m_Sway = 0;
}

void KSceneSettingPageWind::OnBnClickedWindSwayV()
{
	// TODO: Add your control notification handler code here
	m_Sway = 1;
}

void KSceneSettingPageWind::OnBnClickedWindSwayW()
{
	// TODO: Add your control notification handler code here
	m_Sway = 2;
}

void KSceneSettingPageWind::OnTvnSelchangedWindTree(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	if (pNMTreeView->itemOld.hItem)
	{
		KG3DWIND_DESCSTRUCT *pWind = (KG3DWIND_DESCSTRUCT*)m_Tree.GetItemData(pNMTreeView->itemNew.hItem);
		mCurrentWindid = pWind->m_id;
		SynFromInnerToInterface(0);
	}
	*pResult = 0;
}

void KSceneSettingPageWind::OnBnClickedAddWind()
{
	// TODO: Add your control notification handler code here
	KG3DWIND_DESCSTRUCT wind;
	wind.SetDefaultValue();

	IEKG3DEnvEffWind* pWind = KSH::GetEngineInterfaceEx<IEKG3DEnvEffWind>();
	if(NULL != pWind)
	{
		pWind->RequestModification(&wind);
		mCurrentWindid = wind.m_id;
		UpdateTree();
	}
}

void KSceneSettingPageWind::OnBnClickedDelWind()
{
	IEKG3DEnvEffWind* pWind = KSH::GetEngineInterfaceEx<IEKG3DEnvEffWind>();
	if(mCurrentWindid == 0)
	{
		MessageBox("目前不允许删除第一项", "提示", 0);
		return; //暂时这样写，限制不能删除第一项，将来如果可以在场景中选择具体使用哪个风id的时候再做进一步的限定
	}

	pWind->DelWind(mCurrentWindid);
	UpdateTree();

}
