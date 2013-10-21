////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KG3DMessageBroadcasterGroupImp.cpp
//  Version     : 1.0
//  Creator     : Chen Tianhong
//  Create Date : 2008-12-1 9:58:31
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "KG3DMessageBroadcasterGroupImp.h"
#include "KG3DMessenger.h"

#ifdef _DEBUG
#define new DEBUG_NEW_3DENGINE
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

////////////////////////////////////////////////////////////////////////////////

namespace
{
	//作为Dummy，禁止掉所有行为
	struct KG3DMessageSenderPrivate : public  KG3DMessageSender
	{
		virtual bool __stdcall UnRegisterReceiver( IKG3DMessageReceiverBase* pReceiver ){return false;}
		virtual bool __stdcall RegisterReceiver( IKG3DMessageReceiverBase* pReceiver ){return false;}
		virtual void PostMessage( const KG3DMessage& Message ){return;}
	};

	static  KG3DMessageSenderPrivate stSender;
};

IKG3DMessageSenderBase& KG3DMessageBroadcasterGroupImp::GetBroadcaster( DWORD dwType )
{
	size_t uIndex = static_cast<size_t>(dwType - m_dwFirstID);
	_ASSERTE(uIndex < m_SendersCount);
	
	return m_pSenders != NULL ? m_pSenders[uIndex]: stSender;
}

HRESULT __stdcall KG3DMessageBroadcasterGroupImp::RegisterListener( DWORD dwType, IKG3DMessageReceiverBase& receiver )
{
	bool bRet = this->GetBroadcaster(dwType).RegisterReceiver(&receiver);
	return bRet ? S_OK : E_FAIL;
}

HRESULT __stdcall KG3DMessageBroadcasterGroupImp::UnRegisterListener( DWORD dwType, IKG3DMessageReceiverBase& receiver )
{
	bool bRet = this->GetBroadcaster(dwType).UnRegisterReceiver(&receiver);
	return bRet ? S_OK : E_FAIL;
}

KG3DMessageBroadcasterGroupImp::KG3DMessageBroadcasterGroupImp( size_t uBroadcasterCount, DWORD dwFirstID )
:m_dwFirstID(dwFirstID)
,m_SendersCount(uBroadcasterCount)
,m_pSenders(NULL)
{
	_ASSERTE(uBroadcasterCount < 100);
	m_pSenders = new KG3DMessageSender[uBroadcasterCount];
}

KG3DMessageBroadcasterGroupImp::~KG3DMessageBroadcasterGroupImp()
{
	SAFE_DELETE_ARRAY(m_pSenders);
}


IKG3DMessageSenderBase& KG3DMessageBroadcasterGroupDummy::GetBroadcaster(DWORD dwType)
{
	return stSender;
}
HRESULT __stdcall KG3DMessageBroadcasterGroupDummy::RegisterListener(DWORD dwType, IKG3DMessageReceiverBase& receiver)
{
	return false;
}
HRESULT __stdcall KG3DMessageBroadcasterGroupDummy::UnRegisterListener(DWORD dwType, IKG3DMessageReceiverBase& receiver)
{
	return false;
}
