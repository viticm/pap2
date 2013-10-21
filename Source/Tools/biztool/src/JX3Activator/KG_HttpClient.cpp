#include "stdafx.h"
#include "KG_HttpClient.h"

#define KG_INTERNET_HANDLE_CLOSE(hHandle) \
    do  \
    {   \
        if ((hHandle))   \
        {   \
            ::InternetCloseHandle((hHandle));   \
            (hHandle) = NULL;   \
        }   \
    } while (false)

////////////////////////////////////////////////////////////////////////////////
int KGU_DownloadHttpFile(const char cszAddress[], const char cszObject[], const char cszFileName[])
{
    int nResult  = false;
    int nRetCode = false;
    HINTERNET hSession = NULL;
    HINTERNET hConnection  = NULL;
    HINTERNET hHttpRequest = NULL;
    DWORD dwStatusCode = 0;
    DWORD dwBufferSize = 0;
    DWORD dwReadSize   = 0;
    DWORD dwTotalReadSize = 0;
    DWORD dwTotalFileSize = 0;
    unsigned char byBuffer[1024 * 64];
    FILE *fpLocalFile = NULL;
    const unsigned RECEIVE_TIMEOUT = 5000;
    LPCSTR clpAcceptTypeArray[] = {"*/*", NULL};

    KGLOG_PROCESS_ERROR(cszAddress);
    KGLOG_PROCESS_ERROR(cszAddress[0]);
    KGLOG_PROCESS_ERROR(cszObject);
    KGLOG_PROCESS_ERROR(cszObject[0]);
    KGLOG_PROCESS_ERROR(cszFileName);
    KGLOG_PROCESS_ERROR(cszFileName[0]);

    // Initialize an application's use of the WinINet functions.
    hSession = InternetOpen(
        "Mozilla/4.0 (compatible; MSIE 6.0; Windows NT 5.1)",
        INTERNET_OPEN_TYPE_PRECONFIG,
        NULL, 
        NULL, 
        0
    );
    KGLOG_PROCESS_ERROR(hSession);

    // Set receive timeout for response. 
    // warning: INTERNET_OPTION_CONNECT_TIMEOUT is 20s and can not be changed 
    // because of WinINet BUG.
    nRetCode = InternetSetOption(
        hSession,
        INTERNET_OPTION_RECEIVE_TIMEOUT,
        (void *)&RECEIVE_TIMEOUT,
        sizeof(RECEIVE_TIMEOUT)
    );
    KGLOG_PROCESS_ERROR(nRetCode && "InternetSetOption()");

    hConnection = InternetConnect(
        hSession,
        cszAddress,
        INTERNET_DEFAULT_HTTP_PORT,
        NULL,
        NULL,
        INTERNET_SERVICE_HTTP,
        INTERNET_FLAG_KEEP_CONNECTION | INTERNET_FLAG_NO_CACHE_WRITE,
        0
    );
    KGLOG_PROCESS_ERROR(hConnection);

    // If cszVerb is NULL, the function uses GET as the HTTP verb
    hHttpRequest = HttpOpenRequest(
        hConnection,
        "GET",    
        cszObject,
        HTTP_VERSION,
        NULL,
        clpAcceptTypeArray,
        INTERNET_FLAG_KEEP_CONNECTION | INTERNET_FLAG_NO_CACHE_WRITE | INTERNET_FLAG_FORMS_SUBMIT,
        0
    );
    KGLOG_PROCESS_ERROR(hHttpRequest);

    nRetCode = HttpSendRequest(hHttpRequest, _T("Accept: */*\r\n\r\n"), -1, NULL, 0);
    KGLOG_PROCESS_ERROR(nRetCode);

    // Check the response status code, which should be HTTP_STATUS_OK.
    dwBufferSize = sizeof(dwStatusCode);
    nRetCode = HttpQueryInfo(
        hHttpRequest,
        HTTP_QUERY_STATUS_CODE | HTTP_QUERY_FLAG_NUMBER,
        &dwStatusCode,
        &dwBufferSize,
        NULL
    );
    KGLOG_PROCESS_ERROR(nRetCode && "HttpQueryInfo()");
    if (dwStatusCode != HTTP_STATUS_OK)
    {
        KGLogPrintf(KGLOG_DEBUG, "Response status code : %d\n", dwStatusCode);
        KG_PROCESS_ERROR(false);
    }

    fpLocalFile = fopen(cszFileName, "wb");
    KGLOG_PROCESS_ERROR(fpLocalFile);

    while (true)
    {
        nRetCode = InternetReadFile(
            hHttpRequest, byBuffer, sizeof(byBuffer), &dwReadSize
        );
        KGLOG_PROCESS_ERROR(nRetCode && "InternetReadFile()");

        if (dwReadSize == 0)    // No more data.
            break;

        nRetCode = (int)fwrite(byBuffer, 1, dwReadSize, fpLocalFile);
        KGLOG_PROCESS_ERROR(nRetCode == (int)dwReadSize);

        dwTotalReadSize += dwReadSize;
    }

    // If HTTP server stop when reading data, the file is incomplete and no error
    // occur, so check the file size equal to the value of the Content-Length,
    // if which is included in response header.
    // (If respond is chunked encoded, there is no Content-Length in the header).
    dwBufferSize = sizeof(dwTotalFileSize);
    nRetCode = HttpQueryInfo(
        hHttpRequest,
        HTTP_QUERY_CONTENT_LENGTH | HTTP_QUERY_FLAG_NUMBER,
        &dwTotalFileSize,
        &dwBufferSize,
        NULL
    );
    if (nRetCode)
    {
        KGLOG_PROCESS_ERROR(
            (dwTotalFileSize == dwTotalReadSize) && "Incomplete file."
        );
    }    
    
    nResult = true;
Exit0:
    if (fpLocalFile)
    {
        fclose(fpLocalFile);
        fpLocalFile = NULL;
    }
    if (hHttpRequest)
    {
        InternetCloseHandle(hHttpRequest);
        hHttpRequest = NULL;
    }
    if (hConnection)
    {
        InternetCloseHandle(hConnection);
        hConnection = NULL;
    }
    if (hSession)
    {
        InternetCloseHandle(hSession);
        hSession = NULL;
    }
    if (!nResult)
    {
        DWORD dwLastError = GetLastError();
        KGLogPrintf(KGLOG_DEBUG, "Last error code : %d\n", dwLastError);

        // Remove incomplete file.
        _tremove(cszFileName);
    }
    return nResult;
}

////////////////////////////////////////////////////////////////////////////////
KG_HttpClient::KG_HttpClient(void)
{
    m_hInternetSession = NULL;
    m_hInternetConnection = NULL;
    m_hHttpRequest = NULL;
    m_dwBufferSize = 0;
}

KG_HttpClient::~KG_HttpClient(void)
{
    ASSERT(m_PostList.empty());
    ASSERT(!m_hInternetSession);
    ASSERT(!m_hInternetConnection);
    ASSERT(!m_hHttpRequest);
}

int KG_HttpClient::RequestPost(
   const char cszAgent[], 
   const char cszAddress[], 
   const char cszObjectName[],
   const char cszVerb[],
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
    LPCSTR clpContent = "Content-Type: multipart/form-data; boundary=-------------------------24822581126073\r\n";
    char szContentLength[64];

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

    if (cszVerb[0] == '\0')
        cszVerb = NULL;

    // If cszVerb is NULL, the function uses GET as the HTTP verb
    m_hHttpRequest = ::HttpOpenRequest(
        m_hInternetConnection,
        cszVerb,    
        cszObjectName,
        HTTP_VERSION,
        NULL,
        clpAcceptTypeArray,
        INTERNET_FLAG_KEEP_CONNECTION | INTERNET_FLAG_NO_CACHE_WRITE | INTERNET_FLAG_FORMS_SUBMIT,
        0
    );
    KGLOG_PROCESS_ERROR(m_hHttpRequest);

    nRetCode = GetPostBuffer(&pbyBuffer, &dwBufferSize);
    KGLOG_PROCESS_ERROR(nRetCode);

    nRetCode = ::HttpAddRequestHeaders(m_hHttpRequest, "Accept: */*\r\n", (DWORD)strlen("Accept: */*\r\n"), HTTP_ADDREQ_FLAG_REPLACE);
    KGLOG_PROCESS_ERROR(nRetCode);

    nRetCode = ::HttpAddRequestHeaders(m_hHttpRequest, clpContent, (DWORD)strlen(clpContent), HTTP_ADDREQ_FLAG_ADD_IF_NEW);
    KGLOG_PROCESS_ERROR(nRetCode);

    sprintf(szContentLength, "Content-Length: %d\r\n", dwBufferSize);

    nRetCode = ::HttpAddRequestHeaders(m_hHttpRequest, szContentLength,(DWORD) strlen(szContentLength), HTTP_ADDREQ_FLAG_ADD_IF_NEW);
    KGLOG_PROCESS_ERROR(nRetCode);

    memset(&InternetBuffer, 0, sizeof(InternetBuffer));

    InternetBuffer.dwStructSize = sizeof(InternetBuffer);

    nRetCode = ::HttpSendRequestEx(m_hHttpRequest, &InternetBuffer, NULL, HSR_INITIATE, 0);
    KGLOG_PROCESS_ERROR(nRetCode);

    nRetCode = ::InternetWriteFile(m_hHttpRequest, pbyBuffer, dwBufferSize, &dwBytesWrite);
    KGLOG_PROCESS_ERROR(nRetCode);

    nRetCode = ::HttpEndRequest(m_hHttpRequest, NULL, HSR_INITIATE, 0);
    KGLOG_PROCESS_ERROR(nRetCode);

    dwBytesToRead = sizeof(m_byResponseBuffer);
    nRetCode = ::InternetReadFile(m_hHttpRequest, m_byResponseBuffer, dwBytesToRead, &dwBytesRead);
    KGLOG_PROCESS_ERROR(nRetCode);
    KGLOG_PROCESS_ERROR(dwBytesRead < sizeof(m_byResponseBuffer));

    m_byResponseBuffer[dwBytesRead] = '\0';

    m_dwBufferSize = dwBytesRead;

    nResult = true;
Exit0:
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


int KG_HttpClient::AddPostParam(const char cszParamName[], const char cszValue[], KG_POST_PARAM_TYPE ePostParamType /* = eNormal */)
{
    int nResult = false;
    DWORD dwParamNameLen = 0;
    DWORD dwValueLen = 0;
    KG_POST_PARAM PostParam;

    memset(&PostParam, 0, sizeof(PostParam));

    KGLOG_PROCESS_ERROR(ePostParamType == eNormal || ePostParamType == eBinaryFile);
    
    dwParamNameLen = (DWORD)strlen(cszParamName);
    KGLOG_PROCESS_ERROR(dwParamNameLen > 0);

    dwValueLen = (DWORD)strlen(cszValue);
    KGLOG_PROCESS_ERROR(dwValueLen >= 0);
   
    PostParam.eType = ePostParamType;
  
    PostParam.pParamName = new char[dwParamNameLen + 1];
    strncpy(PostParam.pParamName, cszParamName, dwParamNameLen);
    PostParam.pParamName[dwParamNameLen] = '\0';
    
    PostParam.pValue = new char[dwValueLen + 1];
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

int KG_HttpClient::ClearPostParam()
{
	for (size_t i = 0; i < m_PostList.size(); ++i)
	{
		KG_DELETE_ARRAY(m_PostList[i].pParamName);
		KG_DELETE_ARRAY(m_PostList[i].pValue);
	}
	m_PostList.clear();
	return true;
}

BOOL KG_HttpClient::GetResponse(BYTE **ppbyBuffer, DWORD *pdwBufferSize)
{
    BOOL bResult = false;

    KGLOG_PROCESS_ERROR(ppbyBuffer);

    *ppbyBuffer = m_byResponseBuffer;
    *pdwBufferSize = m_dwBufferSize;

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
    char szBoundary[] = "-------------------------24822581126073";
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

            nRetCode = sprintf(
                (char *)PostBlock.pbyData,
                "--%s\r\n"
                "Content-Disposition: form-data; name=\"%s\"\r\n"
                "\r\n%s\r\n",
                szBoundary, m_PostList[i].pParamName, m_PostList[i].pValue
            );

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
			KGLOG_CHECK_ERROR(hFile != INVALID_HANDLE_VALUE);

			dwFileSize = 0;
			if (hFile != INVALID_HANDLE_VALUE)
			{
				dwFileSize = ::GetFileSize(hFile, NULL);
				if (dwFileSize == INVALID_FILE_SIZE)
					dwFileSize = 0;
			}

            PostBlock.dwDataSize = 0;
            PostBlock.pbyData = new BYTE[2 * 1024 + dwFileSize];

            nRetCode = sprintf(
                (char *)PostBlock.pbyData,
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

			dwReadBytes = 0;
			if (hFile != INVALID_HANDLE_VALUE && dwFileSize)
			{
				nRetCode = ::ReadFile(hFile, pbyBuffer, dwFileSize, &dwReadBytes, NULL);
				KGLOG_CHECK_ERROR(nRetCode);
			}

			if (hFile != INVALID_HANDLE_VALUE)
			{
				::CloseHandle(hFile);
				hFile = INVALID_HANDLE_VALUE;
			}

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

    CopyMemory(pbyBuffer, "\r\n--", strlen("\r\n--"));

    dwPostSize += (DWORD)strlen("\r\n--");
    pbyBuffer += (DWORD)strlen("\r\n--");

    CopyMemory(pbyBuffer, szBoundary, strlen(szBoundary));

    dwPostSize += (DWORD)strlen(szBoundary);
    pbyBuffer += (DWORD)strlen(szBoundary);

    CopyMemory(pbyBuffer, "--\r\n", strlen("--\r\n"));

    dwPostSize +=  (DWORD)strlen("--\r\n");
    pbyBuffer = NULL;

    *ppbyPostBuffer = pbyPostBuffer;
    *pdwPostBufferSize = dwPostSize;
    pbyPostBuffer = NULL;

    nResult = true;
Exit0:
	KG_DELETE_ARRAY(PostBlock.pbyData);
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
