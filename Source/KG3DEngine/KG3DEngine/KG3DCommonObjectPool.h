////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KG3DCommonObjectPool.h
//  Version     : 1.0
//  Creator     : Chen Tianhong
//  Create Date : 2008-4-10 17:00:20
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _INCLUDE_KG3DCOMMONOBJECTPOOL_H_
#define _INCLUDE_KG3DCOMMONOBJECTPOOL_H_
#include "KG3DCommonObject.h"
////////////////////////////////////////////////////////////////////////////////
//这个类只是为了自动释放方便而已
class KG3DCommonObjectPool : public KG3DCommonObjectSimple
{
public:
	STDMETHOD_(ULONG, GetType)();
	STDMETHOD_(ULONG, GetContainedType)();

	STDMETHOD_(ULONG, GetCount)();
	STDMETHOD(Push)(KG3DCommonObject* p, KG3DCOBJ_HANDLE* pObjectRet);
	STDMETHOD_(KG3DCommonObject*, Get)(const KG3DCOBJ_HANDLE& Handle);//用类型得到，不是顺序
	STDMETHOD(Del)(const KG3DCOBJ_HANDLE& Handle);
	STDMETHOD(Clear)();
	
	STDMETHOD(Traverse)(KG3DCommonObjectVisitor& visitor);//返回值由visitor控制，过程由visitor返回E_FAILED打断

	/*加速表，用于加速对象的Get，用于经常Get的对象。
	在内部保存一个加速表指针，用FastGet方法来访问内部的数据。如
	定义了加速表
	KG3DCOBJ_HANDLE AccelaratorTable[] = {KG3DCOBJ_HANDLE(1000, 0), KG3DCOBJ_HANDLE(1000, 1),};
	那么当对应Handle的物件被创建之后，GetFast(0)能高速取得对应Handle(1000, 0)物件。
	注意后面的顺序在插入等的操作时会被修改，所以加速表最好用于只有一个的成员
	*/
	STDMETHOD(InitAccelaratorTable)(KG3DCOBJ_HANDLE* pTable, size_t uTableCount);
	STDMETHOD_(KG3DCommonObject*, GetFast)(size_t iPos);

	KG3DCommonObjectPool();
	~KG3DCommonObjectPool();
	
private:
	typedef std::vector<KG3DCommonObject*> TypeVec;
	typedef std::map<DWORD, TypeVec> TypeCon;
	TypeCon	m_Con;
	ULONG	m_Count;

	typedef std::map<KG3DCOBJ_HANDLE, size_t> TypeAccelaratorPoses;
	struct AccelaratorData 
	{
		TypeAccelaratorPoses m_mapOfPos;
		std::vector<KG3DCommonObject*>	m_vecObj;
	};
	AccelaratorData* m_pAccelaratorData;
};

class KG3DCommonObjectPoolDummy : public KG3DCommonObjectPool
{
	STDMETHOD(Push)(KG3DCommonObject* p, KG3DCOBJ_HANDLE* pObjectRet){return E_FAIL;}
};
extern KG3DCommonObjectPool& g_GetKG3DCommonObjectPoolDummy();


#endif //_INCLUDE_KG3DCOMMONOBJECTPOOL_H_
