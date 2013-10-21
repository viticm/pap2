////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : SO3Interaction.h
//  Version     : 1.0
//  Creator     : Hu Changyin
//  Create Date : 2008-09-26 14:40:49
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _INCLUDE_SO3INTERACTION_H_
#define _INCLUDE_SO3INTERACTION_H_

////////////////////////////////////////////////////////////////////////////////

class KG_HttpClient;
class KThread;
class KSO3Interaction : public ISO3Interaction
{
public:
	virtual int Init();
	virtual void Exit();
	virtual void Active();

	virtual void Clear();
	virtual int AddParam(const char * pcszParamName, const char * pcszValue, BOOL bFile);
	virtual int Send(
        const char * pcszName,
        const char * pcszAddress,
        const char * pcszObjectName,
        const char * pcszVerb,
        int nPort,
        fnSendResult fnResult
    );
	virtual int Request(
        const char * pcszName,
        const char * pcszAddress,
        const char * pcszObjectName,
        const char * pcszVerb,
        int nPort,
        fnRequestResult fnResult
    );

	void AddRef();
public:
	KSO3Interaction();
	virtual ~KSO3Interaction();
private:
	static	void ThreadFunction(void* lpParam);

private:
	struct KTreadHttpClient
	{
		char szName[256];
		char szAddress[256];
        char szObjectName[256];
		char szVerb[256];
		int nPort;
		KThread Thread;
		KG_HttpClient HttpClient;
		BOOL bRequest;
		BOOL bSucess;
		BOOL bFinished;
		union
		{
			fnSendResult SendResultFunc;
			fnRequestResult RequestResultFunc;
		};
		BYTE *pbyBuffer;
		DWORD dwBufferSize;
		KTreadHttpClient()
		{
			szName[0] = '\0';
			szAddress[0] = '\0';
            szObjectName[0] = '\0';
			szVerb[0] = '\0';
			nPort = 0;
			bRequest = FALSE;
			bSucess = FALSE;
			bFinished = FALSE;
			SendResultFunc = NULL;
			pbyBuffer = NULL;
			dwBufferSize = 0;
		}
		virtual ~KTreadHttpClient()
		{
			Thread.Destroy();
		}
	};
	KTreadHttpClient * m_pTreadHttpClient;
	typedef std::vector<KTreadHttpClient *> KTreadHttpClientArray;
	KTreadHttpClientArray m_KTreadHttpClientArray;
	int m_nRef;
};


#endif //_INCLUDE_SO3INTERACTION_H_
