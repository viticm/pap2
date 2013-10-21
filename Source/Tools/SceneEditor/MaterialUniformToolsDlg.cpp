// MaterialUniformToolsDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "sceneeditor.h"
#include "MaterialUniformToolsDlg.h"
#include "KG3DMaterialSetter.h"
//#include "kg3denginemanager.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CMaterialUniformToolsDlg 对话框




CMaterialUniformToolsDlg::CMaterialUniformToolsDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CMaterialUniformToolsDlg::IDD, pParent)
	, m_strBaseDir(_T(""))
	, m_fAlpha(0)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_Colors[BUTTON_DIFFUSE] = RGB(255, 255, 255);
	m_Colors[BUTTON_EMISSIVE] = RGB(0, 0, 0);
	m_Colors[BUTTON_SPECULAR] = RGB(0, 0, 0);
	m_Colors[BUTTON_AMBIENT] = RGB(255, 255, 255);

	m_AlphaOperation.EnableAlphaBlend = FALSE;
	m_AlphaOperation.SrcBlend = D3DBLEND_ONE;
	m_AlphaOperation.DestBlend = D3DBLEND_ZERO;
	m_AlphaOperation.BlendFactor = 0xffffffff;
	m_AlphaOperation.BlendOperator = D3DBLENDOP_ADD;

	m_AlphaOperation.EnableSeparateAlphaBlend = FALSE;
	m_AlphaOperation.BlendOperatorAlpha = D3DBLENDOP_ADD;
	m_AlphaOperation.SrcBlendAlpha = D3DBLEND_ONE;
	m_AlphaOperation.DestBlendAlpha = D3DBLEND_ZERO;

	m_AlphaOperation.EnableAlphaTest = FALSE;
	m_AlphaOperation.AlphaTestRef = 0;
	m_AlphaOperation.AlphaFunction = D3DCMP_ALWAYS;

    m_AlphaOperation.EnableAlphaBlend = FALSE;
	m_AlphaOperation.SrcBlend = D3DBLEND_SRCALPHA;
	m_AlphaOperation.DestBlend = D3DBLEND_INVSRCALPHA;
	m_AlphaOperation.BlendFactor = 0xffffffff;
	m_AlphaOperation.BlendOperator = D3DBLENDOP_ADD;

	m_AlphaOperation.EnableSeparateAlphaBlend = FALSE;
	m_AlphaOperation.BlendOperatorAlpha = D3DBLENDOP_ADD;
	m_AlphaOperation.SrcBlendAlpha = D3DBLEND_ONE;
	m_AlphaOperation.DestBlendAlpha = D3DBLEND_ZERO;

	m_AlphaOperation.EnableAlphaTest = FALSE;
	m_AlphaOperation.AlphaTestRef = 0x20;
	m_AlphaOperation.AlphaFunction = D3DCMP_GREATEREQUAL;
}

void CMaterialUniformToolsDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_DIR, m_strBaseDir);
	DDX_Text(pDX, IDC_EDIT_ALPHA1, m_fAlpha);

	DDX_Control(pDX, IDC_BUTTON_AMBIENT, m_Buttons[BUTTON_AMBIENT]);
	DDX_Control(pDX, IDC_BUTTON_DIFFUSE, m_Buttons[BUTTON_DIFFUSE]);
	DDX_Control(pDX, IDC_BUTTON_EMISSIVE, m_Buttons[BUTTON_EMISSIVE]);
	DDX_Control(pDX, IDC_BUTTON_SPECULAR, m_Buttons[BUTTON_SPECULAR]);
	DDX_Control(pDX, IDC_LIST1, m_ctlResult);
}

BEGIN_MESSAGE_MAP(CMaterialUniformToolsDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BUTTON_BROWSE, &CMaterialUniformToolsDlg::OnBnClickedButtonBrowse)
	ON_BN_CLICKED(IDC_BUTTON_AMBIENT, &CMaterialUniformToolsDlg::OnBnClickedButtonAmbient)
	ON_BN_CLICKED(IDC_BUTTON_SPECULAR, &CMaterialUniformToolsDlg::OnBnClickedButtonSpecular)
	ON_BN_CLICKED(IDC_BUTTON_DIFFUSE, &CMaterialUniformToolsDlg::OnBnClickedButtonDiffuse)
	ON_BN_CLICKED(IDC_BUTTON_EMISSIVE, &CMaterialUniformToolsDlg::OnBnClickedButtonEmissive)
	ON_BN_CLICKED(IDC_BUTTON6, &CMaterialUniformToolsDlg::OnBnClickedButton6)
	ON_BN_CLICKED(ID_BUTTON_CLEAR, &CMaterialUniformToolsDlg::OnBnClickedButtonClear)
	ON_BN_CLICKED(IDC_BUTTON_ALPHAOP, &CMaterialUniformToolsDlg::OnBnClickedButtonAlphaop)
	ON_BN_CLICKED(IDC_BUTTON_SELECTFILE, &CMaterialUniformToolsDlg::OnBnClickedButtonSelectfile)
END_MESSAGE_MAP()


// CMaterialUniformToolsDlg 消息处理程序

BOOL CMaterialUniformToolsDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	for (DWORD i = 0; i < BUTTON_COUNT; i++)
	{
		CreateBitmapForButton(i);
	}

	static_cast<CButton*>(GetDlgItem(IDC_CHECK_DEPTH))->SetCheck(BST_CHECKED);
	static_cast<CButton*>(GetDlgItem(IDC_CHECK_LIGHT))->SetCheck(BST_CHECKED);
	static_cast<CButton*>(GetDlgItem(IDC_CHECK_HIGHLIGHT))->SetCheck(BST_CHECKED);
	m_fAlpha = 255;

	UpdateData(FALSE);

	//m_ctlResult.EnableScrollBar(SB_BOTH, ESB_ENABLE_BOTH);
	m_ctlResult.InsertColumn(0, "aaa", LVCFMT_LEFT, 330);
	
	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CMaterialUniformToolsDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标显示。
//
HCURSOR CMaterialUniformToolsDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CMaterialUniformToolsDlg::OnBnClickedButtonBrowse()
{
	TCHAR strDir[MAX_PATH];
	BROWSEINFO BrowseInfo;
	memset(&BrowseInfo, 0, sizeof(BROWSEINFO));
	BrowseInfo.hwndOwner = GetSafeHwnd();
	LPCITEMIDLIST pItem = SHBrowseForFolder(&BrowseInfo);
	UpdateData(TRUE);
	if (pItem)
	{
		if (SHGetPathFromIDList(pItem, strDir))
		{
			m_strBaseDir = strDir;
		}
		else
		{
			m_strBaseDir = "";
		}
		UpdateData(FALSE);
	}
}

void CMaterialUniformToolsDlg::OnBnClickedButtonAmbient()
{
	CColorDialog dlgAmbient;
	if (dlgAmbient.DoModal() == IDOK)
	{
		m_Colors[BUTTON_AMBIENT] = dlgAmbient.GetColor();
		UpdateColorButton(BUTTON_AMBIENT);
	}
}

void CMaterialUniformToolsDlg::OnBnClickedButtonSpecular()
{
	CColorDialog dlgSpecular;
	if (dlgSpecular.DoModal() == IDOK)
	{
		m_Colors[BUTTON_SPECULAR] = dlgSpecular.GetColor();
		UpdateColorButton(BUTTON_SPECULAR);
	}
}

void CMaterialUniformToolsDlg::OnBnClickedButtonDiffuse()
{
	CColorDialog dlgDiffuse;
	if (dlgDiffuse.DoModal() == IDOK)
	{
		m_Colors[BUTTON_DIFFUSE] = dlgDiffuse.GetColor();
		UpdateColorButton(BUTTON_DIFFUSE);
	}
}

void CMaterialUniformToolsDlg::OnBnClickedButtonEmissive()
{
	CColorDialog dlgEmissive;
	if (dlgEmissive.DoModal() == IDOK)
	{
		m_Colors[BUTTON_EMISSIVE] = dlgEmissive.GetColor();
		UpdateColorButton(BUTTON_EMISSIVE);
	}
}

void CMaterialUniformToolsDlg::CreateBitmapForButton(DWORD Index)
{
	RECT rect;
	CDC* pDc = GetDC();
	m_Buttons[Index].GetWindowRect(&rect);
	
	int nWidth = abs(rect.right - rect.left);
	int nHeight = abs(rect.bottom - rect.top);
	
	BOOL bReturn = m_BmpMaps[Index].CreateCompatibleBitmap(pDc, nWidth, nHeight);
	
	if (bReturn)
	{
		COLORREF* pColorBuffer = new COLORREF[nWidth * nHeight];
		for(int i = 0; i < nWidth * nHeight; i++)
		{
			pColorBuffer[i] = m_Colors[Index];
		}
		m_BmpMaps[Index].SetBitmapBits(nHeight * nWidth * sizeof(COLORREF), pColorBuffer);
		m_Buttons[Index].SetBitmap(m_BmpMaps[Index]);
		delete[] pColorBuffer;
	}
	
	ReleaseDC(pDc);
}

void CMaterialUniformToolsDlg::UpdateColorButton(DWORD Index)
{
	RECT rect;
	CDC* pDc = GetDC();
	m_Buttons[Index].GetWindowRect(&rect);

	int nWidth = abs(rect.right - rect.left);
	int nHeight = abs(rect.bottom - rect.top);

	COLORREF* pColorBuffer = new COLORREF[nWidth * nHeight];
	COLORREF refFix = RGB((m_Colors[Index] & 0x00ff0000) >> 16,
		                  (m_Colors[Index] & 0x0000ff00) >> 8,
						   m_Colors[Index] & 0x000000ff);
	for(int i = 0; i < nWidth * nHeight; i++)
	{
		pColorBuffer[i] = refFix;
	}
	m_BmpMaps[Index].SetBitmapBits(nHeight * nWidth * sizeof(COLORREF), pColorBuffer);
	m_Buttons[Index].SetBitmap(m_BmpMaps[Index]);
	delete[] pColorBuffer;

	ReleaseDC(pDc);

}


void CMaterialUniformToolsDlg::GetMaterialOption(MaterialOption& Option)
{
	if (static_cast<CButton*>(GetDlgItem(IDC_CHECK_LIGHT))->GetCheck() == BST_CHECKED)
	{
		Option.bLighting = TRUE;
	}
	else 
	{
		Option.bLighting = FALSE;
	}

	if (static_cast<CButton*>(GetDlgItem(IDC_CHECK_HIGHLIGHT))->GetCheck() == BST_CHECKED)
	{
		Option.bHighLight = TRUE;
	}
	else
	{
		Option.bHighLight = FALSE;
	}

	if (static_cast<CButton*>(GetDlgItem(IDC_CHECK_HIDE))->GetCheck() == BST_CHECKED)
	{
		Option.bHide = TRUE;
	}
	else
	{
		Option.bHide = FALSE;
	}

	if (static_cast<CButton*>(GetDlgItem(IDC_CHECK_DEPTH))->GetCheck() == BST_CHECKED)
	{
		Option.bEnableZ = TRUE;
	}
	else
	{
		Option.bEnableZ = FALSE;
	}

	if (static_cast<CButton*>(GetDlgItem(IDC_CHECK_CULLNONE))->GetCheck() == BST_CHECKED)
	{
		Option.dwCullMode = 0;
	}
	else
	{
		Option.dwCullMode = 1;
	}

	UpdateData();
	for (int i = 0; i < BUTTON_COUNT; i++)
	{
		Option.crColors[i] = m_Colors[i];
	}

	m_fAlpha *= 255.0f;
	DWORD dwAlpha = static_cast<DWORD>(m_fAlpha * 255);
	Option.crColors[BUTTON_DIFFUSE] |= ((dwAlpha << 24) & 0xff000000);

	Option.pAlphaOperation = &m_AlphaOperation;
}

void CMaterialUniformToolsDlg::FindFilesInSubDir(LPCSTR strDirName, 
												 const MaterialOption& Option, 
												 KG3DMaterialSetter& Setter)
{
	TCHAR strInput[MAX_PATH];
	CFileFind Find;
	sprintf(strInput, "%s\\*.*", strDirName);
	BOOL bFound = Find.FindFile(strInput);
	BOOL bBackup = (static_cast<CButton*>(GetDlgItem(IDC_CHECK_BACKUP))->GetCheck() == BST_CHECKED) ? TRUE : FALSE; 
	while (bFound)
	{
		bFound = Find.FindNextFile();
		if (Find.IsDirectory())
		{
			CString strDirName = Find.GetFileName();
			if (strDirName != "." && strDirName != "..")
			{
				FindFilesInSubDir(Find.GetFilePath(), Option, Setter);
			}
		}
		else
		{
			CString strFileName = Find.GetFilePath();
			strFileName.MakeLower();
			TCHAR strExt[MAX_PATH];
			_splitpath(strFileName, NULL, NULL, NULL, strExt);
			if (!stricmp(strExt, ".mtl"))
			{
				Setter.SetByOption(strFileName, Option, bBackup, &m_ctlResult);
			}
		}
	}

	Find.Close();
}

void CMaterialUniformToolsDlg::OnBnClickedButton6()
{
	KG3DMaterialSetter Setter;
	MaterialOption Option;
	CFileFind Find;
	UpdateData();
	GetMaterialOption(Option);
	TCHAR strInput[MAX_PATH];
	
	BOOL bBackup = (static_cast<CButton*>(GetDlgItem(IDC_CHECK_BACKUP))->GetCheck() == BST_CHECKED) ? TRUE : FALSE; 
	if (!m_vecSelectFiles.size())
	{
		sprintf(strInput, "%s\\*.*", m_strBaseDir);
		BOOL bFound = Find.FindFile(strInput);
		while (bFound)
		{
			bFound = Find.FindNextFile();
			if (Find.IsDirectory())
			{
				CString strDirName = Find.GetFileName();
				if (strDirName != "." && strDirName != "..")
				{
					FindFilesInSubDir(Find.GetFilePath(), Option, Setter);
				}
			}
			else
			{
				CString strFileName = Find.GetFilePath();
				strFileName.MakeLower();
				TCHAR strExt[MAX_PATH];
				_splitpath(strFileName, NULL, NULL, NULL, strExt);
				
				if (!stricmp(strExt, ".mtl"))
				{
					Setter.SetByOption(strFileName, Option, bBackup, &m_ctlResult);
				}
			}
		}
		Find.Close();
	}
	else
	{
		for (size_t i = 0; i < m_vecSelectFiles.size(); i++)
		{
			Setter.SetByOption(m_vecSelectFiles[i].c_str(), Option, bBackup, &m_ctlResult);
		}
	}
	MessageBox("修改完成");
}

void CMaterialUniformToolsDlg::OnBnClickedButtonClear()
{
	m_ctlResult.DeleteAllItems();
}


void CMaterialUniformToolsDlg::OnBnClickedButtonAlphaop()
{
	m_dlgAlphaOperation.SetAlphaOperation(&m_AlphaOperation);
	m_dlgAlphaOperation.Create(IDD_MATERIALUNIFORMTOOL_ALPHABLEND, this);
	m_dlgAlphaOperation.ShowWindow(SW_SHOW);
}


void CMaterialUniformToolsDlg::OnBnClickedButtonSelectfile()
{
	TCHAR strPathName[MAX_PATH];
	TCHAR strBuffer[2048] = "";
	sprintf_s(strPathName, 
		MAX_PATH,
		"%sData\\source\\maps_source\\",
		g_szDefWorkDirectory);

	CFileDialog dlgFiles(TRUE, 
		".mtl", 
		NULL,
		4 | 2 |OFN_ALLOWMULTISELECT | OFN_ENABLESIZING,
		"材质文件(*.mtl)|*.mtl||",
		this);
	//strcpy_s(strBuffer, MAX_PATH, strPathName);
	dlgFiles.m_ofn.lpstrInitialDir = strPathName;
	dlgFiles.m_ofn.lpstrFile = strBuffer;
	dlgFiles.m_ofn.nMaxFile = 2048;
	m_vecSelectFiles.clear();
	
	if (IDOK == dlgFiles.DoModal())
	{
		POSITION FilePos = dlgFiles.GetStartPosition();
		while(FilePos)
		{
			CString strPathName = dlgFiles.GetNextPathName(FilePos);
			m_vecSelectFiles.push_back(strPathName.GetString());
		}
	}
}
