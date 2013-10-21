#pragma once
#include "afxwin.h"

class CExtractImageApp :
	public CWinApp
{
public:
	CExtractImageApp(void);
	~CExtractImageApp(void);

	virtual BOOL InitInstance();
	virtual int ExitInstance();

	BOOL CreateThumbnail(HBITMAP* phBmpThumbnail, LPCSTR lpFileName);


};
