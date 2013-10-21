// KSceneModelEditorPropertySheet.cpp : implementation file
//

#include "stdafx.h"
#include "sceneeditor.h"
#include "KSceneModelEditorPropertySheet.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// KSceneModelEditorPropertySheet

IMPLEMENT_DYNAMIC(KSceneModelEditorPropertySheet, CPropertySheet)

KSceneModelEditorPropertySheet::KSceneModelEditorPropertySheet(CWnd* pParentWnd)
	:CPropertySheet(AFX_IDS_APP_TITLE,pParentWnd)
{
	AddPage(&m_PropertyPageMesh);
	AddPage(&m_PropertyPageMaterial);
	AddPage(&m_PropertyPageAnimation);
	AddPage(&m_PropertyPageMisc);
}

KSceneModelEditorPropertySheet::~KSceneModelEditorPropertySheet()
{
}


BEGIN_MESSAGE_MAP(KSceneModelEditorPropertySheet, CPropertySheet)
	//{{AFX_MSG_MAP(KSceneModelEditorPropertySheet)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// KSceneModelEditorPropertySheet message handlers
