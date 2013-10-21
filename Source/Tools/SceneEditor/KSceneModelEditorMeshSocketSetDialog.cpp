// KSceneModelEditorMeshSocketSetDialog.cpp : implementation file
//

#include "stdafx.h"
#include "SceneEditor.h"
#include "KSceneModelEditorMeshSocketSetDialog.h"
#include ".\kscenemodeleditormeshsocketsetdialog.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

////////////////////////////////////////////////////////////////////////////////


// KSceneModelEditorMeshSocketSetDialog dialog

IMPLEMENT_DYNAMIC(KSceneModelEditorMeshSocketSetDialog, CDialog)
KSceneModelEditorMeshSocketSetDialog::KSceneModelEditorMeshSocketSetDialog(CWnd* pParent /*=NULL*/)
	: CDialog(KSceneModelEditorMeshSocketSetDialog::IDD, pParent)
{
	/*m_lpSocket = NULL;
	m_lpModelEditor = NULL;*/
	m_bMoving = FALSE;
	m_bRotation = FALSE;
}

KSceneModelEditorMeshSocketSetDialog::~KSceneModelEditorMeshSocketSetDialog()
{
	//m_lpSocket = NULL;
	//m_lpModelEditor = NULL;
}

void KSceneModelEditorMeshSocketSetDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	//{{AFX_DATA_MAP(KSceneModelEditorPropertyPageAnimation)
	DDX_Control(pDX, IDC_SLIDER_Y, m_SliderCtrl_Y);
	DDX_Control(pDX, IDC_SLIDER_P, m_SliderCtrl_P);
	DDX_Control(pDX, IDC_SLIDER_R, m_SliderCtrl_R);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(KSceneModelEditorMeshSocketSetDialog, CDialog)
	ON_BN_CLICKED(IDC_BUTTON_EDIT, OnBnClickedButtonEdit)
	ON_WM_TIMER()
	ON_WM_SHOWWINDOW()
	ON_BN_CLICKED(IDC_BUTTON_EDITROTATION, OnBnClickedButtonEditrotation)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_SLIDER_Y, OnNMCustomdrawSliderY)
	ON_WM_HSCROLL()
END_MESSAGE_MAP()

// KSceneModelEditorMeshSocketSetDialog message handlers

void KSceneModelEditorMeshSocketSetDialog::OnBnClickedButtonEdit()
{
	// TODO: Add your control notification handler code here
	//if(!m_lpModelEditor)
	//	return;

	//if(!m_lpSocket)
	//	return;

	//m_bMoving = !m_bMoving;

	//if(m_bMoving)
	//{
	//	m_listEntityRender = m_lpModelEditor->m_RenderEntityList;
	//	m_listEntitySeletable = m_lpModelEditor->m_SelectableList;
	//	m_listEntitySeleted = m_lpModelEditor->m_SelectedList;

	//	m_lpModelEditor->m_SelectableList.clear();
	//	m_lpModelEditor->m_SelectedList.clear();

	//	m_lpSocket->GetCurPosition();

	//	m_lpModelEditor->AddRenderEntity(SCENEENTITY_POSITION,&m_lpSocket->CurPosition);
	//	m_lpModelEditor->AddSelectableEntity(SCENEENTITY_POSITION,&m_lpSocket->CurPosition);
	//	m_lpModelEditor->AddSelectedEntity(SCENEENTITY_POSITION,&m_lpSocket->CurPosition);

	//	m_lpModelEditor->m_dwState = EDITSTATE_MOVE;
	//}
	//else
	//{
	//	m_lpModelEditor->m_RenderEntityList = m_listEntityRender;
	//	m_lpModelEditor->m_SelectableList = m_listEntitySeletable;
	//	m_lpModelEditor->m_SelectedList = m_listEntitySeleted;

	//	m_lpModelEditor->m_dwState = EDITSTATE_SELECT;
	//}


}

void KSceneModelEditorMeshSocketSetDialog::OnTimer(UINT nIDEvent)
{
	// TODO: Add your message handler code here and/or call default
	//if(!m_lpModelEditor)
	//	return;

	//if(!m_lpSocket)
	//	return;

	//if(m_bMoving)
	//{
	//	m_lpSocket->CurMatrix._41 = m_lpSocket->CurPosition.x;
	//	m_lpSocket->CurMatrix._42 = m_lpSocket->CurPosition.y;
	//	m_lpSocket->CurMatrix._43 = m_lpSocket->CurPosition.z;

	//	m_lpSocket->MatrixOffset._41 = m_lpSocket->CurPosition.x - m_lpSocket->MatrixBase._41;
	//	m_lpSocket->MatrixOffset._42 = m_lpSocket->CurPosition.y - m_lpSocket->MatrixBase._42;
	//	m_lpSocket->MatrixOffset._43 = m_lpSocket->CurPosition.z - m_lpSocket->MatrixBase._43;
	//}

	//if(m_bRotation)
	//{
	//	D3DXMATRIX Rot,RotBase;

	//	D3DXMatrixRotationYawPitchRoll(&Rot,
	//		m_lpSocket->YawPitchRoll.x,
	//		m_lpSocket->YawPitchRoll.y,
	//		m_lpSocket->YawPitchRoll.z);

	//	/*D3DXMatrixLookAtLH(&Rot,
	//		&m_lpSocket->CurPosition,
	//		&m_lpSocket->CurDirectionPosition,
	//		&D3DXVECTOR3(0,1,0));
	//	Rot._41 = 0;
	//	Rot._42 = 0;
	//	Rot._43 = 0;*/

	//	RotBase = m_lpSocket->MatrixOffsetBack;
	//	RotBase._41 = 0;
	//	RotBase._42 = 0;
	//	RotBase._43 = 0;
	//	float XX = m_lpSocket->MatrixOffset._41;
	//	float YY = m_lpSocket->MatrixOffset._42;
	//	float ZZ = m_lpSocket->MatrixOffset._43;

	//	//D3DXMatrixInverse(&RotBase,NULL,&RotBase);

	//	m_lpSocket->MatrixOffset = RotBase * Rot;
	//
	//	m_lpSocket->MatrixOffset._41 = XX;
	//	m_lpSocket->MatrixOffset._42 = YY;
	//	m_lpSocket->MatrixOffset._43 = ZZ;
	//}

	CDialog::OnTimer(nIDEvent);
}

void KSceneModelEditorMeshSocketSetDialog::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CDialog::OnShowWindow(bShow, nStatus);
	/*SetTimer(0,10,0);

	m_SliderCtrl_Y.SetRange(0,360);
	m_SliderCtrl_P.SetRange(0,360);
	m_SliderCtrl_R.SetRange(0,360);

	m_SliderCtrl_Y.SetPos(0);
	m_SliderCtrl_P.SetPos(0);
	m_SliderCtrl_R.SetPos(0);*/
	// TODO: Add your message handler code here
}

void KSceneModelEditorMeshSocketSetDialog::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	/*if(!m_lpModelEditor)
		return;

	if(!m_lpSocket)
		return;
	m_lpSocket->YawPitchRoll.x = m_SliderCtrl_Y.GetPos() * 0.5f/D3DX_PI;
	m_lpSocket->YawPitchRoll.y = m_SliderCtrl_P.GetPos() * 0.5f/D3DX_PI;
	m_lpSocket->YawPitchRoll.z = m_SliderCtrl_R.GetPos() * 0.5f/D3DX_PI;

	m_bRotation = TRUE;*/
	CheckDlgButton(IDC_CHECK_ROTATE,m_bRotation);
}
void KSceneModelEditorMeshSocketSetDialog::OnBnClickedButtonEditrotation()
{
	// TODO: Add your control notification handler code here
	//if(!m_lpModelEditor)
	//	return;

	//if(!m_lpSocket)
	//	return;

	//m_bRotation = !m_bRotation;

	///*if(m_bRotation)
	//{
	//	m_listEntityRender = m_lpModelEditor->m_RenderEntityList;
	//	m_listEntitySeletable = m_lpModelEditor->m_SelectableList;
	//	m_listEntitySeleted = m_lpModelEditor->m_SelectedList;

	//	m_lpModelEditor->m_SelectableList.clear();
	//	m_lpModelEditor->m_SelectedList.clear();

	//	m_lpSocket->GetCurDirectionPosition();

	//	m_lpModelEditor->AddRenderEntity(SCENEENTITY_POSITION,&m_lpSocket->CurDirectionPosition);
	//	m_lpModelEditor->AddSelectableEntity(SCENEENTITY_POSITION,&m_lpSocket->CurDirectionPosition);
	//	m_lpModelEditor->AddSelectedEntity(SCENEENTITY_POSITION,&m_lpSocket->CurDirectionPosition);

	//	m_lpModelEditor->m_dwState = EDITSTATE_MOVE;
	//}
	//else
	//{
	//	m_lpModelEditor->m_RenderEntityList = m_listEntityRender;
	//	m_lpModelEditor->m_SelectableList = m_listEntitySeletable;
	//	m_lpModelEditor->m_SelectedList = m_listEntitySeleted;

	//	m_lpModelEditor->m_dwState = EDITSTATE_SELECT;
	//}*/
}

void KSceneModelEditorMeshSocketSetDialog::OnNMCustomdrawSliderY(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
	// TODO: Add your control notification handler code here
	*pResult = 0;
}
