#pragma once
#include "UiWndData.h"

class IIniFile;


class KUiClientManage
{
public:
    KUiClientManage();
    ~KUiClientManage();

    void    UpDataWndInClient(IIniFile *pIni);
    void    Clear();
    int     SafeCheck(IIniFile *pIni);
    int     SafeCheckUnitKey(
        IIniFile *pIni, LPCTSTR pszUnit, 
        LPCTSTR pszKey, LPCTSTR pszKeyValue
    );
    int     GetPositonFromClient(
        IN LPCSTR pszSectionName, 
        OUT int *pnX, OUT int *pnY
    );
    int     GetSizeFromClient(
        IN LPCSTR pszSectionName,
        OUT int *pnWidth, OUT int *pnHeight
    );

    int     GetAllItemSizeAndPos();

    void BindScript(int nBindFlag, IIniFile *pIni);
    void EnableUpDataData(int nFlag);

    int IsFocusWndChangedInClient(OUT char *pszChangedName);
    int SetClientChooseWnd(int nFlag);

	int ReloadImageFromFile();

protected:
private:
    char m_szFrameName[128];
    int  m_nBindScript;
    int  m_nUpDataData;
    int  m_nClientSelWnd;
    char m_szFocusWndName[128];
};


extern KUiClientManage g_ClientManager;