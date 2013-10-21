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

struct LANGANDCODEPAGE
{
    WORD wLanguage;
    WORD wCodePage;
};

class KFileVersionInfo
{
public:    
    KFileVersionInfo(const TCHAR szFilePath[]);
    virtual ~KFileVersionInfo(void);

    TCHAR *ProductVersion();
    TCHAR *FileVersion();
    TCHAR *OriginalFilename();

private:    
    void *m_pvVersionInfoData;
    TCHAR m_szLanguageCode[16];
};

#endif //_KFILEVERSIONINFO_H_