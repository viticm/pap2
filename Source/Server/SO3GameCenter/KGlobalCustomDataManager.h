#ifndef _KGLOBAL_CUSTOM_DATA_MANAGER_H_
#define _KGLOBAL_CUSTOM_DATA_MANAGER_H_

#include "Engine/KMemory.h"

class KGlobalCustomDataManager
{
public:
    KGlobalCustomDataManager();

    BOOL Init();
    void UnInit();

    void Activate();
    BOOL Add(const char cszName[], BYTE* byData, size_t uDataLen, BOOL bSaved);

    void SaveAll();

    int LuaSet(Lua_State* L);
    int LuaGet(Lua_State* L);

private:
    struct KCUSTOM_DATA
    {
        BOOL    bSaved;
        size_t  uDataLen;
        BYTE    byData[0];
    };

    typedef KMemory::KAllocator<std::pair<kstring, KCUSTOM_DATA*> > KCUSTOM_DATA_ALLOCATOR;
    typedef std::map<kstring, KCUSTOM_DATA*, std::less<kstring>, KCUSTOM_DATA_ALLOCATOR> KCUSTOM_DATA_TABLE;

    KCUSTOM_DATA_TABLE  m_DataTable;
    time_t              m_nNextSaveTime;
    BYTE                m_byOriginalBuffer[LZO_ORIGINAL_BUFFER_LEN];

public:
    DECLARE_LUA_CLASS(KGlobalCustomDataManager);
};

#endif
