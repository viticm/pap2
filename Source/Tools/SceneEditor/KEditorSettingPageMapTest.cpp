// KEditorSettingPageMapTest.cpp : implementation file
//

#include "stdafx.h"
#include "sceneeditor.h"
#include "KEditorSettingPageMapTest.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// KEditorSettingPageMapTest property page

IMPLEMENT_DYNCREATE(KEditorSettingPageMapTest, CPropertyPage)

KEditorSettingPageMapTest::KEditorSettingPageMapTest() : CPropertyPage(KEditorSettingPageMapTest::IDD)
{
	//{{AFX_DATA_INIT(KEditorSettingPageMapTest)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

KEditorSettingPageMapTest::~KEditorSettingPageMapTest()
{
}

void KEditorSettingPageMapTest::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(KEditorSettingPageMapTest)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(KEditorSettingPageMapTest, CPropertyPage)
	//{{AFX_MSG_MAP(KEditorSettingPageMapTest)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// KEditorSettingPageMapTest message handlers
