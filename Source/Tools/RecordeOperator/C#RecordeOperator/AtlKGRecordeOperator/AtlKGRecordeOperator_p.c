

/* this ALWAYS GENERATED file contains the proxy stub code */


 /* File created by MIDL compiler version 6.00.0366 */
/* at Tue May 26 11:50:38 2009
 */
/* Compiler settings for .\AtlKGRecordeOperator.idl:
    Oicf, W1, Zp8, env=Win32 (32b run)
    protocol : dce , ms_ext, c_ext
    error checks: allocation ref bounds_check enum stub_data 
    VC __declspec() decoration level: 
         __declspec(uuid()), __declspec(selectany), __declspec(novtable)
         DECLSPEC_UUID(), MIDL_INTERFACE()
*/
//@@MIDL_FILE_HEADING(  )

#if !defined(_M_IA64) && !defined(_M_AMD64)


#pragma warning( disable: 4049 )  /* more than 64k source lines */
#if _MSC_VER >= 1200
#pragma warning(push)
#endif
#pragma warning( disable: 4100 ) /* unreferenced arguments in x86 call */
#pragma warning( disable: 4211 )  /* redefine extent to static */
#pragma warning( disable: 4232 )  /* dllimport identity*/
#pragma optimize("", off ) 

#define USE_STUBLESS_PROXY


/* verify that the <rpcproxy.h> version is high enough to compile this file*/
#ifndef __REDQ_RPCPROXY_H_VERSION__
#define __REQUIRED_RPCPROXY_H_VERSION__ 440
#endif


#include "rpcproxy.h"
#ifndef __RPCPROXY_H_VERSION__
#error this stub requires an updated version of <rpcproxy.h>
#endif // __RPCPROXY_H_VERSION__


#include "AtlKGRecordeOperator.h"

#define TYPE_FORMAT_STRING_SIZE   63                                
#define PROC_FORMAT_STRING_SIZE   941                               
#define TRANSMIT_AS_TABLE_SIZE    0            
#define WIRE_MARSHAL_TABLE_SIZE   1            

typedef struct _MIDL_TYPE_FORMAT_STRING
    {
    short          Pad;
    unsigned char  Format[ TYPE_FORMAT_STRING_SIZE ];
    } MIDL_TYPE_FORMAT_STRING;

typedef struct _MIDL_PROC_FORMAT_STRING
    {
    short          Pad;
    unsigned char  Format[ PROC_FORMAT_STRING_SIZE ];
    } MIDL_PROC_FORMAT_STRING;


static RPC_SYNTAX_IDENTIFIER  _RpcTransferSyntax = 
{{0x8A885D04,0x1CEB,0x11C9,{0x9F,0xE8,0x08,0x00,0x2B,0x10,0x48,0x60}},{2,0}};


extern const MIDL_TYPE_FORMAT_STRING __MIDL_TypeFormatString;
extern const MIDL_PROC_FORMAT_STRING __MIDL_ProcFormatString;


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO IBaseLib_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO IBaseLib_ProxyInfo;


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO IRecordPlayer_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO IRecordPlayer_ProxyInfo;


extern const USER_MARSHAL_ROUTINE_QUADRUPLE UserMarshalRoutines[ WIRE_MARSHAL_TABLE_SIZE ];

#if !defined(__RPC_WIN32__)
#error  Invalid build platform for this stub.
#endif

#if !(TARGET_IS_NT40_OR_LATER)
#error You need a Windows NT 4.0 or later to run this stub because it uses these features:
#error   -Oif or -Oicf, [wire_marshal] or [user_marshal] attribute.
#error However, your C/C++ compilation flags indicate you intend to run this app on earlier systems.
#error This app will die there with the RPC_X_WRONG_STUB_VERSION error.
#endif


static const MIDL_PROC_FORMAT_STRING __MIDL_ProcFormatString =
    {
        0,
        {

	/* Procedure Init */

			0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/*  2 */	NdrFcLong( 0x0 ),	/* 0 */
/*  6 */	NdrFcShort( 0x3 ),	/* 3 */
/*  8 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 10 */	NdrFcShort( 0x8 ),	/* 8 */
/* 12 */	NdrFcShort( 0x8 ),	/* 8 */
/* 14 */	0x4,		/* Oi2 Flags:  has return, */
			0x2,		/* 2 */

	/* Parameter nhWnd */

/* 16 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 18 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 20 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 22 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 24 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 26 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure UnInit */

/* 28 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 30 */	NdrFcLong( 0x0 ),	/* 0 */
/* 34 */	NdrFcShort( 0x4 ),	/* 4 */
/* 36 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 38 */	NdrFcShort( 0x0 ),	/* 0 */
/* 40 */	NdrFcShort( 0x8 ),	/* 8 */
/* 42 */	0x4,		/* Oi2 Flags:  has return, */
			0x1,		/* 1 */

	/* Return value */

/* 44 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 46 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 48 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure RePlay */

/* 50 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 52 */	NdrFcLong( 0x0 ),	/* 0 */
/* 56 */	NdrFcShort( 0x3 ),	/* 3 */
/* 58 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 60 */	NdrFcShort( 0x0 ),	/* 0 */
/* 62 */	NdrFcShort( 0x24 ),	/* 36 */
/* 64 */	0x4,		/* Oi2 Flags:  has return, */
			0x2,		/* 2 */

	/* Parameter pRetResult */

/* 66 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 68 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 70 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 72 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 74 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 76 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure PauseRePlay */

/* 78 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 80 */	NdrFcLong( 0x0 ),	/* 0 */
/* 84 */	NdrFcShort( 0x4 ),	/* 4 */
/* 86 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 88 */	NdrFcShort( 0x0 ),	/* 0 */
/* 90 */	NdrFcShort( 0x24 ),	/* 36 */
/* 92 */	0x4,		/* Oi2 Flags:  has return, */
			0x2,		/* 2 */

	/* Parameter pRetResult */

/* 94 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 96 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 98 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 100 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 102 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 104 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure StopRePlay */

/* 106 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 108 */	NdrFcLong( 0x0 ),	/* 0 */
/* 112 */	NdrFcShort( 0x5 ),	/* 5 */
/* 114 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 116 */	NdrFcShort( 0x0 ),	/* 0 */
/* 118 */	NdrFcShort( 0x8 ),	/* 8 */
/* 120 */	0x4,		/* Oi2 Flags:  has return, */
			0x1,		/* 1 */

	/* Return value */

/* 122 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 124 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 126 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure LoadRecordFile */

/* 128 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 130 */	NdrFcLong( 0x0 ),	/* 0 */
/* 134 */	NdrFcShort( 0x6 ),	/* 6 */
/* 136 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 138 */	NdrFcShort( 0x0 ),	/* 0 */
/* 140 */	NdrFcShort( 0x8 ),	/* 8 */
/* 142 */	0x6,		/* Oi2 Flags:  clt must size, has return, */
			0x2,		/* 2 */

	/* Parameter pstrFileName */

/* 144 */	NdrFcShort( 0x8b ),	/* Flags:  must size, must free, in, by val, */
/* 146 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 148 */	NdrFcShort( 0x1e ),	/* Type Offset=30 */

	/* Return value */

/* 150 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 152 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 154 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure SetReplayWindow */

/* 156 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 158 */	NdrFcLong( 0x0 ),	/* 0 */
/* 162 */	NdrFcShort( 0x7 ),	/* 7 */
/* 164 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 166 */	NdrFcShort( 0x8 ),	/* 8 */
/* 168 */	NdrFcShort( 0x8 ),	/* 8 */
/* 170 */	0x4,		/* Oi2 Flags:  has return, */
			0x2,		/* 2 */

	/* Parameter uhWnd */

/* 172 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 174 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 176 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 178 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 180 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 182 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure FrameMoveRePlay */

/* 184 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 186 */	NdrFcLong( 0x0 ),	/* 0 */
/* 190 */	NdrFcShort( 0x8 ),	/* 8 */
/* 192 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 194 */	NdrFcShort( 0x0 ),	/* 0 */
/* 196 */	NdrFcShort( 0x8 ),	/* 8 */
/* 198 */	0x4,		/* Oi2 Flags:  has return, */
			0x1,		/* 1 */

	/* Return value */

/* 200 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 202 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 204 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure IsPlayFinished */

/* 206 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 208 */	NdrFcLong( 0x0 ),	/* 0 */
/* 212 */	NdrFcShort( 0x9 ),	/* 9 */
/* 214 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 216 */	NdrFcShort( 0x0 ),	/* 0 */
/* 218 */	NdrFcShort( 0x24 ),	/* 36 */
/* 220 */	0x4,		/* Oi2 Flags:  has return, */
			0x2,		/* 2 */

	/* Parameter pbFinished */

/* 222 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 224 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 226 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 228 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 230 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 232 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetLoadingProgress */

/* 234 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 236 */	NdrFcLong( 0x0 ),	/* 0 */
/* 240 */	NdrFcShort( 0xa ),	/* 10 */
/* 242 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 244 */	NdrFcShort( 0x0 ),	/* 0 */
/* 246 */	NdrFcShort( 0x5c ),	/* 92 */
/* 248 */	0x4,		/* Oi2 Flags:  has return, */
			0x4,		/* 4 */

	/* Parameter pnLoadFilePer */

/* 250 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 252 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 254 */	0xa,		/* FC_FLOAT */
			0x0,		/* 0 */

	/* Parameter pnStepToEventPer */

/* 256 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 258 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 260 */	0xa,		/* FC_FLOAT */
			0x0,		/* 0 */

	/* Parameter pnLoadScenePer */

/* 262 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 264 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 266 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 268 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 270 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 272 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure SaveRecordFile */

/* 274 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 276 */	NdrFcLong( 0x0 ),	/* 0 */
/* 280 */	NdrFcShort( 0xb ),	/* 11 */
/* 282 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 284 */	NdrFcShort( 0x0 ),	/* 0 */
/* 286 */	NdrFcShort( 0x8 ),	/* 8 */
/* 288 */	0x6,		/* Oi2 Flags:  clt must size, has return, */
			0x2,		/* 2 */

	/* Parameter pstrFileName */

/* 290 */	NdrFcShort( 0x8b ),	/* Flags:  must size, must free, in, by val, */
/* 292 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 294 */	NdrFcShort( 0x1e ),	/* Type Offset=30 */

	/* Return value */

/* 296 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 298 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 300 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure SetOption */

/* 302 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 304 */	NdrFcLong( 0x0 ),	/* 0 */
/* 308 */	NdrFcShort( 0xc ),	/* 12 */
/* 310 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 312 */	NdrFcShort( 0x10 ),	/* 16 */
/* 314 */	NdrFcShort( 0x8 ),	/* 8 */
/* 316 */	0x4,		/* Oi2 Flags:  has return, */
			0x3,		/* 3 */

	/* Parameter nWidth */

/* 318 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 320 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 322 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter nHeight */

/* 324 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 326 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 328 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 330 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 332 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 334 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure FreeCamera */

/* 336 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 338 */	NdrFcLong( 0x0 ),	/* 0 */
/* 342 */	NdrFcShort( 0xd ),	/* 13 */
/* 344 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 346 */	NdrFcShort( 0x0 ),	/* 0 */
/* 348 */	NdrFcShort( 0x8 ),	/* 8 */
/* 350 */	0x4,		/* Oi2 Flags:  has return, */
			0x1,		/* 1 */

	/* Return value */

/* 352 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 354 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 356 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure IsFreeCamera */

/* 358 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 360 */	NdrFcLong( 0x0 ),	/* 0 */
/* 364 */	NdrFcShort( 0xe ),	/* 14 */
/* 366 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 368 */	NdrFcShort( 0x0 ),	/* 0 */
/* 370 */	NdrFcShort( 0x24 ),	/* 36 */
/* 372 */	0x4,		/* Oi2 Flags:  has return, */
			0x2,		/* 2 */

	/* Parameter pnRetResult */

/* 374 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 376 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 378 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 380 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 382 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 384 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetCameraType */

/* 386 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 388 */	NdrFcLong( 0x0 ),	/* 0 */
/* 392 */	NdrFcShort( 0xf ),	/* 15 */
/* 394 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 396 */	NdrFcShort( 0x0 ),	/* 0 */
/* 398 */	NdrFcShort( 0x8 ),	/* 8 */
/* 400 */	0x4,		/* Oi2 Flags:  has return, */
			0x1,		/* 1 */

	/* Return value */

/* 402 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 404 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 406 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetObjSize */

/* 408 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 410 */	NdrFcLong( 0x0 ),	/* 0 */
/* 414 */	NdrFcShort( 0x10 ),	/* 16 */
/* 416 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 418 */	NdrFcShort( 0x0 ),	/* 0 */
/* 420 */	NdrFcShort( 0x24 ),	/* 36 */
/* 422 */	0x4,		/* Oi2 Flags:  has return, */
			0x2,		/* 2 */

	/* Parameter pnSize */

/* 424 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 426 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 428 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 430 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 432 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 434 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetFirstObj */

/* 436 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 438 */	NdrFcLong( 0x0 ),	/* 0 */
/* 442 */	NdrFcShort( 0x11 ),	/* 17 */
/* 444 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 446 */	NdrFcShort( 0x0 ),	/* 0 */
/* 448 */	NdrFcShort( 0x40 ),	/* 64 */
/* 450 */	0x5,		/* Oi2 Flags:  srv must size, has return, */
			0x4,		/* 4 */

	/* Parameter pnID */

/* 452 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 454 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 456 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter pstrRetName */

/* 458 */	NdrFcShort( 0x2113 ),	/* Flags:  must size, must free, out, simple ref, srv alloc size=8 */
/* 460 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 462 */	NdrFcShort( 0x34 ),	/* Type Offset=52 */

	/* Parameter pnType */

/* 464 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 466 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 468 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 470 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 472 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 474 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetNextObj */

/* 476 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 478 */	NdrFcLong( 0x0 ),	/* 0 */
/* 482 */	NdrFcShort( 0x12 ),	/* 18 */
/* 484 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 486 */	NdrFcShort( 0x0 ),	/* 0 */
/* 488 */	NdrFcShort( 0x40 ),	/* 64 */
/* 490 */	0x5,		/* Oi2 Flags:  srv must size, has return, */
			0x4,		/* 4 */

	/* Parameter pnID */

/* 492 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 494 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 496 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter pstrRetName */

/* 498 */	NdrFcShort( 0x2113 ),	/* Flags:  must size, must free, out, simple ref, srv alloc size=8 */
/* 500 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 502 */	NdrFcShort( 0x34 ),	/* Type Offset=52 */

	/* Parameter pnType */

/* 504 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 506 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 508 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 510 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 512 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 514 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure InitTree */

/* 516 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 518 */	NdrFcLong( 0x0 ),	/* 0 */
/* 522 */	NdrFcShort( 0x13 ),	/* 19 */
/* 524 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 526 */	NdrFcShort( 0x0 ),	/* 0 */
/* 528 */	NdrFcShort( 0x8 ),	/* 8 */
/* 530 */	0x4,		/* Oi2 Flags:  has return, */
			0x1,		/* 1 */

	/* Return value */

/* 532 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 534 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 536 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetFirstEvent */

/* 538 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 540 */	NdrFcLong( 0x0 ),	/* 0 */
/* 544 */	NdrFcShort( 0x14 ),	/* 20 */
/* 546 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 548 */	NdrFcShort( 0x0 ),	/* 0 */
/* 550 */	NdrFcShort( 0x24 ),	/* 36 */
/* 552 */	0x5,		/* Oi2 Flags:  srv must size, has return, */
			0x3,		/* 3 */

	/* Parameter pnFunction */

/* 554 */	NdrFcShort( 0x2113 ),	/* Flags:  must size, must free, out, simple ref, srv alloc size=8 */
/* 556 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 558 */	NdrFcShort( 0x34 ),	/* Type Offset=52 */

	/* Parameter pnInfo */

/* 560 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 562 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 564 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 566 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 568 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 570 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetNextEvent */

/* 572 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 574 */	NdrFcLong( 0x0 ),	/* 0 */
/* 578 */	NdrFcShort( 0x15 ),	/* 21 */
/* 580 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 582 */	NdrFcShort( 0x0 ),	/* 0 */
/* 584 */	NdrFcShort( 0x24 ),	/* 36 */
/* 586 */	0x5,		/* Oi2 Flags:  srv must size, has return, */
			0x3,		/* 3 */

	/* Parameter pnFunction */

/* 588 */	NdrFcShort( 0x2113 ),	/* Flags:  must size, must free, out, simple ref, srv alloc size=8 */
/* 590 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 592 */	NdrFcShort( 0x34 ),	/* Type Offset=52 */

	/* Parameter pnInfo */

/* 594 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 596 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 598 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 600 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 602 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 604 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetInformation */

/* 606 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 608 */	NdrFcLong( 0x0 ),	/* 0 */
/* 612 */	NdrFcShort( 0x16 ),	/* 22 */
/* 614 */	NdrFcShort( 0x18 ),	/* x86 Stack size/offset = 24 */
/* 616 */	NdrFcShort( 0x8 ),	/* 8 */
/* 618 */	NdrFcShort( 0x24 ),	/* 36 */
/* 620 */	0x5,		/* Oi2 Flags:  srv must size, has return, */
			0x5,		/* 5 */

	/* Parameter pnInfo */

/* 622 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 624 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 626 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter pstrLabel */

/* 628 */	NdrFcShort( 0x2113 ),	/* Flags:  must size, must free, out, simple ref, srv alloc size=8 */
/* 630 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 632 */	NdrFcShort( 0x34 ),	/* Type Offset=52 */

	/* Parameter pstrTextBox */

/* 634 */	NdrFcShort( 0x2113 ),	/* Flags:  must size, must free, out, simple ref, srv alloc size=8 */
/* 636 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 638 */	NdrFcShort( 0x34 ),	/* Type Offset=52 */

	/* Parameter pnNum */

/* 640 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 642 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 644 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 646 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 648 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 650 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure AddEvent */

/* 652 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 654 */	NdrFcLong( 0x0 ),	/* 0 */
/* 658 */	NdrFcShort( 0x17 ),	/* 23 */
/* 660 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 662 */	NdrFcShort( 0x8 ),	/* 8 */
/* 664 */	NdrFcShort( 0x8 ),	/* 8 */
/* 666 */	0x6,		/* Oi2 Flags:  clt must size, has return, */
			0x4,		/* 4 */

	/* Parameter nNum */

/* 668 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 670 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 672 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter strName */

/* 674 */	NdrFcShort( 0x8b ),	/* Flags:  must size, must free, in, by val, */
/* 676 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 678 */	NdrFcShort( 0x1e ),	/* Type Offset=30 */

	/* Parameter strValue */

/* 680 */	NdrFcShort( 0x8b ),	/* Flags:  must size, must free, in, by val, */
/* 682 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 684 */	NdrFcShort( 0x1e ),	/* Type Offset=30 */

	/* Return value */

/* 686 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 688 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 690 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure RemoveEvent */

/* 692 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 694 */	NdrFcLong( 0x0 ),	/* 0 */
/* 698 */	NdrFcShort( 0x18 ),	/* 24 */
/* 700 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 702 */	NdrFcShort( 0x8 ),	/* 8 */
/* 704 */	NdrFcShort( 0x8 ),	/* 8 */
/* 706 */	0x4,		/* Oi2 Flags:  has return, */
			0x2,		/* 2 */

	/* Parameter nInfo */

/* 708 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 710 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 712 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 714 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 716 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 718 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetNowTime */

/* 720 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 722 */	NdrFcLong( 0x0 ),	/* 0 */
/* 726 */	NdrFcShort( 0x19 ),	/* 25 */
/* 728 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 730 */	NdrFcShort( 0x0 ),	/* 0 */
/* 732 */	NdrFcShort( 0x24 ),	/* 36 */
/* 734 */	0x4,		/* Oi2 Flags:  has return, */
			0x2,		/* 2 */

	/* Parameter pnNowTime */

/* 736 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 738 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 740 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 742 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 744 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 746 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetTotalTime */

/* 748 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 750 */	NdrFcLong( 0x0 ),	/* 0 */
/* 754 */	NdrFcShort( 0x1a ),	/* 26 */
/* 756 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 758 */	NdrFcShort( 0x0 ),	/* 0 */
/* 760 */	NdrFcShort( 0x24 ),	/* 36 */
/* 762 */	0x4,		/* Oi2 Flags:  has return, */
			0x2,		/* 2 */

	/* Parameter pnTotalTime */

/* 764 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 766 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 768 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 770 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 772 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 774 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetPlayState */

/* 776 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 778 */	NdrFcLong( 0x0 ),	/* 0 */
/* 782 */	NdrFcShort( 0x1b ),	/* 27 */
/* 784 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 786 */	NdrFcShort( 0x0 ),	/* 0 */
/* 788 */	NdrFcShort( 0x24 ),	/* 36 */
/* 790 */	0x4,		/* Oi2 Flags:  has return, */
			0x2,		/* 2 */

	/* Parameter pPlayState */

/* 792 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 794 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 796 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 798 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 800 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 802 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetObjInfo */

/* 804 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 806 */	NdrFcLong( 0x0 ),	/* 0 */
/* 810 */	NdrFcShort( 0x1c ),	/* 28 */
/* 812 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 814 */	NdrFcShort( 0x8 ),	/* 8 */
/* 816 */	NdrFcShort( 0x24 ),	/* 36 */
/* 818 */	0x4,		/* Oi2 Flags:  has return, */
			0x3,		/* 3 */

	/* Parameter nID */

/* 820 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 822 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 824 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter pnSuccess */

/* 826 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 828 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 830 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 832 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 834 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 836 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetFirstEventTime */

/* 838 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 840 */	NdrFcLong( 0x0 ),	/* 0 */
/* 844 */	NdrFcShort( 0x1d ),	/* 29 */
/* 846 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 848 */	NdrFcShort( 0x0 ),	/* 0 */
/* 850 */	NdrFcShort( 0x40 ),	/* 64 */
/* 852 */	0x4,		/* Oi2 Flags:  has return, */
			0x3,		/* 3 */

	/* Parameter pnTime */

/* 854 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 856 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 858 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter pnInfo */

/* 860 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 862 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 864 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 866 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 868 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 870 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetNextEventTime */

/* 872 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 874 */	NdrFcLong( 0x0 ),	/* 0 */
/* 878 */	NdrFcShort( 0x1e ),	/* 30 */
/* 880 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 882 */	NdrFcShort( 0x0 ),	/* 0 */
/* 884 */	NdrFcShort( 0x40 ),	/* 64 */
/* 886 */	0x4,		/* Oi2 Flags:  has return, */
			0x3,		/* 3 */

	/* Parameter pnTime */

/* 888 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 890 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 892 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter pnInfo */

/* 894 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 896 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 898 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 900 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 902 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 904 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetEventFunction */

/* 906 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 908 */	NdrFcLong( 0x0 ),	/* 0 */
/* 912 */	NdrFcShort( 0x1f ),	/* 31 */
/* 914 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 916 */	NdrFcShort( 0x8 ),	/* 8 */
/* 918 */	NdrFcShort( 0x8 ),	/* 8 */
/* 920 */	0x5,		/* Oi2 Flags:  srv must size, has return, */
			0x3,		/* 3 */

	/* Parameter pnInfo */

/* 922 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 924 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 926 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter pstrFunction */

/* 928 */	NdrFcShort( 0x2113 ),	/* Flags:  must size, must free, out, simple ref, srv alloc size=8 */
/* 930 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 932 */	NdrFcShort( 0x34 ),	/* Type Offset=52 */

	/* Return value */

/* 934 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 936 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 938 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

			0x0
        }
    };

static const MIDL_TYPE_FORMAT_STRING __MIDL_TypeFormatString =
    {
        0,
        {
			NdrFcShort( 0x0 ),	/* 0 */
/*  2 */	
			0x11, 0xc,	/* FC_RP [alloced_on_stack] [simple_pointer] */
/*  4 */	0x8,		/* FC_LONG */
			0x5c,		/* FC_PAD */
/*  6 */	
			0x12, 0x0,	/* FC_UP */
/*  8 */	NdrFcShort( 0xc ),	/* Offset= 12 (20) */
/* 10 */	
			0x1b,		/* FC_CARRAY */
			0x1,		/* 1 */
/* 12 */	NdrFcShort( 0x2 ),	/* 2 */
/* 14 */	0x9,		/* Corr desc: FC_ULONG */
			0x0,		/*  */
/* 16 */	NdrFcShort( 0xfffc ),	/* -4 */
/* 18 */	0x6,		/* FC_SHORT */
			0x5b,		/* FC_END */
/* 20 */	
			0x17,		/* FC_CSTRUCT */
			0x3,		/* 3 */
/* 22 */	NdrFcShort( 0x8 ),	/* 8 */
/* 24 */	NdrFcShort( 0xfff2 ),	/* Offset= -14 (10) */
/* 26 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 28 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 30 */	0xb4,		/* FC_USER_MARSHAL */
			0x83,		/* 131 */
/* 32 */	NdrFcShort( 0x0 ),	/* 0 */
/* 34 */	NdrFcShort( 0x4 ),	/* 4 */
/* 36 */	NdrFcShort( 0x0 ),	/* 0 */
/* 38 */	NdrFcShort( 0xffe0 ),	/* Offset= -32 (6) */
/* 40 */	
			0x11, 0xc,	/* FC_RP [alloced_on_stack] [simple_pointer] */
/* 42 */	0xa,		/* FC_FLOAT */
			0x5c,		/* FC_PAD */
/* 44 */	
			0x11, 0x4,	/* FC_RP [alloced_on_stack] */
/* 46 */	NdrFcShort( 0x6 ),	/* Offset= 6 (52) */
/* 48 */	
			0x13, 0x0,	/* FC_OP */
/* 50 */	NdrFcShort( 0xffe2 ),	/* Offset= -30 (20) */
/* 52 */	0xb4,		/* FC_USER_MARSHAL */
			0x83,		/* 131 */
/* 54 */	NdrFcShort( 0x0 ),	/* 0 */
/* 56 */	NdrFcShort( 0x4 ),	/* 4 */
/* 58 */	NdrFcShort( 0x0 ),	/* 0 */
/* 60 */	NdrFcShort( 0xfff4 ),	/* Offset= -12 (48) */

			0x0
        }
    };

static const USER_MARSHAL_ROUTINE_QUADRUPLE UserMarshalRoutines[ WIRE_MARSHAL_TABLE_SIZE ] = 
        {
            
            {
            BSTR_UserSize
            ,BSTR_UserMarshal
            ,BSTR_UserUnmarshal
            ,BSTR_UserFree
            }

        };



/* Object interface: IUnknown, ver. 0.0,
   GUID={0x00000000,0x0000,0x0000,{0xC0,0x00,0x00,0x00,0x00,0x00,0x00,0x46}} */


/* Object interface: IBaseLib, ver. 0.0,
   GUID={0xDCA6998A,0x7600,0x481F,{0x90,0xBB,0x5E,0xCB,0x27,0xFA,0x2B,0x12}} */

#pragma code_seg(".orpc")
static const unsigned short IBaseLib_FormatStringOffsetTable[] =
    {
    0,
    28
    };

static const MIDL_STUBLESS_PROXY_INFO IBaseLib_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &IBaseLib_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IBaseLib_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &IBaseLib_FormatStringOffsetTable[-3],
    0,
    0,
    0,
    0};
CINTERFACE_PROXY_VTABLE(5) _IBaseLibProxyVtbl = 
{
    &IBaseLib_ProxyInfo,
    &IID_IBaseLib,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    (void *) (INT_PTR) -1 /* IBaseLib::Init */ ,
    (void *) (INT_PTR) -1 /* IBaseLib::UnInit */
};

const CInterfaceStubVtbl _IBaseLibStubVtbl =
{
    &IID_IBaseLib,
    &IBaseLib_ServerInfo,
    5,
    0, /* pure interpreted */
    CStdStubBuffer_METHODS
};


/* Object interface: IRecordPlayer, ver. 0.0,
   GUID={0x18A7E96F,0xA8C0,0x4335,{0x87,0xA5,0xF7,0x3A,0xF7,0xA8,0x88,0xF6}} */

#pragma code_seg(".orpc")
static const unsigned short IRecordPlayer_FormatStringOffsetTable[] =
    {
    50,
    78,
    106,
    128,
    156,
    184,
    206,
    234,
    274,
    302,
    336,
    358,
    386,
    408,
    436,
    476,
    516,
    538,
    572,
    606,
    652,
    692,
    720,
    748,
    776,
    804,
    838,
    872,
    906
    };

static const MIDL_STUBLESS_PROXY_INFO IRecordPlayer_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &IRecordPlayer_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IRecordPlayer_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &IRecordPlayer_FormatStringOffsetTable[-3],
    0,
    0,
    0,
    0};
CINTERFACE_PROXY_VTABLE(32) _IRecordPlayerProxyVtbl = 
{
    &IRecordPlayer_ProxyInfo,
    &IID_IRecordPlayer,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    (void *) (INT_PTR) -1 /* IRecordPlayer::RePlay */ ,
    (void *) (INT_PTR) -1 /* IRecordPlayer::PauseRePlay */ ,
    (void *) (INT_PTR) -1 /* IRecordPlayer::StopRePlay */ ,
    (void *) (INT_PTR) -1 /* IRecordPlayer::LoadRecordFile */ ,
    (void *) (INT_PTR) -1 /* IRecordPlayer::SetReplayWindow */ ,
    (void *) (INT_PTR) -1 /* IRecordPlayer::FrameMoveRePlay */ ,
    (void *) (INT_PTR) -1 /* IRecordPlayer::IsPlayFinished */ ,
    (void *) (INT_PTR) -1 /* IRecordPlayer::GetLoadingProgress */ ,
    (void *) (INT_PTR) -1 /* IRecordPlayer::SaveRecordFile */ ,
    (void *) (INT_PTR) -1 /* IRecordPlayer::SetOption */ ,
    (void *) (INT_PTR) -1 /* IRecordPlayer::FreeCamera */ ,
    (void *) (INT_PTR) -1 /* IRecordPlayer::IsFreeCamera */ ,
    (void *) (INT_PTR) -1 /* IRecordPlayer::GetCameraType */ ,
    (void *) (INT_PTR) -1 /* IRecordPlayer::GetObjSize */ ,
    (void *) (INT_PTR) -1 /* IRecordPlayer::GetFirstObj */ ,
    (void *) (INT_PTR) -1 /* IRecordPlayer::GetNextObj */ ,
    (void *) (INT_PTR) -1 /* IRecordPlayer::InitTree */ ,
    (void *) (INT_PTR) -1 /* IRecordPlayer::GetFirstEvent */ ,
    (void *) (INT_PTR) -1 /* IRecordPlayer::GetNextEvent */ ,
    (void *) (INT_PTR) -1 /* IRecordPlayer::GetInformation */ ,
    (void *) (INT_PTR) -1 /* IRecordPlayer::AddEvent */ ,
    (void *) (INT_PTR) -1 /* IRecordPlayer::RemoveEvent */ ,
    (void *) (INT_PTR) -1 /* IRecordPlayer::GetNowTime */ ,
    (void *) (INT_PTR) -1 /* IRecordPlayer::GetTotalTime */ ,
    (void *) (INT_PTR) -1 /* IRecordPlayer::GetPlayState */ ,
    (void *) (INT_PTR) -1 /* IRecordPlayer::GetObjInfo */ ,
    (void *) (INT_PTR) -1 /* IRecordPlayer::GetFirstEventTime */ ,
    (void *) (INT_PTR) -1 /* IRecordPlayer::GetNextEventTime */ ,
    (void *) (INT_PTR) -1 /* IRecordPlayer::GetEventFunction */
};

const CInterfaceStubVtbl _IRecordPlayerStubVtbl =
{
    &IID_IRecordPlayer,
    &IRecordPlayer_ServerInfo,
    32,
    0, /* pure interpreted */
    CStdStubBuffer_METHODS
};

static const MIDL_STUB_DESC Object_StubDesc = 
    {
    0,
    NdrOleAllocate,
    NdrOleFree,
    0,
    0,
    0,
    0,
    0,
    __MIDL_TypeFormatString.Format,
    1, /* -error bounds_check flag */
    0x20000, /* Ndr library version */
    0,
    0x600016e, /* MIDL Version 6.0.366 */
    0,
    UserMarshalRoutines,
    0,  /* notify & notify_flag routine table */
    0x1, /* MIDL flag */
    0, /* cs routines */
    0,   /* proxy/server info */
    0   /* Reserved5 */
    };

const CInterfaceProxyVtbl * _AtlKGRecordeOperator_ProxyVtblList[] = 
{
    ( CInterfaceProxyVtbl *) &_IRecordPlayerProxyVtbl,
    ( CInterfaceProxyVtbl *) &_IBaseLibProxyVtbl,
    0
};

const CInterfaceStubVtbl * _AtlKGRecordeOperator_StubVtblList[] = 
{
    ( CInterfaceStubVtbl *) &_IRecordPlayerStubVtbl,
    ( CInterfaceStubVtbl *) &_IBaseLibStubVtbl,
    0
};

PCInterfaceName const _AtlKGRecordeOperator_InterfaceNamesList[] = 
{
    "IRecordPlayer",
    "IBaseLib",
    0
};


#define _AtlKGRecordeOperator_CHECK_IID(n)	IID_GENERIC_CHECK_IID( _AtlKGRecordeOperator, pIID, n)

int __stdcall _AtlKGRecordeOperator_IID_Lookup( const IID * pIID, int * pIndex )
{
    IID_BS_LOOKUP_SETUP

    IID_BS_LOOKUP_INITIAL_TEST( _AtlKGRecordeOperator, 2, 1 )
    IID_BS_LOOKUP_RETURN_RESULT( _AtlKGRecordeOperator, 2, *pIndex )
    
}

const ExtendedProxyFileInfo AtlKGRecordeOperator_ProxyFileInfo = 
{
    (PCInterfaceProxyVtblList *) & _AtlKGRecordeOperator_ProxyVtblList,
    (PCInterfaceStubVtblList *) & _AtlKGRecordeOperator_StubVtblList,
    (const PCInterfaceName * ) & _AtlKGRecordeOperator_InterfaceNamesList,
    0, // no delegation
    & _AtlKGRecordeOperator_IID_Lookup, 
    2,
    2,
    0, /* table of [async_uuid] interfaces */
    0, /* Filler1 */
    0, /* Filler2 */
    0  /* Filler3 */
};
#pragma optimize("", on )
#if _MSC_VER >= 1200
#pragma warning(pop)
#endif


#endif /* !defined(_M_IA64) && !defined(_M_AMD64)*/

