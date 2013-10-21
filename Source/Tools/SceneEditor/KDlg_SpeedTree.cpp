// KDlg_SpeedTree.cpp : implementation file
//

#include "stdafx.h"
#include "sceneeditor.h"
#include "KDlg_SpeedTree.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

////////////////////////////////////////////////////////////////////////////
// KDlg_SpeedTree dialog


KDlg_SpeedTree::KDlg_SpeedTree(CWnd* pParent /*=NULL*/)
	: CPropertyPage(KDlg_SpeedTree::IDD)
{
	//{{AFX_DATA_INIT(KDlg_SpeedTree)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void KDlg_SpeedTree::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(KDlg_SpeedTree)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}

void KDlg_SpeedTree::Create()
{
	CPropertyPage::Create(IDD);
}

//void TrimFloatString(string& sFloat, int PosNumAfterDot = 2)
//{
//	/*if(!sFloat.IfContain('.'))
//	{
//		return;
//	}*/
//	if(PosNumAfterDot <= 0)
//	{
//		sFloat = sFloat.substr(0, sFloat.find_first_of('.'));
//		return;
//	}
//
//	unsigned int NumOfPoses = sFloat.size();
//	unsigned int PosOfDot = sFloat.find_first_of('.');
//	int temp = NumOfPoses - PosOfDot - 1;
//	if(temp <= PosNumAfterDot)
//	{
//		return;
//	}
//
//	sFloat = sFloat.substr(0, PosOfDot + PosNumAfterDot + 1);
//}

//void KDlg_SpeedTree::SetSpeedTree(KSpeedTree* pST)
//{
//	m_pST = pST;
//
//	// Backup the states
//	m_Swing = pST->Pro_AngleSwing;
//	m_MinVel = pST->Pro_MinVelocity;
//	m_MaxVel = pST->Pro_MaxVelocity;
//	m_MinWidth = pST->Pro_MinWidth;
//	m_MaxWidth = pST->Pro_MaxWidth;
//	m_MinHeight = pST->Pro_MinHeight;
//	m_MaxHeight = pST->Pro_MaxHeight;
//	m_SegNum = pST->Pro_Segment;
//	m_Lx = pST->m_LightDir.x;
//	m_Ly = pST->m_LightDir.y;
//	m_Lz = pST->m_LightDir.z;
//	m_LightColor = D3DCOLOR_XRGB((int)(pST->m_LightColor.x * 255), (int)(pST->m_LightColor.y * 255), (int)(pST->m_LightColor.z * 255));
//	m_AmbientColor = D3DCOLOR_XRGB((int)(pST->m_AmbientColor.x * 255), (int)(pST->m_AmbientColor.y * 255), (int)(pST->m_AmbientColor.z * 255));
//	m_TexFilename = pST->m_Texture.m_TexName;
//	m_pTex = pST->m_Texture.m_pTexture;
//	m_bNeedReleaseTex = false;
//
//	m_bReady = false;
//
//	// Now show these states on the window
//	//string sSwing = m_Swing;
//	//string sMinVel = m_MinVel * 100;
//	//string sMaxVel = m_MaxVel * 100;
//	//string sMinWidth = m_MinWidth;
//	//string sMaxWidth = m_MaxWidth;
//	//string sMinHeight = m_MinHeight;
//	//string sMaxHeight = m_MaxHeight;
//	//string sLx = m_Lx;
//	//string sLy = m_Ly;
//	//string sLz = m_Lz;
//
//	//TrimFloatString(sSwing, 0);
//	//TrimFloatString(sMinVel, 0);
//	//TrimFloatString(sMaxVel, 0);
//	//TrimFloatString(sMinWidth, 1);
//	//TrimFloatString(sMaxWidth, 0);
//	//TrimFloatString(sMinHeight, 1);
//	//TrimFloatString(sMaxHeight, 0);
//	//TrimFloatString(sLx, 1);
//	//TrimFloatString(sLy, 1);
//	//TrimFloatString(sLz, 1);
//
//	TCHAR strTemp[MAX_PATH];
//	sprintf(strTemp, "%f", m_Swing);
//	SetDlgItemText(IDD_ST_SWING, strTemp);
//	sprintf(strTemp, "%f", m_MinVel);
//	SetDlgItemText(IDD_ST_MINVEL, strTemp);
//	sprintf(strTemp, "%f", m_MaxVel);
//	SetDlgItemText(IDD_ST_MAXVEL, strTemp);
//	//SetDlgItemText(IDD_ST_MINWIDTH, sMinWidth);
//	sprintf(strTemp, "%f", m_MaxWidth);
//	SetDlgItemText(IDD_ST_MAXWIDTH, strTemp);
//	//SetDlgItemText(IDD_ST_MINHEIGHT, sMinHeight);
//	sprintf(strTemp, "%f", m_MaxHeight);
//	SetDlgItemText(IDD_ST_MAXHEIGHT, strTemp);
//	sprintf(strTemp, "%d", m_SegNum);
//	SetDlgItemText(IDD_ST_SEGNUM, strTemp);
//	sprintf(strTemp, "%f", m_Lx);
//	SetDlgItemText(IDD_ST_LX, strTemp);
//	sprintf(strTemp, "%f", m_Ly);
//	SetDlgItemText(IDD_ST_LY, strTemp);
//	sprintf(strTemp, "%f", m_Lz);
//	SetDlgItemText(IDD_ST_LZ, strTemp);
//	string temp;
//	DecToHex(m_LightColor, temp);
//	SetDlgItemText(IDD_ST_LIGHTCOLOR, temp.c_str());
//	DecToHex(m_AmbientColor, temp);
//	SetDlgItemText(IDD_ST_AMBIENTCOLOR, temp.c_str());
//	//SetDlgItemText(IDD_ST_SWING, sSwing);
//	//SetDlgItemText(IDD_ST_MINVEL, sMinVel);
//	//SetDlgItemText(IDD_ST_MAXVEL, sMaxVel);
//	////SetDlgItemText(IDD_ST_MINWIDTH, sMinWidth);
//	//SetDlgItemText(IDD_ST_MAXWIDTH, sMaxWidth);
//	////SetDlgItemText(IDD_ST_MINHEIGHT, sMinHeight);
//	//SetDlgItemText(IDD_ST_MAXHEIGHT, sMaxHeight);
//	//SetDlgItemText(IDD_ST_SEGNUM, string(m_SegNum));
//	//SetDlgItemText(IDD_ST_LX, sLx);
//	//SetDlgItemText(IDD_ST_LY, sLy);
//	//SetDlgItemText(IDD_ST_LZ, sLz);
//	//string temp;
//	//DecToHex(m_LightColor, temp);
//	//SetDlgItemText(IDD_ST_LIGHTCOLOR, temp);
//	//DecToHex(m_AmbientColor, temp);
//	//SetDlgItemText(IDD_ST_AMBIENTCOLOR, temp);
//	if(!m_TexFilename.empty())
//	{
//		m_TexFile.clear();
//		SetDlgItemText(IDD_ST_TEXTUREFILENAME, m_TexFilename.c_str());
//	}
//
//	m_AlphaFactor = m_pST->m_AlphaFactor;
//
//	CSliderCtrl* pSlider = static_cast<CSliderCtrl*>(GetDlgItem(IDD_ST_ALPHASLIDER));
//	if(pSlider)
//	{
//		pSlider->SetRange(0, 99);
//		int pos = static_cast<int>(m_AlphaFactor * 50 + 49);
//		pos = (pos < 0)? 0: (pos > 99)? 99: pos;
//		pSlider->SetPos(pos);
//	}
//
//	m_bReady = true;
//	LightingChanged();
//}

void KDlg_SpeedTree::Apply()
{
	if(m_bNeedReleaseTex)
	{
		SAFE_RELEASE(m_pTex);
	}
}

void KDlg_SpeedTree::Show()
{
	ShowWindow(1);
}


BEGIN_MESSAGE_MAP(KDlg_SpeedTree, CPropertyPage)
	//{{AFX_MSG_MAP(KDlg_SpeedTree)
		// NOTE: the ClassWizard will add message map macros here
		ON_BN_CLICKED(IDOK, OnOKClicked)
		ON_BN_CLICKED(IDCANCEL, OnCancelClicked)
		ON_BN_CLICKED(IDD_ST_SAVE, OnSaveClicked)

		ON_EN_CHANGE(IDD_ST_SWING, OnSwingChanged)
		ON_EN_CHANGE(IDD_ST_MINVEL, OnMinVelChanged)
		ON_EN_CHANGE(IDD_ST_MAXVEL, OnMaxVelChanged)
		//ON_EN_CHANGE(IDD_ST_MINWIDTH, OnMinWidthChanged)
		ON_EN_CHANGE(IDD_ST_MAXWIDTH, OnMaxWidthChanged)
		//ON_EN_CHANGE(IDD_ST_MINHEIGHT, OnMinHeightChanged)
		ON_EN_CHANGE(IDD_ST_MAXHEIGHT, OnMaxHeightChanged)
		ON_EN_CHANGE(IDD_ST_SEGNUM, OnSegNumChanged)
		ON_EN_CHANGE(IDD_ST_LX, OnLXChanged)
		ON_EN_CHANGE(IDD_ST_LY, OnLYChanged)
		ON_EN_CHANGE(IDD_ST_LZ, OnLZChanged)
		ON_EN_CHANGE(IDD_ST_LIGHTCOLOR, OnLightColorChanged)
		ON_EN_CHANGE(IDD_ST_AMBIENTCOLOR, OnAmbientColorChanged)
		ON_BN_CLICKED(IDD_ST_LIGHTCOLORTABLE, OnLightColorTableClicked)
		ON_BN_CLICKED(IDD_ST_AMBIENTCOLORTABLE, OnAmbientColorTableClicked)
		ON_EN_CHANGE(IDD_ST_TEXTUREFILENAME, OnTexFilenameChanged)
		ON_BN_CLICKED(IDD_ST_TEXTUREFILENAMEBUTTON, OnTexFilenameButtonClicked)
	//}}AFX_MSG_MAP
	ON_WM_HSCROLL()
	ON_BN_CLICKED(IDD_ST_RESET, OnResetClicked)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// KDlg_SpeedTree message handlers

void KDlg_SpeedTree::OnClose()
{
	CPropertyPage::OnClose();

	ShowWindow(false);
}

void KDlg_SpeedTree::OnOKClicked()
{
	Apply();

	OnClose();
}

void KDlg_SpeedTree::OnCancelClicked()
{
	//m_pST->Pro_AngleSwing = m_Swing;
	//m_pST->Pro_MinVelocity = m_MinVel;
	//m_pST->Pro_MaxVelocity = m_MaxVel;
	//m_pST->Pro_MinWidth = m_MinWidth;
	//m_pST->Pro_MaxWidth = m_MaxWidth;
	//m_pST->Pro_MinHeight = m_MinHeight;
	//m_pST->Pro_MaxHeight = m_MaxHeight;
	//m_pST->Pro_Segment = m_SegNum;
	//m_pST->m_LightDir.x = m_Lx;
	//m_pST->m_LightDir.y = m_Ly;
	//m_pST->m_LightDir.z = m_Lz;
	//m_pST->m_Texture.m_pTexture = m_pTex;
	//m_pST->m_Texture.m_TexName = m_TexFilename;

	//m_pST->UpdateParameters();

	//float lr, lg, lb, ar, ag, ab;
	//lr = (float)((m_LightColor & 0x00FF0000) >> 16) / 255;
	//lg = (float)((m_LightColor & 0x0000FF00) >> 8) / 255;
	//lb = (float)(m_LightColor & 0x000000FF) / 255;
	//ar = (float)((m_AmbientColor & 0x00FF0000) >> 16) / 255;
	//ag = (float)((m_AmbientColor & 0x0000FF00) >> 8) / 255;
	//ab = (float)(m_AmbientColor & 0x000000FF) / 255;

	//m_pST->m_LightColor.x = lr;
	//m_pST->m_LightColor.y = lg;
	//m_pST->m_LightColor.z = lb;
	//m_pST->m_AmbientColor.x = ar;
	//m_pST->m_AmbientColor.y = ag;
	//m_pST->m_AmbientColor.z = ab;

	//m_pST->m_AlphaFactor = m_AlphaFactor;

	//m_pST->SetLight(m_Lx, m_Ly, m_Lz, lr, lg, lb, ar, ag, ab);

	//OnClose();
}

void KDlg_SpeedTree::OnSaveClicked()
{
	Apply();

	//CFileDialog dlg(false, "stf", 0, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, "SpeedTree文件 (*.stf)|*.stf\0\0");
	//if(dlg.DoModal() != IDOK)
	//{
	//	return;
	//}

	//char szDir[256];
	//GetCurrentDirectory(256, szDir);

	//m_pST->SaveMesh(0);
}




void KDlg_SpeedTree::OnSwingChanged()
{
	//char szText[256];
	//memset(szText, 0, sizeof(szText));

	//GetDlgItemText(IDD_ST_SWING, szText, sizeof(szText));

	////string sText = szText;
	//TCHAR* pEnd = NULL;
	//m_pST->Pro_AngleSwing = static_cast<float>(strtod(szText, &pEnd));
	//m_pST->Pro_AngleSwing = (m_pST->Pro_AngleSwing < 0)? -m_pST->Pro_AngleSwing: m_pST->Pro_AngleSwing;

	//m_pST->UpdateParameters();
}

void KDlg_SpeedTree::OnMinVelChanged()
{
	//char szText[256];
	//memset(szText, 0, sizeof(szText));

	//GetDlgItemText(IDD_ST_MINVEL, szText, sizeof(szText));

	////string sText = szText;
	//TCHAR* pEnd = NULL;
	////float temp = sText.FloatValue() * 0.01f;
	//float temp = static_cast<float>(strtod(szText, &pEnd)) * 0.01f;
	//if(temp >= m_pST->Pro_MaxVelocity)
	//{
	//	string szMsg = "请键入一个比最大转速";
	//	TCHAR strVel[255];
	//	sprintf(strVel, "%f", m_pST->Pro_MaxVelocity * 100.0f );
	//	szMsg += string(strVel);
	//	szMsg += "小的值";
	//	MessageBox(szMsg.c_str(), "最小转速不合要求", MB_OK);
	//	sprintf(strVel, "%f", m_pST->Pro_MinVelocity * 100.0f );
	//	//szMsg = string(strVel);//m_pST->Pro_MinVelocity * 100);
	//	//TrimFloatString(szMsg, 0);
	//	SetDlgItemText(IDD_ST_MINVEL, strVel);//szMsg);
	//	return;
	//}

	//m_pST->Pro_MinVelocity = temp;
	//m_pST->UpdateParameters();
}

void KDlg_SpeedTree::OnMaxVelChanged()
{
	//char szText[256];
	//memset(szText, 0, sizeof(szText));

	//GetDlgItemText(IDD_ST_MAXVEL, szText, sizeof(szText));

	////string sText = szText;
	//TCHAR* pEnd = NULL;
	////float temp = sText.FloatValue() * 0.01f;
	//float temp = static_cast<float>(strtod(szText, &pEnd)) * 0.01f;
	//if(temp <= m_pST->Pro_MinVelocity)
	//{
	//	string szMsg = "请键入一个比最小转速";
	//	TCHAR strVel[255];
	//	sprintf(strVel, "%f", m_pST->Pro_MinVelocity * 100.0f );
	//	szMsg += string(strVel);//m_pST->Pro_MinVelocity * 100);
	//	szMsg += "大的值";
	//	MessageBox(szMsg.c_str(), "最大转速不合要求", MB_OK);
	//	sprintf(strVel, "%f", m_pST->Pro_MaxVelocity * 100.0f );
	//	//szMsg = string(strVel);//m_pST->Pro_MaxVelocity * 100);
	//	//TrimFloatString(szMsg, 0);
	//	SetDlgItemText(IDD_ST_MAXVEL, strVel);//szMsg);
	//	return;
	//}

	//m_pST->Pro_MaxVelocity = temp;
	//m_pST->UpdateParameters();
}

//void KDlg_SpeedTree::OnMinWidthChanged()
//{
//	char szText[256];
//	memset(szText, 0, sizeof(szText));
//
//	GetDlgItemText(IDD_ST_MINWIDTH, szText, sizeof(szText));
//
//	string sText = szText;
//	m_pST->Pro_MinWidth = sText.FloatValue();
//
//	m_pST->UpdateParameters();
//}

void KDlg_SpeedTree::OnMaxWidthChanged()
{
	//char szText[256];
	////string sText;

	//GetDlgItemText(IDD_ST_MAXWIDTH, szText, sizeof(szText));
	////sText = szText;
	////float mw = sText.FloatValue();
	//TCHAR* pEnd = NULL;
	//float mw = static_cast<float>(strtod(szText, &pEnd));

	//GetDlgItemText(IDD_ST_MAXHEIGHT, szText, sizeof(szText));
	////sText = szText;
	////float mh = sText.FloatValue();
	//float mh = static_cast<float>(strtod(szText, &pEnd));

	//if(mw < mh)
	//{
	//	m_pST->Pro_MaxWidth = mw;
	//	m_pST->Pro_MaxHeight = mh;
	//	m_pST->UpdateParameters();
	//}
}

//void KDlg_SpeedTree::OnMinHeightChanged()
//{
//	char szText[256];
//	memset(szText, 0, sizeof(szText));
//
//	GetDlgItemText(IDD_ST_MINHEIGHT, szText, sizeof(szText));
//
//	string sText = szText;
//	m_pST->Pro_MinHeight = sText.FloatValue();
//
//	m_pST->UpdateParameters();
//}

void KDlg_SpeedTree::OnMaxHeightChanged()
{
	//char szText[256];
	////string sText;

	//GetDlgItemText(IDD_ST_MAXWIDTH, szText, sizeof(szText));
	////sText = szText;
	////float mw = sText.FloatValue();
	//TCHAR* pEnd = NULL;
	//float mw = static_cast<float>(strtod(szText, &pEnd));

	//GetDlgItemText(IDD_ST_MAXHEIGHT, szText, sizeof(szText));
	////sText = szText;
	////float mh = sText.FloatValue();
	//float mh = static_cast<float>(strtod(szText, &pEnd));

	//if(mw < mh)
	//{
	//	m_pST->Pro_MaxWidth = mw;
	//	m_pST->Pro_MaxHeight = mh;
	//	m_pST->UpdateParameters();
	//}
}

void KDlg_SpeedTree::OnSegNumChanged()
{
	//char szText[256];
	//memset(szText, 0, sizeof(szText));

	//GetDlgItemText(IDD_ST_SEGNUM, szText, sizeof(szText));

	////string sText = szText;
	////m_pST->Pro_Segment = sText.FloatValue();
	//TCHAR* pEnd = NULL;
	//m_pST->Pro_Segment = static_cast<int>(strtod(szText, &pEnd));

	//m_pST->UpdateParameters();
}

void KDlg_SpeedTree::LightingChanged()
{
	//if(!m_bReady)
	//{
	//	return;
	//}

	//D3DXVECTOR3 LightDir;
	//D3DCOLOR LightColor, AmbientColor;

	//char szText[256];
	////string sText;

	//GetDlgItemText(IDD_ST_LX, szText, sizeof(szText));
	////sText = szText;
 //   //LightDir.x = sText.FloatValue();
	//TCHAR* pEnd = NULL;
	//LightDir.x = static_cast<float>(strtod(szText, &pEnd));

	//GetDlgItemText(IDD_ST_LY, szText, sizeof(szText));
	////sText = szText;
	////LightDir.y = sText.FloatValue();
	//LightDir.y = static_cast<float>(strtod(szText, &pEnd));
	//GetDlgItemText(IDD_ST_LZ, szText, sizeof(szText));
	////sText = szText;
	////LightDir.z = sText.FloatValue();
	//LightDir.z = static_cast<float>(strtod(szText, &pEnd));
	////LightDir.Normalize();
	//D3DXVec3Normalize(&LightDir, &LightDir);

	//GetDlgItemText(IDD_ST_LIGHTCOLOR, szText, sizeof(szText));
	//string sText = szText;
	//LightColor = HexToDec(sText);

	//GetDlgItemText(IDD_ST_AMBIENTCOLOR, szText, sizeof(szText));
	//sText = szText;
	//AmbientColor = HexToDec(sText);

	//float lr, lg, lb, ar, ag, ab;
	//lr = (float)((LightColor & 0x00FF0000) >> 16) / 255;
	//lg = (float)((LightColor & 0x0000FF00) >> 8) / 255;
	//lb = (float)(LightColor & 0x000000FF) / 255;
	//ar = (float)((AmbientColor & 0x00FF0000) >> 16) / 255;
	//ag = (float)((AmbientColor & 0x0000FF00) >> 8) / 255;
	//ab = (float)(AmbientColor & 0x000000FF) / 255;

	//m_pST->SetLight(LightDir.x, LightDir.y, LightDir.z, lr, lg, lb, ar, ag, ab);
}

void KDlg_SpeedTree::OnLXChanged()
{
	//LightingChanged();
}

void KDlg_SpeedTree::OnLYChanged()
{
	LightingChanged();
}

void KDlg_SpeedTree::OnLZChanged()
{
	LightingChanged();
}

void KDlg_SpeedTree::OnLightColorChanged()
{
	LightingChanged();

	char szColor[256];
	GetDlgItemText(IDD_ST_LIGHTCOLOR, szColor, 256);
	string sColor = szColor;
	unsigned int color = HexToDec(sColor);

	//unsigned char a, r, g, b;
	//a = (color & 0xFF000000) >> 24;
	//r = (color & 0x00FF0000) >> 16;
	//g = (color & 0x0000FF00) >> 8;
	//b = color & 0x000000FF;



	CWnd* pWnd = GetDlgItem(IDD_ST_LIGHTCOLORTABLE);
	RECT rc;
	pWnd->GetClientRect(&rc);
	CDC* pDC = pWnd->GetDC();

	CPen PenSelColor(PS_SOLID, 1, color);
	CBrush BrushSelColor(color);
	CPen* PenSave = pDC->SelectObject(&PenSelColor);
	CBrush* BrushSave = pDC->SelectObject(&BrushSelColor);
	Rectangle(pDC->m_hDC, rc.left, rc.top, rc.right, rc.bottom);
	//pDC->Rectangle(&rc);
	//pDC->SelectObject(BrushSave);
	//pDC->SelectObject(PenSave);
}

void KDlg_SpeedTree::OnAmbientColorChanged()
{
	LightingChanged();
}

void KDlg_SpeedTree::OnLightColorTableClicked()
{
	//CColorDialog dlg(RGB((m_pST->m_LightColor.x * 255), (m_pST->m_LightColor.y * 255), (m_pST->m_LightColor.z * 255)), CC_FULLOPEN);
	//if(dlg.DoModal() != IDOK)
	//{
	//	return;
	//}

	//COLORREF cr = dlg.GetColor();
	//unsigned char r, g, b;
	//r = GetRValue(cr);
	//g = GetGValue(cr);
	//b = GetBValue(cr);
	//D3DCOLOR dcolor = D3DCOLOR_XRGB(r, g, b);

	//string sColor;
	//DecToHex(dcolor, sColor);
	//SetDlgItemText(IDD_ST_LIGHTCOLOR, (LPCTSTR)sColor.c_str());
}

void KDlg_SpeedTree::OnAmbientColorTableClicked()
{
	/*CColorDialog dlg(RGB((m_pST->m_AmbientColor.x * 255), (m_pST->m_AmbientColor.y * 255), (m_pST->m_AmbientColor.z * 255)), CC_FULLOPEN);
	if(dlg.DoModal() != IDOK)
	{
		return;
	}

	COLORREF cr = dlg.GetColor();
	unsigned char r, g, b;
	r = GetRValue(cr);
	g = GetGValue(cr);
	b = GetBValue(cr);
	D3DCOLOR dcolor = D3DCOLOR_XRGB(r, g, b);

	string sColor;
	DecToHex(dcolor, sColor);
	SetDlgItemText(IDD_ST_AMBIENTCOLOR, (LPCTSTR)sColor.c_str());*/
}

void KDlg_SpeedTree::OnTexFilenameChanged()
{
	/*char szText[256];
	memset(szText, 0, sizeof(szText));

	GetDlgItemText(IDD_ST_TEXTUREFILENAME, szText, sizeof(szText));

	string sTexName = szText;

	if(m_TexFile.size())
	{
		IDirect3DTexture9* pTex = 0;
		if(FAILED(D3DXCreateTextureFromFile(g_pd3dDevice, m_TexFile.c_str(), &pTex)))
		{
			return;
		}

		m_pST->m_Texture.m_TexName = sTexName;
		m_pST->m_Texture.m_pTexture = pTex;
		m_bNeedReleaseTex = true;
	}*/
}

void KDlg_SpeedTree::OnTexFilenameButtonClicked()
{
	/*CFileDialog dlg(true, "tga", 0, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, "TGA贴图文件(*.tga)|*.tga||");
	if(dlg.DoModal() != IDOK)
	{
		return;
	}

	string sAppDirectory = g_Def_AppDirectory;
	string sTexPath = dlg.GetPathName();
	string sTexFilename = sTexPath.substr(sAppDirectory.size() + 1);
	string sTexFile = dlg.GetFileName();
	m_TexFile = sTexFile;

	SetDlgItemText(IDD_ST_TEXTUREFILENAME, sTexFilename.c_str());*/
}
void KDlg_SpeedTree::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	// TODO: Add your message handler code here and/or call default

	/*CPropertyPage::OnHScroll(nSBCode, nPos, pScrollBar);

	CSliderCtrl* pSlider = (CSliderCtrl*)pScrollBar;
	float pos = static_cast<float>(pSlider->GetPos());
	
	m_pST->m_AlphaFactor = (pos - 49) / 50;

	LightingChanged();*/
}

void KDlg_SpeedTree::OnResetClicked()
{
	/*m_pST->Pro_AngleSwing = m_Swing;
	m_pST->Pro_MinVelocity = m_MinVel;
	m_pST->Pro_MaxVelocity = m_MaxVel;
	m_pST->Pro_MinWidth = m_MinWidth;
	m_pST->Pro_MaxWidth = m_MaxWidth;
	m_pST->Pro_MinHeight = m_MinHeight;
	m_pST->Pro_MaxHeight = m_MaxHeight;
	m_pST->Pro_Segment = m_SegNum;
	m_pST->m_LightDir.x = m_Lx;
	m_pST->m_LightDir.y = m_Ly;
	m_pST->m_LightDir.z = m_Lz;
	m_pST->m_Texture.m_pTexture = m_pTex;
	m_pST->m_Texture.m_TexName = m_TexFilename;

	m_pST->UpdateParameters();

	float lr, lg, lb, ar, ag, ab;
	lr = (float)((m_LightColor & 0x00FF0000) >> 16) / 255;
	lg = (float)((m_LightColor & 0x0000FF00) >> 8) / 255;
	lb = (float)(m_LightColor & 0x000000FF) / 255;
	ar = (float)((m_AmbientColor & 0x00FF0000) >> 16) / 255;
	ag = (float)((m_AmbientColor & 0x0000FF00) >> 8) / 255;
	ab = (float)(m_AmbientColor & 0x000000FF) / 255;

	m_pST->m_LightColor.x = lr;
	m_pST->m_LightColor.y = lg;
	m_pST->m_LightColor.z = lb;
	m_pST->m_AmbientColor.x = ar;
	m_pST->m_AmbientColor.y = ag;
	m_pST->m_AmbientColor.z = ab;

	m_pST->m_AlphaFactor = m_AlphaFactor;

	m_pST->SetLight(m_Lx, m_Ly, m_Lz, lr, lg, lb, ar, ag, ab);

	OnClose();
	SetSpeedTree(m_pST);
	Show();*/
}
