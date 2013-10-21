// KGNormalMapCtreatorSetting.cpp : implementation file
//

#include "stdafx.h"
#include "SceneEditor.h"
#include "KGNormalMapCtreatorSetting.h"


// KGNormalMapCtreatorSetting dialog

IMPLEMENT_DYNAMIC(KGNormalMapCtreatorSetting, CDialog)

KGNormalMapCtreatorSetting::KGNormalMapCtreatorSetting(NormalMapParam param, CWnd* pParent /*=NULL*/)
	: m_Params(param), CDialog(KGNormalMapCtreatorSetting::IDD, pParent)
    , m_nNp0(0)
    , m_nNp1(0)
    , m_nSp0(0)
    , m_nSp1(0)
    , m_nSp2(0)
    , m_nDp0(0)
    , m_nDp1(0)
    , m_nDp2(0)
    , m_nAp0(0)
    , m_nAp1(0)
    , m_nAp2(0)
    , m_nAp3(0)
    , m_nAp4(0)
    , m_nAp5(0)
{
  
}

KGNormalMapCtreatorSetting::~KGNormalMapCtreatorSetting()
{
}

void KGNormalMapCtreatorSetting::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    DDX_Text(pDX, IDC_EDIT_NM_LEVEL, m_nNp0);
    DDV_MinMaxInt(pDX, m_nNp0, -100, 100);
    DDX_Text(pDX, IDC_EDIT_NM_INTENSITY, m_nNp1);
    DDV_MinMaxInt(pDX, m_nNp1, 0, 500);
    DDX_Text(pDX, IDC_EDIT_SM_LEVEL, m_nSp0);
    DDV_MinMaxInt(pDX, m_nSp0, -100, 100);
    DDX_Text(pDX, IDC_EDIT_SM_BRIGH, m_nSp1);
    DDV_MinMaxInt(pDX, m_nSp1, -100, 100);
    DDX_Text(pDX, IDC_EDIT_SM_CONTRAST, m_nSp2);
    DDV_MinMaxInt(pDX, m_nSp2, 0, 100);
    DDX_Text(pDX, IDC_EDIT_DM_LEVEL, m_nDp0);
    DDV_MinMaxInt(pDX, m_nDp0, -100, 100);
    DDX_Text(pDX, IDC_EDIT_DM_SMOOTH, m_nDp1);
    DDV_MinMaxInt(pDX, m_nDp1, 0, 100);
    DDX_Text(pDX, IDC_EDIT_DM_POSBLUR, m_nDp2);
    DDV_MinMaxInt(pDX, m_nDp2, 0, 100);
    DDX_Text(pDX, IDC_EDIT_AO_PIXELBASE, m_nAp0);
    DDV_MinMaxInt(pDX, m_nAp0, 0, 100);
    DDX_Text(pDX, IDC_EDIT_SAMPLES, m_nAp1);
    DDV_MinMaxInt(pDX, m_nAp1, 1, 10000);
    DDX_Text(pDX, IDC_EDIT_AO_HEIGHTBIAS, m_nAp2);
    DDV_MinMaxInt(pDX, m_nAp2, 0, 100);
    DDX_Text(pDX, IDC_EDIT_AO_INTENSITY, m_nAp3);
    DDV_MinMaxInt(pDX, m_nAp3, 0, 100);
    DDX_Text(pDX, IDC_EDIT_AO_POSBLUR, m_nAp4);
    DDV_MinMaxInt(pDX, m_nAp4, 0, 100);
    DDX_CBIndex(pDX, IDC_COMBO_AO_NORMALIZE, m_nAp5);
    DDV_MinMaxInt(pDX, m_nAp5, 0, 1);
    DDX_Control(pDX, IDC_HELP_LINK, m_HelpLink);
}


BEGIN_MESSAGE_MAP(KGNormalMapCtreatorSetting, CDialog)
    ON_BN_CLICKED(IDOK, &KGNormalMapCtreatorSetting::OnBnClickedOk)
    ON_BN_CLICKED(IDC_BUTTON_DEF, &KGNormalMapCtreatorSetting::OnBnClickedButtonDef)
END_MESSAGE_MAP()


BOOL KGNormalMapCtreatorSetting::OnInitDialog()
{
    BOOL bRes = CDialog::OnInitDialog();

    CComboBox* pCombox = (CComboBox*)GetDlgItem(IDC_COMBO_AO_NORMALIZE);

    pCombox->AddString("False");
    pCombox->AddString("True");

    m_nNp0 = m_Params.np[0];
    m_nNp1 = m_Params.np[1];

    m_nSp0 = m_Params.sp[0];
    m_nSp1 = m_Params.sp[1];
    m_nSp2 = m_Params.sp[2];

    m_nDp0 = m_Params.dp[0];
    m_nDp1 = m_Params.dp[1];
    m_nDp2 = m_Params.dp[2];

    m_nAp0 = m_Params.ap[0];
    m_nAp1 = m_Params.ap[1];
    m_nAp2 = m_Params.ap[2];
    m_nAp3 = m_Params.ap[3];
    m_nAp4 = m_Params.ap[4];
    m_nAp5 = m_Params.ap[5];

    UpdateData(FALSE);

    m_HelpLink.SetURL("http://www.renderingsystems.com/documents.php?folder=sm_cl_1_2_2");
    m_HelpLink.SetColors(RGB(0, 0, 255), RGB(0, 0, 255), RGB(0, 0, 255));
    m_HelpLink.SetVisited(true);

    return bRes;
}

void KGNormalMapCtreatorSetting::OnBnClickedOk()
{ 

    UpdateData();

    m_Params.np[0] = m_nNp0;
    m_Params.np[1] = m_nNp1;

    m_Params.sp[0] = m_nSp0;
    m_Params.sp[1] = m_nSp1;
    m_Params.sp[2] = m_nSp2;

    m_Params.dp[0] = m_nDp0;
    m_Params.dp[1] = m_nDp1;
    m_Params.dp[2] = m_nDp2;

    m_Params.ap[0] = m_nAp0;
    m_Params.ap[1] = m_nAp1;
    m_Params.ap[2] = m_nAp2;
    m_Params.ap[3] = m_nAp3;
    m_Params.ap[4] = m_nAp4;
    m_Params.ap[5] = m_nAp5;

    OnOK();
}

void KGNormalMapCtreatorSetting::OnBnClickedButtonDef()
{
    m_nNp0 = 100;
    m_nNp1 = 200;

    m_nSp0 = 100;
    m_nSp1 = -50;
    m_nSp2 = 52;

    m_nDp0 = 60;
    m_nDp1 = 100;
    m_nDp2 = 12;

    m_nAp0 = 100;
    m_nAp1 = 10;
    m_nAp2 = 10;
    m_nAp3 = 35;
    m_nAp4 = 25;
    m_nAp5 = 0;

    UpdateData(FALSE);
}
