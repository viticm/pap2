// KMovieOptionDialog.cpp : implementation file
//

#include "stdafx.h"
#include "RecordeOperator.h"
#include "KMovieOptionDialog.h"


// KMovieOptionDialog dialog

IMPLEMENT_DYNAMIC(KMovieOptionDialog, CDialog)

KMovieOptionDialog::KMovieOptionDialog(CWnd* pParent /*=NULL*/)
	: CDialog(KMovieOptionDialog::IDD, pParent)
{

}

KMovieOptionDialog::~KMovieOptionDialog()
{
}

BOOL KMovieOptionDialog::OnInitDialog()
{	
	int nResult = CDialog::OnInitDialog();
	char szText[128];
	RecordOption *pOption = g_p3DEngine->GetRecordOption();
	_itoa(pOption->Size.cx, szText, 10);
	GetDlgItem(IDC_EDIT_W)->SetWindowText(szText);
	_itoa(pOption->Size.cy, szText, 10);
	GetDlgItem(IDC_EDIT_H)->SetWindowText(szText);
	gcvt(pOption->fInterval,5,szText);
	GetDlgItem(IDC_EDIT_I)->SetWindowText(szText);

	m_ImageType.AddString(TEXT("jpg"));
	m_ImageType.AddString(TEXT("bmp"));
	m_ImageType.AddString(TEXT("png"));
	m_ImageType.AddString(TEXT("dds"));
	m_ImageType.AddString(TEXT("tga"));

	m_ImageType.SelectString(0, TEXT("jpg"));
	switch (pOption->ImageType)
	{
	case D3DXIFF_JPG :
		m_ImageType.SelectString(0, TEXT("jpg"));
		break;
	case D3DXIFF_BMP :
		m_ImageType.SelectString(0, TEXT("bmp"));
		break;
	case D3DXIFF_PNG :
		m_ImageType.SelectString(0, TEXT("png"));
		break;
	case D3DXIFF_DDS :
		m_ImageType.SelectString(0, TEXT("dds"));
		break;
	case D3DXIFF_TGA :
		m_ImageType.SelectString(0, TEXT("tga"));
		break;
	default :
		break;
	}

	m_FiterType.AddString(TEXT("point"));
	m_FiterType.AddString(TEXT("linear"));
	m_FiterType.SelectString(0, TEXT("point"));
	switch (pOption->FiterType)
	{
	case D3DTEXF_POINT :
		m_FiterType.SelectString(0, TEXT("point"));
		break;
	case D3DTEXF_LINEAR :
		m_FiterType.SelectString(0, TEXT("linear"));
		break;
	case D3DTEXF_ANISOTROPIC :
		break;
	case D3DTEXF_GAUSSIANQUAD :
		break;
	default :
		break;
	}
	((CButton*)GetDlgItem(IDC_CHECK_AUTOSIZE))->SetCheck(pOption->nAutoScale);
	((CButton*)GetDlgItem(IDC_CHECK_KEEP))->SetCheck(pOption->bKeep);
	return nResult;
}

void KMovieOptionDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_IMAGE, m_ImageType);
	DDX_Control(pDX, IDC_COMBO_FITER, m_FiterType);
}


BEGIN_MESSAGE_MAP(KMovieOptionDialog, CDialog)
	ON_BN_CLICKED(IDC_OK, &KMovieOptionDialog::OnBnClickedOk)
	ON_BN_CLICKED(IDC_CANCEL, &KMovieOptionDialog::OnBnClickedCancel)
END_MESSAGE_MAP()


// KMovieOptionDialog message handlers

void KMovieOptionDialog::OnBnClickedOk()
{
	CString strText;
	SIZE  Size;
	float fInterval;
	INT   nAutoScal;
	BOOL  bKeep;

	GetDlgItem(IDC_EDIT_W)->GetWindowText(strText);
	Size.cx = atoi(strText.GetBuffer());
	GetDlgItem(IDC_EDIT_H)->GetWindowText(strText);
	Size.cy = atoi(strText.GetBuffer());


	if (((CButton*)GetDlgItem(IDC_CHECK_AUTOSIZE))->GetCheck())
		nAutoScal = TRUE;
	else
		nAutoScal = FALSE;

	if (((CButton*)GetDlgItem(IDC_CHECK_KEEP))->GetCheck())
		bKeep = TRUE;
	else
		bKeep = FALSE;


	if (Size.cx < 10 || Size.cy < 10)
	{
		AfxMessageBox(TEXT("格式有错!!"));
		return;
	}

	GetDlgItem(IDC_EDIT_I)->GetWindowText(strText);
	fInterval = (float)atof(strText.GetBuffer());

	if (fInterval < 0)
	{
		AfxMessageBox(TEXT("帧间隔有错!!"));
		return;
	}

	D3DTEXTUREFILTERTYPE fiterType = D3DTEXF_LINEAR;
	switch (m_FiterType.GetCurSel())
	{
	case 0 :
		fiterType = D3DTEXF_POINT;
		break;
	case 1 :
		fiterType = D3DTEXF_LINEAR;
		break;
	case 2 :
		fiterType = D3DTEXF_ANISOTROPIC;
		break;
	case 3 :
		fiterType = D3DTEXF_GAUSSIANQUAD;
		break;
	default :
		fiterType = D3DTEXF_LINEAR;
		break;
	}

	D3DXIMAGE_FILEFORMAT imageType = D3DXIFF_JPG;
	switch (m_ImageType.GetCurSel())
	{
	case 0 :
		imageType = D3DXIFF_JPG;
		break;
	case 1 :
		imageType = D3DXIFF_BMP;
		break;
	case 2 :
		imageType = D3DXIFF_PNG;
		break;
	case 3 :
		imageType = D3DXIFF_DDS;
		break;
	case 4 :
		imageType = D3DXIFF_TGA;
		break;
	default :
		imageType = D3DXIFF_JPG;
		break;
	}

	RecordOption recordOption;
	recordOption.Size = Size;
	recordOption.fInterval = fInterval;
	recordOption.FiterType = fiterType;
	recordOption.ImageType = imageType;
	recordOption.nAutoScale = nAutoScal;
	recordOption.bKeep = bKeep;
	g_p3DEngine->SetRecordOption(&recordOption);
	OnOK();
}

void KMovieOptionDialog::OnBnClickedCancel()
{
	OnCancel();
}
