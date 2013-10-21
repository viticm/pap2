////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : kscript.h
//  Version     : 1.0
//  Creator     : Hu Changyin
//  Create Date : 2007-12-7 11:02:08
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _INCLUDE_KSCRIPT_H_
#define _INCLUDE_KSCRIPT_H_

////////////////////////////////////////////////////////////////////////////////


template <typename T>
bool TNewUserData(lua_State* L, T* pUserData, LPCSTR szMetatable)
{
	ASSERT(szMetatable);

	T** ppUserData = reinterpret_cast<T**>(lua_newuserdata(L, sizeof(T*)));
	KGLOG_PROCESS_ERROR(ppUserData);

	*ppUserData = pUserData;

	luaL_getmetatable(L, szMetatable);
	lua_setmetatable(L, -2);

	return true;
Exit0:
	return false;
}


#endif //_INCLUDE_KSCRIPT_H_
