// KSceneSceneEditorDialogGroundTextureUV.cpp : implementation file
//

#include "stdafx.h"
#include "SceneEditor.h"
#include "KSceneSceneEditorDialogGroundTextureUV.h"
#include "IEKG3DTerrain.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

////////////////////////////////////////////////////////////////////////////////

// KSceneSceneEditorDialogGroundTextureUV dialog

IMPLEMENT_DYNAMIC(KSceneSceneEditorDialogGroundTextureUV, CDialog)

KSceneSceneEditorDialogGroundTextureUV::KSceneSceneEditorDialogGroundTextureUV(CWnd* pParent /*=NULL*/)
	: CDialog(KSceneSceneEditorDialogGroundTextureUV::IDD, pParent)
	, m_fXZAngle(0)
	, m_fYAngle(0)
	, m_fScaleU(0)
	, m_fScaleV(0)
{
	m_pTerrain = NULL;
}

KSceneSceneEditorDialogGroundTextureUV::~KSceneSceneEditorDialogGroundTextureUV()
{
}

void KSceneSceneEditorDialogGroundTextureUV::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_XZANGLE, m_fXZAngle);
	DDX_Text(pDX, IDC_EDIT_YANGLE, m_fYAngle);
	DDX_Text(pDX, IDC_EDIT_SCALEU, m_fScaleU);
	DDX_Text(pDX, IDC_EDIT_SCALEV, m_fScaleV);
}


BEGIN_MESSAGE_MAP(KSceneSceneEditorDialogGroundTextureUV, CDialog)
	ON_EN_CHANGE(IDC_EDIT_XZANGLE, &KSceneSceneEditorDialogGroundTextureUV::OnEnChangeEditXzangle)
	ON_WM_SHOWWINDOW()
	ON_EN_CHANGE(IDC_EDIT_YANGLE, &KSceneSceneEditorDialogGroundTextureUV::OnEnChangeEditYangle)
	ON_EN_CHANGE(IDC_EDIT_SCALEU, &KSceneSceneEditorDialogGroundTextureUV::OnEnChangeEditScaleu)
	ON_EN_CHANGE(IDC_EDIT_SCALEV, &KSceneSceneEditorDialogGroundTextureUV::OnEnChangeEditScalev)
END_MESSAGE_MAP()


// KSceneSceneEditorDialogGroundTextureUV message handlers

void KSceneSceneEditorDialogGroundTextureUV::OnEnChangeEditXzangle()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
	if(!m_pTerrain)
		return;
	UpdateData(TRUE);

	m_pTerrain->SetGroundTextureUV(m_nLayer,m_nIndex,m_fXZAngle,m_fYAngle,m_fScaleU,m_fScaleV);
}

void KSceneSceneEditorDialogGroundTextureUV::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CDialog::OnShowWindow(bShow, nStatus);

	// TODO: Add your message handler code here
	if(!m_pTerrain)
		return;
	m_pTerrain->GetGroundTextureUV(m_nLayer,m_nIndex,
		&m_fXZAngle,&m_fYAngle,&m_fScaleU,&m_fScaleV);
	UpdateData(FALSE);
}

void KSceneSceneEditorDialogGroundTextureUV::OnEnChangeEditYangle()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
	if(!m_pTerrain)
		return;
	UpdateData(TRUE);
	m_pTerrain->SetGroundTextureUV(m_nLayer,m_nIndex,m_fXZAngle,m_fYAngle,m_fScaleU,m_fScaleV);
}

void KSceneSceneEditorDialogGroundTextureUV::OnEnChangeEditScaleu()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
	if(!m_pTerrain)
		return;
	UpdateData(TRUE);
	m_pTerrain->SetGroundTextureUV(m_nLayer,m_nIndex,m_fXZAngle,m_fYAngle,m_fScaleU,m_fScaleV);

}

void KSceneSceneEditorDialogGroundTextureUV::OnEnChangeEditScalev()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
	if(!m_pTerrain)
		return;
	UpdateData(TRUE);
	m_pTerrain->SetGroundTextureUV(m_nLayer,m_nIndex,m_fXZAngle,m_fYAngle,m_fScaleU,m_fScaleV);
}
