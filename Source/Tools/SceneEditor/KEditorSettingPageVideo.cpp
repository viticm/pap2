// KEditorSettingPageVideo.cpp : implementation file
//

#include "stdafx.h"
#include "sceneeditor.h"
#include "KEditorSettingPageVideo.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// KEditorSettingPageVideo property page

IMPLEMENT_DYNCREATE(KEditorSettingPageVideo, CPropertyPage)

KEditorSettingPageVideo::KEditorSettingPageVideo() : CPropertyPage(KEditorSettingPageVideo::IDD)
{
	//{{AFX_DATA_INIT(KEditorSettingPageVideo)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

KEditorSettingPageVideo::~KEditorSettingPageVideo()
{
}

void KEditorSettingPageVideo::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(KEditorSettingPageVideo)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(KEditorSettingPageVideo, CPropertyPage)
	//{{AFX_MSG_MAP(KEditorSettingPageVideo)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// KEditorSettingPageVideo message handlers
