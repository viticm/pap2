#include "stdafx.h"
#include "KProtocolRecorder.h"
#include "KProtocolRecorderBase.h"
#include "KSO3World.h"

#define MAX_RECORD_FILE_SIZE 1024 * 1024 * 500

HRESULT STDMETHODCALLTYPE KProtocolRecorder::QueryInterface(REFIID riid, void** ppvObject)
{
    KG_USE_ARGUMENT(riid);
    KG_USE_ARGUMENT(ppvObject);
    return KG_E_FAIL;
}

ULONG STDMETHODCALLTYPE KProtocolRecorder::AddRef(void)
{
    return (ULONG)KG_InterlockedIncrement((long*)&m_ulRefCount);
}

ULONG STDMETHODCALLTYPE KProtocolRecorder::Release(void)
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

BOOL KProtocolRecorder::Push(void* pvData, size_t uDataLen)
{
    BOOL bResult    = false;
    BOOL bRetCode   = false;
    int  nRetCode   = false;
    
    if (uDataLen == 0)
    {
        goto Exit1;
    }

    assert(pvData);
    
    if (m_nRecordFile == -1 || (m_uFileSize + uDataLen >= MAX_RECORD_FILE_SIZE))
    {
        if (m_nRecordFile != -1)
        {
            close(m_nRecordFile);
            m_nRecordFile = -1;
        }
        
        bRetCode = OpenFileByIndex();
        KGLOG_PROCESS_ERROR(bRetCode);
    }

    nRetCode = write(m_nRecordFile, pvData, (unsigned)uDataLen);
    KGLOG_PROCESS_ERROR(nRetCode != -1);

    m_uFileSize += uDataLen;

Exit1:
    bResult = true;
Exit0:
    return bResult;
}

BOOL KProtocolRecorder::OpenFileByIndex()
{
    BOOL bResult    = false;
    int  nRetCode   = 0;
    char szFullFileName[MAX_PATH];

    nRetCode = (int)snprintf(szFullFileName, sizeof(szFullFileName), "%s/gs_%d", m_szDirName, m_nFileIndex);
    KGLOG_PROCESS_ERROR(nRetCode > 0 && nRetCode < (int)sizeof(szFullFileName));

#ifdef WIN32
    m_nRecordFile = open(szFullFileName, (O_CREAT | O_APPEND | O_WRONLY | O_TRUNC | O_BINARY), _S_IREAD | _S_IWRITE);
#else
    m_nRecordFile = open(szFullFileName, (O_CREAT | O_APPEND | O_WRONLY | O_TRUNC), S_IRUSR | S_IWUSR);
#endif

    if (m_nRecordFile == -1)
    {
        KGLogPrintf(KGLOG_DEBUG, "[Record] Failed to open file: %s", szFullFileName);

#ifdef _SERVER
        if (g_pSO3World)
        {
            g_pSO3World->m_bRunFlag = false;
        }
#endif
        goto Exit0;
    }
    
    ++m_nFileIndex;
    
    m_uFileSize = 0;

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
