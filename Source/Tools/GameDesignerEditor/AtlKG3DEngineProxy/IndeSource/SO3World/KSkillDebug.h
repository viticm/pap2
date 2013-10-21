#ifndef _KSKILLDEBUG_H_
#define _KSKILLDEBUG_H_ 1

#include <map>
#include <list>

class KCharacter;
class KSkillDebug;

struct IKG_DebugMsgOutput
{
    virtual ~IKG_DebugMsgOutput(){};
    virtual BOOL PushMsg(char* pszMessage){return true;};
};

class KG_DebugMsgPlayerOutput : public IKG_DebugMsgOutput
{
public:
    KG_DebugMsgPlayerOutput();
    virtual ~KG_DebugMsgPlayerOutput();

    virtual BOOL PushMsg(char* pszMessage);
    BOOL SetOwner(KCharacter* pCharacter);
private:
    KCharacter*     m_pOwner;
};

class KSkillDebug
{
public:
    KSkillDebug();
    ~KSkillDebug();

    BOOL Init(KCharacter* pOwner);
    BOOL UnInit();

    BOOL Attach(DWORD dwCharacter, int nDebugLevel);
    BOOL Detach(DWORD dwCharacter);
    BOOL DetachAll();

    BOOL OnAttach(DWORD dwChatacter);
    BOOL OnDetach(DWORD dwCharacter);

    BOOL SetOwner(KCharacter* pOwner);

    BOOL Trace(int nDebugLevel, const char* pszFormat, ...);

private:
    KCharacter*         m_pOwner;
    int                 m_nDebugLevel;
    std::vector<DWORD>  m_OutprotVector;
    std::vector<DWORD>  m_OnAttachVector;
};

#endif // _KSKILLDEBUG_H_
