// SO3Interaction.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"
#include "ISO3Interaction.h"
#include "KG_HttpClient.h"
#include "SO3Interaction.h"

#ifdef _MANAGED
#pragma managed(push, off)
#endif

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{
    return TRUE;
}

#ifdef _MANAGED
#pragma managed(pop)
#endif

KSO3Interaction * g_pKSO3Interaction = NULL;

KSO3Interaction::KSO3Interaction()
{
	m_pTreadHttpClient = NULL;;
	m_nRef = 0;
};

KSO3Interaction::~KSO3Interaction()
{
	SAFE_DELETE(m_pTreadHttpClient);
	KTreadHttpClientArray::iterator it = m_KTreadHttpClientArray.begin();
	while (it != m_KTreadHttpClientArray.end())
	{
		SAFE_DELETE(*it);
		++it;
	}
	m_KTreadHttpClientArray.clear();
};


int KSO3Interaction::Init()
{
	Clear();
	return TRUE;
}

void KSO3Interaction::Exit()
{
	m_nRef--;
	if (m_nRef <= 0)
	{
		g_pKSO3Interaction = NULL;
		delete this;
	}
}

void KSO3Interaction::Clear()
{
	if (m_pTreadHttpClient)
		m_pTreadHttpClient->HttpClient.ClearPostParam();
}

void KSO3Interaction::Active()
{
	KTreadHttpClientArray::iterator it = m_KTreadHttpClientArray.begin();
	while (it != m_KTreadHttpClientArray.end())
	{
		KTreadHttpClient * pClient = (*it);
		if (pClient->bFinished)
		{
            it = m_KTreadHttpClientArray.erase(it);

            if (pClient->bRequest)
			{
				if (pClient->RequestResultFunc)
					pClient->RequestResultFunc(pClient->szName, pClient->bSucess, pClient->pbyBuffer, pClient->dwBufferSize);
			}
			else
			{
				if (pClient->SendResultFunc)
					pClient->SendResultFunc(pClient->szName, pClient->bSucess);
			}
			
			SAFE_DELETE(pClient);
            break;
		}
		else
		{
			++it;
		}
	}
}

int KSO3Interaction::AddParam(const char * pcszParamName, const char * pcszValue, BOOL bFile)
{
	int nRetCode = false;

	if (!m_pTreadHttpClient)
	{
		m_pTreadHttpClient = new KTreadHttpClient;
		KGLOG_PROCESS_ERROR(m_pTreadHttpClient);
	}

	nRetCode = m_pTreadHttpClient->HttpClient.AddPostParam(pcszParamName, pcszValue, bFile ? KG_HttpClient::eBinaryFile : KG_HttpClient::eNormal);
	KGLOG_PROCESS_ERROR(nRetCode);

	return true;
Exit0:
	return false;
}

int KSO3Interaction::Send(
    const char *pcszName,
    const char *pcszAddress,
    const char *pcszObjectName,
    const char *pcszVerb,
    int nPort,
    fnSendResult fnResult
)
{
	int nRetCode = false;
	KTreadHttpClient *pClient = NULL;

	if (!m_pTreadHttpClient)
	{
		m_pTreadHttpClient = new KTreadHttpClient;
		KGLOG_PROCESS_ERROR(m_pTreadHttpClient);
	}
	pClient = m_pTreadHttpClient;
	m_pTreadHttpClient = NULL;

	strncpy(pClient->szName, pcszName, _countof(pClient->szName));
	pClient->szName[_countof(pClient->szName) - 1] = '\0';
	strncpy(pClient->szAddress, pcszAddress, _countof(pClient->szAddress));
    pClient->szAddress[_countof(pClient->szAddress) - 1] = '\0';
    strncpy(pClient->szObjectName, pcszObjectName, _countof(pClient->szObjectName));
    pClient->szObjectName[_countof(pClient->szObjectName) - 1] = '\0';
	strncpy(pClient->szVerb, pcszVerb, _countof(pClient->szVerb));
	pClient->szVerb[_countof(pClient->szVerb) - 1] = '\0';
	pClient->nPort = nPort;
	pClient->SendResultFunc = fnResult;
	m_KTreadHttpClientArray.push_back(pClient);
	pClient->Thread.Create(ThreadFunction, pClient);

	return true;
Exit0:
	return false;
}

int KSO3Interaction::Request(
    const char *pcszName,
    const char *pcszAddress,
    const char *pcszObjectName,
    const char *pcszVerb,
    int nPort,
    fnRequestResult fnResult
)
{
	int nRetCode = false;
	KTreadHttpClient * pClient = NULL;

	if (!m_pTreadHttpClient)
	{
		m_pTreadHttpClient = new KTreadHttpClient;
		KGLOG_PROCESS_ERROR(m_pTreadHttpClient);
	}
	pClient = m_pTreadHttpClient;
	m_pTreadHttpClient = NULL;

	strncpy(pClient->szName, pcszName, _countof(pClient->szName));
	pClient->szName[_countof(pClient->szName) - 1] = '\0';
	strncpy(pClient->szAddress, pcszAddress, _countof(pClient->szAddress));
    pClient->szAddress[_countof(pClient->szAddress) - 1] = '\0';
    strncpy(pClient->szObjectName, pcszObjectName, _countof(pClient->szObjectName));
    pClient->szObjectName[_countof(pClient->szObjectName) - 1] = '\0';
	strncpy(pClient->szVerb, pcszVerb, _countof(pClient->szVerb));
	pClient->szVerb[_countof(pClient->szVerb) - 1] = '\0';
	pClient->nPort = nPort;
	pClient->RequestResultFunc = fnResult;
	pClient->bRequest = TRUE;
	m_KTreadHttpClientArray.push_back(pClient);
	pClient->Thread.Create(ThreadFunction, pClient);

	return true;
Exit0:
	return false;
}

void KSO3Interaction::ThreadFunction(void* lpParam)
{
	int nRetCode = false;
	KTreadHttpClient *pClient = (KTreadHttpClient *)lpParam;

	assert(pClient);
    if (pClient->szVerb[0] == '\0')
    {
        nRetCode = pClient->HttpClient.RequestGet(
            "SO3Interaction",
            pClient->szAddress,
            pClient->szObjectName,
            pClient->nPort
        );
    }
    else
    {
        nRetCode = pClient->HttpClient.RequestPost(
            "SO3Interaction",
            pClient->szAddress,
            pClient->szObjectName,
            pClient->nPort
        );
    }

	if (!nRetCode)
	{
		pClient->bSucess = FALSE;
		pClient->bFinished = TRUE;
		return;
	}

	if (pClient->bRequest)
	{
		nRetCode = pClient->HttpClient.GetResponse(&pClient->pbyBuffer, &pClient->dwBufferSize);
		if (!nRetCode)
		{
			pClient->bSucess = FALSE;
			pClient->bFinished = TRUE;
			return;
		}
	}

	pClient->bSucess = TRUE;
	pClient->bFinished = TRUE;
}

void KSO3Interaction::AddRef()
{
	m_nRef++;
}

extern "C" __declspec(dllexport) ISO3Interaction* CreateSO3Interaction()
{
	if (g_pKSO3Interaction)
		g_pKSO3Interaction->AddRef();
	else
		g_pKSO3Interaction = new KSO3Interaction;

	return g_pKSO3Interaction;
}