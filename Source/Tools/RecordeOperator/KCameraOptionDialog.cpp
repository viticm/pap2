// KCameraOptionDialog.cpp : implementation file
//

#include "stdafx.h"
#include "RecordeOperator.h"
#include "KCameraOptionDialog.h"


// KCameraOptionDialog dialog

IMPLEMENT_DYNAMIC(KCameraOptionDialog, CDialog)

KCameraOptionDialog::KCameraOptionDialog(IERecPlayer* pPlayer,CWnd* pParent /*=NULL*/)
	: CDialog(KCameraOptionDialog::IDD, pParent)
	, m_nCameraType(1)
	, m_nFreeCamera(1)
{
	m_pPlayer = pPlayer;
	//m_radioBindCamera.SetCheck(TRUE);
}

KCameraOptionDialog::~KCameraOptionDialog()
{
}


static int s_CameraTypeConvertTable[IERecPlayer::CAMERA_TYPE_COUNT] = 
{
	1,//CAMERA_TYPE_RECORD
	0,//CAMERA_TYPE_BIND
	2,//CAMERA_TYPE_BINDPLAYER
};

static IERecPlayer::CameraType s_CameraTypeReverseConvertTable[IERecPlayer::CAMERA_TYPE_COUNT] = 
{
	IERecPlayer::CAMERA_TYPE_BIND, //0,
	IERecPlayer::CAMERA_TYPE_RECORD, //1
	IERecPlayer::CAMERA_TYPE_BINDPLAYER, //2
};

BOOL KCameraOptionDialog::OnInitDialog()
{
	char szText[128];
	if (!m_pPlayer)
		return FALSE;
	if (m_pPlayer->IsFreeCamera())
		m_nFreeCamera = 1;
	else 
		m_nFreeCamera = 0;

	m_nCameraType = s_CameraTypeConvertTable[m_pPlayer->GetCameraType()];
	UpdateData(FALSE);

	_itoa(m_pPlayer->GetBindID(), szText, 10);
	m_editBindID.SetWindowTextA(szText);
	float fAAA = (m_pPlayer->GetAngle() * 180) / M_PI;
	gcvt(fAAA,5,szText);
	m_editAngle.SetWindowTextA(szText);	
	return TRUE;
}

void KCameraOptionDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Radio(pDX,IDC_RADIO1,m_nCameraType); 
	DDX_Control(pDX, IDC_EDIT1, m_editBindID);
	DDX_Control(pDX, IDC_EDIT2, m_editAngle);
	DDX_Check(pDX,IDC_CHECK1,m_nFreeCamera);
}


BEGIN_MESSAGE_MAP(KCameraOptionDialog, CDialog)
	ON_BN_CLICKED(IDOK, &KCameraOptionDialog::OnBnClickedOk)
END_MESSAGE_MAP()


// KCameraOptionDialog message handlers

void KCameraOptionDialog::OnBnClickedOk()
{
	CString szText;
	if (!m_pPlayer)
		return;
	UpdateData(TRUE);
	if (m_nFreeCamera && !m_pPlayer->IsFreeCamera())
		m_pPlayer->FreeCamera();
	else if (!m_nFreeCamera && m_pPlayer->IsFreeCamera())
		m_pPlayer->FreeCamera();

	m_pPlayer->SetCameraType(s_CameraTypeReverseConvertTable[m_nCameraType]);
	
	m_editBindID.GetWindowTextA(szText);
	m_pPlayer->SetBindID(atoi(szText.GetBuffer()));
	m_editAngle.GetWindowTextA(szText);	
	float fAAA = atof(szText.GetBuffer());
	float fAngle = (fAAA * M_PI) / 180;
	m_pPlayer->SetAngle(fAngle);
	OnOK();
}
