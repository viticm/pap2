#ifndef _KMERGE_SETTINGS_H_
#define _KMERGE_SETTINGS_H_

#define _NAME_LEN 32

struct KMergeSettings
{
	KMergeSettings();

    BOOL Load();

    char    m_szLocale[64];

	char    m_szSrcDBIP[_NAME_LEN];
	int     m_nSrcDBPort;
	char    m_szSrcDBAcc[_NAME_LEN];
	char    m_szSrcDBPsw[_NAME_LEN];
	char    m_szSrcDBName[_NAME_LEN];
    char    m_szSrcServerName[_NAME_LEN];

	char    m_szDstDBIP[_NAME_LEN];
	int     m_nDstDBPort;
	char    m_szDstDBAcc[_NAME_LEN];
	char    m_szDstDBPsw[_NAME_LEN];
	char    m_szDstDBName[_NAME_LEN];
    char    m_szDstServerName[_NAME_LEN];
};

#endif // _KMERGE_SETTINGS_H_
