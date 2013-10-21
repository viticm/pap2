////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KG3DCommonObject.h
//  Version     : 1.0
//  Creator     : Chen Tianhong
//  Create Date : 2008-3-6 14:21:04
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _INCLUDE_KG3DCOMMONOBJECT_H_
#define _INCLUDE_KG3DCOMMONOBJECT_H_


#include "IEKG3DCommonObject.h"

class KG3DCommonContainer;

//这个类是尽量按照ATL的格式写的，下面的类用在引擎中就不需要那么严格，不然不好写,THIS和THIS_都不是必须的
DECLARE_INTERFACE_(KG3DCommonObject, IEKG3DCommonObject)
{
	STDMETHOD_(ULONG, GetType)(THIS) PURE;
	STDMETHOD_(LPVOID, ToAnotherInterface)(THIS_ DWORD_PTR Param) PURE;

	//渲染相关
	STDMETHOD(OnLostDevice)(THIS) PURE;	//对于有资源的东西通常都需要设备恢复
	STDMETHOD(OnResetDevice)(THIS) PURE;

	STDMETHOD(FrameMove)(THIS) PURE;
	STDMETHOD(Render)(THIS) PURE;

	//属性
	STDMETHOD_(KG3DCommonContainer*, GetContainerInterface)(THIS) PURE;	//用虚函数转换比dynamic_cast快很多

	KG_DECLARE_NO_DELETE_OP();//如果编译提示这里，表明你对这个类错误的用了delete操作，应该用Release
};

//带很多默认实现的KG3DCommonObject基类,简单生命周期模型，一次Release就释放，AddRef无效果
class KG3DCommonObjectSimple : public KG3DCommonObject
{
public:
	STDMETHOD_(ULONG, GetType)() PURE;
	STDMETHOD_(LPVOID, ToAnotherInterface)(DWORD_PTR Param){return NULL;}

	//默认情况下，Init必须自己在Build的时候调用，外部不调用Init
	STDMETHOD(Init)(){return S_OK;}	//对于有资源的东西需要延迟初始化，Init是需要自己在Build函数中调用的，外部不调用Init。Init应该能检查出重复Init然后assert
	STDMETHOD(UnInit)(){return S_OK;} //应该能够重复调用而不出错

	STDMETHOD(OnLostDevice)(){return S_OK;}
	STDMETHOD(OnResetDevice)(){return S_OK;}

	STDMETHOD(FrameMove)(){return S_OK;}
	STDMETHOD(Render)(){return S_OK;}

	STDMETHOD_(KG3DCommonContainer*, GetContainerInterface)(){return NULL;}

	//这个类使用的是简单Release模型，AddRef没有效果Release直接删除
	virtual HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void **ppvObject){return E_FAIL;}
	virtual ULONG STDMETHODCALLTYPE AddRef( void){return 1;}
	virtual ULONG STDMETHODCALLTYPE Release( void);
	
	virtual ~KG3DCommonObjectSimple() = 0 {}

	KG_DECLARE_NO_DELETE_OP();//如果编译提示这里，表明你对这个类错误的用了delete操作，应该用Release
};
//带很多默认实现且带引用计数的KG3DCommonObject基类
class KG3DCommonObjectSimpleRef : public KG3DCommonObjectSimple
{
public:
	STDMETHOD_(ULONG, GetType)() PURE;

	//引用计数机制已经内部实现，不过可以不管，如果生命周期不是用引用计数的形式
	virtual HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void **ppvObject){return E_FAIL;}
	virtual ULONG STDMETHODCALLTYPE AddRef( void);
	virtual ULONG STDMETHODCALLTYPE Release( void);	//自己保证Unit的调用，默认应该在Release里面调UnInit

	KG_DECLARE_NO_DELETE_OP();//如果编译提示这里，表明你对这个类错误的用了delete操作，应该用Release
protected:
	KG3DCommonObjectSimpleRef():m_ulRefCount(1){}
	ULONG	m_ulRefCount;
};

struct KG3DCommonObjectVisitor 
{
	virtual HRESULT Accept(KG3DCommonObject& Obj) = 0;
	virtual ~KG3DCommonObjectVisitor() = 0{}
};

class KG3DCommonObjectDummy : public KG3DCommonObjectSimpleRef
{
	STDMETHOD_(ULONG, GetType)(THIS);
};



#endif //_INCLUDE_KG3DCOMMONOBJECT_H_
