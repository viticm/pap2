#ifndef _KAPEX_CLIENT_H_
#define _KAPEX_CLIENT_H_

#ifdef _CLIENT
#include "Apex/ApexClient.h"
#include "Common/KG_Memory.h"
#include <queue>
class KApexClient
{
public:
    KApexClient();

    BOOL Init();
    void UnInit();

    void Activate();

    BOOL StartApexClient();
    void EndApexClient();

    void OnRecvUserData(BYTE* pbyData, size_t uDataSize);

    void PushSendData(IKG_Buffer* piBuffer);

private:
	BOOL                    m_bRun;
	_FUNC_C_REC             m_pRecvFunc;

	std::queue<IKG_Buffer*> m_SendPackageQue;
	KMutex                  m_Mutex;
};
#endif // _CLIENT

#endif // _KAPEX_CLIENT_H_
