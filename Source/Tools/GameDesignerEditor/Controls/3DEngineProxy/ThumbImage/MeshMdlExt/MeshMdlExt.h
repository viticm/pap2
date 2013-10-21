

/* this ALWAYS GENERATED file contains the definitions for the interfaces */


 /* File created by MIDL compiler version 6.00.0366 */
/* at Tue Dec 22 09:38:10 2009
 */
/* Compiler settings for .\MeshMdlExt.idl:
    Oicf, W1, Zp8, env=Win32 (32b run)
    protocol : dce , ms_ext, c_ext
    error checks: allocation ref bounds_check enum stub_data 
    VC __declspec() decoration level: 
         __declspec(uuid()), __declspec(selectany), __declspec(novtable)
         DECLSPEC_UUID(), MIDL_INTERFACE()
*/
//@@MIDL_FILE_HEADING(  )

#pragma warning( disable: 4049 )  /* more than 64k source lines */


/* verify that the <rpcndr.h> version is high enough to compile this file*/
#ifndef __REQUIRED_RPCNDR_H_VERSION__
#define __REQUIRED_RPCNDR_H_VERSION__ 440
#endif

#include "rpc.h"
#include "rpcndr.h"

#ifndef __RPCNDR_H_VERSION__
#error this stub requires an updated version of <rpcndr.h>
#endif // __RPCNDR_H_VERSION__

#ifndef COM_NO_WINDOWS_H
#include "windows.h"
#include "ole2.h"
#endif /*COM_NO_WINDOWS_H*/

#ifndef __MeshMdlExt_h__
#define __MeshMdlExt_h__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

/* Forward Declarations */ 

#ifndef __IMeshMdlShlExt_FWD_DEFINED__
#define __IMeshMdlShlExt_FWD_DEFINED__
typedef interface IMeshMdlShlExt IMeshMdlShlExt;
#endif 	/* __IMeshMdlShlExt_FWD_DEFINED__ */


#ifndef __MeshMdlShlExt_FWD_DEFINED__
#define __MeshMdlShlExt_FWD_DEFINED__

#ifdef __cplusplus
typedef class MeshMdlShlExt MeshMdlShlExt;
#else
typedef struct MeshMdlShlExt MeshMdlShlExt;
#endif /* __cplusplus */

#endif 	/* __MeshMdlShlExt_FWD_DEFINED__ */


/* header files for imported files */
#include "oaidl.h"
#include "ocidl.h"

#ifdef __cplusplus
extern "C"{
#endif 

void * __RPC_USER MIDL_user_allocate(size_t);
void __RPC_USER MIDL_user_free( void * ); 

#ifndef __IMeshMdlShlExt_INTERFACE_DEFINED__
#define __IMeshMdlShlExt_INTERFACE_DEFINED__

/* interface IMeshMdlShlExt */
/* [unique][helpstring][uuid][object] */ 


EXTERN_C const IID IID_IMeshMdlShlExt;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("51BD6BAE-9464-4D9C-B614-32D2615287A7")
    IMeshMdlShlExt : public IUnknown
    {
    public:
    };
    
#else 	/* C style interface */

    typedef struct IMeshMdlShlExtVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IMeshMdlShlExt * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IMeshMdlShlExt * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IMeshMdlShlExt * This);
        
        END_INTERFACE
    } IMeshMdlShlExtVtbl;

    interface IMeshMdlShlExt
    {
        CONST_VTBL struct IMeshMdlShlExtVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IMeshMdlShlExt_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IMeshMdlShlExt_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IMeshMdlShlExt_Release(This)	\
    (This)->lpVtbl -> Release(This)


#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IMeshMdlShlExt_INTERFACE_DEFINED__ */



#ifndef __MeshMdlExtLib_LIBRARY_DEFINED__
#define __MeshMdlExtLib_LIBRARY_DEFINED__

/* library MeshMdlExtLib */
/* [helpstring][version][uuid] */ 


EXTERN_C const IID LIBID_MeshMdlExtLib;

EXTERN_C const CLSID CLSID_MeshMdlShlExt;

#ifdef __cplusplus

class DECLSPEC_UUID("F2F594C3-33F2-47D1-B035-61F88F803033")
MeshMdlShlExt;
#endif
#endif /* __MeshMdlExtLib_LIBRARY_DEFINED__ */

/* Additional Prototypes for ALL interfaces */

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif


