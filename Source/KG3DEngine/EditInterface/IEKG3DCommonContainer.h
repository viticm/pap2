////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : IEKG3DCommonContainer.h
//  Version     : 1.0
//  Creator     : Chen Tianhong
//  Create Date : 2008-3-5 15:52:53
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _INCLUDE_IEKG3DCOMMONCONTAINER_H_
#define _INCLUDE_IEKG3DCOMMONCONTAINER_H_
////////////////////////////////////////////////////////////////////////////////

#include "IEKG3DCommonObject.h"
struct IEKG3DCommonObject;
struct IKG3DMessageSenderBase;
DECLARE_INTERFACE_(IEKG3DCommonContainer, IEKG3DCommonObject)
{
	STDMETHOD_(ULONG, GetType)(THIS) PURE;
	STDMETHOD_(LPVOID, ToAnotherInterface)(THIS_ DWORD_PTR Param) PURE;
	STDMETHOD_(ULONG, GetContainedType)(THIS)PURE;

	STDMETHOD_(ULONG, GetCount)(THIS) PURE;
	STDMETHOD(GetFirst)(THIS_ IEKG3DCommonObject** ppObj, KG3DCOBJ_HANDLE* pdwFirstHandle)PURE;
	STDMETHOD(GetNext)(THIS_ KG3DCOBJ_HANDLE Handle, IEKG3DCommonObject** ppObj, KG3DCOBJ_HANDLE* pNextHandle) PURE;///返回的是下一个的Handle
	STDMETHOD(Get)(THIS_ ULONG Index, IEKG3DCommonObject** ppObj)PURE;
	STDMETHOD(Add)(THIS_ LPTSTR lpFileName, DWORD_PTR WParam, DWORD_PTR LParam, IEKG3DCommonObject** ppRet, KG3DCOBJ_HANDLE* pHandle)PURE;
	STDMETHOD(Del)(THIS_ DWORD_PTR dwHandle)PURE;
	STDMETHOD(Clear)(THIS)PURE;

	STDMETHOD(GetMessageSenderInterface)(THIS_ IKG3DMessageSenderBase** ppRet)PURE;//因为通常需要Container通知一些信息用于刷新界面，所以需要有这个
};




#endif //_INCLUDE_IEKG3DCOMMONCONTAINER_H_
