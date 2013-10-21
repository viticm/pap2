

/* this ALWAYS GENERATED file contains the definitions for the interfaces */


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

#ifndef __atlax_h__
#define __atlax_h__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

/* Forward Declarations */ 

#ifndef __ITest3DEngine_FWD_DEFINED__
#define __ITest3DEngine_FWD_DEFINED__
typedef interface ITest3DEngine ITest3DEngine;
#endif 	/* __ITest3DEngine_FWD_DEFINED__ */


#ifndef ___ITest3DEngineEvents_FWD_DEFINED__
#define ___ITest3DEngineEvents_FWD_DEFINED__
typedef interface _ITest3DEngineEvents _ITest3DEngineEvents;
#endif 	/* ___ITest3DEngineEvents_FWD_DEFINED__ */


#ifndef __Test3DEngine_FWD_DEFINED__
#define __Test3DEngine_FWD_DEFINED__

#ifdef __cplusplus
typedef class Test3DEngine Test3DEngine;
#else
typedef struct Test3DEngine Test3DEngine;
#endif /* __cplusplus */

#endif 	/* __Test3DEngine_FWD_DEFINED__ */


/* header files for imported files */
#include "oaidl.h"
#include "ocidl.h"

#ifdef __cplusplus
extern "C"{
#endif 

void * __RPC_USER MIDL_user_allocate(size_t);
void __RPC_USER MIDL_user_free( void * ); 

#ifndef __ITest3DEngine_INTERFACE_DEFINED__
#define __ITest3DEngine_INTERFACE_DEFINED__

/* interface ITest3DEngine */
/* [unique][helpstring][nonextensible][dual][uuid][object] */ 


EXTERN_C const IID IID_ITest3DEngine;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("6AE8174C-9A77-4091-8FC2-68A3905F1D81")
    ITest3DEngine : public IDispatch
    {
    public:
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Init( 
            LONG hWnd) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Render( void) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE FrameMove( void) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE UnInit( void) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE OnResizeWindow( void) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_PlayMode( 
            /* [retval][out] */ VARIANT_BOOL *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_PlayMode( 
            /* [in] */ VARIANT_BOOL newVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE OnWndMsg( 
            /* [in] */ PMSG WndMsg,
            /* [retval][out] */ INT *pnRet) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE LoadFile( 
            BSTR strFileName) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE UnloadFile( 
            BSTR strFileName) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE LoadPlayerModel( 
            BSTR strFileName) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE BindToPlayer( 
            BSTR strFileName,
            DWORD dwType,
            BSTR strBindToObj) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE LoadAni( 
            BSTR strFileName) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ZoomView( 
            LONG lDelta) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ChangeModelPart( 
            BSTR filename,
            INT PartID) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE LoadFace( 
            BSTR strFileName) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE LoadSocket( 
            BSTR strBindToObj,
            BSTR strFileName) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ItemsClear( void) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SaveMDLToFile( 
            BSTR strFileName) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct ITest3DEngineVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            ITest3DEngine * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            ITest3DEngine * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            ITest3DEngine * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            ITest3DEngine * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            ITest3DEngine * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            ITest3DEngine * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            ITest3DEngine * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *Init )( 
            ITest3DEngine * This,
            LONG hWnd);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *Render )( 
            ITest3DEngine * This);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *FrameMove )( 
            ITest3DEngine * This);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *UnInit )( 
            ITest3DEngine * This);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *OnResizeWindow )( 
            ITest3DEngine * This);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_PlayMode )( 
            ITest3DEngine * This,
            /* [retval][out] */ VARIANT_BOOL *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_PlayMode )( 
            ITest3DEngine * This,
            /* [in] */ VARIANT_BOOL newVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *OnWndMsg )( 
            ITest3DEngine * This,
            /* [in] */ PMSG WndMsg,
            /* [retval][out] */ INT *pnRet);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *LoadFile )( 
            ITest3DEngine * This,
            BSTR strFileName);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *UnloadFile )( 
            ITest3DEngine * This,
            BSTR strFileName);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *LoadPlayerModel )( 
            ITest3DEngine * This,
            BSTR strFileName);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *BindToPlayer )( 
            ITest3DEngine * This,
            BSTR strFileName,
            DWORD dwType,
            BSTR strBindToObj);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *LoadAni )( 
            ITest3DEngine * This,
            BSTR strFileName);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *ZoomView )( 
            ITest3DEngine * This,
            LONG lDelta);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *ChangeModelPart )( 
            ITest3DEngine * This,
            BSTR filename,
            INT PartID);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *LoadFace )( 
            ITest3DEngine * This,
            BSTR strFileName);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *LoadSocket )( 
            ITest3DEngine * This,
            BSTR strBindToObj,
            BSTR strFileName);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *ItemsClear )( 
            ITest3DEngine * This);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SaveMDLToFile )( 
            ITest3DEngine * This,
            BSTR strFileName);
        
        END_INTERFACE
    } ITest3DEngineVtbl;

    interface ITest3DEngine
    {
        CONST_VTBL struct ITest3DEngineVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define ITest3DEngine_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define ITest3DEngine_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define ITest3DEngine_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define ITest3DEngine_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define ITest3DEngine_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define ITest3DEngine_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define ITest3DEngine_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define ITest3DEngine_Init(This,hWnd)	\
    (This)->lpVtbl -> Init(This,hWnd)

#define ITest3DEngine_Render(This)	\
    (This)->lpVtbl -> Render(This)

#define ITest3DEngine_FrameMove(This)	\
    (This)->lpVtbl -> FrameMove(This)

#define ITest3DEngine_UnInit(This)	\
    (This)->lpVtbl -> UnInit(This)

#define ITest3DEngine_OnResizeWindow(This)	\
    (This)->lpVtbl -> OnResizeWindow(This)

#define ITest3DEngine_get_PlayMode(This,pVal)	\
    (This)->lpVtbl -> get_PlayMode(This,pVal)

#define ITest3DEngine_put_PlayMode(This,newVal)	\
    (This)->lpVtbl -> put_PlayMode(This,newVal)

#define ITest3DEngine_OnWndMsg(This,WndMsg,pnRet)	\
    (This)->lpVtbl -> OnWndMsg(This,WndMsg,pnRet)

#define ITest3DEngine_LoadFile(This,strFileName)	\
    (This)->lpVtbl -> LoadFile(This,strFileName)

#define ITest3DEngine_UnloadFile(This,strFileName)	\
    (This)->lpVtbl -> UnloadFile(This,strFileName)

#define ITest3DEngine_LoadPlayerModel(This,strFileName)	\
    (This)->lpVtbl -> LoadPlayerModel(This,strFileName)

#define ITest3DEngine_BindToPlayer(This,strFileName,dwType,strBindToObj)	\
    (This)->lpVtbl -> BindToPlayer(This,strFileName,dwType,strBindToObj)

#define ITest3DEngine_LoadAni(This,strFileName)	\
    (This)->lpVtbl -> LoadAni(This,strFileName)

#define ITest3DEngine_ZoomView(This,lDelta)	\
    (This)->lpVtbl -> ZoomView(This,lDelta)

#define ITest3DEngine_ChangeModelPart(This,filename,PartID)	\
    (This)->lpVtbl -> ChangeModelPart(This,filename,PartID)

#define ITest3DEngine_LoadFace(This,strFileName)	\
    (This)->lpVtbl -> LoadFace(This,strFileName)

#define ITest3DEngine_LoadSocket(This,strBindToObj,strFileName)	\
    (This)->lpVtbl -> LoadSocket(This,strBindToObj,strFileName)

#define ITest3DEngine_ItemsClear(This)	\
    (This)->lpVtbl -> ItemsClear(This)

#define ITest3DEngine_SaveMDLToFile(This,strFileName)	\
    (This)->lpVtbl -> SaveMDLToFile(This,strFileName)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ITest3DEngine_Init_Proxy( 
    ITest3DEngine * This,
    LONG hWnd);


void __RPC_STUB ITest3DEngine_Init_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ITest3DEngine_Render_Proxy( 
    ITest3DEngine * This);


void __RPC_STUB ITest3DEngine_Render_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ITest3DEngine_FrameMove_Proxy( 
    ITest3DEngine * This);


void __RPC_STUB ITest3DEngine_FrameMove_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ITest3DEngine_UnInit_Proxy( 
    ITest3DEngine * This);


void __RPC_STUB ITest3DEngine_UnInit_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ITest3DEngine_OnResizeWindow_Proxy( 
    ITest3DEngine * This);


void __RPC_STUB ITest3DEngine_OnResizeWindow_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE ITest3DEngine_get_PlayMode_Proxy( 
    ITest3DEngine * This,
    /* [retval][out] */ VARIANT_BOOL *pVal);


void __RPC_STUB ITest3DEngine_get_PlayMode_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE ITest3DEngine_put_PlayMode_Proxy( 
    ITest3DEngine * This,
    /* [in] */ VARIANT_BOOL newVal);


void __RPC_STUB ITest3DEngine_put_PlayMode_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ITest3DEngine_OnWndMsg_Proxy( 
    ITest3DEngine * This,
    /* [in] */ PMSG WndMsg,
    /* [retval][out] */ INT *pnRet);


void __RPC_STUB ITest3DEngine_OnWndMsg_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ITest3DEngine_LoadFile_Proxy( 
    ITest3DEngine * This,
    BSTR strFileName);


void __RPC_STUB ITest3DEngine_LoadFile_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ITest3DEngine_UnloadFile_Proxy( 
    ITest3DEngine * This,
    BSTR strFileName);


void __RPC_STUB ITest3DEngine_UnloadFile_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ITest3DEngine_LoadPlayerModel_Proxy( 
    ITest3DEngine * This,
    BSTR strFileName);


void __RPC_STUB ITest3DEngine_LoadPlayerModel_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ITest3DEngine_BindToPlayer_Proxy( 
    ITest3DEngine * This,
    BSTR strFileName,
    DWORD dwType,
    BSTR strBindToObj);


void __RPC_STUB ITest3DEngine_BindToPlayer_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ITest3DEngine_LoadAni_Proxy( 
    ITest3DEngine * This,
    BSTR strFileName);


void __RPC_STUB ITest3DEngine_LoadAni_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ITest3DEngine_ZoomView_Proxy( 
    ITest3DEngine * This,
    LONG lDelta);


void __RPC_STUB ITest3DEngine_ZoomView_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ITest3DEngine_ChangeModelPart_Proxy( 
    ITest3DEngine * This,
    BSTR filename,
    INT PartID);


void __RPC_STUB ITest3DEngine_ChangeModelPart_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ITest3DEngine_LoadFace_Proxy( 
    ITest3DEngine * This,
    BSTR strFileName);


void __RPC_STUB ITest3DEngine_LoadFace_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ITest3DEngine_LoadSocket_Proxy( 
    ITest3DEngine * This,
    BSTR strBindToObj,
    BSTR strFileName);


void __RPC_STUB ITest3DEngine_LoadSocket_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ITest3DEngine_ItemsClear_Proxy( 
    ITest3DEngine * This);


void __RPC_STUB ITest3DEngine_ItemsClear_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ITest3DEngine_SaveMDLToFile_Proxy( 
    ITest3DEngine * This,
    BSTR strFileName);


void __RPC_STUB ITest3DEngine_SaveMDLToFile_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __ITest3DEngine_INTERFACE_DEFINED__ */



#ifndef __atlaxLib_LIBRARY_DEFINED__
#define __atlaxLib_LIBRARY_DEFINED__

/* library atlaxLib */
/* [helpstring][version][uuid] */ 


EXTERN_C const IID LIBID_atlaxLib;

#ifndef ___ITest3DEngineEvents_DISPINTERFACE_DEFINED__
#define ___ITest3DEngineEvents_DISPINTERFACE_DEFINED__

/* dispinterface _ITest3DEngineEvents */
/* [helpstring][uuid] */ 


EXTERN_C const IID DIID__ITest3DEngineEvents;

#if defined(__cplusplus) && !defined(CINTERFACE)

    MIDL_INTERFACE("F63BB8BC-6ACD-4271-900E-FA07F9FB8943")
    _ITest3DEngineEvents : public IDispatch
    {
    };
    
#else 	/* C style interface */

    typedef struct _ITest3DEngineEventsVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            _ITest3DEngineEvents * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            _ITest3DEngineEvents * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            _ITest3DEngineEvents * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            _ITest3DEngineEvents * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            _ITest3DEngineEvents * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            _ITest3DEngineEvents * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            _ITest3DEngineEvents * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        END_INTERFACE
    } _ITest3DEngineEventsVtbl;

    interface _ITest3DEngineEvents
    {
        CONST_VTBL struct _ITest3DEngineEventsVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define _ITest3DEngineEvents_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define _ITest3DEngineEvents_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define _ITest3DEngineEvents_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define _ITest3DEngineEvents_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define _ITest3DEngineEvents_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define _ITest3DEngineEvents_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define _ITest3DEngineEvents_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)

#endif /* COBJMACROS */


#endif 	/* C style interface */


#endif 	/* ___ITest3DEngineEvents_DISPINTERFACE_DEFINED__ */


EXTERN_C const CLSID CLSID_Test3DEngine;

#ifdef __cplusplus

class DECLSPEC_UUID("D159A323-D278-475B-B556-57C064884B9B")
Test3DEngine;
#endif
#endif /* __atlaxLib_LIBRARY_DEFINED__ */

/* Additional Prototypes for ALL interfaces */

unsigned long             __RPC_USER  BSTR_UserSize(     unsigned long *, unsigned long            , BSTR * ); 
unsigned char * __RPC_USER  BSTR_UserMarshal(  unsigned long *, unsigned char *, BSTR * ); 
unsigned char * __RPC_USER  BSTR_UserUnmarshal(unsigned long *, unsigned char *, BSTR * ); 
void                      __RPC_USER  BSTR_UserFree(     unsigned long *, BSTR * ); 

unsigned long             __RPC_USER  HWND_UserSize(     unsigned long *, unsigned long            , HWND * ); 
unsigned char * __RPC_USER  HWND_UserMarshal(  unsigned long *, unsigned char *, HWND * ); 
unsigned char * __RPC_USER  HWND_UserUnmarshal(unsigned long *, unsigned char *, HWND * ); 
void                      __RPC_USER  HWND_UserFree(     unsigned long *, HWND * ); 

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif


