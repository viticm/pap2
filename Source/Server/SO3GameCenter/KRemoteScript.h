#ifndef _KREMOTELUA_H_
#define _KREMOTELUA_H_ 

#include "lzo/lzoconf.h"

class KRemoteScript 
{
public:
    KRemoteScript();

    void Setup();

    BOOL Accept(int nConnIndex, const char cszFunction[], BYTE* pbyData, size_t uDataLen, BOOL bCompressed);

    int  Call(Lua_State* L);

    void    Reset();
    BYTE*   PushNumber(double fValue);
    BYTE*   PushString(const char cszValue[]);
    BYTE*   PushNill();
    BYTE*   PushBool(BOOL bValue);
    BYTE*   NewTable();
    BYTE*   SetTable(BYTE* pbyTableBegin, BYTE* pbyTableEnd);
    BOOL    Call(int nConnIndex, const char cszFunction[]);

private:
    DWORD       m_dwScriptID;
    BYTE        m_byOriginalBuffer[LZO_ORIGINAL_BUFFER_LEN];
    BYTE        m_byCompressBuffer[LZO_COMPRESS_BUFFER_LEN];
    lzo_align_t m_LzoWorkBuffer[((lzo_uint32)(16384L * lzo_sizeof_dict_t) + (sizeof(lzo_align_t) - 1)) / sizeof(lzo_align_t)];
    BYTE*       m_pbyPos;
};

#endif
