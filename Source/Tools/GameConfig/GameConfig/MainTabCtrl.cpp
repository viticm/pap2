// MainTabCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "GameConfig.h"
#include "MainTabCtrl.h"

#include "KGPublic.h"
#include "kglog.h"

// KMainTabCtrl

IMPLEMENT_DYNAMIC(KMainTabCtrl, CTabCtrl)

KMainTabCtrl::KMainTabCtrl()
{
	m_nCurrent  = 0;
}

KMainTabCtrl::~KMainTabCtrl()
{
}

BEGIN_MESSAGE_MAP(KMainTabCtrl, CTabCtrl)
	ON_WM_LBUTTONDOWN()
END_MESSAGE_MAP()

int KMainTabCtrl::Init()
{
	InsertItem(0, _T("视频设置"));
	InsertItem(1, _T("硬件信息"));

	m_ConfigPage.Create(IDD_CONFIG_DLG, this);
	m_ClientInfo.Create(IDD_INFORMATON_DLG, this);

	m_ConfigPage.ShowWindow(SW_SHOW);
	m_ClientInfo.ShowWindow(SW_HIDE);

	m_ConfigPage.Init();
	_SetRectangle();

	return true;
}

void KMainTabCtrl::ConfigSave()
{
	m_ConfigPage.Save();
}



void KMainTabCtrl::_SetRectangle()
{
	CRect rctTabRect, rctItemRect;
	int nX  = 0; 
	int nY  = 0;
	int nXc = 0;
	int nYc = 0;

	GetClientRect(&rctTabRect);
	GetItemRect(0, &rctItemRect);

	nX  = rctItemRect.left;
	nY  = rctItemRect.bottom + 1;
	nXc = rctTabRect.right - rctItemRect.left - 1;
	nYc = rctTabRect.bottom - nY -1;

	switch (m_nCurrent)
	{
	case 0:
		m_ConfigPage.SetWindowPos(&wndTop, nX, nY, nXc, nYc, SWP_SHOWWINDOW);
		m_ClientInfo.SetWindowPos(&wndTop, nX, nY, nXc, nYc, SWP_HIDEWINDOW);
		break;
	case 1:
		m_ConfigPage.SetWindowPos(&wndTop, nX, nY, nXc, nYc, SWP_HIDEWINDOW);
		m_ClientInfo.SetWindowPos(&wndTop, nX, nY, nXc, nYc, SWP_SHOWWINDOW);
		break;
	default:
		m_ConfigPage.ShowWindow(SW_HIDE);
		m_ClientInfo.ShowWindow(SW_HIDE);
		break;
	}
}


void KMainTabCtrl::OnLButtonDown(UINT nFlags, CPoint point) 
{
	CTabCtrl::OnLButtonDown(nFlags, point);

	switch (GetCurFocus())
	{
	case 0:
		m_ConfigPage.ShowWindow(SW_SHOW);
		m_ClientInfo.ShowWindow(SW_HIDE);
		break;
	case 1:
		m_ClientInfo.Init();
		m_ConfigPage.ShowWindow(SW_HIDE);
		m_ClientInfo.ShowWindow(SW_SHOW);
		break;
	default:
		m_ConfigPage.ShowWindow(SW_HIDE);
		m_ClientInfo.ShowWindow(SW_HIDE);
		break;
	}
}

