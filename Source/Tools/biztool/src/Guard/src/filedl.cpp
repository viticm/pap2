//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright (C) 2006
//
//      Created_datetime : 2006-08-03 11:00
//      File_base        : client
//      File_ext         : .cpp
//      Author           : zolazuo(zuolizhi)
//      Description      : 
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "filedl.h"
#include "client.h"
/////////////////////////////////////////////////////////////////////////////
//
//              Global variables and Macro and Structure Definitions
//
/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
//
//              Function Definitions
//
/////////////////////////////////////////////////////////////////////////////

_FileTrans::_FileTrans() : m_pGuardClient(NULL)
{
	m_stream	=	NULL;
	m_nCMDComplete	=	FALSE;
	m_nCMDPID	=	INVALID_VALUE;
	m_nInStream =	INVALID_VALUE;
	m_nOutStream=	INVALID_VALUE;
}

_FileTrans::~_FileTrans( )
{
}

int _FileTrans::ProcessNetMessage( 
		unsigned char* pData,
		unsigned int datasize )
{
	switch( PH( pData ) )
	{
	case e2g_openfile_def:
		{
			if( datasize != sizeof(e2g_openfile) )
				break;

			e2g_openfile* pOF = (e2g_openfile*)pData;
			g2e_openfile	g2eof;
			g2eof.wProtocol = g2e_openfile_def;
			g2eof.wServer	= 0;
			g2eof.nRetCode	= OpenFile( 
				pOF->szFileName,
				g2eof.nFileLen,
				pOF->bFlag, 
				pOF->bFullPath );

			m_pGuardClient->SendPackToServer( 
				(unsigned char*)&g2eof, 
				sizeof(g2e_openfile) );
		} 
		break;
	case e2g_readfile_def:
		{
			e2g_readfile* pRF = (e2g_readfile*)pData;
			if (datasize != sizeof(e2g_readfile) ||
				pRF->nDataLen > sizeof(m_szBuff) - sizeof(g2e_readfile))
			{
				ASSERT(FALSE);
				break;
			}
			
			g2e_readfile* pRes = (g2e_readfile*)m_szBuff;
			unsigned char *pszBuf = (unsigned char *)(pRes + 1);
			pRes->wProtocol = g2e_readfile_def;
			pRes->wServer	= 0;
			pRes->nRetCode	= ReadFile(pszBuf, pRF->nDataLen, pRes->nReadLen);
			
			m_pGuardClient->SendPackToServer( 
				(unsigned char*)pRes, 
				sizeof(g2e_readfile) + pRes->nReadLen);
		}
		break;
	case e2g_writefile_def:
		{
			e2g_writefile* pWF = (e2g_writefile*)pData;
			if (datasize != sizeof(e2g_writefile) + pWF->nDataLen ||
				pWF->nDataLen == 0)
			{
				ASSERT(FALSE);
				break;
			}
			
			UCHAR* pszBuf = (UCHAR*)(pWF + 1);
			g2e_writefile g2ewf;
			g2ewf.wProtocol = g2e_writefile_def;
			g2ewf.wServer	= 0;
			g2ewf.nRetCode	= WriteFile(pszBuf, pWF->nDataLen, g2ewf.nWritedLen);

			m_pGuardClient->SendPackToServer( 
				(unsigned char*)&g2ewf, 
				sizeof(g2e_writefile) );
		}
		break;
	case e2g_seekfile_def:
		{
			if( datasize != sizeof(e2g_seekfile) )
				break;

			e2g_seekfile* pSF = (e2g_seekfile*)pData;
			g2e_seekfile g2esf;
			g2esf.wProtocol = g2e_seekfile_def;
			g2esf.wServer	= 0;
			g2esf.nRetCode	= SeekFile( pSF->bKeep, pSF->nOffset );

			m_pGuardClient->SendPackToServer( 
				(unsigned char*)&g2esf, 
				sizeof(g2e_seekfile) );
		}
		break;
	case e2g_closefile_def:
		{
			if( datasize != sizeof(KProtocolHead) )
				break;

			g2e_closefile g2ecf;
			g2ecf.wProtocol = g2e_closefile_def;
			g2ecf.wServer	= 0;
			g2ecf.nRetCode	= CloseFile( );
			
			m_pGuardClient->SendPackToServer( 
				(unsigned char*)&g2ecf, 
				sizeof(g2e_closefile) );
		}
		break;
	case e2g_exesyscmd_def:
		{
			e2g_exesyscmd* pEC = (e2g_exesyscmd*)pData;
			if (datasize != sizeof(e2g_exesyscmd) + pEC->nCmdLen + pEC->nBufLen ||
				pEC->nCmdLen == 0 ||
				pEC->nBufLen == 0)
			{
				break;
			}
			g2e_exesyscmd g2eec;
			memset(&g2eec, 0, sizeof(g2eec));
			g2eec.wProtocol = g2e_exesyscmd_def;
			g2eec.wServer = 0;
			g2eec.ReturnCode = g2e_ExeSysCmd_busy;

			LPSTR pszCmd = (LPSTR)(pEC + 1);
			LPSTR pszBuf = &pszCmd[pEC->nCmdLen];
			pszCmd[pEC->nCmdLen - 1] = '\0';
			pszBuf[pEC->nBufLen - 1] = '\0';
			if (m_nCMDPID == INVALID_VALUE)
			{
				exesyscmd(pszCmd, pszBuf, pEC->bNeedOutput);
			}
			else
			{
				m_pGuardClient->Log("Fialed to execute command [%s:%s]: busy",
					pszCmd,
					pszBuf);
				m_pGuardClient->SendPackToServer((unsigned char*)&g2eec, sizeof(g2e_exesyscmd));
			}
		}
		break;

	case e2g_exesyscmd_large_def:
		{
			e2g_exesyscmd_large* pECL = (e2g_exesyscmd_large*)pData;
			g2e_exesyscmd g2eec;
			memset(&g2eec, 0, sizeof(g2eec));
			g2eec.wProtocol = g2e_exesyscmd_def;
			g2eec.wServer = 0;
			g2eec.ReturnCode = g2e_ExeSysCmd_busy;
			
			if( m_nCMDPID == INVALID_VALUE )
				exesyscmd( pECL->Command, pECL->InputBuff, pECL->bNeedOutput);
			else
				m_pGuardClient->SendPackToServer( 
				(unsigned char*)&g2eec, 
				sizeof(g2e_exesyscmd) );
		}
		break;
	case e2g_GetGuardDir_def:
		{
			if( datasize != sizeof(KProtocolHead) )
				break;

			g2e_GetGuardDir gdir;
			memset(&gdir, 0, sizeof(gdir));

			gdir.wProtocol = g2e_GetGuardDir_def;
			gdir.wServer = 0;
			getcwd( gdir.GuardDir, sizeof(gdir.GuardDir) );

			m_pGuardClient->SendPackToServer( 
				(unsigned char*)&gdir, 
				sizeof(gdir) );

		}
		break;
	/*
	case e2g_UpdateGuard_def:
		exit( 0 );
		break;
	*/
	default:
		return guard_err;
	}

	return fseye_success;
}

int _FileTrans::OpenFile(
		char szFileName[],
		unsigned int& nFileLen,
		unsigned char bFlag,
		unsigned char bFullPath )
{
	char szFullFileName[MAX_NAME];

	if( m_stream != NULL )
		return filetran_opening_err;

	if( bFlag > 2 )
		return filetran_opening_err;
	
	if( bFullPath )
		sprintf( szFullFileName, "%s", szFileName );
	else
		sprintf( szFullFileName, "%s%s", FILEPATH, szFileName );

	nFileLen = 0;
	
	if( bFlag == 2 )
	{
		if( (m_stream  = fopen( szFullFileName, "r" )) == NULL )
		{
			return filetran_app_err;
		}
		else
		{
			fseek( m_stream, 0, SEEK_END );
			nFileLen = ftell( m_stream );
			fclose( m_stream );
			if( (m_stream  = fopen( szFullFileName, "ab" )) == NULL )
				return filetran_cre_err;
		}
	}
	else
	{
		if( bFlag == 0 )
		{
			if( (m_stream  = fopen( szFullFileName, "rb" )) == NULL )
				return filetran_cre_err;
		}

		if( bFlag == 1 )
		{
			if( (m_stream  = fopen( szFullFileName, "wb" )) == NULL )
				return filetran_cre_err;
		}

		fseek( m_stream, 0, SEEK_END );
		nFileLen = ftell( m_stream );
		fseek( m_stream, 0, SEEK_SET );
	}

	return fseye_success;
}

int _FileTrans::ReadFile( 
	unsigned char szBuf[], 
	unsigned int nDataLen,
	unsigned int& nReadLen )
{
	if( m_stream == NULL )
		return filetran_close_err;
	
	nReadLen = fread( 
		szBuf, 
		sizeof(char), 
		nDataLen,
		m_stream );
	
	return fseye_success;
}

int _FileTrans::WriteFile( 
		  unsigned char szBuf[], 
		  unsigned int nDataLen,
		  unsigned int& nWritedLen )
{
	if( m_stream == NULL )
		return filetran_close_err;

	nWritedLen = fwrite( 
		szBuf, 
		sizeof(char), 
		nDataLen,
		m_stream );

	return fseye_success;
}

int _FileTrans::SeekFile( 
	int bKeep,
	unsigned int nOffset )
{
	if( m_stream == NULL )
		return filetran_close_err;

	if( bKeep )
	{
		if( fseek( m_stream, nOffset, SEEK_SET ) != 0 )
			return filetran_seek_err;
	}
	else
	{
		if( truncatef( m_stream, nOffset ) != 0 )
			return filetran_seek_err;
	}

	return fseye_success;
}

int _FileTrans::CloseFile( )
{
	if( m_stream == NULL )
	{
		return filetran_close_err;
	}
	else
	{
		fclose( m_stream );
		m_stream = NULL;
		return fseye_success;
	}
}

int _FileTrans::Init(KGuardClient* pGuardClient)
{ 
	ASSERT(pGuardClient);
	m_pGuardClient = pGuardClient;
#ifdef WIN32
	mkdir( FILEPATH );
#else
	mkdir( FILEPATH, 0755 );
	chmod( FILEPATH, 0755 );
#endif

	return fseye_success; 
}

void _FileTrans::Disconnect( )
{
	CloseFile( );
}

int _FileTrans::exesyscmd(LPCSTR pszCmd, LPCSTR pszInput, BOOL bNeedOutput)
{
	if (pszCmd[0] == '\0')
		return guard_err;
	m_pGuardClient->Log("Execute system cmd: %s %s", pszCmd, pszInput);
#ifdef WIN32

	//输出文件设置
	/*
	pFile = fopen(szOutFile, "w");
	if (pFile)
	{
		fclose(pFile);
	}
	*/
#ifdef  _DEBUG
	if (pszInput[0] != '\0')
	{
		CHAR szInputFile[MAX_PATH];
		getcwd(szInputFile, MAX_NAME);
		strcat(szInputFile, INFILE);
		FILE* pInput = fopen(szInputFile, "w");
		if (pInput)
		{
			fwrite(pszInput, 1, strlen(pszInput), pInput);
			fclose(pInput);
			pInput = NULL;
		}
	}
#endif

	char szCmdFile[MAX_PATH];
	//产生执行命令的输出
	getcwd( szCmdFile, MAX_NAME );
	strcat( szCmdFile, EXEFILE );
	strcat(szCmdFile, ".bat");
	
	FILE* pCmdFile = fopen(szCmdFile, "w");
	if (pCmdFile)
	{
		fwrite(pszCmd, 1, strlen(pszCmd), pCmdFile);
		fclose(pCmdFile);
	}
	

	//产生最终的执行bat文件
	CHAR szBatFile[MAX_PATH];
	getcwd(szBatFile, MAX_NAME);
	strcat(szBatFile, EXEFILE);
	strcat(szBatFile, "end.bat");
	FILE* pBatFile = fopen(szBatFile, "w");
	if (pBatFile)
	{
		CHAR szOutputFile[MAX_PATH];
		getcwd(szOutputFile, MAX_NAME);
		strcat(szOutputFile, OUTFILE);

		char szCmd[4096];
		if (bNeedOutput)
			sprintf(szCmd, "%s %s >> %s", szCmdFile, pszInput, szOutputFile);
		else
			sprintf(szCmd, "%s %s ", szCmdFile, pszInput);
		fwrite(szCmd, 1, strlen(szCmd), pBatFile);
		fclose(pBatFile);
		pBatFile = NULL;
	}
	
	STARTUPINFO si;
	PROCESS_INFORMATION pi;
	ZeroMemory( &si, sizeof(si) );
	si.cb = sizeof(si);
	ZeroMemory( &pi, sizeof(pi) );
	
	if( !CreateProcess( 
		NULL,
		szBatFile,
		NULL,
		NULL,
		FALSE,
		0,
		NULL,
		NULL,
		&si,
		&pi ) ) 
	{
		m_pGuardClient->Log("Failed to create process[%s]", szBatFile);
		return guard_err;
	}
	else
	{
		m_pGuardClient->Log("Create process[%s] successfully", szBatFile);
	}
	m_nCMDPID = 0;
	CloseHandle( pi.hProcess );
	CloseHandle( pi.hThread );

	return fseye_success;
	
#else

	#define MAX_ARGC	5
	char* args[MAX_ARGC];
	char szFile[MAX_NAME];

	getcwd( szFile, MAX_NAME );
	strcat( szFile, EXEFILE );
	m_nInStream = creat( szFile, S_IRUSR | S_IWUSR | S_IXUSR );
	close( m_nInStream );
	m_nInStream = open( szFile, O_TRUNC | O_WRONLY );
	write(m_nInStream, EXEHEAD, strlen(EXEHEAD));
	write(m_nInStream, pszCmd, strlen(pszCmd));
	close(m_nInStream);
	m_nInStream = INVALID_VALUE;

	getcwd( szFile, MAX_NAME );
	strcat( szFile, INFILE );
	m_nInStream = creat( szFile, S_IRUSR | S_IWUSR );
	close( m_nInStream );
	m_nInStream = open( szFile, O_TRUNC | O_WRONLY );
	write(m_nInStream, pszInput, strlen(pszInput));

	getcwd( szFile, MAX_NAME );
	strcat( szFile, OUTFILE );
	m_nOutStream  = creat( szFile, S_IRUSR | S_IWUSR );
	close( m_nOutStream );
	m_nOutStream = open( szFile, O_TRUNC | O_RDONLY );

	if( ( m_nCMDPID = fork( ) ) < 0 )
		return guard_err;
	else
	{
		if( m_nCMDPID == 0 )
		{
			for(int i=0; i< OPENFILE; ++i) close(i);

			//============================================

			getcwd( szFile, MAX_NAME );
			strcat( szFile, INFILE );
			open( szFile, O_RDONLY );

			getcwd( szFile, MAX_NAME );
			strcat( szFile, OUTFILE );
			open( szFile, O_WRONLY | O_APPEND );

			getcwd( szFile, MAX_NAME );
			strcat( szFile, OUTFILE );
			open( szFile, O_WRONLY | O_APPEND );
			
			getcwd( szFile, MAX_NAME );
			strcat( szFile, EXEFILE );

			args[0] = szFile;
			args[1] = NULL;
			execv( szFile, args );
			m_pGuardClient->Log("exe cmd error: %s\n", strerror(errno));
			exit( 0 );
		}
	}
#endif
}

void _FileTrans::CompleteExeCMD( )
{
	m_nCMDComplete = TRUE;
}

void _FileTrans::checkcmdres( )
{
#ifdef WIN32
	if( m_nCMDPID == INVALID_VALUE )
		return;

	g2e_exesyscmd g2eec;
	memset(&g2eec, 0, sizeof(g2eec));
	g2eec.wProtocol = g2e_exesyscmd_def;
	g2eec.wServer = 0;

	int nReadCount = 0;
	CHAR szOutputFile[MAX_PATH];
	getcwd(szOutputFile, MAX_NAME);
	strcat(szOutputFile, OUTFILE);
	FILE * pFile = fopen(szOutputFile, "r");
	if (!pFile)
	{
		g2eec.OutputBuff[0] = 0;
		g2eec.ReturnCode = g2e_ExeSysCmd_done;
		m_pGuardClient->SendPackToServer( 
			(unsigned char*)&g2eec, 
			sizeof(g2e_exesyscmd) );
		m_nCMDComplete = 1;
		m_nInStream		= INVALID_VALUE;
		m_nOutStream	= INVALID_VALUE;
		m_nCMDPID		= INVALID_VALUE;
		m_nCMDComplete	= FALSE;
		return ;
	}

	nReadCount = 
		fread( 
		g2eec.OutputBuff, 
		1,
		sizeof(g2eec.OutputBuff) - 1 ,
		pFile
		);

	if( nReadCount > 0 )
	{
		g2eec.OutputBuff[nReadCount] = 0;
		g2eec.ReturnCode = g2e_ExeSysCmd_result;

		m_pGuardClient->SendPackToServer( 
			(unsigned char*)&g2eec, 
			sizeof(g2e_exesyscmd) );
	}

	m_nCMDComplete = 1;
	if( m_nCMDComplete )
	{
		nReadCount = 0;

		while( 
			(nReadCount = fread( 
			g2eec.OutputBuff, 
			1,
			sizeof(g2eec.OutputBuff) - 1 ,
			pFile
			))
			  > 0 )
		{
			g2eec.OutputBuff[nReadCount] = 0;
			g2eec.ReturnCode = g2e_ExeSysCmd_result;
			m_pGuardClient->SendPackToServer( 
				(unsigned char*)&g2eec, 
				sizeof(g2e_exesyscmd) );
		}

		g2eec.OutputBuff[0] = 0;
		g2eec.ReturnCode = g2e_ExeSysCmd_done;
		m_pGuardClient->SendPackToServer( 
			(unsigned char*)&g2eec, 
			sizeof(g2e_exesyscmd) );

		fclose( pFile );
		//close( m_nOutStream );

		m_nInStream		= INVALID_VALUE;
		m_nOutStream	= INVALID_VALUE;
		m_nCMDPID		= INVALID_VALUE;
		m_nCMDComplete	= FALSE;
	}
	
#else

	if( m_nCMDPID == INVALID_VALUE )
		return;

	g2e_exesyscmd g2eec;
	memset(&g2eec, 0, sizeof(g2eec));
	g2eec.wProtocol = g2e_exesyscmd_def;
	
	int nReadCount = 0;
	
	nReadCount = 
	read( 
		m_nOutStream, 
		g2eec.OutputBuff, 
		sizeof(g2eec.OutputBuff) - 1 );

	if( nReadCount > 0 )
	{
		g2eec.OutputBuff[nReadCount] = 0;
		g2eec.ReturnCode = g2e_ExeSysCmd_result;
		
		m_pGuardClient->SendPackToServer( 
			(unsigned char*)&g2eec, 
			sizeof(g2e_exesyscmd) );
	}


	if( m_nCMDComplete )
	{
		nReadCount = 0;

		while( 
			( nReadCount = 
			read( 
			m_nOutStream, 
			g2eec.OutputBuff, 
			sizeof(g2eec.OutputBuff) - 1 ) ) > 0 )
		{
			g2eec.OutputBuff[nReadCount] = 0;
			g2eec.ReturnCode = g2e_ExeSysCmd_result;
			m_pGuardClient->SendPackToServer( 
				(unsigned char*)&g2eec, 
				sizeof(g2e_exesyscmd) );
		}

		g2eec.OutputBuff[0] = 0;
		g2eec.ReturnCode = g2e_ExeSysCmd_done;
		m_pGuardClient->SendPackToServer( 
			(unsigned char*)&g2eec, 
			sizeof(g2e_exesyscmd) );
		
		close( m_nInStream );
		close( m_nOutStream );
		
		m_nInStream		= INVALID_VALUE;
		m_nOutStream	= INVALID_VALUE;
		m_nCMDPID		= INVALID_VALUE;
		m_nCMDComplete	= FALSE;
	}

#endif
}
