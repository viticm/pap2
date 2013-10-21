

/* this ALWAYS GENERATED file contains the proxy stub code */


 /* File created by MIDL compiler version 6.00.0366 */
/* at Thu Sep 24 17:57:35 2009
 */
/* Compiler settings for .\ThumbExtract.idl:
    Oicf, W1, Zp8, env=Win32 (32b run)
    protocol : dce , ms_ext, c_ext, robust
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
#define __REQUIRED_RPCPROXY_H_VERSION__ 475
#endif


#include "rpcproxy.h"
#ifndef __RPCPROXY_H_VERSION__
#error this stub requires an updated version of <rpcproxy.h>
#endif // __RPCPROXY_H_VERSION__


#include "ThumbExtract.h"

#define TYPE_FORMAT_STRING_SIZE   1071                              
#define PROC_FORMAT_STRING_SIZE   475                               
#define TRANSMIT_AS_TABLE_SIZE    0            
#define WIRE_MARSHAL_TABLE_SIZE   2            

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


extern const MIDL_SERVER_INFO IFileThumbExtract_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO IFileThumbExtract_ProxyInfo;


extern const USER_MARSHAL_ROUTINE_QUADRUPLE UserMarshalRoutines[ WIRE_MARSHAL_TABLE_SIZE ];

#if !defined(__RPC_WIN32__)
#error  Invalid build platform for this stub.
#endif

#if !(TARGET_IS_NT50_OR_LATER)
#error You need a Windows 2000 or later to run this stub because it uses these features:
#error   /robust command line switch.
#error However, your C/C++ compilation flags indicate you intend to run this app on earlier systems.
#error This app will die there with the RPC_X_WRONG_STUB_VERSION error.
#endif


static const MIDL_PROC_FORMAT_STRING __MIDL_ProcFormatString =
    {
        0,
        {

	/* Procedure SetPath */

			0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/*  2 */	NdrFcLong( 0x0 ),	/* 0 */
/*  6 */	NdrFcShort( 0x7 ),	/* 7 */
/*  8 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 10 */	NdrFcShort( 0x0 ),	/* 0 */
/* 12 */	NdrFcShort( 0x8 ),	/* 8 */
/* 14 */	0x46,		/* Oi2 Flags:  clt must size, has return, has ext, */
			0x2,		/* 2 */
/* 16 */	0x8,		/* 8 */
			0x5,		/* Ext Flags:  new corr desc, srv corr check, */
/* 18 */	NdrFcShort( 0x0 ),	/* 0 */
/* 20 */	NdrFcShort( 0x1 ),	/* 1 */
/* 22 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter Path */

/* 24 */	NdrFcShort( 0x8b ),	/* Flags:  must size, must free, in, by val, */
/* 26 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 28 */	NdrFcShort( 0x1c ),	/* Type Offset=28 */

	/* Return value */

/* 30 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 32 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 34 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure SetDir */

/* 36 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 38 */	NdrFcLong( 0x0 ),	/* 0 */
/* 42 */	NdrFcShort( 0x8 ),	/* 8 */
/* 44 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 46 */	NdrFcShort( 0x0 ),	/* 0 */
/* 48 */	NdrFcShort( 0x8 ),	/* 8 */
/* 50 */	0x46,		/* Oi2 Flags:  clt must size, has return, has ext, */
			0x2,		/* 2 */
/* 52 */	0x8,		/* 8 */
			0x5,		/* Ext Flags:  new corr desc, srv corr check, */
/* 54 */	NdrFcShort( 0x0 ),	/* 0 */
/* 56 */	NdrFcShort( 0x1 ),	/* 1 */
/* 58 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter Dir */

/* 60 */	NdrFcShort( 0x8b ),	/* Flags:  must size, must free, in, by val, */
/* 62 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 64 */	NdrFcShort( 0x1c ),	/* Type Offset=28 */

	/* Return value */

/* 66 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 68 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 70 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure SetFile */

/* 72 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 74 */	NdrFcLong( 0x0 ),	/* 0 */
/* 78 */	NdrFcShort( 0x9 ),	/* 9 */
/* 80 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 82 */	NdrFcShort( 0x0 ),	/* 0 */
/* 84 */	NdrFcShort( 0x8 ),	/* 8 */
/* 86 */	0x46,		/* Oi2 Flags:  clt must size, has return, has ext, */
			0x2,		/* 2 */
/* 88 */	0x8,		/* 8 */
			0x5,		/* Ext Flags:  new corr desc, srv corr check, */
/* 90 */	NdrFcShort( 0x0 ),	/* 0 */
/* 92 */	NdrFcShort( 0x1 ),	/* 1 */
/* 94 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter File */

/* 96 */	NdrFcShort( 0x8b ),	/* Flags:  must size, must free, in, by val, */
/* 98 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 100 */	NdrFcShort( 0x1c ),	/* Type Offset=28 */

	/* Return value */

/* 102 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 104 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 106 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure SetThumbnailSize */

/* 108 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 110 */	NdrFcLong( 0x0 ),	/* 0 */
/* 114 */	NdrFcShort( 0xa ),	/* 10 */
/* 116 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 118 */	NdrFcShort( 0x10 ),	/* 16 */
/* 120 */	NdrFcShort( 0x8 ),	/* 8 */
/* 122 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x3,		/* 3 */
/* 124 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 126 */	NdrFcShort( 0x0 ),	/* 0 */
/* 128 */	NdrFcShort( 0x0 ),	/* 0 */
/* 130 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter Width */

/* 132 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 134 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 136 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter Height */

/* 138 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 140 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 142 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 144 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 146 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 148 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetPath */

/* 150 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 152 */	NdrFcLong( 0x0 ),	/* 0 */
/* 156 */	NdrFcShort( 0xb ),	/* 11 */
/* 158 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 160 */	NdrFcShort( 0x0 ),	/* 0 */
/* 162 */	NdrFcShort( 0x8 ),	/* 8 */
/* 164 */	0x45,		/* Oi2 Flags:  srv must size, has return, has ext, */
			0x2,		/* 2 */
/* 166 */	0x8,		/* 8 */
			0x3,		/* Ext Flags:  new corr desc, clt corr check, */
/* 168 */	NdrFcShort( 0x1 ),	/* 1 */
/* 170 */	NdrFcShort( 0x0 ),	/* 0 */
/* 172 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter Path */

/* 174 */	NdrFcShort( 0x2113 ),	/* Flags:  must size, must free, out, simple ref, srv alloc size=8 */
/* 176 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 178 */	NdrFcShort( 0x2e ),	/* Type Offset=46 */

	/* Return value */

/* 180 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 182 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 184 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetDir */

/* 186 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 188 */	NdrFcLong( 0x0 ),	/* 0 */
/* 192 */	NdrFcShort( 0xc ),	/* 12 */
/* 194 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 196 */	NdrFcShort( 0x0 ),	/* 0 */
/* 198 */	NdrFcShort( 0x8 ),	/* 8 */
/* 200 */	0x45,		/* Oi2 Flags:  srv must size, has return, has ext, */
			0x2,		/* 2 */
/* 202 */	0x8,		/* 8 */
			0x3,		/* Ext Flags:  new corr desc, clt corr check, */
/* 204 */	NdrFcShort( 0x1 ),	/* 1 */
/* 206 */	NdrFcShort( 0x0 ),	/* 0 */
/* 208 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter Dir */

/* 210 */	NdrFcShort( 0x2113 ),	/* Flags:  must size, must free, out, simple ref, srv alloc size=8 */
/* 212 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 214 */	NdrFcShort( 0x2e ),	/* Type Offset=46 */

	/* Return value */

/* 216 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 218 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 220 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetFile */

/* 222 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 224 */	NdrFcLong( 0x0 ),	/* 0 */
/* 228 */	NdrFcShort( 0xd ),	/* 13 */
/* 230 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 232 */	NdrFcShort( 0x0 ),	/* 0 */
/* 234 */	NdrFcShort( 0x8 ),	/* 8 */
/* 236 */	0x45,		/* Oi2 Flags:  srv must size, has return, has ext, */
			0x2,		/* 2 */
/* 238 */	0x8,		/* 8 */
			0x3,		/* Ext Flags:  new corr desc, clt corr check, */
/* 240 */	NdrFcShort( 0x1 ),	/* 1 */
/* 242 */	NdrFcShort( 0x0 ),	/* 0 */
/* 244 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter File */

/* 246 */	NdrFcShort( 0x2113 ),	/* Flags:  must size, must free, out, simple ref, srv alloc size=8 */
/* 248 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 250 */	NdrFcShort( 0x2e ),	/* Type Offset=46 */

	/* Return value */

/* 252 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 254 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 256 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure ExtractThumbnail */

/* 258 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 260 */	NdrFcLong( 0x0 ),	/* 0 */
/* 264 */	NdrFcShort( 0xe ),	/* 14 */
/* 266 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 268 */	NdrFcShort( 0x0 ),	/* 0 */
/* 270 */	NdrFcShort( 0x24 ),	/* 36 */
/* 272 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x2,		/* 2 */
/* 274 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 276 */	NdrFcShort( 0x0 ),	/* 0 */
/* 278 */	NdrFcShort( 0x0 ),	/* 0 */
/* 280 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter phBitmap */

/* 282 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 284 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 286 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 288 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 290 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 292 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure FreeThumbnail */

/* 294 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 296 */	NdrFcLong( 0x0 ),	/* 0 */
/* 300 */	NdrFcShort( 0xf ),	/* 15 */
/* 302 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 304 */	NdrFcShort( 0x0 ),	/* 0 */
/* 306 */	NdrFcShort( 0x8 ),	/* 8 */
/* 308 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x1,		/* 1 */
/* 310 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 312 */	NdrFcShort( 0x0 ),	/* 0 */
/* 314 */	NdrFcShort( 0x0 ),	/* 0 */
/* 316 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Return value */

/* 318 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 320 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 322 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure SaveToFile */

/* 324 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 326 */	NdrFcLong( 0x0 ),	/* 0 */
/* 330 */	NdrFcShort( 0x10 ),	/* 16 */
/* 332 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 334 */	NdrFcShort( 0x0 ),	/* 0 */
/* 336 */	NdrFcShort( 0x8 ),	/* 8 */
/* 338 */	0x46,		/* Oi2 Flags:  clt must size, has return, has ext, */
			0x2,		/* 2 */
/* 340 */	0x8,		/* 8 */
			0x5,		/* Ext Flags:  new corr desc, srv corr check, */
/* 342 */	NdrFcShort( 0x0 ),	/* 0 */
/* 344 */	NdrFcShort( 0x1 ),	/* 1 */
/* 346 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter FilePath */

/* 348 */	NdrFcShort( 0x8b ),	/* Flags:  must size, must free, in, by val, */
/* 350 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 352 */	NdrFcShort( 0x1c ),	/* Type Offset=28 */

	/* Return value */

/* 354 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 356 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 358 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure get_Stride */

/* 360 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 362 */	NdrFcLong( 0x0 ),	/* 0 */
/* 366 */	NdrFcShort( 0x11 ),	/* 17 */
/* 368 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 370 */	NdrFcShort( 0x0 ),	/* 0 */
/* 372 */	NdrFcShort( 0x24 ),	/* 36 */
/* 374 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x2,		/* 2 */
/* 376 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 378 */	NdrFcShort( 0x0 ),	/* 0 */
/* 380 */	NdrFcShort( 0x0 ),	/* 0 */
/* 382 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter pVal */

/* 384 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 386 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 388 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 390 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 392 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 394 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure get_Version */

/* 396 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 398 */	NdrFcLong( 0x0 ),	/* 0 */
/* 402 */	NdrFcShort( 0x12 ),	/* 18 */
/* 404 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 406 */	NdrFcShort( 0x0 ),	/* 0 */
/* 408 */	NdrFcShort( 0x8 ),	/* 8 */
/* 410 */	0x45,		/* Oi2 Flags:  srv must size, has return, has ext, */
			0x2,		/* 2 */
/* 412 */	0x8,		/* 8 */
			0x3,		/* Ext Flags:  new corr desc, clt corr check, */
/* 414 */	NdrFcShort( 0x1 ),	/* 1 */
/* 416 */	NdrFcShort( 0x0 ),	/* 0 */
/* 418 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter pVal */

/* 420 */	NdrFcShort( 0x2113 ),	/* Flags:  must size, must free, out, simple ref, srv alloc size=8 */
/* 422 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 424 */	NdrFcShort( 0x2e ),	/* Type Offset=46 */

	/* Return value */

/* 426 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 428 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 430 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure get_ThumbJpgData */

/* 432 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 434 */	NdrFcLong( 0x0 ),	/* 0 */
/* 438 */	NdrFcShort( 0x13 ),	/* 19 */
/* 440 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 442 */	NdrFcShort( 0x8 ),	/* 8 */
/* 444 */	NdrFcShort( 0x8 ),	/* 8 */
/* 446 */	0x45,		/* Oi2 Flags:  srv must size, has return, has ext, */
			0x3,		/* 3 */
/* 448 */	0x8,		/* 8 */
			0x3,		/* Ext Flags:  new corr desc, clt corr check, */
/* 450 */	NdrFcShort( 0x20 ),	/* 32 */
/* 452 */	NdrFcShort( 0x0 ),	/* 0 */
/* 454 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter JpegQuality */

/* 456 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 458 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 460 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter pVal */

/* 462 */	NdrFcShort( 0x4113 ),	/* Flags:  must size, must free, out, simple ref, srv alloc size=16 */
/* 464 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 466 */	NdrFcShort( 0x424 ),	/* Type Offset=1060 */

	/* Return value */

/* 468 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 470 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 472 */	0x8,		/* FC_LONG */
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
			0x12, 0x0,	/* FC_UP */
/*  4 */	NdrFcShort( 0xe ),	/* Offset= 14 (18) */
/*  6 */	
			0x1b,		/* FC_CARRAY */
			0x1,		/* 1 */
/*  8 */	NdrFcShort( 0x2 ),	/* 2 */
/* 10 */	0x9,		/* Corr desc: FC_ULONG */
			0x0,		/*  */
/* 12 */	NdrFcShort( 0xfffc ),	/* -4 */
/* 14 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 16 */	0x6,		/* FC_SHORT */
			0x5b,		/* FC_END */
/* 18 */	
			0x17,		/* FC_CSTRUCT */
			0x3,		/* 3 */
/* 20 */	NdrFcShort( 0x8 ),	/* 8 */
/* 22 */	NdrFcShort( 0xfff0 ),	/* Offset= -16 (6) */
/* 24 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 26 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 28 */	0xb4,		/* FC_USER_MARSHAL */
			0x83,		/* 131 */
/* 30 */	NdrFcShort( 0x0 ),	/* 0 */
/* 32 */	NdrFcShort( 0x4 ),	/* 4 */
/* 34 */	NdrFcShort( 0x0 ),	/* 0 */
/* 36 */	NdrFcShort( 0xffde ),	/* Offset= -34 (2) */
/* 38 */	
			0x11, 0x4,	/* FC_RP [alloced_on_stack] */
/* 40 */	NdrFcShort( 0x6 ),	/* Offset= 6 (46) */
/* 42 */	
			0x13, 0x0,	/* FC_OP */
/* 44 */	NdrFcShort( 0xffe6 ),	/* Offset= -26 (18) */
/* 46 */	0xb4,		/* FC_USER_MARSHAL */
			0x83,		/* 131 */
/* 48 */	NdrFcShort( 0x0 ),	/* 0 */
/* 50 */	NdrFcShort( 0x4 ),	/* 4 */
/* 52 */	NdrFcShort( 0x0 ),	/* 0 */
/* 54 */	NdrFcShort( 0xfff4 ),	/* Offset= -12 (42) */
/* 56 */	
			0x11, 0xc,	/* FC_RP [alloced_on_stack] [simple_pointer] */
/* 58 */	0x8,		/* FC_LONG */
			0x5c,		/* FC_PAD */
/* 60 */	
			0x11, 0x4,	/* FC_RP [alloced_on_stack] */
/* 62 */	NdrFcShort( 0x3e6 ),	/* Offset= 998 (1060) */
/* 64 */	
			0x13, 0x0,	/* FC_OP */
/* 66 */	NdrFcShort( 0x3ce ),	/* Offset= 974 (1040) */
/* 68 */	
			0x2b,		/* FC_NON_ENCAPSULATED_UNION */
			0x9,		/* FC_ULONG */
/* 70 */	0x7,		/* Corr desc: FC_USHORT */
			0x0,		/*  */
/* 72 */	NdrFcShort( 0xfff8 ),	/* -8 */
/* 74 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 76 */	NdrFcShort( 0x2 ),	/* Offset= 2 (78) */
/* 78 */	NdrFcShort( 0x10 ),	/* 16 */
/* 80 */	NdrFcShort( 0x2f ),	/* 47 */
/* 82 */	NdrFcLong( 0x14 ),	/* 20 */
/* 86 */	NdrFcShort( 0x800b ),	/* Simple arm type: FC_HYPER */
/* 88 */	NdrFcLong( 0x3 ),	/* 3 */
/* 92 */	NdrFcShort( 0x8008 ),	/* Simple arm type: FC_LONG */
/* 94 */	NdrFcLong( 0x11 ),	/* 17 */
/* 98 */	NdrFcShort( 0x8001 ),	/* Simple arm type: FC_BYTE */
/* 100 */	NdrFcLong( 0x2 ),	/* 2 */
/* 104 */	NdrFcShort( 0x8006 ),	/* Simple arm type: FC_SHORT */
/* 106 */	NdrFcLong( 0x4 ),	/* 4 */
/* 110 */	NdrFcShort( 0x800a ),	/* Simple arm type: FC_FLOAT */
/* 112 */	NdrFcLong( 0x5 ),	/* 5 */
/* 116 */	NdrFcShort( 0x800c ),	/* Simple arm type: FC_DOUBLE */
/* 118 */	NdrFcLong( 0xb ),	/* 11 */
/* 122 */	NdrFcShort( 0x8006 ),	/* Simple arm type: FC_SHORT */
/* 124 */	NdrFcLong( 0xa ),	/* 10 */
/* 128 */	NdrFcShort( 0x8008 ),	/* Simple arm type: FC_LONG */
/* 130 */	NdrFcLong( 0x6 ),	/* 6 */
/* 134 */	NdrFcShort( 0xe8 ),	/* Offset= 232 (366) */
/* 136 */	NdrFcLong( 0x7 ),	/* 7 */
/* 140 */	NdrFcShort( 0x800c ),	/* Simple arm type: FC_DOUBLE */
/* 142 */	NdrFcLong( 0x8 ),	/* 8 */
/* 146 */	NdrFcShort( 0xff98 ),	/* Offset= -104 (42) */
/* 148 */	NdrFcLong( 0xd ),	/* 13 */
/* 152 */	NdrFcShort( 0xdc ),	/* Offset= 220 (372) */
/* 154 */	NdrFcLong( 0x9 ),	/* 9 */
/* 158 */	NdrFcShort( 0xe8 ),	/* Offset= 232 (390) */
/* 160 */	NdrFcLong( 0x2000 ),	/* 8192 */
/* 164 */	NdrFcShort( 0xf4 ),	/* Offset= 244 (408) */
/* 166 */	NdrFcLong( 0x24 ),	/* 36 */
/* 170 */	NdrFcShort( 0x31c ),	/* Offset= 796 (966) */
/* 172 */	NdrFcLong( 0x4024 ),	/* 16420 */
/* 176 */	NdrFcShort( 0x316 ),	/* Offset= 790 (966) */
/* 178 */	NdrFcLong( 0x4011 ),	/* 16401 */
/* 182 */	NdrFcShort( 0x314 ),	/* Offset= 788 (970) */
/* 184 */	NdrFcLong( 0x4002 ),	/* 16386 */
/* 188 */	NdrFcShort( 0x312 ),	/* Offset= 786 (974) */
/* 190 */	NdrFcLong( 0x4003 ),	/* 16387 */
/* 194 */	NdrFcShort( 0x310 ),	/* Offset= 784 (978) */
/* 196 */	NdrFcLong( 0x4014 ),	/* 16404 */
/* 200 */	NdrFcShort( 0x30e ),	/* Offset= 782 (982) */
/* 202 */	NdrFcLong( 0x4004 ),	/* 16388 */
/* 206 */	NdrFcShort( 0x30c ),	/* Offset= 780 (986) */
/* 208 */	NdrFcLong( 0x4005 ),	/* 16389 */
/* 212 */	NdrFcShort( 0x30a ),	/* Offset= 778 (990) */
/* 214 */	NdrFcLong( 0x400b ),	/* 16395 */
/* 218 */	NdrFcShort( 0x2f4 ),	/* Offset= 756 (974) */
/* 220 */	NdrFcLong( 0x400a ),	/* 16394 */
/* 224 */	NdrFcShort( 0x2f2 ),	/* Offset= 754 (978) */
/* 226 */	NdrFcLong( 0x4006 ),	/* 16390 */
/* 230 */	NdrFcShort( 0x2fc ),	/* Offset= 764 (994) */
/* 232 */	NdrFcLong( 0x4007 ),	/* 16391 */
/* 236 */	NdrFcShort( 0x2f2 ),	/* Offset= 754 (990) */
/* 238 */	NdrFcLong( 0x4008 ),	/* 16392 */
/* 242 */	NdrFcShort( 0x2f4 ),	/* Offset= 756 (998) */
/* 244 */	NdrFcLong( 0x400d ),	/* 16397 */
/* 248 */	NdrFcShort( 0x2f2 ),	/* Offset= 754 (1002) */
/* 250 */	NdrFcLong( 0x4009 ),	/* 16393 */
/* 254 */	NdrFcShort( 0x2f0 ),	/* Offset= 752 (1006) */
/* 256 */	NdrFcLong( 0x6000 ),	/* 24576 */
/* 260 */	NdrFcShort( 0x2ee ),	/* Offset= 750 (1010) */
/* 262 */	NdrFcLong( 0x400c ),	/* 16396 */
/* 266 */	NdrFcShort( 0x2ec ),	/* Offset= 748 (1014) */
/* 268 */	NdrFcLong( 0x10 ),	/* 16 */
/* 272 */	NdrFcShort( 0x8002 ),	/* Simple arm type: FC_CHAR */
/* 274 */	NdrFcLong( 0x12 ),	/* 18 */
/* 278 */	NdrFcShort( 0x8006 ),	/* Simple arm type: FC_SHORT */
/* 280 */	NdrFcLong( 0x13 ),	/* 19 */
/* 284 */	NdrFcShort( 0x8008 ),	/* Simple arm type: FC_LONG */
/* 286 */	NdrFcLong( 0x15 ),	/* 21 */
/* 290 */	NdrFcShort( 0x800b ),	/* Simple arm type: FC_HYPER */
/* 292 */	NdrFcLong( 0x16 ),	/* 22 */
/* 296 */	NdrFcShort( 0x8008 ),	/* Simple arm type: FC_LONG */
/* 298 */	NdrFcLong( 0x17 ),	/* 23 */
/* 302 */	NdrFcShort( 0x8008 ),	/* Simple arm type: FC_LONG */
/* 304 */	NdrFcLong( 0xe ),	/* 14 */
/* 308 */	NdrFcShort( 0x2ca ),	/* Offset= 714 (1022) */
/* 310 */	NdrFcLong( 0x400e ),	/* 16398 */
/* 314 */	NdrFcShort( 0x2ce ),	/* Offset= 718 (1032) */
/* 316 */	NdrFcLong( 0x4010 ),	/* 16400 */
/* 320 */	NdrFcShort( 0x2cc ),	/* Offset= 716 (1036) */
/* 322 */	NdrFcLong( 0x4012 ),	/* 16402 */
/* 326 */	NdrFcShort( 0x288 ),	/* Offset= 648 (974) */
/* 328 */	NdrFcLong( 0x4013 ),	/* 16403 */
/* 332 */	NdrFcShort( 0x286 ),	/* Offset= 646 (978) */
/* 334 */	NdrFcLong( 0x4015 ),	/* 16405 */
/* 338 */	NdrFcShort( 0x284 ),	/* Offset= 644 (982) */
/* 340 */	NdrFcLong( 0x4016 ),	/* 16406 */
/* 344 */	NdrFcShort( 0x27a ),	/* Offset= 634 (978) */
/* 346 */	NdrFcLong( 0x4017 ),	/* 16407 */
/* 350 */	NdrFcShort( 0x274 ),	/* Offset= 628 (978) */
/* 352 */	NdrFcLong( 0x0 ),	/* 0 */
/* 356 */	NdrFcShort( 0x0 ),	/* Offset= 0 (356) */
/* 358 */	NdrFcLong( 0x1 ),	/* 1 */
/* 362 */	NdrFcShort( 0x0 ),	/* Offset= 0 (362) */
/* 364 */	NdrFcShort( 0xffff ),	/* Offset= -1 (363) */
/* 366 */	
			0x15,		/* FC_STRUCT */
			0x7,		/* 7 */
/* 368 */	NdrFcShort( 0x8 ),	/* 8 */
/* 370 */	0xb,		/* FC_HYPER */
			0x5b,		/* FC_END */
/* 372 */	
			0x2f,		/* FC_IP */
			0x5a,		/* FC_CONSTANT_IID */
/* 374 */	NdrFcLong( 0x0 ),	/* 0 */
/* 378 */	NdrFcShort( 0x0 ),	/* 0 */
/* 380 */	NdrFcShort( 0x0 ),	/* 0 */
/* 382 */	0xc0,		/* 192 */
			0x0,		/* 0 */
/* 384 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 386 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 388 */	0x0,		/* 0 */
			0x46,		/* 70 */
/* 390 */	
			0x2f,		/* FC_IP */
			0x5a,		/* FC_CONSTANT_IID */
/* 392 */	NdrFcLong( 0x20400 ),	/* 132096 */
/* 396 */	NdrFcShort( 0x0 ),	/* 0 */
/* 398 */	NdrFcShort( 0x0 ),	/* 0 */
/* 400 */	0xc0,		/* 192 */
			0x0,		/* 0 */
/* 402 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 404 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 406 */	0x0,		/* 0 */
			0x46,		/* 70 */
/* 408 */	
			0x13, 0x10,	/* FC_OP [pointer_deref] */
/* 410 */	NdrFcShort( 0x2 ),	/* Offset= 2 (412) */
/* 412 */	
			0x13, 0x0,	/* FC_OP */
/* 414 */	NdrFcShort( 0x216 ),	/* Offset= 534 (948) */
/* 416 */	
			0x2a,		/* FC_ENCAPSULATED_UNION */
			0x49,		/* 73 */
/* 418 */	NdrFcShort( 0x18 ),	/* 24 */
/* 420 */	NdrFcShort( 0xa ),	/* 10 */
/* 422 */	NdrFcLong( 0x8 ),	/* 8 */
/* 426 */	NdrFcShort( 0x5a ),	/* Offset= 90 (516) */
/* 428 */	NdrFcLong( 0xd ),	/* 13 */
/* 432 */	NdrFcShort( 0x7e ),	/* Offset= 126 (558) */
/* 434 */	NdrFcLong( 0x9 ),	/* 9 */
/* 438 */	NdrFcShort( 0x9e ),	/* Offset= 158 (596) */
/* 440 */	NdrFcLong( 0xc ),	/* 12 */
/* 444 */	NdrFcShort( 0xc8 ),	/* Offset= 200 (644) */
/* 446 */	NdrFcLong( 0x24 ),	/* 36 */
/* 450 */	NdrFcShort( 0x124 ),	/* Offset= 292 (742) */
/* 452 */	NdrFcLong( 0x800d ),	/* 32781 */
/* 456 */	NdrFcShort( 0x140 ),	/* Offset= 320 (776) */
/* 458 */	NdrFcLong( 0x10 ),	/* 16 */
/* 462 */	NdrFcShort( 0x15a ),	/* Offset= 346 (808) */
/* 464 */	NdrFcLong( 0x2 ),	/* 2 */
/* 468 */	NdrFcShort( 0x174 ),	/* Offset= 372 (840) */
/* 470 */	NdrFcLong( 0x3 ),	/* 3 */
/* 474 */	NdrFcShort( 0x18e ),	/* Offset= 398 (872) */
/* 476 */	NdrFcLong( 0x14 ),	/* 20 */
/* 480 */	NdrFcShort( 0x1a8 ),	/* Offset= 424 (904) */
/* 482 */	NdrFcShort( 0xffff ),	/* Offset= -1 (481) */
/* 484 */	
			0x1b,		/* FC_CARRAY */
			0x3,		/* 3 */
/* 486 */	NdrFcShort( 0x4 ),	/* 4 */
/* 488 */	0x19,		/* Corr desc:  field pointer, FC_ULONG */
			0x0,		/*  */
/* 490 */	NdrFcShort( 0x0 ),	/* 0 */
/* 492 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 494 */	
			0x4b,		/* FC_PP */
			0x5c,		/* FC_PAD */
/* 496 */	
			0x48,		/* FC_VARIABLE_REPEAT */
			0x49,		/* FC_FIXED_OFFSET */
/* 498 */	NdrFcShort( 0x4 ),	/* 4 */
/* 500 */	NdrFcShort( 0x0 ),	/* 0 */
/* 502 */	NdrFcShort( 0x1 ),	/* 1 */
/* 504 */	NdrFcShort( 0x0 ),	/* 0 */
/* 506 */	NdrFcShort( 0x0 ),	/* 0 */
/* 508 */	0x13, 0x0,	/* FC_OP */
/* 510 */	NdrFcShort( 0xfe14 ),	/* Offset= -492 (18) */
/* 512 */	
			0x5b,		/* FC_END */

			0x8,		/* FC_LONG */
/* 514 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 516 */	
			0x16,		/* FC_PSTRUCT */
			0x3,		/* 3 */
/* 518 */	NdrFcShort( 0x8 ),	/* 8 */
/* 520 */	
			0x4b,		/* FC_PP */
			0x5c,		/* FC_PAD */
/* 522 */	
			0x46,		/* FC_NO_REPEAT */
			0x5c,		/* FC_PAD */
/* 524 */	NdrFcShort( 0x4 ),	/* 4 */
/* 526 */	NdrFcShort( 0x4 ),	/* 4 */
/* 528 */	0x11, 0x0,	/* FC_RP */
/* 530 */	NdrFcShort( 0xffd2 ),	/* Offset= -46 (484) */
/* 532 */	
			0x5b,		/* FC_END */

			0x8,		/* FC_LONG */
/* 534 */	0x8,		/* FC_LONG */
			0x5b,		/* FC_END */
/* 536 */	
			0x21,		/* FC_BOGUS_ARRAY */
			0x3,		/* 3 */
/* 538 */	NdrFcShort( 0x0 ),	/* 0 */
/* 540 */	0x19,		/* Corr desc:  field pointer, FC_ULONG */
			0x0,		/*  */
/* 542 */	NdrFcShort( 0x0 ),	/* 0 */
/* 544 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 546 */	NdrFcLong( 0xffffffff ),	/* -1 */
/* 550 */	NdrFcShort( 0x0 ),	/* Corr flags:  */
/* 552 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 554 */	NdrFcShort( 0xff4a ),	/* Offset= -182 (372) */
/* 556 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 558 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 560 */	NdrFcShort( 0x8 ),	/* 8 */
/* 562 */	NdrFcShort( 0x0 ),	/* 0 */
/* 564 */	NdrFcShort( 0x6 ),	/* Offset= 6 (570) */
/* 566 */	0x8,		/* FC_LONG */
			0x36,		/* FC_POINTER */
/* 568 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 570 */	
			0x11, 0x0,	/* FC_RP */
/* 572 */	NdrFcShort( 0xffdc ),	/* Offset= -36 (536) */
/* 574 */	
			0x21,		/* FC_BOGUS_ARRAY */
			0x3,		/* 3 */
/* 576 */	NdrFcShort( 0x0 ),	/* 0 */
/* 578 */	0x19,		/* Corr desc:  field pointer, FC_ULONG */
			0x0,		/*  */
/* 580 */	NdrFcShort( 0x0 ),	/* 0 */
/* 582 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 584 */	NdrFcLong( 0xffffffff ),	/* -1 */
/* 588 */	NdrFcShort( 0x0 ),	/* Corr flags:  */
/* 590 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 592 */	NdrFcShort( 0xff36 ),	/* Offset= -202 (390) */
/* 594 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 596 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 598 */	NdrFcShort( 0x8 ),	/* 8 */
/* 600 */	NdrFcShort( 0x0 ),	/* 0 */
/* 602 */	NdrFcShort( 0x6 ),	/* Offset= 6 (608) */
/* 604 */	0x8,		/* FC_LONG */
			0x36,		/* FC_POINTER */
/* 606 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 608 */	
			0x11, 0x0,	/* FC_RP */
/* 610 */	NdrFcShort( 0xffdc ),	/* Offset= -36 (574) */
/* 612 */	
			0x1b,		/* FC_CARRAY */
			0x3,		/* 3 */
/* 614 */	NdrFcShort( 0x4 ),	/* 4 */
/* 616 */	0x19,		/* Corr desc:  field pointer, FC_ULONG */
			0x0,		/*  */
/* 618 */	NdrFcShort( 0x0 ),	/* 0 */
/* 620 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 622 */	
			0x4b,		/* FC_PP */
			0x5c,		/* FC_PAD */
/* 624 */	
			0x48,		/* FC_VARIABLE_REPEAT */
			0x49,		/* FC_FIXED_OFFSET */
/* 626 */	NdrFcShort( 0x4 ),	/* 4 */
/* 628 */	NdrFcShort( 0x0 ),	/* 0 */
/* 630 */	NdrFcShort( 0x1 ),	/* 1 */
/* 632 */	NdrFcShort( 0x0 ),	/* 0 */
/* 634 */	NdrFcShort( 0x0 ),	/* 0 */
/* 636 */	0x13, 0x0,	/* FC_OP */
/* 638 */	NdrFcShort( 0x192 ),	/* Offset= 402 (1040) */
/* 640 */	
			0x5b,		/* FC_END */

			0x8,		/* FC_LONG */
/* 642 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 644 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 646 */	NdrFcShort( 0x8 ),	/* 8 */
/* 648 */	NdrFcShort( 0x0 ),	/* 0 */
/* 650 */	NdrFcShort( 0x6 ),	/* Offset= 6 (656) */
/* 652 */	0x8,		/* FC_LONG */
			0x36,		/* FC_POINTER */
/* 654 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 656 */	
			0x11, 0x0,	/* FC_RP */
/* 658 */	NdrFcShort( 0xffd2 ),	/* Offset= -46 (612) */
/* 660 */	
			0x2f,		/* FC_IP */
			0x5a,		/* FC_CONSTANT_IID */
/* 662 */	NdrFcLong( 0x2f ),	/* 47 */
/* 666 */	NdrFcShort( 0x0 ),	/* 0 */
/* 668 */	NdrFcShort( 0x0 ),	/* 0 */
/* 670 */	0xc0,		/* 192 */
			0x0,		/* 0 */
/* 672 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 674 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 676 */	0x0,		/* 0 */
			0x46,		/* 70 */
/* 678 */	
			0x1b,		/* FC_CARRAY */
			0x0,		/* 0 */
/* 680 */	NdrFcShort( 0x1 ),	/* 1 */
/* 682 */	0x19,		/* Corr desc:  field pointer, FC_ULONG */
			0x0,		/*  */
/* 684 */	NdrFcShort( 0x4 ),	/* 4 */
/* 686 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 688 */	0x1,		/* FC_BYTE */
			0x5b,		/* FC_END */
/* 690 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 692 */	NdrFcShort( 0x10 ),	/* 16 */
/* 694 */	NdrFcShort( 0x0 ),	/* 0 */
/* 696 */	NdrFcShort( 0xa ),	/* Offset= 10 (706) */
/* 698 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 700 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 702 */	NdrFcShort( 0xffd6 ),	/* Offset= -42 (660) */
/* 704 */	0x36,		/* FC_POINTER */
			0x5b,		/* FC_END */
/* 706 */	
			0x13, 0x0,	/* FC_OP */
/* 708 */	NdrFcShort( 0xffe2 ),	/* Offset= -30 (678) */
/* 710 */	
			0x1b,		/* FC_CARRAY */
			0x3,		/* 3 */
/* 712 */	NdrFcShort( 0x4 ),	/* 4 */
/* 714 */	0x19,		/* Corr desc:  field pointer, FC_ULONG */
			0x0,		/*  */
/* 716 */	NdrFcShort( 0x0 ),	/* 0 */
/* 718 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 720 */	
			0x4b,		/* FC_PP */
			0x5c,		/* FC_PAD */
/* 722 */	
			0x48,		/* FC_VARIABLE_REPEAT */
			0x49,		/* FC_FIXED_OFFSET */
/* 724 */	NdrFcShort( 0x4 ),	/* 4 */
/* 726 */	NdrFcShort( 0x0 ),	/* 0 */
/* 728 */	NdrFcShort( 0x1 ),	/* 1 */
/* 730 */	NdrFcShort( 0x0 ),	/* 0 */
/* 732 */	NdrFcShort( 0x0 ),	/* 0 */
/* 734 */	0x13, 0x0,	/* FC_OP */
/* 736 */	NdrFcShort( 0xffd2 ),	/* Offset= -46 (690) */
/* 738 */	
			0x5b,		/* FC_END */

			0x8,		/* FC_LONG */
/* 740 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 742 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 744 */	NdrFcShort( 0x8 ),	/* 8 */
/* 746 */	NdrFcShort( 0x0 ),	/* 0 */
/* 748 */	NdrFcShort( 0x6 ),	/* Offset= 6 (754) */
/* 750 */	0x8,		/* FC_LONG */
			0x36,		/* FC_POINTER */
/* 752 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 754 */	
			0x11, 0x0,	/* FC_RP */
/* 756 */	NdrFcShort( 0xffd2 ),	/* Offset= -46 (710) */
/* 758 */	
			0x1d,		/* FC_SMFARRAY */
			0x0,		/* 0 */
/* 760 */	NdrFcShort( 0x8 ),	/* 8 */
/* 762 */	0x1,		/* FC_BYTE */
			0x5b,		/* FC_END */
/* 764 */	
			0x15,		/* FC_STRUCT */
			0x3,		/* 3 */
/* 766 */	NdrFcShort( 0x10 ),	/* 16 */
/* 768 */	0x8,		/* FC_LONG */
			0x6,		/* FC_SHORT */
/* 770 */	0x6,		/* FC_SHORT */
			0x4c,		/* FC_EMBEDDED_COMPLEX */
/* 772 */	0x0,		/* 0 */
			NdrFcShort( 0xfff1 ),	/* Offset= -15 (758) */
			0x5b,		/* FC_END */
/* 776 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 778 */	NdrFcShort( 0x18 ),	/* 24 */
/* 780 */	NdrFcShort( 0x0 ),	/* 0 */
/* 782 */	NdrFcShort( 0xa ),	/* Offset= 10 (792) */
/* 784 */	0x8,		/* FC_LONG */
			0x36,		/* FC_POINTER */
/* 786 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 788 */	NdrFcShort( 0xffe8 ),	/* Offset= -24 (764) */
/* 790 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 792 */	
			0x11, 0x0,	/* FC_RP */
/* 794 */	NdrFcShort( 0xfefe ),	/* Offset= -258 (536) */
/* 796 */	
			0x1b,		/* FC_CARRAY */
			0x0,		/* 0 */
/* 798 */	NdrFcShort( 0x1 ),	/* 1 */
/* 800 */	0x19,		/* Corr desc:  field pointer, FC_ULONG */
			0x0,		/*  */
/* 802 */	NdrFcShort( 0x0 ),	/* 0 */
/* 804 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 806 */	0x1,		/* FC_BYTE */
			0x5b,		/* FC_END */
/* 808 */	
			0x16,		/* FC_PSTRUCT */
			0x3,		/* 3 */
/* 810 */	NdrFcShort( 0x8 ),	/* 8 */
/* 812 */	
			0x4b,		/* FC_PP */
			0x5c,		/* FC_PAD */
/* 814 */	
			0x46,		/* FC_NO_REPEAT */
			0x5c,		/* FC_PAD */
/* 816 */	NdrFcShort( 0x4 ),	/* 4 */
/* 818 */	NdrFcShort( 0x4 ),	/* 4 */
/* 820 */	0x13, 0x0,	/* FC_OP */
/* 822 */	NdrFcShort( 0xffe6 ),	/* Offset= -26 (796) */
/* 824 */	
			0x5b,		/* FC_END */

			0x8,		/* FC_LONG */
/* 826 */	0x8,		/* FC_LONG */
			0x5b,		/* FC_END */
/* 828 */	
			0x1b,		/* FC_CARRAY */
			0x1,		/* 1 */
/* 830 */	NdrFcShort( 0x2 ),	/* 2 */
/* 832 */	0x19,		/* Corr desc:  field pointer, FC_ULONG */
			0x0,		/*  */
/* 834 */	NdrFcShort( 0x0 ),	/* 0 */
/* 836 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 838 */	0x6,		/* FC_SHORT */
			0x5b,		/* FC_END */
/* 840 */	
			0x16,		/* FC_PSTRUCT */
			0x3,		/* 3 */
/* 842 */	NdrFcShort( 0x8 ),	/* 8 */
/* 844 */	
			0x4b,		/* FC_PP */
			0x5c,		/* FC_PAD */
/* 846 */	
			0x46,		/* FC_NO_REPEAT */
			0x5c,		/* FC_PAD */
/* 848 */	NdrFcShort( 0x4 ),	/* 4 */
/* 850 */	NdrFcShort( 0x4 ),	/* 4 */
/* 852 */	0x13, 0x0,	/* FC_OP */
/* 854 */	NdrFcShort( 0xffe6 ),	/* Offset= -26 (828) */
/* 856 */	
			0x5b,		/* FC_END */

			0x8,		/* FC_LONG */
/* 858 */	0x8,		/* FC_LONG */
			0x5b,		/* FC_END */
/* 860 */	
			0x1b,		/* FC_CARRAY */
			0x3,		/* 3 */
/* 862 */	NdrFcShort( 0x4 ),	/* 4 */
/* 864 */	0x19,		/* Corr desc:  field pointer, FC_ULONG */
			0x0,		/*  */
/* 866 */	NdrFcShort( 0x0 ),	/* 0 */
/* 868 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 870 */	0x8,		/* FC_LONG */
			0x5b,		/* FC_END */
/* 872 */	
			0x16,		/* FC_PSTRUCT */
			0x3,		/* 3 */
/* 874 */	NdrFcShort( 0x8 ),	/* 8 */
/* 876 */	
			0x4b,		/* FC_PP */
			0x5c,		/* FC_PAD */
/* 878 */	
			0x46,		/* FC_NO_REPEAT */
			0x5c,		/* FC_PAD */
/* 880 */	NdrFcShort( 0x4 ),	/* 4 */
/* 882 */	NdrFcShort( 0x4 ),	/* 4 */
/* 884 */	0x13, 0x0,	/* FC_OP */
/* 886 */	NdrFcShort( 0xffe6 ),	/* Offset= -26 (860) */
/* 888 */	
			0x5b,		/* FC_END */

			0x8,		/* FC_LONG */
/* 890 */	0x8,		/* FC_LONG */
			0x5b,		/* FC_END */
/* 892 */	
			0x1b,		/* FC_CARRAY */
			0x7,		/* 7 */
/* 894 */	NdrFcShort( 0x8 ),	/* 8 */
/* 896 */	0x19,		/* Corr desc:  field pointer, FC_ULONG */
			0x0,		/*  */
/* 898 */	NdrFcShort( 0x0 ),	/* 0 */
/* 900 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 902 */	0xb,		/* FC_HYPER */
			0x5b,		/* FC_END */
/* 904 */	
			0x16,		/* FC_PSTRUCT */
			0x3,		/* 3 */
/* 906 */	NdrFcShort( 0x8 ),	/* 8 */
/* 908 */	
			0x4b,		/* FC_PP */
			0x5c,		/* FC_PAD */
/* 910 */	
			0x46,		/* FC_NO_REPEAT */
			0x5c,		/* FC_PAD */
/* 912 */	NdrFcShort( 0x4 ),	/* 4 */
/* 914 */	NdrFcShort( 0x4 ),	/* 4 */
/* 916 */	0x13, 0x0,	/* FC_OP */
/* 918 */	NdrFcShort( 0xffe6 ),	/* Offset= -26 (892) */
/* 920 */	
			0x5b,		/* FC_END */

			0x8,		/* FC_LONG */
/* 922 */	0x8,		/* FC_LONG */
			0x5b,		/* FC_END */
/* 924 */	
			0x15,		/* FC_STRUCT */
			0x3,		/* 3 */
/* 926 */	NdrFcShort( 0x8 ),	/* 8 */
/* 928 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 930 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 932 */	
			0x1b,		/* FC_CARRAY */
			0x3,		/* 3 */
/* 934 */	NdrFcShort( 0x8 ),	/* 8 */
/* 936 */	0x7,		/* Corr desc: FC_USHORT */
			0x0,		/*  */
/* 938 */	NdrFcShort( 0xffd8 ),	/* -40 */
/* 940 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 942 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 944 */	NdrFcShort( 0xffec ),	/* Offset= -20 (924) */
/* 946 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 948 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 950 */	NdrFcShort( 0x28 ),	/* 40 */
/* 952 */	NdrFcShort( 0xffec ),	/* Offset= -20 (932) */
/* 954 */	NdrFcShort( 0x0 ),	/* Offset= 0 (954) */
/* 956 */	0x6,		/* FC_SHORT */
			0x6,		/* FC_SHORT */
/* 958 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 960 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 962 */	NdrFcShort( 0xfdde ),	/* Offset= -546 (416) */
/* 964 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 966 */	
			0x13, 0x0,	/* FC_OP */
/* 968 */	NdrFcShort( 0xfeea ),	/* Offset= -278 (690) */
/* 970 */	
			0x13, 0x8,	/* FC_OP [simple_pointer] */
/* 972 */	0x1,		/* FC_BYTE */
			0x5c,		/* FC_PAD */
/* 974 */	
			0x13, 0x8,	/* FC_OP [simple_pointer] */
/* 976 */	0x6,		/* FC_SHORT */
			0x5c,		/* FC_PAD */
/* 978 */	
			0x13, 0x8,	/* FC_OP [simple_pointer] */
/* 980 */	0x8,		/* FC_LONG */
			0x5c,		/* FC_PAD */
/* 982 */	
			0x13, 0x8,	/* FC_OP [simple_pointer] */
/* 984 */	0xb,		/* FC_HYPER */
			0x5c,		/* FC_PAD */
/* 986 */	
			0x13, 0x8,	/* FC_OP [simple_pointer] */
/* 988 */	0xa,		/* FC_FLOAT */
			0x5c,		/* FC_PAD */
/* 990 */	
			0x13, 0x8,	/* FC_OP [simple_pointer] */
/* 992 */	0xc,		/* FC_DOUBLE */
			0x5c,		/* FC_PAD */
/* 994 */	
			0x13, 0x0,	/* FC_OP */
/* 996 */	NdrFcShort( 0xfd8a ),	/* Offset= -630 (366) */
/* 998 */	
			0x13, 0x10,	/* FC_OP [pointer_deref] */
/* 1000 */	NdrFcShort( 0xfc42 ),	/* Offset= -958 (42) */
/* 1002 */	
			0x13, 0x10,	/* FC_OP [pointer_deref] */
/* 1004 */	NdrFcShort( 0xfd88 ),	/* Offset= -632 (372) */
/* 1006 */	
			0x13, 0x10,	/* FC_OP [pointer_deref] */
/* 1008 */	NdrFcShort( 0xfd96 ),	/* Offset= -618 (390) */
/* 1010 */	
			0x13, 0x10,	/* FC_OP [pointer_deref] */
/* 1012 */	NdrFcShort( 0xfda4 ),	/* Offset= -604 (408) */
/* 1014 */	
			0x13, 0x10,	/* FC_OP [pointer_deref] */
/* 1016 */	NdrFcShort( 0x2 ),	/* Offset= 2 (1018) */
/* 1018 */	
			0x13, 0x0,	/* FC_OP */
/* 1020 */	NdrFcShort( 0x14 ),	/* Offset= 20 (1040) */
/* 1022 */	
			0x15,		/* FC_STRUCT */
			0x7,		/* 7 */
/* 1024 */	NdrFcShort( 0x10 ),	/* 16 */
/* 1026 */	0x6,		/* FC_SHORT */
			0x1,		/* FC_BYTE */
/* 1028 */	0x1,		/* FC_BYTE */
			0x8,		/* FC_LONG */
/* 1030 */	0xb,		/* FC_HYPER */
			0x5b,		/* FC_END */
/* 1032 */	
			0x13, 0x0,	/* FC_OP */
/* 1034 */	NdrFcShort( 0xfff4 ),	/* Offset= -12 (1022) */
/* 1036 */	
			0x13, 0x8,	/* FC_OP [simple_pointer] */
/* 1038 */	0x2,		/* FC_CHAR */
			0x5c,		/* FC_PAD */
/* 1040 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x7,		/* 7 */
/* 1042 */	NdrFcShort( 0x20 ),	/* 32 */
/* 1044 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1046 */	NdrFcShort( 0x0 ),	/* Offset= 0 (1046) */
/* 1048 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 1050 */	0x6,		/* FC_SHORT */
			0x6,		/* FC_SHORT */
/* 1052 */	0x6,		/* FC_SHORT */
			0x6,		/* FC_SHORT */
/* 1054 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 1056 */	NdrFcShort( 0xfc24 ),	/* Offset= -988 (68) */
/* 1058 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 1060 */	0xb4,		/* FC_USER_MARSHAL */
			0x83,		/* 131 */
/* 1062 */	NdrFcShort( 0x1 ),	/* 1 */
/* 1064 */	NdrFcShort( 0x10 ),	/* 16 */
/* 1066 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1068 */	NdrFcShort( 0xfc14 ),	/* Offset= -1004 (64) */

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
            },
            {
            VARIANT_UserSize
            ,VARIANT_UserMarshal
            ,VARIANT_UserUnmarshal
            ,VARIANT_UserFree
            }

        };



/* Object interface: IUnknown, ver. 0.0,
   GUID={0x00000000,0x0000,0x0000,{0xC0,0x00,0x00,0x00,0x00,0x00,0x00,0x46}} */


/* Object interface: IDispatch, ver. 0.0,
   GUID={0x00020400,0x0000,0x0000,{0xC0,0x00,0x00,0x00,0x00,0x00,0x00,0x46}} */


/* Object interface: IFileThumbExtract, ver. 0.0,
   GUID={0xA1B8E7E1,0x2B2E,0x46D2,{0xB6,0x0F,0x99,0x30,0x28,0xA5,0x81,0x82}} */

#pragma code_seg(".orpc")
static const unsigned short IFileThumbExtract_FormatStringOffsetTable[] =
    {
    (unsigned short) -1,
    (unsigned short) -1,
    (unsigned short) -1,
    (unsigned short) -1,
    0,
    36,
    72,
    108,
    150,
    186,
    222,
    258,
    294,
    324,
    360,
    396,
    432
    };

static const MIDL_STUBLESS_PROXY_INFO IFileThumbExtract_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &IFileThumbExtract_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IFileThumbExtract_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &IFileThumbExtract_FormatStringOffsetTable[-3],
    0,
    0,
    0,
    0};
CINTERFACE_PROXY_VTABLE(20) _IFileThumbExtractProxyVtbl = 
{
    &IFileThumbExtract_ProxyInfo,
    &IID_IFileThumbExtract,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    0 /* (void *) (INT_PTR) -1 /* IDispatch::GetTypeInfoCount */ ,
    0 /* (void *) (INT_PTR) -1 /* IDispatch::GetTypeInfo */ ,
    0 /* (void *) (INT_PTR) -1 /* IDispatch::GetIDsOfNames */ ,
    0 /* IDispatch_Invoke_Proxy */ ,
    (void *) (INT_PTR) -1 /* IFileThumbExtract::SetPath */ ,
    (void *) (INT_PTR) -1 /* IFileThumbExtract::SetDir */ ,
    (void *) (INT_PTR) -1 /* IFileThumbExtract::SetFile */ ,
    (void *) (INT_PTR) -1 /* IFileThumbExtract::SetThumbnailSize */ ,
    (void *) (INT_PTR) -1 /* IFileThumbExtract::GetPath */ ,
    (void *) (INT_PTR) -1 /* IFileThumbExtract::GetDir */ ,
    (void *) (INT_PTR) -1 /* IFileThumbExtract::GetFile */ ,
    (void *) (INT_PTR) -1 /* IFileThumbExtract::ExtractThumbnail */ ,
    (void *) (INT_PTR) -1 /* IFileThumbExtract::FreeThumbnail */ ,
    (void *) (INT_PTR) -1 /* IFileThumbExtract::SaveToFile */ ,
    (void *) (INT_PTR) -1 /* IFileThumbExtract::get_Stride */ ,
    (void *) (INT_PTR) -1 /* IFileThumbExtract::get_Version */ ,
    (void *) (INT_PTR) -1 /* IFileThumbExtract::get_ThumbJpgData */
};


static const PRPC_STUB_FUNCTION IFileThumbExtract_table[] =
{
    STUB_FORWARDING_FUNCTION,
    STUB_FORWARDING_FUNCTION,
    STUB_FORWARDING_FUNCTION,
    STUB_FORWARDING_FUNCTION,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2
};

CInterfaceStubVtbl _IFileThumbExtractStubVtbl =
{
    &IID_IFileThumbExtract,
    &IFileThumbExtract_ServerInfo,
    20,
    &IFileThumbExtract_table[-3],
    CStdStubBuffer_DELEGATING_METHODS
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
    0x50002, /* Ndr library version */
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

const CInterfaceProxyVtbl * _ThumbExtract_ProxyVtblList[] = 
{
    ( CInterfaceProxyVtbl *) &_IFileThumbExtractProxyVtbl,
    0
};

const CInterfaceStubVtbl * _ThumbExtract_StubVtblList[] = 
{
    ( CInterfaceStubVtbl *) &_IFileThumbExtractStubVtbl,
    0
};

PCInterfaceName const _ThumbExtract_InterfaceNamesList[] = 
{
    "IFileThumbExtract",
    0
};

const IID *  _ThumbExtract_BaseIIDList[] = 
{
    &IID_IDispatch,
    0
};


#define _ThumbExtract_CHECK_IID(n)	IID_GENERIC_CHECK_IID( _ThumbExtract, pIID, n)

int __stdcall _ThumbExtract_IID_Lookup( const IID * pIID, int * pIndex )
{
    
    if(!_ThumbExtract_CHECK_IID(0))
        {
        *pIndex = 0;
        return 1;
        }

    return 0;
}

const ExtendedProxyFileInfo ThumbExtract_ProxyFileInfo = 
{
    (PCInterfaceProxyVtblList *) & _ThumbExtract_ProxyVtblList,
    (PCInterfaceStubVtblList *) & _ThumbExtract_StubVtblList,
    (const PCInterfaceName * ) & _ThumbExtract_InterfaceNamesList,
    (const IID ** ) & _ThumbExtract_BaseIIDList,
    & _ThumbExtract_IID_Lookup, 
    1,
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

