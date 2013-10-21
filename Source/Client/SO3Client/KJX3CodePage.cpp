#include "stdafx.h"
#include "./KJX3CodePage.h"
#include "SO3UI/KJX3UIDefine.h"

static unsigned int g_uCodePage = CODE_PAGE_GBK;

int  (unsigned int uCodePage)
{
	g_uCodePage = uCodePage;
	return true;
}

unsigned int GetCodePage()
{
	return g_uCodePage;
}
