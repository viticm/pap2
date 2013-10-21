// KDlg_PropertyPage.cpp : implementation file
//

#include "stdafx.h"
#include "sceneeditor.h"
#include "KDlg_PropertyPage.h"
#include "./KDlg_ProItem_Int.h"
#include "./KDlg_ProItem_Bool.h"
#include "./KDlg_ProItem_Float.h"
#include "./KDlg_ProItem_String.h"
#include "KDlg_PorItem_STLString.h"
#include "KDlg_ProItem_Enum.h"
#include "KDlg_ProItem_Color.h"
#include "KDlg_ProItem_MtlList.h"
#include "KDlg_ProItem_MtlTex.h"
#include "KDlg_ProItem_MtlOption.h"
#include "KDlg_ProItem_MtlState.h"
#include "KDlg_ProItem_MtlEffect.h"
#include "KDlg_ProItem_Ani.h"
#include "KDlg_ProItem_SFXNew.h"
#include "KDlg_ProItem_Vec3.h"
#include "KDlg_ProItem_Particle.h"
#include "KDlg_ProItem_SceneObjects.h"
#include "KDlg_Timepointedit.h"
#include "./KDlg_ProItem_Terrain_Ground_Texture.h"
#include "KDlg_Timepoint_color.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// KDlg_PropertyPage dialog
namespace PropertyBase
{

KDlg_PropertyPage::KDlg_PropertyPage(CWnd* pParent /*=NULL*/)
	: CDialog(KDlg_PropertyPage::IDD, pParent)
{
	//{{AFX_DATA_INIT(KDlg_PropertyPage)
	//}}AFX_DATA_INIT
	m_lpProperPage = NULL;
	m_bScrollBarCreated = FALSE;
	m_bEnableScroll = FALSE;
	m_Top = 1;
	m_uControlID = CONTROL_START;
}

KDlg_PropertyPage::~KDlg_PropertyPage()
{
	//SAFE_DELETE(m_pStatic);
	for(DWORD i=0;i<m_vecGroup.size();i++)
	{
		_Group* pGroup = &m_vecGroup[i];
		pGroup->Clear();
	}
	m_vecGroup.size();
}

void KDlg_PropertyPage::SetPropertyPage(PropertyBase::_PropertyPage* pPage)
{
	m_lpProperPage = pPage;
}

void KDlg_PropertyPage::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(KDlg_PropertyPage)
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(KDlg_PropertyPage, CDialog)
	//{{AFX_MSG_MAP(KDlg_PropertyPage)
	ON_WM_MOVE()
	ON_WM_CLOSE()
	ON_WM_VSCROLL()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// KDlg_PropertyPage message handlers

BOOL KDlg_PropertyPage::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here

	CRect Rect(20,0,40,350);
	/*m_pStatic = new CStatic();
	m_pStatic->Create("Static Text",0,Rect,this);
	m_pStatic->ShowWindow(TRUE);*/

	m_Scroll.Create(SBS_VERT|WS_CHILD|WS_VISIBLE,Rect,this,SCORLLBAR);
	m_Scroll.ShowWindow(m_bEnableScroll);
	m_bScrollBarCreated = TRUE;
	m_Scroll.SetScrollRange(0,100);
	m_Scroll.SetScrollPos(0);

	if(m_lpProperPage)
	{
		for(DWORD i=0;i<m_lpProperPage->vecGroup.size();i++)
		{
			PropertyBase::_PropertyGroup* pGroup = 
				&m_lpProperPage->vecGroup[i];

			_Group NewGroup(i,pGroup,this);

			int K = (int)m_vecGroup.size();
			m_vecGroup.push_back(NewGroup);
			
			_Group* pGroupMe = &m_vecGroup[K];
			pGroupMe->CreateItems(pGroup,this);

			/*int Height = 150;
			CRect Rect(3,10 + Height * i,250,Height * (i+1));

			CStatic* pButton = new CStatic();
			pButton->Create(pGroup->scGroupName.c_str(),
				SS_BLACKFRAME|SS_LEFT,Rect,this,0);
			pButton->ShowWindow(TRUE);*/
		}
	}

	//RecalcLayout();
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void KDlg_PropertyPage::OnMove(int x, int y) 
{
	CDialog::OnMove(x, y);
	RecalcLayout();
	// TODO: Add your message handler code here
	CRect m_Rect;
	GetWindowRect(&m_Rect);
	m_PageHeight = m_Rect.Height();
	
	if(m_bScrollBarCreated)
	{
		CRect ScrollRect;
		ScrollRect.left = m_Rect.Width()-10;
		ScrollRect.right = m_Rect.Width();
		ScrollRect.top = 0;
		ScrollRect.bottom = m_Rect.Height();

		m_Scroll.MoveWindow(ScrollRect);
	}
}

UINT KDlg_PropertyPage::GetNewControlID()
{
	return m_uControlID++;
}

void KDlg_PropertyPage::OnClose() 
{
	// TODO: Add your message handler code here and/or call default
	for(DWORD i=0;i<m_vecGroup.size();i++)
	{
		_Group* pGroup = &m_vecGroup[i];
		pGroup->EnableWindow(FALSE);
		pGroup->ShowWindow(FALSE);
	}
	CDialog::OnClose();
}

void KDlg_PropertyPage::OnFinalRelease() 
{
	// TODO: Add your specialized code here and/or call the base class
	/*if(m_pStatic)
	{
		SAFE_DELETE(m_pStatic);
	}*/	
	CDialog::OnFinalRelease();
}

void KDlg_PropertyPage::RecalcLayout()
{
	int Top = m_Top;
	for(DWORD i=0;i<m_vecGroup.size();i++)
	{
		_Group* pGroup = &m_vecGroup[i];
		int End = pGroup->OnMove(Top,this);
		Top = End + 3;

	}
	BOOL bScroll = FALSE;
	CRect Rect;
	GetWindowRect(&Rect);
	m_TotalHeight = Top - m_Top;
	
	if(m_TotalHeight > Rect.Height())
	{
		bScroll = TRUE;
	}
	else
	{
		bScroll = FALSE;
		m_Top = 0;
	}

	if(bScroll!=m_bEnableScroll)
	{
		m_bEnableScroll = bScroll;
		m_Scroll.ShowWindow(m_bEnableScroll);
		
		int Top = m_Top;
		for(DWORD i=0;i<m_vecGroup.size();i++)
		{
			_Group* pGroup = &m_vecGroup[i];
			int End = pGroup->OnMove(Top,this);
			Top = End + 3;
		}
	}
	
}

LRESULT KDlg_PropertyPage::WindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
{
	// TODO: Add your specialized code here and/or call the base class
	if(message==WM_COMMAND)
	{
		int Index = (int)wParam;

		if((Index>=GROUP_START)&&(Index<GROUP_START+(int)m_vecGroup.size()))
		{
			_Group* pGroup = &m_vecGroup[Index-GROUP_START];

			int Check = pGroup->pButton->GetCheck();
			pGroup->pButton->SetCheck(!Check);

			
			RecalcLayout();
		}	
	}

	return CDialog::WindowProc(message, wParam, lParam);
}

void KDlg_PropertyPage::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	// TODO: Add your message handler code here and/or call default
	CDialog::OnVScroll(nSBCode, nPos, pScrollBar);

	if(SB_THUMBTRACK == nSBCode)
	{
		int ScrollPos = nPos;
		m_Scroll.SetScrollPos(ScrollPos);

		m_Top = (int)((m_PageHeight - m_TotalHeight)*ScrollPos*0.01);
		RecalcLayout();
	}
}

int _ProItemInput_Int::Create(int Top,int Width,PropertyBase::_VarProperty* pPro,KDlg_PropertyPage* pParent)
{
	switch(pPro->Type)
	{
	case PropertyBase::_VarProperty::VAR_INT:
		{
			PropertyBase::_VarProperty::_StoreInt* pStore = 
				(PropertyBase::_VarProperty::_StoreInt*)pPro->DateStore;

			KDlg_ProItem_Int* pDialog_Int = new KDlg_ProItem_Int((CWnd*)pParent);
			//pDialog_Int->SetStore(pPro,pStore);
			pDialog_Int->Create(IDD_PROITEM_INT,(CWnd*)pParent);
			pDialog_Int->ShowWindow(TRUE);

			m_lpDailog = pDialog_Int;
			break;
		}
	}
	return Top + 30;
}

int _ProItemInput_Bool::Create(int Top,int Width,PropertyBase::_VarProperty* pPro,KDlg_PropertyPage* pParent)
{
	switch(pPro->Type)
	{
	case PropertyBase::_VarProperty::VAR_BOOL:
		{
			PropertyBase::_VarProperty::_StoreBool* pStore = 
				(PropertyBase::_VarProperty::_StoreBool*)pPro->DateStore;

			KDlg_ProItem_Bool* pDialog_Bool = new KDlg_ProItem_Bool((CWnd*)pParent);
			//pDialog_Bool->SetStore(pPro,pStore);
			pDialog_Bool->Create(IDD_PROITEM_BOOL,(CWnd*)pParent);
			pDialog_Bool->ShowWindow(TRUE);

			m_lpDailog = pDialog_Bool;
			break;
		}
	}
	return Top + 30;
}
int _ProItemInput_Float::Create(int Top,int Width,PropertyBase::_VarProperty* pPro,KDlg_PropertyPage* pParent)
{
	switch(pPro->Type)
	{
	case PropertyBase::_VarProperty::VAR_FLOAT:
		{
			PropertyBase::_VarProperty::_StoreFloat* pStore = 
				(PropertyBase::_VarProperty::_StoreFloat*)pPro->DateStore;

			KDlg_ProItem_Float* pDialog = new KDlg_ProItem_Float((CWnd*)pParent);
			//pDialog->SetStore(pPro,pStore);
			pDialog->Create(IDD_PROITEM_FLOAT,(CWnd*)pParent);
			pDialog->ShowWindow(TRUE);

			m_lpDailog = pDialog;
			break;
		}
	}
	return Top + 30;
}
int _ProItemInput_Enum::Create(int Top,int Width,PropertyBase::_VarProperty* pPro,KDlg_PropertyPage* pParent)
{
	/*switch(pPro->Type)
	{
	case PropertyBase::_VarProperty::VAR_ENUM:
		{
			PropertyBase::_VarProperty::_StoreEnum* pStore = 
				(PropertyBase::_VarProperty::_StoreEnum*)pPro->DateStore;

			KDlg_ProItem_Enum* pDialog = new KDlg_ProItem_Enum((CWnd*)pParent);
			pDialog->SetStore(pPro,pStore);
			pDialog->Create(IDD_PROITEM_ENUM,(CWnd*)pParent);
			pDialog->ShowWindow(TRUE);

			m_lpDailog = pDialog;
			break;
		}
	}*/
	return Top + 30;
}
int _ProItemInput_String::Create(int Top,int Width,PropertyBase::_VarProperty* pPro,KDlg_PropertyPage* pParent)
{
	/*switch(pPro->Type)
	{
	case PropertyBase::_VarProperty::VAR_STRING:
	case PropertyBase::_VarProperty::VAR_STLSTRING:
		{
			KDlg_ProItem_String* pDialog = new KDlg_ProItem_String((CWnd*)pParent);
			pDialog->SetStore(pPro);
			pDialog->Create(IDD_PROITEM_STRING,(CWnd*)pParent);
			pDialog->ShowWindow(TRUE);

			m_lpDailog = pDialog;
			break;
		}
	}*/
	return Top + 30;
}

//////////////////////////////////////////////////////////////////////////
int _ProItemInput_TimePoint::Create(int Top,int Width,PropertyBase::_VarProperty* pPro,KDlg_PropertyPage* pParent)
{
	//switch(pPro->Type)
	//{
	//case PropertyBase::_VarProperty::VAR_TIMEPOINT:
	//	{
	//		/*KDlg_TimePointEdit* pDialog = new KDlg_TimePointEdit((CWnd*)pParent);
	//		pDialog->SetStore(pPro);
	//		pDialog->Create(IDD_TIMEPOINT_EDIT, (CWnd*)pParent);
	//		pDialog->ShowWindow(TRUE);
	//		m_lpDailog = pDialog;*/
	//		break;
	//	}
	//}
	return Top + 30;
}
//////////////////////////////////////////////////////////////////////////

int _ProItemInput_TimePoint_Color::Create(int Top,int Width,PropertyBase::_VarProperty* pPro,KDlg_PropertyPage* pParent)
{
	//switch(pPro->Type)
	//{
	//case PropertyBase::_VarProperty::VAR_TIMEPOINT_COLOR:
	//	{
	//		/*KDlg_TimePoint_Color* pDialog = new KDlg_TimePoint_Color((CWnd*)pParent);
	//		pDialog->SetStore(pPro);
	//		pDialog->Create(IDD_TIMEPOINT_COLOR, (CWnd*)pParent);
	//		pDialog->ShowWindow(TRUE);
	//		m_lpDailog = pDialog;*/
	//		break;
	//	}
	//}
	return Top + 30;

}
//////////////////////////////////////////////////////////////////////////

int _ProItemInput_Color::Create(int Top,int Width,PropertyBase::_VarProperty* pPro,KDlg_PropertyPage* pParent)
{
	/*switch(pPro->Type)
	{
	case PropertyBase::_VarProperty::VAR_COLOR:
	case PropertyBase::_VarProperty::VAR_COLORVALUE:
		{
			KDlg_ProItem_Color* pDialog = new KDlg_ProItem_Color((CWnd*)pParent);
			pDialog->SetStore(pPro);
			pDialog->Create(IDD_PROITEM_COLOR,(CWnd*)pParent);
			pDialog->ShowWindow(TRUE);

			m_lpDailog = pDialog;
			break;
		}
	}*/
	return Top + 30;
}


int _ProItemInput_MtlList::Create(int Top,int Width,PropertyBase::_VarProperty* pPro,KDlg_PropertyPage* pParent)
{
	switch(pPro->Type)
	{
	case PropertyBase::_VarProperty::VAR_MTLLIST:
		{
			KDlg_ProItem_MtlList* pDialog = new KDlg_ProItem_MtlList((CWnd*)pParent);
			//pDialog->SetStore(pPro,pParent);
			//pDialog->Create(IDD_PROITEM_MTLLIST,(CWnd*)pParent);
			//pDialog->ShowWindow(TRUE);

			m_lpDailog = pDialog;
			break;
		}
	}
	return Top + 30;
}

int _ProItemInput_MtlTex::Create(int Top,int Width,PropertyBase::_VarProperty* pPro,KDlg_PropertyPage* pParent)
{
	switch(pPro->Type)
	{
	case PropertyBase::_VarProperty::VAR_MTLTEX:
		{
			KDlg_ProItem_MtlTex* pDialog = new KDlg_ProItem_MtlTex((CWnd*)pParent);
			//pDialog->SetStore(pPro);
			pDialog->Create(IDD_PROITEM_MTLTEX,(CWnd*)pParent);
			pDialog->ShowWindow(TRUE);

			m_lpDailog = pDialog;
			break;
		}
	}
	return Top + 30;
}

int _ProItemInput_MtlOption::Create(int Top,int Width,PropertyBase::_VarProperty* pPro,KDlg_PropertyPage* pParent)
{
	switch(pPro->Type)
	{
	case PropertyBase::_VarProperty::VAR_MTLOPTOIN:
		{
			KDlg_ProItem_MtlOption* pDialog = new KDlg_ProItem_MtlOption((CWnd*)pParent);
			//pDialog->SetStore(pPro);
			pDialog->Create(IDD_PROITEM_MTLOPTION,(CWnd*)pParent);
			pDialog->ShowWindow(TRUE);

			m_lpDailog = pDialog;
			break;
		}
	}
	return Top + 30;
}

int _ProItemInput_MtlState::Create(int Top,int Width,PropertyBase::_VarProperty* pPro,KDlg_PropertyPage* pParent)
{
	switch(pPro->Type)
	{
	case PropertyBase::_VarProperty::VAR_MTLSTATE:
		{
			KDlg_ProItem_MtlState* pDialog = new KDlg_ProItem_MtlState((CWnd*)pParent);
			//pDialog->SetStore(pPro);
			pDialog->Create(IDD_PROITEM_MTLSTATE,(CWnd*)pParent);
			pDialog->ShowWindow(TRUE);

			m_lpDailog = pDialog;
			break;
		}
	}
	return Top + 30;
}

int _ProItemInput_MtlEffect::Create(int Top,int Width,PropertyBase::_VarProperty* pPro,KDlg_PropertyPage* pParent)
{
	switch(pPro->Type)
	{
	case PropertyBase::_VarProperty::VAR_MTLEFFECT:
		{
			KDlg_ProItem_MtlEffect* pDialog = new KDlg_ProItem_MtlEffect((CWnd*)pParent);
			//pDialog->SetStore(pPro);
			pDialog->Create(IDD_PROITEM_MTLEFFECT,(CWnd*)pParent);
			pDialog->ShowWindow(TRUE);

			m_lpDailog = pDialog;
			break;
		}
	}
	return Top + 30;
}

int _ProItemInput_Ani::Create(int Top,int Width,PropertyBase::_VarProperty* pPro,KDlg_PropertyPage* pParent)
{
	switch(pPro->Type)
	{
	case PropertyBase::_VarProperty::VAR_ANILIST:
		{
			KDlg_ProItem_Ani* pDialog = new KDlg_ProItem_Ani((CWnd*)pParent);
			//pDialog->SetStore(pPro);
			pDialog->Create(IDD_PROITEM_ANI,(CWnd*)pParent);
			pDialog->ShowWindow(TRUE);

			m_lpDailog = pDialog;
			break;
		}
	}
	return Top + 30;
}



int _ProItemInput_SFXNew::Create(int Top,int Width,PropertyBase::_VarProperty* pPro,KDlg_PropertyPage* pParent)
{
	switch(pPro->Type)
	{
	case PropertyBase::_VarProperty::VAR_MODELEDITORSFXNEW:
		{
			KDlg_ProItem_SFXNew* pDialog = new KDlg_ProItem_SFXNew((CWnd*)pParent);
			//pDialog->SetStore(pPro);
			pDialog->Create(IDD_PROITEM_SFXNEW,(CWnd*)pParent);
			pDialog->ShowWindow(TRUE);

			m_lpDailog = pDialog;
			break;
		}
	}
	return Top + 30;
}

int _ProItemInput_SceneObjectTable::Create(int Top,int Width,PropertyBase::_VarProperty* pPro,KDlg_PropertyPage* pParent)
{
	switch(pPro->Type)
	{
	case PropertyBase::_VarProperty::VAR_SCENEEDITOROBJECT:
		{
			KDlg_ProItem_SceneObjects* pDialog = new KDlg_ProItem_SceneObjects((CWnd*)pParent);
			//pDialog->SetStore(pPro,pParent);
			pDialog->Create(IDD_PROITEM_SCENEOBJECT,(CWnd*)pParent);
			pDialog->ShowWindow(TRUE);

			m_lpDailog = pDialog;
			break;
		}
	}
	return Top + 30;
}


int _ProItemInput_Terrain_Ground_Texture::Create(int Top,int Width,PropertyBase::_VarProperty* pPro,KDlg_PropertyPage* pParent)
{
	switch(pPro->Type)
	{
	case PropertyBase::_VarProperty::VAR_TERRAIN_GROUND_TEXTURE:
		{
			KDlg_ProItem_Terrain_Ground_Texture* pDialog = new KDlg_ProItem_Terrain_Ground_Texture((CWnd*)pParent);
			//pDialog->SetStore(pPro,pParent);
			pDialog->Create(IDD_PROITEM_TERRAIN_GROUNDTEXTURE,(CWnd*)pParent);
			pDialog->ShowWindow(TRUE);

			m_lpDailog = pDialog;
			break;
		}
	}
	return Top + 30;
}

int _ProItemInput_Position::Create(int Top,int Width,PropertyBase::_VarProperty* pPro,KDlg_PropertyPage* pParent)
{
	switch(pPro->Type)
	{
	case PropertyBase::_VarProperty::VAR_POSITION:
		{
			KDlg_ProItem_Vec3* pDialog = new KDlg_ProItem_Vec3((CWnd*)pParent);
			//pDialog->SetStore(pPro);
			pDialog->Create(IDD_PROITEM_VEC3,(CWnd*)pParent);
			pDialog->ShowWindow(TRUE);

			m_lpDailog = pDialog;
			break;
		}
	}
	return Top + 30;
}

int _ProItemInput_Particle::Create(int Top,int Width,PropertyBase::_VarProperty* pPro,KDlg_PropertyPage* pParent)
{
	switch(pPro->Type)
	{
	case PropertyBase::_VarProperty::VAR_PARTICLE:
		{
			KDlg_ProItem_Particle* pDialog = new KDlg_ProItem_Particle((CWnd*)pParent);
			pDialog->SetStore(pPro,pParent);
			pDialog->Create(IDD_PROITEM_SFXPART,(CWnd*)pParent);
			pDialog->ShowWindow(TRUE);

			m_lpDailog = pDialog;
			break;
		}
	}
	return Top + 30;
}

};