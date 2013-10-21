// EffectCompilerDlg.cpp : implementation file
//

#include "stdafx.h"
#include "EffectCompiler.h"
#include "EffectCompilerDlg.h"
#include "Core.h"
#include "CmdLineFileEnumer.h"
#include "DiskFileController.h"
#include "FileEnumerForDirectory.h"
#include "MemCompileFileController.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CEffectCompilerDlg dialog

class COutputDeviceList : public ICCompileResultOutput
{
public:
	COutputDeviceList(CEffectCompilerDlg& dlg)
		:m_dlg(dlg)
		,m_bOnlyOutputError(FALSE)
	{
		m_dlg.m_GridCtrl.SetRowCount(1);	//留下最上面的一行，准备加数据
		m_dlg.GetWindowText(m_dlgOldCaption);
		m_dlg.m_EditStatistic.SetWindowText(_T(""));
		m_dlg.m_EditContent.SetWindowText(_T(""));
	}
	~COutputDeviceList()
	{
		m_dlg.m_EditStatistic.SetWindowText(m_strInfo);
		m_dlg.SetWindowText(m_dlgOldCaption);
		m_dlg.Invalidate();
	}
	virtual VOID SetOnlyOutputError(BOOL bOnlyOutputError)
	{
		m_bOnlyOutputError = bOnlyOutputError;
	}
	virtual VOID Output(const COutputItem& outputItem)
	{
		CGridCtrl& gridCtrl = m_dlg.m_GridCtrl;
		switch(outputItem.dwOutputCategory)
		{
		case COutputItem::em_out_file_result:
			{
				_ASSERTE(gridCtrl.GetColumnCount() >= _countof(outputItem.strResult));
				int nRowCount = gridCtrl.GetRowCount();
				_ASSERTE(nRowCount >= 1);
				gridCtrl.SetRowCount(nRowCount + 1);
				int nCurRow = nRowCount;

				if (m_bOnlyOutputError && outputItem.bCompileSuccess)
				{
					gridCtrl.SetRowHeight(nCurRow, 0);
				}

				for (int i = 0; i < _countof(outputItem.strResult); ++i)
				{	
					gridCtrl.SetItemText(nCurRow, i, outputItem.strResult[i]);	
				}

				CGridCellBase* pCell = gridCtrl.GetCell(nCurRow, 0);
				if (NULL != pCell)
				{
					if (! outputItem.bCompileSuccess)
					{
						pCell->SetTextClr(RGB(255,0,0));
					}
				}				
			}
			break;
		case COutputItem::em_out_statistic:
		case COutputItem::em_out_notice:
			{
				m_strInfo += outputItem.GetFileName();
				m_strInfo += _T(": ");
				m_strInfo += outputItem.GetCompileResult();
				m_strInfo += _T("\r\n");
			}
			break;
		default:
		    break;
		}
	}
	virtual VOID OutputPercent(FLOAT fPercent)
	{
		fPercent = min(fPercent, 1);
		fPercent = max(fPercent, 0);
		INT iPercent = (INT)(fPercent * 100);

		TCHAR strTitle[MAX_PATH] = _T("");
		_stprintf_s(strTitle, _T("Compiling: %d%%"), iPercent);
		m_dlg.SetWindowText(strTitle);

		//m_dlg.UpdateWindow();
	}

private:
	CEffectCompilerDlg& m_dlg;
	CString m_strInfo;
	BOOL	m_bOnlyOutputError;
	CString	m_dlgOldCaption;
};



CEffectCompilerDlg::CEffectCompilerDlg(CWnd* pParent /*= NULL*/ )
	: CDialog(CEffectCompilerDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}
CEffectCompilerDlg::~CEffectCompilerDlg()
{
}

void CEffectCompilerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_GRID, m_GridCtrl);
	DDX_Control(pDX, IDC_EDIT_CONTENT, m_EditContent);
	DDX_Control(pDX, IDC_EDIT_STATISTIC, m_EditStatistic);
	DDX_Control(pDX, IDC_EDIT1, m_EditDirectory);
}

BEGIN_MESSAGE_MAP(CEffectCompilerDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BUTTON1, &CEffectCompilerDlg::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_CHECK_FORCE_COMPILE, &CEffectCompilerDlg::OnBnClickedCheckForceCompile)
	ON_BN_CLICKED(IDC_CHECK_SHOW_ONLY_ERROR, &CEffectCompilerDlg::OnBnClickedCheckShowOnlyError)
	ON_NOTIFY(GVN_SELCHANGED, IDC_GRID, &CEffectCompilerDlg::OnGridSelectChange)
	ON_BN_CLICKED(IDC_BUTTON_BROWSE, &CEffectCompilerDlg::OnBnClickedButtonBrowse)
END_MESSAGE_MAP()


// CEffectCompilerDlg message handlers

BOOL CEffectCompilerDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here

	{
		TCHAR strDir[MAX_PATH * 2] = _T("");
		::GetCurrentDirectory(_countof(strDir), strDir);
		m_EditDirectory.SetWindowText(strDir);
	}

	{
		LPCTSTR* pElementDesc = NULL;
		size_t	uItemCount = 0;
		COutputItem::GetElementDesc(&pElementDesc, &uItemCount);

		m_GridCtrl.SetColumnCount((int)uItemCount);
		m_GridCtrl.SetRowCount(1);
		m_GridCtrl.SetFixedRowCount(1);

		//利用Item描述动态生成Grid，其中第一行是描述
		CRect rectOfGrid;
		m_GridCtrl.GetWindowRect(&rectOfGrid);
		float fColumnWidth = (float)rectOfGrid.Width() / (float)uItemCount;
		
		m_GridCtrl.SetColumnWidth(1, (int)(fColumnWidth * 0.4f));
		m_GridCtrl.SetColumnWidth(2, (int)(fColumnWidth * 0.4f));
		
		for (int i = 0; i < (int)uItemCount; ++i)
		{
			m_GridCtrl.SetColumnWidth(i, (int)fColumnWidth);
			m_GridCtrl.SetItemText(0, i, pElementDesc[i]);
		}
	}

	this->CheckDlgButton(IDC_CHECK_FORCE_COMPILE, g_AppCmdParams.bForceCompile ? BST_CHECKED : BST_UNCHECKED);
	this->CheckDlgButton(IDC_CHECK_SHOW_ONLY_ERROR, g_AppCmdParams.bShowOnlyError ? BST_CHECKED : BST_UNCHECKED);
	
	_ASSERTE(AppCmdParams::em_run_mode_dialog == g_AppCmdParams.nRunMode);

	if (g_AppCmdParams.bHasCmdFilePathsToBeCompiled)
	{
		CString csDir;
		m_EditDirectory.GetWindowText(csDir);

		ICFileEnumer* pFileEnumer = ::CreateCmdLineFileEnumer(::GetCommandLine(), csDir);
		ICCompileFileController* pController = ::CreateDiskFileController();
		if (NULL != pFileEnumer && NULL != pController)
		{
			COutputDeviceList outDevice(*this);
			
			Core::CompileShaders(*pFileEnumer, outDevice, *pController, g_AppCmdParams.strOutputDir
				,g_AppCmdParams.strCopyToDir, g_AppCmdParams.bForceCompile, g_AppCmdParams.bShowOnlyError);
		}
		SAFE_DELETE(pFileEnumer);
		SAFE_DELETE(pController);
	}

	return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CEffectCompilerDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CEffectCompilerDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CEffectCompilerDlg::OnBnClickedButton1()
{
	// TODO: Add your control notification handler code here
	CString csDirectory;
	m_EditDirectory.GetWindowText(csDirectory);
	ICFileEnumer* pFileEnumer = ::CreateFileEnumerForDirectory(csDirectory);
	ICCompileFileController* pCompileFileController = ::CreateMemFileController();

	if (NULL != pFileEnumer && NULL != pCompileFileController)
	{
		COutputDeviceList outDevice(*this);

		Core::CompileShaders(*pFileEnumer, outDevice, *pCompileFileController, NULL, NULL, g_AppCmdParams.bForceCompile, g_AppCmdParams.bShowOnlyError);
	}

	SAFE_DELETE(pCompileFileController);
	SAFE_DELETE(pFileEnumer);
}




void CEffectCompilerDlg::OnBnClickedCheckForceCompile()
{
	// TODO: Add your control notification handler code here
	BOOL bForceCompile = this->IsDlgButtonChecked(IDC_CHECK_FORCE_COMPILE);
	
	g_AppCmdParams.bForceCompile = bForceCompile;
}

void CEffectCompilerDlg::OnBnClickedCheckShowOnlyError()
{
	// TODO: Add your control notification handler code here
	BOOL bShowOnlyError = this->IsDlgButtonChecked(IDC_CHECK_SHOW_ONLY_ERROR);

	g_AppCmdParams.bShowOnlyError = bShowOnlyError;

	//隐藏非Error的段
	{
		CGridCtrl& gridCtrl = m_GridCtrl;
		if (gridCtrl.GetRowCount() <= 1)
			return;

		INT nDefaultRowHeight = gridCtrl.GetRowHeight(0);

		for (int i = 1; i < gridCtrl.GetRowCount(); ++i)
		{
			if (bShowOnlyError)
			{
				CGridCellBase* pCell = gridCtrl.GetCell(i, 0);
				if (NULL == pCell)
					continue;

				BOOL bErrorRow = (pCell->GetTextClr() == RGB(255,0,0));
				if (bErrorRow)
				{
					gridCtrl.SetRowHeight(i, nDefaultRowHeight);
				}
				else
				{
					gridCtrl.SetRowHeight(i, 0);
				}
			}
			else
			{
				gridCtrl.SetRowHeight(i, nDefaultRowHeight);
			}
		}
		gridCtrl.Invalidate();
	}

	
}

void CEffectCompilerDlg::OnGridSelectChange(NMHDR *pNMHDR, LRESULT *pResult)
{
	*pResult = 0;
	if(pNMHDR->idFrom != m_GridCtrl.GetDlgCtrlID())
		return;

	NM_GRIDVIEW* pGridView = (NM_GRIDVIEW*)	pNMHDR;
	CString strText = m_GridCtrl.GetItemText(pGridView->iRow, pGridView->iColumn);
	m_EditContent.SetWindowText(strText);
}

void CEffectCompilerDlg::OnBnClickedButtonBrowse()
{
	// TODO: Add your control notification handler code here
	BROWSEINFO   info;  
	ZeroMemory(&info,   sizeof(info));  
	info.hwndOwner   =   m_hWnd;  
	TCHAR   tchPath[MAX_PATH * 2] = _T("");   
	info.lpszTitle = _T("选择shader所在路径");
	info.ulFlags = BIF_RETURNONLYFSDIRS | BIF_EDITBOX;

	LPITEMIDLIST   lpDlist;  
	//用来保存返回信息的IDList，使用SHGetPathFromIDList函数转换为字符串  
	lpDlist   =   SHBrowseForFolder(&info)   ;   //显示选择对话框  
	if(lpDlist   !=   NULL)  
	{  
		SHGetPathFromIDList(lpDlist,   tchPath);//把项目标识列表转化成目录  
		TRACE(tchPath); 

		m_EditDirectory.SetWindowText(tchPath);
	}

}
