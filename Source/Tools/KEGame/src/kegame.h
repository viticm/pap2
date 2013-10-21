

/* this ALWAYS GENERATED file contains the definitions for the interfaces */


 /* File created by MIDL compiler version 6.00.0366 */
/* at Thu Sep 18 13:23:57 2008
 */
/* Compiler settings for .\kegame.idl:
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

#ifndef __kegame_h__
#define __kegame_h__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

/* Forward Declarations */ 

#ifndef __IGame_FWD_DEFINED__
#define __IGame_FWD_DEFINED__
typedef interface IGame IGame;
#endif 	/* __IGame_FWD_DEFINED__ */


#ifndef ___IGameEvents_FWD_DEFINED__
#define ___IGameEvents_FWD_DEFINED__
typedef interface _IGameEvents _IGameEvents;
#endif 	/* ___IGameEvents_FWD_DEFINED__ */


#ifndef __Game_FWD_DEFINED__
#define __Game_FWD_DEFINED__

#ifdef __cplusplus
typedef class Game Game;
#else
typedef struct Game Game;
#endif /* __cplusplus */

#endif 	/* __Game_FWD_DEFINED__ */


/* header files for imported files */
#include "oaidl.h"
#include "ocidl.h"

#ifdef __cplusplus
extern "C"{
#endif 

void * __RPC_USER MIDL_user_allocate(size_t);
void __RPC_USER MIDL_user_free( void * ); 

#ifndef __IGame_INTERFACE_DEFINED__
#define __IGame_INTERFACE_DEFINED__

/* interface IGame */
/* [unique][helpstring][nonextensible][dual][uuid][object] */ 


EXTERN_C const IID IID_IGame;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("57C7866D-BD22-4BA3-9F96-DD34FA82C6AF")
    IGame : public IDispatch
    {
    public:
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetSystemVersion( 
            /* [retval][out] */ BSTR *bstrVersion) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetSystemMemInfo( 
            /* [retval][out] */ BSTR *bstrMem) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetSystemCpuInfo( 
            /* [retval][out] */ BSTR *bstrCpu) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetDisplayInfo( 
            /* [retval][out] */ BSTR *bstrDisplay) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetHardDiskId( 
            /* [retval][out] */ BSTR *bstrHardId) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE EncryptString( 
            /* [in] */ BSTR bstrSrc,
            /* [retval][out] */ BSTR *bstrDest) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IGameVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IGame * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IGame * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IGame * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IGame * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IGame * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IGame * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IGame * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetSystemVersion )( 
            IGame * This,
            /* [retval][out] */ BSTR *bstrVersion);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetSystemMemInfo )( 
            IGame * This,
            /* [retval][out] */ BSTR *bstrMem);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetSystemCpuInfo )( 
            IGame * This,
            /* [retval][out] */ BSTR *bstrCpu);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetDisplayInfo )( 
            IGame * This,
            /* [retval][out] */ BSTR *bstrDisplay);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetHardDiskId )( 
            IGame * This,
            /* [retval][out] */ BSTR *bstrHardId);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *EncryptString )( 
            IGame * This,
            /* [in] */ BSTR bstrSrc,
            /* [retval][out] */ BSTR *bstrDest);
        
        END_INTERFACE
    } IGameVtbl;

    interface IGame
    {
        CONST_VTBL struct IGameVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IGame_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IGame_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IGame_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IGame_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IGame_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IGame_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IGame_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IGame_GetSystemVersion(This,bstrVersion)	\
    (This)->lpVtbl -> GetSystemVersion(This,bstrVersion)

#define IGame_GetSystemMemInfo(This,bstrMem)	\
    (This)->lpVtbl -> GetSystemMemInfo(This,bstrMem)

#define IGame_GetSystemCpuInfo(This,bstrCpu)	\
    (This)->lpVtbl -> GetSystemCpuInfo(This,bstrCpu)

#define IGame_GetDisplayInfo(This,bstrDisplay)	\
    (This)->lpVtbl -> GetDisplayInfo(This,bstrDisplay)

#define IGame_GetHardDiskId(This,bstrHardId)	\
    (This)->lpVtbl -> GetHardDiskId(This,bstrHardId)

#define IGame_EncryptString(This,bstrSrc,bstrDest)	\
    (This)->lpVtbl -> EncryptString(This,bstrSrc,bstrDest)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IGame_GetSystemVersion_Proxy( 
    IGame * This,
    /* [retval][out] */ BSTR *bstrVersion);


void __RPC_STUB IGame_GetSystemVersion_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IGame_GetSystemMemInfo_Proxy( 
    IGame * This,
    /* [retval][out] */ BSTR *bstrMem);


void __RPC_STUB IGame_GetSystemMemInfo_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IGame_GetSystemCpuInfo_Proxy( 
    IGame * This,
    /* [retval][out] */ BSTR *bstrCpu);


void __RPC_STUB IGame_GetSystemCpuInfo_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IGame_GetDisplayInfo_Proxy( 
    IGame * This,
    /* [retval][out] */ BSTR *bstrDisplay);


void __RPC_STUB IGame_GetDisplayInfo_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IGame_GetHardDiskId_Proxy( 
    IGame * This,
    /* [retval][out] */ BSTR *bstrHardId);


void __RPC_STUB IGame_GetHardDiskId_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IGame_EncryptString_Proxy( 
    IGame * This,
    /* [in] */ BSTR bstrSrc,
    /* [retval][out] */ BSTR *bstrDest);


void __RPC_STUB IGame_EncryptString_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IGame_INTERFACE_DEFINED__ */



#ifndef __kegameLib_LIBRARY_DEFINED__
#define __kegameLib_LIBRARY_DEFINED__

/* library kegameLib */
/* [helpstring][version][uuid] */ 


EXTERN_C const IID LIBID_kegameLib;

#ifndef ___IGameEvents_DISPINTERFACE_DEFINED__
#define ___IGameEvents_DISPINTERFACE_DEFINED__

/* dispinterface _IGameEvents */
/* [helpstring][uuid] */ 


EXTERN_C const IID DIID__IGameEvents;

#if defined(__cplusplus) && !defined(CINTERFACE)

    MIDL_INTERFACE("3EA707FF-C213-4248-AA20-0F1DCF65343D")
    _IGameEvents : public IDispatch
    {
    };
    
#else 	/* C style interface */

    typedef struct _IGameEventsVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            _IGameEvents * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            _IGameEvents * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            _IGameEvents * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            _IGameEvents * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            _IGameEvents * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            _IGameEvents * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            _IGameEvents * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        END_INTERFACE
    } _IGameEventsVtbl;

    interface _IGameEvents
    {
        CONST_VTBL struct _IGameEventsVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define _IGameEvents_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define _IGameEvents_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define _IGameEvents_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define _IGameEvents_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define _IGameEvents_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define _IGameEvents_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define _IGameEvents_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)

#endif /* COBJMACROS */


#endif 	/* C style interface */


#endif 	/* ___IGameEvents_DISPINTERFACE_DEFINED__ */


EXTERN_C const CLSID CLSID_Game;

#ifdef __cplusplus

class DECLSPEC_UUID("19CC8AC3-7B26-40FF-8389-AB2460E647A9")
Game;
#endif
#endif /* __kegameLib_LIBRARY_DEFINED__ */

/* Additional Prototypes for ALL interfaces */

unsigned long             __RPC_USER  BSTR_UserSize(     unsigned long *, unsigned long            , BSTR * ); 
unsigned char * __RPC_USER  BSTR_UserMarshal(  unsigned long *, unsigned char *, BSTR * ); 
unsigned char * __RPC_USER  BSTR_UserUnmarshal(unsigned long *, unsigned char *, BSTR * ); 
void                      __RPC_USER  BSTR_UserFree(     unsigned long *, BSTR * ); 

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif


