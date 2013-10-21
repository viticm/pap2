//******************************************************************************
// 决战项目服务器接口定义头文件
// 创建时间: 2003-11-02
//
// 决战项目定义接口协议的头文件
// 生成并输出一个接口的步骤:
//    1. 接口的所有成员函数必须为纯虚函数,且接口没有数据成员.
//    2. 为接口实例起一个名字,如"Net1.0".
//    3. 在接口的实现文件里, 使用 EXPOSE_INTERFACE 或 EXPOSE_SINGLE_INTERFACE.
//
// 版本控制
// There are two versioning cases that are handled by this:
// 1. 你在一个接口上增加了新的功能形成一个新的接口, 所以它也是二进制兼容前一个接
//    口的. 这种情况下, 你需要写两个 EXPOSE_INTERFACEs: 一个输出你的类为以前的接
//    口,一个输出为新接口
// 2. 你修改了一个接口使它与以前不再兼容 (但你仍然希望为兼容以前的代码输出旧接口
//    ). 这种情况下, 你需要为你的新接口起一个新版本的名字, 并且生成一个代理接口,
//    输出它为老接口
//******************************************************************************

#ifndef _RINTERFACE_H_
#define _RINTERFACE_H_

#include "DLLBase.h" 
#include "windows.h"

#define CREATEINTERFACE_PROCNAME	"CreateInterface"
typedef void* (*CreateInterfaceFn)(const char *pName);

// 每个接口实现类生成接口实例的函数类型
typedef void* (*InstantiateInterfaceFn)();


// 用于注册接口实例的数据结构
class InterfaceReg
{
public:
	InterfaceReg(InstantiateInterfaceFn fn, const char *pName);

public:

	InstantiateInterfaceFn	m_CreateFn;
	const char				*m_pName;

	InterfaceReg			*m_pNext; // For the global list.
	static InterfaceReg		*s_pInterfaceRegs;
};


// 用于输出接口,可以有多个实例
// 如:
// EXPOSE_INTERFACE( CInterfaceImp, IInterface, "MyInterface001" )
// 输出类 CInterfaceImp 实现 IInterface (a pure class)
// 客户可以调用 CreateInterface( "MyInterface001" ) 来取得这个实例指针
//
// 接口的头文件定义接口 (IInterface) 和版本名字 ("MyInterface001")
//
// 单个类可以通过多重继承实现多个接口
//
// 如果你想自己写创建函数,使用这个宏
#define EXPOSE_INTERFACE_FN(functionName, interfaceName, versionName) \
	static InterfaceReg __g_Create##interfaceName##_reg(functionName, versionName);

// 自动生成创建函数,查询接口时自动生成接口实例
#define EXPOSE_INTERFACE(className, interfaceName, versionName) \
	static void* __Create##className##_interface() {return (interfaceName *)new className;}\
	static InterfaceReg __g_Create##className##_reg(__Create##className##_interface, versionName );

// 通过你定义的全局变量输出单个接口
#define EXPOSE_SINGLE_INTERFACE_GLOBALVAR(className, interfaceName, versionName, globalVarName) \
	static void* __Create##className##interfaceName##_interface() {return (interfaceName *)&globalVarName;}\
	static InterfaceReg __g_Create##className##interfaceName##_reg(__Create##className##interfaceName##_interface, versionName);

// 输出单个接口,自动定义全局变量
#define EXPOSE_SINGLE_INTERFACE(className, interfaceName, versionName) \
	static className __g_##className##_singleton;\
	EXPOSE_SINGLE_INTERFACE_GLOBALVAR(className, interfaceName, versionName, __g_##className##_singleton)

// 每个模块的接口工厂
// pName:接口实例的名字
// 返回接口实例指针
DLL_EXPORT void* CreateInterface(const char *pName);

// 取得本模块的接口工厂
extern CreateInterfaceFn Sys_GetFactoryThis( void );

// 从模块句柄取得接口工厂函数指针
// hModule:模块句柄
// 返回模块接口工厂函数指针
extern CreateInterfaceFn Sys_GetFactory( HMODULE hModule );


#endif



