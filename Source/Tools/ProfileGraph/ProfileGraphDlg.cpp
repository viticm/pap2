// ProfileGraphDlg.cpp : 实现文件
//

#include "stdafx.h"
#include <algorithm>
#include "ProfileGraph.h"
#include "ProfileGraphDlg.h"
//#include "ProfileGraphDlgTable.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CProfileGraphDlg 对话框
struct GraphSort_t
{
public:
	GraphSort_t( int cr, float tm )
		:	color( cr ),
			time( tm )
	{

	}


	int		color;
	float	time;
};



inline bool SortCompare_Graph( GraphSort_t const& lhs, GraphSort_t const& rhs )
{
	return ( lhs.time > rhs.time );
}


CProfileGraphDlg::CProfileGraphDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CProfileGraphDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

CProfileGraphDlg::~CProfileGraphDlg()
{

}

void CProfileGraphDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_GRAPHCALL, m_treeGraphCall);
	DDX_Control(pDX, IDC_GRAPHCOUNTER, m_lstGraphCounter);
}

BEGIN_MESSAGE_MAP(CProfileGraphDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BTN_OPEN, OnBnClickedBtnOpen)
	ON_BN_CLICKED(IDC_BTN_TABLE, OnBnClickedBtnTable)

	ON_NOTIFY(TVN_SELCHANGED, IDC_GRAPHCALL, OnTvnSelchangedGraphcall)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_GRAPHCOUNTER, OnLvnItemchangedGraphcounter)
END_MESSAGE_MAP()


// CProfileGraphDlg 消息处理程序

BOOL CProfileGraphDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码
	DWORD extStyle = m_lstGraphCounter.GetExtendedStyle();
	
	m_lstGraphCounter.SetExtendedStyle( extStyle | LVS_EX_FULLROWSELECT );
	m_lstGraphCounter.InsertColumn( 0, "NAME", LVCFMT_LEFT, 150 );
	m_lstGraphCounter.InsertColumn( 1, "%", LVCFMT_CENTER, 50 );
	m_lstGraphCounter.InsertColumn( 2, "%", LVCFMT_CENTER, 50 );
	m_lstGraphCounter.InsertColumn( 3, "FRAME AVG", LVCFMT_CENTER, 80 );
	m_lstGraphCounter.InsertColumn( 4, "FRAME MIN", LVCFMT_CENTER, 80 );
	m_lstGraphCounter.InsertColumn( 5, "FRAME MAX", LVCFMT_CENTER, 80 );
	m_lstGraphCounter.InsertColumn( 6, "TOTAL CALL", LVCFMT_CENTER, 100 );
	m_lstGraphCounter.InsertColumn( 7, "TOTAL TIMES", LVCFMT_CENTER, 100 );


	// create graph chart
	m_GraphChart.SubclassDlgItem( IDC_GRAPHCHART, this );
	m_GraphChart.PrepareColors( CNSChartCtrl::SimpleColors );
	m_GraphChart.SetChartStyle( NSCS_BAR );
	m_GraphChart.ShowWindow( SW_SHOW );


	m_GraphLine.SubclassDlgItem( IDC_GRAPHLINE, this );
	m_GraphSysInfo.SubclassDlgItem( IDC_EDIT_SYSINFO, this );



	return TRUE;  // 除非设置了控件的焦点，否则返回 TRUE
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CProfileGraphDlg::OnPaint() 
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


		// get client rect
		CClientDC tempDC( &m_GraphLine );
		CBitmap* tempBitmap;
		CRect    rectTemp;

	
		if ( NULL != m_CompatibleDC.GetSafeHdc() )
		{
			m_GraphLine.GetClientRect(&rectTemp);
			tempBitmap = m_CompatibleDC.SelectObject( &m_CompatibleBitmap );
			tempDC.BitBlt( 0, 0, rectTemp.Width(), rectTemp.Height(), &m_CompatibleDC, 0, 0, SRCCOPY );
			m_CompatibleDC.SelectObject( tempBitmap );
		}

	}

}

//当用户拖动最小化窗口时系统调用此函数取得光标显示。
HCURSOR CProfileGraphDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CProfileGraphDlg::OnBnClickedBtnOpen()
{
	// TODO: Add your control notification handler code here
	static char szFilters[] = "KPG Files (*.kpg)|*.kpg|All Files (*.*)|*.*||";
	CFileDialog filedlg (TRUE, "kpg", "*.kpg",
		OFN_FILEMUSTEXIST| OFN_HIDEREADONLY, szFilters, this);

	if( filedlg.DoModal ()==IDOK )
	{
		CString pathName = filedlg.GetPathName();

		m_GraphDoc.Close();
		m_GraphChart.ResetChart();

		if ( m_GraphDoc.Open( (LPCSTR)pathName ) )
		{
			// change the window's title to the opened file's title.
			CString fileName = filedlg.GetFileTitle ();
			CString captionName;

			captionName.Format( "%s    FRAME = %d", fileName, m_GraphDoc.GetFrameNumber() );
			SetWindowText(captionName);


			// fill graph sys info
			fillGraphSysInfo();


			// fill graph call tree
			fillGraphCallTree();

		}

	}

}

void CProfileGraphDlg::OnBnClickedBtnTable()
{
	//CProfileGraphDlgTable  dlgSorted( &m_GraphDoc, this );

	//dlgSorted.DoModal();
}



////////////////////////////////////////////////
// fill system info
void  CProfileGraphDlg::fillGraphSysInfo()
{
	// get sys info
	CString strCpuInfo;
	CString strMemInfo;
	CString strOsInfo;


	// get cpu info
	SYSTEM_INFO const& sysInfo = m_GraphDoc.GetSystemInfo();
	
	strCpuInfo.Format( "PROCESSORS NUMBER: %d\r\nPROCESSORS INFO:%s\r\n\r\n", sysInfo.dwNumberOfProcessors, m_GraphDoc.GetCpuInfo() );


	// get memory info
	MEMORYSTATUS const& memStatus = m_GraphDoc.GetMemoryStatus();
	int const memMSize = 1024 * 1024;

	CString strMemVirtual;


	strMemInfo.Format( "RAM TOTAL: %d MB\r\nRAM FREE: %d MB\r\n\r\n", memStatus.dwTotalPhys / memMSize, memStatus.dwAvailPhys / memMSize );
	strMemVirtual.Format( "VIRTUAL TOTAL: %d MB\r\nVIRTUAL FREE: %d MB", memStatus.dwTotalVirtual / memMSize, memStatus.dwAvailVirtual / memMSize );
	strMemInfo += strMemVirtual;


	// get os info
	OSVERSIONINFOEX const& osvi = m_GraphDoc.GetOsVersionInfoEx();
	
	strOsInfo = "OS: ";
	switch (osvi.dwPlatformId)
	{
		// Test for the Windows NT product family.
	case VER_PLATFORM_WIN32_NT:
		{
			// Test for the specific product.
			if ( osvi.dwMajorVersion == 5 && osvi.dwMinorVersion == 2 )
			{
				strOsInfo += "Microsoft Windows Server 2003 ";
			}
			if ( osvi.dwMajorVersion == 5 && osvi.dwMinorVersion == 1 )
			{
				strOsInfo += "Microsoft Windows XP ";
			}
			if ( osvi.dwMajorVersion == 5 && osvi.dwMinorVersion == 0 )
			{	
				strOsInfo += "Microsoft Windows 2000 ";
			}
            if ( osvi.dwMajorVersion <= 4 )
			{
				strOsInfo += "Microsoft Windows NT ";
			}


			// Test for the workstation type.
			if ( osvi.wProductType == VER_NT_WORKSTATION )
			{
				if( osvi.dwMajorVersion == 4 )
				{
					strOsInfo += "Workstation 4.0 ";
				}
				else if( osvi.wSuiteMask & VER_SUITE_PERSONAL )
				{
					strOsInfo += "Home Edition ";
				}
				else
				{
					strOsInfo += "Professional ";
				}

			}
			// Test for the server type.
			else if ( osvi.wProductType == VER_NT_SERVER || 
				osvi.wProductType == VER_NT_DOMAIN_CONTROLLER )
			{
				if( osvi.dwMajorVersion == 5 && osvi.dwMinorVersion == 2 )
				{
					if( osvi.wSuiteMask & VER_SUITE_DATACENTER )
						strOsInfo += "Datacenter Edition ";
					else if( osvi.wSuiteMask & VER_SUITE_ENTERPRISE )
						strOsInfo += "Enterprise Edition ";
					else if ( osvi.wSuiteMask == VER_SUITE_BLADE )
						strOsInfo += "Web Edition ";
					else
						strOsInfo += "Standard Edition ";
				}
				else if( osvi.dwMajorVersion == 5 && osvi.dwMinorVersion == 0 )
				{
					if( osvi.wSuiteMask & VER_SUITE_DATACENTER )
						strOsInfo += "Datacenter Server ";
					else if( osvi.wSuiteMask & VER_SUITE_ENTERPRISE )
						strOsInfo += "Advanced Server ";
					else
						strOsInfo += "Server ";
				}
				else  // Windows NT 4.0 
				{
					if( osvi.wSuiteMask & VER_SUITE_ENTERPRISE )
						strOsInfo += "Server 4.0, Enterprise Edition ";
					else
						strOsInfo += "Server 4.0 ";
				}
			}

			strOsInfo += osvi.szCSDVersion;
		}
		break;

		// Test for the Windows Me/98/95.
	case VER_PLATFORM_WIN32_WINDOWS:
		{
			if (osvi.dwMajorVersion == 4 && osvi.dwMinorVersion == 0)
			{
				strOsInfo += "Microsoft Windows 95 ";
				if ( osvi.szCSDVersion[1] == 'C' || osvi.szCSDVersion[1] == 'B' )
					strOsInfo += "OSR2 ";
			} 
			if (osvi.dwMajorVersion == 4 && osvi.dwMinorVersion == 10)
			{
				strOsInfo += "Microsoft Windows 98 ";
				if ( osvi.szCSDVersion[1] == 'A' )
					strOsInfo += "SE ";
			} 
			if (osvi.dwMajorVersion == 4 && osvi.dwMinorVersion == 90)
			{
				strOsInfo += "Microsoft Windows Millennium Edition";
			} 
		}
		break;

	case VER_PLATFORM_WIN32s:
		{
			strOsInfo += "Microsoft Win32s";
		}
		
		break;
	}


	

	// fill sys info
	CString strSysInfo;

	strSysInfo += strOsInfo + "\r\n\r\n";
	strSysInfo += strCpuInfo;
	strSysInfo += strMemInfo;



	// fill graphic info
	PropGraph_t const& propGraph = m_GraphDoc.GetPropInfo();
	CString strGraphInfo;


	strGraphInfo.Format( "\r\n\r\nTOTAL TEXTURES: %d \r\nTOTAL RENDER TEXTURE: %d \r\n", propGraph.m_nTotalTextures, propGraph.m_nTotalRenderTextures );
	strSysInfo += strGraphInfo;

	strGraphInfo.Format( "TOTAL DEPTH STENCIL TEXTURES: %d \r\n\r\n", propGraph.m_nTotalDepthStencilTextures );
	strSysInfo += strGraphInfo;

	strGraphInfo.Format( "TOTAL IB USED MEM: %d \r\nTOTAL VB USED MEM: %d \r\n", propGraph.m_nTotalIBUsedMems, propGraph.m_nTotalVBUsedMems );
	strSysInfo += strGraphInfo;

	strGraphInfo.Format( "TOTAL INDEX BUFFERS: %d \r\nTOTAL VERTEX BUFFERS: %d \r\n", propGraph.m_nTotalIndexBuffers, propGraph.m_nTotalVertexBuffers );
	strSysInfo += strGraphInfo;



	m_GraphSysInfo.SetWindowText( strSysInfo );

}


////////////////////////////////////////////////
// fill tree / list ctrl
void  CProfileGraphDlg::fillGraphCallTree()
{
	m_treeGraphCall.DeleteAllItems();
	m_lstGraphCounter.DeleteAllItems();


	CProfileGraphNode* pNode = m_GraphDoc.GetNode();
	if ( NULL != pNode )
	{
		fillGraphCallTree( NULL, pNode );
	}

}

void  CProfileGraphDlg::fillGraphCallTree( HTREEITEM hParent, CProfileGraphNode* pNode )
{
	HTREEITEM hTreeNode = NULL;
	
	hTreeNode = m_treeGraphCall.InsertItem( pNode->GetName(), hParent );
	m_treeGraphCall.SetItemData( hTreeNode, m_GraphDoc.GetNodeIndexByPtr( pNode ) );


	for ( CProfileGraphNode* pChild = pNode->GetChildPtr(); 
		NULL != pChild; 
		pChild = pChild->GetSiblingPtr() 
		)
	{
		fillGraphCallTree( hTreeNode, pChild );
	}


	m_treeGraphCall.Expand( hTreeNode, TVE_EXPAND );
}


void  CProfileGraphDlg::fillGraphList( HTREEITEM hTreeSel )
{
	DWORD_PTR itemData = m_treeGraphCall.GetItemData( hTreeSel );
	CProfileGraphNode* pNode = m_GraphDoc.GetNodeByIndex( static_cast< UINT >( itemData ) );

	m_lstGraphCounter.DeleteAllItems();
	if ( NULL != pNode )
	{
		fillGraphList( "..", pNode, pNode );
		for ( CProfileGraphNode* pChild = pNode->GetChildPtr(); 
			NULL != pChild; 
			pChild = pChild->GetSiblingPtr() 
			)
		{
			fillGraphList( pChild->GetName(), pChild, pNode );
		}

	}

}

void  CProfileGraphDlg::fillGraphList( char const* name, CProfileGraphNode* pChild, CProfileGraphNode* pNode )
{
	CString temp;
	int item = m_lstGraphCounter.GetItemCount();

	item = m_lstGraphCounter.InsertItem( item, name );

	temp.Format( "%f", pChild->GetTimes() / pNode->GetTimes() * 100.0f );
	m_lstGraphCounter.SetItemText( item, 1, temp );

	temp.Format( "%f", pChild->GetTimes() / m_GraphDoc.GetNode()->GetTimes() * 100.0f );
	m_lstGraphCounter.SetItemText( item, 2, temp );

	temp.Format( "%f", pChild->GetTimes() / pChild->GetCalls() );
	m_lstGraphCounter.SetItemText( item, 3, temp );

	temp.Format( "%f", pChild->GetTimeMin() );
	m_lstGraphCounter.SetItemText( item, 4, temp );

	temp.Format( "%f", pChild->GetTimeMax() );
	m_lstGraphCounter.SetItemText( item, 5, temp );


	temp.Format( "%d", pChild->GetCalls() );
	m_lstGraphCounter.SetItemText( item, 6, temp );

	temp.Format( "%f", pChild->GetTimes() );
	m_lstGraphCounter.SetItemText( item, 7, temp );
}

void  CProfileGraphDlg::fillGraphChart( HTREEITEM hTreeSel )
{
	DWORD_PTR itemData = m_treeGraphCall.GetItemData( hTreeSel );
	CProfileGraphNode* pNode = m_GraphDoc.GetNodeByIndex( static_cast< UINT >( itemData ) );

	m_GraphChart.ResetChart();
	if ( NULL != pNode )
	{
		float other = 100.0f;
		int i = 0;

		for ( CProfileGraphNode* pChild = pNode->GetChildPtr(); 
			NULL != pChild; 
			pChild = pChild->GetSiblingPtr() 
			)
		{
			CString temp;
			float item = pChild->GetTimes() / pNode->GetTimes() * 100.0f;

			temp.Format( "ITEM_%d", i ++ );
			m_GraphChart.AddValue( item, temp );
			other -= item;
		}


		m_GraphChart.AddValue( other, "OTHER" );
	}

	
	m_GraphChart.Invalidate();
	m_GraphChart.UpdateWindow();

}


void  CProfileGraphDlg::fillGraphLine( HTREEITEM hTreeSel )
{
	// get client rect
	CRect  rectTemp;

	m_GraphLine.GetClientRect(&rectTemp);


	// create graph line
	CClientDC tempDC( &m_GraphLine );
	CBitmap*  tempBitmap = NULL;

	m_CompatibleDC.DeleteDC();
	m_CompatibleBitmap.DeleteObject();

	m_CompatibleDC.CreateCompatibleDC( &tempDC );
	m_CompatibleBitmap.CreateCompatibleBitmap( &tempDC, rectTemp.Width(), rectTemp.Height() );

	tempBitmap = m_CompatibleDC.SelectObject( &m_CompatibleBitmap );

	m_CompatibleDC.SetBkColor( RGB( 255, 255, 255 ) );
	m_CompatibleDC.SelectStockObject( WHITE_BRUSH );
	m_CompatibleDC.Rectangle( &rectTemp );
	
	m_CompatibleDC.SelectStockObject( BLACK_BRUSH );
	m_CompatibleDC.MoveTo( 0, 0 );
	m_CompatibleDC.LineTo( rectTemp.Width(), 0 );
	m_CompatibleDC.LineTo( rectTemp.Width(), rectTemp.Height() );
	m_CompatibleDC.LineTo( 0, rectTemp.Height() );
	m_CompatibleDC.LineTo( 0, 0 );



	// draw graph line
	DWORD_PTR itemData = m_treeGraphCall.GetItemData( hTreeSel );
	CProfileGraphNode* pNode = m_GraphDoc.GetNodeByIndex( static_cast< UINT >( itemData ) );

	if ( NULL != pNode )
	{
		// insert seg
		static COLORREF colorTable[] = {
			0x0000CC00, 0x00FF0000, 0x00CCCC00, 0x000000FF,
			0x000000CC, 0x0000FF00,	0x00FF00FF, 0x00CC00CC,
            0x00CC0000,	0x0000FFFF, 0x0000CCCC, 0x00FFFF00
		};



		float frameFreq = 1;//rectStatic.Width() / m_GraphDoc.GetFrameNumber();
		float timeFreq = 2;//rectStatic.Height() / 1000;
		int   offLeft = 5;
		int   offTop = -5;

		for ( int frame = 0; frame < m_GraphDoc.GetFrameNumber(); ++ frame )
		{
			// push back
			typedef std::vector< GraphSort_t >	SortVector_t;
			SortVector_t	sortVector;
			int   i = 0;

			for ( CProfileGraphNode* pChild = pNode->GetChildPtr(); 
				NULL != pChild; 
				pChild = pChild->GetSiblingPtr(), ++ i
				)
			{
				float firstTime = m_GraphDoc.GetFrameNodeTime( frame == 0 ? 0 : frame - 1, pChild->GetCounter() );
				float curTime = m_GraphDoc.GetFrameNodeTime( frame, pChild->GetCounter() );

				sortVector.push_back( GraphSort_t( i, curTime - firstTime ) );
			}


			// sort graph
			std::sort( sortVector.begin(), sortVector.end(), SortCompare_Graph );
			for ( SortVector_t::iterator iter  = sortVector.begin();
										 iter != sortVector.end();
										 ++ iter 
										 )
			{
				int x = frame * frameFreq + offLeft;
				int y = rectTemp.Height() - iter->time * timeFreq + offTop;

				//m_CompatibleDC.SetPixel( x, y, colorTable[i] );
				CPen  tempPen( PS_SOLID, 1, colorTable[iter->color] );
				CPen* pOldPen = NULL;

				pOldPen = m_CompatibleDC.SelectObject( &tempPen );
				m_CompatibleDC.MoveTo( x, rectTemp.Height() );
				m_CompatibleDC.LineTo( x, y );
				m_CompatibleDC.SelectObject( pOldPen );
			}

		}

	}


	tempDC.BitBlt( 0, 0, rectTemp.Width(), rectTemp.Height(), &m_CompatibleDC, 0, 0, SRCCOPY );
	m_CompatibleDC.SelectObject( tempBitmap );

}

void CProfileGraphDlg::OnTvnSelchangedGraphcall(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	// TODO: Add your control notification handler code here
	ASSERT( m_treeGraphCall.GetSelectedItem() == pNMTreeView->itemNew.hItem );
	fillGraphList( pNMTreeView->itemNew.hItem );
	fillGraphChart( pNMTreeView->itemNew.hItem );
	fillGraphLine( pNMTreeView->itemNew.hItem );


	*pResult = 0;
}

void CProfileGraphDlg::OnLvnItemchangedGraphcounter(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	// TODO: Add your control notification handler code here
	*pResult = 0;
}

