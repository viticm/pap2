#include "stdafx.h"
#include "./KJX3ClientEvent.h"
#include "./SO3Client.h"
#include "SO3UI/ISO3UI.h"
#include "SO3UI/IKGUI.h"
#include "SDOA4Client.h"

static LPCSTR GetJX3SdoaErrorCode(int nErrorCode)
{
    switch (nErrorCode)
    {
    case SDOA_ERRORCODE_OK:
        return "OK";
    case SDOA_ERRORCODE_CANCEL:
        return "CANCEL";
    case SDOA_ERRORCODE_UILOST:
        return "UILOST";
    case SDOA_ERRORCODE_FAILED:
        return "FAILED";
    case SDOA_ERRORCODE_UNKNOWN:
        return "UNKNOWN";
    case SDOA_ERRORCODE_INVALIDPARAM:
        return "INVALIDPARAM";
    case SDOA_ERRORCODE_UNEXCEPT:
        return "UNEXCEPT";
    case SDOA_ERRORCODE_ALREAYEXISTS:
        return "ALREAYEXISTS";
    case SDOA_ERRORCODE_SHOWMESSAGE:
        return "SHOWMESSAGE";
    default:
        return "";
    }
}

int FireJX3SdoaEvent(int nErrorCode, LoginResult const* pLoginResult, int nUserData, int nReserved)
{
    int nRetCode = false;
    int nResult = false;
    IKGUI* pUI = NULL;
    LPCSTR szErrorCode = "";
    LPCSTR szSessionId = "";
    LPCSTR szSndaId = "";
    LPCSTR szIdentityState = "";
    LPCSTR szAppendix = "";

    KGLOG_PROCESS_ERROR(g_SO3Client.m_pUI);

    pUI = g_SO3Client.m_pUI->GetIKGUI();
    KGLOG_PROCESS_ERROR(pUI);

    szErrorCode = GetJX3SdoaErrorCode(nErrorCode);
    KGLOG_PROCESS_ERROR(szErrorCode);

    if (pLoginResult)
    {
        szSessionId = pLoginResult->szSessionId;
        szSndaId = pLoginResult->szSndaid;
        szIdentityState = pLoginResult->szIdentityState;
        szAppendix = pLoginResult->szAppendix;
    }

    pUI->PushArg("arg0", szErrorCode);
    pUI->PushArg("arg1", szSessionId);
    pUI->PushArg("arg2", szSndaId);
    pUI->PushArg("arg3", szIdentityState);
    pUI->PushArg("arg4", szAppendix);
    pUI->FireEvent("SDOA_LOGIN", 0);

    nResult = true;
Exit0:
    if (nErrorCode != SDOA_ERRORCODE_OK && nErrorCode != SDOA_ERRORCODE_CANCEL && nErrorCode != SDOA_ERRORCODE_SHOWMESSAGE)
    {
        KGLogPrintf(KGLOG_ERR, "[Sdoa] %s %s %s %s\n", szErrorCode, szSessionId, szSndaId, szIdentityState, szAppendix);
    }
    return nResult;
}

int FireJX3SdoaInitFinishedEvent()
{
    int nRetCode = false;
    int nResult = false;
    IKGUI* pUI = NULL;

    KGLOG_PROCESS_ERROR(g_SO3Client.m_pUI);

    pUI = g_SO3Client.m_pUI->GetIKGUI();
    KGLOG_PROCESS_ERROR(pUI);

    pUI->PushArg("arg0", "FINISHED");
    pUI->FireEvent("SDOA_INIT", 0);

    nResult = true;
Exit0:
    return nResult;
}

