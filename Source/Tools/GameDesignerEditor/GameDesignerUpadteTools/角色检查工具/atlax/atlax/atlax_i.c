

/* this ALWAYS GENERATED file contains the IIDs and CLSIDs */

/* link this file in with the server and any clients */


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

MIDL_DEFINE_GUID(IID, IID_ITest3DEngine,0x6AE8174C,0x9A77,0x4091,0x8F,0xC2,0x68,0xA3,0x90,0x5F,0x1D,0x81);


MIDL_DEFINE_GUID(IID, LIBID_atlaxLib,0x6F9FD12F,0x5554,0x49D7,0xB1,0x44,0x9F,0x52,0xB8,0x12,0xD8,0x4D);


MIDL_DEFINE_GUID(IID, DIID__ITest3DEngineEvents,0xF63BB8BC,0x6ACD,0x4271,0x90,0x0E,0xFA,0x07,0xF9,0xFB,0x89,0x43);


MIDL_DEFINE_GUID(CLSID, CLSID_Test3DEngine,0xD159A323,0xD278,0x475B,0xB5,0x56,0x57,0xC0,0x64,0x88,0x4B,0x9B);

#undef MIDL_DEFINE_GUID

#ifdef __cplusplus
}
#endif



