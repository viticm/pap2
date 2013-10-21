// KEditorSettingPageView.cpp : implementation file
//

#include "stdafx.h"
#include "sceneeditor.h"
#include "KEditorSettingPageView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// KEditorSettingPageView property page

IMPLEMENT_DYNCREATE(KEditorSettingPageView, CPropertyPage)

KEditorSettingPageView::KEditorSettingPageView() : CPropertyPage(KEditorSettingPageView::IDD)
{
	//{{AFX_DATA_INIT(KEditorSettingPageView)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

KEditorSettingPageView::~KEditorSettingPageView()
{
}

void KEditorSettingPageView::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(KEditorSettingPageView)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(KEditorSettingPageView, CPropertyPage)
	//{{AFX_MSG_MAP(KEditorSettingPageView)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// KEditorSettingPageView message handlers
