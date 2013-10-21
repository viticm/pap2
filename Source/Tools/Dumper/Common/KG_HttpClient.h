#ifndef _KG_HTTP_CLIENT_
#define _KG_HTTP_CLIENT_

#include <wininet.h>
#include <list>
#include <vector>

class KG_HttpClient
{
public:
    enum KG_POST_PARAM_TYPE
    {
        eNormal = 0,
        eBinaryFile,
    };

public:
    KG_HttpClient(void);
    ~KG_HttpClient(void);
    
    int AddPostParam(const TCHAR cszParamName[], const TCHAR cszValue[], KG_POST_PARAM_TYPE ePostParamType = eNormal);
    int RequestPost(
        const TCHAR cszAgent[], 
        const TCHAR cszAddress[], 
        const TCHAR cszVerb[],
        int nPort
    );

    int GetResponse(BYTE **ppbyBuffer, DWORD *pdwBufferSize);

private:

    struct KG_POST_PARAM
    {
        KG_POST_PARAM_TYPE eType;
        TCHAR *pParamName;
        TCHAR *pValue;
    };

    int GetPostBuffer(BYTE **ppbyPostBuffer, DWORD *pdwPostBufferSize);
	int GetPostDataSize(DWORD *pdwPostDataSize);
	int SendPostData();
	int SendData(BYTE *pData, DWORD dwSize);
	int FlushSendBuffer();

    HINTERNET m_hInternetSession;
    HINTERNET m_hInternetConnection;
    HINTERNET m_hHttpRequest;

	enum{MAX_SEND_BUFFER_SIZE = 1024 * 1024 * 5};
	BYTE *m_pSendBuffer;
	DWORD m_dwSendDataSize;

    BYTE *m_pResponseBuffer;
    DWORD m_dwBufferSize;
    
    std::vector<KG_POST_PARAM> m_PostList;
    
};

#endif // _KG_HTTP_CLIENT_