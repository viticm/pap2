#include "stdafx.h"
#include "KApexClient.h"
#include "KPlayerClient.h"
#include "KSO3World.h"
#ifdef _CLIENT

long NetSendToGameServer(const char* pBuffer,int nLen)
{
    IKG_Buffer* piBuffer = NULL;
    BYTE*       pbyData   = NULL;

    assert(pBuffer);

    KGLOG_PROCESS_ERROR(nLen >= 0);

    piBuffer = KG_MemoryCreateBuffer((unsigned)nLen);
    KGLOG_PROCESS_ERROR(piBuffer);

    pbyData = (BYTE*)piBuffer->GetData();
    KGLOG_PROCESS_ERROR(pbyData);

    memcpy(pbyData, pBuffer, nLen);

    g_pSO3World->m_ApexClient.PushSendData(piBuffer);

Exit0:
    KG_COM_RELEASE(piBuffer);
    return 0;
}

KApexClient::KApexClient()
{
}

BOOL KApexClient::Init()
{
    m_bRun = false;
    m_pRecvFunc = NULL;

    return true;
}

void KApexClient::UnInit()
{
}

void KApexClient::Activate()
{
    IKG_Buffer* piBuffer = NULL;
    BYTE*       pbyData  = NULL;
    size_t      uSize    = 0;

    m_Mutex.Lock();

    if (!m_bRun)
    {
        goto Exit0;
    }

    while (!m_SendPackageQue.empty())
    {
        piBuffer = m_SendPackageQue.front();
        m_SendPackageQue.pop();
        KGLOG_PROCESS_ERROR(piBuffer);

        pbyData = (BYTE*)piBuffer->GetData();
        KGLOG_PROCESS_ERROR(pbyData);

        uSize = piBuffer->GetSize();

        g_PlayerClient.DoApexProtocol(true, pbyData, uSize);

        KG_COM_RELEASE(piBuffer);
    }

Exit0:
    KG_COM_RELEASE(piBuffer);
    m_Mutex.Unlock();
}

BOOL KApexClient::StartApexClient()
{
    BOOL    bResult     = false;
    int     nRetCode    = false;

    if (m_bRun)
    {
        goto Exit0;
    }

    nRetCode = CHCStart(NetSendToGameServer, m_pRecvFunc);

    g_PlayerClient.DoApexProtocol(false, (BYTE*)&nRetCode, sizeof(int));

    m_bRun = true;

    bResult = true;
Exit0:
    return bResult;
}

void KApexClient::EndApexClient()
{
    CHCEnd();

    m_Mutex.Lock();

    while (!m_SendPackageQue.empty())
    {
        m_SendPackageQue.pop();
    }

    m_Mutex.Unlock();

    m_bRun = false;
}

void KApexClient::OnRecvUserData(BYTE* pbyData, size_t uDataSize)
{
    KG_PROCESS_ERROR(m_pRecvFunc);

    m_pRecvFunc((const char*)pbyData, (int)uDataSize);

Exit0:
    return;
}

void KApexClient::PushSendData(IKG_Buffer* piBuffer)
{
    m_Mutex.Lock();

    piBuffer->AddRef();
    m_SendPackageQue.push(piBuffer);

    m_Mutex.Unlock();
}

#endif
