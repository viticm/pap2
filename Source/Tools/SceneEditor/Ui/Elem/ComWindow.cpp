// -------------------------------------------------------------------------
//	文件名		：	ComWindow.cpp
//	创建者		：	万里
//	创建时间	：	2003-7-24 10:34:26
//	功能描述	：
//
// -------------------------------------------------------------------------

#include "../../Public/KWin32.h"
#include "ComWindow.h"
#include "string.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

////////////////////////////////////////////////////////////////////////////////


static KComClassFactory gs_Factory;

KComClassFactory* gGetComClassFactory()
{
	return &gs_Factory;
}

#define MAX_CLASSINFO 100
static KComClassInfo g_ClassInfs[MAX_CLASSINFO];
static int g_nClassCount;

KComClassFactory::KComClassFactory()
{
	g_nClassCount = 0;
}

KComClassInfo* KComClassFactory::FindComClass(const char* szClassType)
{
	for (int i = 0; i < g_nClassCount; i++)
	{
		if (strcmp(szClassType, g_ClassInfs[i].szClassType) == 0)
			return g_ClassInfs + i;
	}

	return NULL;
}

KComClassInfo* KComClassFactory::FindComClass(const KComClassInfo* pDest)
{
	if (pDest == NULL)
		return NULL;
	return FindComClass(pDest->szClassType);
}

int KComClassFactory::RegisterComClass(const KComClassInfo& one)
{
	if (!FindComClass(one.szClassType))
	{
		if (g_nClassCount < MAX_CLASSINFO - 1)
		{
			g_ClassInfs[g_nClassCount] = one;
			g_nClassCount++;
			return g_nClassCount;
		}
	}

	return 0;
}

KWndWindow* KComClassFactory::CreateComObject(const char* szClassType)
{
	KComClassInfo* p = FindComClass(szClassType);
	if (p && p->pfnCreateObject)
	{
		return p->pfnCreateObject();
	}

	return NULL;
}

