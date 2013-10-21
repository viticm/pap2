//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright (C) 2006
//
//      Created_datetime : 2006-08-16 11:00
//      File_base        : sysinfo
//      File_ext         : .h
//      Author           : zolazuo(zuolizhi)
//      Description      : 
//
//////////////////////////////////////////////////////////////////////
#ifndef _IFIMPLE_H_
#define _IFIMPLE_H_

#include "def.h"
#include "mespump.h"

class monitor : public IGuardPlugin
{
public:
	monitor( );
	~monitor( );

	int LoadPlug( ISender*	pSender );
	
	int Release( );
	
	int ProcessNetMessage(
		unsigned char* pData,
		unsigned int datasize );
	
	int Disconnect( );
	
	int GetDescribe( 
		char* szDes,
		int nSize );
	
	int GetGUID( 
		char* szGUID,
		int nSize );
	
	int GetVersion( 
		int& nMaxVer, 
		int& nMinVer );
	
	int GetAuthor(
		char* szAuthor,
		int nSize  );

	int Breathe( );

private:


private:
	ISender*	m_pSender;
	void*		m_pDBuff;
	mespump		m_MesPump;
};

#endif