#ifndef _KSNDAAGENCY_H_
#define _KSNDAAGENCY_H_

#include "GatewayDef.h"

#ifdef WIN32
    #include "../../../../DevEnv/SDOA_SDK/Server/win32/bin/SDOA4Server1.1_Win32_Vc7.1_X86/sdoa4server.h"
    #ifdef _DEBUG
        #define SDOA_DLL_FILE "SDOA4ServerD.dll"
    #else
        #define SDOA_DLL_FILE "SDOA4Server.dll"
    #endif
    typedef HMODULE KG_HMODULE;
#else
    #include "../../../../DevEnv/SDOA_SDK/Server/linux/bin/SDOA4Server1.1_Linux2.6_Gcc4.1_X86/sdoa4server.h"
    #define SDOA_DLL_FILE "./libSDOA4Server.so"
    typedef void* KG_HMODULE;
#endif
#define SDOA_CONFIG_FILE "sdoa4server.ini"

struct KTOKEN_VERIFY_RESPOND
{
    int     nCode;
    int     nPlayerIndex;
    char    szSndaID[SNDA_ID_LEN];
    char    szIDCard[ID_CARD_LEN];
};

class KPlayerAgency;
class KPlayerManager;
class KPaysysAgency;
class KSO3Gateway;

class KSndaAgency
{
public:
    KSndaAgency();
    ~KSndaAgency();

    BOOL Init();
    void UnInit();

    void Activate();

    void AttachModule(KPlayerManager* pModule) { m_pPlayerManager = pModule; }
    void AttachModule(KPaysysAgency* pModule) { m_pPaysysAgency = pModule; }
    void AttachModule(KSO3Gateway* pModule) { m_pGateway = pModule; }

    BOOL DoTokenVerifyRequest(KPlayerAgency* pPlayer);

    void OnTokenVerifyRespond(const KTOKEN_VERIFY_RESPOND* pRespond);

private:
    KG_HMODULE              m_hSDOALibrary;
    ISDOAUserInfoAuthen*    m_piUserInfoAuthen;

    KPlayerManager*         m_pPlayerManager;
    KPaysysAgency*          m_pPaysysAgency;
    KSO3Gateway*            m_pGateway;
};

#endif
