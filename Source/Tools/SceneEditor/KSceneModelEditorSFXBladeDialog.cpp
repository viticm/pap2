// KSceneModelEditorSFXBladeDialog.cpp : implementation file
//

#include "stdafx.h"
#include "sceneeditor.h"
#include "KSceneModelEditorSFXBladeDialog.h"
#include ".\kscenemodeleditorsfxbladedialog.h"
#include "KSceneModelEditorMeshBindDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// KSceneModelEditorSFXBladeDialog dialog


KSceneModelEditorSFXBladeDialog::KSceneModelEditorSFXBladeDialog(CWnd* pParent /*=NULL*/)
	: CDialog(KSceneModelEditorSFXBladeDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(KSceneModelEditorSFXBladeDialog)
	m_fTotalLength = 0.0f;
	m_fSampleLength = 0.0f;
	m_A_X = 0.0f;
	m_A_Y = 0.0f;
	m_A_Z = 0.0f;
	m_B_X = 0.0f;
	m_B_Y = 0.0f;
	m_B_Z = 0.0f;
	m_fPlaySpeed = 1.0f;
	m_bSelectFrame = FALSE;
	m_fSetSelectLength = 1000.0f;
	m_CString_CurFrame = _T("");
	//}}AFX_DATA_INIT
	//m_lpBlade = NULL;
	m_nSelectStart = 0;
	//m_lpModelWeapon = NULL;
}


void KSceneModelEditorSFXBladeDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(KSceneModelEditorSFXBladeDialog)
	DDX_Control(pDX, IDC_CHECK_FRAMESELECT, m_Check_SelectFrame);
	DDX_Control(pDX, IDC_SLIDER_TIME, m_Slider_Time);
	DDX_Control(pDX, IDC_LIST_ANI, m_ListBox_Ani);
	DDX_Control(pDX, IDC_COMBO_SOCKET, m_Combo_Socket);
	DDX_Control(pDX, IDC_LIST_BLADE, m_ListBox_Blade);
	DDX_Text(pDX, IDC_EDIT_TOTALLENGTH, m_fTotalLength);
	DDX_Text(pDX, IDC_EDIT_SAMPLELENGTH, m_fSampleLength);
	DDX_Text(pDX, IDC_EDIT_A_X, m_A_X);
	DDX_Text(pDX, IDC_EDIT_A_Y, m_A_Y);
	DDX_Text(pDX, IDC_EDIT_A_Z, m_A_Z);
	DDX_Text(pDX, IDC_EDIT_B_X, m_B_X);
	DDX_Text(pDX, IDC_EDIT_B_Y, m_B_Y);
	DDX_Text(pDX, IDC_EDIT_B_Z, m_B_Z);
	DDX_Text(pDX, IDC_EDIT_PLAYSPEED, m_fPlaySpeed);
	DDV_MinMaxFloat(pDX, m_fPlaySpeed, 1.e-003f, 10.f);
	DDX_Check(pDX, IDC_CHECK_FRAMESELECT, m_bSelectFrame);
	DDX_Text(pDX, IDC_EDIT_SELECTLENGTH, m_fSetSelectLength);
	DDX_Text(pDX, IDC_STATIC_CURFRAME, m_CString_CurFrame);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(KSceneModelEditorSFXBladeDialog, CDialog)
	//{{AFX_MSG_MAP(KSceneModelEditorSFXBladeDialog)
	ON_WM_SHOWWINDOW()
	ON_WM_PAINT()
	ON_WM_HSCROLL()
	//}}AFX_MSG_MAP
	ON_LBN_SELCHANGE(IDC_LIST1, OnLbnSelchangeList1)
	ON_LBN_SELCHANGE(IDC_LIST_BLADE, OnLbnSelchangeListBlade)
	ON_EN_CHANGE(IDC_EDIT_SAMPLELENGTH, OnEnChangeEditSamplelength)
	ON_EN_CHANGE(IDC_EDIT_TOTALLENGTH, OnEnChangeEditTotallength)
	ON_EN_CHANGE(IDC_EDIT_A_X, OnEnChangeEditAX)
	ON_EN_CHANGE(IDC_EDIT_A_Y, OnEnChangeEditAY)
	ON_EN_CHANGE(IDC_EDIT_A_Z, OnEnChangeEditAZ)
	ON_EN_CHANGE(IDC_EDIT_B_X, OnEnChangeEditBX)
	ON_EN_CHANGE(IDC_EDIT_B_Y, OnEnChangeEditBY)
	ON_EN_CHANGE(IDC_EDIT_B_Z, OnEnChangeEditBZ)
	ON_BN_CLICKED(IDC_BUTTON_BLADENEW, OnBnClickedButtonBladenew)
	ON_BN_CLICKED(IDC_BUTTON_BLADESAVE, OnBnClickedButtonBladesave)
	ON_BN_CLICKED(IDC_BUTTON_BLADELOAD, OnBnClickedButtonBladeload)
	ON_BN_CLICKED(IDC_BUTTON1, OnBnClickedButton1)
	ON_CBN_SELCHANGE(IDC_COMBO_SOCKET, OnCbnSelchangeComboSocket)
	ON_BN_CLICKED(IDC_BUTTON_AUTOANI, OnBnClickedButtonAutoani)
	ON_LBN_SELCHANGE(IDC_LIST_ANI, OnLbnSelchangeListAni)
	ON_BN_CLICKED(IDC_BUTTON_LOADANI, OnBnClickedButtonLoadani)
	ON_BN_CLICKED(IDC_BUTTON_ANIPLAY, OnBnClickedButtonAniplay)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_SLIDER4, OnNMCustomdrawSlider4)
	ON_EN_CHANGE(IDC_EDIT_PLAYSPEED, OnEnChangeEditPlayspeed)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_SLIDER_TIME, OnNMCustomdrawSliderTime)
	ON_BN_CLICKED(IDC_CHECK_FRAMESELECT, OnBnClickedCheckFrameselect)
	ON_BN_CLICKED(IDC_BUTTON_HIDESELECT, OnBnClickedButtonHideselect)
	ON_BN_CLICKED(IDC_BUTTON_SHOWSELECT, OnBnClickedButtonShowselect)
	ON_BN_CLICKED(IDC_BUTTON_SETSELECTLENGTH, OnBnClickedButtonSetselectlength)
	ON_BN_CLICKED(IDC_BUTTON_REFRESH, OnBnClickedButtonRefresh)
	ON_BN_CLICKED(IDC_BUTTON_ANILAOD, OnBnClickedButtonAnilaod)
	ON_BN_CLICKED(IDC_BUTTON_ANISAVE, OnBnClickedButtonAnisave)
	ON_BN_CLICKED(IDC_BUTTON_ANINEW, OnBnClickedButtonAninew)
	ON_BN_CLICKED(IDC_BUTTON_ANIDELETE, OnBnClickedButtonAnidelete)
	ON_BN_CLICKED(IDC_BUTTON_LOADWEAPON, OnBnClickedButtonLoadweapon)
	ON_BN_CLICKED(IDC_BUTTON_AUTOANI2, OnBnClickedButtonAutoani2)
	ON_EN_CHANGE(IDC_EDIT_SELECTLENGTH, OnEnChangeEditSelectlength)
//	ON_BN_CLICKED(IDC_BUTTON_HIDESELECT2, OnBnClickedButtonHideselect2)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// KSceneModelEditorSFXBladeDialog message handlers

void KSceneModelEditorSFXBladeDialog::OnLbnSelchangeList1()
{
	// TODO: Add your control notification handler code here
}

BOOL KSceneModelEditorSFXBladeDialog::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	FillListBoxBlade();
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void KSceneModelEditorSFXBladeDialog::FillListBoxBlade()
{
	//{
	//	m_ListBox_Blade.SetCurSel(-1);
	//	int Count = m_ListBox_Blade.GetCount();
	//	for(int i=Count+1;i>=0;i--)
	//	{
	//		m_ListBox_Blade.DeleteString(i);
	//	}
	//}

	//if(!m_lpBlade)
	//	return;

	//for(DWORD i=0;i<m_lpBlade->m_vecBlade.size();i++)
	//{
	//	TCHAR str[256];
	//	wsprintf(str,"刀光第 %d 条",i+1);
	//	int Index = m_ListBox_Blade.AddString(str);
	//	m_ListBox_Blade.SetItemData(Index,i);
	//}

}

void KSceneModelEditorSFXBladeDialog::OnLbnSelchangeListBlade()
{
	// TODO: Add your control notification handler code here
	//if(!m_lpBlade)
	//	return;
	//int Index = m_ListBox_Blade.GetCurSel();
	//if(Index>=0)
	//{
	//	DWORD i = m_ListBox_Blade.GetItemData(Index);
	//	m_fTotalLength = m_lpBlade->m_vecBlade[i]->m_fCurLengthLimit;
	//	m_fSampleLength = m_lpBlade->m_vecBlade[i]->m_fSampleLength;
	//	m_A_X = m_lpBlade->m_vecBlade[i]->m_CurScrLine.A.x;
	//	m_A_Y = m_lpBlade->m_vecBlade[i]->m_CurScrLine.A.y;
	//	m_A_Z = m_lpBlade->m_vecBlade[i]->m_CurScrLine.A.z;
	//	m_B_X = m_lpBlade->m_vecBlade[i]->m_CurScrLine.B.x;
	//	m_B_Y = m_lpBlade->m_vecBlade[i]->m_CurScrLine.B.y;
	//	m_B_Z = m_lpBlade->m_vecBlade[i]->m_CurScrLine.B.z;
	//	UpdateData(FALSE);
	//}
}

void KSceneModelEditorSFXBladeDialog::OnShowWindow(BOOL bShow, UINT nStatus) 
{
	CDialog::OnShowWindow(bShow, nStatus);
	
	// TODO: Add your message handler code here
	FillListBoxBlade();

}

void KSceneModelEditorSFXBladeDialog::OnEnChangeEditSamplelength()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
	//if(!m_lpBlade)
	//	return;
	//UpdateData(TRUE);
	//int Index = m_ListBox_Blade.GetCurSel();
	//if(Index>=0)
	//{
	//	DWORD i = m_ListBox_Blade.GetItemData(Index);
	//	m_lpBlade->m_vecBlade[i]->m_fSampleLength = m_fSampleLength;
	//}
}

void KSceneModelEditorSFXBladeDialog::OnEnChangeEditTotallength()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
	//if(!m_lpBlade)
	//	return;
	//UpdateData(TRUE);
	//int Index = m_ListBox_Blade.GetCurSel();
	//if(Index>=0)
	//{
	//	DWORD i = m_ListBox_Blade.GetItemData(Index);
	//	m_lpBlade->m_vecBlade[i]->m_fCurLengthLimit = m_fTotalLength;
	//}
}

void KSceneModelEditorSFXBladeDialog::OnEnChangeEditAX()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
	//if(!m_lpBlade)
	//	return;
	//UpdateData(TRUE);
	//int Index = m_ListBox_Blade.GetCurSel();
	//if(Index>=0)
	//{
	//	DWORD i = m_ListBox_Blade.GetItemData(Index);
	//	m_lpBlade->m_vecBlade[i]->m_CurScrLine.A.x = m_A_X;
	//}
}

void KSceneModelEditorSFXBladeDialog::OnEnChangeEditAY()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
	//if(!m_lpBlade)
	//	return;
	//UpdateData(TRUE);
	//int Index = m_ListBox_Blade.GetCurSel();
	//if(Index>=0)
	//{
	//	DWORD i = m_ListBox_Blade.GetItemData(Index);
	//	m_lpBlade->m_vecBlade[i]->m_CurScrLine.A.y = m_A_Y;
	//}
}

void KSceneModelEditorSFXBladeDialog::OnEnChangeEditAZ()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
	//if(!m_lpBlade)
	//	return;
	//UpdateData(TRUE);
	//int Index = m_ListBox_Blade.GetCurSel();
	//if(Index>=0)
	//{
	//	DWORD i = m_ListBox_Blade.GetItemData(Index);
	//	m_lpBlade->m_vecBlade[i]->m_CurScrLine.A.z = m_A_Z;
	//}
}

void KSceneModelEditorSFXBladeDialog::OnEnChangeEditBX()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
	//if(!m_lpBlade)
	//	return;
	//UpdateData(TRUE);
	//int Index = m_ListBox_Blade.GetCurSel();
	//if(Index>=0)
	//{
	//	DWORD i = m_ListBox_Blade.GetItemData(Index);
	//	m_lpBlade->m_vecBlade[i]->m_CurScrLine.B.x = m_B_X;
	//}
}

void KSceneModelEditorSFXBladeDialog::OnEnChangeEditBY()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
	//if(!m_lpBlade)
	//	return;
	//UpdateData(TRUE);
	//int Index = m_ListBox_Blade.GetCurSel();
	//if(Index>=0)
	//{
	//	DWORD i = m_ListBox_Blade.GetItemData(Index);
	//	m_lpBlade->m_vecBlade[i]->m_CurScrLine.B.y = m_B_Y;
	//}
}

void KSceneModelEditorSFXBladeDialog::OnEnChangeEditBZ()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
	//if(!m_lpBlade)
	//	return;
	//UpdateData(TRUE);
	//int Index = m_ListBox_Blade.GetCurSel();
	//if(Index>=0)
	//{
	//	DWORD i = m_ListBox_Blade.GetItemData(Index);
	//	m_lpBlade->m_vecBlade[i]->m_CurScrLine.B.z = m_B_Z;
	//}
}

void KSceneModelEditorSFXBladeDialog::OnBnClickedButtonBladenew()
{
	// TODO: Add your control notification handler code here
	//if(!m_lpBlade)
	//	return;
	//ModelSFX::KModelSFXBlade::LPBLADEINFO pBlade = new ModelSFX::KModelSFXBlade::BLADEINFO;
	//pBlade->CreateVerticesBuffer();
	//m_lpBlade->m_vecBlade.push_back(pBlade);

	FillListBoxBlade();
}

void KSceneModelEditorSFXBladeDialog::OnBnClickedButtonBladesave()
{
	//if(!m_lpBlade)
	//	return;
	//char bigBuff[800] = "";  // maximum common dialog buffer size
	//char szFilter[] ="刀光文件 (*.Blade)|*.Blade|";

	//CFileDialog dlg(false, "Blade", NULL,
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
	//	wsprintf(filename,"%s%s%s.Blade",drive,dir,fname);

	//	m_lpBlade->SaveMesh(filename);
	//}
}

void KSceneModelEditorSFXBladeDialog::OnBnClickedButtonBladeload()
{
	// TODO: Add your control notification handler code here
	//if(!m_lpBlade)
	//	return;
	//char bigBuff[800] = "";  // maximum common dialog buffer size
	//char szFilter[] ="刀光文件 (*.Blade)|*.Blade|";

	//CFileDialog dlg(false, "Blade", NULL,
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
	//	wsprintf(filename,"%s%s%s.Blade",drive,dir,fname);

	//	m_lpBlade->LoadMesh(filename);
	//}
	FillListBoxBlade();
}

void KSceneModelEditorSFXBladeDialog::OnBnClickedButton1()
{
	//if(!m_lpBlade)
	//	return;
	//// TODO: Add your control notification handler code here
	//KSceneModelEditorMeshBindDialog Dlg;
	//Dlg.SetChildModel(m_lpModelEditor,m_lpBlade);
	//Dlg.DoModal();

	//FillComboSocket(m_lpBlade->m_pBindMesh);
	//if(m_lpBlade->m_pBindMesh)
	//{
	//	FillListBoxAni();
	//}
}

void KSceneModelEditorSFXBladeDialog::OnCbnSelchangeComboSocket()
{
	// TODO: Add your control notification handler code here
	/*if(!m_lpBlade)
		return;
	if(!m_lpBlade->m_pBindMesh)
		return;
	int Index = m_Combo_Socket.GetCurSel();
	if(Index>=0)
	{
		int i = m_Combo_Socket.GetItemData(Index);
		if ((i>=0)&&(i<(int)m_lpBlade->m_pBindMesh->m_dNumSocket))
		{
			KModel::LPSOCKET pScoket = &m_lpBlade->m_pBindMesh->m_lpSockets[i];

			m_lpBlade->Bind(m_lpBlade->m_pBindMesh,(LPSTR)pScoket->scName.c_str());
			
			if(m_lpModelWeapon)
			{
				m_lpModelWeapon->Bind(m_lpBlade->m_pBindMesh,(LPSTR)pScoket->scName.c_str());
			}
			
		}
	}*/
}

//void KSceneModelEditorSFXBladeDialog::FillComboSocket(LPMODEL pModel)
//{
//	/*{
//		m_Combo_Socket.SetCurSel(-1);
//		int Count = m_Combo_Socket.GetCount();
//		for(int i=Count+1;i>=0;i--)
//		{
//			m_Combo_Socket.DeleteString(i);
//		}
//	}
//	if(!pModel)
//		return;
//	for(DWORD i=0;i<pModel->m_dNumSocket;i++)
//	{
//		KModel::LPSOCKET pScoket = &pModel->m_lpSockets[i];
//		int Index = m_Combo_Socket.AddString(pScoket->scName.c_str());
//		m_Combo_Socket.SetItemData(Index,i);
//	}
//	m_Combo_Socket.SetCurSel(0);*/
//}

void KSceneModelEditorSFXBladeDialog::FillListBoxAni()
{
	/*{
		m_ListBox_Ani.SetCurSel(-1);
		int Count = m_ListBox_Ani.GetCount();
		for(int i=Count+1;i>=0;i--)
		{
			m_ListBox_Ani.DeleteString(i);
		}
	}

	if(!m_lpBlade)
		return;
	if(!m_lpBlade->m_pBindMesh)
		return;

	LPMODEL pMesh = m_lpBlade->m_pBindMesh;

	for(DWORD i=0;i<pMesh->m_lpAnimations.size();i++)
	{
		KModel::LPANIMATION pAni = pMesh->m_lpAnimations[i];
		int iBladeAni = m_lpBlade->FindAnimation(pAni->Name);

		TCHAR str[256];
		if(iBladeAni>=0)
		{
			wsprintf(str,"%s(已匹配)",pAni->Name);
		}
		else
		{
			wsprintf(str,"%s(未匹配)",pAni->Name);
		}
		int Index = m_ListBox_Ani.AddString(str);
		m_ListBox_Ani.SetItemData(Index,i);
	}*/
}

void KSceneModelEditorSFXBladeDialog::OnBnClickedButtonAutoani()
{
	// TODO: Add your control notification handler code here
	/*if(!m_lpBlade)
		return;
	if(!m_lpBlade->m_pBindMesh)
		return;

	m_lpBlade->ClearnAnimations();

	for(DWORD i=0;i<m_lpBlade->m_pBindMesh->m_lpAnimations.size();i++)
	{
		KModel::LPANIMATION pAniMesh = m_lpBlade->m_pBindMesh->m_lpAnimations[i];
		float Time = pAniMesh->NumFrames * pAniMesh->FrameLength;
		m_lpBlade->Add1NewAnimation(pAniMesh->Name,pAniMesh->NumFrames,pAniMesh->FrameLength);
	}
	FillListBoxAni();*/
}

void KSceneModelEditorSFXBladeDialog::OnLbnSelchangeListAni()
{
	// TODO: Add your control notification handler code here
	/*if(!m_lpBlade)
		return;
	if(!m_lpBlade->m_pBindMesh)
		return;

	int Index = m_ListBox_Ani.GetCurSel();
	if(Index<0)
		return;
	DWORD i = m_ListBox_Ani.GetItemData(Index);
	m_lpBlade->PlayAnimation(i,PLAY_NONE,m_fPlaySpeed);
	m_lpBlade->m_pBindMesh->PlayAnimation(i,PLAY_NONE,m_fPlaySpeed);

	if ((i>=0)&&(i<m_lpBlade->m_pBindMesh->m_lpAnimations.size()))
	{
		KModel::LPANIMATION pAni = m_lpBlade->m_pBindMesh->m_lpAnimations[i];
		m_Slider_Time.SetRange(0,pAni->NumFrames-1);
		m_Slider_Time.SetPageSize(1);
		m_Slider_Time.SetSelection(0,0);
		m_Check_SelectFrame.SetWindowText("未选择");
	}*/

}

void KSceneModelEditorSFXBladeDialog::OnBnClickedButtonLoadani()
{
	// TODO: Add your control notification handler code here
	//if(!m_lpBlade)
	//	return;
	//if(!m_lpBlade->m_pBindMesh)
	//	return;

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


	//	LPMODEL pMesh = m_lpBlade->m_pBindMesh;
	//	if (!pMesh)
	//		return ;

	//	if (FAILED(pMesh->LoadAnimation(filename)))
	//		return ;
	//}

	//FillListBoxAni();
}

void KSceneModelEditorSFXBladeDialog::OnBnClickedButtonAniplay()
{
	// TODO: Add your control notification handler code here
	/*if(!m_lpBlade)
		return;
	if(!m_lpBlade->m_pBindMesh)
		return;

	int Index = m_ListBox_Ani.GetCurSel();
	if(Index<0)
		return;
	DWORD i = m_ListBox_Ani.GetItemData(Index);
	m_lpBlade->PlayAnimation(i,PLAY_NONE,m_fPlaySpeed);
	m_lpBlade->m_pBindMesh->PlayAnimation(i,PLAY_NONE,m_fPlaySpeed);*/
}

void KSceneModelEditorSFXBladeDialog::OnNMCustomdrawSlider4(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
	// TODO: Add your control notification handler code here
	*pResult = 0;
}

void KSceneModelEditorSFXBladeDialog::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	
	// TODO: Add your message handler code here
	
	// Do not call CDialog::OnPaint() for painting messages
	//RECT rc;
	//RECT rcDlg;
	//GetWindowRect(&rcDlg);
	//m_Static_KeyFrame.GetWindowRect(&rc);
	//int W = rc.right - rc.left;
	//int H = rc.bottom - rc.top;
	//rc.left -= rcDlg.left;
	//rc.right = rc.left + W;
	//rc.top -= rcDlg.top;
	//rc.bottom = rc.top + H;

	//dc.Rectangle(&rc);
}


void KSceneModelEditorSFXBladeDialog::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	// TODO: Add your message handler code here and/or call default
	//if(!m_lpBlade)
	//	return;
	//if(!m_lpBlade->m_pBindMesh)
	//	return;

	//int Index = m_ListBox_Ani.GetCurSel();
	//if(Index<0)
	//	return;
	//DWORD i = m_ListBox_Ani.GetItemData(Index);

	//int Pos = m_Slider_Time.GetPos();

	//if ((i>=0)&&(i<m_lpBlade->m_pBindMesh->m_lpAnimations.size()))
	//{
	//	KModel::LPANIMATION pAni = m_lpBlade->m_pBindMesh->m_lpAnimations[i];
	//	float Time = pAni->FrameLength * Pos;

	//	m_lpBlade->m_pBindMesh->m_nCurAnimationIndex = i;
	//	m_lpBlade->m_pBindMesh->m_fCurAnimationStart = Time;
	//	m_lpBlade->m_pBindMesh->m_dCurAnimationMethod = PLAY_HOLD;

	//	int i_BladeAni = m_lpBlade->FindAnimation(pAni->Name);
	//	m_lpBlade->m_nCurAnimationIndex = i;
	//	m_lpBlade->m_fCurAnimationStart = Time;
	//	m_lpBlade->m_dCurAnimationMethod = PLAY_HOLD;
	//	
	//	if ((i_BladeAni>=0)&&(i_BladeAni<(int)m_lpBlade->m_lpAnimations.size()))
	//	{
	//		KModel::LPANIMATION pAniBlade = m_lpBlade->m_lpAnimations[i_BladeAni];
	//		int CurFrame = Pos;
	//		if(CurFrame<0) CurFrame = 0;
	//		if(CurFrame>(int)pAniBlade->NumFrames-1) CurFrame = pAniBlade->NumFrames-1;

	//		if(pAniBlade->BladeData)
	//		{
	//			KModel::_Animation::_Blade_ShowLength* pData
	//				= (KModel::_Animation::_Blade_ShowLength*) pAniBlade->BladeData;
	//			if(pData[CurFrame].bShow)
	//			{
	//				m_CString_CurFrame.Format("当前第%d桢 显示 长度 %.1f",CurFrame,pData[CurFrame].fLength);
	//			}
	//			else
	//			{
	//				m_CString_CurFrame.Format("当前第%d桢 隐藏 长度 %.1f",CurFrame,pData[CurFrame].fLength);
	//			}
	//		}
	//	}
	//	else
	//	{
	//		m_CString_CurFrame.Format("当前刀光没有动画");
	//	}
	//}

	//if(m_bSelectFrame)
	//{
	//	m_Slider_Time.SetSelection(m_nSelectStart,Pos);

	//	TCHAR str[256];
	//	int min,max;
	//	m_Slider_Time.GetSelection(min,max);
	//	wsprintf(str,"选择%d--%d",min,max);
	//	m_Check_SelectFrame.SetWindowText(str);
	//}
	//
	//CDialog::OnHScroll(nSBCode, nPos, pScrollBar);

	//UpdateData(FALSE);
}

void KSceneModelEditorSFXBladeDialog::OnEnChangeEditPlayspeed()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
	UpdateData(TRUE);

}

void KSceneModelEditorSFXBladeDialog::OnNMCustomdrawSliderTime(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
	// TODO: Add your control notification handler code here
	*pResult = 0;
}

void KSceneModelEditorSFXBladeDialog::OnBnClickedCheckFrameselect()
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
	/*if(m_bSelectFrame)
	{
		m_nSelectStart = m_Slider_Time.GetPos();
	}
	else
	{
		TCHAR str[256];
		int min,max;
		m_Slider_Time.GetSelection(min,max);
		wsprintf(str,"选择%d--%d",min,max);
		m_Check_SelectFrame.SetWindowText(str);
	}*/
}

void KSceneModelEditorSFXBladeDialog::OnBnClickedButtonHideselect()
{
	// TODO: Add your control notification handler code here
	/*if(!m_lpBlade)
		return;
	if(!m_lpBlade->m_pBindMesh)
		return;

	int Index = m_ListBox_Ani.GetCurSel();
	if(Index<0)
		return;
	DWORD i = m_ListBox_Ani.GetItemData(Index);

	int min,max;
	m_Slider_Time.GetSelection(min,max);

	if ((i>=0)&&(i<m_lpBlade->m_pBindMesh->m_lpAnimations.size()))
	{
		KModel::LPANIMATION pAni = m_lpBlade->m_pBindMesh->m_lpAnimations[i];

		int i_BladeAni = m_lpBlade->FindAnimation(pAni->Name);
		if ((i_BladeAni>=0)&&(i_BladeAni<(int)m_lpBlade->m_lpAnimations.size()))
		{
			KModel::LPANIMATION pAniBlade = m_lpBlade->m_lpAnimations[i_BladeAni];
			if(min<0) min = 0;
			if(min>(int)pAniBlade->NumFrames-1) min = pAniBlade->NumFrames-1;
			if(max<0) max = 0;
			if(max>(int)pAniBlade->NumFrames-1) max = pAniBlade->NumFrames-1;

			if(pAniBlade->BladeData)
			{
				KModel::_Animation::_Blade_ShowLength* pData
					= (KModel::_Animation::_Blade_ShowLength*) pAniBlade->BladeData;
				for(int k=min;k<=max;k++)
				{
					pData[k].bShow = FALSE;
				}
			}
		}
		else
		{
			MessageBox("请先为刀光匹配动作","错误",MB_OK);
		}
	}
	else
	{
		MessageBox("没有指定动作文件","错误",MB_OK);
	}*/
}

void KSceneModelEditorSFXBladeDialog::OnBnClickedButtonShowselect()
{
	// TODO: Add your control notification handler code here
	/*if(!m_lpBlade)
		return;
	if(!m_lpBlade->m_pBindMesh)
		return;

	int Index = m_ListBox_Ani.GetCurSel();
	if(Index<0)
		return;
	DWORD i = m_ListBox_Ani.GetItemData(Index);

	int min,max;
	m_Slider_Time.GetSelection(min,max);

	if ((i>=0)&&(i<m_lpBlade->m_pBindMesh->m_lpAnimations.size()))
	{
		KModel::LPANIMATION pAni = m_lpBlade->m_pBindMesh->m_lpAnimations[i];

		int i_BladeAni = m_lpBlade->FindAnimation(pAni->Name);
		if ((i_BladeAni>=0)&&(i_BladeAni<(int)m_lpBlade->m_lpAnimations.size()))
		{
			KModel::LPANIMATION pAniBlade = m_lpBlade->m_lpAnimations[i_BladeAni];
			if(min<0) min = 0;
			if(min>(int)pAniBlade->NumFrames-1) min = pAniBlade->NumFrames-1;
			if(max<0) max = 0;
			if(max>(int)pAniBlade->NumFrames-1) max = pAniBlade->NumFrames-1;

			if(pAniBlade->BladeData)
			{
				KModel::_Animation::_Blade_ShowLength* pData
					= (KModel::_Animation::_Blade_ShowLength*) pAniBlade->BladeData;
				for(int k=min;k<=max;k++)
				{
					pData[k].bShow = TRUE;
				}
			}
		}
		else
		{
			MessageBox("请先为刀光匹配动作","错误",MB_OK);
		}
	}
	else
	{
		MessageBox("没有指定动作文件","错误",MB_OK);
	}*/
}

void KSceneModelEditorSFXBladeDialog::OnBnClickedButtonSetselectlength()
{
	// TODO: Add your control notification handler code here
	// TODO: Add your control notification handler code here
	/*if(!m_lpBlade)
		return;
	if(!m_lpBlade->m_pBindMesh)
		return;

	int Index = m_ListBox_Ani.GetCurSel();
	if(Index<0)
		return;
	DWORD i = m_ListBox_Ani.GetItemData(Index);

	int min,max;
	m_Slider_Time.GetSelection(min,max);

	if ((i>=0)&&(i<m_lpBlade->m_pBindMesh->m_lpAnimations.size()))
	{
		KModel::LPANIMATION pAni = m_lpBlade->m_pBindMesh->m_lpAnimations[i];

		int i_BladeAni = m_lpBlade->FindAnimation(pAni->Name);
		if ((i_BladeAni>=0)&&(i_BladeAni<(int)m_lpBlade->m_lpAnimations.size()))
		{
			KModel::LPANIMATION pAniBlade = m_lpBlade->m_lpAnimations[i_BladeAni];
			if(min<0) min = 0;
			if(min>(int)pAniBlade->NumFrames-1) min = pAniBlade->NumFrames-1;
			if(max<0) max = 0;
			if(max>(int)pAniBlade->NumFrames-1) max = pAniBlade->NumFrames-1;

			if(pAniBlade->BladeData)
			{
				KModel::_Animation::_Blade_ShowLength* pData
					= (KModel::_Animation::_Blade_ShowLength*) pAniBlade->BladeData;
				for(int k=min;k<=max;k++)
				{
					pData[k].fLength = m_fSetSelectLength;
				}
			}
		}
		else
		{
			MessageBox("请先为刀光匹配动作","错误",MB_OK);
		}
	}
	else
	{
		MessageBox("没有指定动作文件","错误",MB_OK);
	}*/
}

void KSceneModelEditorSFXBladeDialog::OnBnClickedButtonRefresh()
{
	// TODO: Add your control notification handler code here
	FillListBoxAni();
}

void KSceneModelEditorSFXBladeDialog::OnBnClickedButtonAnilaod()
{
	// TODO: Add your control notification handler code here
}

void KSceneModelEditorSFXBladeDialog::OnBnClickedButtonAnisave()
{
	// TODO: Add your control notification handler code here
}

void KSceneModelEditorSFXBladeDialog::OnBnClickedButtonAninew()
{
	// TODO: Add your control notification handler code here
}

void KSceneModelEditorSFXBladeDialog::OnBnClickedButtonAnidelete()
{
	// TODO: Add your control notification handler code here
}

void KSceneModelEditorSFXBladeDialog::OnBnClickedButtonLoadweapon()
{
	// TODO: Add your control notification handler code here
	/*CFileDialog dlg(true, "mesh", 0, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, "剑网3D模型文件(*.mesh)|*.mesh||");
	if(dlg.DoModal() != IDOK)
	{
		return;
	}

	TCHAR FileName[256];
	wsprintf(FileName,"%s",dlg.GetPathName());

	KSceneModelEditor* pScene = (KSceneModelEditor*) m_lpModelEditor;

	if(pScene)
	{
		DWORD MeshID = 0;
		if(FAILED(g_cMeshTable.LoadFromFile(&MeshID,FileName)))
			return;

		if(FAILED(g_cMeshTable.GetXMesh(&m_lpModelWeapon,MeshID)))
			return;

		pScene->AddRenderMesh(m_lpModelWeapon);

		if(!m_lpBlade)
			return;
		if(!m_lpBlade->m_pBindMesh)
			return;
		int i = 0;
		if ((i>=0)&&(i<(int)m_lpBlade->m_pBindMesh->m_dNumSocket))
		{
			KModel::LPSOCKET pScoket = &m_lpBlade->m_pBindMesh->m_lpSockets[i];

			m_lpModelWeapon->Bind(m_lpBlade->m_pBindMesh,(LPSTR)pScoket->scName.c_str());
		}

		{
			CFileDialog dlg(true, "Mtl", 0, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, "剑网3D材质文件(*.Mtl)|*.Mtl||");
			if(dlg.DoModal() != IDOK)
			{
				return;
			}

			wsprintf(FileName,"%s",dlg.GetPathName());

			if(m_lpModelWeapon)
			{
				m_lpModelWeapon->LoadMaterial(FileName);
			}
		}
	}*/
}

void KSceneModelEditorSFXBladeDialog::OnBnClickedButtonAutoani2()
{
	// TODO: Add your control notification handler code here
	/*if(!m_lpBlade)
		return;
	if(!m_lpBlade->m_pBindMesh)
		return;

	int Index = m_ListBox_Ani.GetCurSel();
	if(Index<0)
		return;
	DWORD i = m_ListBox_Ani.GetItemData(Index);

	if ((i>=0)&&(i<m_lpBlade->m_pBindMesh->m_lpAnimations.size()))
	{
		KModel::LPANIMATION pAniMesh = m_lpBlade->m_pBindMesh->m_lpAnimations[i];

		if(m_lpBlade->FindAnimation(pAniMesh->Name)>=0)
		{
			MessageBox("改动作已经存在","错误",0);
			return;
		}
		m_lpBlade->Add1NewAnimation(pAniMesh->Name,pAniMesh->NumFrames,pAniMesh->FrameLength);
	}
	FillListBoxAni();*/
}

void KSceneModelEditorSFXBladeDialog::OnEnChangeEditSelectlength()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
	UpdateData(TRUE);
}

void KSceneModelEditorSFXBladeDialog::OnBnClickedButtonHideselect2()
{
	// TODO: Add your control notification handler code here
	// TODO: Add your control notification handler code here
	/*if(!m_lpBlade)
		return;
	if(!m_lpBlade->m_pBindMesh)
		return;

	int Index = m_ListBox_Ani.GetCurSel();
	if(Index<0)
		return;
	DWORD i = m_ListBox_Ani.GetItemData(Index);

	if ((i>=0)&&(i<m_lpBlade->m_pBindMesh->m_lpAnimations.size()))
	{
		KModel::LPANIMATION pAni = m_lpBlade->m_pBindMesh->m_lpAnimations[i];

		int i_BladeAni = m_lpBlade->FindAnimation(pAni->Name);
		if ((i_BladeAni>=0)&&(i_BladeAni<(int)m_lpBlade->m_lpAnimations.size()))
		{
			KModel::LPANIMATION pAniBlade = m_lpBlade->m_lpAnimations[i_BladeAni];

			if(pAniBlade->BladeData)
			{
				KModel::_Animation::_Blade_ShowLength* pData
					= (KModel::_Animation::_Blade_ShowLength*) pAniBlade->BladeData;
				for(int k=0;k<=(int)pAniBlade->NumFrames;k++)
				{
					pData[k].bShow = FALSE;
				}
			}
		}
		else
		{
			MessageBox("请先为刀光匹配动作","错误",MB_OK);
		}
	}
	else
	{
		MessageBox("没有指定动作文件","错误",MB_OK);
	}*/
}
