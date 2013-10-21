#ifndef _KCENTERREMOTE_H_ 
#define _KCENTERREMOTE_H_ 

#ifdef _SERVER

class KCenterRemote
{
public:
    KCenterRemote();

    void Setup();

    BOOL Accept(const char cszFunction[], BYTE* pbyData, size_t uDataLen);

    int  Call(Lua_State* L);

private:
    DWORD   m_dwScript;
    BYTE    m_byParamData[1024];
};

#endif

#endif
