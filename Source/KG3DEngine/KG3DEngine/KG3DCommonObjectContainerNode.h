////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KG3DCommonObjectContainerNode.h
//  Version     : 1.0
//  Creator     : Chen Tianhong
//  Create Date : 2008-6-25 9:56:47
//  Comment     : 单一创建的类型，需要Build和Save指针，可选
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _INCLUDE_KG3DCOMMONOBJECTCONTAINERNODE_H_
#define _INCLUDE_KG3DCOMMONOBJECTCONTAINERNODE_H_
#include "KG3DCommonObjectPool.h"
#include "KG3DCommonObjectBuilder.h"
#include "KG3DCommonObjectNode.h"
////////////////////////////////////////////////////////////////////////////////
class KG3DCommonObjectContainerNode : public KG3DCommonObjectNodeSimple
{
public:
	STDMETHOD_(ULONG, GetType)();
	
	//必须实现
	STDMETHOD_(ULONG, GetContainedType)() = 0;//装的什么类型
	//virtual HRESULT Save(const KG3DCOBJ_HANDLE& Handle, LPCTSTR FileName, DWORD_PTR wParam, DWORD_PTR lParam) = 0;
	
	STDMETHOD_(ULONG, GetCount)();
	STDMETHOD_(KG3DCommonObject*, Add)(LPTSTR lpFileName, DWORD_PTR WParam, DWORD_PTR LParam, KG3DCOBJ_HANDLE* pHandle);
	STDMETHOD(Del)(KG3DCOBJ_HANDLE Handle);
	STDMETHOD(Clear)();	
	STDMETHOD_(KG3DCommonObject*, Get)(const KG3DCOBJ_HANDLE& Handle);
	STDMETHOD_(KG3DCommonObjectPool&,GetCommonObjectPool)(){return m_Pool;}

	KG3DCommonObjectContainerNode(TypeFuncCommonObjectBuilder FuncBuilder, TypeFuncCommonObjectSaver FuncSaver)
		:m_Builder(m_Builder), m_Saver(FuncSaver){}	
private:
	TypeFuncCommonObjectBuilder	m_Builder;
	TypeFuncCommonObjectSaver	m_Saver;
	KG3DCommonObjectPool	m_Pool;
};

#endif //_INCLUDE_KG3DCOMMONOBJECTCONTAINERNODE_H_
