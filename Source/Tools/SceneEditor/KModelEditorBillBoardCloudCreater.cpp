// KModelEditorBillBoardCloudCreater.cpp : implementation file
//

#include "stdafx.h"
#include "SceneEditor.h"
#include "KModelEditorBillBoardCloudCreater.h"
#include "IEEditor.h"

// KModelEditorBillBoardCloudCreater dialog

IMPLEMENT_DYNAMIC(KModelEditorBillBoardCloudCreater, CDialog)

KModelEditorBillBoardCloudCreater::KModelEditorBillBoardCloudCreater(CWnd* pParent /*=NULL*/)
	: CDialog(KModelEditorBillBoardCloudCreater::IDD, pParent)
	, m_fPrecision(0)
{
	m_lpCreater = NULL;
}

KModelEditorBillBoardCloudCreater::~KModelEditorBillBoardCloudCreater()
{
	m_lpCreater = NULL;
}

void KModelEditorBillBoardCloudCreater::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT1, m_fPrecision);
}


BEGIN_MESSAGE_MAP(KModelEditorBillBoardCloudCreater, CDialog)
	ON_BN_CLICKED(IDC_BUTTON1, &KModelEditorBillBoardCloudCreater::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON2, &KModelEditorBillBoardCloudCreater::OnBnClickedButton2)
	ON_BN_CLICKED(IDC_BUTTON3, &KModelEditorBillBoardCloudCreater::OnBnClickedButton3)
	ON_EN_CHANGE(IDC_EDIT1, &KModelEditorBillBoardCloudCreater::OnEnChangeEdit1)
	ON_WM_SHOWWINDOW()
END_MESSAGE_MAP()


// KModelEditorBillBoardCloudCreater message handlers

void KModelEditorBillBoardCloudCreater::OnBnClickedButton1()
{
	if(m_lpCreater)
	{
		//m_lpCreater->CreateBillBoardCloudForModel();
		DWORD dwCount = m_lpCreater->GetBillBoardCount();

		TCHAR name[MAX_PATH];
		wsprintf(name,"BillBoard %d",dwCount);
		this->SetWindowText(name);
	}
}

void KModelEditorBillBoardCloudCreater::OnBnClickedButton2()
{
	if(m_lpCreater)
	{
		m_lpCreater->ComputeUV();
	}
}

void KModelEditorBillBoardCloudCreater::OnBnClickedButton3()
{
	if(m_lpCreater)
	{
		m_lpCreater->ComputeTexture();
	}
}

void KModelEditorBillBoardCloudCreater::OnEnChangeEdit1()
{
	UpdateData(TRUE);

	if(m_lpCreater)
	{
		m_lpCreater->SetPrecision(m_fPrecision);
	}
}

void KModelEditorBillBoardCloudCreater::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CDialog::OnShowWindow(bShow, nStatus);
	if (m_lpCreater)
	{
		m_fPrecision = m_lpCreater->GetPrecision();
	}
	UpdateData(FALSE);
}
