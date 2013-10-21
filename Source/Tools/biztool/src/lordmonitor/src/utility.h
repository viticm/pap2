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

#ifndef _UTILITY_H_
#define _UTILITY_H_
#include "def.h"

/////////////////////////////////////////////////////////////////////////////
//
//              Global variables and Macro and Structure Definitions
//
/////////////////////////////////////////////////////////////////////////////
#define B						( 1 )
#define KB						( 1024 )
#define MB						( 1024 * 1024 )

/*
 *	RB Buffer define
 */
#define RB_DEFHL	( 4	)
#define RB_SPACE	( 8 )
#define RB_DEFIBL	( KB * 16 + RB_SPACE )
#define RB_MAXPACK	( KB * 4  )

class IPCStream
{

public:
	IPCStream( );

	IPCStream( 
		int nBufOffset, 
		int nBufLen,
		int nMaxPack );

	~IPCStream( );


	int SndPack( const void* pData, unsigned int datasize );
	const void* RcvPack( unsigned int& datasize );

	int SndBuf( const void* pData, unsigned int datasize );
	const void* RcvBuf( unsigned int datasize, bool bMovePtr = true );
	int CopBuf( const void* pData, unsigned int datasize );

	int GetContentSize( unsigned int& datasize );
	int GetEmptySize( unsigned int& datasize );
	int GetUnreleaseSize( unsigned int& datasize );
	int GetPreSndSize( unsigned int& datasize );

	int MoveLeftPtr( unsigned int datasize );
	int MoveReleasePtr( );

	int MoveRightPtr( unsigned int datasize );
	int MovePreSndPtr( );
	int ResetPrePtr( );

	int GetLPtoRightMargin( unsigned int& datasize );
	int GetLPtoLeftMargin( unsigned int& datasize );
	
	int GetRPtoRightMargin( unsigned int& datasize );
	int GetRPtoLeftMargin( unsigned int& datasize );

	int GetPPtoRightMargin( unsigned int& datasize );

	int	Init(
		int nBufOffset = KB,
		int nBufLen = RB_DEFIBL,
		int nMaxPack = RB_MAXPACK );

	int Reinit( );

	int Uninit( );
	
private:

	bool Iswrapped( )
	{ return ( m_rightpos >= m_leftpos ? false : true ); }
	
	bool Isrelwrapped( )
	{ return ( m_leftpos >= m_releasepos ? false : true ); }

	bool Ispresndwrapped( )
	{ return ( m_presndpos >= m_rightpos ? false : true ); }

	void Clear( );

private:
	/*
	 *	ptr
	 */

	int m_leftpos;
	int m_rightpos;
	int m_releasepos;
	int m_presndpos;
	int m_tmppos;

	/*
	 *	var
	 */

	int		m_nOffset;
	int		m_nBufLen;
	int		m_nMaxPack;
	bool	m_bAutoAlloc;
};

/////////////////////////////////////////////////////////////////////////////

#endif