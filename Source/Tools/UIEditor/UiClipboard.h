////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : UiClipboard.h
//  Version     : 1.0
//  Creator     : Hu Changyin
//  Create Date : 2006-2-8 9:22:51
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _INCLUDE_UICLIPBOARD_H_
#define _INCLUDE_UICLIPBOARD_H_

#include "UiEditor.h"

////////////////////////////////////////////////////////////////////////////////

class KUiClipBoard
{
protected:
	KUiClipBoard();
	~KUiClipBoard();
public:
	static int Init();
	static int Exit();
	static KUiClipBoard &GetSelf();

	int Copy();
	int Past();

	int CopyAndSave();
	int PastFromSave();

private:
	HWND m_hWnd;
	static KUiClipBoard *ms_pSelf;

	IIniFile *m_pIniSave;
	IIniFile *m_pIniTemp;
};

#endif //_INCLUDE_UICLIPBOARD_H_
