////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KG3DMessageBroadcasterGroup.h
//  Version     : 1.0
//  Creator     : Chen Tianhong
//  Create Date : 2008-12-1 9:55:17
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _INCLUDE_KG3DMESSAGEBROADCASTERGROUP_H_
#define _INCLUDE_KG3DMESSAGEBROADCASTERGROUP_H_

////////////////////////////////////////////////////////////////////////////////
struct IKG3DMessageReceiverBase;
struct IKG3DMessageSenderBase;
struct KG3DMessageBroadcasterGroup
{
	virtual IKG3DMessageSenderBase& GetBroadcaster(DWORD dwType) = 0;
	virtual HRESULT __stdcall RegisterListener(DWORD dwType, IKG3DMessageReceiverBase& receiver) = 0;
	virtual HRESULT __stdcall UnRegisterListener(DWORD dwType, IKG3DMessageReceiverBase& receiver) = 0;
	virtual ~KG3DMessageBroadcasterGroup() = 0{}
};

struct KG3DMessageBroadcasterGroupDummy : public KG3DMessageBroadcasterGroup
{
	virtual IKG3DMessageSenderBase& GetBroadcaster(DWORD dwType);
	virtual HRESULT __stdcall RegisterListener(DWORD dwType, IKG3DMessageReceiverBase& receiver);
	virtual HRESULT __stdcall UnRegisterListener(DWORD dwType, IKG3DMessageReceiverBase& receiver);
};

#endif //_INCLUDE_KG3DMESSAGEBROADCASTERGROUP_H_
