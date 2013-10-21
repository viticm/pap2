///////////////////////////////////////////////////////////////
// Copyright(c) Kingsoft
//
// FileName : KFileVersionInfo.H
// Creator  : chengzhaohui
// Date     : 
// Comment  :
//
///////////////////////////////////////////////////////////////

#ifndef _KFILEVERSIONINFO_H_
#define _KFILEVERSIONINFO_H_

#pragma comment(lib, "Version.lib")
#pragma comment(lib, "strsafe.lib")

struct LANGANDCODEPAGE
{
    WORD wLanguage;
    WORD wCodePage;
};

class KFileVersionInfo
{
public:
	KFileVersionInfo() {}
    KFileVersionInfo(const TCHAR szFilePath[]);
    virtual ~KFileVersionInfo(void);

	int    GetFileVerInfo(const TCHAR szFilePath[]);
    TCHAR *ProductVersion();
    TCHAR *FileVersion();
    TCHAR *OriginalFilename();

private:    
    void *m_pvVersionInfoData;
    TCHAR m_szLanguageCode[16];
};

#endif //_KFILEVERSIONINFO_H_