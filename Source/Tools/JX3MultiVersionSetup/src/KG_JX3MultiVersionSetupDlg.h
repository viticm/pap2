////////////////////////////////////////////////////////////////////////////////
//  Copyright(c) Kingsoft
//
//  FileName    : KG_JX3MultiVersionSetupDlg.h
//  Version     : 1.0
//  Creator     : Hu Hao
//  Create Date : 2009-10-22  16:56:36
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <string>
#include <list>
#include <map>
#include "KThread.h"

////////////////////////////////////////////////////////////////////////////////////////////////////
#if defined(UNICODE) || defined(_UNICODE)
typedef std::wstring tstring;
#else
typedef std::string tstring;
#endif

////////////////////////////////////////////////////////////////////////////////////////////////////
class KG_JX3MultiVersionSetupDlg : public CDialog
{
public:
	KG_JX3MultiVersionSetupDlg(CWnd* pParent = NULL);

	enum { IDD = IDD_JX3MultiVersionSETUP_DIALOG };

private:
    virtual void DoDataExchange(CDataExchange* pDX);

    virtual BOOL    OnInitDialog();
    afx_msg void    OnPaint();
    afx_msg void    OnCancel();
    afx_msg void    OnDestroy();
    afx_msg HCURSOR OnQueryDragIcon();
    afx_msg void    OnBnClickedButtonStart();
    DECLARE_MESSAGE_MAP()

    static void _WorkThreadFunction(void *pvParam);
    int _ThreadFunction();

    int _SetProcessPos(int nPos);

    int _CheckBasedClientVersion();
    int _ReadBasedVersionPathFromReg();
    int _AssumeAppPathAsBaseVersionPath();
    int _IsNecessaryFilesExist();

    int _CheckBasedVersionClientNotRunning();

    int _CreateNewVersionDirectory();

    int _CopyAllFiles();
    int _GetClientFilesInfo();
    int _CheckFileShouldBeCopied(const WIN32_FIND_DATA &crFindData, int *pnRetCopyFlag);
    int _CheckIsSelfExeFile(const TCHAR cszFileName[], int *pnRetIsSelfFlag);
    int _IsIgnoreCopyFile(const TCHAR cszFileName[]);
    int _GetPakFilePath();
    int _GetBasedUpdatePakPrefix();
    int _GetUpdatePakFilesInfo();
    int _GetFileSize(const TCHAR cszFileName[], LONGLONG *pllRetSize);
    int _RemoveExistNewVersionFiles();
    int _RemoveExistNewUpdatePakFiles();
    int _CopyClientFiles();
    int _CopyUpdatePakFiles();

    int _EditNewVersionConfigureFiles();
    int _EditNewPackageIniFile();
    int _EditNewGameUpdaterIniFile();
    int _EditNewVersionCfgFile();
    int _EditNewConfigIniFile();

    int _UnzipJX3Launcher();
    int _EditJX3LauncherIniFile();
    int _MakeSureSection0AsBasedVersion(const TCHAR cszIniFileName[]);
    int _AddNewVersionSectionIfNotExist(const TCHAR cszIniFileName[]);

    int _EditJX3Shortcut();
    int _EditShortcutInDir(const TCHAR cszDir[]);
    int _EditShortcutIfLinkedToJX3(const TCHAR cszLinkFileFullName[]);

    int _IsIncludeJX3Shortcut(const TCHAR cszDir[]);
    int _IsJX3DesktopShortcut(const TCHAR cszFileName[]);
    int _CreateJX3DesktopShortcut(const TCHAR cszDir[]);

    int _GetShortcutLinkedFileName(
        const TCHAR cszShortcutFileName[], tstring *pstrRetLinkedFileName, tstring *pstrRetArgument
    );
    int _SetShortcutLinkedFileName(
        const TCHAR cszShortcutFileName[], const TCHAR cszLinkedFileName[], const TCHAR cszArgument[]
    );

    int _WriteRegForNewVersion();

    int _LoadJX3Launcher();
private:
    typedef std::list<tstring>          _FILE_NAME_LIST;
    typedef std::map<tstring, tstring>  _UPDATE_PAK_MAP;    // map the update pak name before copy with after copy.
private:
    HICON               m_hIcon;
    CProgressCtrl       m_ProcessCtrl;
    CFont               m_ContextTitleFont;

    int                 m_nRunningFlag;
    int                 m_nFinishFlag;
    KThread             m_WorkThread;
    tstring             m_strErrorInfo;
    tstring             m_strBasedVersionPath;
    tstring             m_strNewVersionType;
    tstring             m_strNewVersionPath;
    tstring             m_strPakFilePath;
    tstring             m_strBasedUpdatePakPrefix;

    _FILE_NAME_LIST     m_ClientFileList;
    _UPDATE_PAK_MAP     m_UpdatePakMap;
    LONGLONG            m_llCopyFileTotalSize;
    LONGLONG            m_llUpdatePakTotalSize;
};
