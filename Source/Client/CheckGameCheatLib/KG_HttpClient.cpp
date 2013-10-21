#include "stdafx.h"
#include "KG_HttpClient.h"
#include <tchar.h>

#pragma comment(lib,"Wininet.lib")

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
	m_pSendBuffer = new BYTE[MAX_SEND_BUFFER_SIZE];
	m_dwSendDataSize = 0;
}

KG_HttpClient::~KG_HttpClient(void)
{
	KG_DELETE_ARRAY(m_pSendBuffer);
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
    LPCTSTR clpAcceptTypeArray[] = {TEXT("*/*"), NULL};
    BYTE *pbyBuffer = NULL;
    DWORD dwBufferSize = 0;
    DWORD dwBytesWrite = 0;
    DWORD dwBytesRead = 0;
    DWORD dwBytesToRead = 0;
    INTERNET_BUFFERS InternetBuffer;
    LPCTSTR clpContent = TEXT("Content-Type: multipart/form-data; boundary=-------------------------24822581126073\r\n");
    TCHAR szContentLength[64];

    KG_DELETE_ARRAY(m_pResponseBuffer);

    m_hInternetSession = ::InternetOpen(
        cszAgent,
        INTERNET_OPEN_TYPE_PRECONFIG,
        NULL,
        NULL,
        0
    );
    KG_PROCESS_ERROR(m_hInternetSession);

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
    KG_PROCESS_ERROR(m_hInternetConnection);

    m_hHttpRequest = ::HttpOpenRequest(
        m_hInternetConnection,
        TEXT("POST"),
        cszVerb,
        HTTP_VERSION,
        NULL,
        clpAcceptTypeArray,
        INTERNET_FLAG_KEEP_CONNECTION | INTERNET_FLAG_NO_CACHE_WRITE | INTERNET_FLAG_FORMS_SUBMIT,
        0
    );
    KG_PROCESS_ERROR(m_hHttpRequest);

    //nRetCode = GetPostBuffer(&pbyBuffer, &dwBufferSize);	
	nRetCode = GetPostDataSize(&dwBufferSize);	
    KG_PROCESS_ERROR(nRetCode);	

    nRetCode = ::HttpAddRequestHeaders(m_hHttpRequest, TEXT("Accept: */*\r\n"), (DWORD)_tcslen(TEXT("Accept: */*\r\n")), HTTP_ADDREQ_FLAG_REPLACE);
    KG_PROCESS_ERROR(nRetCode);

    nRetCode = ::HttpAddRequestHeaders(m_hHttpRequest, clpContent, (DWORD)_tcslen(clpContent), HTTP_ADDREQ_FLAG_ADD_IF_NEW);
    KG_PROCESS_ERROR(nRetCode);

    _stprintf(szContentLength, TEXT("Content-Length: %d\r\n"), dwBufferSize);

    nRetCode = ::HttpAddRequestHeaders(m_hHttpRequest, szContentLength,(DWORD) _tcslen(szContentLength), HTTP_ADDREQ_FLAG_ADD_IF_NEW);
    KG_PROCESS_ERROR(nRetCode);

    memset(&InternetBuffer, 0, sizeof(InternetBuffer));

    InternetBuffer.dwStructSize = sizeof(InternetBuffer);

    nRetCode = ::HttpSendRequestEx(m_hHttpRequest, &InternetBuffer, NULL, HSR_INITIATE, 0);
    KG_PROCESS_ERROR(nRetCode);

    //nRetCode = ::InternetWriteFile(m_hHttpRequest, pbyBuffer, dwBufferSize, &dwBytesWrite);
	nRetCode = SendPostData();
    KG_PROCESS_ERROR(nRetCode);

    nRetCode = ::HttpEndRequest(m_hHttpRequest, NULL, HSR_INITIATE, 0);
    KG_PROCESS_ERROR(nRetCode);

    dwBufferSize = (DWORD)sizeof(szContentLength);

    nRetCode = ::HttpQueryInfo(m_hHttpRequest, HTTP_QUERY_CONTENT_LENGTH, szContentLength, &dwBufferSize, NULL);
    KG_PROCESS_ERROR(nRetCode);

    dwBytesToRead = _ttoi(szContentLength);

    KG_DELETE_ARRAY(m_pResponseBuffer);

    m_pResponseBuffer = new BYTE[dwBytesToRead + 1];
    ZeroMemory(m_pResponseBuffer, dwBytesToRead + 1);

    nRetCode = ::InternetReadFile(m_hHttpRequest, m_pResponseBuffer, dwBytesToRead, &dwBytesRead);
    KG_PROCESS_ERROR(nRetCode);

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


int KG_HttpClient::AddPostParam(const CHAR cszParamName[], const CHAR cszValue[], KG_POST_PARAM_TYPE ePostParamType /* = eNormal */)
{
    int nResult = false;
    DWORD dwParamNameLen = 0;
    DWORD dwValueLen = 0;
    KG_POST_PARAM PostParam;

    memset(&PostParam, 0, sizeof(PostParam));

    KG_PROCESS_ERROR(ePostParamType == eNormal || ePostParamType == eBinaryFile);
    
    dwParamNameLen = (DWORD)strlen(cszParamName);
    KG_PROCESS_ERROR(dwParamNameLen > 0);

    dwValueLen = (DWORD)strlen(cszValue);
    KG_PROCESS_ERROR(dwValueLen >= 0);
   
    PostParam.eType = ePostParamType;
  
    PostParam.pParamName = new CHAR[dwParamNameLen + 1];
    strncpy(PostParam.pParamName, cszParamName, dwParamNameLen);
    PostParam.pParamName[dwParamNameLen] = '\0';
    
    PostParam.pValue = new CHAR[dwValueLen + 1];
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

    KG_PROCESS_ERROR(ppbyBuffer);

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
    TCHAR szBoundary[] = TEXT("-------------------------24822581126073");
    HANDLE hFile = INVALID_HANDLE_VALUE;
    struct POST_BLOCK
    {
        DWORD dwDataSize;
        BYTE *pbyData;
    } PostBlock;

    std::vector<POST_BLOCK> PostBlocksList;

    ZeroMemory(&PostBlock, sizeof(PostBlock));

    KG_PROCESS_ERROR(ppbyPostBuffer);

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
                TEXT("--%s\r\n")
                TEXT("Content-Disposition: form-data; name=\"%s\"\r\n")
                TEXT("\r\n%s\r\n"),
                szBoundary, m_PostList[i].pParamName, m_PostList[i].pValue
            );
            KG_PROCESS_ERROR(nRetCode > 0);
            
            PostBlock.dwDataSize = nRetCode;
            PostBlocksList.push_back(PostBlock);
            dwPostSize += nRetCode;

            PostBlock.dwDataSize = 0;
            PostBlock.pbyData = NULL;

            break;
        }
        case eBinaryFile:
        {
            hFile = ::CreateFileA(
                m_PostList[i].pValue,
                GENERIC_READ,
                FILE_SHARE_READ,
                NULL,
                OPEN_EXISTING,
                FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN,
                NULL
            );
            KG_PROCESS_ERROR(hFile != INVALID_HANDLE_VALUE);

            dwFileSize = GetFileSize(hFile, NULL);

            PostBlock.dwDataSize = 0;
            PostBlock.pbyData = new BYTE[2 * 1024 + dwFileSize];

            nRetCode = _stprintf(
                (TCHAR *)PostBlock.pbyData,
                TEXT("--%s\r\n")
                TEXT("Content-Disposition: form-data; name=\"%s\"; filename=\"%s\"\r\n")
                TEXT("Content-Type: application/octet-stream\r\n")
                TEXT("\r\n"),
                szBoundary, m_PostList[i].pParamName, m_PostList[i].pValue
            );
            KG_PROCESS_ERROR(nRetCode > 0);

            PostBlock.dwDataSize += nRetCode;
            dwPostSize += nRetCode;

            pbyBuffer = PostBlock.pbyData + nRetCode;

            nRetCode = ::ReadFile(hFile, pbyBuffer, dwFileSize, &dwReadBytes, NULL);
            KG_PROCESS_ERROR(nRetCode);

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
            KG_PROCESS_ERROR(false);
        }
    }

    pbyPostBuffer = new BYTE[dwPostSize + 2 * 1024];
    pbyBuffer = pbyPostBuffer;
    for (unsigned i = 0; i < PostBlocksList.size(); ++i)
    {
        CopyMemory(pbyBuffer, PostBlocksList[i].pbyData, PostBlocksList[i].dwDataSize);
        pbyBuffer += PostBlocksList[i].dwDataSize;
    }

    CopyMemory(pbyBuffer, TEXT("\r\n--"), _tcslen(TEXT("\r\n--")));

    dwPostSize += (DWORD)_tcslen(TEXT("\r\n--"));
    pbyBuffer += (DWORD)_tcslen(TEXT("\r\n--"));

    CopyMemory(pbyBuffer, szBoundary, _tcslen(szBoundary));

    dwPostSize += (DWORD)_tcslen(szBoundary);
    pbyBuffer += (DWORD)_tcslen(szBoundary);

    CopyMemory(pbyBuffer, TEXT("--\r\n"), _tcslen(TEXT("--\r\n")));

    dwPostSize +=  (DWORD)_tcslen(TEXT("--\r\n"));
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

int KG_HttpClient::GetPostDataSize(DWORD *pdwPostDataSize)
{
	int nResult = false;
	int  nRetCode = false;
	CHAR szBoundary[] = "-------------------------24822581126073";
	BYTE pbyTempBuffer[2048];
	HANDLE hFile = INVALID_HANDLE_VALUE;

	*pdwPostDataSize = 0;

	for (size_t i = 0; i < m_PostList.size(); ++i)
	{
		switch (m_PostList[i].eType)
		{
		case eNormal:
			{
				nRetCode = sprintf(
					(CHAR*)pbyTempBuffer,
					"--%s\r\n"
					"Content-Disposition: form-data; name=\"%s\"\r\n"
					"\r\n%s\r\n",
					szBoundary, m_PostList[i].pParamName, m_PostList[i].pValue
					);
				KG_PROCESS_ERROR(nRetCode > 0);

				*pdwPostDataSize += nRetCode;

				break;
			}
		case eBinaryFile:
			{			
				DWORD dwFileSize = 0;

				nRetCode = sprintf(
					(CHAR*)pbyTempBuffer,
					"--%s\r\n"
					"Content-Disposition: form-data; name=\"%s\"; filename=\"%s\"\r\n"
					"Content-Type: application/octet-stream\r\n"
					"\r\n",
					szBoundary, m_PostList[i].pParamName, m_PostList[i].pValue
				);
				KG_PROCESS_ERROR(nRetCode);

				*pdwPostDataSize += nRetCode;

				hFile = ::CreateFileA(
					m_PostList[i].pValue,
					GENERIC_READ,
					FILE_SHARE_READ,
					NULL,
					OPEN_EXISTING,
					FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN,
					NULL
					);
				KG_PROCESS_ERROR(hFile != INVALID_HANDLE_VALUE);

				*pdwPostDataSize += GetFileSize(hFile, NULL);

				break;
			} // case eBinaryFile:
		default:
			KG_PROCESS_ERROR(false);
		}
	}

	*pdwPostDataSize += (DWORD)strlen("\r\n--");
	*pdwPostDataSize += (DWORD)strlen(szBoundary);
	*pdwPostDataSize += (DWORD)strlen("--\r\n");

	nResult = true;
Exit0:
	if (hFile != INVALID_HANDLE_VALUE)
	{
		::CloseHandle(hFile);
		hFile = INVALID_HANDLE_VALUE;
	}
	return nResult;
}

int KG_HttpClient::SendPostData()
{
	int nResult = false;
    int  nRetCode = false;
    CHAR szBoundary[] = "-------------------------24822581126073";    
	BYTE pbyTempBuffer[2048];
	HANDLE hFile = INVALID_HANDLE_VALUE;

    for (size_t i = 0; i < m_PostList.size(); ++i)
    {
        switch (m_PostList[i].eType)
        {
        case eNormal:
        {
            nRetCode = sprintf(
                (CHAR*)pbyTempBuffer,
                "--%s\r\n"
                "Content-Disposition: form-data; name=\"%s\"\r\n"
                "\r\n%s\r\n",
                szBoundary, m_PostList[i].pParamName, m_PostList[i].pValue
            );
            KG_PROCESS_ERROR(nRetCode > 0);

			nRetCode = SendData(pbyTempBuffer, nRetCode);
			KG_PROCESS_ERROR(nRetCode);

            break;
        }
        case eBinaryFile:
        {			
			DWORD dwFileSize = 0;

			nRetCode = sprintf(
				(CHAR*)pbyTempBuffer,
				"--%s\r\n"
				"Content-Disposition: form-data; name=\"%s\"; filename=\"%s\"\r\n"
				"Content-Type: application/octet-stream\r\n"
				"\r\n",
				szBoundary, m_PostList[i].pParamName, m_PostList[i].pValue
				);
			KG_PROCESS_ERROR(nRetCode > 0);

			nRetCode = SendData(pbyTempBuffer, nRetCode);
			KG_PROCESS_ERROR(nRetCode);

            hFile = ::CreateFileA(
                m_PostList[i].pValue,
                GENERIC_READ,
                FILE_SHARE_READ,
                NULL,
                OPEN_EXISTING,
                FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN,
                NULL
            );
            KG_PROCESS_ERROR(hFile != INVALID_HANDLE_VALUE);

            dwFileSize = GetFileSize(hFile, NULL);
			KG_PROCESS_ERROR(dwFileSize > 0);

			while (dwFileSize > 0)
			{
				DWORD dwReadBytes = 0;			
				nRetCode = ::ReadFile(hFile, pbyTempBuffer, sizeof(pbyTempBuffer), &dwReadBytes, NULL);
				KG_PROCESS_ERROR(nRetCode);

				nRetCode = SendData(pbyTempBuffer, dwReadBytes);
				KG_PROCESS_ERROR(nRetCode);
				
				dwFileSize -= dwReadBytes;
			}
            break;
        } // case eBinaryFile:
        default:
            KG_PROCESS_ERROR(false);
        }
    }

	SendData((BYTE*)"\r\n--", (DWORD)strlen("\r\n--"));
	SendData((BYTE*)szBoundary, (DWORD)strlen(szBoundary));
	SendData((BYTE*)"--\r\n", (DWORD)strlen("--\r\n"));

	FlushSendBuffer();

    nResult = true;
Exit0:
    if (hFile != INVALID_HANDLE_VALUE)
    {
        ::CloseHandle(hFile);
        hFile = INVALID_HANDLE_VALUE;
    }

    return nResult;
}

int KG_HttpClient::SendData(BYTE *pData, DWORD dwSize)
{
	int nRetCode = false;
	int nResult = false;

	if (dwSize > MAX_SEND_BUFFER_SIZE - m_dwSendDataSize)
	{
		nRetCode = FlushSendBuffer();
		KG_PROCESS_ERROR(nRetCode);
		m_dwSendDataSize = 0;
	}

	CopyMemory(m_pSendBuffer + m_dwSendDataSize, pData, dwSize);
	m_dwSendDataSize += dwSize;

	nResult = true;
Exit0:
	return nResult;
}

int KG_HttpClient::FlushSendBuffer()
{
	int nRetCode = false;
	int nResult = false;
	DWORD dwBytesWrite = 0;
	while (dwBytesWrite < m_dwSendDataSize)
	{
		DWORD dwRealSend = 0;
		nRetCode = ::InternetWriteFile(m_hHttpRequest, m_pSendBuffer + dwBytesWrite, m_dwSendDataSize - dwBytesWrite, &dwRealSend);
		KG_PROCESS_ERROR(nRetCode);
		dwBytesWrite += dwRealSend;
	}
	m_dwSendDataSize = 0;

	nResult = true;
Exit0:
	return nResult;
}