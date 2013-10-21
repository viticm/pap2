// KSceneSwordTestWizard.cpp : implementation file
//

#include "stdafx.h"
#include "sceneeditor.h"
#include "KSceneSwordTestWizard.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// KSceneSwordTestWizard dialog


KSceneSwordTestWizard::KSceneSwordTestWizard(CWnd* pParent /*=NULL*/)
	: CDialog(KSceneSwordTestWizard::IDD, pParent)
{
	//{{AFX_DATA_INIT(KSceneSwordTestWizard)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_mTestType =0;
}


void KSceneSwordTestWizard::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(KSceneSwordTestWizard)
	DDX_Control(pDX, IDC_LIST1, m_ListBoxFileName);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(KSceneSwordTestWizard, CDialog)
	//{{AFX_MSG_MAP(KSceneSwordTestWizard)
	ON_BN_CLICKED(IDC_RADIO_TEST1, OnRadioTest1)
	ON_BN_CLICKED(IDC_RADIO_TEST2, OnRadioTest2)
	ON_BN_CLICKED(IDC_RADIO_TEST3, OnRadioTest3)
	ON_BN_CLICKED(IDC_RADIO_TEST4, OnRadioTest4)
	ON_BN_CLICKED(IDC_BUTTON_FILE_ADD, OnButtonFileAdd)
	ON_BN_CLICKED(IDC_BUTTON_FILE_REMOVE, OnButtonFileRemove)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// KSceneSwordTestWizard message handlers

void KSceneSwordTestWizard::OnRadioTest1() 
{
	// TODO: Add your control notification handler code here
	m_mTestType =0;
}

void KSceneSwordTestWizard::OnRadioTest2() 
{
	// TODO: Add your control notification handler code here
	m_mTestType =1;
}

void KSceneSwordTestWizard::OnRadioTest3() 
{
	// TODO: Add your control notification handler code here
	m_mTestType =2;
}

void KSceneSwordTestWizard::OnRadioTest4() 
{
	// TODO: Add your control notification handler code here
	m_mTestType =3;
}

void KSceneSwordTestWizard::OnButtonFileAdd() 
{
	// TODO: Add your control notification handler code here
	char bigBuff[8000] = "";  // maximum common dialog buffer size
	char szFilter[] ="模型文件(*.Mesh)|*.Mesh|";

	CFileDialog dlg(true, NULL, NULL,
		OFN_HIDEREADONLY|OFN_ALLOWMULTISELECT , szFilter);
	// Modify OPENFILENAME members directly to point to bigBuff
	dlg.m_ofn.lpstrFile = bigBuff;
	dlg.m_ofn.nMaxFile = sizeof(bigBuff);

	if (dlg.DoModal() == IDOK)
	{
		POSITION ps = dlg.GetStartPosition(); 
		while(ps != NULL)
		{
			CString Vs = dlg.GetNextPathName(ps);
			TCHAR path_buffer[256];
			TCHAR drive[256];
			TCHAR dir[256];
			TCHAR fname[256];
			TCHAR ext[256];

			wsprintf(path_buffer,"%s",Vs);
			_splitpath( path_buffer, drive, dir, fname, ext );

			//string str = Vs;
			m_vecFileName.push_back(Vs);

			m_ListBoxFileName.AddString(fname);
		}
	}
}

void KSceneSwordTestWizard::OnButtonFileRemove() 
{
	// TODO: Add your control notification handler code here
	
}
