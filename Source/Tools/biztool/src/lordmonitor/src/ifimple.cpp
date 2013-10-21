//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright (C) 2006
//
//      Created_datetime : 2007-03-12 17:00
//      File_base        : mydbop.cpp
//      File_ext         : .cpp
//      Author           : zolazuo(zuolizhi)
//      Description      : 
//
//////////////////////////////////////////////////////////////////////

#include "ifimple.h"
/////////////////////////////////////////////////////////////////////////////
//
//              Global variables and Macro and Structure Definitions
//
/////////////////////////////////////////////////////////////////////////////

monitor::monitor( )
{
	m_pSender	= NULL;
	m_pDBuff	= NULL;
}

monitor::~monitor( )
{
}

int monitor::LoadPlug( ISender* pSender )
{
	m_pSender = pSender;

	if( !m_MesPump.Init( ) )
		return guard_err;

	return fseye_success;
}

int monitor::Release( )
{
	return fseye_success;
}

int monitor::ProcessNetMessage(
		  unsigned char* pData,
		  unsigned int datasize )
{
	int nRet = guard_err;

	switch( PH( pData ) ) 
	{
	case e2l_header_def:
		{
			m_MesPump.SendPack( 
				pData, 
				datasize );

			break;
		}
	default:
		break;
	}
	
	return nRet;
}

int monitor::Disconnect( )
{
	return fseye_success;
}

int monitor::GetDescribe( 
			char* szDes,
			int nSize )
{
	if (szDes)
		strcpy( szDes, "Monitor Lord etc..\n" );
	return fseye_success;
}

int monitor::GetGUID( 
			char* szGUID,
			int nSize )
{
	if (szGUID)
		strcpy( szGUID, "{96F8F5AD-9435-4cff-952A-149EFE7F13D6}" );
	return fseye_success;
}

int monitor::Breathe( )
{
	const void* pData = NULL;
	unsigned int datasize = 0;

	pData = m_MesPump.RcvPack( datasize );

	if( pData != NULL && datasize != 0 )
	{
		m_pSender->SendPackToServer( (unsigned char *)pData, datasize );
	}

	return fseye_success;
}

int monitor::GetVersion( 
	int& nMaxVer, 
	int& nMinVer )
{
	nMaxVer = EXESQLMAXVER;
	nMinVer = EXESQLMINVER;
	
	return fseye_success;
}

int monitor::GetAuthor(
	char* szAuthor,
	int nSize  )
{
	if (szAuthor)
		strcpy( szAuthor, "zuolizhi(zola)" );
	return fseye_success;
}
