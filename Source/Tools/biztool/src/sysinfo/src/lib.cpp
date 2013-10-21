//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright (C) 2006
//
//      Created_datetime : 2006-08-16 17:00
//      File_base        : lib
//      File_ext         : .cpp
//      Author           : zolazuo(zuolizhi)
//      Description      : 
//
//////////////////////////////////////////////////////////////////////

#include "lib.h"
/////////////////////////////////////////////////////////////////////////////
//
//              Global variables and Macro and Structure Definitions
//
/////////////////////////////////////////////////////////////////////////////

char* ftostr( char* name )
{
	static char szFileBuf[MAX_TEMPBUF*10];
	FILE* file = fopen( name, "r" );
	
	if( file )
	{
		long len = 0;
		int nreaded = 0;
		int ntotalrd = 0;

		while( !feof( file ) )
		{
		  nreaded = fread( szFileBuf + ntotalrd, 1, 100, file );
		  ntotalrd += nreaded;
		}

		szFileBuf[ntotalrd] = 0;

		fclose( file );
		return szFileBuf;

	}
	else
		return 0;
}
