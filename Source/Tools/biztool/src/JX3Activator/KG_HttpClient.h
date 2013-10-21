#ifndef _KG_HTTP_CLIENT_
#define _KG_HTTP_CLIENT_

#include <wininet.h>
#include <vector>

////////////////////////////////////////////////////////////////////////////////
int KGU_DownloadHttpFile(const char cszAddress[], const char cszObject[], const char cszFileName[]);


////////////////////////////////////////////////////////////////////////////////
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
    
    int AddPostParam(const char cszParamName[], const char cszValue[], KG_POST_PARAM_TYPE ePostParamType = eNormal);
	int ClearPostParam();
    int RequestPost(
        const char cszAgent[], 
        const char cszAddress[], 
        const char cszObjectName[],
        const char cszVerb[],
        int nPort
    );

    int GetResponse(BYTE **ppbyBuffer, DWORD *pdwBufferSize);

private:

    struct KG_POST_PARAM
    {
        KG_POST_PARAM_TYPE eType;
        char *pParamName;
        char *pValue;
    };

    int GetPostBuffer(BYTE **ppbyPostBuffer, DWORD *pdwPostBufferSize);

    HINTERNET m_hInternetSession;
    HINTERNET m_hInternetConnection;
    HINTERNET m_hHttpRequest;

    BYTE  m_byResponseBuffer[8192];
    DWORD m_dwBufferSize;
    
    std::vector<KG_POST_PARAM> m_PostList;
    
};

#endif // _KG_HTTP_CLIENT_