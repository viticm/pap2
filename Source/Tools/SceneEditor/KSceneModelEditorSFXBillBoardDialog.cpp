// KSceneModelEditorSFXBillBoardDialog.cpp : implementation file
//

#include "stdafx.h"
#include "sceneeditor.h"
#include "KSceneModelEditorSFXBillBoardDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// KSceneModelEditorSFXBillBoardDialog dialog


KSceneModelEditorSFXBillBoardDialog::KSceneModelEditorSFXBillBoardDialog(CWnd* pParent /*=NULL*/)
	: CDialog(KSceneModelEditorSFXBillBoardDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(KSceneModelEditorSFXBillBoardDialog)
	m_nNumSprite = 0;
	m_scTextureName = _T("");
	m_fAnimationLength = 0.0f;
	m_bZBuffer = FALSE;
	m_bSort = FALSE;
	m_fSpriteSize = 0.0f;
	m_bPositioning = FALSE;
	m_fBeta = 0.0f;
	m_fGama = 0.0f;
	m_fRadiu = 0.0f;
	m_fSpeed = 0.0f;
	//}}AFX_DATA_INIT
	//m_lpModelSFXBillBoard = NULL;
	//m_lpSceneModelEditor = NULL;
}


void KSceneModelEditorSFXBillBoardDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(KSceneModelEditorSFXBillBoardDialog)
	DDX_Control(pDX, IDC_COMBO_MOTIONTYPE, m_ComboMotionType);
	DDX_Control(pDX, IDC_COMBO_TYPE, m_ComType);
	DDX_Control(pDX, IDC_SPIN, m_Spin_NumSprite);
	DDX_Control(pDX, IDC_LIST_SPRITE, m_ListBoxSprite);
	DDX_Text(pDX, IDC_EDIT_NUMSPRITE, m_nNumSprite);
	DDV_MinMaxInt(pDX, m_nNumSprite, 1, 100);
	DDX_Text(pDX, IDC_EDIT_TEXTURENAME, m_scTextureName);
	DDX_Text(pDX, IDC_EDIT_ANIMATIONLENGTH, m_fAnimationLength);
	DDX_Check(pDX, IDC_CHECK_ZBUFFER, m_bZBuffer);
	DDX_Check(pDX, IDC_CHECK_SORT, m_bSort);
	DDX_Text(pDX, IDC_EDIT_SIZE, m_fSpriteSize);
	DDV_MinMaxFloat(pDX, m_fSpriteSize, 0.f, 1000.f);
	DDX_Check(pDX, IDC_CHECK_POSITION, m_bPositioning);
	DDX_Text(pDX, IDC_EDIT_BETA, m_fBeta);
	DDX_Text(pDX, IDC_EDIT_GAMA, m_fGama);
	DDX_Text(pDX, IDC_EDIT_RADIU, m_fRadiu);
	DDX_Text(pDX, IDC_EDIT_SPEED, m_fSpeed);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(KSceneModelEditorSFXBillBoardDialog, CDialog)
	//{{AFX_MSG_MAP(KSceneModelEditorSFXBillBoardDialog)
	ON_EN_CHANGE(IDC_EDIT_NUMSPRITE, OnChangeEditNumsprite)
	ON_WM_VSCROLL()
	ON_LBN_SELCHANGE(IDC_LIST_SPRITE, OnSelchangeListSprite)
	ON_EN_CHANGE(IDC_EDIT_SIZE, OnChangeEditSize)
	ON_BN_CLICKED(IDC_CHECK_POSITION, OnCheckPosition)
	ON_CBN_SELCHANGE(IDC_COMBO_MOTIONTYPE, OnSelchangeComboMotiontype)
	ON_EN_CHANGE(IDC_EDIT_RADIU, OnChangeEditRadiu)
	ON_EN_CHANGE(IDC_EDIT_SPEED, OnChangeEditSpeed)
	ON_EN_CHANGE(IDC_EDIT_BETA, OnChangeEditBeta)
	ON_EN_CHANGE(IDC_EDIT_GAMA, OnChangeEditGama)
	ON_BN_CLICKED(IDC_CHECK_SORT, OnCheckSort)
	//}}AFX_MSG_MAP
	ON_CBN_SELCHANGE(IDC_COMBO_TYPE, OnCbnSelchangeComboType)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// KSceneModelEditorSFXBillBoardDialog message handlers

BOOL KSceneModelEditorSFXBillBoardDialog::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	/*if(!m_lpModelSFXBillBoard)
		return TRUE;*/

	//m_nNumSprite = m_lpModelSFXBillBoard->m_nNumSprite;
	
	m_Spin_NumSprite.SetBuddy( GetDlgItem(IDC_EDIT_NUMSPRITE) );
	m_Spin_NumSprite.SetRange32(1,100);
	m_Spin_NumSprite.SetPos32(1);

	UpdateData(FALSE);

	m_ComType.AddString("普通公告版");
	m_ComType.AddString("竖直公告版");
	m_ComType.AddString("水平公告版");
	m_ComType.AddString("骨骼节点公告版");

	m_ComType.SetCurSel(0);

	m_ComboMotionType.AddString("静止");
	m_ComboMotionType.AddString("转动");

	m_ComboMotionType.SetCurSel(0);
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void KSceneModelEditorSFXBillBoardDialog::FillListBoxSprite()
{
	{
		m_ListBoxSprite.SetCurSel(-1);
		int Count = m_ListBoxSprite.GetCount();
		for(int i=Count+1;i>=0;i--)
		{
			m_ListBoxSprite.DeleteString(i);
		}
	}

	/*if(!m_lpModelSFXBillBoard)
		return ;

	TCHAR Name[256];
	for(int i=0;i<m_lpModelSFXBillBoard->m_nNumSpriteBack;i++)
	{
		wsprintf(Name,"公告板%d",i+1);
		m_ListBoxSprite.AddString(Name);
	}*/
}

void KSceneModelEditorSFXBillBoardDialog::OnChangeEditNumsprite() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here
	/*if(!m_lpModelSFXBillBoard)
		return ;

	UpdateData(TRUE);

	m_lpModelSFXBillBoard->m_nNumSpriteBack = m_nNumSprite;
	m_Spin_NumSprite.SetPos32(m_nNumSprite);

	FillListBoxSprite();*/
}

void KSceneModelEditorSFXBillBoardDialog::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	// TODO: Add your message handler code here and/or call default
	/*if(!m_lpModelSFXBillBoard)
		return ;

	m_nNumSprite = m_Spin_NumSprite.GetPos32() ;
	UpdateData(FALSE);

	m_lpModelSFXBillBoard->m_nNumSpriteBack = m_nNumSprite;

	FillListBoxSprite();*/
	CDialog::OnVScroll(nSBCode, nPos, pScrollBar);
}

void KSceneModelEditorSFXBillBoardDialog::OnSelchangeListSprite() 
{
	// TODO: Add your control notification handler code here
	/*if(!m_lpModelSFXBillBoard)
		return ;
	if(!m_lpSceneModelEditor)
		return;

	if(m_bPositioning)
	{
		m_lpSceneModelEditor->m_SelectedList.clear();
	}

	if(m_ListBoxSprite.GetSelCount()==0)
	{
		GetDlgItem(IDC_EDIT_SIZE)->EnableWindow(FALSE);
		GetDlgItem(IDC_EDIT_RADIU)->EnableWindow(FALSE);
		GetDlgItem(IDC_EDIT_SPEED)->EnableWindow(FALSE);
		GetDlgItem(IDC_EDIT_BETA)->EnableWindow(FALSE);
		GetDlgItem(IDC_EDIT_GAMA)->EnableWindow(FALSE);

		GetDlgItem(IDC_CHECK_POSITION)->EnableWindow(FALSE);
		
		m_lpModelSFXBillBoard->m_bSort = TRUE;
		m_bSort = TRUE;
		GetDlgItem(IDC_CHECK_SORT)->EnableWindow(TRUE);
		UpdateData(FALSE);
	}
	else
	{
		GetDlgItem(IDC_EDIT_SIZE)->EnableWindow(TRUE);
		GetDlgItem(IDC_EDIT_RADIU)->EnableWindow(TRUE);
		GetDlgItem(IDC_EDIT_SPEED)->EnableWindow(TRUE);
		GetDlgItem(IDC_EDIT_BETA)->EnableWindow(TRUE);
		GetDlgItem(IDC_EDIT_GAMA)->EnableWindow(TRUE);
		GetDlgItem(IDC_CHECK_POSITION)->EnableWindow(TRUE);

		m_lpModelSFXBillBoard->m_bSort = FALSE;
		m_bSort = FALSE;
		GetDlgItem(IDC_CHECK_SORT)->EnableWindow(FALSE);

		int K = 0;
		for(int i=0;i<m_nNumSprite;i++)
		{
			ModelSFX::KModelSFXBillBoard::_Sprite* pSprite = 
				m_lpModelSFXBillBoard->m_lpSprite[i];
			
			if(m_ListBoxSprite.GetSel(i))
			{
				if(K==0)
				{
					m_fSpriteSize = pSprite->Width;

					m_ComboMotionType.SetCurSel(pSprite->MotionType);

					m_fRadiu = pSprite->Arg[0];
					m_fSpeed = pSprite->Arg[1];
					m_fBeta  = pSprite->Arg[2] * 180.0f / D3DX_PI;
					m_fGama  = pSprite->Arg[3] * 180.0f / D3DX_PI;

					K++;
				}
				KSceneEntity Entity(SCENEENTITY_POSITION,&pSprite->Position);
				if(m_bPositioning)
				{
					m_lpSceneModelEditor->m_SelectedList.push_back(Entity);
				}	
			}

			UpdateData(FALSE);
		}
	}*/
}

void KSceneModelEditorSFXBillBoardDialog::OnChangeEditSize() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
	/*if(!m_lpModelSFXBillBoard)
		return ;

	for(int i=0;i<m_nNumSprite;i++)
	{
		ModelSFX::KModelSFXBillBoard::_Sprite* pSprite = 
			m_lpModelSFXBillBoard->m_lpSprite[i];
		if(m_ListBoxSprite.GetSel(i))
		{
			pSprite->Width = m_fSpriteSize;
		}
	}*/
}

void KSceneModelEditorSFXBillBoardDialog::OnCheckPosition() 
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);

	/*if(!m_lpSceneModelEditor)
		return;

	if(m_bPositioning)
	{
		m_listEntityRender = m_lpSceneModelEditor->m_RenderEntityList;
		m_listEntitySeletable = m_lpSceneModelEditor->m_SelectableList;
		m_listEntitySeleted = m_lpSceneModelEditor->m_SelectedList;

		m_lpSceneModelEditor->m_SelectableList.clear();
		m_lpSceneModelEditor->m_SelectedList.clear();

		for(int i=0;i<m_nNumSprite;i++)
		{
			ModelSFX::KModelSFXBillBoard::_Sprite* pSprite = 
				m_lpModelSFXBillBoard->m_lpSprite[i];
	
			KSceneEntity Entity(SCENEENTITY_POSITION,&pSprite->Position);
			m_lpSceneModelEditor->m_RenderEntityList.push_back(Entity);
			m_lpSceneModelEditor->m_SelectableList.push_back(Entity);
		
			if(m_ListBoxSprite.GetSel(i))
			{
				m_lpSceneModelEditor->m_SelectedList.push_back(Entity);

			}
		}
		m_lpSceneModelEditor->m_dwState = EDITSTATE_MOVE;
	}
	else
	{
		m_lpSceneModelEditor->m_RenderEntityList = m_listEntityRender;
		m_lpSceneModelEditor->m_SelectableList = m_listEntitySeletable;
		m_lpSceneModelEditor->m_SelectedList = m_listEntitySeleted;
		
		m_lpSceneModelEditor->m_dwState = EDITSTATE_SELECT;

		m_lpSceneModelEditor->SetCurMesh(m_lpModelSFXBillBoard);
	}*/
}

void KSceneModelEditorSFXBillBoardDialog::OnSelchangeComboMotiontype() 
{
	// TODO: Add your control notification handler code here
	/*if(!m_lpModelSFXBillBoard)
		return ;

	int Motion = m_ComboMotionType.GetCurSel();

	for(int i=0;i<m_nNumSprite;i++)
	{
		ModelSFX::KModelSFXBillBoard::_Sprite* pSprite = 
			m_lpModelSFXBillBoard->m_lpSprite[i];
		
		if(m_ListBoxSprite.GetSel(i))
		{
			pSprite->MotionType = Motion;
		}
	}*/

}

void KSceneModelEditorSFXBillBoardDialog::OnChangeEditRadiu() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);

	/*if(!m_lpModelSFXBillBoard)
		return ;

	int Motion = m_ComboMotionType.GetCurSel();
	if(Motion!=1)
		return;

	for(int i=0;i<m_nNumSprite;i++)
	{
		ModelSFX::KModelSFXBillBoard::_Sprite* pSprite = 
			m_lpModelSFXBillBoard->m_lpSprite[i];

		if(m_ListBoxSprite.GetSel(i))
		{
			pSprite->Arg[0] = m_fRadiu;
		}
	}*/
}

void KSceneModelEditorSFXBillBoardDialog::OnChangeEditSpeed() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);

	/*if(!m_lpModelSFXBillBoard)
		return ;

	int Motion = m_ComboMotionType.GetCurSel();
	if(Motion!=1)
		return;

	for(int i=0;i<m_nNumSprite;i++)
	{
		ModelSFX::KModelSFXBillBoard::_Sprite* pSprite = 
			m_lpModelSFXBillBoard->m_lpSprite[i];

		if(m_ListBoxSprite.GetSel(i))
		{
			pSprite->Arg[1] = m_fSpeed;
		}
	}*/
}

void KSceneModelEditorSFXBillBoardDialog::OnChangeEditBeta() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);

	/*if(!m_lpModelSFXBillBoard)
		return ;

	int Motion = m_ComboMotionType.GetCurSel();
	if(Motion!=1)
		return;

	for(int i=0;i<m_nNumSprite;i++)
	{
		ModelSFX::KModelSFXBillBoard::_Sprite* pSprite = 
			m_lpModelSFXBillBoard->m_lpSprite[i];

		if(m_ListBoxSprite.GetSel(i))
		{
			pSprite->Arg[2] = m_fBeta * D3DX_PI / 180.0f;
		}
	}*/
}

void KSceneModelEditorSFXBillBoardDialog::OnChangeEditGama() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);

	/*if(!m_lpModelSFXBillBoard)
		return ;

	int Motion = m_ComboMotionType.GetCurSel();
	if(Motion!=1)
		return;

	for(int i=0;i<m_nNumSprite;i++)
	{
		ModelSFX::KModelSFXBillBoard::_Sprite* pSprite = 
			m_lpModelSFXBillBoard->m_lpSprite[i];

		if(m_ListBoxSprite.GetSel(i))
		{
			pSprite->Arg[3] = m_fGama * D3DX_PI / 180.0f;
		}
	}	*/
}

void KSceneModelEditorSFXBillBoardDialog::OnCheckSort() 
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
	/*if(!m_lpModelSFXBillBoard)
		return ;
	m_lpModelSFXBillBoard->m_bSort = m_bSort;*/


}

void KSceneModelEditorSFXBillBoardDialog::OnCbnSelchangeComboType()
{
	// TODO: Add your control notification handler code here
}
