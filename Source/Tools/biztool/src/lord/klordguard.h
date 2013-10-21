#ifndef __KLORDGUARD_H__
#define __KLORDGUARD_H__

#include "kguardclient.h"
#include "kguardtimer_t.h"

class KLordGuard : public KGuardClient
{
public:
	KLordGuard();
	~KLordGuard();
	virtual VOID Activate();
private:
	VOID SendPlayerCount();
	virtual VOID ProcessMessage(CHAR* pData, size_t nLen);
private:
	KGuardTimer_T<KLordGuard> m_cTimer;
};

#endif //__KLORDGUARD_H__
