// KSceneEditorPVSSetupDialog.cpp : implementation file
//

#include "stdafx.h"
#include "SceneEditor.h"
#include "KSceneEditorPVSSetupDialog.h"
#include "IEEditor.h"

// KSceneEditorPVSSetupDialog dialog

IMPLEMENT_DYNAMIC(KSceneEditorPVSSetupDialog, CDialog)

KSceneEditorPVSSetupDialog::KSceneEditorPVSSetupDialog(CWnd* pParent /*=NULL*/)
	: CDialog(KSceneEditorPVSSetupDialog::IDD, pParent)
	, m_fPositionX(0)
	, m_fPositionY(0)
	, m_fPositionZ(0)
	, m_fDirectionX(0)
	, m_fDirectionY(0)
	, m_fDirectionZ(0)
	, m_fWidth(0)
	, m_fHeight(0)
	, m_fBoxLength(0)
	, m_fBoxWidth(0)
	, m_fBoxHeight(0)
	, m_bShowBorder(FALSE)
{
	m_lpSceneEditor = NULL;
	m_lpPVS = NULL;
}

KSceneEditorPVSSetupDialog::~KSceneEditorPVSSetupDialog()
{
}

void KSceneEditorPVSSetupDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_PORTAL, m_ListBoxPortal);
	DDX_Control(pDX, IDC_LIST_OBJECT, m_ListBoxObjectInside);
	DDX_Text(pDX, IDC_EDIT_POSX, m_fPositionX);
	DDX_Text(pDX, IDC_EDIT_POSY, m_fPositionY);
	DDX_Text(pDX, IDC_EDIT_POSZ, m_fPositionZ);
	DDX_Text(pDX, IDC_EDIT_DIRX, m_fDirectionX);
	DDX_Text(pDX, IDC_EDIT_DIRY, m_fDirectionY);
	DDX_Text(pDX, IDC_EDIT_DIRZ, m_fDirectionZ);
	DDX_Text(pDX, IDC_EDIT_WIDTH, m_fWidth);
	DDX_Text(pDX, IDC_EDIT_HEIGHT, m_fHeight);
	DDX_Text(pDX, IDC_EDIT_BOXL, m_fBoxLength);
	DDX_Text(pDX, IDC_EDIT_BOXW, m_fBoxWidth);
	DDX_Text(pDX, IDC_EDIT_BOXH, m_fBoxHeight);
	DDX_Check(pDX, IDC_CHECK_USEBOX, m_bShowBorder);
}




BEGIN_MESSAGE_MAP(KSceneEditorPVSSetupDialog, CDialog)
	ON_WM_SHOWWINDOW()
	ON_BN_CLICKED(IDC_BUTTON_CONVERPPVS, &KSceneEditorPVSSetupDialog::OnBnClickedButtonConverppvs)
	ON_LBN_SELCHANGE(IDC_LIST_PORTAL, &KSceneEditorPVSSetupDialog::OnLbnSelchangeListPortal)
	ON_EN_CHANGE(IDC_EDIT_HEIGHT, &KSceneEditorPVSSetupDialog::OnEnChangeEditHeight)
	ON_BN_CLICKED(IDC_BUTTON_ADDPORTAL, &KSceneEditorPVSSetupDialog::OnBnClickedButtonAddportal)
	ON_BN_CLICKED(IDC_BUTTON_DELETEPORTAL, &KSceneEditorPVSSetupDialog::OnBnClickedButtonDeleteportal)
	ON_BN_CLICKED(IDC_BUTTON51, &KSceneEditorPVSSetupDialog::OnBnClickedButtonAddobject)
	ON_EN_CHANGE(IDC_EDIT_POSX, &KSceneEditorPVSSetupDialog::OnEnChangeEditPosx)
	ON_EN_CHANGE(IDC_EDIT_POSY, &KSceneEditorPVSSetupDialog::OnEnChangeEditPosy)
	ON_EN_CHANGE(IDC_EDIT_POSZ, &KSceneEditorPVSSetupDialog::OnEnChangeEditPosz)
	ON_EN_CHANGE(IDC_EDIT_DIRX, &KSceneEditorPVSSetupDialog::OnEnChangeEditDirx)
	ON_EN_CHANGE(IDC_EDIT_DIRY, &KSceneEditorPVSSetupDialog::OnEnChangeEditDiry)
	ON_EN_CHANGE(IDC_EDIT_DIRZ, &KSceneEditorPVSSetupDialog::OnEnChangeEditDirz)
	ON_EN_CHANGE(IDC_EDIT_WIDTH, &KSceneEditorPVSSetupDialog::OnEnChangeEditWidth)
	ON_BN_CLICKED(IDC_BUTTON2, &KSceneEditorPVSSetupDialog::OnBnClickedButton2)
	ON_EN_CHANGE(IDC_EDIT_BOXW, &KSceneEditorPVSSetupDialog::OnEnChangeEditBoxw)
	ON_EN_CHANGE(IDC_EDIT_BOXH, &KSceneEditorPVSSetupDialog::OnEnChangeEditBoxh)
	ON_EN_CHANGE(IDC_EDIT_BOXL, &KSceneEditorPVSSetupDialog::OnEnChangeEditBoxl)
	ON_BN_CLICKED(IDC_CHECK_USEBOX, &KSceneEditorPVSSetupDialog::OnBnClickedCheckUsebox)
END_MESSAGE_MAP()


// KSceneEditorPVSSetupDialog message handlers

void KSceneEditorPVSSetupDialog::OnShowWindow(BOOL bShow, UINT nStatus)
{
	HRESULT hr = E_FAIL;
	CDialog::OnShowWindow(bShow, nStatus);

	if(!m_lpSceneEditor)
		return;

	hr = m_lpSceneEditor->GetCurrentSelectedObjectPVS(&m_lpPVS);
	KGLOG_COM_PROCESS_ERROR(hr);

	hr = S_OK;
Exit0:
	FillData();
}

void KSceneEditorPVSSetupDialog::OnBnClickedButtonConverppvs()
{
	if(!m_lpSceneEditor)
		return;

	HRESULT hr = m_lpSceneEditor->ConvertCurrentSelectedObjectToPVS(&m_lpPVS);
	if (SUCCEEDED(hr))
	{
		FillData();
	}
}

void KSceneEditorPVSSetupDialog::FillData()
{
	FillPortalListBox();
	FillObjectInsideListBox();

	if(!m_lpSceneEditor)
		return;
	if(!m_lpPVS)
		return;

	//m_lpPVS->GetBorderBox(&m_fBoxLength,&m_fBoxWidth,&m_fBoxHeight);

	m_bShowBorder = m_lpPVS->GetRenderBorder();
	UpdateData(FALSE);
}

void KSceneEditorPVSSetupDialog::OnLbnSelchangeListPortal()
{
	//if(!m_lpSceneEditor)
	//	return;
	//if(!m_lpPVS)
	//	return;

	//int nIndex = m_ListBoxPortal.GetCurSel();
	//if(nIndex==-1)
	//	return;
	//D3DXVECTOR3 vPos,vDir;
	//HRESULT hr = m_lpPVS->GetPortal(nIndex,&vPos,&vDir,&m_fWidth,&m_fHeight);


	//m_fPositionX = vPos.x;
	//m_fPositionY = vPos.y;
	//m_fPositionZ = vPos.z;
	//m_fDirectionX= vDir.x;
	//m_fDirectionY= vDir.y;
	//m_fDirectionZ= vDir.z;

	//UpdateData(FALSE);
}


void KSceneEditorPVSSetupDialog::FillPortalListBox()
{
	/*{
		int nCount = m_ListBoxPortal.GetCount();
		for (int i=0;i<nCount;i++)
		{
			m_ListBoxPortal.DeleteString(0);
		}
	}

	if(!m_lpSceneEditor)
		return;
	if(!m_lpPVS)
		return;

	int nCount = m_lpPVS->GetPortalCount();
	for (int i=0;i<nCount;i++)
	{
		D3DXVECTOR3 vPos,vDir;
		float Width,Height;
		m_lpPVS->GetPortal(i,&vPos,&vDir,&Width,&Height);

		TCHAR name[MAX_PATH];
		wsprintf(name,"Portal %d",i);

		int nIndex = m_ListBoxPortal.AddString(name);
		m_ListBoxPortal.SetItemData(nIndex,(DWORD)i);
	}
	return ;*/
}

void KSceneEditorPVSSetupDialog::FillObjectInsideListBox()
{/*
	{
		int nCount = m_ListBoxObjectInside.GetCount();
		for (int i=0;i<nCount;i++)
		{
			m_ListBoxObjectInside.DeleteString(0);
		}
	}

	if(!m_lpSceneEditor)
		return;
	if(!m_lpPVS)
		return;

	int nCount = m_lpPVS->GetInsideObjectCount();
	for (int i=0;i<nCount;i++)
	{
		TCHAR name[MAX_PATH];
		m_lpPVS->GetInsideObject(i,name, _countof(name));

		int nIndex = m_ListBoxObjectInside.AddString(name);
		m_ListBoxObjectInside.SetItemData(nIndex,(DWORD)i);
	}*/
	return;
}

void KSceneEditorPVSSetupDialog::OnEnChangeEditHeight()
{
	SetPortal();
}

void KSceneEditorPVSSetupDialog::OnBnClickedButtonAddportal()
{
	/*if(!m_lpSceneEditor)
		return;
	if(!m_lpPVS)
		return;

	D3DXVECTOR3 vPos(0,0,0);
	D3DXVECTOR3 vDir(0,0,-1);

	int n = m_lpPVS->AddPortal(vPos,vDir,1000,1000);
	if(n!=-1)
	{
		FillData();*/
//	}

} 

void KSceneEditorPVSSetupDialog::OnBnClickedButtonDeleteportal()
{
	if(!m_lpSceneEditor)
		return;
	if(!m_lpPVS)
		return;
	int nIndex = m_ListBoxPortal.GetCurSel();
	if(nIndex==-1)
		return;

	m_lpPVS->DeletePortal(nIndex);

	FillData();
}

void KSceneEditorPVSSetupDialog::OnBnClickedButtonAddobject()
{
	if(!m_lpSceneEditor)
		return;
	if(!m_lpPVS)
		return;
	m_lpSceneEditor->MoveSceneObjectIntoPVS(m_lpPVS);

	FillData();
}

void KSceneEditorPVSSetupDialog::OnEnChangeEditPosx()
{
	SetPortal();
}

void KSceneEditorPVSSetupDialog::SetPortal()
{
	//UpdateData(TRUE);

	//if(!m_lpSceneEditor)
	//	return;
	//if(!m_lpPVS)
	//	return;
	//int nIndex = m_ListBoxPortal.GetCurSel();
	//if(nIndex==-1)
	//	return;

	//D3DXVECTOR3 vPos,vDir;
	//vPos.x = m_fPositionX ;
	//vPos.y = m_fPositionY ;
	//vPos.z = m_fPositionZ ;
	//vDir.x = m_fDirectionX;
	//vDir.y = m_fDirectionY;
	//vDir.z = m_fDirectionZ;

	//HRESULT hr = m_lpPVS->SetPortal(nIndex,vPos,vDir,m_fWidth,m_fHeight);

}

void KSceneEditorPVSSetupDialog::SetBorderBox()
{
	UpdateData(TRUE);

	if(!m_lpSceneEditor)
		return;
	if(!m_lpPVS)
		return;
//	m_lpPVS->SetBorderBox(m_fBoxLength,m_fBoxWidth,m_fBoxHeight);
}

void KSceneEditorPVSSetupDialog::OnEnChangeEditPosy()
{
	SetPortal();
}

void KSceneEditorPVSSetupDialog::OnEnChangeEditPosz()
{
	SetPortal();
}

void KSceneEditorPVSSetupDialog::OnEnChangeEditDirx()
{
	SetPortal();
}

void KSceneEditorPVSSetupDialog::OnEnChangeEditDiry()
{
	SetPortal();
}

void KSceneEditorPVSSetupDialog::OnEnChangeEditDirz()
{
	SetPortal();
}

void KSceneEditorPVSSetupDialog::OnEnChangeEditWidth()
{
	SetPortal();
}

void KSceneEditorPVSSetupDialog::OnBnClickedButton2()//选择边界模型
{
	if(!m_lpPVS)
		return;
	CFileDialog dlg(true, "Mesh", 0, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, "剑网3D模型文件(*.mesh)|*.mesh||");
	if(dlg.DoModal() == IDOK)
	{
		TCHAR Name[256];
		wsprintf(Name,"%s",dlg.GetPathName());

		//m_lpPVS->SetBorderModel(Name);
	}
}

void KSceneEditorPVSSetupDialog::OnEnChangeEditBoxw()
{
	SetBorderBox();
}

void KSceneEditorPVSSetupDialog::OnEnChangeEditBoxh()
{
	SetBorderBox();
}

void KSceneEditorPVSSetupDialog::OnEnChangeEditBoxl()
{
	SetBorderBox();
}

void KSceneEditorPVSSetupDialog::OnBnClickedCheckUsebox()
{
	UpdateData(TRUE);
	if(!m_lpPVS)
		return;
	m_lpPVS->SetRenderBorder(m_bShowBorder);
}
