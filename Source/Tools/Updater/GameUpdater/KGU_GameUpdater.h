////////////////////////////////////////////////////////////////////////////////
//  Copyright(c) Kingsoft
//
//  FileName    : KGU_GameUpdater.h
//  Version     : 1.0
//  Creator     : Hu Hao
//  Create Date : 2009-7-7 17:43:13
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _KGU_GAME_UPADTER_H_
#define _KGU_GAME_UPADTER_H_

class KGU_GameUpdater
{
public:
    KGU_GameUpdater();
    ~KGU_GameUpdater();

    int Init(const TCHAR cszCommandLine[], void *pvContext);
    int UnInit(void *pvContext);

    int Run();
private:
    int _AnalyzeCommandLine(const TCHAR cszCommandLine[]);

    int _SelfUpdate();
    int _PatchUpdate();

    HANDLE _CreateAutoUpdateWithWaitProcess(HANDLE hSuccessEvent, HANDLE hFailedEvent);
    HANDLE _CreateAutoUpdateWithoutWaitProcess();

    int _MakeSureFlashInstalled();

    //gameupdater.exe is a zip file, the update.swf should be extracted firstly.
    int _ExtractSwfFile(const TCHAR cszSwfFileName[]);
    int _RemoveSwfFile(const TCHAR cszSwfFileName[]);
private:
    enum GAME_UPDATE_TYPE
    {
        TYPE_SELF_UPDATE                = 0,
        TYPE_AUTO_UPDATE_WITH_WAIT      = 1,
        TYPE_AUTO_UPDATE_WITHOUT_WAIT   = 2,
        TYPE_MANUAL_UPDATE              = 3,
    };

    struct COMMAND_LINE_PARAMETER
    {
        GAME_UPDATE_TYPE eType;
        // Auto update should wait until self update finish, but If self update failed, auto update
        // should show the error information and not downloading patches.
        // We use two event to differ self update success or not. If both NULL, means auto update 
        // not need wait for self update, or both should be valid handle value.
        HANDLE  hSuccessSelfUpdateEvent;
        HANDLE  hFailedSelfUpdateEvent;
        TCHAR   szManualPackageName[MAX_PATH];
    };
private:
    COMMAND_LINE_PARAMETER  m_Parameter;
    tstring                 m_strApplicationPath;
    tstring                 m_strApplicationName;
    tstring                 m_strTempFilePath;
};

#endif // _KGU_GAME_UPADTER_H_