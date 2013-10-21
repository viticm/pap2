////////////////////////////////////////////////////////////////////////////////
//  Copyright(c) Kingsoft
//
//  FileName    : KGU_SelfUpdater.h
//  Version     : 1.0
//  Creator     : Hu Hao
//  Create Date : 2009-6-11 10:15:54
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _KGU_SELF_UPDATER_H_
#define _KGU_SELF_UPDATER_H_

#include <list>

class KGU_SelfUpdater
{
public:
    KGU_SelfUpdater(void);
    ~KGU_SelfUpdater(void);

    int Init(
        const TCHAR cszApplicationName[],
        const TCHAR cszApplicationPath[],
        const TCHAR cszTempFilePath[],
        void *pvContext
    );
    int UnInit(void *pvContext);

    int CheckNeedUpdate(int *pnRetNeedUpdateFlag);

    int DownloadFiles();

    int UpdateFiles();
private:
    int _LoadDownloadURL(tstring *pstrRetURL);
    int _LoadVersionType(tstring *pstrRetVersionType);

    int _GetLastVersionNumber(tstring *pstrRetVersionNumber);
    int _GetSelfVersionNumber(tstring *pstrRetVersionNumber);

    int _GetUpdateFileList();
    int _DownloadFileList();

    // @brief : MoveFileEx() will fail if other process, such as virus scanner, is handling the 
    //          source file, The error code is ERROR_SHARING_VIOLATION, so we try MovFileEx() for 
    //          more than one time.
    int _MoveFile(const TCHAR cszSourceFileName[], const TCHAR cszTargetFileName[]);
private:
    typedef std::list<tstring> UPDATE_FILE_LIST;
private:
    tstring             m_strApplicationName;
    tstring             m_strApplicationPath;
    tstring             m_strTempFilePath;
    tstring             m_strSelfUpdateURLPrefix;
    tstring             m_strUpdateInfoFileName;
    UPDATE_FILE_LIST    m_UpdateFileList;
};

#endif  // _KGU_SELF_UPDATER_H_