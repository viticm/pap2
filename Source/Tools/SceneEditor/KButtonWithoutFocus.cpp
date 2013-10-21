// KButtonWithoutFocus.cpp : implementation file
//

#include "stdafx.h"
#include "KButtonWithoutFocus.h"

namespace PropertyBase
{


// KButtonWithoutFocus

IMPLEMENT_DYNAMIC(KButtonWithoutFocus, CButton)
KButtonWithoutFocus::KButtonWithoutFocus()
{
}

KButtonWithoutFocus::~KButtonWithoutFocus()
{
}


BEGIN_MESSAGE_MAP(KButtonWithoutFocus, CButton)
END_MESSAGE_MAP()



// KbuttonWithoutFocus message handlers


void KButtonWithoutFocus::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	
	UINT uStyle = DFCS_BUTTONPUSH;

	ASSERT(lpDrawItemStruct->CtlType == ODT_BUTTON);

	if (lpDrawItemStruct->itemState & ODS_SELECTED)
		uStyle |= DFCS_PUSHED;

	::DrawFrameControl(lpDrawItemStruct->hDC, &lpDrawItemStruct->rcItem, 
		DFC_BUTTON, uStyle);

	UINT uButtonStyle = GetButtonStyle();
	
	if ( uButtonStyle & BS_BITMAP )//Button with bitmaps
	{
		HBITMAP hBmp = NULL;
		
		RECT rect;
		GetClientRect(&rect);
		int iHeight = max(rect.bottom - rect.top - 2, 0);
		int iWidth = max(rect.right - rect.left - 2, 0);
		if ( (hBmp = GetBitmap()) != NULL )
		{
			BITMAP bmpInfo;
			GetObject(hBmp, sizeof(BITMAP), &bmpInfo);
			int iX = max( (iWidth - bmpInfo.bmWidth) / 2, 2);
			int iY = max( (iHeight - bmpInfo.bmHeight) / 2, 2);
			if (uStyle & DFCS_PUSHED)
				iY++;
			HDC hDc = ::CreateCompatibleDC(lpDrawItemStruct->hDC);
			SelectObject(hDc, hBmp);
			BitBlt(lpDrawItemStruct->hDC, iX, iY, iHeight, iWidth, hDc, 0, 0, SRCCOPY);
			DeleteDC(hDc);
		}
	}
	else//Text displayed
	{
		CString strText;
		GetWindowText(strText);

		::DrawText(lpDrawItemStruct->hDC, strText, strText.GetLength(), 
			&lpDrawItemStruct->rcItem, DT_SINGLELINE|DT_VCENTER|DT_CENTER);

	}
}

}
