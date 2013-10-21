/* -------------------------------------------------------------------------
//	文件名		：	kguardclient.h
//	创建者		：	wangbin
//	创建时间	：	2008-1-7 9:53:00
//	功能描述	：	进程间通信基类
// -----------------------------------------------------------------------*/
#ifndef __KGUARDCLIENT_H__
#define __KGUARDCLIENT_H__

#include <list>
#include <time.h>
#include "kclientipcstream.h"
#include "fseye_protocol.h"
#include "ksdef.h"

class KGuardClient
{
public:
	enum
	{
		emKPING_INTERVAL = 20,	// ping的间隔时间（秒）
	};
	KGuardClient() : m_tLastPing(0), m_pStream(NULL)
	{
	}
	virtual ~KGuardClient()
	{
	}
	virtual BOOL Initialize(IKPackStream* pStream)
	{
		ASSERT(pStream);
		m_pStream = pStream;
		return TRUE;
	}
	virtual VOID UnInitialize()
	{
		m_pStream = NULL;
	}
	virtual VOID Activate(time_t tNow)
	{
		if (!m_pStream)
			return;
		// 处理协议
		size_t nLen = 0;
		CHAR* pData = m_pStream->GetPack(nLen);
		while (pData)
		{
			ASSERT(nLen > 0);
			BYTE byProtocol = *(BYTE*)pData;
			if (byProtocol == e2l_getpid_def)
			{
				l2e_getpid sMsg;
				sMsg.wProtocol = l2e_getpid_def;
				sMsg.wServer   = 0;
				sMsg.nPid	   = getpid();
				m_pStream->PutPack(&sMsg, sizeof(sMsg));
			}
			else if (byProtocol == g2l_header_def)
			{
				ProcessGuardMessages(pData, nLen);
			}
			else
			{
				ProcessMessage(pData, nLen);
			}
			m_pStream->PopPack();
			pData = m_pStream->GetPack(nLen);
		}
		if (tNow - m_tLastPing > emKPING_INTERVAL)
		{
			Ping();
			m_tLastPing = tNow;
		}
	}
protected:
	VOID Ping()
	{
		if (m_pStream)
		{
			KSubProtocolHead sPing;
			sPing.wProtocol		= l2e_header_def;
			sPing.wSubProtocol	= l2e_ping_def;
			sPing.wServer		= 0;
			m_pStream->PutPack(&sPing, sizeof(sPing));
		}
	}
private:
	virtual VOID ProcessMessage(CHAR* pData, size_t nLen) {}
	void ProcessGuardMessages(CHAR* pData, size_t nLen)
	{
		BYTE bySubProto = *(BYTE*)pData;
		KSubProtocolHead* pHead = (KSubProtocolHead*)pData;
		if (nLen == sizeof(KSubProtocolHead) &&
			pHead->wSubProtocol == emKPROTOCOL_G2L_PIDREQ)
		{
			KPROTOCOL_L2G_PIDRET sRet;
			sRet.wProtocol		= l2g_header_def;
			sRet.wSubProtocol	= emKPROTOCOL_L2G_PIDRET;
			sRet.nPid			= getpid();
			sRet.wServer		= 0;
			m_pStream->PutPack(&sRet, sizeof(sRet));
		}
	}
protected:
	IKPackStream*	m_pStream;
private:
	time_t	m_tLastPing;
};

#endif //__KGUARDCLIENT_H__
