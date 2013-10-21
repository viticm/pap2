

/* this ALWAYS GENERATED file contains the proxy stub code */


 /* File created by MIDL compiler version 6.00.0366 */
/* at Thu May 10 11:13:04 2007
 */
/* Compiler settings for .\atlax.idl:
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


#include "atlax.h"

#define TYPE_FORMAT_STRING_SIZE   113                               
#define PROC_FORMAT_STRING_SIZE   533                               
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


extern const MIDL_SERVER_INFO ITest3DEngine_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO ITest3DEngine_ProxyInfo;


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
/*  6 */	NdrFcShort( 0x7 ),	/* 7 */
/*  8 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 10 */	NdrFcShort( 0x8 ),	/* 8 */
/* 12 */	NdrFcShort( 0x8 ),	/* 8 */
/* 14 */	0x4,		/* Oi2 Flags:  has return, */
			0x2,		/* 2 */

	/* Parameter hWnd */

/* 16 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 18 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 20 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 22 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 24 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 26 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure Render */

/* 28 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 30 */	NdrFcLong( 0x0 ),	/* 0 */
/* 34 */	NdrFcShort( 0x8 ),	/* 8 */
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

	/* Procedure FrameMove */

/* 50 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 52 */	NdrFcLong( 0x0 ),	/* 0 */
/* 56 */	NdrFcShort( 0x9 ),	/* 9 */
/* 58 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 60 */	NdrFcShort( 0x0 ),	/* 0 */
/* 62 */	NdrFcShort( 0x8 ),	/* 8 */
/* 64 */	0x4,		/* Oi2 Flags:  has return, */
			0x1,		/* 1 */

	/* Return value */

/* 66 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 68 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 70 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure UnInit */

/* 72 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 74 */	NdrFcLong( 0x0 ),	/* 0 */
/* 78 */	NdrFcShort( 0xa ),	/* 10 */
/* 80 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 82 */	NdrFcShort( 0x0 ),	/* 0 */
/* 84 */	NdrFcShort( 0x8 ),	/* 8 */
/* 86 */	0x4,		/* Oi2 Flags:  has return, */
			0x1,		/* 1 */

	/* Return value */

/* 88 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 90 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 92 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure OnResizeWindow */

/* 94 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 96 */	NdrFcLong( 0x0 ),	/* 0 */
/* 100 */	NdrFcShort( 0xb ),	/* 11 */
/* 102 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 104 */	NdrFcShort( 0x0 ),	/* 0 */
/* 106 */	NdrFcShort( 0x8 ),	/* 8 */
/* 108 */	0x4,		/* Oi2 Flags:  has return, */
			0x1,		/* 1 */

	/* Return value */

/* 110 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 112 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 114 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure get_PlayMode */

/* 116 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 118 */	NdrFcLong( 0x0 ),	/* 0 */
/* 122 */	NdrFcShort( 0xc ),	/* 12 */
/* 124 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 126 */	NdrFcShort( 0x0 ),	/* 0 */
/* 128 */	NdrFcShort( 0x22 ),	/* 34 */
/* 130 */	0x4,		/* Oi2 Flags:  has return, */
			0x2,		/* 2 */

	/* Parameter pVal */

/* 132 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 134 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 136 */	0x6,		/* FC_SHORT */
			0x0,		/* 0 */

	/* Return value */

/* 138 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 140 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 142 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure put_PlayMode */

/* 144 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 146 */	NdrFcLong( 0x0 ),	/* 0 */
/* 150 */	NdrFcShort( 0xd ),	/* 13 */
/* 152 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 154 */	NdrFcShort( 0x6 ),	/* 6 */
/* 156 */	NdrFcShort( 0x8 ),	/* 8 */
/* 158 */	0x4,		/* Oi2 Flags:  has return, */
			0x2,		/* 2 */

	/* Parameter newVal */

/* 160 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 162 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 164 */	0x6,		/* FC_SHORT */
			0x0,		/* 0 */

	/* Return value */

/* 166 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 168 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 170 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure OnWndMsg */

/* 172 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 174 */	NdrFcLong( 0x0 ),	/* 0 */
/* 178 */	NdrFcShort( 0xe ),	/* 14 */
/* 180 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 182 */	NdrFcShort( 0x0 ),	/* 0 */
/* 184 */	NdrFcShort( 0x24 ),	/* 36 */
/* 186 */	0x6,		/* Oi2 Flags:  clt must size, has return, */
			0x3,		/* 3 */

	/* Parameter WndMsg */

/* 188 */	NdrFcShort( 0x10b ),	/* Flags:  must size, must free, in, simple ref, */
/* 190 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 192 */	NdrFcShort( 0x34 ),	/* Type Offset=52 */

	/* Parameter pnRet */

/* 194 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 196 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 198 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 200 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 202 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 204 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure LoadFile */

/* 206 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 208 */	NdrFcLong( 0x0 ),	/* 0 */
/* 212 */	NdrFcShort( 0xf ),	/* 15 */
/* 214 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 216 */	NdrFcShort( 0x0 ),	/* 0 */
/* 218 */	NdrFcShort( 0x8 ),	/* 8 */
/* 220 */	0x6,		/* Oi2 Flags:  clt must size, has return, */
			0x2,		/* 2 */

	/* Parameter strFileName */

/* 222 */	NdrFcShort( 0x8b ),	/* Flags:  must size, must free, in, by val, */
/* 224 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 226 */	NdrFcShort( 0x66 ),	/* Type Offset=102 */

	/* Return value */

/* 228 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 230 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 232 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure UnloadFile */

/* 234 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 236 */	NdrFcLong( 0x0 ),	/* 0 */
/* 240 */	NdrFcShort( 0x10 ),	/* 16 */
/* 242 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 244 */	NdrFcShort( 0x0 ),	/* 0 */
/* 246 */	NdrFcShort( 0x8 ),	/* 8 */
/* 248 */	0x6,		/* Oi2 Flags:  clt must size, has return, */
			0x2,		/* 2 */

	/* Parameter strFileName */

/* 250 */	NdrFcShort( 0x8b ),	/* Flags:  must size, must free, in, by val, */
/* 252 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 254 */	NdrFcShort( 0x66 ),	/* Type Offset=102 */

	/* Return value */

/* 256 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 258 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 260 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure LoadPlayerModel */

/* 262 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 264 */	NdrFcLong( 0x0 ),	/* 0 */
/* 268 */	NdrFcShort( 0x11 ),	/* 17 */
/* 270 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 272 */	NdrFcShort( 0x0 ),	/* 0 */
/* 274 */	NdrFcShort( 0x8 ),	/* 8 */
/* 276 */	0x6,		/* Oi2 Flags:  clt must size, has return, */
			0x2,		/* 2 */

	/* Parameter strFileName */

/* 278 */	NdrFcShort( 0x8b ),	/* Flags:  must size, must free, in, by val, */
/* 280 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 282 */	NdrFcShort( 0x66 ),	/* Type Offset=102 */

	/* Return value */

/* 284 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 286 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 288 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure BindToPlayer */

/* 290 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 292 */	NdrFcLong( 0x0 ),	/* 0 */
/* 296 */	NdrFcShort( 0x12 ),	/* 18 */
/* 298 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 300 */	NdrFcShort( 0x8 ),	/* 8 */
/* 302 */	NdrFcShort( 0x8 ),	/* 8 */
/* 304 */	0x6,		/* Oi2 Flags:  clt must size, has return, */
			0x4,		/* 4 */

	/* Parameter strFileName */

/* 306 */	NdrFcShort( 0x8b ),	/* Flags:  must size, must free, in, by val, */
/* 308 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 310 */	NdrFcShort( 0x66 ),	/* Type Offset=102 */

	/* Parameter dwType */

/* 312 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 314 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 316 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter strBindToObj */

/* 318 */	NdrFcShort( 0x8b ),	/* Flags:  must size, must free, in, by val, */
/* 320 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 322 */	NdrFcShort( 0x66 ),	/* Type Offset=102 */

	/* Return value */

/* 324 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 326 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 328 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure LoadAni */

/* 330 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 332 */	NdrFcLong( 0x0 ),	/* 0 */
/* 336 */	NdrFcShort( 0x13 ),	/* 19 */
/* 338 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 340 */	NdrFcShort( 0x0 ),	/* 0 */
/* 342 */	NdrFcShort( 0x8 ),	/* 8 */
/* 344 */	0x6,		/* Oi2 Flags:  clt must size, has return, */
			0x2,		/* 2 */

	/* Parameter strFileName */

/* 346 */	NdrFcShort( 0x8b ),	/* Flags:  must size, must free, in, by val, */
/* 348 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 350 */	NdrFcShort( 0x66 ),	/* Type Offset=102 */

	/* Return value */

/* 352 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 354 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 356 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure ZoomView */

/* 358 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 360 */	NdrFcLong( 0x0 ),	/* 0 */
/* 364 */	NdrFcShort( 0x14 ),	/* 20 */
/* 366 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 368 */	NdrFcShort( 0x8 ),	/* 8 */
/* 370 */	NdrFcShort( 0x8 ),	/* 8 */
/* 372 */	0x4,		/* Oi2 Flags:  has return, */
			0x2,		/* 2 */

	/* Parameter lDelta */

/* 374 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 376 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 378 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 380 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 382 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 384 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure ChangeModelPart */

/* 386 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 388 */	NdrFcLong( 0x0 ),	/* 0 */
/* 392 */	NdrFcShort( 0x15 ),	/* 21 */
/* 394 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 396 */	NdrFcShort( 0x8 ),	/* 8 */
/* 398 */	NdrFcShort( 0x8 ),	/* 8 */
/* 400 */	0x6,		/* Oi2 Flags:  clt must size, has return, */
			0x3,		/* 3 */

	/* Parameter filename */

/* 402 */	NdrFcShort( 0x8b ),	/* Flags:  must size, must free, in, by val, */
/* 404 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 406 */	NdrFcShort( 0x66 ),	/* Type Offset=102 */

	/* Parameter PartID */

/* 408 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 410 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 412 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 414 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 416 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 418 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure LoadFace */

/* 420 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 422 */	NdrFcLong( 0x0 ),	/* 0 */
/* 426 */	NdrFcShort( 0x16 ),	/* 22 */
/* 428 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 430 */	NdrFcShort( 0x0 ),	/* 0 */
/* 432 */	NdrFcShort( 0x8 ),	/* 8 */
/* 434 */	0x6,		/* Oi2 Flags:  clt must size, has return, */
			0x2,		/* 2 */

	/* Parameter strFileName */

/* 436 */	NdrFcShort( 0x8b ),	/* Flags:  must size, must free, in, by val, */
/* 438 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 440 */	NdrFcShort( 0x66 ),	/* Type Offset=102 */

	/* Return value */

/* 442 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 444 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 446 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure LoadSocket */

/* 448 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 450 */	NdrFcLong( 0x0 ),	/* 0 */
/* 454 */	NdrFcShort( 0x17 ),	/* 23 */
/* 456 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 458 */	NdrFcShort( 0x0 ),	/* 0 */
/* 460 */	NdrFcShort( 0x8 ),	/* 8 */
/* 462 */	0x6,		/* Oi2 Flags:  clt must size, has return, */
			0x3,		/* 3 */

	/* Parameter strBindToObj */

/* 464 */	NdrFcShort( 0x8b ),	/* Flags:  must size, must free, in, by val, */
/* 466 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 468 */	NdrFcShort( 0x66 ),	/* Type Offset=102 */

	/* Parameter strFileName */

/* 470 */	NdrFcShort( 0x8b ),	/* Flags:  must size, must free, in, by val, */
/* 472 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 474 */	NdrFcShort( 0x66 ),	/* Type Offset=102 */

	/* Return value */

/* 476 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 478 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 480 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure ItemsClear */

/* 482 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 484 */	NdrFcLong( 0x0 ),	/* 0 */
/* 488 */	NdrFcShort( 0x18 ),	/* 24 */
/* 490 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 492 */	NdrFcShort( 0x0 ),	/* 0 */
/* 494 */	NdrFcShort( 0x8 ),	/* 8 */
/* 496 */	0x4,		/* Oi2 Flags:  has return, */
			0x1,		/* 1 */

	/* Return value */

/* 498 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 500 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 502 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure SaveMDLToFile */

/* 504 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 506 */	NdrFcLong( 0x0 ),	/* 0 */
/* 510 */	NdrFcShort( 0x19 ),	/* 25 */
/* 512 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 514 */	NdrFcShort( 0x0 ),	/* 0 */
/* 516 */	NdrFcShort( 0x8 ),	/* 8 */
/* 518 */	0x6,		/* Oi2 Flags:  clt must size, has return, */
			0x2,		/* 2 */

	/* Parameter strFileName */

/* 520 */	NdrFcShort( 0x8b ),	/* Flags:  must size, must free, in, by val, */
/* 522 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 524 */	NdrFcShort( 0x66 ),	/* Type Offset=102 */

	/* Return value */

/* 526 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 528 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 530 */	0x8,		/* FC_LONG */
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
/*  4 */	0x6,		/* FC_SHORT */
			0x5c,		/* FC_PAD */
/*  6 */	
			0x11, 0x0,	/* FC_RP */
/*  8 */	NdrFcShort( 0x2c ),	/* Offset= 44 (52) */
/* 10 */	
			0x12, 0x0,	/* FC_UP */
/* 12 */	NdrFcShort( 0x2 ),	/* Offset= 2 (14) */
/* 14 */	
			0x2a,		/* FC_ENCAPSULATED_UNION */
			0x48,		/* 72 */
/* 16 */	NdrFcShort( 0x4 ),	/* 4 */
/* 18 */	NdrFcShort( 0x2 ),	/* 2 */
/* 20 */	NdrFcLong( 0x48746457 ),	/* 1215587415 */
/* 24 */	NdrFcShort( 0x8008 ),	/* Simple arm type: FC_LONG */
/* 26 */	NdrFcLong( 0x52746457 ),	/* 1383359575 */
/* 30 */	NdrFcShort( 0x8008 ),	/* Simple arm type: FC_LONG */
/* 32 */	NdrFcShort( 0xffff ),	/* Offset= -1 (31) */
/* 34 */	0xb4,		/* FC_USER_MARSHAL */
			0x83,		/* 131 */
/* 36 */	NdrFcShort( 0x0 ),	/* 0 */
/* 38 */	NdrFcShort( 0x4 ),	/* 4 */
/* 40 */	NdrFcShort( 0x0 ),	/* 0 */
/* 42 */	NdrFcShort( 0xffe0 ),	/* Offset= -32 (10) */
/* 44 */	
			0x15,		/* FC_STRUCT */
			0x3,		/* 3 */
/* 46 */	NdrFcShort( 0x8 ),	/* 8 */
/* 48 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 50 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 52 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 54 */	NdrFcShort( 0x1c ),	/* 28 */
/* 56 */	NdrFcShort( 0x0 ),	/* 0 */
/* 58 */	NdrFcShort( 0x0 ),	/* Offset= 0 (58) */
/* 60 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 62 */	NdrFcShort( 0xffe4 ),	/* Offset= -28 (34) */
/* 64 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 66 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 68 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 70 */	NdrFcShort( 0xffe6 ),	/* Offset= -26 (44) */
/* 72 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 74 */	
			0x11, 0xc,	/* FC_RP [alloced_on_stack] [simple_pointer] */
/* 76 */	0x8,		/* FC_LONG */
			0x5c,		/* FC_PAD */
/* 78 */	
			0x12, 0x0,	/* FC_UP */
/* 80 */	NdrFcShort( 0xc ),	/* Offset= 12 (92) */
/* 82 */	
			0x1b,		/* FC_CARRAY */
			0x1,		/* 1 */
/* 84 */	NdrFcShort( 0x2 ),	/* 2 */
/* 86 */	0x9,		/* Corr desc: FC_ULONG */
			0x0,		/*  */
/* 88 */	NdrFcShort( 0xfffc ),	/* -4 */
/* 90 */	0x6,		/* FC_SHORT */
			0x5b,		/* FC_END */
/* 92 */	
			0x17,		/* FC_CSTRUCT */
			0x3,		/* 3 */
/* 94 */	NdrFcShort( 0x8 ),	/* 8 */
/* 96 */	NdrFcShort( 0xfff2 ),	/* Offset= -14 (82) */
/* 98 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 100 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 102 */	0xb4,		/* FC_USER_MARSHAL */
			0x83,		/* 131 */
/* 104 */	NdrFcShort( 0x1 ),	/* 1 */
/* 106 */	NdrFcShort( 0x4 ),	/* 4 */
/* 108 */	NdrFcShort( 0x0 ),	/* 0 */
/* 110 */	NdrFcShort( 0xffe0 ),	/* Offset= -32 (78) */

			0x0
        }
    };

static const USER_MARSHAL_ROUTINE_QUADRUPLE UserMarshalRoutines[ WIRE_MARSHAL_TABLE_SIZE ] = 
        {
            
            {
            HWND_UserSize
            ,HWND_UserMarshal
            ,HWND_UserUnmarshal
            ,HWND_UserFree
            },
            {
            BSTR_UserSize
            ,BSTR_UserMarshal
            ,BSTR_UserUnmarshal
            ,BSTR_UserFree
            }

        };



/* Object interface: IUnknown, ver. 0.0,
   GUID={0x00000000,0x0000,0x0000,{0xC0,0x00,0x00,0x00,0x00,0x00,0x00,0x46}} */


/* Object interface: IDispatch, ver. 0.0,
   GUID={0x00020400,0x0000,0x0000,{0xC0,0x00,0x00,0x00,0x00,0x00,0x00,0x46}} */


/* Object interface: ITest3DEngine, ver. 0.0,
   GUID={0x6AE8174C,0x9A77,0x4091,{0x8F,0xC2,0x68,0xA3,0x90,0x5F,0x1D,0x81}} */

#pragma code_seg(".orpc")
static const unsigned short ITest3DEngine_FormatStringOffsetTable[] =
    {
    (unsigned short) -1,
    (unsigned short) -1,
    (unsigned short) -1,
    (unsigned short) -1,
    0,
    28,
    50,
    72,
    94,
    116,
    144,
    172,
    206,
    234,
    262,
    290,
    330,
    358,
    386,
    420,
    448,
    482,
    504
    };

static const MIDL_STUBLESS_PROXY_INFO ITest3DEngine_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &ITest3DEngine_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO ITest3DEngine_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &ITest3DEngine_FormatStringOffsetTable[-3],
    0,
    0,
    0,
    0};
CINTERFACE_PROXY_VTABLE(26) _ITest3DEngineProxyVtbl = 
{
    &ITest3DEngine_ProxyInfo,
    &IID_ITest3DEngine,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    0 /* (void *) (INT_PTR) -1 /* IDispatch::GetTypeInfoCount */ ,
    0 /* (void *) (INT_PTR) -1 /* IDispatch::GetTypeInfo */ ,
    0 /* (void *) (INT_PTR) -1 /* IDispatch::GetIDsOfNames */ ,
    0 /* IDispatch_Invoke_Proxy */ ,
    (void *) (INT_PTR) -1 /* ITest3DEngine::Init */ ,
    (void *) (INT_PTR) -1 /* ITest3DEngine::Render */ ,
    (void *) (INT_PTR) -1 /* ITest3DEngine::FrameMove */ ,
    (void *) (INT_PTR) -1 /* ITest3DEngine::UnInit */ ,
    (void *) (INT_PTR) -1 /* ITest3DEngine::OnResizeWindow */ ,
    (void *) (INT_PTR) -1 /* ITest3DEngine::get_PlayMode */ ,
    (void *) (INT_PTR) -1 /* ITest3DEngine::put_PlayMode */ ,
    (void *) (INT_PTR) -1 /* ITest3DEngine::OnWndMsg */ ,
    (void *) (INT_PTR) -1 /* ITest3DEngine::LoadFile */ ,
    (void *) (INT_PTR) -1 /* ITest3DEngine::UnloadFile */ ,
    (void *) (INT_PTR) -1 /* ITest3DEngine::LoadPlayerModel */ ,
    (void *) (INT_PTR) -1 /* ITest3DEngine::BindToPlayer */ ,
    (void *) (INT_PTR) -1 /* ITest3DEngine::LoadAni */ ,
    (void *) (INT_PTR) -1 /* ITest3DEngine::ZoomView */ ,
    (void *) (INT_PTR) -1 /* ITest3DEngine::ChangeModelPart */ ,
    (void *) (INT_PTR) -1 /* ITest3DEngine::LoadFace */ ,
    (void *) (INT_PTR) -1 /* ITest3DEngine::LoadSocket */ ,
    (void *) (INT_PTR) -1 /* ITest3DEngine::ItemsClear */ ,
    (void *) (INT_PTR) -1 /* ITest3DEngine::SaveMDLToFile */
};


static const PRPC_STUB_FUNCTION ITest3DEngine_table[] =
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
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2
};

CInterfaceStubVtbl _ITest3DEngineStubVtbl =
{
    &IID_ITest3DEngine,
    &ITest3DEngine_ServerInfo,
    26,
    &ITest3DEngine_table[-3],
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

const CInterfaceProxyVtbl * _atlax_ProxyVtblList[] = 
{
    ( CInterfaceProxyVtbl *) &_ITest3DEngineProxyVtbl,
    0
};

const CInterfaceStubVtbl * _atlax_StubVtblList[] = 
{
    ( CInterfaceStubVtbl *) &_ITest3DEngineStubVtbl,
    0
};

PCInterfaceName const _atlax_InterfaceNamesList[] = 
{
    "ITest3DEngine",
    0
};

const IID *  _atlax_BaseIIDList[] = 
{
    &IID_IDispatch,
    0
};


#define _atlax_CHECK_IID(n)	IID_GENERIC_CHECK_IID( _atlax, pIID, n)

int __stdcall _atlax_IID_Lookup( const IID * pIID, int * pIndex )
{
    
    if(!_atlax_CHECK_IID(0))
        {
        *pIndex = 0;
        return 1;
        }

    return 0;
}

const ExtendedProxyFileInfo atlax_ProxyFileInfo = 
{
    (PCInterfaceProxyVtblList *) & _atlax_ProxyVtblList,
    (PCInterfaceStubVtblList *) & _atlax_StubVtblList,
    (const PCInterfaceName * ) & _atlax_InterfaceNamesList,
    (const IID ** ) & _atlax_BaseIIDList,
    & _atlax_IID_Lookup, 
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

