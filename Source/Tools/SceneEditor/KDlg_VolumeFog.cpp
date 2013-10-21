// KDlg_VolumeFog.cpp : implementation file
//

#include "stdafx.h"
#include "SceneEditor.h"
//#include "KVolumeFog.h"
#include "KDlg_VolumeFog.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

////////////////////////////////////////////////////////////////////////////////



// KDlg_VolumeFog dialog

IMPLEMENT_DYNAMIC(KDlg_VolumeFog, CPropertyPage)
KDlg_VolumeFog::KDlg_VolumeFog()
	: CPropertyPage(KDlg_VolumeFog::IDD)
{
}

KDlg_VolumeFog::~KDlg_VolumeFog()
{
}

void KDlg_VolumeFog::Create()
{
	CPropertyPage::Create(IDD);
}

void KDlg_VolumeFog::Show()
{
	ShowWindow(true);
}

//void KDlg_VolumeFog::SetVolumeFog(KVolumeFog* pVF)
//{
//	m_pVF = pVF;
//
//	// Backup the property
//	m_Filename = m_pVF->m_FilePath + m_pVF->m_Filename;
//	m_Color = m_pVF->m_Color;
//	m_XSpan = m_pVF->m_HalfXSpan * 2;
//	m_ZSpan = m_pVF->m_HalfZSpan * 2;
//	m_ViewOffset = m_pVF->m_ViewOffset;
//	m_DistFP = m_pVF->m_DistBetweenFPs;
//
//	// Display
//	SetDlgItemText(IDD_VOLUMEFOG_FILENAME, m_Filename.c_str());
//	string sTemp;
//	DecToHex(m_Color & 0x00FFFFFF, sTemp);
//	SetDlgItemText(IDD_VOLUMEFOG_COLOR, sTemp.c_str());
//	sTemp = (int)((m_Color & 0xFF000000) >> 24);
//	SetDlgItemText(IDD_VOLUMEFOG_DENSITY, sTemp.c_str());
//
//	TCHAR strTemp[255];
//	sprintf(strTemp, "%f", m_XSpan);
//	SetDlgItemText(IDD_VOLUMEFOG_XSPAN, strTemp);
//	//sTemp = m_ZSpan;
//	sprintf(strTemp, "%f", m_ZSpan);
//	SetDlgItemText(IDD_VOLUMEFOG_ZSPAN, strTemp);
//
//	//sTemp = m_ViewOffset;
//	sprintf(strTemp, "%f",m_ViewOffset);
//	SetDlgItemText(IDD_VOLUMEFOG_VIEWOFFSET, strTemp);
//	//sTemp = m_DistFP;
//	sprintf(strTemp, "%f", m_DistFP);
//	SetDlgItemText(IDD_VOLUMEFOG_DISTFP, strTemp);
//}

void KDlg_VolumeFog::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(KDlg_VolumeFog, CPropertyPage)
	ON_BN_CLICKED(IDCANCEL, OnCancelClicked)
	ON_BN_CLICKED(IDD_VOLUMEFOG_RESET, OnResetClicked)
	ON_BN_CLICKED(IDOK, OnOkClicked)
	ON_EN_CHANGE(IDD_VOLUMEFOG_COLOR, OnColorChanged)
	ON_BN_CLICKED(IDD_VOLUMEFOG_COLORTABLE, OnColorTableClicked)
	ON_EN_CHANGE(IDD_VOLUMEFOG_DENSITY, OnDensityChanged)
	ON_EN_CHANGE(IDD_VOLUMEFOG_XSPAN, OnXSpanChanged)
	ON_EN_CHANGE(IDD_VOLUMEFOG_ZSPAN, OnZSpanChanged)
	ON_EN_CHANGE(IDD_VOLUMEFOG_VIEWOFFSET, OnViewOffsetChanged)
	ON_EN_CHANGE(IDD_VOLUMEFOG_DISTFP, OnDistFPChanged)
	ON_BN_CLICKED(IDD_VOLUMEFOG_SAVE, OnSaveClicked)
END_MESSAGE_MAP()


// KDlg_VolumeFog message handlers

void KDlg_VolumeFog::OnClose()
{
	CPropertyPage::OnClose();

	ShowWindow(false);
}

void KDlg_VolumeFog::OnCancelClicked()
{
	//m_pVF->SetHalfSpanX(m_XSpan * 0.5f);
	//m_pVF->SetHalfSpanZ(m_ZSpan * 0.5f);
	//m_pVF->SetDiffuse(m_Color);
	//m_pVF->SetViewOffset(m_ViewOffset);
	//m_pVF->SetDistBetweenFPs(m_DistFP);

	OnClose();
}

void KDlg_VolumeFog::OnResetClicked()
{
	//m_pVF->SetHalfSpanX(m_XSpan * 0.5f);
	//m_pVF->SetHalfSpanZ(m_ZSpan * 0.5f);
	//m_pVF->SetDiffuse(m_Color);
	//m_pVF->SetViewOffset(m_ViewOffset);
	//m_pVF->SetDistBetweenFPs(m_DistFP);

	//ShowWindow(false);
	//SetVolumeFog(m_pVF);
	//ShowWindow(true);
}

void KDlg_VolumeFog::OnOkClicked()
{
	OnClose();
}

void KDlg_VolumeFog::OnColorChanged()
{
	/*char szText[256];
	GetDlgItemText(IDD_VOLUMEFOG_COLOR, szText, 256);

	string sTemp = szText;
	unsigned int color = HexToDec(sTemp);

	GetDlgItemText(IDD_VOLUMEFOG_DENSITY, szText, 256);
	sTemp = szText;

    color &= 0x00FFFFFF;
	color |= (static_cast<int>(strtod(sTemp.c_str(), 0)) << 24);

	m_pVF->SetDiffuse(color);*/
}

void KDlg_VolumeFog::OnColorTableClicked()
{
	/*CColorDialog dlg;
	if(dlg.DoModal() != IDOK)
	{
		return;
	}

	COLORREF cr = dlg.GetColor();
	unsigned char r, g, b;
	r = GetRValue(cr);
	g = GetGValue(cr);
	b = GetBValue(cr);
	unsigned int color = 0;
	color = (r << 16) | (g << 8) | b;

	string sColor;
	DecToHex(color, sColor);
	SetDlgItemText(IDD_VOLUMEFOG_COLOR, (LPCTSTR)sColor.c_str());*/
}

void KDlg_VolumeFog::OnDensityChanged()
{
	//char szText[256];
	//GetDlgItemText(IDD_VOLUMEFOG_COLOR, szText, 256);

	//string sTemp = szText;
	//unsigned int color = HexToDec(sTemp);

	//GetDlgItemText(IDD_VOLUMEFOG_DENSITY, szText, 256);
	//sTemp = szText;

 //   color &= 0x00FFFFFF;
	//unsigned int alpha = static_cast<int>(strtod(szText, 0));//sTemp.IntValue();
	//alpha = (alpha > 255)? 255: (alpha < 0)? 0: alpha;
	//color |= (alpha << 24);

	//m_pVF->SetDiffuse(color);
}

void KDlg_VolumeFog::OnXSpanChanged()
{
	//char sz[256];
	//GetDlgItemText(IDD_VOLUMEFOG_XSPAN, sz, 256);
	////Common::String s = sz;
	////m_pVF->SetHalfSpanX(s.FloatValue() * 0.5f);
	//TCHAR* pEnd = NULL;
	//m_pVF->SetHalfSpanX(static_cast<float>(strtod(sz, &pEnd)) * 0.5f);
}

void KDlg_VolumeFog::OnZSpanChanged()
{
	//char sz[256];
	//GetDlgItemText(IDD_VOLUMEFOG_ZSPAN, sz, 256);
	////Common::String s = sz;
	//TCHAR* pEnd = NULL;
	//m_pVF->SetHalfSpanZ(static_cast<float>(strtod(sz, &pEnd)) * 0.5f);
}

void KDlg_VolumeFog::OnViewOffsetChanged()
{
	//char sz[256];
	//GetDlgItemText(IDD_VOLUMEFOG_VIEWOFFSET, sz, 256);
	////Common::String s = sz;
	//m_pVF->SetViewOffset(static_cast<float>(strtod(sz, 0)));//s.FloatValue());
}

void KDlg_VolumeFog::OnDistFPChanged()
{
	//char sz[256];
	//GetDlgItemText(IDD_VOLUMEFOG_DISTFP, sz, 256);
	////Common::String s = sz;
	//m_pVF->SetDistBetweenFPs(static_cast<float>(strtod(sz, 0)));//s.FloatValue());
}

void KDlg_VolumeFog::OnSaveClicked()
{
	/*FILE* pFile = fopen(m_Filename.c_str(), "wb");
	if(pFile)
	{
		m_pVF->SaveMesh(pFile);
		fclose(pFile);
	}*/
}
