////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KG3DRepresentObjectBuilder.h
//  Version     : 1.0
//  Creator     : Chen Tianhong
//  Create Date : 2008-4-3 15:42:17
/*  Comment     : 和ObjectTable不同，这个类能直接创建出绑定好内部物件的RepresentObject。
和KG3DRepresentObjectTable创建的是空的Object。这个功能之所以要分开是因为不同RepresentObject
会挂接不同的类，如果直接在KG3DRepresentObjectTable创建完整的（如带Model）的RepresentObject，
KG3DRepresentObjectTable就需要和别的创建器挂接起来了，从而形成了耦合。

KG3DRepresentObjectBuilder能够直接创建完整的RepresentObject连带里面的模型或者SoundBall。

使用方式：
KG3DRepresentObjectBuilder::Build(pFile, REPRESENTOBJECT_DEFAULT, 0, 0, &pObj);
或者通过g_cEngineManager.BuildKG3DRepresentObject,这样不用包含这个头文件。

如果传入pFile，那么类型能直接通过pFile确认。也可以不用这个机制，只要写对第二个参数就可以了
如果不传入pFile,直接创建出类型。对于有些Obj，是不需要文件的。


要使用这个机制，需要
1.KG3DRepresentObjectTable的GetTypeInfoByFileName能通过文件名辨别出创建哪种类型的Object。
因为已经有现成的机制了，只需要在KG3DRepresentObjectTable的Init函数中填写stTypeInfo那个表

2.确保KG3DRepresentObjectTable的Get1NewResourcePointer能够创建出那个类型的类。直接写
switch case也行，更好的是通过KG3DCollocator，用KG3DRepresentObjectTable::RegisterType来注册一个类型
这样就不用KG3DRepresentObjectTable和别的文件耦合起来，KG3DCollocator怎么耦合都无所谓

3.重载KG3DRepresentObjectTable的Load函数。其中wParam,lParam是DWORD_PTR类型的，可以传入
指针，或者别的内建类型，足够放构造信息了。

*/
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _INCLUDE_KG3DREPRESENTOBJECTBUILDER_H_
#define _INCLUDE_KG3DREPRESENTOBJECTBUILDER_H_
class KG3DRepresentObject;
class IEKG3DRepresentObject;
////////////////////////////////////////////////////////////////////////////////
class KG3DRepresentObjectBuilder
{
public:
	/*
	Build是万用函数，如果lpFileName不为NULL，那么Type不是必须的，会根据FileName来判断类型，
	有些东西是不需要文件的，则必须有Type，不需要FileName
	*/
	static HRESULT Build(LPCTSTR lpFileName, DWORD dwType, DWORD_PTR wParam, DWORD_PTR lParam, KG3DRepresentObject** ppRet);
	static HRESULT Build(LPCTSTR lpFileName, DWORD dwType, DWORD_PTR wParam, DWORD_PTR lParam, IEKG3DRepresentObject** ppRet);
	static HRESULT New(DWORD dwType, DWORD_PTR wParam, DWORD_PTR lParam, KG3DRepresentObject** ppRet);
};

#endif //_INCLUDE_KG3DREPRESENTOBJECTBUILDER_H_
