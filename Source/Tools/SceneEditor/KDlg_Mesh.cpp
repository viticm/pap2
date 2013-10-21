// KDlg_Mesh.cpp : implementation file
//

#include "stdafx.h"
#include "sceneeditor.h"
#include "KDlg_Mesh.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// KDlg_Mesh

IMPLEMENT_DYNAMIC(KDlg_Mesh, CPropertySheet)

KDlg_Mesh::KDlg_Mesh(UINT nIDCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CPropertySheet(nIDCaption, pParentWnd, iSelectPage)
{
}

KDlg_Mesh::KDlg_Mesh(LPCTSTR pszCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CPropertySheet(pszCaption, pParentWnd, iSelectPage)
{
}

KDlg_Mesh::~KDlg_Mesh()
{
}


BEGIN_MESSAGE_MAP(KDlg_Mesh, CPropertySheet)
	//{{AFX_MSG_MAP(KDlg_Mesh)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// KDlg_Mesh message handlers
