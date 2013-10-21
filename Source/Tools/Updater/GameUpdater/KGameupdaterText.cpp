#include "StdAfx.h"
#include "KGameupdaterText.h"

HINSTANCE KGameupdaterText::ms_hInstance = NULL;

KGameupdaterText::KGameupdaterText(void)
{
	if (KGameupdaterText::ms_hInstance == NULL)
		KGameupdaterText::ms_hInstance = GetModuleHandle(NULL);

	m_szText[0] = _T('\0');
}

KGameupdaterText::~KGameupdaterText(void)
{
}

const tstring KGameupdaterText::GetText(int nTextID)
{
	LoadString(KGameupdaterText::ms_hInstance, nTextID, m_szText, sizeof(m_szText));
	return tstring(m_szText);
}