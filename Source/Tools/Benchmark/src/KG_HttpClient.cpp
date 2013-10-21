#include "stdafx.h"
#include "KG_HttpClient.h"
#include <tchar.h>

#define KG_INTERNET_HANDLE_CLOSE(hHandle) \
    do  \
    {   \
        if ((hHandle))   \
        {   \
            ::InternetCloseHandle((hHandle));   \
            (hHandle) = NULL;   \
        }   \
    } while (false)

KG_HttpClient::KG_HttpClient(void)
{
    m_hInternetSession = NULL;
    m_hInternetConnection = NULL;
    m_hHttpRequest = NULL;
    m_pResponseBuffer = NULL;
    m_dwBufferSize = 0;
}

KG_HttpClient::~KG_HttpClient(void)
{
    KG_DELETE_ARRAY(m_pResponseBuffer);
    ASSERT(m_PostList.empty());
    ASSERT(!m_hInternetSession);
    ASSERT(!m_hInternetConnection);
    ASSERT(!m_hHttpRequest);
}

int KG_HttpClient::RequestPost(
   const TCHAR cszAgent[], 
   const TCHAR cszAddress[], 
   const TCHAR cszVerb[],
   int nPort
)
{
    int nResult = false;
    int nRetCode = false;
    LPCSTR clpAcceptTypeArray[] = {"*/*", NULL};
    BYTE *pbyBuffer = NULL;
    DWORD dwBufferSize = 0;
    DWORD dwBytesWrite = 0;
    DWORD dwBytesRead = 0;
    DWORD dwBytesToRead = 0;
    INTERNET_BUFFERS InternetBuffer;
    LPCSTR clpContent = TEXT("Content-Type: multipart/form-data; boundary=-------------------------24822581126073\r\n");
    TCHAR szContentLength[64];

    KG_DELETE_ARRAY(m_pResponseBuffer);

    m_hInternetSession = ::InternetOpen(
        cszAgent,
        INTERNET_OPEN_TYPE_PRECONFIG,
        NULL,
        NULL,
        0
    );
    KGLOG_PROCESS_ERROR(m_hInternetSession);

    m_hInternetConnection = ::InternetConnect(
        m_hInternetSession,
        cszAddress,
        (INTERNET_PORT)nPort,
        NULL,
        NULL,
        INTERNET_SERVICE_HTTP,
        INTERNET_FLAG_KEEP_CONNECTION | INTERNET_FLAG_NO_CACHE_WRITE,
        0
    );
    KGLOG_PROCESS_ERROR(m_hInternetConnection);

    m_hHttpRequest = ::HttpOpenRequest(
        m_hInternetConnection,
        "POST",
        cszVerb,
        HTTP_VERSION,
        NULL,
        clpAcceptTypeArray,
        INTERNET_FLAG_KEEP_CONNECTION | INTERNET_FLAG_NO_CACHE_WRITE | INTERNET_FLAG_FORMS_SUBMIT,
        0
    );
    KGLOG_PROCESS_ERROR(m_hHttpRequest);

    nRetCode = GetPostBuffer(&pbyBuffer, &dwBufferSize);
    KGLOG_PROCESS_ERROR(nRetCode);

    nRetCode = ::HttpAddRequestHeaders(m_hHttpRequest, "Accept: */*\r\n", (DWORD)_tcslen("Accept: */*\r\n"), HTTP_ADDREQ_FLAG_REPLACE);
    KGLOG_PROCESS_ERROR(nRetCode);

    nRetCode = ::HttpAddRequestHeaders(m_hHttpRequest, clpContent, (DWORD)_tcslen(clpContent), HTTP_ADDREQ_FLAG_ADD_IF_NEW);
    KGLOG_PROCESS_ERROR(nRetCode);

    _stprintf(szContentLength, "Content-Length: %d\r\n", dwBufferSize);

    nRetCode = ::HttpAddRequestHeaders(m_hHttpRequest, szContentLength,(DWORD) _tcslen(szContentLength), HTTP_ADDREQ_FLAG_ADD_IF_NEW);
    KGLOG_PROCESS_ERROR(nRetCode);

    memset(&InternetBuffer, 0, sizeof(InternetBuffer));

    InternetBuffer.dwStructSize = sizeof(InternetBuffer);

    nRetCode = ::HttpSendRequestEx(m_hHttpRequest, &InternetBuffer, NULL, HSR_INITIATE, 0);
    KGLOG_PROCESS_ERROR(nRetCode);

    nRetCode = ::InternetWriteFile(m_hHttpRequest, pbyBuffer, dwBufferSize, &dwBytesWrite);
    KGLOG_PROCESS_ERROR(nRetCode);

    nRetCode = ::HttpEndRequest(m_hHttpRequest, NULL, HSR_INITIATE, 0);
    KGLOG_PROCESS_ERROR(nRetCode);

    dwBufferSize = (DWORD)sizeof(szContentLength);

    nRetCode = ::HttpQueryInfo(m_hHttpRequest, HTTP_QUERY_CONTENT_LENGTH, szContentLength, &dwBufferSize, NULL);
    KGLOG_PROCESS_ERROR(nRetCode);

    dwBytesToRead = atoi(szContentLength);

    KG_DELETE_ARRAY(m_pResponseBuffer);

    m_pResponseBuffer = new BYTE[dwBytesToRead + 1];
    ZeroMemory(m_pResponseBuffer, dwBytesToRead + 1);

    nRetCode = ::InternetReadFile(m_hHttpRequest, m_pResponseBuffer, dwBytesToRead, &dwBytesRead);
    KGLOG_PROCESS_ERROR(nRetCode);

    m_pResponseBuffer[dwBytesToRead] = '\0';

    m_dwBufferSize = dwBytesRead;

    nResult = true;
Exit0:
    if (!nResult)
    {
        KG_DELETE_ARRAY(m_pResponseBuffer);
    }

    KG_DELETE_ARRAY(pbyBuffer);

    KG_INTERNET_HANDLE_CLOSE(m_hHttpRequest);
    KG_INTERNET_HANDLE_CLOSE(m_hInternetConnection);
    KG_INTERNET_HANDLE_CLOSE(m_hInternetSession);

    for (size_t i = 0; i < m_PostList.size(); ++i)
    {
        KG_DELETE_ARRAY(m_PostList[i].pParamName);
        KG_DELETE_ARRAY(m_PostList[i].pValue);
    }
    m_PostList.clear();

    return nResult;
}


int KG_HttpClient::AddPostParam(const TCHAR cszParamName[], const TCHAR cszValue[], KG_POST_PARAM_TYPE ePostParamType /* = eNormal */)
{
    int nResult = false;
    DWORD dwParamNameLen = 0;
    DWORD dwValueLen = 0;
    KG_POST_PARAM PostParam;

    memset(&PostParam, 0, sizeof(PostParam));

    KGLOG_PROCESS_ERROR(ePostParamType == eNormal || ePostParamType == eBinaryFile);
    
    dwParamNameLen = (DWORD)_tcslen(cszParamName);
    KGLOG_PROCESS_ERROR(dwParamNameLen > 0);

    dwValueLen = (DWORD)_tcslen(cszValue);
    KGLOG_PROCESS_ERROR(dwValueLen >= 0);
   
    PostParam.eType = ePostParamType;
  
    PostParam.pParamName = new TCHAR[dwParamNameLen + 1];
    strncpy(PostParam.pParamName, cszParamName, dwParamNameLen);
    PostParam.pParamName[dwParamNameLen] = '\0';
    
    PostParam.pValue = new TCHAR[dwValueLen + 1];
    strncpy(PostParam.pValue, cszValue, dwValueLen);
    PostParam.pValue[dwValueLen] = '\0';

    m_PostList.push_back(PostParam);

    nResult = true;
Exit0:
    if (!nResult)
    {
        KG_DELETE_ARRAY(PostParam.pParamName);
        KG_DELETE_ARRAY(PostParam.pValue);
    }

    return nResult;
}

BOOL KG_HttpClient::GetResponse(BYTE **ppbyBuffer, DWORD *pdwBufferSize)
{
    BOOL bResult = false;

    KGLOG_PROCESS_ERROR(ppbyBuffer);

    *ppbyBuffer = m_pResponseBuffer;
    *pdwBufferSize = m_dwBufferSize;

    m_pResponseBuffer = NULL;
    m_dwBufferSize = 0;

    bResult = true;
Exit0:
    return bResult;
}

BOOL KG_HttpClient::GetPostBuffer(BYTE **ppbyPostBuffer, DWORD *pdwPostBufferSize)
{
    int nResult = false;
    int  nRetCode = false;
    DWORD dwFileSize = 0;
    DWORD dwReadBytes = 0;
    BYTE *pbyBuffer = NULL;
    BYTE *pbyPostBuffer = NULL;
    DWORD dwPostSize = 0;
    TCHAR szBoundary[] = "-------------------------24822581126073";
    HANDLE hFile = INVALID_HANDLE_VALUE;
    struct POST_BLOCK
    {
        DWORD dwDataSize;
        BYTE *pbyData;
    } PostBlock;

    std::vector<POST_BLOCK> PostBlocksList;

    ZeroMemory(&PostBlock, sizeof(PostBlock));

    KGLOG_PROCESS_ERROR(ppbyPostBuffer);

    for (size_t i = 0; i < m_PostList.size(); ++i)
    {
        switch (m_PostList[i].eType)
        {
        case eNormal:
        {
            PostBlock.dwDataSize = 0;
            PostBlock.pbyData = new BYTE[2 * 1024];

            nRetCode = _stprintf(
                (TCHAR *)PostBlock.pbyData,
                "--%s\r\n"
                "Content-Disposition: form-data; name=\"%s\"\r\n"
                "\r\n%s\r\n",
                szBoundary, m_PostList[i].pParamName, m_PostList[i].pValue
            );
            KGLOG_PROCESS_ERROR(nRetCode > 0);
            
            PostBlock.dwDataSize = nRetCode;
            PostBlocksList.push_back(PostBlock);
            dwPostSize += nRetCode;

            PostBlock.dwDataSize = 0;
            PostBlock.pbyData = NULL;

            break;
        }
        case eBinaryFile:
        {
            hFile = ::CreateFile(
                m_PostList[i].pValue,
                GENERIC_READ,
                FILE_SHARE_READ,
                NULL,
                OPEN_EXISTING,
                FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN,
                NULL
            );
            KGLOG_PROCESS_ERROR(hFile != INVALID_HANDLE_VALUE);

            dwFileSize = GetFileSize(hFile, NULL);

            PostBlock.dwDataSize = 0;
            PostBlock.pbyData = new BYTE[2 * 1024 + dwFileSize];

            nRetCode = _stprintf(
                (TCHAR *)PostBlock.pbyData,
                "--%s\r\n"
                "Content-Disposition: form-data; name=\"%s\"; filename=\"%s\"\r\n"
                "Content-Type: application/octet-stream\r\n"
                "\r\n",
                szBoundary, m_PostList[i].pParamName, m_PostList[i].pValue
            );
            KGLOG_PROCESS_ERROR(nRetCode > 0);

            PostBlock.dwDataSize += nRetCode;
            dwPostSize += nRetCode;

            pbyBuffer = PostBlock.pbyData + nRetCode;

            nRetCode = ::ReadFile(hFile, pbyBuffer, dwFileSize, &dwReadBytes, NULL);
            KGLOG_PROCESS_ERROR(nRetCode);

            ::CloseHandle(hFile);
            hFile = INVALID_HANDLE_VALUE;

            PostBlock.dwDataSize += dwReadBytes;
            PostBlocksList.push_back(PostBlock);
            dwPostSize += dwReadBytes;

            PostBlock.dwDataSize = 0;
            PostBlock.pbyData = NULL;
            pbyBuffer = NULL;

            break;
        } // case eBinaryFile:
        default:
            KGLOG_PROCESS_ERROR(false);
        }
    }

    pbyPostBuffer = new BYTE[dwPostSize + 2 * 1024];
    pbyBuffer = pbyPostBuffer;
    for (unsigned i = 0; i < PostBlocksList.size(); ++i)
    {
        CopyMemory(pbyBuffer, PostBlocksList[i].pbyData, PostBlocksList[i].dwDataSize);
        pbyBuffer += PostBlocksList[i].dwDataSize;
    }

    CopyMemory(pbyBuffer, "\r\n--", _tcslen("\r\n--"));

    dwPostSize += (DWORD)_tcslen("\r\n--");
    pbyBuffer += (DWORD)_tcslen("\r\n--");

    CopyMemory(pbyBuffer, szBoundary, _tcslen(szBoundary));

    dwPostSize += (DWORD)_tcslen(szBoundary);
    pbyBuffer += (DWORD)_tcslen(szBoundary);

    CopyMemory(pbyBuffer, "--\r\n", _tcslen("--\r\n"));

    dwPostSize +=  (DWORD)_tcslen("--\r\n");
    pbyBuffer = NULL;

    *ppbyPostBuffer = pbyPostBuffer;
    *pdwPostBufferSize = dwPostSize;
    pbyPostBuffer = NULL;

    nResult = true;
Exit0:
    for (unsigned i = 0; i < PostBlocksList.size(); ++i)
    {
        KG_DELETE_ARRAY(PostBlocksList[i].pbyData);
    }

    if (hFile!= INVALID_HANDLE_VALUE)
    {
        ::CloseHandle(hFile);
        hFile = INVALID_HANDLE_VALUE;
    }

    if (!nResult)
    {
        KG_DELETE_ARRAY(pbyPostBuffer);
    }

    return nResult;
}
