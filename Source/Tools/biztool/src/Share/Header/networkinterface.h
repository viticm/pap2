//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright (C) 2006
//
//      Created_datetime : 2006-06-28 18:10
//      File_base        : networkinterface.h
//      File_ext         : .h
//      Author           : zolazuo(zuolizhi)
//      Description      : network interface
//
//////////////////////////////////////////////////////////////////////
#ifndef _NETWORK_INTERFACE_H_
#define _NETWORK_INTERFACE_H_

#ifndef interface
#define interface struct
#endif

#include "IEncrypt.h"
/////////////////////////////////////////////////////////////////////////////
//
//              Interface Declare
//
/////////////////////////////////////////////////////////////////////////////
enum enumClientConnectInfo 
{
	enumClientConnectCreate = 0x100,
	enumClientConnectClose
};

typedef void (*CALLBACK_SERVER_EVENT)(
		void * lpParam, 
		const unsigned long ulnID, 
		const unsigned long ulnEventType);

enum enumServerConnectInfo {
	enumServerConnectCreate = 0x100,
	enumServerConnectClose
};

typedef void (*CALLBACK_CLIENT_EVENT )(
		void *lpParam, 
		const unsigned long uInID, 
		const unsigned long ulnEventType);

interface IServer
{

	virtual
	int Startup( ) = 0;
	
	virtual
	int Cleanup( ) = 0;

	virtual
	int Release( ) = 0;

	virtual 
	int OpenSvc(
	const unsigned int intf_idx, 
	const unsigned short usnPort ) = 0;
	
	virtual
	int CloseService( ) = 0;
	
	virtual
	int RegisterMsgFilter(
		void * lpParam, 
		CALLBACK_SERVER_EVENT pfnEventNotify ) = 0;

	virtual
	int PackDataToClient(
		const unsigned int ulnClientID, 
		const void *pData, 
		unsigned int datalength ) = 0;

	virtual
	int SendPackToClient( int index ) = 0;

	virtual
	int SendData(
		const unsigned int ulnClientID, 
		const void *pData, 
		unsigned int datalength ) = 0;

	virtual
	const void *GetPackFromClient(
		unsigned int ulnClientID, 
		unsigned int &datalength ) = 0;

	virtual
	int ShutdownClient( const unsigned int ulnClientID ) = 0;

	virtual
	const char * GetClientInfo( const unsigned int ulnClientID ) = 0;

	//-----------------------------
	virtual
	int PreparePackSink( ) = 0;

	virtual
	int SendPackToClient( ) = 0;
};

interface IClient
{
	virtual int  Startup( ) = 0;
	virtual int  Cleanup( ) = 0;
	virtual int  Shutdown( ) = 0;
	virtual int  Release( ) = 0;
	virtual int  ConnectTo(char * strAddr, unsigned short usPort) = 0;
	virtual int  Disconnect(unsigned int uiConnID) = 0;
	virtual void RegisterMsgFilter(void * lpParam, CALLBACK_CLIENT_EVENT pfnEventNotify ) = 0;
	virtual int  SendPackToServer( 
		unsigned int uiConnID, 
		const void * pData, 
		unsigned int length ) = 0;

	virtual
	const void *GetPackFromServer(
		unsigned int uiConnID, 
		unsigned int &length ) = 0;

	virtual
	int  FlushData( ) = 0;

	virtual
	int  FlushData( unsigned int uiConnID ) = 0;

	virtual 
	bool IsAllRecvKey( )	=	0;

	virtual 
	bool IsRecvKey( unsigned int uiConnID )	=	0;
	virtual void Bind(const char* pszBindIp) = 0;
};

void CreateServer( 
		IServer* & pServer,
		unsigned int number, 
		IEncrypt*	pEncrypt = 0,
		unsigned int thread_number = 0, 
		unsigned int max_send = 256 * 1024, 
		unsigned int max_receive = 32 * 1024 );

void CreateClient( 
		IClient* & pClient ,		
		unsigned int max_conn, 
		IEncrypt*	pEncrypt = 0,
		int max_send = 1024*1024, 
		int max_receive = 1024*1024 );

#endif
