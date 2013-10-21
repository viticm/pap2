// KSceneObjectEditorPropertySheet.cpp : implementation file
//

#include "stdafx.h"
#include "sceneeditor.h"
#include "KSceneObjectEditorPropertySheet.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// KSceneObjectEditorPropertySheet

IMPLEMENT_DYNAMIC(KSceneObjectEditorPropertySheet, CPropertySheet)

KSceneObjectEditorPropertySheet::KSceneObjectEditorPropertySheet(CWnd* pParentWnd)
	:CPropertySheet(AFX_IDS_APP_TITLE, pParentWnd)
{
	m_lpSceneObjectEditorDoc = NULL;
	//AddPage(&m_PropertyPageModel);
	//AddPage(&m_PropertyPageBlock);
    //AddPage(&m_ObjectManagerSheet);
}


KSceneObjectEditorPropertySheet::~KSceneObjectEditorPropertySheet()
{
}


BEGIN_MESSAGE_MAP(KSceneObjectEditorPropertySheet, CPropertySheet)
	//{{AFX_MSG_MAP(KSceneObjectEditorPropertySheet)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// KSceneObjectEditorPropertySheet message handlers
