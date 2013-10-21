////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KSaveManager.h
//  Version     : 1.0
//  Creator     : Hu Changyin
//  Create Date : 2005-12-23 10:08:07
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _INCLUDE_KSAVEMANAGER_H_
#define _INCLUDE_KSAVEMANAGER_H_

////////////////////////////////////////////////////////////////////////////////

struct UITEXFILEHEADER;

class KSaveManager
{
protected:
    KSaveManager();
    ~KSaveManager();
public:
    static int Init();
    static int Exit();
    static KSaveManager &GetSelf();

    int SetCurrentEditFile(LPCTSTR pcszUITex, LPCTSTR pcszTga = NULL, LPCTSTR pcszSTex = NULL, LPCTSTR pcszTxt = NULL);
    int GetCurrentEditFile(LPTSTR pszName, int nNameBufferSize);

    int Clear();

    int Save();

    int ExportText(LPCTSTR pcszName);
    int IsSaveFileNameEmpty();

    int SetLastOpenOrSaveDir(LPCTSTR pcszPath);
    int SetLastCurrentPath(LPCTSTR pcszCurrent);
    int SetLastUITexOpenOrSaveDir(LPCTSTR pcszUITexPath);
    LPCTSTR GetLastOpenOrSaveDir();
    LPCTSTR GetLastCurrentPath();
    LPCTSTR GetLastUITexOpenOrSaveDir();
    int InitLastOpenOrSaveDir();
    int IsLastOpenDirExist();
    int IsLastUITexOpenDirExist();
    int IsLastCurrentPath();

    int SetSaveHideCode();
    int ClearSaveHideCode();

    int ExportText1(LPCTSTR pcszName);
    int GetCorrectWidthAndHeight(IN OUT int &nWidth, IN  OUT int &nHeight);

protected:
    int AutoSetTgaFileAndSTexFile();
    int GetTgaPureFileName(LPTSTR pszName, int nNameBufferSize);
    int GetSTexPureFileName(LPTSTR pszName, int nNameBufferSize);
    int FormateUITexHeader(UITEXFILEHEADER &UiTexHeader);
private:
    static KSaveManager *ms_pSelf;
	TCHAR m_szTxtFile[MAX_PATH];
    TCHAR m_szUITexFile[MAX_PATH];
    TCHAR m_szTgaFile[MAX_PATH];
    TCHAR m_szSTexFile[MAX_PATH];
    TCHAR m_szLastOpenOrSaveDir[MAX_PATH];
    TCHAR m_szLastUITexOpenOrSaveDir[MAX_PATH];
    TCHAR m_szLastCurrentPath[MAX_PATH];

    int m_nSaveHideCode;
};



#endif //_INCLUDE_KSAVEMANAGER_H_
