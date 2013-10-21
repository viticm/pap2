#include "stdafx.h"
#include "RInterface.h"
#include <string.h>


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

////////////////////////////////////////////////////////////////////////////////

// ------------------------------------------------------------------------------------ //
// InterfaceReg.
// ------------------------------------------------------------------------------------ //
InterfaceReg *InterfaceReg::s_pInterfaceRegs = NULL;


InterfaceReg::InterfaceReg( InstantiateInterfaceFn fn, const char *pName ) :
	m_pName(pName)
{
	m_CreateFn = fn;
	m_pNext = s_pInterfaceRegs;
	s_pInterfaceRegs = this;
}



// ------------------------------------------------------------------------------------ //
// 每个模块的接口工厂
// pName:接口实例的名字
// 返回接口实例指针
// ------------------------------------------------------------------------------------ //
void* CreateInterface( const char *pName)
{
	InterfaceReg *pCur;

	for(pCur=InterfaceReg::s_pInterfaceRegs; pCur; pCur=pCur->m_pNext)
	{
		if (strcmp(pCur->m_pName, pName) == 0)
		{
			return pCur->m_CreateFn();
		}
	}

	return NULL;
}

// 从模块句柄取得接口工厂函数指针
// hModule:模块句柄
// 返回模块接口工厂函数指针
CreateInterfaceFn Sys_GetFactory( HMODULE hModule )
{
	if ( !hModule )
		return NULL;

	return reinterpret_cast<CreateInterfaceFn>(GetProcAddress( hModule, CREATEINTERFACE_PROCNAME ));
}

// 取得本模块的接口工厂
CreateInterfaceFn Sys_GetFactoryThis( void )
{
	return CreateInterface;
}

