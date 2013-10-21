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

#define HTTP_BUFF_LENGTH 1024 * 8

KG_HttpClient::KG_HttpClient(void)
{
    m_hInternetSession = NULL;
    m_hInternetConnection = NULL;
    m_hHttpRequest = NULL;
    m_dwBufferSize = 0;
}

KG_HttpClient::~KG_HttpClient(void)
{
    m_ResponseBuffer.clear();

    ASSERT(m_PostList.empty());
    ASSERT(!m_hInternetSession);
    ASSERT(!m_hInternetConnection);
    ASSERT(!m_hHttpRequest);
}

int KG_HttpClient::RequestPost(
   const char cszAgent[], 
   const char cszAddress[], 
   const char cszObjectName[],
   int nPort
)
{
    int nResult = false;
    int nRetCode = false;
    LPCSTR clpAcceptTypeArray[] = {"*/*", NULL};
    BYTE *pbyBuffer = NULL;
    DWORD dwBufferSize = 0;
    DWORD dwBytesWrite = 0;
    DWORD dwStatusCode = 0;
    DWORD dwStatusCodeSize = 0;
    INTERNET_BUFFERS InternetBuffer;
    LPCSTR clpContent = "Content-Type: multipart/form-data; boundary=-------------------------24822581126073\r\n";
    char szContentLength[64];

    KGLOG_PROCESS_ERROR(cszAgent);
    KGLOG_PROCESS_ERROR(cszAgent[0]);
    KGLOG_PROCESS_ERROR(cszAddress);
    KGLOG_PROCESS_ERROR(cszAddress[0]);
    KGLOG_PROCESS_ERROR(cszObjectName);
    
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

    nRetCode = ::HttpAddRequestHeaders(m_hHttpRequest, szContentLength, (DWORD)strlen(szContentLength), HTTP_ADDREQ_FLAG_ADD_IF_NEW);
    KGLOG_PROCESS_ERROR(nRetCode);

    memset(&InternetBuffer, 0, sizeof(InternetBuffer));

    InternetBuffer.dwStructSize = sizeof(InternetBuffer);

    nRetCode = ::HttpSendRequestEx(m_hHttpRequest, &InternetBuffer, NULL, HSR_INITIATE, 0);
    KGLOG_PROCESS_ERROR(nRetCode);

    nRetCode = ::InternetWriteFile(m_hHttpRequest, pbyBuffer, dwBufferSize, &dwBytesWrite);
    KGLOG_PROCESS_ERROR(nRetCode);

    nRetCode = ::HttpEndRequest(m_hHttpRequest, NULL, HSR_INITIATE, 0);
    KGLOG_PROCESS_ERROR(nRetCode);

    // Check the response status code, which should be HTTP_STATUS_OK.
    dwStatusCodeSize = sizeof(dwStatusCode);
    nRetCode = HttpQueryInfo(
        m_hHttpRequest,
        HTTP_QUERY_STATUS_CODE | HTTP_QUERY_FLAG_NUMBER,
        &dwStatusCode,
        &dwStatusCodeSize,
        NULL
    );
    KGLOG_PROCESS_ERROR(nRetCode);
    KGLOG_PROCESS_ERROR(dwStatusCode == HTTP_STATUS_OK);

    nRetCode = ReadHttpBuffer();
    KGLOG_PROCESS_ERROR(nRetCode);

    nResult = true;
Exit0:
    if (!nResult)
    {
        if (dwStatusCode != HTTP_STATUS_OK)
        {
            KGLogPrintf(KGLOG_DEBUG, "Response status code : %d\n", dwStatusCode);
        }

        m_ResponseBuffer.clear();
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

int KG_HttpClient::RequestGet(
    const char cszAgent[], 
    const char cszAddress[], 
    const char cszObjectName[],
    int nPort
)
{
    int nResult = false;
    int nRetCode = false;
    LPCSTR clpAcceptTypeArray[] = {"*/*", NULL};
    DWORD dwStatusCode = 0;
    DWORD dwStatusCodeSize = 0;
    const unsigned RECEIVE_TIMEOUT = 10000;
    char szURL[1024];

    szURL[0] = '\0';

    KGLOG_PROCESS_ERROR(cszAgent);
    KGLOG_PROCESS_ERROR(cszAgent[0]);
    KGLOG_PROCESS_ERROR(cszAddress);
    KGLOG_PROCESS_ERROR(cszAddress[0]);
    KGLOG_PROCESS_ERROR(cszObjectName);

    snprintf(szURL, sizeof(szURL), "%s%s", cszAddress, cszObjectName);
    szURL[sizeof(szURL) - 1] = '\0';

    m_hInternetSession = ::InternetOpen(
        cszAgent,
        INTERNET_OPEN_TYPE_PRECONFIG,
        NULL,
        NULL,
        0
    );
    KGLOG_PROCESS_ERROR(m_hInternetSession);

    // Set receive timeout for response. 
    // warning: INTERNET_OPTION_CONNECT_TIMEOUT is 20s and can not be changed 
    // because of WinINet BUG.
    nRetCode = InternetSetOption(
        m_hInternetSession,
        INTERNET_OPTION_RECEIVE_TIMEOUT,
        (void *)&RECEIVE_TIMEOUT,
        sizeof(RECEIVE_TIMEOUT)
    );
    KGLOG_PROCESS_ERROR(nRetCode);

    m_hHttpRequest = InternetOpenUrl(
        m_hInternetSession,
        szURL,
        "Accept: */*\r\n\r\n",
        (DWORD)-1, 
        INTERNET_FLAG_NO_CACHE_WRITE | INTERNET_FLAG_PRAGMA_NOCACHE | INTERNET_FLAG_RELOAD,
        NULL
    );
    KGLOG_PROCESS_ERROR(m_hHttpRequest);

    // Check the response status code, which should be HTTP_STATUS_OK.
    dwStatusCodeSize = sizeof(dwStatusCode);
    nRetCode = HttpQueryInfo(
        m_hHttpRequest,
        HTTP_QUERY_STATUS_CODE | HTTP_QUERY_FLAG_NUMBER,
        &dwStatusCode,
        &dwStatusCodeSize,
        NULL
    );
    KGLOG_PROCESS_ERROR(nRetCode);
    KGLOG_PROCESS_ERROR(dwStatusCode == HTTP_STATUS_OK);

    nRetCode = ReadHttpBuffer();
    KGLOG_PROCESS_ERROR(nRetCode);

    nResult = true;
Exit0:
    if (!nResult && szURL)
    {
        KGLogPrintf(
            KGLOG_DEBUG, "Failed to download `%s`, Error Code %d.",
            szURL, GetLastError()
            );
    }
    if (!nResult)
    {
        if (dwStatusCode != HTTP_STATUS_OK)
        {
            KGLogPrintf(KGLOG_DEBUG, "Response status code : %d\n", dwStatusCode);
        }

        m_ResponseBuffer.clear();
    }

    KG_INTERNET_HANDLE_CLOSE(m_hHttpRequest);
    KG_INTERNET_HANDLE_CLOSE(m_hInternetConnection);
    KG_INTERNET_HANDLE_CLOSE(m_hInternetSession);

    return nResult;
}

int KG_HttpClient::ReadHttpBuffer()
{
    int nResult = false;
    int nRetCode = false;

    BYTE byBuffer[HTTP_BUFF_LENGTH];
    DWORD dwBytesRead = 0;

    KGLOG_PROCESS_ERROR(m_hHttpRequest);

    m_dwBufferSize = 0;

    m_ResponseBuffer.resize(HTTP_BUFF_LENGTH);

    do 
    {
        nRetCode = ::InternetReadFile(
            m_hHttpRequest,
            byBuffer,
            sizeof(byBuffer) - 1,
            &dwBytesRead
        );
        KGLOG_PROCESS_ERROR(nRetCode);

        if (m_dwBufferSize + dwBytesRead > m_ResponseBuffer.size() - 1)
        {
            m_ResponseBuffer.resize((m_dwBufferSize + dwBytesRead) * 2);
        }

        memcpy(&(m_ResponseBuffer[0]) + m_dwBufferSize, byBuffer, dwBytesRead);
        m_dwBufferSize += dwBytesRead;

    } while (dwBytesRead > 0);

    m_ResponseBuffer[m_dwBufferSize] = '\0';

    nResult = true;
Exit0:
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

int KG_HttpClient::GetResponse(BYTE **ppbyBuffer, DWORD *pdwBufferSize)
{
    int nResult = false;
    int nRetCode = false;

    KGLOG_PROCESS_ERROR(ppbyBuffer);
    KGLOG_PROCESS_ERROR(pdwBufferSize);

    nRetCode = !m_ResponseBuffer.empty();
    KGLOG_PROCESS_ERROR(nRetCode);

    *ppbyBuffer = &m_ResponseBuffer[0];
    *pdwBufferSize = m_dwBufferSize;

    nResult = true;
Exit0:
    return nResult;
}

int KG_HttpClient::GetPostBuffer(BYTE **ppbyPostBuffer, DWORD *pdwPostBufferSize)
{
    int nResult = false;
    int nRetCode = false;
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
