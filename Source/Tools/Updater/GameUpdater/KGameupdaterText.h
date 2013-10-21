#ifndef _KGAMEUPDATERTEXT_H_
#define _KGAMEUPDATERTEXT_H_

#include "resource.h"

class KGameupdaterText
{
public:
	KGameupdaterText(void);
	~KGameupdaterText(void);

public:
	const tstring GetText(int nTextID);

private:
	static HINSTANCE ms_hInstance;
	TCHAR m_szText[1024];
};

#endif