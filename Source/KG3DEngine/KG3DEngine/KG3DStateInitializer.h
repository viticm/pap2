////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KG3DStateInitializer.h
//  Version     : 1.0
//  Creator     : Chen Tianhong
//  Create Date : 2008-3-6 11:01:16
//  Comment     : 用于配置状态，譬如连接各个构造器。这样子就不用把头文件都包含在比较复杂的类中
//					而是用这个比较简单的类来配置比较复杂的类
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _INCLUDE_KG3DSTATEINITIALIZER_H_
#define _INCLUDE_KG3DSTATEINITIALIZER_H_

////////////////////////////////////////////////////////////////////////////////

class KG3DScene;
class KG3DEngineManager;
struct KG3DStateInitializer
{
	static VOID	InitScene(KG3DScene& Scene);
};

#endif //_INCLUDE_KG3DSTATEINITIALIZER_H_
