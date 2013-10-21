// KDlg_ProItem_Ani.cpp : implementation file
//

#include "stdafx.h"
#include "sceneeditor.h"
#include "KDlg_ProItem_Ani.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// KDlg_ProItem_Ani dialog

namespace PropertyBase
{
KDlg_ProItem_Ani::KDlg_ProItem_Ani(CWnd* pParent /*=NULL*/)
	: CDialog(KDlg_ProItem_Ani::IDD, pParent)
{
	//{{AFX_DATA_INIT(KDlg_ProItem_Ani)
	m_FrameLength = 0.0f;
	m_TotalTime = 0.0f;
	//}}AFX_DATA_INIT
}


void KDlg_ProItem_Ani::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(KDlg_ProItem_Ani)
	DDX_Control(pDX, IDC_SLIDER1, m_Slider);
	DDX_Control(pDX, IDC_LIST1, m_List);
	DDX_Text(pDX, IDC_EFRAME, m_FrameLength);
	DDV_MinMaxFloat(pDX, m_FrameLength, 1.e-002f, 1000.f);
	DDX_Text(pDX, IDC_ETOTALTIME, m_TotalTime);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(KDlg_ProItem_Ani, CDialog)
	//{{AFX_MSG_MAP(KDlg_ProItem_Ani)
	ON_BN_CLICKED(IDC_BOPEN, OnBopen)
	ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
	ON_BN_CLICKED(IDC_BPLAY, OnBplay)
	ON_BN_CLICKED(IDC_BDELETE, OnBdelete)
	ON_BN_CLICKED(IDC_BSTOP, OnBstop)
	ON_BN_CLICKED(IDC_RPLAYCIRCLE, OnRplaycircle)
	ON_BN_CLICKED(IDC_RPLAYNORMAL, OnRplaynormal)
	ON_BN_CLICKED(IDC_RPLAYPPANG, OnRplayppang)
	ON_BN_CLICKED(IDC_RPLAYREVERSE, OnRplayreverse)
	ON_NOTIFY(NM_RELEASEDCAPTURE, IDC_SLIDER1, OnReleasedcaptureSlider1)
	ON_EN_CHANGE(IDC_EFRAME, OnChangeEframe)
	ON_EN_CHANGE(IDC_ETOTALTIME, OnChangeEtotaltime)
	ON_NOTIFY(NM_CLICK, IDC_LIST1, OnClickList1)
	ON_WM_VSCROLL()
	ON_BN_CLICKED(IDC_BSAVE, OnBsave)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

HRESULT KDlg_ProItem_Ani::CreateListCum()
{
	LV_COLUMN lvc;
	{
		lvc.mask     = LVCF_FMT | LVCF_SUBITEM | LVCF_TEXT | LVCF_WIDTH;
		lvc.fmt      = LVCFMT_LEFT;
		lvc.pszText  = "名称";
		lvc.iSubItem = 0;         
		lvc.cx       = 100;
		m_List.InsertColumn( 0, &lvc );
	}
	{
		lvc.mask     = LVCF_FMT | LVCF_SUBITEM | LVCF_TEXT | LVCF_WIDTH;
		lvc.fmt      = LVCFMT_LEFT;
		lvc.pszText  = "序号";
		lvc.iSubItem = 1;         
		lvc.cx       = 50;
		m_List.InsertColumn( 1, &lvc );
	}
	{
		lvc.mask     = LVCF_FMT | LVCF_SUBITEM | LVCF_TEXT | LVCF_WIDTH;
		lvc.fmt      = LVCFMT_LEFT;
		lvc.pszText  = "桢数";
		lvc.iSubItem = 2;         
		lvc.cx       = 50;
		m_List.InsertColumn( 3, &lvc );
	}
	{
		lvc.mask     = LVCF_FMT | LVCF_SUBITEM | LVCF_TEXT | LVCF_WIDTH;
		lvc.fmt      = LVCFMT_LEFT;
		lvc.pszText  = "间隔";
		lvc.iSubItem = 3;         
		lvc.cx       = 50;
		m_List.InsertColumn( 3, &lvc );
	}
	{
		lvc.mask     = LVCF_FMT | LVCF_SUBITEM | LVCF_TEXT | LVCF_WIDTH;
		lvc.fmt      = LVCFMT_LEFT;
		lvc.pszText  = "总长度";
		lvc.iSubItem = 4;         
		lvc.cx       = 50;
		m_List.InsertColumn( 4, &lvc );
	}
	return S_OK;
}
HRESULT KDlg_ProItem_Ani::FillList()
{
	m_List.DeleteAllItems();
	
	//if(m_lpProperty)
	//{
	//	m_lpModel = (LPMODEL)(m_lpProperty->Address);
	//	
	//	for(DWORD i=0;i<m_lpModel->m_dNumAnimation;i++)
	//	{
	//		KModel::LPANIMATION pAni = m_lpModel->m_lpAnimations[i];

	//		TCHAR szBuf[256];

	//		LV_ITEM lvItem;
	//		{
	//			wsprintf( szBuf, _T( "%s" ), pAni->Name );
	//			lvItem.mask = LVIF_TEXT | LVIF_IMAGE;
	//			lvItem.iItem = i;      
	//			lvItem.iSubItem = 0;
	//			lvItem.iImage   = 0;
	//			lvItem.pszText = szBuf; 
	//			int index = m_List.InsertItem( &lvItem );
	//			m_List.SetItemData(index,i); 
	//		}
	//		{
	//			wsprintf( szBuf, _T( "%d" ) , i+1 );
	//			lvItem.mask = LVIF_TEXT | LVIF_IMAGE;
	//			lvItem.iItem = i;      
	//			lvItem.iSubItem = 1;
	//			lvItem.iImage   = 0;
	//			lvItem.pszText = szBuf; 
	//			m_List.SetItem(&lvItem );
	//		}
	//		{
	//			wsprintf( szBuf, _T( "%d" ),pAni->NumFrames);
	//			lvItem.mask = LVIF_TEXT | LVIF_IMAGE;
	//			lvItem.iItem = i;      
	//			lvItem.iSubItem = 2;
	//			lvItem.iImage   = 0;
	//			lvItem.pszText = szBuf; 
	//			m_List.SetItem(&lvItem );
	//		}
	//		{
	//			sprintf( szBuf, _T( "%.1f" ),pAni->FrameLength);
	//			lvItem.mask = LVIF_TEXT | LVIF_IMAGE;
	//			lvItem.iItem = i;      
	//			lvItem.iSubItem = 3;
	//			lvItem.iImage   = 0;
	//			lvItem.pszText = szBuf; 
	//			m_List.SetItem(&lvItem );
	//		}
	//		{
	//			sprintf( szBuf, _T( "%.1f" ),pAni->FrameLength*pAni->NumFrames);
	//			lvItem.mask = LVIF_TEXT | LVIF_IMAGE;
	//			lvItem.iItem = i;      
	//			lvItem.iSubItem = 4;
	//			lvItem.iImage   = 0;
	//			lvItem.pszText = szBuf; 
	//			m_List.SetItem(&lvItem );
	//		}
	//	}
	//}
	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// KDlg_ProItem_Ani message handlers

BOOL KDlg_ProItem_Ani::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	CreateListCum();
	FillList();
	m_Slider.SetRange(0,1000);
	m_Slider.SetBuddy(this);
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void KDlg_ProItem_Ani::OnBopen() 
{
	//if(!m_lpModel)
	//	return;

	//// TODO: Add your control notification handler code here
	//char bigBuff[800] = "";  // maximum common dialog buffer size
	//char szFilter[] ="Animation Files (*.Ani)|*.Ani|";

	//CFileDialog dlg(true, NULL, NULL,
	//	OFN_HIDEREADONLY , szFilter);
	//// Modify OPENFILENAME members directly to point to bigBuff
	//dlg.m_ofn.lpstrFile = bigBuff;
	//dlg.m_ofn.nMaxFile = sizeof(bigBuff);
	//if (	dlg.DoModal() == IDOK)
	//{
	//	TCHAR path_buffer[256];
	//	TCHAR drive[256];
	//	TCHAR dir[256];
	//	TCHAR fname[256];
	//	TCHAR ext[256];

	//	CString Vs = dlg.GetPathName();

	//	wsprintf(path_buffer,"%s",Vs);
	//	_splitpath( path_buffer, drive, dir, fname, ext );


	//	TCHAR filename[256];
	//	wsprintf(filename,"%s%s%s.Ani",drive,dir,fname);

	//	if (FAILED(m_lpModel->LoadAnimation(filename)))
	//		return ;
	//}
	//FillList();
	return;
}

void KDlg_ProItem_Ani::OnAppAbout() 
{

}

void KDlg_ProItem_Ani::OnBplay() 
{
	// TODO: Add your control notification handler code here
	//if(!m_lpModel)
	//	return;

	//// TODO: Add your command handler code here
	//POSITION pos = m_List.GetFirstSelectedItemPosition();
	//if(pos)
	//{
	//	if(m_lpModel->m_dCurAnimationMethod == PLAY_HOLD)
	//		m_lpModel->m_dCurAnimationMethod = PLAY_NONE;

	//	int Index = m_List.GetNextSelectedItem(pos);
	//	m_lpModel->PlayAnimation(Index,
	//		m_lpModel->m_dCurAnimationMethod,
	//		m_lpModel->m_fCurAnimationSpeed);
	//}	
}


void KDlg_ProItem_Ani::OnBdelete() 
{
	// TODO: Add your control notification handler code here
	//if(!m_lpModel)
	//	return;

	//// TODO: Add your command handler code here
	//POSITION pos = m_List.GetFirstSelectedItemPosition();
	//if(pos)
	//{
	//	int Index = m_List.GetNextSelectedItem(pos);
	//	TCHAR str[256];
	//	wsprintf(str,"%s",m_lpModel->m_lpAnimations[Index]->Name);
	//	m_lpModel->DeleteAnimation(str);
	//}	
	FillList();
}



void KDlg_ProItem_Ani::OnBstop() 
{
	// TODO: Add your control notification handler code here
	/*if(!m_lpModel)
		return;
	m_lpModel->PlayAnimation(m_lpModel->m_nCurAnimationIndex,
		PLAY_HOLD,
		m_lpModel->m_fCurAnimationSpeed);*/

}

void KDlg_ProItem_Ani::OnRplaycircle() 
{
	// TODO: Add your control notification handler code here
	/*if(!m_lpModel)
		return;	
	m_lpModel->m_dCurAnimationMethod = PLAY_CIRCLE;*/

}

void KDlg_ProItem_Ani::OnRplaynormal() 
{
	// TODO: Add your control notification handler code here
	/*if(!m_lpModel)
		return;	
	m_lpModel->m_dCurAnimationMethod = PLAY_NONE;*/

}

void KDlg_ProItem_Ani::OnRplayppang() 
{
	// TODO: Add your control notification handler code here
	/*if(!m_lpModel)
		return;	
	m_lpModel->m_dCurAnimationMethod = PLAY_PINGPANG;*/

}

void KDlg_ProItem_Ani::OnRplayreverse() 
{
	// TODO: Add your control notification handler code here
	/*if(!m_lpModel)
		return;	
	m_lpModel->m_dCurAnimationMethod = PLAY_NONE;*/

}

void KDlg_ProItem_Ani::OnReleasedcaptureSlider1(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here
	/*if(!m_lpModel)
		return;	
	if(m_lpModel->m_nCurAnimationIndex<0)
		return;

	KModel::LPANIMATION pAni = m_lpModel->m_lpAnimations[m_lpModel->m_nCurAnimationIndex];

	float Percent = m_Slider.GetPos()*0.001f;
	float Time = Percent * pAni->NumFrames*pAni->FrameLength;
	m_lpModel->m_fCurAnimationStart = Time;
	m_lpModel->m_dCurAnimationMethod = PLAY_HOLD;*/

	*pResult = 0;
}

void KDlg_ProItem_Ani::OnChangeEframe() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here
	//if(!m_lpModel)
	//	return;	
	//if(m_lpModel->m_nCurAnimationIndex<0)
	//	return;
	//UpdateData(TRUE);
	//KModel::LPANIMATION pAni = m_lpModel->m_lpAnimations[m_lpModel->m_nCurAnimationIndex];
	//pAni->FrameLength = m_FrameLength;
	//m_TotalTime = m_FrameLength * pAni->NumFrames;
	//UpdateData(FALSE);
	////FillList();
	//POSITION pos = m_List.GetFirstSelectedItemPosition();
	//if(pos)
	//{
	//	int Index = m_List.GetNextSelectedItem(pos);
	//	TCHAR szBuf[256];

	//	LV_ITEM lvItem;
	//	{
	//		sprintf( szBuf, _T( "%.1f" ),pAni->FrameLength);
	//		lvItem.mask = LVIF_TEXT | LVIF_IMAGE;
	//		lvItem.iItem = Index;      
	//		lvItem.iSubItem = 3;
	//		lvItem.iImage   = 0;
	//		lvItem.pszText = szBuf; 
	//		m_List.SetItem(&lvItem );
	//	}
	//	{
	//		sprintf( szBuf, _T( "%.1f" ),pAni->FrameLength*pAni->NumFrames);
	//		lvItem.mask = LVIF_TEXT | LVIF_IMAGE;
	//		lvItem.iItem = Index;      
	//		lvItem.iSubItem = 4;
	//		lvItem.iImage   = 0;
	//		lvItem.pszText = szBuf; 
	//		m_List.SetItem(&lvItem );
	//	}
	//}
}

void KDlg_ProItem_Ani::OnChangeEtotaltime() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here
	//if(!m_lpModel)
	//	return;	
	//if(m_lpModel->m_nCurAnimationIndex<0)
	//	return;
	//UpdateData(TRUE);
	//KModel::LPANIMATION pAni = 
	//	m_lpModel->m_lpAnimations[m_lpModel->m_nCurAnimationIndex];
	//pAni->FrameLength = m_TotalTime / pAni->NumFrames;
	//m_FrameLength = pAni->FrameLength;
	//UpdateData(FALSE);
	////FillList();
	//POSITION pos = m_List.GetFirstSelectedItemPosition();
	//if(pos)
	//{
	//	int Index = m_List.GetNextSelectedItem(pos);
	//	TCHAR szBuf[256];

	//	LV_ITEM lvItem;
	//	{
	//		sprintf( szBuf, _T( "%.1f" ),pAni->FrameLength);
	//		lvItem.mask = LVIF_TEXT | LVIF_IMAGE;
	//		lvItem.iItem = Index;      
	//		lvItem.iSubItem = 3;
	//		lvItem.iImage   = 0;
	//		lvItem.pszText = szBuf; 
	//		m_List.SetItem(&lvItem );
	//	}
	//	{
	//		sprintf( szBuf, _T( "%.1f" ),pAni->FrameLength*pAni->NumFrames);
	//		lvItem.mask = LVIF_TEXT | LVIF_IMAGE;
	//		lvItem.iItem = Index;      
	//		lvItem.iSubItem = 4;
	//		lvItem.iImage   = 0;
	//		lvItem.pszText = szBuf; 
	//		m_List.SetItem(&lvItem );
	//	}
	//}
}

void KDlg_ProItem_Ani::OnClickList1(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here
	//if(!m_lpModel)
	//	return;

	//// TODO: Add your command handler code here
	//POSITION pos = m_List.GetFirstSelectedItemPosition();
	//if(pos)
	//{
	//	int Index = m_List.GetNextSelectedItem(pos);
	//	if((Index>=0)&&(Index<(int)m_lpModel->m_dNumAnimation))
	//	{
	//		m_lpModel->m_nCurAnimationIndex = Index;
	//		KModel::LPANIMATION pAni = 
	//			m_lpModel->m_lpAnimations[m_lpModel->m_nCurAnimationIndex];
	//		m_FrameLength = pAni->FrameLength;
	//		m_TotalTime = m_FrameLength * pAni->NumFrames;
	//		UpdateData(FALSE);
	//	}
	//}		
	*pResult = 0;
}



void KDlg_ProItem_Ani::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	// TODO: Add your message handler code here and/or call default
	
	CDialog::OnVScroll(nSBCode, nPos, pScrollBar);
}


void KDlg_ProItem_Ani::OnBsave() 
{
	// TODO: Add your control notification handler code here
	//	if(!m_lpModel)
	//	return;

	//// TODO: Add your control notification handler code here
	//char bigBuff[800] = "";  // maximum common dialog buffer size
	//char szFilter[] ="Animation Files (*.Ani)|*.Ani|";

	//CFileDialog dlg(false, "Ani", NULL,
	//	OFN_HIDEREADONLY , szFilter);
	//// Modify OPENFILENAME members directly to point to bigBuff
	//dlg.m_ofn.lpstrFile = bigBuff;
	//dlg.m_ofn.nMaxFile = sizeof(bigBuff);
	//if (	dlg.DoModal() == IDOK)
	//{
	//	TCHAR path_buffer[256];
	//	TCHAR drive[256];
	//	TCHAR dir[256];
	//	TCHAR fname[256];
	//	TCHAR ext[256];

	//	CString Vs = dlg.GetPathName();

	//	wsprintf(path_buffer,"%s",Vs);
	//	_splitpath( path_buffer, drive, dir, fname, ext );


	//	TCHAR filename[256];
	//	wsprintf(filename,"%s%s%s.Ani",drive,dir,fname);

	//	if (FAILED(m_lpModel->SaveAnimation(filename)))
	//		return ;
	//}

	return;
}
}