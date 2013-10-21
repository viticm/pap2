////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KG3DCommonObjectContainer.h
//  Version     : 1.0
//  Creator     : Chen Tianhong
//  Create Date : 2008-3-6 14:21:32
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _INCLUDE_KG3DCOMMONOBJECTCONTAINER_H_
#define _INCLUDE_KG3DCOMMONOBJECTCONTAINER_H_
#include "IEKG3DCommonContainer.h"
#include "KG3DMessenger.h"
#include "KG3DCommonObject.h"
////////////////////////////////////////////////////////////////////////////////
class KG3DCommonContainer;

//这个类是尽量按照ATL的格式写的，下面的类用在引擎中就不需要那么严格，不然不好写,THIS和THIS_都不是必须的
DECLARE_INTERFACE_(KG3DCommonContainer, IEKG3DCommonContainer)
{
	STDMETHOD_(ULONG, GetType)(THIS) PURE;
	STDMETHOD_(LPVOID, ToAnotherInterface)(THIS_ DWORD_PTR Param) PURE;

	STDMETHOD_(ULONG, GetContainedType)(THIS)PURE;
	STDMETHOD_(KG3DCommonContainer*, GetContainerInterface)(THIS)PURE;

	STDMETHOD_(ULONG, GetCount)(THIS)PURE;

	STDMETHOD_(KG3DCommonObject*, GetFirst)(THIS_ KG3DCOBJ_HANDLE* pdwFirstHandle)PURE;
	STDMETHOD_(KG3DCommonObject*, GetNext)(THIS_ KG3DCOBJ_HANDLE Handle, KG3DCOBJ_HANDLE* pNextHandle)PURE;///返回的是下一个的Handle
	STDMETHOD_(KG3DCommonObject*, Get)(THIS_ ULONG Index)PURE;

	STDMETHOD(GetFirst)(THIS_ IEKG3DCommonObject** ppObj, KG3DCOBJ_HANDLE* pdwFirstHandle)PURE;
	STDMETHOD(GetNext)(THIS_ KG3DCOBJ_HANDLE Handle, IEKG3DCommonObject** ppObj, KG3DCOBJ_HANDLE* pNextHandle) PURE;///返回的是下一个的Handle
	STDMETHOD(Get)(THIS_ ULONG Index, IEKG3DCommonObject** ppObj)PURE;

	STDMETHOD_(KG3DCommonObject*, Add)(THIS_ LPTSTR lpFileName, DWORD_PTR WParam, DWORD_PTR LParam, KG3DCOBJ_HANDLE* pHandle)PURE;
	STDMETHOD(Add)(THIS_ LPTSTR lpFileName, DWORD_PTR WParam, DWORD_PTR LParam, IEKG3DCommonObject** ppRet, KG3DCOBJ_HANDLE* pHandle)PURE;
	STDMETHOD(Del)(THIS_ DWORD_PTR dwHandle)PURE;
	STDMETHOD(Clear)(THIS)PURE;

	STDMETHOD(GetMessageSenderInterface)(THIS_ IKG3DMessageSenderBase** ppRet)PURE;//因为通常需要Container通知一些信息用于刷新界面，所以需要有这个

#if defined(DEBUG) | defined(_DEBUG)	//禁止所有形式的delete的调用
#ifdef __cplusplus
private:
	void operator delete(void* _Ptr) throw( );
	void operator delete(void *, void *) throw( );
	void operator delete(void* _Ptr,const std::nothrow_t&) throw( );
	void operator delete[](void* _Ptr) throw( );
	void operator delete[](void *, void *) throw( );
	void operator delete[](void* _Ptr, const std::nothrow_t&) throw( );
#endif
#endif
};

class KG3DCommonContainerSimple : public KG3DCommonObjectSimple, public KG3DCommonContainer
{
public:
	virtual ~KG3DCommonContainerSimple() = 0{}

	//必须实现
	STDMETHOD_(ULONG, GetType)() PURE;
	STDMETHOD_(ULONG, GetContainedType)()PURE;
	STDMETHOD_(ULONG, GetCount)() PURE;
	STDMETHOD_(KG3DCommonObject*, Add)(LPTSTR lpFileName, DWORD_PTR WParam, DWORD_PTR LParam, KG3DCOBJ_HANDLE* pHandle)PURE;
	
	STDMETHOD(Del)(DWORD_PTR dwHandle)PURE;
	STDMETHOD(Clear)()PURE;
	
	//应该实现，选择实现FirstNext形式的遍历，或者Get的遍历
	STDMETHOD_(KG3DCommonObject*, GetFirst)(KG3DCOBJ_HANDLE* pdwFirstHandle){return NULL;}
	STDMETHOD_(KG3DCommonObject*, GetNext)(KG3DCOBJ_HANDLE Handle, KG3DCOBJ_HANDLE* pNextHandle)	{return NULL;}///返回的是下一个的Handle
	STDMETHOD_(KG3DCommonObject*, Get)(ULONG Index)	{return NULL;}

	//下面的方法选择实现
protected:
	STDMETHOD(Init)()		{return S_OK;}
	STDMETHOD(UnInit)()		{return S_OK;}	

public:
	STDMETHOD_(LPVOID, ToAnotherInterface)(DWORD_PTR Param)	{return NULL;}//得到另外一种界面，如果没有就不用实现	

	//帮忙转移调用，不用覆盖
	STDMETHOD(GetFirst)(IEKG3DCommonObject** ppObj, KG3DCOBJ_HANDLE* pdwFirstHandle);
	STDMETHOD(GetNext)(KG3DCOBJ_HANDLE Handle, IEKG3DCommonObject** ppObj, KG3DCOBJ_HANDLE* pNextHandle);///返回的是下一个的Handle
	STDMETHOD(Get)(ULONG Index, IEKG3DCommonObject** ppObj);
	STDMETHOD(Add)(LPTSTR lpFileName, DWORD_PTR WParam, DWORD_PTR LParam, IEKG3DCommonObject** ppRet, KG3DCOBJ_HANDLE* pHandle);

	STDMETHOD(GetMessageSenderInterface)(IKG3DMessageSenderBase** ppRet);

	//Messenger机制，如果有，就Return	
	STDMETHOD_(IKG3DMessageSenderBase*, GetMessageSenderInterface)(){return NULL;}//因为通常需要Container通知一些信息用于刷新界面，所以需要有这个

	//属性
	STDMETHOD_(KG3DCommonContainer*, GetContainerInterface)(){return this;}

	//简单生命模型
	virtual HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void **ppvObject){return E_FAIL;}
	virtual ULONG STDMETHODCALLTYPE AddRef( void){return 1;}
	virtual ULONG STDMETHODCALLTYPE Release( void);

#if defined(DEBUG) | defined(_DEBUG)	//禁止使用delete
private:
	using KG3DCommonObject::operator delete;
	using KG3DCommonObject::operator delete[];
#endif
	
};


#ifdef __cplusplus
//用于测试或者虚拟行为
struct  KG3DDummyCommonContainer : public KG3DCommonContainerSimple
{
	STDMETHOD_(ULONG, GetType)();
	STDMETHOD_(ULONG, GetContainedType)(){return UINT_MAX;}
	STDMETHOD_(ULONG, GetCount)(){return 0;}
	STDMETHOD_(KG3DCommonObject*, Add)(LPTSTR lpFileName, DWORD_PTR WParam, DWORD_PTR LParam, KG3DCOBJ_HANDLE* pHandle){return NULL;}

	STDMETHOD(Del)(DWORD_PTR dwHandle){return E_FAIL;}
	STDMETHOD(Clear)(){return E_FAIL;}
};
#endif

#endif //_INCLUDE_KG3DCOMMONOBJECTCONTAINER_H_
