

/* this ALWAYS GENERATED file contains the definitions for the interfaces */


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

#pragma warning( disable: 4049 )  /* more than 64k source lines */


/* verify that the <rpcndr.h> version is high enough to compile this file*/
#ifndef __REQUIRED_RPCNDR_H_VERSION__
#define __REQUIRED_RPCNDR_H_VERSION__ 475
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

#ifndef __ThumbExtract_h__
#define __ThumbExtract_h__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

/* Forward Declarations */ 

#ifndef __IFileThumbExtract_FWD_DEFINED__
#define __IFileThumbExtract_FWD_DEFINED__
typedef interface IFileThumbExtract IFileThumbExtract;
#endif 	/* __IFileThumbExtract_FWD_DEFINED__ */


#ifndef __FileThumbExtract_FWD_DEFINED__
#define __FileThumbExtract_FWD_DEFINED__

#ifdef __cplusplus
typedef class FileThumbExtract FileThumbExtract;
#else
typedef struct FileThumbExtract FileThumbExtract;
#endif /* __cplusplus */

#endif 	/* __FileThumbExtract_FWD_DEFINED__ */


/* header files for imported files */
#include "oaidl.h"
#include "ocidl.h"

#ifdef __cplusplus
extern "C"{
#endif 

void * __RPC_USER MIDL_user_allocate(size_t);
void __RPC_USER MIDL_user_free( void * ); 

#ifndef __IFileThumbExtract_INTERFACE_DEFINED__
#define __IFileThumbExtract_INTERFACE_DEFINED__

/* interface IFileThumbExtract */
/* [unique][helpstring][dual][uuid][object] */ 


EXTERN_C const IID IID_IFileThumbExtract;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("A1B8E7E1-2B2E-46D2-B60F-993028A58182")
    IFileThumbExtract : public IDispatch
    {
    public:
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SetPath( 
            /* [in] */ BSTR Path) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SetDir( 
            /* [in] */ BSTR Dir) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SetFile( 
            /* [in] */ BSTR File) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SetThumbnailSize( 
            /* [in] */ long Width,
            /* [in] */ long Height) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetPath( 
            /* [retval][out] */ BSTR *Path) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetDir( 
            /* [retval][out] */ BSTR *Dir) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetFile( 
            /* [retval][out] */ BSTR *File) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ExtractThumbnail( 
            /* [retval][out] */ long *phBitmap) = 0;
        
        virtual /* [hidden][helpstring][id] */ HRESULT STDMETHODCALLTYPE FreeThumbnail( void) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SaveToFile( 
            /* [in] */ BSTR FilePath) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Stride( 
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Version( 
            /* [retval][out] */ BSTR *pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_ThumbJpgData( 
            /* [defaultvalue][in] */ long JpegQuality,
            /* [retval][out] */ VARIANT *pVal) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IFileThumbExtractVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IFileThumbExtract * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IFileThumbExtract * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IFileThumbExtract * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IFileThumbExtract * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IFileThumbExtract * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IFileThumbExtract * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IFileThumbExtract * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SetPath )( 
            IFileThumbExtract * This,
            /* [in] */ BSTR Path);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SetDir )( 
            IFileThumbExtract * This,
            /* [in] */ BSTR Dir);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SetFile )( 
            IFileThumbExtract * This,
            /* [in] */ BSTR File);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SetThumbnailSize )( 
            IFileThumbExtract * This,
            /* [in] */ long Width,
            /* [in] */ long Height);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetPath )( 
            IFileThumbExtract * This,
            /* [retval][out] */ BSTR *Path);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetDir )( 
            IFileThumbExtract * This,
            /* [retval][out] */ BSTR *Dir);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetFile )( 
            IFileThumbExtract * This,
            /* [retval][out] */ BSTR *File);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *ExtractThumbnail )( 
            IFileThumbExtract * This,
            /* [retval][out] */ long *phBitmap);
        
        /* [hidden][helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *FreeThumbnail )( 
            IFileThumbExtract * This);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SaveToFile )( 
            IFileThumbExtract * This,
            /* [in] */ BSTR FilePath);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Stride )( 
            IFileThumbExtract * This,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Version )( 
            IFileThumbExtract * This,
            /* [retval][out] */ BSTR *pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_ThumbJpgData )( 
            IFileThumbExtract * This,
            /* [defaultvalue][in] */ long JpegQuality,
            /* [retval][out] */ VARIANT *pVal);
        
        END_INTERFACE
    } IFileThumbExtractVtbl;

    interface IFileThumbExtract
    {
        CONST_VTBL struct IFileThumbExtractVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IFileThumbExtract_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IFileThumbExtract_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IFileThumbExtract_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IFileThumbExtract_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IFileThumbExtract_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IFileThumbExtract_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IFileThumbExtract_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IFileThumbExtract_SetPath(This,Path)	\
    (This)->lpVtbl -> SetPath(This,Path)

#define IFileThumbExtract_SetDir(This,Dir)	\
    (This)->lpVtbl -> SetDir(This,Dir)

#define IFileThumbExtract_SetFile(This,File)	\
    (This)->lpVtbl -> SetFile(This,File)

#define IFileThumbExtract_SetThumbnailSize(This,Width,Height)	\
    (This)->lpVtbl -> SetThumbnailSize(This,Width,Height)

#define IFileThumbExtract_GetPath(This,Path)	\
    (This)->lpVtbl -> GetPath(This,Path)

#define IFileThumbExtract_GetDir(This,Dir)	\
    (This)->lpVtbl -> GetDir(This,Dir)

#define IFileThumbExtract_GetFile(This,File)	\
    (This)->lpVtbl -> GetFile(This,File)

#define IFileThumbExtract_ExtractThumbnail(This,phBitmap)	\
    (This)->lpVtbl -> ExtractThumbnail(This,phBitmap)

#define IFileThumbExtract_FreeThumbnail(This)	\
    (This)->lpVtbl -> FreeThumbnail(This)

#define IFileThumbExtract_SaveToFile(This,FilePath)	\
    (This)->lpVtbl -> SaveToFile(This,FilePath)

#define IFileThumbExtract_get_Stride(This,pVal)	\
    (This)->lpVtbl -> get_Stride(This,pVal)

#define IFileThumbExtract_get_Version(This,pVal)	\
    (This)->lpVtbl -> get_Version(This,pVal)

#define IFileThumbExtract_get_ThumbJpgData(This,JpegQuality,pVal)	\
    (This)->lpVtbl -> get_ThumbJpgData(This,JpegQuality,pVal)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IFileThumbExtract_SetPath_Proxy( 
    IFileThumbExtract * This,
    /* [in] */ BSTR Path);


void __RPC_STUB IFileThumbExtract_SetPath_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IFileThumbExtract_SetDir_Proxy( 
    IFileThumbExtract * This,
    /* [in] */ BSTR Dir);


void __RPC_STUB IFileThumbExtract_SetDir_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IFileThumbExtract_SetFile_Proxy( 
    IFileThumbExtract * This,
    /* [in] */ BSTR File);


void __RPC_STUB IFileThumbExtract_SetFile_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IFileThumbExtract_SetThumbnailSize_Proxy( 
    IFileThumbExtract * This,
    /* [in] */ long Width,
    /* [in] */ long Height);


void __RPC_STUB IFileThumbExtract_SetThumbnailSize_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IFileThumbExtract_GetPath_Proxy( 
    IFileThumbExtract * This,
    /* [retval][out] */ BSTR *Path);


void __RPC_STUB IFileThumbExtract_GetPath_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IFileThumbExtract_GetDir_Proxy( 
    IFileThumbExtract * This,
    /* [retval][out] */ BSTR *Dir);


void __RPC_STUB IFileThumbExtract_GetDir_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IFileThumbExtract_GetFile_Proxy( 
    IFileThumbExtract * This,
    /* [retval][out] */ BSTR *File);


void __RPC_STUB IFileThumbExtract_GetFile_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IFileThumbExtract_ExtractThumbnail_Proxy( 
    IFileThumbExtract * This,
    /* [retval][out] */ long *phBitmap);


void __RPC_STUB IFileThumbExtract_ExtractThumbnail_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [hidden][helpstring][id] */ HRESULT STDMETHODCALLTYPE IFileThumbExtract_FreeThumbnail_Proxy( 
    IFileThumbExtract * This);


void __RPC_STUB IFileThumbExtract_FreeThumbnail_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IFileThumbExtract_SaveToFile_Proxy( 
    IFileThumbExtract * This,
    /* [in] */ BSTR FilePath);


void __RPC_STUB IFileThumbExtract_SaveToFile_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IFileThumbExtract_get_Stride_Proxy( 
    IFileThumbExtract * This,
    /* [retval][out] */ long *pVal);


void __RPC_STUB IFileThumbExtract_get_Stride_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IFileThumbExtract_get_Version_Proxy( 
    IFileThumbExtract * This,
    /* [retval][out] */ BSTR *pVal);


void __RPC_STUB IFileThumbExtract_get_Version_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IFileThumbExtract_get_ThumbJpgData_Proxy( 
    IFileThumbExtract * This,
    /* [defaultvalue][in] */ long JpegQuality,
    /* [retval][out] */ VARIANT *pVal);


void __RPC_STUB IFileThumbExtract_get_ThumbJpgData_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IFileThumbExtract_INTERFACE_DEFINED__ */



#ifndef __THUMBEXTRACTLib_LIBRARY_DEFINED__
#define __THUMBEXTRACTLib_LIBRARY_DEFINED__

/* library THUMBEXTRACTLib */
/* [helpstring][version][uuid] */ 


EXTERN_C const IID LIBID_THUMBEXTRACTLib;

EXTERN_C const CLSID CLSID_FileThumbExtract;

#ifdef __cplusplus

class DECLSPEC_UUID("FC9850E3-9FC3-4C1F-8EE7-28F51EC014E1")
FileThumbExtract;
#endif
#endif /* __THUMBEXTRACTLib_LIBRARY_DEFINED__ */

/* Additional Prototypes for ALL interfaces */

unsigned long             __RPC_USER  BSTR_UserSize(     unsigned long *, unsigned long            , BSTR * ); 
unsigned char * __RPC_USER  BSTR_UserMarshal(  unsigned long *, unsigned char *, BSTR * ); 
unsigned char * __RPC_USER  BSTR_UserUnmarshal(unsigned long *, unsigned char *, BSTR * ); 
void                      __RPC_USER  BSTR_UserFree(     unsigned long *, BSTR * ); 

unsigned long             __RPC_USER  VARIANT_UserSize(     unsigned long *, unsigned long            , VARIANT * ); 
unsigned char * __RPC_USER  VARIANT_UserMarshal(  unsigned long *, unsigned char *, VARIANT * ); 
unsigned char * __RPC_USER  VARIANT_UserUnmarshal(unsigned long *, unsigned char *, VARIANT * ); 
void                      __RPC_USER  VARIANT_UserFree(     unsigned long *, VARIANT * ); 

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif


