

/* this ALWAYS GENERATED file contains the IIDs and CLSIDs */

/* link this file in with the server and any clients */


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

#pragma warning( disable: 4049 )  /* more than 64k source lines */


#ifdef __cplusplus
extern "C"{
#endif 


#include <rpc.h>
#include <rpcndr.h>

#ifdef _MIDL_USE_GUIDDEF_

#ifndef INITGUID
#define INITGUID
#include <guiddef.h>
#undef INITGUID
#else
#include <guiddef.h>
#endif

#define MIDL_DEFINE_GUID(type,name,l,w1,w2,b1,b2,b3,b4,b5,b6,b7,b8) \
        DEFINE_GUID(name,l,w1,w2,b1,b2,b3,b4,b5,b6,b7,b8)

#else // !_MIDL_USE_GUIDDEF_

#ifndef __IID_DEFINED__
#define __IID_DEFINED__

typedef struct _IID
{
    unsigned long x;
    unsigned short s1;
    unsigned short s2;
    unsigned char  c[8];
} IID;

#endif // __IID_DEFINED__

#ifndef CLSID_DEFINED
#define CLSID_DEFINED
typedef IID CLSID;
#endif // CLSID_DEFINED

#define MIDL_DEFINE_GUID(type,name,l,w1,w2,b1,b2,b3,b4,b5,b6,b7,b8) \
        const type name = {l,w1,w2,{b1,b2,b3,b4,b5,b6,b7,b8}}

#endif !_MIDL_USE_GUIDDEF_

MIDL_DEFINE_GUID(IID, IID_IBaseLib,0xDCA6998A,0x7600,0x481F,0x90,0xBB,0x5E,0xCB,0x27,0xFA,0x2B,0x12);


MIDL_DEFINE_GUID(IID, IID_IRecordPlayer,0x18A7E96F,0xA8C0,0x4335,0x87,0xA5,0xF7,0x3A,0xF7,0xA8,0x88,0xF6);


MIDL_DEFINE_GUID(IID, LIBID_AtlKGRecordeOperatorLib,0x1EFD303F,0xCA43,0x4418,0x8B,0xCA,0x5E,0x18,0xF5,0x79,0x3F,0x0E);


MIDL_DEFINE_GUID(CLSID, CLSID_BaseLib,0x6951EF60,0xC91E,0x408D,0xB4,0x68,0x4F,0x62,0xF3,0xC6,0x7A,0x81);


MIDL_DEFINE_GUID(CLSID, CLSID_RecordPlayer,0x3D5D9834,0xA2BA,0x419F,0xA1,0x1B,0x7D,0x69,0x55,0x18,0xD9,0x54);

#undef MIDL_DEFINE_GUID

#ifdef __cplusplus
}
#endif



