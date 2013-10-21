

/* this ALWAYS GENERATED file contains the definitions for the interfaces */


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

#ifndef __AtlKGRecordeOperator_h__
#define __AtlKGRecordeOperator_h__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

/* Forward Declarations */ 

#ifndef __IBaseLib_FWD_DEFINED__
#define __IBaseLib_FWD_DEFINED__
typedef interface IBaseLib IBaseLib;
#endif 	/* __IBaseLib_FWD_DEFINED__ */


#ifndef __IRecordPlayer_FWD_DEFINED__
#define __IRecordPlayer_FWD_DEFINED__
typedef interface IRecordPlayer IRecordPlayer;
#endif 	/* __IRecordPlayer_FWD_DEFINED__ */


#ifndef __BaseLib_FWD_DEFINED__
#define __BaseLib_FWD_DEFINED__

#ifdef __cplusplus
typedef class BaseLib BaseLib;
#else
typedef struct BaseLib BaseLib;
#endif /* __cplusplus */

#endif 	/* __BaseLib_FWD_DEFINED__ */


#ifndef __RecordPlayer_FWD_DEFINED__
#define __RecordPlayer_FWD_DEFINED__

#ifdef __cplusplus
typedef class RecordPlayer RecordPlayer;
#else
typedef struct RecordPlayer RecordPlayer;
#endif /* __cplusplus */

#endif 	/* __RecordPlayer_FWD_DEFINED__ */


/* header files for imported files */
#include "oaidl.h"
#include "ocidl.h"

#ifdef __cplusplus
extern "C"{
#endif 

void * __RPC_USER MIDL_user_allocate(size_t);
void __RPC_USER MIDL_user_free( void * ); 

#ifndef __IBaseLib_INTERFACE_DEFINED__
#define __IBaseLib_INTERFACE_DEFINED__

/* interface IBaseLib */
/* [unique][helpstring][uuid][object] */ 


EXTERN_C const IID IID_IBaseLib;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("DCA6998A-7600-481F-90BB-5ECB27FA2B12")
    IBaseLib : public IUnknown
    {
    public:
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE Init( 
            /* [in] */ int nhWnd) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE UnInit( void) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IBaseLibVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IBaseLib * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IBaseLib * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IBaseLib * This);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *Init )( 
            IBaseLib * This,
            /* [in] */ int nhWnd);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *UnInit )( 
            IBaseLib * This);
        
        END_INTERFACE
    } IBaseLibVtbl;

    interface IBaseLib
    {
        CONST_VTBL struct IBaseLibVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IBaseLib_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IBaseLib_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IBaseLib_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IBaseLib_Init(This,nhWnd)	\
    (This)->lpVtbl -> Init(This,nhWnd)

#define IBaseLib_UnInit(This)	\
    (This)->lpVtbl -> UnInit(This)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring] */ HRESULT STDMETHODCALLTYPE IBaseLib_Init_Proxy( 
    IBaseLib * This,
    /* [in] */ int nhWnd);


void __RPC_STUB IBaseLib_Init_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE IBaseLib_UnInit_Proxy( 
    IBaseLib * This);


void __RPC_STUB IBaseLib_UnInit_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IBaseLib_INTERFACE_DEFINED__ */


#ifndef __IRecordPlayer_INTERFACE_DEFINED__
#define __IRecordPlayer_INTERFACE_DEFINED__

/* interface IRecordPlayer */
/* [unique][helpstring][uuid][object] */ 


EXTERN_C const IID IID_IRecordPlayer;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("18A7E96F-A8C0-4335-87A5-F73AF7A888F6")
    IRecordPlayer : public IUnknown
    {
    public:
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE RePlay( 
            /* [out] */ int *pRetResult) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE PauseRePlay( 
            /* [out] */ int *pRetResult) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE StopRePlay( void) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE LoadRecordFile( 
            /* [in] */ BSTR pstrFileName) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE SetReplayWindow( 
            /* [in] */ ULONG uhWnd) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE FrameMoveRePlay( void) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE IsPlayFinished( 
            /* [out] */ int *pbFinished) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE GetLoadingProgress( 
            /* [out] */ FLOAT *pnLoadFilePer,
            /* [out] */ FLOAT *pnStepToEventPer,
            /* [out] */ INT *pnLoadScenePer) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE SaveRecordFile( 
            /* [in] */ BSTR pstrFileName) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE SetOption( 
            /* [in] */ int nWidth,
            /* [in] */ int nHeight) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE FreeCamera( void) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE IsFreeCamera( 
            /* [out] */ int *pnRetResult) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE GetCameraType( void) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE GetObjSize( 
            /* [out] */ int *pnSize) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE GetFirstObj( 
            /* [out] */ int *pnID,
            /* [out] */ BSTR *pstrRetName,
            /* [out] */ int *pnType) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE GetNextObj( 
            /* [out] */ int *pnID,
            /* [out] */ BSTR *pstrRetName,
            /* [out] */ int *pnType) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE InitTree( void) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE GetFirstEvent( 
            /* [out] */ BSTR *pnFunction,
            /* [out] */ int *pnInfo) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE GetNextEvent( 
            /* [out] */ BSTR *pnFunction,
            /* [out] */ int *pnInfo) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE GetInformation( 
            /* [in] */ int pnInfo,
            /* [out] */ BSTR *pstrLabel,
            /* [out] */ BSTR *pstrTextBox,
            /* [out] */ int *pnNum) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE AddEvent( 
            /* [in] */ int nNum,
            /* [in] */ BSTR strName,
            /* [in] */ BSTR strValue) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE RemoveEvent( 
            /* [in] */ int nInfo) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE GetNowTime( 
            /* [out] */ int *pnNowTime) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE GetTotalTime( 
            /* [out] */ int *pnTotalTime) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE GetPlayState( 
            /* [out] */ int *pPlayState) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE GetObjInfo( 
            int nID,
            /* [out] */ int *pnSuccess) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE GetFirstEventTime( 
            /* [out] */ int *pnTime,
            /* [out] */ int *pnInfo) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE GetNextEventTime( 
            /* [out] */ int *pnTime,
            /* [out] */ int *pnInfo) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE GetEventFunction( 
            /* [in] */ int pnInfo,
            /* [out] */ BSTR *pstrFunction) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IRecordPlayerVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IRecordPlayer * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IRecordPlayer * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IRecordPlayer * This);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *RePlay )( 
            IRecordPlayer * This,
            /* [out] */ int *pRetResult);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *PauseRePlay )( 
            IRecordPlayer * This,
            /* [out] */ int *pRetResult);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *StopRePlay )( 
            IRecordPlayer * This);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *LoadRecordFile )( 
            IRecordPlayer * This,
            /* [in] */ BSTR pstrFileName);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *SetReplayWindow )( 
            IRecordPlayer * This,
            /* [in] */ ULONG uhWnd);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *FrameMoveRePlay )( 
            IRecordPlayer * This);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *IsPlayFinished )( 
            IRecordPlayer * This,
            /* [out] */ int *pbFinished);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *GetLoadingProgress )( 
            IRecordPlayer * This,
            /* [out] */ FLOAT *pnLoadFilePer,
            /* [out] */ FLOAT *pnStepToEventPer,
            /* [out] */ INT *pnLoadScenePer);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *SaveRecordFile )( 
            IRecordPlayer * This,
            /* [in] */ BSTR pstrFileName);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *SetOption )( 
            IRecordPlayer * This,
            /* [in] */ int nWidth,
            /* [in] */ int nHeight);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *FreeCamera )( 
            IRecordPlayer * This);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *IsFreeCamera )( 
            IRecordPlayer * This,
            /* [out] */ int *pnRetResult);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *GetCameraType )( 
            IRecordPlayer * This);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *GetObjSize )( 
            IRecordPlayer * This,
            /* [out] */ int *pnSize);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *GetFirstObj )( 
            IRecordPlayer * This,
            /* [out] */ int *pnID,
            /* [out] */ BSTR *pstrRetName,
            /* [out] */ int *pnType);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *GetNextObj )( 
            IRecordPlayer * This,
            /* [out] */ int *pnID,
            /* [out] */ BSTR *pstrRetName,
            /* [out] */ int *pnType);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *InitTree )( 
            IRecordPlayer * This);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *GetFirstEvent )( 
            IRecordPlayer * This,
            /* [out] */ BSTR *pnFunction,
            /* [out] */ int *pnInfo);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *GetNextEvent )( 
            IRecordPlayer * This,
            /* [out] */ BSTR *pnFunction,
            /* [out] */ int *pnInfo);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *GetInformation )( 
            IRecordPlayer * This,
            /* [in] */ int pnInfo,
            /* [out] */ BSTR *pstrLabel,
            /* [out] */ BSTR *pstrTextBox,
            /* [out] */ int *pnNum);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *AddEvent )( 
            IRecordPlayer * This,
            /* [in] */ int nNum,
            /* [in] */ BSTR strName,
            /* [in] */ BSTR strValue);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *RemoveEvent )( 
            IRecordPlayer * This,
            /* [in] */ int nInfo);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *GetNowTime )( 
            IRecordPlayer * This,
            /* [out] */ int *pnNowTime);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *GetTotalTime )( 
            IRecordPlayer * This,
            /* [out] */ int *pnTotalTime);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *GetPlayState )( 
            IRecordPlayer * This,
            /* [out] */ int *pPlayState);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *GetObjInfo )( 
            IRecordPlayer * This,
            int nID,
            /* [out] */ int *pnSuccess);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *GetFirstEventTime )( 
            IRecordPlayer * This,
            /* [out] */ int *pnTime,
            /* [out] */ int *pnInfo);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *GetNextEventTime )( 
            IRecordPlayer * This,
            /* [out] */ int *pnTime,
            /* [out] */ int *pnInfo);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *GetEventFunction )( 
            IRecordPlayer * This,
            /* [in] */ int pnInfo,
            /* [out] */ BSTR *pstrFunction);
        
        END_INTERFACE
    } IRecordPlayerVtbl;

    interface IRecordPlayer
    {
        CONST_VTBL struct IRecordPlayerVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IRecordPlayer_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IRecordPlayer_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IRecordPlayer_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IRecordPlayer_RePlay(This,pRetResult)	\
    (This)->lpVtbl -> RePlay(This,pRetResult)

#define IRecordPlayer_PauseRePlay(This,pRetResult)	\
    (This)->lpVtbl -> PauseRePlay(This,pRetResult)

#define IRecordPlayer_StopRePlay(This)	\
    (This)->lpVtbl -> StopRePlay(This)

#define IRecordPlayer_LoadRecordFile(This,pstrFileName)	\
    (This)->lpVtbl -> LoadRecordFile(This,pstrFileName)

#define IRecordPlayer_SetReplayWindow(This,uhWnd)	\
    (This)->lpVtbl -> SetReplayWindow(This,uhWnd)

#define IRecordPlayer_FrameMoveRePlay(This)	\
    (This)->lpVtbl -> FrameMoveRePlay(This)

#define IRecordPlayer_IsPlayFinished(This,pbFinished)	\
    (This)->lpVtbl -> IsPlayFinished(This,pbFinished)

#define IRecordPlayer_GetLoadingProgress(This,pnLoadFilePer,pnStepToEventPer,pnLoadScenePer)	\
    (This)->lpVtbl -> GetLoadingProgress(This,pnLoadFilePer,pnStepToEventPer,pnLoadScenePer)

#define IRecordPlayer_SaveRecordFile(This,pstrFileName)	\
    (This)->lpVtbl -> SaveRecordFile(This,pstrFileName)

#define IRecordPlayer_SetOption(This,nWidth,nHeight)	\
    (This)->lpVtbl -> SetOption(This,nWidth,nHeight)

#define IRecordPlayer_FreeCamera(This)	\
    (This)->lpVtbl -> FreeCamera(This)

#define IRecordPlayer_IsFreeCamera(This,pnRetResult)	\
    (This)->lpVtbl -> IsFreeCamera(This,pnRetResult)

#define IRecordPlayer_GetCameraType(This)	\
    (This)->lpVtbl -> GetCameraType(This)

#define IRecordPlayer_GetObjSize(This,pnSize)	\
    (This)->lpVtbl -> GetObjSize(This,pnSize)

#define IRecordPlayer_GetFirstObj(This,pnID,pstrRetName,pnType)	\
    (This)->lpVtbl -> GetFirstObj(This,pnID,pstrRetName,pnType)

#define IRecordPlayer_GetNextObj(This,pnID,pstrRetName,pnType)	\
    (This)->lpVtbl -> GetNextObj(This,pnID,pstrRetName,pnType)

#define IRecordPlayer_InitTree(This)	\
    (This)->lpVtbl -> InitTree(This)

#define IRecordPlayer_GetFirstEvent(This,pnFunction,pnInfo)	\
    (This)->lpVtbl -> GetFirstEvent(This,pnFunction,pnInfo)

#define IRecordPlayer_GetNextEvent(This,pnFunction,pnInfo)	\
    (This)->lpVtbl -> GetNextEvent(This,pnFunction,pnInfo)

#define IRecordPlayer_GetInformation(This,pnInfo,pstrLabel,pstrTextBox,pnNum)	\
    (This)->lpVtbl -> GetInformation(This,pnInfo,pstrLabel,pstrTextBox,pnNum)

#define IRecordPlayer_AddEvent(This,nNum,strName,strValue)	\
    (This)->lpVtbl -> AddEvent(This,nNum,strName,strValue)

#define IRecordPlayer_RemoveEvent(This,nInfo)	\
    (This)->lpVtbl -> RemoveEvent(This,nInfo)

#define IRecordPlayer_GetNowTime(This,pnNowTime)	\
    (This)->lpVtbl -> GetNowTime(This,pnNowTime)

#define IRecordPlayer_GetTotalTime(This,pnTotalTime)	\
    (This)->lpVtbl -> GetTotalTime(This,pnTotalTime)

#define IRecordPlayer_GetPlayState(This,pPlayState)	\
    (This)->lpVtbl -> GetPlayState(This,pPlayState)

#define IRecordPlayer_GetObjInfo(This,nID,pnSuccess)	\
    (This)->lpVtbl -> GetObjInfo(This,nID,pnSuccess)

#define IRecordPlayer_GetFirstEventTime(This,pnTime,pnInfo)	\
    (This)->lpVtbl -> GetFirstEventTime(This,pnTime,pnInfo)

#define IRecordPlayer_GetNextEventTime(This,pnTime,pnInfo)	\
    (This)->lpVtbl -> GetNextEventTime(This,pnTime,pnInfo)

#define IRecordPlayer_GetEventFunction(This,pnInfo,pstrFunction)	\
    (This)->lpVtbl -> GetEventFunction(This,pnInfo,pstrFunction)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring] */ HRESULT STDMETHODCALLTYPE IRecordPlayer_RePlay_Proxy( 
    IRecordPlayer * This,
    /* [out] */ int *pRetResult);


void __RPC_STUB IRecordPlayer_RePlay_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE IRecordPlayer_PauseRePlay_Proxy( 
    IRecordPlayer * This,
    /* [out] */ int *pRetResult);


void __RPC_STUB IRecordPlayer_PauseRePlay_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE IRecordPlayer_StopRePlay_Proxy( 
    IRecordPlayer * This);


void __RPC_STUB IRecordPlayer_StopRePlay_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE IRecordPlayer_LoadRecordFile_Proxy( 
    IRecordPlayer * This,
    /* [in] */ BSTR pstrFileName);


void __RPC_STUB IRecordPlayer_LoadRecordFile_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE IRecordPlayer_SetReplayWindow_Proxy( 
    IRecordPlayer * This,
    /* [in] */ ULONG uhWnd);


void __RPC_STUB IRecordPlayer_SetReplayWindow_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE IRecordPlayer_FrameMoveRePlay_Proxy( 
    IRecordPlayer * This);


void __RPC_STUB IRecordPlayer_FrameMoveRePlay_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE IRecordPlayer_IsPlayFinished_Proxy( 
    IRecordPlayer * This,
    /* [out] */ int *pbFinished);


void __RPC_STUB IRecordPlayer_IsPlayFinished_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE IRecordPlayer_GetLoadingProgress_Proxy( 
    IRecordPlayer * This,
    /* [out] */ FLOAT *pnLoadFilePer,
    /* [out] */ FLOAT *pnStepToEventPer,
    /* [out] */ INT *pnLoadScenePer);


void __RPC_STUB IRecordPlayer_GetLoadingProgress_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE IRecordPlayer_SaveRecordFile_Proxy( 
    IRecordPlayer * This,
    /* [in] */ BSTR pstrFileName);


void __RPC_STUB IRecordPlayer_SaveRecordFile_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE IRecordPlayer_SetOption_Proxy( 
    IRecordPlayer * This,
    /* [in] */ int nWidth,
    /* [in] */ int nHeight);


void __RPC_STUB IRecordPlayer_SetOption_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE IRecordPlayer_FreeCamera_Proxy( 
    IRecordPlayer * This);


void __RPC_STUB IRecordPlayer_FreeCamera_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE IRecordPlayer_IsFreeCamera_Proxy( 
    IRecordPlayer * This,
    /* [out] */ int *pnRetResult);


void __RPC_STUB IRecordPlayer_IsFreeCamera_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE IRecordPlayer_GetCameraType_Proxy( 
    IRecordPlayer * This);


void __RPC_STUB IRecordPlayer_GetCameraType_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE IRecordPlayer_GetObjSize_Proxy( 
    IRecordPlayer * This,
    /* [out] */ int *pnSize);


void __RPC_STUB IRecordPlayer_GetObjSize_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE IRecordPlayer_GetFirstObj_Proxy( 
    IRecordPlayer * This,
    /* [out] */ int *pnID,
    /* [out] */ BSTR *pstrRetName,
    /* [out] */ int *pnType);


void __RPC_STUB IRecordPlayer_GetFirstObj_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE IRecordPlayer_GetNextObj_Proxy( 
    IRecordPlayer * This,
    /* [out] */ int *pnID,
    /* [out] */ BSTR *pstrRetName,
    /* [out] */ int *pnType);


void __RPC_STUB IRecordPlayer_GetNextObj_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE IRecordPlayer_InitTree_Proxy( 
    IRecordPlayer * This);


void __RPC_STUB IRecordPlayer_InitTree_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE IRecordPlayer_GetFirstEvent_Proxy( 
    IRecordPlayer * This,
    /* [out] */ BSTR *pnFunction,
    /* [out] */ int *pnInfo);


void __RPC_STUB IRecordPlayer_GetFirstEvent_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE IRecordPlayer_GetNextEvent_Proxy( 
    IRecordPlayer * This,
    /* [out] */ BSTR *pnFunction,
    /* [out] */ int *pnInfo);


void __RPC_STUB IRecordPlayer_GetNextEvent_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE IRecordPlayer_GetInformation_Proxy( 
    IRecordPlayer * This,
    /* [in] */ int pnInfo,
    /* [out] */ BSTR *pstrLabel,
    /* [out] */ BSTR *pstrTextBox,
    /* [out] */ int *pnNum);


void __RPC_STUB IRecordPlayer_GetInformation_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE IRecordPlayer_AddEvent_Proxy( 
    IRecordPlayer * This,
    /* [in] */ int nNum,
    /* [in] */ BSTR strName,
    /* [in] */ BSTR strValue);


void __RPC_STUB IRecordPlayer_AddEvent_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE IRecordPlayer_RemoveEvent_Proxy( 
    IRecordPlayer * This,
    /* [in] */ int nInfo);


void __RPC_STUB IRecordPlayer_RemoveEvent_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE IRecordPlayer_GetNowTime_Proxy( 
    IRecordPlayer * This,
    /* [out] */ int *pnNowTime);


void __RPC_STUB IRecordPlayer_GetNowTime_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE IRecordPlayer_GetTotalTime_Proxy( 
    IRecordPlayer * This,
    /* [out] */ int *pnTotalTime);


void __RPC_STUB IRecordPlayer_GetTotalTime_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE IRecordPlayer_GetPlayState_Proxy( 
    IRecordPlayer * This,
    /* [out] */ int *pPlayState);


void __RPC_STUB IRecordPlayer_GetPlayState_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE IRecordPlayer_GetObjInfo_Proxy( 
    IRecordPlayer * This,
    int nID,
    /* [out] */ int *pnSuccess);


void __RPC_STUB IRecordPlayer_GetObjInfo_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE IRecordPlayer_GetFirstEventTime_Proxy( 
    IRecordPlayer * This,
    /* [out] */ int *pnTime,
    /* [out] */ int *pnInfo);


void __RPC_STUB IRecordPlayer_GetFirstEventTime_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE IRecordPlayer_GetNextEventTime_Proxy( 
    IRecordPlayer * This,
    /* [out] */ int *pnTime,
    /* [out] */ int *pnInfo);


void __RPC_STUB IRecordPlayer_GetNextEventTime_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE IRecordPlayer_GetEventFunction_Proxy( 
    IRecordPlayer * This,
    /* [in] */ int pnInfo,
    /* [out] */ BSTR *pstrFunction);


void __RPC_STUB IRecordPlayer_GetEventFunction_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IRecordPlayer_INTERFACE_DEFINED__ */



#ifndef __AtlKGRecordeOperatorLib_LIBRARY_DEFINED__
#define __AtlKGRecordeOperatorLib_LIBRARY_DEFINED__

/* library AtlKGRecordeOperatorLib */
/* [helpstring][version][uuid] */ 


EXTERN_C const IID LIBID_AtlKGRecordeOperatorLib;

EXTERN_C const CLSID CLSID_BaseLib;

#ifdef __cplusplus

class DECLSPEC_UUID("6951EF60-C91E-408D-B468-4F62F3C67A81")
BaseLib;
#endif

EXTERN_C const CLSID CLSID_RecordPlayer;

#ifdef __cplusplus

class DECLSPEC_UUID("3D5D9834-A2BA-419F-A11B-7D695518D954")
RecordPlayer;
#endif
#endif /* __AtlKGRecordeOperatorLib_LIBRARY_DEFINED__ */

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


