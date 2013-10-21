//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright (C) 2006
//
//      Created_datetime : 2006-08-03 11:00
//      File_base        : client
//      File_ext         : .h
//      Author           : zolazuo(zuolizhi)
//      Description      : 
//
//////////////////////////////////////////////////////////////////////
#ifndef _GUARD_FILEDL_H_
#define _GUARD_FILEDL_H_

#include "def.h"

class KGuardClient;

class _FileTrans
{
public:
	
	_FileTrans( );
	~_FileTrans( );

	int Init(KGuardClient* pGuardClient);

	int ProcessNetMessage( 
		unsigned char* pData,
		unsigned int datasize );
	
	void Disconnect( );

	void CompleteExeCMD( );

	void checkcmdres( );	

private:
	
	int OpenFile(
		char szFileName[],
		unsigned int& nFileLen,
		unsigned char bFlag,
		unsigned char bFullPath );

	int ReadFile( 
		unsigned char szBuf[], 
		unsigned int nDataLen,
		unsigned int& nReadLen );

	int WriteFile( 
		unsigned char szBuf[], 
		unsigned int nDataLen,
		unsigned int& nWritedLen );

	int SeekFile( 
		int bKeep,
		unsigned int nOffset );

	int CloseFile( );

	int exesyscmd(LPCSTR pszCmd, LPCSTR pszInput, BOOL bNeedOutput);

private:

	int				m_nCMDComplete;
	int				m_nCMDPID;
	int				m_nInStream;
	int				m_nOutStream;

	FILE*			m_stream;
	KGuardClient*	m_pGuardClient;
	CHAR			m_szBuff[8192];
};

#endif
