//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright (C) 2006
//
//      Created_datetime : 2007-03-12 17:00
//      File_base        : sysinfo.cpp
//      File_ext         : .h
//      Author           : zolazuo(zuolizhi)
//      Description      : 
//
//////////////////////////////////////////////////////////////////////

#ifndef _MESPUMP_H_
#define _MESPUMP_H_
#include "def.h"
#include "utility.h"
/////////////////////////////////////////////////////////////////////////////
//
//              Global variables and Macro and Structure Definitions
//
/////////////////////////////////////////////////////////////////////////////

class mespump
{
public:
	mespump( );
	~mespump( );

	int Init( );

	int SendPack( const void* pData, unsigned int datasize );
	const void* RcvPack( unsigned int& datasize );

protected:

	void* GetShareMemory(
		const char* pKeyName );
	
private:

	void* m_pShareMemToLord;
	void* m_pShareMemFromLord;

	IPCStream* m_ToLord;
	IPCStream* m_FromLord;
};


#endif