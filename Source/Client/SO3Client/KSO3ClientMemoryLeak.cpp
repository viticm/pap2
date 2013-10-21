#include "stdafx.h"
#include "KSO3Client.h"

#define _SO3_MEMORY_LEAK_DETECT_

#ifdef _SO3_MEMORY_LEAK_DETECT_

#define nNoMansLandSize 4

typedef struct _CrtMemBlockHeader
{
        struct _CrtMemBlockHeader * pBlockHeaderNext;
        struct _CrtMemBlockHeader * pBlockHeaderPrev;
        char *                      szFileName;
        int                         nLine;
#ifdef _WIN64
        /* These items are reversed on Win64 to eliminate gaps in the struct
         * and ensure that sizeof(struct)%16 == 0, so 16-byte alignment is
         * maintained in the debug heap.
         */
        int                         nBlockUse;
        size_t                      nDataSize;
#else  /* _WIN64 */
        size_t                      nDataSize;
        int                         nBlockUse;
#endif  /* _WIN64 */
        long                        lRequest;
        unsigned char               gap[nNoMansLandSize];
        /* followed by:
         *  unsigned char           data[nDataSize];
         *  unsigned char           anotherGap[nNoMansLandSize];
         */
} _CrtMemBlockHeader;

#define KALLOC_RECORD_COUNT 2048
static long g_lAllocCounter[KALLOC_RECORD_COUNT];
static long g_lAllocCounterBackBuffer[KALLOC_RECORD_COUNT];
static long g_lTotalAlloc = 0;
static DWORD g_dwLastMemoryCheckFrame = 0;
static DWORD g_dwLastMemoryCheckTickCount = 0;

static int MyAllocHook(int nAllocType, void* pvData, size_t nSize, int nBlockUse, long lRequest, const unsigned char * szFileName, int nLine)
{
    size_t              uAllocSize      = 0;
    size_t              uFreeSize       = 0;
    _CrtMemBlockHeader* pHeader         = NULL;
    int                 nRecordIndex    = 0;

    switch (nAllocType)
    {
    case _HOOK_ALLOC:

        uAllocSize = sizeof(_CrtMemBlockHeader) + nSize + 4;
        InterlockedExchangeAdd(&g_lTotalAlloc, (long)uAllocSize);
        nRecordIndex = (int)uAllocSize / 16;
        KG_PROCESS_ERROR(nRecordIndex < KALLOC_RECORD_COUNT);
		InterlockedIncrement(g_lAllocCounter + nRecordIndex);

        break;

    case _HOOK_REALLOC:

        if (pvData)
        {
            pHeader = (_CrtMemBlockHeader*)pvData - 1;

            if (pHeader->nDataSize > 0)
            {
                uFreeSize = sizeof(_CrtMemBlockHeader) + pHeader->nDataSize + 4;
                InterlockedExchangeAdd(&g_lTotalAlloc, -(long)uFreeSize);
            }

            nRecordIndex = (int)uFreeSize / 16;
            if (nRecordIndex < KALLOC_RECORD_COUNT)
            {
                InterlockedDecrement(g_lAllocCounter + nRecordIndex);
            }
        }
        
        if (nSize > 0)
        {
            uAllocSize = sizeof(_CrtMemBlockHeader) + nSize + 4;
            InterlockedExchangeAdd(&g_lTotalAlloc, (long)uAllocSize);
        }

        nRecordIndex = (int)uAllocSize / 16;
        if (nRecordIndex < KALLOC_RECORD_COUNT)
        {
            InterlockedIncrement(g_lAllocCounter + nRecordIndex);
        }

        break;

    case _HOOK_FREE:

        KG_PROCESS_ERROR(pvData);
        pHeader = (_CrtMemBlockHeader*)pvData - 1;
        uFreeSize = sizeof(_CrtMemBlockHeader) + pHeader->nDataSize + 4;
        InterlockedExchangeAdd(&g_lTotalAlloc, -(long)uFreeSize);
        nRecordIndex = (int)uFreeSize / 16;
        KG_PROCESS_ERROR(nRecordIndex < KALLOC_RECORD_COUNT);
        InterlockedDecrement(g_lAllocCounter + nRecordIndex);

        break;

    default:
        ASSERT(!"Unexpected memory operate !");
    }

Exit0:
	return true;
}

#endif // _SO3_MEMORY_LEAK_DETECT_

void KSO3Client::MemoryLeakDetect()
{
#ifdef _SO3_MEMORY_LEAK_DETECT_
    DWORD dwTickCount = KG_GetTickCount();
    if (dwTickCount > g_dwLastMemoryCheckTickCount + 10000)
    {        
        long lAllocCounterBackBuffer[KALLOC_RECORD_COUNT];

        for (int i = 0; i < KALLOC_RECORD_COUNT; ++i)
            lAllocCounterBackBuffer[i] = g_lAllocCounter[i] - g_lAllocCounterBackBuffer[i];

        memcpy(g_lAllocCounterBackBuffer, g_lAllocCounter, sizeof(g_lAllocCounterBackBuffer));

        printf("------------------ Total %d M ------------------------\n", g_lTotalAlloc / 1024 / 1024);

        for (int i = 0; i < KALLOC_RECORD_COUNT; i++)
        {
            int nDeltaBlock = lAllocCounterBackBuffer[i];

            if (nDeltaBlock > 4)
            {
                printf("size(%d) --> %d\n", i, nDeltaBlock);
            }
        }

        printf("\n");

        g_dwLastMemoryCheckTickCount = dwTickCount;
    }
#endif // #ifdef _SO3_MEMORY_LEAK_DETECT_
}
