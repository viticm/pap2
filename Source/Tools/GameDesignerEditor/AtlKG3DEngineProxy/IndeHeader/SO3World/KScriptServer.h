#ifndef _KSCRIPTSERVER_H_  
#define _KSCRIPTSERVER_H_  

#ifdef _SERVER

#include <set>
#include <string>
#include "Engine/KMemory.h"

class KScriptServer
{
public:
    KScriptServer();

    BOOL Setup();
    BOOL IsFunctionRegistered(const char cszFunction[]);

    BOOL Accept(KPlayer* pPlayer, const char cszFunction[], BYTE* pbyData, size_t uDataLen);

    int  Call(Lua_State* L);

private:
    DWORD   m_dwScript;
    BYTE    m_byParamData[1024];

    typedef std::set<kstring, std::less<kstring>, KMemory::KAllocator<kstring> > KREGISTER_TABLE;
    KREGISTER_TABLE m_RegisterTable;

public:
    int  LuaRegister(Lua_State* L);

    DECLARE_LUA_CLASS(KScriptServer);
};

#endif

#endif
