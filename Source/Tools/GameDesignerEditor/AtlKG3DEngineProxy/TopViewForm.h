// TopViewForm.h : Declaration of the CTopViewForm

#pragma once

#include "resource.h"       // main symbols

#include <atlhost.h>

#include "IEEditor.h"

// CTopViewForm

class CTopViewForm : 
	public CAxDialogImpl<CTopViewForm>
{
public:
	CTopViewForm()
	{
	}

	~CTopViewForm()
	{
	}

	enum { IDD = IDD_TOPVIEWFORM };

BEGIN_MSG_MAP(CTopViewForm)
	MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
	COMMAND_HANDLER(IDOK, BN_CLICKED, OnClickedOK)
	COMMAND_HANDLER(IDCANCEL, BN_CLICKED, OnClickedCancel)
	MESSAGE_HANDLER(WM_CLOSE, OnClose)
	MESSAGE_HANDLER(WM_LBUTTONDOWN, OnLButtonDown)
	MESSAGE_HANDLER(WM_MBUTTONDOWN, OnMButtonDown)
	MESSAGE_HANDLER(WM_RBUTTONDOWN, OnRButtonDown)
	MESSAGE_HANDLER(WM_MOUSEMOVE, OnMouseMove)
	MESSAGE_HANDLER(WM_LBUTTONUP, OnLButtonUp)
	MESSAGE_HANDLER(WM_MBUTTONUP, OnMButtonUp)
	MESSAGE_HANDLER(WM_RBUTTONUP, OnRButtonUp)
	MESSAGE_HANDLER(WM_LBUTTONDBLCLK, OnLButtonDblClk)
	CHAIN_MSG_MAP(CAxDialogImpl<CTopViewForm>)
END_MSG_MAP()

// Handler prototypes:
//  LRESULT MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
//  LRESULT CommandHandler(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
//  LRESULT NotifyHandler(int idCtrl, LPNMHDR pnmh, BOOL& bHandled);

	LRESULT OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		CAxDialogImpl<CTopViewForm>::OnInitDialog(uMsg, wParam, lParam, bHandled);
		bHandled = TRUE;
		return 1;  // Let the system set the focus
	}

	LRESULT OnClickedOK(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
	{
		EndDialog(wID);
		return 0;
	}

	LRESULT OnClickedCancel(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
	{
		EndDialog(wID);
		return 0;
	}

public:
	IEKG3DSceneSceneEditor* m_lpSceneEditor;
	int m_nOutputWndID;
	CPoint m_LastMousePos;

	CPoint m_MouseDown;
	D3DXVECTOR3 m_vCameraPos;

	DWORD m_dwSelectionPoint;
	BOOL  m_bSelecting;
	BOOL  m_bAddPolyVertex;

	BOOL  m_bInited;

	BOOL  m_bLBPushed;
	BOOL  m_bMBPushed;
	BOOL  m_bRBPushed;

	void  InitTopView(IEKG3DSceneSceneEditor* pEditor);
	void  UnInitTopView();
	LRESULT OnClose(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnLButtonDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnMButtonDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnRButtonDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnMouseMove(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnLButtonUp(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnMButtonUp(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnRButtonUp(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnLButtonDblClk(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
};


