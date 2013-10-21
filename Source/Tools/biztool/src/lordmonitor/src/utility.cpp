//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright (C) 2006
//
//      Created_datetime : 2007-03-12 17:00
//      File_base        : sysinfo.cpp
//      File_ext         : .cpp
//      Author           : zolazuo(zuolizhi)
//      Description      : 
//
//////////////////////////////////////////////////////////////////////

#include "utility.h"
/////////////////////////////////////////////////////////////////////////////
//
//              Global variables and Macro and Structure Definitions
//
/////////////////////////////////////////////////////////////////////////////


IPCStream::IPCStream( )
{
	Clear( );
}

IPCStream::IPCStream( 
		int nBufOffset,
		int nBufLen,
		int nMaxPack )
{
	this->Init( nBufOffset, nBufLen, nMaxPack );	
}

IPCStream::~IPCStream( )
{
}

void IPCStream::Clear( )
{
	m_nOffset		=	0;
	m_nMaxPack		=	0;
	m_nBufLen		=	0;
	m_leftpos		=	0;
	m_rightpos		=	0;
	m_releasepos	=	0;
	m_presndpos		=	0;
	m_tmppos		=	0;
	m_tmppos		=	0;
	
}

int IPCStream::Init(
		int nBufOffset, 
		int nBufLen,
		int nMaxPack )
{
	if( nBufLen < nMaxPack )
		return INVALID_VALUE;
	
	m_nOffset = nBufOffset;

	m_nMaxPack		=	nMaxPack;
	m_nBufLen		=	nBufLen - ( m_nMaxPack + RB_SPACE );

	m_leftpos		=	0;
	m_rightpos		=	0;
	m_releasepos	=	0;
	m_presndpos		=	0;
	m_tmppos		=	0;

	m_tmppos		=	m_nBufLen + RB_SPACE;

	return SUCCESS_VALUE;
}

int IPCStream::Reinit( )
{
	m_leftpos		=	0;
	m_rightpos		=	0;
	m_releasepos	=	0;
	m_presndpos		=	0;
	m_tmppos		=	0;
	
	m_tmppos		=	m_nBufLen + RB_SPACE;	

	return SUCCESS_VALUE;
}

int IPCStream::Uninit( )
{
	return Reinit( );
}

int IPCStream::SndPack( const void* pData, unsigned int datasize )
{
	if( !m_nBufLen )
		return INVALID_VALUE;

	unsigned int nLen;
	GetEmptySize( nLen );

	if( datasize + RB_DEFHL > nLen ||
		datasize == 0 )
		return INVALID_VALUE;
	
	ResetPrePtr( );
	CopBuf( &datasize, RB_DEFHL );
	CopBuf( pData, datasize );
	MovePreSndPtr( );

	return SUCCESS_VALUE;
}

const void* IPCStream::RcvPack( unsigned int& datasize )
{
	datasize = 0;
	if( !m_nBufLen )
		return NULL;

	const void* pHead = RcvBuf( RB_DEFHL, false );

	if( pHead != NULL )
	{
		unsigned int nLen;
		GetContentSize( nLen );
		datasize = *(reinterpret_cast<const unsigned int *>(pHead));
		if( datasize > nLen )
		{
			datasize = 0;
			return NULL;
		}
		else
		{
			RcvBuf( RB_DEFHL );
			return RcvBuf( datasize );
		}
	}
	else
	{
		datasize = 0;
		return NULL;
	}
}

int IPCStream::SndBuf( const void* pData, unsigned int datasize )
{
	unsigned int nLen;

	GetEmptySize( nLen );

	if( datasize > nLen || 
		datasize == 0 )
		return INVALID_VALUE;

	char*	szInternalBuf = ((char*)this) + m_nOffset;	

	if( Iswrapped( ) )
	{
		memcpy( szInternalBuf + m_rightpos, pData, datasize );
		m_rightpos += datasize;
	}
	else
	{
		GetRPtoRightMargin( nLen );
		
		if( datasize <= nLen )
		{
			memcpy( szInternalBuf + m_rightpos, pData, datasize );
			m_rightpos += datasize;
		}
		else
		{
			char* p = (char*)pData;
			memcpy( szInternalBuf + m_rightpos, pData, nLen );
			memcpy( szInternalBuf, p + nLen, ( datasize - nLen ) );
			m_rightpos = datasize - nLen;
		}
	}

	return SUCCESS_VALUE;
}

int IPCStream::CopBuf( const void* pData, unsigned int datasize )
{
	unsigned int nLen;
	unsigned int nPreLen;
	
	GetEmptySize( nLen );
	GetPreSndSize( nPreLen );
	nLen -= nPreLen;
	
	if( datasize > nLen || 
		datasize == 0 )
		return INVALID_VALUE;
	
	char*	szInternalBuf = ((char*)this) + m_nOffset;

	if( Ispresndwrapped( ) )
	{
		memcpy( szInternalBuf + m_presndpos, pData, datasize );
		m_presndpos += datasize;
	}
	else
	{
		GetPPtoRightMargin( nLen );
		
		if( datasize <= nLen )
		{
			memcpy( szInternalBuf + m_presndpos, pData, datasize );
			m_presndpos += datasize;
		}
		else
		{
			char* p = (char*)pData;
			memcpy( szInternalBuf + m_presndpos, pData, nLen );
			memcpy( szInternalBuf, p + nLen, ( datasize - nLen ) );
			m_presndpos = datasize - nLen;
		}
	}
	
	return SUCCESS_VALUE;
}

const void* IPCStream::RcvBuf( unsigned int datasize, bool bMovePtr )
{
	unsigned int nLen;
	
	GetContentSize( nLen );
	
	if( datasize > nLen ||
		nLen == 0 )
		return NULL;
	
	MoveReleasePtr( );
	
	char*	szInternalBuf = ((char*)this) + m_nOffset;

	if( Iswrapped( ) )
	{
		GetLPtoRightMargin( nLen );
		
		if( datasize <= nLen )
		{
			if( bMovePtr )
				m_leftpos += datasize;
			return szInternalBuf+m_releasepos;
		}
		else
		{
			memcpy( szInternalBuf + m_tmppos, szInternalBuf + m_leftpos, nLen );
			memcpy( szInternalBuf + m_tmppos + nLen, szInternalBuf, ( datasize - nLen ) );
			if( bMovePtr )
				m_leftpos = datasize - nLen;
			return szInternalBuf + m_tmppos;
		}
	}
	else
	{
		if( bMovePtr )
			m_leftpos += datasize;
		return szInternalBuf + m_releasepos;
	}
	
	return NULL;
}

int IPCStream::GetContentSize( unsigned int& datasize )
{
	if( Iswrapped( ) )
		datasize = m_nBufLen - ( m_leftpos - m_rightpos );
	else
		datasize = ( m_rightpos - m_leftpos );

	return SUCCESS_VALUE;
}

int IPCStream::GetEmptySize( unsigned int& datasize )
{
	unsigned int nLen;
	GetUnreleaseSize( nLen );
	GetContentSize( datasize );
	datasize = m_nBufLen - datasize;
	datasize -= nLen;

	//keep space
	datasize -= 1;

	return SUCCESS_VALUE;
}

int IPCStream::GetUnreleaseSize( unsigned int& datasize )
{
	if( Isrelwrapped( ) )
		datasize = m_nBufLen - ( m_releasepos - m_leftpos );
	else
		datasize = ( m_leftpos - m_releasepos );
	
	return SUCCESS_VALUE;
}

int IPCStream::GetPreSndSize( unsigned int& datasize )
{
	if( Ispresndwrapped( ) )
		datasize = m_nBufLen - ( m_rightpos - m_presndpos );
	else
		datasize = ( m_presndpos - m_rightpos );
	
	return SUCCESS_VALUE;
}

int IPCStream::MoveLeftPtr( unsigned int datasize )
{
	unsigned int nLen;

	GetContentSize( nLen );

	if( datasize > nLen )
		return INVALID_VALUE;

	MoveReleasePtr( );
	
	if( Iswrapped( ) )
	{
		GetLPtoRightMargin( nLen );

		if( datasize <= nLen )
			m_leftpos += datasize;
		else
			m_leftpos = datasize - nLen;
	}
	else
	{
		m_leftpos += datasize;
	}
	
	return SUCCESS_VALUE;
}

int IPCStream::MoveRightPtr( unsigned int datasize )
{
	unsigned int nLen;
	
	GetEmptySize( nLen );
	
	if( datasize > nLen )
		return INVALID_VALUE;
	
	if( Iswrapped( ) )
	{
		m_rightpos += datasize;
	}
	else
	{
		GetRPtoRightMargin( nLen );

		if( datasize <= nLen )
			m_rightpos += datasize;
		else
			m_rightpos = datasize - nLen;
	}
	
	return SUCCESS_VALUE;
}

int IPCStream::MoveReleasePtr(  )
{
	m_releasepos = m_leftpos;

	return SUCCESS_VALUE;
}

int IPCStream::MovePreSndPtr( )
{
	m_rightpos = m_presndpos;

	return SUCCESS_VALUE;
}

int IPCStream::ResetPrePtr( )
{
	m_presndpos = m_rightpos;
	
	return SUCCESS_VALUE;
}

int IPCStream::GetLPtoRightMargin( unsigned int& datasize )
{
	datasize = m_nBufLen - m_leftpos;

	return SUCCESS_VALUE;
}

int IPCStream::GetLPtoLeftMargin( unsigned int& datasize )
{
	datasize = m_leftpos;
	return SUCCESS_VALUE;
}

int IPCStream::GetRPtoRightMargin( unsigned int& datasize )
{
	datasize = m_nBufLen - m_rightpos;
	
	return SUCCESS_VALUE;
}

int IPCStream::GetRPtoLeftMargin( unsigned int& datasize )
{
	datasize = m_rightpos;
	return SUCCESS_VALUE;
}

int IPCStream::GetPPtoRightMargin( unsigned int& datasize )
{
	datasize = m_nBufLen - m_presndpos;
	
	return SUCCESS_VALUE;
}

/////////////////////////////////////////////////////////////////////////////