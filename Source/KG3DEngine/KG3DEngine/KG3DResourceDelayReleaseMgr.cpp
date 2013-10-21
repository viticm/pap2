#include "StdAfx.h"
#include "KG_InterlockSingleList.h"
#include "KG3DResourceDelayReleaseMgr.h"
#include "KMemory.h"

struct RESOURCE_BLOCK
{
    IUnknown *pResource;
    KG_SLIST_ENTRY Link;
};

KG_SLIST_HEADER     g_ResourceListHead;
BOOL                g_bDelayRelease = FALSE;;

HRESULT KG3DResourceDelayReleaseMgr::Init()
{
    KG_InitializeSListHead(&g_ResourceListHead);
    g_bDelayRelease = TRUE;

    return S_OK;
}

HRESULT KG3DResourceDelayReleaseMgr::UnInit()
{
    g_bDelayRelease = FALSE;
    ReleaseAll();

#ifdef _DEBUG
    KG_SLIST_ENTRY *pList = KG_InterlockedFlushSList(&g_ResourceListHead);
    ASSERT(!pList);
#endif 

    return S_OK;
}

HRESULT KG3DResourceDelayReleaseMgr::Push(IUnknown *pResource)
{
    HRESULT         hrResult        = E_FAIL;
    RESOURCE_BLOCK *pResourceBlock  = NULL;

    KG_PROCESS_SUCCESS(!pResource);
    
    if (g_bDelayRelease)
    {
        pResourceBlock = (RESOURCE_BLOCK *)KMemory::RawAlloc(sizeof(RESOURCE_BLOCK));
        KGLOG_PROCESS_ERROR(pResourceBlock);

        pResourceBlock->pResource = pResource;
        pResourceBlock->Link.Next = NULL;

        KG_InterlockedPushEntrySList(&g_ResourceListHead, &pResourceBlock->Link);
    }
    else
    {
        SAFE_RELEASE(pResource);
    }

Exit1:
    hrResult = S_OK;
Exit0:
    return hrResult;
}

HRESULT KG3DResourceDelayReleaseMgr::ReleaseAll()
{
    KG_SLIST_ENTRY *pList           = NULL;
    RESOURCE_BLOCK *pResourceBlock  = NULL;

    pList = KG_InterlockedFlushSList(&g_ResourceListHead);

    while (pList)
    {
        pResourceBlock = CONTAINING_RECORD(pList, RESOURCE_BLOCK, Link);
        ASSERT(pResourceBlock);

        SAFE_RELEASE(pResourceBlock->pResource);

        pList = pList->Next;

        KMemory::RawFree(pResourceBlock, sizeof(*pResourceBlock));
    }

    return S_OK;
}
