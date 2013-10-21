////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KG3DSystemEventListener.cpp
//  Version     : 1.0
//  Creator     : Chen Tianhong
//  Create Date : 2008-8-12 15:39:05
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "KG3DSystemEventListener.h"
#include "KG3DMessenger.h"

////////////////////////////////////////////////////////////////////////////////
class KG3DSystemEventListenerSolid : public KG3DSystemEventListener, public KG3DMessageSender
{
	virtual VOID OnD3DDeviceCreate()
	{
		this->PostMessage(KG3DMessage(KM_D3DDEVICE_CREATE));
	}
	virtual VOID OnD3DDeviceRelease()
	{
		this->PostMessage(KG3DMessage(KM_D3DDEVICE_RELEASE));
	}
	virtual VOID OnD3DDeviceLost()
	{
		this->PostMessage(KG3DMessage(KM_D3DDEVICE_LOST));
	}
	virtual VOID OnD3DDeviceReset()
	{
		this->PostMessage(KG3DMessage(KM_D3DDEVICE_RESET));
	}
	virtual bool __stdcall UnRegisterReceiver( IKG3DMessageReceiverBase* pReceiver )
	{
		return KG3DMessageSender::UnRegisterReceiver(pReceiver);
	}
	virtual bool __stdcall RegisterReceiver( IKG3DMessageReceiverBase* pReceiver )
	{
		return KG3DMessageSender::RegisterReceiver(pReceiver);
	}
};



KG3DSystemEventListener& g_GetSystemEventListener()
{
	static KG3DSystemEventListenerSolid stLt;
	return stLt;
}