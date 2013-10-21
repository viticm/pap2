

/* this ALWAYS GENERATED file contains the definitions for the interfaces */


 /* File created by MIDL compiler version 6.00.0366 */
/* at Tue Feb 17 10:49:29 2009
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
            LONG hWnd,
            BSTR strEnginePath,
            BSTR strStartUpPath) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Render( void) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE FrameMove( void) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE UnInit( void) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE OnResizeWindow( void) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_PlayMode( 
            /* [retval][out] */ VARIANT_BOOL *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_PlayMode( 
            /* [in] */ VARIANT_BOOL newVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE OnWndMsg( 
            /* [in] */ BSTR editor_name,
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
            BSTR strFileName,
            BOOL bLoop) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ZoomView( 
            LONG lDelta) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ChangeModelPart( 
            BSTR filename,
            INT PartID) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE LoadFace( 
            BSTR strFileName) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ItemsClear( void) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SaveMDLToFile( 
            BSTR strFileName) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE LoadMaterial( 
            /* [in] */ BSTR strFileName) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE LoadAnimation( 
            /* [in] */ BSTR strFileName,
            BOOL bLoopPlay) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE LoadPluginModel( 
            /* [in] */ BSTR strFileName,
            BSTR strBindDest) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ClearPluginModel( 
            /* [in] */ BSTR strBindDest) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ActivateScene( 
            /* [in] */ BSTR editor_name) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE RemoveScene( 
            /* [in] */ BSTR editor_name) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE AddScene( 
            /* [in] */ LONG hwndOutput,
            /* [in] */ BSTR editor_name,
            /* [in] */ LONG iOption) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetEditorSceneCount( 
            /* [in] */ BSTR editor_name,
            /* [out] */ LONG *count) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE LoadSelectorSfxFile( 
            /* [in] */ BSTR sfx_name) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ScaleSelectorSfx( 
            /* [in] */ FLOAT x,
            /* [in] */ FLOAT y,
            /* [in] */ FLOAT z) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE LoadOneMap_NoOutputWindow( 
            /* [in] */ BSTR filename,
            /* [out] */ LONG *ptrScene) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE LoadBillboardSfx( 
            /* [in] */ BSTR filename) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SetBillboardHeight( 
            /* [in] */ INT npc_height,
            /* [in] */ INT head_top_adjust) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ToggleRotateModelEditor( void) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SetCameraInfo( 
            LONG pX,
            LONG pY,
            LONG pZ,
            LONG lX,
            LONG lY,
            LONG lZ,
            LONG width,
            LONG height) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetCameraInfo( 
            /* [out] */ LONG *pX,
            /* [out] */ LONG *pY,
            /* [out] */ LONG *pZ,
            /* [out] */ LONG *lX,
            /* [out] */ LONG *lY,
            /* [out] */ LONG *lZ,
            /* [out] */ LONG *width,
            /* [out] */ LONG *height) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE AdjustCameraOrthogonal( 
            /* [in] */ LONG iOffset) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SetModelScale( 
            /* [in] */ FLOAT fScale) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SaveModelToImageFile( 
            /* [in] */ BSTR fileName) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE LoadSkillSfx( 
            /* [in] */ BSTR filename,
            /* [in] */ FLOAT scale,
            /* [in] */ BSTR socketName,
            /* [in] */ LONG iIndex) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ReadMapTerrainInfo( 
            /* [in] */ LONG ptrScene,
            /* [out] */ LONG *width,
            /* [out] */ LONG *height) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE FileToImage( 
            /* [in] */ BSTR src3DFileName,
            /* [in] */ BSTR desImageFileName) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetCurSceneIndex( 
            /* [out] */ LONG *index) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ActivateSceneByIndex( 
            /* [in] */ LONG index) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SaveMdlWithAniToImages( 
            /* [in] */ LONG ptrModel,
            /* [in] */ BSTR filename) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SaveLogicScene( 
            /* [in] */ BSTR iniFile,
            /* [in] */ LONG ptr3DScene) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ShowProgressForm( 
            /* [in] */ BOOL bShow) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE UnloadPlayerModel( void) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetHeight( 
            /* [in] */ LONG ptrScene,
            /* [in] */ FLOAT x,
            /* [in] */ FLOAT z,
            /* [out] */ FLOAT *y) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetFileNameHash( 
            /* [in] */ BSTR fileName,
            /* [out] */ DWORD *hashid) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE QuaternionRotationByNDir( 
            FLOAT *x,
            FLOAT *y,
            FLOAT *z,
            FLOAT *w,
            /* [in] */ LONG nDir) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE LoadPluginMaterial( 
            /* [in] */ BSTR strSocketName,
            /* [in] */ BSTR strMtlFile) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetAniFrameCount( 
            /* [in] */ BSTR strAniFileName,
            /* [retval][out] */ LONG *count) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE LoadPlaneForSelector( 
            /* [in] */ BSTR strFileName) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SetCaptionHeight( 
            /* [in] */ FLOAT fHeight) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SetCaptionText( 
            /* [in] */ BSTR strName,
            /* [in] */ BSTR strTitle) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetBBoxXY( 
            /* [in] */ BSTR mdl,
            /* [out] */ FLOAT *x,
            /* [out] */ FLOAT *y) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetMdlBBoxMax2DLength( 
            /* [in] */ BSTR mdl,
            /* [retval][out] */ FLOAT *pVal) = 0;
        
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
            LONG hWnd,
            BSTR strEnginePath,
            BSTR strStartUpPath);
        
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
            /* [in] */ BSTR editor_name,
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
            BSTR strFileName,
            BOOL bLoop);
        
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
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *ItemsClear )( 
            ITest3DEngine * This);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SaveMDLToFile )( 
            ITest3DEngine * This,
            BSTR strFileName);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *LoadMaterial )( 
            ITest3DEngine * This,
            /* [in] */ BSTR strFileName);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *LoadAnimation )( 
            ITest3DEngine * This,
            /* [in] */ BSTR strFileName,
            BOOL bLoopPlay);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *LoadPluginModel )( 
            ITest3DEngine * This,
            /* [in] */ BSTR strFileName,
            BSTR strBindDest);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *ClearPluginModel )( 
            ITest3DEngine * This,
            /* [in] */ BSTR strBindDest);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *ActivateScene )( 
            ITest3DEngine * This,
            /* [in] */ BSTR editor_name);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *RemoveScene )( 
            ITest3DEngine * This,
            /* [in] */ BSTR editor_name);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *AddScene )( 
            ITest3DEngine * This,
            /* [in] */ LONG hwndOutput,
            /* [in] */ BSTR editor_name,
            /* [in] */ LONG iOption);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetEditorSceneCount )( 
            ITest3DEngine * This,
            /* [in] */ BSTR editor_name,
            /* [out] */ LONG *count);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *LoadSelectorSfxFile )( 
            ITest3DEngine * This,
            /* [in] */ BSTR sfx_name);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *ScaleSelectorSfx )( 
            ITest3DEngine * This,
            /* [in] */ FLOAT x,
            /* [in] */ FLOAT y,
            /* [in] */ FLOAT z);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *LoadOneMap_NoOutputWindow )( 
            ITest3DEngine * This,
            /* [in] */ BSTR filename,
            /* [out] */ LONG *ptrScene);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *LoadBillboardSfx )( 
            ITest3DEngine * This,
            /* [in] */ BSTR filename);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SetBillboardHeight )( 
            ITest3DEngine * This,
            /* [in] */ INT npc_height,
            /* [in] */ INT head_top_adjust);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *ToggleRotateModelEditor )( 
            ITest3DEngine * This);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SetCameraInfo )( 
            ITest3DEngine * This,
            LONG pX,
            LONG pY,
            LONG pZ,
            LONG lX,
            LONG lY,
            LONG lZ,
            LONG width,
            LONG height);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetCameraInfo )( 
            ITest3DEngine * This,
            /* [out] */ LONG *pX,
            /* [out] */ LONG *pY,
            /* [out] */ LONG *pZ,
            /* [out] */ LONG *lX,
            /* [out] */ LONG *lY,
            /* [out] */ LONG *lZ,
            /* [out] */ LONG *width,
            /* [out] */ LONG *height);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *AdjustCameraOrthogonal )( 
            ITest3DEngine * This,
            /* [in] */ LONG iOffset);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SetModelScale )( 
            ITest3DEngine * This,
            /* [in] */ FLOAT fScale);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SaveModelToImageFile )( 
            ITest3DEngine * This,
            /* [in] */ BSTR fileName);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *LoadSkillSfx )( 
            ITest3DEngine * This,
            /* [in] */ BSTR filename,
            /* [in] */ FLOAT scale,
            /* [in] */ BSTR socketName,
            /* [in] */ LONG iIndex);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *ReadMapTerrainInfo )( 
            ITest3DEngine * This,
            /* [in] */ LONG ptrScene,
            /* [out] */ LONG *width,
            /* [out] */ LONG *height);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *FileToImage )( 
            ITest3DEngine * This,
            /* [in] */ BSTR src3DFileName,
            /* [in] */ BSTR desImageFileName);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetCurSceneIndex )( 
            ITest3DEngine * This,
            /* [out] */ LONG *index);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *ActivateSceneByIndex )( 
            ITest3DEngine * This,
            /* [in] */ LONG index);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SaveMdlWithAniToImages )( 
            ITest3DEngine * This,
            /* [in] */ LONG ptrModel,
            /* [in] */ BSTR filename);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SaveLogicScene )( 
            ITest3DEngine * This,
            /* [in] */ BSTR iniFile,
            /* [in] */ LONG ptr3DScene);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *ShowProgressForm )( 
            ITest3DEngine * This,
            /* [in] */ BOOL bShow);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *UnloadPlayerModel )( 
            ITest3DEngine * This);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetHeight )( 
            ITest3DEngine * This,
            /* [in] */ LONG ptrScene,
            /* [in] */ FLOAT x,
            /* [in] */ FLOAT z,
            /* [out] */ FLOAT *y);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetFileNameHash )( 
            ITest3DEngine * This,
            /* [in] */ BSTR fileName,
            /* [out] */ DWORD *hashid);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *QuaternionRotationByNDir )( 
            ITest3DEngine * This,
            FLOAT *x,
            FLOAT *y,
            FLOAT *z,
            FLOAT *w,
            /* [in] */ LONG nDir);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *LoadPluginMaterial )( 
            ITest3DEngine * This,
            /* [in] */ BSTR strSocketName,
            /* [in] */ BSTR strMtlFile);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetAniFrameCount )( 
            ITest3DEngine * This,
            /* [in] */ BSTR strAniFileName,
            /* [retval][out] */ LONG *count);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *LoadPlaneForSelector )( 
            ITest3DEngine * This,
            /* [in] */ BSTR strFileName);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SetCaptionHeight )( 
            ITest3DEngine * This,
            /* [in] */ FLOAT fHeight);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SetCaptionText )( 
            ITest3DEngine * This,
            /* [in] */ BSTR strName,
            /* [in] */ BSTR strTitle);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetBBoxXY )( 
            ITest3DEngine * This,
            /* [in] */ BSTR mdl,
            /* [out] */ FLOAT *x,
            /* [out] */ FLOAT *y);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetMdlBBoxMax2DLength )( 
            ITest3DEngine * This,
            /* [in] */ BSTR mdl,
            /* [retval][out] */ FLOAT *pVal);
        
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


#define ITest3DEngine_Init(This,hWnd,strEnginePath,strStartUpPath)	\
    (This)->lpVtbl -> Init(This,hWnd,strEnginePath,strStartUpPath)

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

#define ITest3DEngine_OnWndMsg(This,editor_name,WndMsg,pnRet)	\
    (This)->lpVtbl -> OnWndMsg(This,editor_name,WndMsg,pnRet)

#define ITest3DEngine_LoadFile(This,strFileName)	\
    (This)->lpVtbl -> LoadFile(This,strFileName)

#define ITest3DEngine_UnloadFile(This,strFileName)	\
    (This)->lpVtbl -> UnloadFile(This,strFileName)

#define ITest3DEngine_LoadPlayerModel(This,strFileName)	\
    (This)->lpVtbl -> LoadPlayerModel(This,strFileName)

#define ITest3DEngine_BindToPlayer(This,strFileName,dwType,strBindToObj)	\
    (This)->lpVtbl -> BindToPlayer(This,strFileName,dwType,strBindToObj)

#define ITest3DEngine_LoadAni(This,strFileName,bLoop)	\
    (This)->lpVtbl -> LoadAni(This,strFileName,bLoop)

#define ITest3DEngine_ZoomView(This,lDelta)	\
    (This)->lpVtbl -> ZoomView(This,lDelta)

#define ITest3DEngine_ChangeModelPart(This,filename,PartID)	\
    (This)->lpVtbl -> ChangeModelPart(This,filename,PartID)

#define ITest3DEngine_LoadFace(This,strFileName)	\
    (This)->lpVtbl -> LoadFace(This,strFileName)

#define ITest3DEngine_ItemsClear(This)	\
    (This)->lpVtbl -> ItemsClear(This)

#define ITest3DEngine_SaveMDLToFile(This,strFileName)	\
    (This)->lpVtbl -> SaveMDLToFile(This,strFileName)

#define ITest3DEngine_LoadMaterial(This,strFileName)	\
    (This)->lpVtbl -> LoadMaterial(This,strFileName)

#define ITest3DEngine_LoadAnimation(This,strFileName,bLoopPlay)	\
    (This)->lpVtbl -> LoadAnimation(This,strFileName,bLoopPlay)

#define ITest3DEngine_LoadPluginModel(This,strFileName,strBindDest)	\
    (This)->lpVtbl -> LoadPluginModel(This,strFileName,strBindDest)

#define ITest3DEngine_ClearPluginModel(This,strBindDest)	\
    (This)->lpVtbl -> ClearPluginModel(This,strBindDest)

#define ITest3DEngine_ActivateScene(This,editor_name)	\
    (This)->lpVtbl -> ActivateScene(This,editor_name)

#define ITest3DEngine_RemoveScene(This,editor_name)	\
    (This)->lpVtbl -> RemoveScene(This,editor_name)

#define ITest3DEngine_AddScene(This,hwndOutput,editor_name,iOption)	\
    (This)->lpVtbl -> AddScene(This,hwndOutput,editor_name,iOption)

#define ITest3DEngine_GetEditorSceneCount(This,editor_name,count)	\
    (This)->lpVtbl -> GetEditorSceneCount(This,editor_name,count)

#define ITest3DEngine_LoadSelectorSfxFile(This,sfx_name)	\
    (This)->lpVtbl -> LoadSelectorSfxFile(This,sfx_name)

#define ITest3DEngine_ScaleSelectorSfx(This,x,y,z)	\
    (This)->lpVtbl -> ScaleSelectorSfx(This,x,y,z)

#define ITest3DEngine_LoadOneMap_NoOutputWindow(This,filename,ptrScene)	\
    (This)->lpVtbl -> LoadOneMap_NoOutputWindow(This,filename,ptrScene)

#define ITest3DEngine_LoadBillboardSfx(This,filename)	\
    (This)->lpVtbl -> LoadBillboardSfx(This,filename)

#define ITest3DEngine_SetBillboardHeight(This,npc_height,head_top_adjust)	\
    (This)->lpVtbl -> SetBillboardHeight(This,npc_height,head_top_adjust)

#define ITest3DEngine_ToggleRotateModelEditor(This)	\
    (This)->lpVtbl -> ToggleRotateModelEditor(This)

#define ITest3DEngine_SetCameraInfo(This,pX,pY,pZ,lX,lY,lZ,width,height)	\
    (This)->lpVtbl -> SetCameraInfo(This,pX,pY,pZ,lX,lY,lZ,width,height)

#define ITest3DEngine_GetCameraInfo(This,pX,pY,pZ,lX,lY,lZ,width,height)	\
    (This)->lpVtbl -> GetCameraInfo(This,pX,pY,pZ,lX,lY,lZ,width,height)

#define ITest3DEngine_AdjustCameraOrthogonal(This,iOffset)	\
    (This)->lpVtbl -> AdjustCameraOrthogonal(This,iOffset)

#define ITest3DEngine_SetModelScale(This,fScale)	\
    (This)->lpVtbl -> SetModelScale(This,fScale)

#define ITest3DEngine_SaveModelToImageFile(This,fileName)	\
    (This)->lpVtbl -> SaveModelToImageFile(This,fileName)

#define ITest3DEngine_LoadSkillSfx(This,filename,scale,socketName,iIndex)	\
    (This)->lpVtbl -> LoadSkillSfx(This,filename,scale,socketName,iIndex)

#define ITest3DEngine_ReadMapTerrainInfo(This,ptrScene,width,height)	\
    (This)->lpVtbl -> ReadMapTerrainInfo(This,ptrScene,width,height)

#define ITest3DEngine_FileToImage(This,src3DFileName,desImageFileName)	\
    (This)->lpVtbl -> FileToImage(This,src3DFileName,desImageFileName)

#define ITest3DEngine_GetCurSceneIndex(This,index)	\
    (This)->lpVtbl -> GetCurSceneIndex(This,index)

#define ITest3DEngine_ActivateSceneByIndex(This,index)	\
    (This)->lpVtbl -> ActivateSceneByIndex(This,index)

#define ITest3DEngine_SaveMdlWithAniToImages(This,ptrModel,filename)	\
    (This)->lpVtbl -> SaveMdlWithAniToImages(This,ptrModel,filename)

#define ITest3DEngine_SaveLogicScene(This,iniFile,ptr3DScene)	\
    (This)->lpVtbl -> SaveLogicScene(This,iniFile,ptr3DScene)

#define ITest3DEngine_ShowProgressForm(This,bShow)	\
    (This)->lpVtbl -> ShowProgressForm(This,bShow)

#define ITest3DEngine_UnloadPlayerModel(This)	\
    (This)->lpVtbl -> UnloadPlayerModel(This)

#define ITest3DEngine_GetHeight(This,ptrScene,x,z,y)	\
    (This)->lpVtbl -> GetHeight(This,ptrScene,x,z,y)

#define ITest3DEngine_GetFileNameHash(This,fileName,hashid)	\
    (This)->lpVtbl -> GetFileNameHash(This,fileName,hashid)

#define ITest3DEngine_QuaternionRotationByNDir(This,x,y,z,w,nDir)	\
    (This)->lpVtbl -> QuaternionRotationByNDir(This,x,y,z,w,nDir)

#define ITest3DEngine_LoadPluginMaterial(This,strSocketName,strMtlFile)	\
    (This)->lpVtbl -> LoadPluginMaterial(This,strSocketName,strMtlFile)

#define ITest3DEngine_GetAniFrameCount(This,strAniFileName,count)	\
    (This)->lpVtbl -> GetAniFrameCount(This,strAniFileName,count)

#define ITest3DEngine_LoadPlaneForSelector(This,strFileName)	\
    (This)->lpVtbl -> LoadPlaneForSelector(This,strFileName)

#define ITest3DEngine_SetCaptionHeight(This,fHeight)	\
    (This)->lpVtbl -> SetCaptionHeight(This,fHeight)

#define ITest3DEngine_SetCaptionText(This,strName,strTitle)	\
    (This)->lpVtbl -> SetCaptionText(This,strName,strTitle)

#define ITest3DEngine_GetBBoxXY(This,mdl,x,y)	\
    (This)->lpVtbl -> GetBBoxXY(This,mdl,x,y)

#define ITest3DEngine_GetMdlBBoxMax2DLength(This,mdl,pVal)	\
    (This)->lpVtbl -> GetMdlBBoxMax2DLength(This,mdl,pVal)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ITest3DEngine_Init_Proxy( 
    ITest3DEngine * This,
    LONG hWnd,
    BSTR strEnginePath,
    BSTR strStartUpPath);


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
    /* [in] */ BSTR editor_name,
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
    BSTR strFileName,
    BOOL bLoop);


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


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ITest3DEngine_LoadMaterial_Proxy( 
    ITest3DEngine * This,
    /* [in] */ BSTR strFileName);


void __RPC_STUB ITest3DEngine_LoadMaterial_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ITest3DEngine_LoadAnimation_Proxy( 
    ITest3DEngine * This,
    /* [in] */ BSTR strFileName,
    BOOL bLoopPlay);


void __RPC_STUB ITest3DEngine_LoadAnimation_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ITest3DEngine_LoadPluginModel_Proxy( 
    ITest3DEngine * This,
    /* [in] */ BSTR strFileName,
    BSTR strBindDest);


void __RPC_STUB ITest3DEngine_LoadPluginModel_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ITest3DEngine_ClearPluginModel_Proxy( 
    ITest3DEngine * This,
    /* [in] */ BSTR strBindDest);


void __RPC_STUB ITest3DEngine_ClearPluginModel_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ITest3DEngine_ActivateScene_Proxy( 
    ITest3DEngine * This,
    /* [in] */ BSTR editor_name);


void __RPC_STUB ITest3DEngine_ActivateScene_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ITest3DEngine_RemoveScene_Proxy( 
    ITest3DEngine * This,
    /* [in] */ BSTR editor_name);


void __RPC_STUB ITest3DEngine_RemoveScene_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ITest3DEngine_AddScene_Proxy( 
    ITest3DEngine * This,
    /* [in] */ LONG hwndOutput,
    /* [in] */ BSTR editor_name,
    /* [in] */ LONG iOption);


void __RPC_STUB ITest3DEngine_AddScene_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ITest3DEngine_GetEditorSceneCount_Proxy( 
    ITest3DEngine * This,
    /* [in] */ BSTR editor_name,
    /* [out] */ LONG *count);


void __RPC_STUB ITest3DEngine_GetEditorSceneCount_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ITest3DEngine_LoadSelectorSfxFile_Proxy( 
    ITest3DEngine * This,
    /* [in] */ BSTR sfx_name);


void __RPC_STUB ITest3DEngine_LoadSelectorSfxFile_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ITest3DEngine_ScaleSelectorSfx_Proxy( 
    ITest3DEngine * This,
    /* [in] */ FLOAT x,
    /* [in] */ FLOAT y,
    /* [in] */ FLOAT z);


void __RPC_STUB ITest3DEngine_ScaleSelectorSfx_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ITest3DEngine_LoadOneMap_NoOutputWindow_Proxy( 
    ITest3DEngine * This,
    /* [in] */ BSTR filename,
    /* [out] */ LONG *ptrScene);


void __RPC_STUB ITest3DEngine_LoadOneMap_NoOutputWindow_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ITest3DEngine_LoadBillboardSfx_Proxy( 
    ITest3DEngine * This,
    /* [in] */ BSTR filename);


void __RPC_STUB ITest3DEngine_LoadBillboardSfx_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ITest3DEngine_SetBillboardHeight_Proxy( 
    ITest3DEngine * This,
    /* [in] */ INT npc_height,
    /* [in] */ INT head_top_adjust);


void __RPC_STUB ITest3DEngine_SetBillboardHeight_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ITest3DEngine_ToggleRotateModelEditor_Proxy( 
    ITest3DEngine * This);


void __RPC_STUB ITest3DEngine_ToggleRotateModelEditor_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ITest3DEngine_SetCameraInfo_Proxy( 
    ITest3DEngine * This,
    LONG pX,
    LONG pY,
    LONG pZ,
    LONG lX,
    LONG lY,
    LONG lZ,
    LONG width,
    LONG height);


void __RPC_STUB ITest3DEngine_SetCameraInfo_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ITest3DEngine_GetCameraInfo_Proxy( 
    ITest3DEngine * This,
    /* [out] */ LONG *pX,
    /* [out] */ LONG *pY,
    /* [out] */ LONG *pZ,
    /* [out] */ LONG *lX,
    /* [out] */ LONG *lY,
    /* [out] */ LONG *lZ,
    /* [out] */ LONG *width,
    /* [out] */ LONG *height);


void __RPC_STUB ITest3DEngine_GetCameraInfo_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ITest3DEngine_AdjustCameraOrthogonal_Proxy( 
    ITest3DEngine * This,
    /* [in] */ LONG iOffset);


void __RPC_STUB ITest3DEngine_AdjustCameraOrthogonal_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ITest3DEngine_SetModelScale_Proxy( 
    ITest3DEngine * This,
    /* [in] */ FLOAT fScale);


void __RPC_STUB ITest3DEngine_SetModelScale_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ITest3DEngine_SaveModelToImageFile_Proxy( 
    ITest3DEngine * This,
    /* [in] */ BSTR fileName);


void __RPC_STUB ITest3DEngine_SaveModelToImageFile_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ITest3DEngine_LoadSkillSfx_Proxy( 
    ITest3DEngine * This,
    /* [in] */ BSTR filename,
    /* [in] */ FLOAT scale,
    /* [in] */ BSTR socketName,
    /* [in] */ LONG iIndex);


void __RPC_STUB ITest3DEngine_LoadSkillSfx_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ITest3DEngine_ReadMapTerrainInfo_Proxy( 
    ITest3DEngine * This,
    /* [in] */ LONG ptrScene,
    /* [out] */ LONG *width,
    /* [out] */ LONG *height);


void __RPC_STUB ITest3DEngine_ReadMapTerrainInfo_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ITest3DEngine_FileToImage_Proxy( 
    ITest3DEngine * This,
    /* [in] */ BSTR src3DFileName,
    /* [in] */ BSTR desImageFileName);


void __RPC_STUB ITest3DEngine_FileToImage_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ITest3DEngine_GetCurSceneIndex_Proxy( 
    ITest3DEngine * This,
    /* [out] */ LONG *index);


void __RPC_STUB ITest3DEngine_GetCurSceneIndex_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ITest3DEngine_ActivateSceneByIndex_Proxy( 
    ITest3DEngine * This,
    /* [in] */ LONG index);


void __RPC_STUB ITest3DEngine_ActivateSceneByIndex_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ITest3DEngine_SaveMdlWithAniToImages_Proxy( 
    ITest3DEngine * This,
    /* [in] */ LONG ptrModel,
    /* [in] */ BSTR filename);


void __RPC_STUB ITest3DEngine_SaveMdlWithAniToImages_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ITest3DEngine_SaveLogicScene_Proxy( 
    ITest3DEngine * This,
    /* [in] */ BSTR iniFile,
    /* [in] */ LONG ptr3DScene);


void __RPC_STUB ITest3DEngine_SaveLogicScene_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ITest3DEngine_ShowProgressForm_Proxy( 
    ITest3DEngine * This,
    /* [in] */ BOOL bShow);


void __RPC_STUB ITest3DEngine_ShowProgressForm_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ITest3DEngine_UnloadPlayerModel_Proxy( 
    ITest3DEngine * This);


void __RPC_STUB ITest3DEngine_UnloadPlayerModel_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ITest3DEngine_GetHeight_Proxy( 
    ITest3DEngine * This,
    /* [in] */ LONG ptrScene,
    /* [in] */ FLOAT x,
    /* [in] */ FLOAT z,
    /* [out] */ FLOAT *y);


void __RPC_STUB ITest3DEngine_GetHeight_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ITest3DEngine_GetFileNameHash_Proxy( 
    ITest3DEngine * This,
    /* [in] */ BSTR fileName,
    /* [out] */ DWORD *hashid);


void __RPC_STUB ITest3DEngine_GetFileNameHash_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ITest3DEngine_QuaternionRotationByNDir_Proxy( 
    ITest3DEngine * This,
    FLOAT *x,
    FLOAT *y,
    FLOAT *z,
    FLOAT *w,
    /* [in] */ LONG nDir);


void __RPC_STUB ITest3DEngine_QuaternionRotationByNDir_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ITest3DEngine_LoadPluginMaterial_Proxy( 
    ITest3DEngine * This,
    /* [in] */ BSTR strSocketName,
    /* [in] */ BSTR strMtlFile);


void __RPC_STUB ITest3DEngine_LoadPluginMaterial_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ITest3DEngine_GetAniFrameCount_Proxy( 
    ITest3DEngine * This,
    /* [in] */ BSTR strAniFileName,
    /* [retval][out] */ LONG *count);


void __RPC_STUB ITest3DEngine_GetAniFrameCount_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ITest3DEngine_LoadPlaneForSelector_Proxy( 
    ITest3DEngine * This,
    /* [in] */ BSTR strFileName);


void __RPC_STUB ITest3DEngine_LoadPlaneForSelector_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ITest3DEngine_SetCaptionHeight_Proxy( 
    ITest3DEngine * This,
    /* [in] */ FLOAT fHeight);


void __RPC_STUB ITest3DEngine_SetCaptionHeight_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ITest3DEngine_SetCaptionText_Proxy( 
    ITest3DEngine * This,
    /* [in] */ BSTR strName,
    /* [in] */ BSTR strTitle);


void __RPC_STUB ITest3DEngine_SetCaptionText_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ITest3DEngine_GetBBoxXY_Proxy( 
    ITest3DEngine * This,
    /* [in] */ BSTR mdl,
    /* [out] */ FLOAT *x,
    /* [out] */ FLOAT *y);


void __RPC_STUB ITest3DEngine_GetBBoxXY_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ITest3DEngine_GetMdlBBoxMax2DLength_Proxy( 
    ITest3DEngine * This,
    /* [in] */ BSTR mdl,
    /* [retval][out] */ FLOAT *pVal);


void __RPC_STUB ITest3DEngine_GetMdlBBoxMax2DLength_Stub(
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


