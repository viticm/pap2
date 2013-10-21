#ifndef _KGLOBAL_SYSTEM_VALUE_CACHE_H_
#define _KGLOBAL_SYSTEM_VALUE_CACHE_H_

#ifdef _SERVER

#include "Luna.h"

class KGlobalSystemValueCache
{
public:
    KGlobalSystemValueCache();

public:
    int m_nGameWorldStartTime;
    int m_nMaxPlayerLevel;

    DECLARE_LUA_CLASS(KGlobalSystemValueCache);
    DECLARE_LUA_INTEGER(GameWorldStartTime);
    DECLARE_LUA_INTEGER(MaxPlayerLevel);
};
#endif // _SERVER

#endif
