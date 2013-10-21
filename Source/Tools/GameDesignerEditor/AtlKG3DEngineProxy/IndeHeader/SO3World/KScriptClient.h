#ifndef _KSCRIPTCLIENT_H_ 
#define _KSCRIPTCLIENT_H_ 

#ifdef _CLIENT

#include <set>
#include <string>
#include "Engine/KMemory.h"

class KScriptClient 
{
public:
    KScriptClient();

    BOOL Setup();
    BOOL Accept(const char cszFunction[], BYTE* pbyData, size_t uDataLen);

    int  Call(Lua_State* L);

private:
    DWORD   m_dwScript;
    BYTE    m_byParamData[1024];
};

#endif

#endif
