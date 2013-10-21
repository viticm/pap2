//////////////////////////////////////////////////////////////////////////////////////
// Copyright(c) Kingsoft
//
//  FileName    :   KFile.cpp    
//  Version     :   1.0
//  Creater     :   dengwenfeng
//  Date        :   2007-09-21 
//  Comment     :   
//
//////////////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "KFile.h"

KFile::KFile()
{
    m_pFile = NULL;
}

KFile::~KFile()
{
    
}

int KFile::OpenFile(const char* pcszFilePath)
{
    int nResult = FALSE;
    
	KG_PROCESS_ERROR(pcszFilePath);

    m_pFile = fopen(pcszFilePath, "w");
    KG_PROCESS_ERROR(m_pFile);

    nResult = TRUE;
Exit0:
    if (!nResult)
    {
        if (m_pFile)
        {
            fclose(m_pFile);
            m_pFile = NULL;
        }
    }   
    return nResult;
}

void KFile::CloseFile()
{
    if (m_pFile)
    {
        fclose(m_pFile);
        m_pFile = NULL;
    }
     
}

int KFile::SaveFile(const char *cpszInfo)
{
    int nResult     = FALSE;
    int nRetCode    = 0;

    KG_PROCESS_ERROR(m_pFile);
    KG_PROCESS_ERROR(cpszInfo);

    nRetCode = (int)fprintf(m_pFile, "%s\n", cpszInfo);
    KG_PROCESS_ERROR(nRetCode >= 0);

    fflush(m_pFile);

    nResult = TRUE;
Exit0:
    return nResult;
}