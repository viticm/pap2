#include "stdafx.h"
#include "KGlobalSystemValueCache.h"

#ifdef _SERVER

KGlobalSystemValueCache::KGlobalSystemValueCache()
{
    m_nGameWorldStartTime   = 0;
    m_nMaxPlayerLevel       = 0;
}

DEFINE_LUA_CLASS_BEGIN(KGlobalSystemValueCache)

    REGISTER_LUA_INTEGER_READONLY(KGlobalSystemValueCache, GameWorldStartTime)
    REGISTER_LUA_INTEGER_READONLY(KGlobalSystemValueCache, MaxPlayerLevel)
    REGISTER_LUA_INTEGER_READONLY(KGlobalSystemValueCache, GameWorldChargeTime)

DEFINE_LUA_CLASS_END(KGlobalSystemValueCache)

#endif // _SERVER
