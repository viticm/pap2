////////////////////////////////////////////////////////////////////////////////
//  Copyright(c) Kingsoft
//
//  FileName    : KG_JX3LauncherDlg.h
//  Version     : 1.0
//  Creator     : Hu Hao
//  Create Date : 2009-10-29  15:23:38
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <string>
#include <list>
#include "KG_BitmapButton.h"
#include "KG_StaticText.h"

////////////////////////////////////////////////////////////////////////////////////////////////////
#if defined(UNICODE) || defined(_UNICODE)
typedef std::wstring tstring;
#else
typedef std::string tstring;
#endif

////////////////////////////////////////////////////////////////////////////////////////////////////
class KG_JX3LauncherDlg : public CDialog
{
public:
	KG_JX3LauncherDlg(CWnd* pParent = NULL);

private:
	enum { IDD = IDD_JX3LAUNCHER_DIALOG };
	virtual void    DoDataExchange(CDataExchange* pDX);
	virtual BOOL    OnInitDialog();
	afx_msg void    OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
    afx_msg LRESULT OnNcHitTest(CPoint point);
    afx_msg BOOL    OnEraseBkgnd(CDC* pDC);
    afx_msg void    OnDestroy();
    afx_msg void    OnOK();
    afx_msg void    OnBnClickedButton();
    afx_msg LRESULT OnMouseInLaunchButton(WPARAM wParam, LPARAM lParam);
    afx_msg LRESULT OnMouseLeaveLaunchButton(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()
private:
    int _LoadConfigParameters();
    int _InitBackgroundBitmap();
    int _InitDialogRegion();
    int _InitLaunchButtons();
    int _InitQuitButton();
    int _InitClueRegion();

    int _LoadItemExe(const TCHAR cszExePath[]);
private:
    struct _LAUNCH_ITEM
    {
        tstring             strPath;
        tstring             strName;
        tstring             strClue;
        KG_BitmapButton    *pButton;
    };
    typedef std::list<_LAUNCH_ITEM> _LAUNCH_ITEM_LIST;

private:
    HICON               m_hIcon;
    CBitmap             m_BackgroundBitmap;
    CFont               m_LaunchButtonTextFont;
    KG_BitmapButton     m_QuitButton;
    KG_StaticText       m_ClueText;
    _LAUNCH_ITEM_LIST   m_LaunchItemList;
};
