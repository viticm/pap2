#include "stdafx.h"
#include "KMergeSettings.h"

#define MERGE_SETTINGS_FILENAME "merge_settings.ini"
#define MS_SECTION_GLOBAL       "Global"
#define MS_SECTION_SRC_DB       "SrcDB"
#define MS_SECTION_DST_DB       "DstDB"

KMergeSettings::KMergeSettings()
{
    m_szLocale[0] = '\0';
    m_nSrcDBPort = 3306;
    m_nDstDBPort = 3306;
}

BOOL KMergeSettings::Load()
{
    BOOL        bResult     = false;
    int         nRetCode    = false;
    IIniFile*	piIniFile   = NULL;

    piIniFile = g_OpenIniFile(MERGE_SETTINGS_FILENAME);
    KGLOG_PROCESS_ERROR(piIniFile);

    nRetCode = piIniFile->GetString(MS_SECTION_GLOBAL, "Locale", "", m_szLocale, sizeof(m_szLocale));
    KGLOG_PROCESS_ERROR(nRetCode);

    // Source DataBase
    nRetCode = piIniFile->GetString(MS_SECTION_SRC_DB, "IP", "127.0.0.1", m_szSrcDBIP, sizeof(m_szSrcDBIP));
    KGLOG_PROCESS_ERROR(nRetCode);
    nRetCode = piIniFile->GetInteger(MS_SECTION_SRC_DB, "Port", 3306, &m_nSrcDBPort);
    KGLOG_PROCESS_ERROR(nRetCode);
    nRetCode = piIniFile->GetString(MS_SECTION_SRC_DB, "Account", "", m_szSrcDBAcc, sizeof(m_szSrcDBAcc));
    KGLOG_PROCESS_ERROR(nRetCode);
    nRetCode = piIniFile->GetString(MS_SECTION_SRC_DB, "Password", "", m_szSrcDBPsw, sizeof(m_szSrcDBPsw));
    KGLOG_PROCESS_ERROR(nRetCode);
    nRetCode = piIniFile->GetString(MS_SECTION_SRC_DB, "Database", "", m_szSrcDBName, sizeof(m_szSrcDBName));
    KGLOG_PROCESS_ERROR(nRetCode);
    nRetCode = piIniFile->GetString(MS_SECTION_SRC_DB, "ServerName", "", m_szSrcServerName, sizeof(m_szSrcServerName));
    KGLOG_PROCESS_ERROR(nRetCode);
    KGLOG_PROCESS_ERROR(m_szSrcServerName[0] != '\0');

    // Destination DataBase
    nRetCode = piIniFile->GetString(MS_SECTION_DST_DB, "IP", "127.0.0.1", m_szDstDBIP, sizeof(m_szDstDBIP));
    KGLOG_PROCESS_ERROR(nRetCode);
    nRetCode = piIniFile->GetInteger(MS_SECTION_DST_DB, "Port", 3306, &m_nDstDBPort);
    KGLOG_PROCESS_ERROR(nRetCode);
    nRetCode = piIniFile->GetString(MS_SECTION_DST_DB, "Account", "", m_szDstDBAcc, sizeof(m_szDstDBAcc));
    KGLOG_PROCESS_ERROR(nRetCode);
    nRetCode = piIniFile->GetString(MS_SECTION_DST_DB, "Password", "", m_szDstDBPsw, sizeof(m_szDstDBPsw));
    KGLOG_PROCESS_ERROR(nRetCode);
    nRetCode = piIniFile->GetString(MS_SECTION_DST_DB, "Database", "", m_szDstDBName, sizeof(m_szDstDBName));
    KGLOG_PROCESS_ERROR(nRetCode);
    nRetCode = piIniFile->GetString(MS_SECTION_DST_DB, "ServerName", "", m_szDstServerName, sizeof(m_szDstServerName));
    KGLOG_PROCESS_ERROR(nRetCode);
    KGLOG_PROCESS_ERROR(m_szDstServerName[0] != '\0');

    KGLogPrintf(KGLOG_INFO, "Source DataBase %s: %s (%s)", m_szSrcServerName, m_szSrcDBName, m_szSrcDBIP);
    KGLogPrintf(KGLOG_INFO, "Destination DataBase %s: %s (%s)", m_szDstServerName, m_szDstDBName, m_szDstDBIP);

    bResult = true;
Exit0:
    KG_COM_RELEASE(piIniFile);
    return bResult;
}
