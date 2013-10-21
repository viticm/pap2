// KSceneModelEditorSFXSceneSFXDialog.cpp : implementation file
//

#include "stdafx.h"
#include "sceneeditor.h"
#include "KSceneModelEditorSFXSceneSFXDialog.h"
//#include "KModelSceneSFX.h"
#include ".\kscenemodeleditorsfxscenesfxdialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// KSceneModelEditorSFXSceneSFXDialog dialog


KSceneModelEditorSFXSceneSFXDialog::KSceneModelEditorSFXSceneSFXDialog(CWnd* pParent /*=NULL*/)
	: CDialog(KSceneModelEditorSFXSceneSFXDialog::IDD, pParent)
	, m_fYSpeed(0.05F)
	, m_iWhichBB(2)
	, m_fXAccSpeed(0.01F)
	, m_iWidth(10)
	, m_iHeight(50)
	, m_iMinWidth(10)
	, m_iMaxWidth(20)
	, m_iMinHeight(50)
	, m_iMaxHeight(60)
	, m_fZAccSpeed(0)
{
	//{{AFX_DATA_INIT(KSceneModelEditorSFXSceneSFXDialog)
	m_dwMaxParticle = 100;
	m_fYAccSpeed = 0.05F;
	m_fInitSpeed = D3DXVECTOR3(0.0F,0.5F,0.0);
	m_v3InitMinPos = D3DXVECTOR3(-1000.0,500.0F,-1000.0F);
	m_v3InitMaxPos = D3DXVECTOR3(1000.0,1000.0F,1000.0F);
	//}}AFX_DATA_INIT
	//m_lpSceneSFX = NULL;
	m_pde = NULL;
	m_fAlphaBase = 200.0F;
}

void KSceneModelEditorSFXSceneSFXDialog::SetData(const BBHeader &bbheader)
{
	//m_lpSceneSFX->SetAccSpeed(bbheader.m_v3AccSpeed);
	//m_lpSceneSFX->SetInitMinPos(bbheader.m_v3InitMinPos);
	//m_lpSceneSFX->SetInitMaxPos(bbheader.m_v3InitMaxPos);
	//m_lpSceneSFX->SetMaxParticle(bbheader.m_dwMaxParticle);
	//m_lpSceneSFX->SetInitSpeed(bbheader.m_fInitSpeed);
	//m_lpSceneSFX->SetHeight(bbheader.m_iMinHeight,bbheader.m_iMaxHeight);
	//m_lpSceneSFX->SetWidth(bbheader.m_iMinWidth,bbheader.m_iMaxWidth);
	//m_lpSceneSFX->SetWhichBB(bbheader.m_iWhichBB);
//	m_lpSceneSFX->SetAlphaBase(bbheader.m_fAlphaBase);

	//m_dwMaxParticle = bbheader.m_dwMaxParticle;
	//m_fXAccSpeed = bbheader.m_v3AccSpeed.x;
	//m_fYAccSpeed = bbheader.m_v3AccSpeed.y;
	//m_fZAccSpeed = bbheader.m_v3AccSpeed.z;
	//m_iMinWidth = bbheader.m_iMinWidth;
	//m_iMaxWidth = bbheader.m_iMaxWidth; 
	//m_iMinHeight = bbheader.m_iMinHeight;
	//m_iMaxHeight = bbheader.m_iMaxHeight;
	//m_fInitSpeed = bbheader.m_fInitSpeed;
	//m_v3InitMinPos = bbheader.m_v3InitMinPos;
	//m_v3InitMaxPos = bbheader.m_v3InitMaxPos;
	//m_iWhichBB = bbheader.m_iWhichBB;
//	m_fAlphaBase = bbheader.m_fAlphaBase;

/*	if(m_pde)
	{
		CDialog::DoDataExchange(m_pde);
		DDX_Text(m_pde, IDC_EDIT3, m_iWhichBB);
	}*/
}

void KSceneModelEditorSFXSceneSFXDialog::InPutdata(BBHeader &bbheader)
{
	//bbheader.m_dwMaxParticle = m_dwMaxParticle;
	//bbheader.m_fInitSpeed = m_fInitSpeed;
	//bbheader.m_iMaxHeight = m_iMaxHeight;
	//bbheader.m_iMaxWidth = m_iMaxWidth;
	//bbheader.m_iMinHeight = m_iMinHeight;
	//bbheader.m_iMinWidth = m_iMinWidth;
	//bbheader.m_iWhichBB = m_iWhichBB;
	//bbheader.m_v3AccSpeed = D3DXVECTOR3(m_fXAccSpeed,m_fYAccSpeed,m_fZAccSpeed);
	//bbheader.m_v3InitMaxPos = m_v3InitMaxPos;
	//bbheader.m_v3InitMinPos = m_v3InitMinPos;
	//bbheader.m_fAlphaBase = m_fAlphaBase;
}


void KSceneModelEditorSFXSceneSFXDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	if(m_pde == NULL)
		m_pde = pDX;

	//{{AFX_DATA_MAP(KSceneModelEditorSFXSceneSFXDialog)
	DDX_Text(pDX, IDC_EDIT1, m_dwMaxParticle);
	DDV_MinMaxDWord(pDX, m_dwMaxParticle, 1, 2000);


	DDX_Text(pDX, IDC_EDIT4, m_fXAccSpeed);
	DDV_MinMaxFloat(pDX, m_fXAccSpeed, -100.0, 100.0);
	DDX_Text(pDX, IDC_EDIT8, m_fZAccSpeed);
	DDV_MinMaxFloat(pDX, m_fZAccSpeed, -100.0, 100.0);
	//if(m_iWhichBB != FOG)
	//{	
	//	DDX_Text(pDX, IDC_EDIT2, m_fYAccSpeed);
	//	DDV_MinMaxFloat(pDX, m_fYAccSpeed, 0.0, 100.0);	
	//}
	//else
	//{
	//	DDX_Text(pDX, IDC_EDIT2, m_fYAccSpeed);
	//	DDV_MinMaxFloat(pDX, m_fYAccSpeed, -100.0, 100.0);	
	//}


	DDX_Text(pDX, IDC_EDIT3, m_iWhichBB);
	DDV_MinMaxInt(pDX, m_iWhichBB, 0, 3);

	DDX_Text(pDX, IDC_EDIT5, m_iMinWidth);
	DDV_MinMaxInt(pDX, m_iMinWidth, 1, m_iMaxWidth);

	DDX_Text(pDX, IDC_EDIT9, m_iMaxWidth);
	DDV_MinMaxInt(pDX, m_iMaxWidth, m_iMinWidth, 5000);

	DDX_Text(pDX, IDC_EDIT10, m_iMinHeight);
	DDV_MinMaxInt(pDX, m_iMinHeight, -10000, m_iMaxHeight);

	DDX_Text(pDX, IDC_EDIT11, m_iMaxHeight);
	DDV_MinMaxInt(pDX, m_iMaxHeight, m_iMinHeight, 5000);

	DDX_Text(pDX, IDC_EDIT12, m_fInitSpeed.x);
	DDV_MinMaxFloat(pDX, m_fInitSpeed.x, 0.0, 100.0);

	DDX_Text(pDX, IDC_EDIT13, m_fInitSpeed.y);
	DDV_MinMaxFloat(pDX, m_fInitSpeed.y, 0.0, 100.0);

	DDX_Text(pDX, IDC_EDIT14, m_fInitSpeed.z);
	DDV_MinMaxFloat(pDX, m_fInitSpeed.z, 0.0, 100.0);

	DDX_Text(pDX, IDC_EDIT15, m_v3InitMinPos.x);
	DDV_MinMaxFloat(pDX, m_v3InitMinPos.x, -10000.0, m_v3InitMaxPos.x);
	DDX_Text(pDX, IDC_EDIT16, m_v3InitMinPos.y);
	DDV_MinMaxFloat(pDX,m_v3InitMinPos.y, 0.0, m_v3InitMaxPos.y);
	DDX_Text(pDX, IDC_EDIT17, m_v3InitMinPos.z);
	DDV_MinMaxFloat(pDX, m_v3InitMinPos.z, -10000.0, m_v3InitMaxPos.z);

	DDX_Text(pDX, IDC_EDIT18, m_v3InitMaxPos.x);
	DDV_MinMaxFloat(pDX, m_v3InitMaxPos.x, m_v3InitMinPos.x, 20000.0);
	DDX_Text(pDX, IDC_EDIT20, m_v3InitMaxPos.y);
	DDV_MinMaxFloat(pDX,m_v3InitMaxPos.y, m_v3InitMinPos.y, 20000.0);
	DDX_Text(pDX, IDC_EDIT23, m_v3InitMaxPos.z);
	DDV_MinMaxFloat(pDX, m_v3InitMaxPos.z, m_v3InitMinPos.z, 20000.0);

	DDX_Text(pDX, IDC_EDIT24, m_fAlphaBase);
	DDV_MinMaxFloat(pDX, m_fAlphaBase, 1.0, 1000.0);
	
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(KSceneModelEditorSFXSceneSFXDialog, CDialog)
	//{{AFX_MSG_MAP(KSceneModelEditorSFXSceneSFXDialog)
	ON_BN_CLICKED(IDC_BUTTON_SETMAXPARTCLE, OnButtonSetmaxpartcle)
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BUTTON1, SetAccSpeed)
	ON_EN_CHANGE(IDC_EDIT2, OnEnChangeEdit2)
	ON_BN_CLICKED(IDC_BUTTON2, OnSetWichBB)
//	ON_BN_CLICKED(IDC_BUTTON3, SetXAccSpeed)
//	ON_BN_CLICKED(IDC_BUTTON9, SetBBMaxWidth)
	ON_BN_CLICKED(IDC_BUTTON7, SetBBWidth)
	ON_BN_CLICKED(IDC_BUTTON11, SetBBHeight)
//	ON_EN_CHANGE(IDC_EDIT5, SetBBMinWidth)
//	ON_EN_CHANGE(IDC_EDIT9, SetBBMaxWidth)
//	ON_BN_CLICKED(IDC_BUTTON11, SetBBMinHeight)
//	ON_BN_CLICKED(IDC_BUTTON13, SetBBMaxHeight)
//	ON_EN_CHANGE(IDC_EDIT9, OnEnChangeEdit9)
ON_BN_CLICKED(IDC_BUTTON9, SetInitSpeed)
//ON_EN_CHANGE(IDC_EDIT20, OnEnChangeEdit20)
ON_BN_CLICKED(IDC_BUTTON13, SetInitMinPos)
ON_BN_CLICKED(IDC_BUTTON14, SetInitMaxPos)
ON_BN_CLICKED(IDC_BUTTON15, LoadSSFXFile)
ON_BN_CLICKED(IDC_BUTTON16, SaveSSFXFile)
ON_EN_CHANGE(IDC_EDIT24, SetAlphaBase)
ON_BN_CLICKED(IDC_BUTTON17, SetAlphaBase)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// KSceneModelEditorSFXSceneSFXDialog message handlers

void KSceneModelEditorSFXSceneSFXDialog::OnButtonSetmaxpartcle() 
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
	//if(m_lpSceneSFX)
	//{
	//	m_lpSceneSFX->SetMaxParticle(m_dwMaxParticle);
	//}
	
}

void KSceneModelEditorSFXSceneSFXDialog::SetAccSpeed()
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
//	if(m_lpSceneSFX)
//	{
//		m_lpSceneSFX->SetAccSpeed(D3DXVECTOR3(m_fXAccSpeed,m_fYAccSpeed,m_fZAccSpeed));
////		m_lpSceneSFX->SetXAccSpeed(m_fXAccSpeed);
////		m_lpSceneSFX->SetZAccSpeed(m_fZAccSpeed);
//	}
}

void KSceneModelEditorSFXSceneSFXDialog::OnEnChangeEdit2()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
}




void KSceneModelEditorSFXSceneSFXDialog::OnSetWichBB()
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
	//if(m_lpSceneSFX)
	//{
	//	m_lpSceneSFX->SetWhichBB(m_iWhichBB);
	//}
}

void KSceneModelEditorSFXSceneSFXDialog::SetBBWidth()
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
	//if(m_lpSceneSFX)
	//{
	//	m_lpSceneSFX->SetWidth(m_iMinWidth,m_iMaxWidth);
	//}
}

void KSceneModelEditorSFXSceneSFXDialog::SetBBHeight()
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
	//if(m_lpSceneSFX)
	//{
	//	m_lpSceneSFX->SetHeight(m_iMinHeight,m_iMaxHeight);
	//}
}


void KSceneModelEditorSFXSceneSFXDialog::SetInitSpeed()
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
	//if(m_lpSceneSFX)
	//{
	//	m_lpSceneSFX->SetInitSpeed(m_fInitSpeed);
	//}
}


void KSceneModelEditorSFXSceneSFXDialog::SetInitMinPos()
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
	//if(m_lpSceneSFX)
	//{
	//	m_lpSceneSFX->SetInitMinPos(m_v3InitMinPos);
	//}
}

void KSceneModelEditorSFXSceneSFXDialog::SetInitMaxPos()
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
	/*if(m_lpSceneSFX)
	{
		m_lpSceneSFX->SetInitMaxPos(m_v3InitMaxPos);
	}*/
}

void KSceneModelEditorSFXSceneSFXDialog::LoadSSFXFile()
{
	// TODO: Add your control notification handler code here

	char bigBuff[800] = "";  
	char szFilter[] ="公告板文件 (*.ssfx)|*.ssfx|";

	CFileDialog dlg(false, "ssfx", NULL,
					OFN_HIDEREADONLY , szFilter);

	dlg.m_ofn.lpstrFile = bigBuff;
	dlg.m_ofn.nMaxFile = sizeof(bigBuff);

	/*if (	dlg.DoModal() == IDOK)
	{
		TCHAR path_buffer[256];
		TCHAR drive[256];
		TCHAR dir[256];
		TCHAR fname[256];
		TCHAR ext[256];

		CString Vs = dlg.GetPathName();

		wsprintf(path_buffer,"%s",Vs);
		_splitpath( path_buffer, drive, dir, fname, ext );


		TCHAR filename[256];
		wsprintf(filename,"%s%s%s.ssfx",drive,dir,fname);

		BBHeader bbheader;
		DWORD version;
		FILE *fp;

		fp = fopen(filename,"rb");

		fread(&version,sizeof(DWORD),1,fp);
		fread(&bbheader,sizeof(BBHeader),1,fp);

		fclose(fp);

		SetData(bbheader);

		UpdateData(false);
	}*/
}

void KSceneModelEditorSFXSceneSFXDialog::SaveSSFXFile()
{
	// TODO: Add your control notification handler code here

	char bigBuff[800] = "";  // maximum common dialog buffer size
	char szFilter[] ="公告板文件 (*.ssfx)|*.ssfx|";

	CFileDialog dlg(false, "ssfx", NULL,
		OFN_HIDEREADONLY , szFilter);
	// Modify OPENFILENAME members directly to point to bigBuff
	dlg.m_ofn.lpstrFile = bigBuff;
	dlg.m_ofn.nMaxFile = sizeof(bigBuff);
	/*if (	dlg.DoModal() == IDOK)
	{
		TCHAR path_buffer[256];
		TCHAR drive[256];
		TCHAR dir[256];
		TCHAR fname[256];
		TCHAR ext[256];

		CString Vs = dlg.GetPathName();

		wsprintf(path_buffer,"%s",Vs);
		_splitpath( path_buffer, drive, dir, fname, ext );


		TCHAR filename[256];
		wsprintf(filename,"%s%s%s.ssfx",drive,dir,fname);

		BBHeader bbheader;
		DWORD version;
		FILE *fp;

		InPutdata(bbheader);

		fp = fopen(filename,"wb");

		fwrite(&version,sizeof(DWORD),1,fp);
		fwrite(&bbheader,sizeof(BBHeader),1,fp);

		fclose(fp);
	}*/
}

void KSceneModelEditorSFXSceneSFXDialog::SetAlphaBase()
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
	/*if(m_lpSceneSFX)
	{
		m_lpSceneSFX->SetAlphaBase(m_fAlphaBase);
	}*/
}
