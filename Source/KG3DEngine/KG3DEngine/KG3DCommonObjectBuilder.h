////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KG3DCommonObjectBuilder.h
//  Version     : 1.0
//  Creator     : Chen Tianhong
//  Create Date : 2008-4-10 16:27:18
//  Comment     : 用于IOC模式
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _INCLUDE_KG3DCOMMONOBJECTBUILDER_H_
#define _INCLUDE_KG3DCOMMONOBJECTBUILDER_H_

////////////////////////////////////////////////////////////////////////////////
class KG3DCommonObject;
//创建函数
typedef KG3DCommonObject*	(*TypeFuncCommonObjectBuilder)(LPCTSTR strFileName, DWORD_PTR WParam, DWORD_PTR LParam);//约定第一参数如果是NULL就Create，Init必须自己在Build里面调用
//保存函数
typedef HRESULT	(*TypeFuncCommonObjectSaver)(KG3DCommonObject* p, LPCTSTR strFileName, DWORD_PTR WParam, DWORD_PTR LParam);

class KG3DCommonObjectBuilder
{
public:
	//生成器注入机制，生成器和这个类分离，采用注入机制，而不是成员的方式硬耦合，参考CPP中RegisterContainerType前面的注释
	HRESULT	RegisterType(DWORD dwType, TypeFuncCommonObjectBuilder pFuncBuilder
		, TypeFuncCommonObjectSaver pSaver);	//pFuncBuilder必须有，而Saver是可选的
	HRESULT UnRegisterType(DWORD dwType, TypeFuncCommonObjectBuilder* pOldBuilder
		, TypeFuncCommonObjectSaver* pOldSaver);//返回旧的

	ULONG	IsTypeRegister(DWORD dwType);

	KG3DCommonObject* Build(DWORD dwType, LPCTSTR strFileName, DWORD_PTR wParam, DWORD_PTR lParam);
	HRESULT	Save(KG3DCommonObject* p,LPCTSTR strFileName, DWORD_PTR wParamToSave, DWORD_PTR lParamToSave);
private:
	struct TypeConConElem 
	{
		TypeFuncCommonObjectBuilder	pFuncBuilder;
		TypeFuncCommonObjectSaver	pFuncSaver;
		TypeConConElem():pFuncBuilder(NULL), pFuncSaver(NULL){}
	};
	typedef std::map<DWORD, TypeConConElem>	TypeContainerContainer;
	TypeContainerContainer m_ConCon;
	HRESULT ClearContainerContainer();
};

#endif //_INCLUDE_KG3DCOMMONOBJECTBUILDER_H_
