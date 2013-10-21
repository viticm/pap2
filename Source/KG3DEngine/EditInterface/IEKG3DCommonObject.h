////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : IEKG3DCommonObject.h
//  Version     : 1.0
//  Creator     : Chen Tianhong
//  Create Date : 2008-3-6 9:49:32
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _INCLUDE_IEKG3DCOMMONOBJECT_H_
#define _INCLUDE_IEKG3DCOMMONOBJECT_H_

#include "d3d9.h"
////////////////////////////////////////////////////////////////////////////////
DECLARE_INTERFACE_(IEKG3DCommonObject, IUnknown)
{
	STDMETHOD_(ULONG, GetType)(THIS) PURE;
	STDMETHOD_(LPVOID, ToAnotherInterface)(THIS_ DWORD_PTR Param) PURE;	//得到另外一种接口，通常用于IE接口
};

typedef struct _KG3DCOBJ_HANDLE
{
	DWORD m_dwType;
	INT   m_nIndex;
#ifdef __cplusplus
	_KG3DCOBJ_HANDLE():m_dwType(0), m_nIndex(0){}///有构造函数的话，可能拷贝构造就不对了??
	_KG3DCOBJ_HANDLE(DWORD dwType, INT nIndex):m_dwType(dwType),m_nIndex(nIndex){}
	_KG3DCOBJ_HANDLE(const _KG3DCOBJ_HANDLE& Other):m_dwType(Other.m_dwType), m_nIndex(Other.m_nIndex){}
	_KG3DCOBJ_HANDLE& operator=(const _KG3DCOBJ_HANDLE& Other){m_dwType = Other.m_dwType; m_nIndex = Other.m_nIndex;return *this;}
	bool operator==(const _KG3DCOBJ_HANDLE& Other){return 0 == memcmp(this, &Other, sizeof(_KG3DCOBJ_HANDLE));}
	friend bool operator<(const _KG3DCOBJ_HANDLE& A, const _KG3DCOBJ_HANDLE& B)
	{
		return A.m_dwType < B.m_dwType ? true : (A.m_dwType == B.m_dwType ? (A.m_nIndex < B.m_nIndex) : false);
	}
#endif
}KG3DCOBJ_HANDLE;

#endif //_INCLUDE_IEKG3DCOMMONOBJECT_H_
