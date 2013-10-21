////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KG3DSystemEventListener.h
//  Version     : 1.0
//  Creator     : Chen Tianhong
//  Create Date : 2008-8-12 15:39:14
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _INCLUDE_KG3DSYSTEMEVENTLISTENER_H_
#define _INCLUDE_KG3DSYSTEMEVENTLISTENER_H_

////////////////////////////////////////////////////////////////////////////////
class IKG3DMessageReceiverBase;
class KG3DSystemEventListener
{
public:
	virtual VOID OnD3DDeviceCreate() = 0;
	virtual VOID OnD3DDeviceRelease() = 0;
	virtual VOID OnD3DDeviceLost() = 0;
	virtual VOID OnD3DDeviceReset() = 0;

	virtual bool __stdcall UnRegisterReceiver( IKG3DMessageReceiverBase* pReceiver ) = 0;
	virtual bool __stdcall RegisterReceiver( IKG3DMessageReceiverBase* pReceiver ) = 0;
	virtual ~KG3DSystemEventListener() = 0{}
};

extern KG3DSystemEventListener& g_GetSystemEventListener();

struct KG3DSystemEventListenerDummy : public KG3DSystemEventListener 
{
	virtual VOID OnD3DDeviceCreate(){return;}
	virtual VOID OnD3DDeviceRelease(){return;}
	virtual VOID OnD3DDeviceLost(){return;}
	virtual VOID OnD3DDeviceReset(){return;}

	virtual bool __stdcall UnRegisterReceiver( IKG3DMessageReceiverBase* pReceiver ){return true;}
	virtual bool __stdcall RegisterReceiver( IKG3DMessageReceiverBase* pReceiver ){return true;}
};

#endif //_INCLUDE_KG3DSYSTEMEVENTLISTENER_H_
