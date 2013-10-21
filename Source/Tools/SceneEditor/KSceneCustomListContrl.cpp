// KSceneCustomListContrl.cpp : implementation file
//

#include "stdafx.h"
#include "SceneEditor.h"
#include "KSceneCustomListContrl.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
// KSceneCustomListContrl


namespace KSCENEEDITOR_HELPERCLASSES
{
	namespace COMMON_CTRL
	{

		IMPLEMENT_DYNAMIC(KSceneCustomListContrlBase, CListBox)

		KSceneCustomListContrlBase::KSceneCustomListContrlBase()
		{

		}

		KSceneCustomListContrlBase::~KSceneCustomListContrlBase()
		{
		}


		BEGIN_MESSAGE_MAP(KSceneCustomListContrlBase, CListBox)
		END_MESSAGE_MAP()

		int KSceneCustomListContrlBase::SetCurSel( int nSelect )
		{
			HWND hMyWnd = this->GetSafeHwnd();
			HWND hWndParent = ::GetParent(hMyWnd);
			INT nMyID = this->GetDlgCtrlID();
			::PostMessage(hWndParent, WM_COMMAND, MAKELONG(nMyID, LBN_SELCHANGE), reinterpret_cast<LPARAM>(hMyWnd));
			return CListBox::SetCurSel(nSelect);
		}
	};
};

// KSceneCustomListContrl message handlers


