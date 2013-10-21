////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : IPPBase.h
//  Version     : 1.0
//  Creator     : dengchao
//  Create Date : 2006-08-08 14:57:32
//  Comment     : ipp descr
//
////////////////////////////////////////////////////////////////////////////////


#ifndef _INCLUDE_IPPDESCR_H_
#define _INCLUDE_IPPDESCR_H_


#include "KGStdAfx.h"

#include "Engine.h"
#include "Engine/KGLog.h"

#pragma warning(push)
#pragma warning(disable : 4819)
#include  <d3d9.h>
#include  <d3dx9.h>
#pragma warning(pop)


// macro
#ifndef SAFE_RELEASE
	#define SAFE_RELEASE( POINTER )			if ( NULL != POINTER ) { POINTER->Release(); POINTER = NULL; }
#endif

#ifndef SAFE_DELETE
	#define SAFE_DELETE( POINTER )			if ( NULL != POINTER ) { delete POINTER; POINTER = NULL; }
#endif

#ifndef SAFE_DELETE_ARRAY
	#define SAFE_DELETE_ARRAY( POINTER )	if ( NULL != POINTER ) { delete[] POINTER; POINTER = NULL; }
#endif




// MPEG2 = VCD,  标准为352x288
// MPEG4 = DVD,  标准为720x576

/*
640x480x24fps video clips common with DVD movies, 
720x480x30fps for   DVD standard video
1280x720x60fps for  HDTV progressive mode(often used in sports videos)
1920x1080x30fps for HDTV interlaced clips
*/


enum
{
	IPPSPEC_H261,
	IPPSPEC_H263,
	IPPSPEC_H264,

	IPPSPEC_MPEG2,
	IPPSPEC_MPEG4,

	IPPSPEC_COUNT,
	IPPSPEC_DEFAULT = IPPSPEC_MPEG4,

};


enum
{
	IPPSPEC_FPS_25		=	25,
	IPPSPEC_FPS_30		=	30,
	IPPSPEC_FPS_50		=	50,
	IPPSPEC_FPS_60		=	60,

	IPPSPEC_FPS_COUNT	=	IPPSPEC_FPS_60,
	IPPSPEC_FPS_DEFAULT =	IPPSPEC_FPS_25,

};


enum 
{
	IPPSPEC_FILTER_NOT	  = 1,
	IPPSPEC_FILTER_CUBIC  = 2,
	IPPSPEC_FILTER_LINEAR = 3,
};


enum 
{
	IPPSPEC_QUALITY_PIRATE		= 1500000,				// 偷拍
	IPPSPEC_QUALITY_DISC		= 3500000,
	IPPSPEC_QUALITY_CINEMATIC	= 5500000,
	IPPSPEC_QUALITY_CINEMATIC1	= IPPSPEC_QUALITY_CINEMATIC * 2,
	IPPSPEC_QUALITY_CINEMATIC2	= IPPSPEC_QUALITY_CINEMATIC1 * 2,
	IPPSPEC_QUALITY_CINEMATIC3	= IPPSPEC_QUALITY_CINEMATIC2 * 2,
	IPPSPEC_QUALITY_CINEMATIC4	= IPPSPEC_QUALITY_CINEMATIC3 * 2,
	IPPSPEC_QUALITY_MAX			= 5500000 * 16,
};

interface IKG3DSoundShell;
struct IPPSpec_Params_t
{
	TCHAR   FileAvis[MAX_PATH];
	TCHAR   Paths[MAX_PATH];

	UINT	CapsSpec;
	UINT	CapsTimes;

	UINT	CapsFps;		// FPS:  IPPSPEC_FPS_DEFAULT
	UINT	CapsDSize;		// SIZE: standard size * 2 ?

	UINT	ClientWidth;	//client size
	UINT	ClientHeight;	//client height

	UINT	CapsFilter;		// FILTER:  IPPSPEC_FILTER_LINEAR
	UINT	CapsQuality;	// QUALITY: IPPSPEC_QUALITY_CINEMATIC

	IKG3DSoundShell	*pSoundShell;
};




#endif //_INCLUDE_IPPDESCR_H_