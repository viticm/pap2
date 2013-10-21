#include "stdafx.h"
#include "KProtocolReplayer.h"
#include "KSO3World.h"

HRESULT STDMETHODCALLTYPE KProtocolReplayer::QueryInterface(REFIID riid, void** ppvObject)
{
    KG_USE_ARGUMENT(riid);
    KG_USE_ARGUMENT(ppvObject);
    return KG_E_FAIL;
}

ULONG STDMETHODCALLTYPE KProtocolReplayer::AddRef(void)
{
    return (ULONG)KG_InterlockedIncrement((long*)&m_ulRefCount);
}

ULONG STDMETHODCALLTYPE KProtocolReplayer::Release(void)
{
    ULONG ulCount = KG_InterlockedDecrement((long*)&m_ulRefCount);
    if (ulCount == 0)
    {
        if (m_nRecordFile != -1)
        {
            close(m_nRecordFile);
            m_nRecordFile = -1;
        }
        delete this;
    }
    return ulCount;
}

BOOL KProtocolReplayer::Pop(void* pvBuffer, size_t uBufferSize)
{
    BOOL    bResult         = false;
    int     nRetCode        = -1;
    size_t  uCacheDataSize  = m_uCacheEndPos - m_uCacheCurPos;
    BYTE*   pbyDestBuffer   = (BYTE*)pvBuffer;

    assert(pvBuffer);
   
    if (uBufferSize == 0)
    {
        goto Exit1;
    }

    if (uBufferSize > uCacheDataSize)
    {
        memcpy(pbyDestBuffer, m_byCache + m_uCacheCurPos, uCacheDataSize);

        uBufferSize    -= uCacheDataSize;
        pbyDestBuffer  += uCacheDataSize;

        nRetCode = Cache();
        KGLOG_PROCESS_ERROR(nRetCode);
    }

    uCacheDataSize  = m_uCacheEndPos - m_uCacheCurPos;
    KGLOG_PROCESS_ERROR(uCacheDataSize >= uBufferSize);

    memcpy(pbyDestBuffer, m_byCache + m_uCacheCurPos, uBufferSize);
    m_uCacheCurPos += uBufferSize;

Exit1:
    bResult = true;
Exit0:
    return bResult;
}

BOOL KProtocolReplayer::Cache()
{
    BOOL    bResult    = false;
    BOOL    bRetCode   = false;
    int     nRetCode   = 0;

    m_uCacheCurPos  = 0;
    m_uCacheEndPos  = 0;

    while (true)
    {
        if (m_nRecordFile == -1)
        {
            bRetCode = OpenFileByIndex();
            KGLOG_PROCESS_ERROR(bRetCode);
        }

        nRetCode = read(m_nRecordFile, m_byCache, KREPLAY_CACHE_SIZE);
        if (nRetCode > 0)
        {
            m_uCacheCurPos = 0;
            m_uCacheEndPos = nRetCode;
            break;
        }

        KGLOG_PROCESS_ERROR(nRetCode == 0);

        close(m_nRecordFile);
        m_nRecordFile = -1;
    }

    bResult = true;
Exit0:
    return bResult;
}

BOOL KProtocolReplayer::OpenFileByIndex()
{
    BOOL bResult    = false;
    int  nRetCode   = false;
    char szFullFileName[MAX_PATH];

    if (m_nRecordFile != -1)
    {
        close(m_nRecordFile);
        m_nRecordFile = -1;
    }

    nRetCode = (int)snprintf(szFullFileName, sizeof(szFullFileName), "%s/gs_%d", m_szDirName, m_nFileIndex);
    KGLOG_PROCESS_ERROR(nRetCode > 0 && nRetCode < (int)sizeof(szFullFileName));

#ifdef WIN32    
    m_nRecordFile = open(szFullFileName, O_RDONLY | O_BINARY);
#else
    m_nRecordFile = open(szFullFileName, O_RDONLY);
#endif

    if (m_nRecordFile == -1)
    {
        KGLogPrintf(KGLOG_DEBUG, "[Replay] Failed to open file: %s", szFullFileName);

#ifdef _SERVER
        if (g_pSO3World)
        {
            g_pSO3World->m_bRunFlag = false;
        }
#endif
        goto Exit0;
    }

    m_nFileIndex++;
    
    bResult = true;
Exit0:
    if (!bResult)
    {
        if (m_nRecordFile != -1)
        {
            close(m_nRecordFile);
            m_nRecordFile = -1;
        }
    }
    return bResult;
}
