///////////////////////////////////////////////////////////////
// Copyright(c) Kingsoft
//
// FileName : KUpdateEntryParser.h
// Creator  : chengzhaohui
// Date     : 2007-10-18
// Comment  :
//
///////////////////////////////////////////////////////////////

#ifndef _KUPDATE_ENTRY_PARSER_H_
#define _KUPDATE_ENTRY_PARSER_H_

#include "UpdateDefine.h"
#include <map>
#include <vector>

using std::map;

typedef struct _KZIPFILEINFO 
{
	TCHAR szZipFileName[MAX_PATH];
	TCHAR szZipFileMD5[MD5_LENGTH];
}KZipFileInfo;

class KVersionParser
{
	typedef struct _KFileInfo
	{
		tstring strFileName;
		tstring strVersion;
		tstring strFileMd5;
	}KFileInfo, *PKFileInfo;

public:
    KVersionParser(void);
    ~KVersionParser(void);

    int Parse(const tstring &strEntryFile, const tstring &strCurrentVersion);
    
    int NeedUpdate(){return m_nNeedUpdateFlag;}
    int CanDownloadPack(){return m_nCanDownloadPackFlag;}
	int GetPackFileInfo(int nIndex, /*out*/KZipFileInfo &pKZipFileInfo);
	int GetPackFileListCount(){return m_nCountList;}
	KZipFileInfo** GetPackList(){return m_pZipFileList;}
    const tstring &GetLatestVersion(){return m_strLatestVersion;}
	const tstring &GetBeginVersion(){return m_strCurrentVersion;}
	void CleanUp();

	int CanUpdate(const TCHAR* szPackName, const TCHAR* szCurrentVer);  //检查手动更新包是否合法
	
private:
    int ParseLatestVersion(const tstring &strEntryFile, const tstring &strCurrentVersion);
    int ParseUpdatePack(const tstring &strEntryFile, const tstring &strCurrentVersion);
    int ParsePackName(const tstring &strPackName, tstring &strFilePrevVer, tstring &strFileLastVer);
	int SaveUpdatePath(const tstring &strPackName, const tstring &strPrevVersion, 
		               const tstring &strLastVersion, const tstring &strCurrentMD5);
	int AnalysisPath(const tstring &strBeginVersion);
	int SearchClosePath(const tstring &strVerion, /*out*/tstring &strRetVersion);

private:
    tstring m_strLatestVersion;
	tstring m_strCurrentVersion;
	map<tstring, PKFileInfo> m_UpdatePath;
    int m_nNeedUpdateFlag;
    int m_nCanDownloadPackFlag;
	int m_nBestPath;
	KZipFileInfo* m_pZipFileList[MAX_PATH];
	int m_nCountList;
};

#endif //_KUPDATE_ENTRY_PARSER_H_