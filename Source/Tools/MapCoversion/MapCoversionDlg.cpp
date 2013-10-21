// MapCoversionDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Registry.h"
#include "MapSerialize.h"
#include "MapCoversion.h"
#include "MapCoversionDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()


// CMapCoversionDlg dialog




CMapCoversionDlg::CMapCoversionDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CMapCoversionDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CMapCoversionDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_DIRECTORY, m_cboDrives);

}

BEGIN_MESSAGE_MAP(CMapCoversionDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_SEARCH, &CMapCoversionDlg::OnBnClickedSearch)
	ON_BN_CLICKED(IDC_COVERSION, &CMapCoversionDlg::OnBnClickedCoversion)
	ON_BN_CLICKED(IDC_COMPARISON, &CMapCoversionDlg::OnBnClickedComparison)
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDOK, &CMapCoversionDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDC_DELETE, &CMapCoversionDlg::OnBnClickedDelete)
END_MESSAGE_MAP()


// CMapCoversionDlg message handlers

BOOL CMapCoversionDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here
	CRegistry regBackUp;
	char cItem[255];
	int nIndex;
	CString strIndex;

	m_cboDrives.LoadDrives();
	if(regBackUp.Open(HKEY_LOCAL_MACHINE,"Software\\Kingsoft\\MapCoversion\\1.0\\Recent Folders"))
	{
		for(nIndex=1;nIndex<=5;nIndex++)
		{
			strIndex.Format("%d",nIndex);
			if(regBackUp.ReadString(strIndex,cItem,sizeof(cItem)))
				m_cboDrives.SetItemData(m_cboDrives.AddString(cItem),1001);
			else
				break;
		}
		regBackUp.ReadString("HotItem",cItem,sizeof(cItem));
		if(cItem!=NULL) m_cboDrives.SetWindowText(cItem);
		regBackUp.Close();
	}



	// add list header
	CListCtrl* pMapCtrl = ( CListCtrl* )this->GetDlgItem( IDC_LST_INFO );
	ASSERT( NULL != pMapCtrl );

	pMapCtrl->SetExtendedStyle( LVS_EX_FULLROWSELECT );
	pMapCtrl->InsertColumn( 0, "NAME", LVCFMT_CENTER, 220 );
	pMapCtrl->InsertColumn( 1, "SIZE", LVCFMT_CENTER, 100 );
	pMapCtrl->InsertColumn( 2, "REGION", LVCFMT_CENTER, 80 );
	pMapCtrl->InsertColumn( 3, "SPLIT", LVCFMT_CENTER, 100 );
	pMapCtrl->InsertColumn( 4, "COMPACT", LVCFMT_CENTER, 100 );


	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CMapCoversionDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CMapCoversionDlg::OnPaint()
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
HCURSOR CMapCoversionDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CMapCoversionDlg::OnBnClickedSearch()
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);

	m_strDirectory = m_cboDrives.GetSelectedDirectory();
	if ( !m_strDirectory.IsEmpty() )
	{
		cleanMapBox();
		fillMapBox( m_strDirectory );
	}
	else
	{
		MessageBox( TEXT( "Invalid Directory/Filename" ), TEXT( "Map Conversion" ),MB_ICONERROR|MB_OK);
	}


	UpdateData(FALSE);
}

void CMapCoversionDlg::OnBnClickedCoversion()
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);

	CListCtrl* pMapCtrl = ( CListCtrl* )this->GetDlgItem( IDC_LST_INFO );
	ASSERT( NULL != pMapCtrl );


	if ( 0 == pMapCtrl->GetSelectedCount() )
	{
		return ;
	}


	//for ( int i = 0; i < pMapCtrl->GetItemCount(); ++ i )
	POSITION pos = pMapCtrl->GetFirstSelectedItemPosition();
	bool  bModify = true;

#ifdef _DEBUG
	bModify = false;
#endif

	while ( pos )
	{
		int mapIndex = pMapCtrl->GetNextSelectedItem( pos );
		CString  mapName;


		mapName = pMapCtrl->GetItemText( mapIndex, 0 );
		mapName = m_strDirectory + "\\" + mapName;

		CMapSerialize  mapSerialize;
		if ( mapSerialize.Prepare( mapName ) )
		{
			mapSerialize.Coversion();
			mapSerialize.Commit( bModify );
		}


	}

	UpdateData(FALSE);



	// message box
	CString  msgInfo;

	msgInfo.Format( "Coversion %d map", pMapCtrl->GetSelectedCount() );
	MessageBox( msgInfo, TEXT( "Map Conversion" ), MB_OK);

}


void CMapCoversionDlg::OnBnClickedComparison()
{
	// TODO: Add your control notification handler code here
	CListCtrl* pMapCtrl = ( CListCtrl* )this->GetDlgItem( IDC_LST_INFO );
	ASSERT( NULL != pMapCtrl );


	if ( 2 != pMapCtrl->GetSelectedCount() )
	{
		MessageBox( TEXT( "need two file to comparison!" ), TEXT( "Map Conversion" ), MB_OK);
		return ;
	}


	//for ( int i = 0; i < pMapCtrl->GetItemCount(); ++ i )
	POSITION pos = pMapCtrl->GetFirstSelectedItemPosition();
	std::vector< CString >  nameVector;


	while ( pos )
	{
		int mapIndex = pMapCtrl->GetNextSelectedItem( pos );
		CString  mapName;


		mapName = pMapCtrl->GetItemText( mapIndex, 0 );
		mapName = m_strDirectory + "\\" + mapName;
		nameVector.push_back( mapName );
	}


	if ( 2 != nameVector.size() )
	{
		MessageBox( TEXT( "need two file to comparison!" ), TEXT( "Map Conversion" ), MB_OK);
		return ;
	}


	if ( CMapSerialize().Comparison( nameVector.at(0), nameVector.at(1) ) )
	{
		MessageBox( TEXT( "comparison equal!" ), TEXT( "Map Conversion" ), MB_OK);
		return ;
	}


	MessageBox( TEXT( "comparison not equal!" ), TEXT( "Map Conversion" ), MB_OK);


}


void CMapCoversionDlg::saveMapBox()
{
	// TODO: Add your message handler code here and/or call default
	CRegistry regBackUp;
	int nItem,nIndex;
	CString strItem;
	CString strIndex;
	char cItem[255];


	if(!regBackUp.Open(HKEY_LOCAL_MACHINE,"Software\\Kingsoft\\MapCoversion\\1.0\\Recent Folders"))
	{
		regBackUp.CreateKey(HKEY_LOCAL_MACHINE,"Software\\Kingsoft\\MapCoversion\\1.0\\Recent Folders",NULL);
		regBackUp.Open(HKEY_LOCAL_MACHINE,"Software\\Kingsoft\\MapCoversion\\1.0\\Recent Folders");
	}
	if(regBackUp.IsOpen())
	{
		if((nItem=m_cboDrives.FindStringExact(0,"Browse"))!=LB_ERR)
		{
			for(nIndex=1,nItem++;nIndex<=5;nIndex++,nItem++)
			{
				if(nItem<m_cboDrives.GetCount())
				{
					strIndex.Format("%d",nIndex);
					m_cboDrives.GetLBText(nItem,cItem);
					regBackUp.WriteString(strIndex,cItem);
				}
				else
					break;
			}
		}
		if(!m_cboDrives.GetSelectedDirectory().IsEmpty())
		{
			sprintf(cItem, "%s", (LPCTSTR) m_cboDrives.GetSelectedDirectory());
			regBackUp.WriteString("HotItem",cItem);
		}
		regBackUp.Close();
	}


}

void  CMapCoversionDlg::cleanMapBox()
{
	CListCtrl* pMapCtrl = ( CListCtrl* )this->GetDlgItem( IDC_LST_INFO );
	ASSERT( NULL != pMapCtrl );

	pMapCtrl->DeleteAllItems();

}

void  CMapCoversionDlg::fillMapBox( CString& fileFolder )
{
	CFileFind  fileFind;
	CString    fileMap( fileFolder + "\\*.*" );

	BOOL IsFinded = (BOOL)fileFind.FindFile( fileMap );

	while ( IsFinded )
	{
		// 查找到的如果是目录并且不是 . 或 .. 
		IsFinded = fileFind.FindNextFile( );
		if ( fileFind.IsDirectory() 
			&& !fileFind.IsDots() ) 
		{ 
			fillMapBox( fileFind.GetFilePath() );
		}
		else if ( !fileFind.IsDirectory() )
		{
			CFileStatus  fileStatus;

			CFile::GetStatus( fileFind.GetFilePath(), fileStatus );
			fillMapBox( fileStatus );
		}

	
	} 
	// End of while 
	
	
	fileFind.Close(); 

}

void  CMapCoversionDlg::fillMapBox( CFileStatus& fileStatus )
{
	CListCtrl* pMapCtrl = ( CListCtrl* )this->GetDlgItem( IDC_LST_INFO );
	ASSERT( NULL != pMapCtrl );

	CString  fileMap( fileStatus.m_szFullName );

	fileMap.Replace( m_strDirectory + "\\", "" );
	fileMap.MakeLower();
	if ( fileMap.Right( 4 ) == CString( ".map" ) )
	{
		// set base info
		CString tempStr;
		int nItem = pMapCtrl->GetItemCount();
		pMapCtrl->InsertItem( nItem, fileMap );

		tempStr.Format( "%f M", ( (float)fileStatus.m_size ) / 1024 / 1024 );
		pMapCtrl->SetItemText( nItem, 1, tempStr );


		// set version info
		CMapSerialize mapSerialize;
		CString tempFmt, tempSplit;
		CString tempObj, tempRegion;

		tempFmt.Format( "FAILED" );
		tempSplit.Format( "FAILED" );
		if ( mapSerialize.Prepare( fileStatus.m_szFullName ) )
		{
			if ( mapSerialize.IsFmtCompact() )	tempFmt.Format( "YES" );
			else								tempFmt.Format( "NO" );
			
			if ( mapSerialize.IsFmtSplit() )	tempSplit.Format( "YES" );
			else								tempSplit.Format( "NO" );


			tempObj.Format( "%d", mapSerialize.GetMapObjs() );
			tempRegion.Format( "%d", mapSerialize.GetMapRegions() );
			mapSerialize.Rollback( );
		}



		pMapCtrl->SetItemText( nItem, 2, tempRegion );
		pMapCtrl->SetItemText( nItem, 3, tempSplit );
		pMapCtrl->SetItemText( nItem, 4, tempFmt );
		
	}

}

void CMapCoversionDlg::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	saveMapBox();
	OnOK();
}

void CMapCoversionDlg::OnBnClickedDelete()
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);

	CListCtrl* pMapCtrl = ( CListCtrl* )this->GetDlgItem( IDC_LST_INFO );
	ASSERT( NULL != pMapCtrl );


	if ( 0 == pMapCtrl->GetSelectedCount() )
	{
		return ;
	}


	POSITION pos = pMapCtrl->GetFirstSelectedItemPosition();
	std::vector< int > mapVector;

	while ( pos )
	{
		int mapIndex = pMapCtrl->GetNextSelectedItem( pos );
		CString  mapName;
		CString  mapDelInfo( "know without a doubt to delete  " );


		mapName = pMapCtrl->GetItemText( mapIndex, 0 );
		if ( IDYES == MessageBox( mapDelInfo + mapName, TEXT( "Map Conversion" ),MB_ICONWARNING|MB_YESNO) )
		{
			mapVector.push_back( mapIndex );
			mapName = m_strDirectory + "\\" + mapName;
			::DeleteFile( mapName );
		}

	}


	if ( !mapVector.empty() )
	{
		for ( ; !mapVector.empty(); )
		{
			pMapCtrl->DeleteItem( mapVector.back() );
			mapVector.pop_back();
		}
	}


	UpdateData(FALSE);
}
