//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright (C) 2006
//
//      Created_datetime : 2006-08-03 11:00
//      File_base        : fsencrypt
//      File_ext         : .h
//      Author           : zolazuo(zuolizhi)
//      Description      : 
//
//////////////////////////////////////////////////////////////////////

#ifndef _FS_ENCRYPT_H_
#define _FS_ENCRYPT_H_

/////////////////////////////////////////////////////////////////////////////
//
//              Global variables and Macro and Structure Definitions
//
/////////////////////////////////////////////////////////////////////////////
#ifdef WIN32
	#ifdef FSEYEUTIL_EXPORTS
		#define ENCRYPT_API	extern "C"
	#else
		#define ENCRYPT_API	extern "C" __declspec(dllimport)
	#endif
#else
#define ENCRYPT_API
#endif


ENCRYPT_API 
int RC4Encrypt( 
	unsigned char* InData, 
	int nSize, 
	unsigned char KeyData[8] );

ENCRYPT_API
int RSAEncrypt( 
	unsigned char InData[128], 
	int nSize, 
	int* pRet );

ENCRYPT_API
int RSADencrypt( 
	unsigned char InData[128], 
	int nSize, 
	int* pRet );

ENCRYPT_API
int MD5(
	char* szInData,
	char* szOutData);

ENCRYPT_API
int DBEncrypt(
	char* szInData,
	char* szOutData);

ENCRYPT_API
int DBDecrypt(
	char* szInData,
	char* szOutData);

/////////////////////////////////////////////////////////////////////////////
//
//              Function Definitions
//
/////////////////////////////////////////////////////////////////////////////



#endif
