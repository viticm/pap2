#include "stdafx.h"
#include "KShop.h"

DEFINE_LUA_CLASS_BEGIN(KShop)
    REGISTER_LUA_DWORD(KShop,       TemplateID)
    REGISTER_LUA_STRING(KShop,      ShopName)
    REGISTER_LUA_DWORD(KShop,       ShopID)
    REGISTER_LUA_INTEGER(KShop,     ShopType)
    REGISTER_LUA_DWORD(KShop,       ValidPage)
    REGISTER_LUA_BOOL(KShop,        CanRepair)
    REGISTER_LUA_DWORD(KShop,       NpcID)
DEFINE_LUA_CLASS_END(KShop)
