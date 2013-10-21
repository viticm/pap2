////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KG3DMessageBroadcasterGroupImp.h
//  Version     : 1.0
//  Creator     : Chen Tianhong
//  Create Date : 2008-12-1 9:58:27
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _INCLUDE_KG3DMESSAGEBROADCASTERGROUPIMP_H_
#define _INCLUDE_KG3DMESSAGEBROADCASTERGROUPIMP_H_
#include "KG3DMessageBroadcasterGroup.h"

class IKG3DMessageSenderBase;
class IKG3DMessageReceiverBase;
class KG3DMessageSender;
////////////////////////////////////////////////////////////////////////////////
class KG3DMessageBroadcasterGroupImp : public KG3DMessageBroadcasterGroup 
{
public:
	virtual IKG3DMessageSenderBase& GetBroadcaster(DWORD dwType);
	virtual HRESULT __stdcall RegisterListener(DWORD dwType, IKG3DMessageReceiverBase& receiver);
	virtual HRESULT __stdcall UnRegisterListener(DWORD dwType, IKG3DMessageReceiverBase& receiver);

	KG3DMessageBroadcasterGroupImp(size_t uBroadcasterCount, DWORD dwFirstID);
	~KG3DMessageBroadcasterGroupImp();
protected:
	DWORD m_dwFirstID;
	KG3DMessageSender*	m_pSenders;
	size_t m_SendersCount;
};

#endif //_INCLUDE_KG3DMESSAGEBROADCASTERGROUPIMP_H_
