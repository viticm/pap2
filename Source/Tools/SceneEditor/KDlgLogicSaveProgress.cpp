// ***************************************************************
//
//  Description:
//  -------------------------------------------------------------
//	Author: Zeb
//  -------------------------------------------------------------
//	Create Date: 2006-06-12
//	-------------------------------------------------------------
//	History:
//  -------------------------------------------------------------
//  Copyright (C) 2006 - All Rights Reserved
//
// ***************************************************************
// KDlgLogicSaveProgress.cpp : implementation file
//

#include "stdafx.h"
#include "SceneEditor.h"
#include "KDlgLogicSaveProgress.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

////////////////////////////////////////////////////////////////////////////////


// KDlgLogicSaveProgress dialog

IMPLEMENT_DYNAMIC(KDlgLogicSaveProgress, CDialog)

KDlgLogicSaveProgress::KDlgLogicSaveProgress(CWnd* pParent /*=NULL*/)
	: CDialog(KDlgLogicSaveProgress::IDD, pParent)
{
    Create(IDD);
    Invalidate();
    BringWindowToTop();
}

KDlgLogicSaveProgress::~KDlgLogicSaveProgress()
{
}

void KDlgLogicSaveProgress::DoDataExchange(CDataExchange* pDX)
{
    DDX_Control(pDX, IDC_PROGRESS_MODEL, m_progressModel);
    DDX_Control(pDX, IDC_STATIC_MODEL, m_staticModel);

    DDX_Control(pDX, IDC_STATIC_FACE, m_staticFace);

	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(KDlgLogicSaveProgress, CDialog)
END_MESSAGE_MAP()

#define  KGWM_RELEASE_LISTENER (WM_USER+400)
void __stdcall KDlgLogicSaveProgress::OnReceiveMessage( const KG3DMessage& Message , IKG3DMessageSenderBase* pSender )
{
	if (Message.m_uMessage == KM_SCENE_LOADING_PERCENT)
	{
		KG3DMessageSceneLoadingPercent* pPercent = (KG3DMessageSceneLoadingPercent*)Message.m_dwWParam;
		TCHAR	Buffer[MAX_PATH];
		Buffer[MAX_PATH-1] = '\0';
		_stprintf_s(Buffer, _T("加载地形(千份之)：%d"), (int)(pPercent->TerrainLoadingPercent * 1000));
			/*, (int)(pPercent->ObjectLoadingPercent * 1000)
			, (int)(pPercent->RestLoadingPercent * 1000)
			);*/
		int nPos =(int)(pPercent->TerrainLoadingPercent * 100);
		m_progressModel.SetPos(nPos);
		this->SetWindowText(Buffer);
		this->RedrawWindow();
	}
}
// KDlgLogicSaveProgress message handlers
