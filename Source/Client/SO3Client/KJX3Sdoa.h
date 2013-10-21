#ifndef KJX3SDOA_H
#define KJX3SDOA_H

#include "SDOA4Client.h"
#include "SDOADx9.h"

struct KJX3Sdoa
{
    LPigwInitialize igwInitialize;
    LPigwGetModule igwGetModule;
    LPigwTerminal igwTerminal;

    ISDOADx9* pSDOADx9;
    ISDOAApp* pSDOAApp;

    ISDOAAppUtils* pSDOAAppUtils;

    LoginResult Result;
    BOOL bLogin;
};

int InitJX3Sdoa(KJX3Sdoa* pJX3Sdoa, HMODULE hmodule, LPCWSTR pcwszVersion);
int UnInitJX3Sdoa(KJX3Sdoa* pJX3Sdoa);
int HandleJX3SdoaMsgProc(KJX3Sdoa* pJX3Sdoa, HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT* plResult);
int JX3SdoaLogin(KJX3Sdoa* pJX3Sdoa);
int JX3SdoaLogout(KJX3Sdoa* pJX3Sdoa);
int SetJX3SodaRoleInfo(KJX3Sdoa* pJX3Sdoa, RoleInfo const* pRoleInfo);

int GetJX3SdoaWinProperty(KJX3Sdoa* pJX3Sdoa, LPCWSTR pwcsWinName, SDOAWinProperty* pProperty);
int SetJX3SdoaWinProperty(KJX3Sdoa* pJX3Sdoa, LPCWSTR pwcsWinName, SDOAWinProperty* pProperty);

int JX3SdoaWinExists(KJX3Sdoa* pJX3Sdoa, LPCWSTR pwcsWinName, PBOOL pbExists);
int SetJX3SdoaScreenStatus(KJX3Sdoa* pJX3Sdoa, int nStatus);

#endif // KJX3SDOA_H
