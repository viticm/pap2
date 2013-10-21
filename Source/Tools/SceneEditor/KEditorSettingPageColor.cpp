// KEditorSettingPageColor.cpp : implementation file
//

#include "stdafx.h"
#include "sceneeditor.h"
#include "KEditorSettingPageColor.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// KEditorSettingPageColor property page

IMPLEMENT_DYNCREATE(KEditorSettingPageColor, CPropertyPage)

KEditorSettingPageColor::KEditorSettingPageColor() : CPropertyPage(KEditorSettingPageColor::IDD)
{
	//{{AFX_DATA_INIT(KEditorSettingPageColor)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

KEditorSettingPageColor::~KEditorSettingPageColor()
{
}

void KEditorSettingPageColor::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(KEditorSettingPageColor)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(KEditorSettingPageColor, CPropertyPage)
	//{{AFX_MSG_MAP(KEditorSettingPageColor)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// KEditorSettingPageColor message handlers
