#include "stdafx.h"
#include "fseye_protocol.h"
#include "klordguard.h"

KLordGuard::KLordGuard()
{
	m_cTimer.Register(this, &KLordGuard::SendPlayerCount, 60);
}

KLordGuard::~KLordGuard()
{
}

VOID KLordGuard::Activate()
{
	time_t tNow = time(NULL);
	KGuardClient::Activate(tNow);
	m_cTimer.Activate(tNow);
}

VOID KLordGuard::SendPlayerCount()
{
	CHAR szBuff[sizeof(l2e_PlayerCount) + sizeof(INT) * emKGUARD_PLAYERCOUNT_COUNT];
	l2e_PlayerCount* pProtocol = (l2e_PlayerCount*)szBuff;

	pProtocol->wProtocol	= l2e_header_def;
	pProtocol->wServer		= 0;
	pProtocol->wSubProtocol = l2e_PlayerCount_def;
	pProtocol->uCount		= emKGUARD_PLAYERCOUNT_COUNT;

	INT* pnCount = (INT*)(pProtocol + 1);
	for (INT i = 0; i < emKGUARD_PLAYERCOUNT_COUNT; ++i)
	{
		pnCount[i] = -1;
	}
	pnCount[emKGUARD_PLAYERCOUNT_TOTOAL	] = 100;	// 总人数
	pnCount[emKGUARD_PLAYERCOUNT_OFFLIVE] = 10;		// 离线托管人数

	m_pStream->PutPack(szBuff, sizeof(szBuff));
}

VOID KLordGuard::ProcessMessage(CHAR* pData, size_t nLen)
{
	BYTE byProtocol = *(BYTE*)pData;
	extern BOOL g_bRunning;
	switch (byProtocol)
	{
	case e2l_exit_def:
		g_bRunning = FALSE;
		break;
	}
}

