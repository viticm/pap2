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
#pragma once

#include "KG3DMessenger.h"
// KDlgLogicSaveProgress dialog

class KDlgLogicSaveProgress : public CDialog , public KG3DMessageSingleSenderReceiver
{
	DECLARE_DYNAMIC(KDlgLogicSaveProgress)

public:
	KDlgLogicSaveProgress(CWnd* pParent = NULL);   // standard constructor
	virtual ~KDlgLogicSaveProgress();

// Dialog Data
	enum { IDD = IDD_LOGIC_CELL_SAVE_PROGRESS };

    CProgressCtrl   m_progressModel;
    CStatic m_staticModel;
    CStatic m_staticFace;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void __stdcall OnReceiveMessage( const KG3DMessage& Message , IKG3DMessageSenderBase* pSender );
	DECLARE_MESSAGE_MAP()
};
