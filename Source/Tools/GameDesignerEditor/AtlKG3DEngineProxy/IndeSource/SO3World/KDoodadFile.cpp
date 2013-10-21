//////////////////////////////////////////////////////////////////////////////////////
//
//  FileName    :   KDoodadFile.cpp
//  Version     :   1.0
//  Creater     :   Zhao Chunfeng
//  Date        :   2008-05-28 15:11:19
//  Comment     :   Dooodad file 
//
//////////////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "KDoodadFile.h"
#include "Engine/KMemory.h"
#include "Global.h"

enum KDoodadDataVersion
{   
    eDoodadDataVersion1 = 1,
    eDoodadDataVersion2 = 2,
    eDoodadDataCurrentVersion = 3
};

#pragma	pack(1)
struct KDOODAD_FILE_HEADER 
{
    int nVersion;
    int nDoodadCount;
};

struct KDOODAD_DATA_V1
{
	char	szName[_NAME_LEN];
	DWORD	dwTemplateID;
	BYTE	byLevel;
	int		nX;
	int		nY;
	int		nZ;
	DWORD	dwScriptID;
	BYTE	byFaceDirection;	
	DWORD	dwDoodadReliveID;
    WORD    wObstacleGroup;
	BYTE	byReserved[26];
};
#pragma pack()

KDoodadFile::KDoodadFile()
{
    m_uRefCount         = 1;
}

HRESULT KDoodadFile::QueryInterface(REFIID riid, void** ppvObject)
{
    return E_FAIL;
}

ULONG   KDoodadFile::AddRef()
{
    return ++m_uRefCount;
}

ULONG   KDoodadFile::Release()
{
    assert(m_uRefCount > 0);

    m_uRefCount--;

    if (m_uRefCount == 0)
    {
        KMemory::Delete(this);
        return 0;
    }

    return m_uRefCount;
}

BOOL KDoodadFile::Push(const KDOODAD_DATA& crDoodadInfo)
{
    m_DataTable.insert(m_DataTable.begin(), crDoodadInfo);
    return true;
}

BOOL KDoodadFile::Save(const char cszName[])
{
    BOOL                    bResult     = false;
    size_t                  uDataLen    = 0;
    BYTE*                   pbyBuffer   = NULL;
    BYTE*                   pbyOffset   = NULL;
    KDOODAD_FILE_HEADER*    pHeader     = NULL;
    FILE*                   pFile       = NULL;
    size_t                  uWriteCount = 0;
    char                    szFileName[MAX_PATH];

    assert(cszName);

    uDataLen = sizeof(KDOODAD_FILE_HEADER) + sizeof(KDOODAD_DATA) * m_DataTable.size();

    pbyBuffer = (BYTE*)KMemory::Alloc(uDataLen);
    KGLOG_PROCESS_ERROR(pbyBuffer);
    pbyOffset = pbyBuffer;

    pHeader = (KDOODAD_FILE_HEADER*)pbyOffset;
    pbyOffset += sizeof(KDOODAD_FILE_HEADER);

    pHeader->nVersion       = eDoodadDataCurrentVersion;
    pHeader->nDoodadCount   = (int)m_DataTable.size();
    
    for (KDOODAD_DATA_TABLE::iterator it = m_DataTable.begin(); it != m_DataTable.end(); ++it)
    {
        *(KDOODAD_DATA*)pbyOffset = *it;
        pbyOffset += sizeof(KDOODAD_DATA);
    }

	snprintf(szFileName, sizeof(szFileName), "%s/%s/%s.doodad", MAP_DIR, cszName, cszName);
    szFileName[sizeof(szFileName) - 1] = '\0';

    pFile = fopen(szFileName, "wb");
    KGLOG_PROCESS_ERROR(pFile);

    uWriteCount = fwrite(pbyBuffer, uDataLen, 1, pFile);
    KGLOG_PROCESS_ERROR(uWriteCount == 1);

    KGLogPrintf(KGLOG_INFO, "Save logic file: %s, total doodad: %u\n", szFileName, m_DataTable.size());

    bResult = true;
Exit0:
    if (pFile)
    {
        fclose(pFile);
        pFile = NULL;
    }

    if (pbyBuffer)
    {
        KMemory::Free(pbyBuffer);
        pbyBuffer = NULL;
    }
    return bResult;
}

BOOL KDoodadFile::Load(const char cszName[])
{
    BOOL                    bResult     = false;
    IFile*                  piFile      = NULL;
    size_t                  uFileSize   = 0;
    size_t                  uReadSize   = 0;
    BYTE*                   pbyBuffer   = NULL;
    BYTE*                   pbyOffset   = NULL;
    KDOODAD_FILE_HEADER*    pHeader     = NULL;
    char                    szFileName[MAX_PATH];

    assert(cszName);

	snprintf(szFileName, sizeof(szFileName), "%s/%s/%s.doodad", MAP_DIR, cszName, cszName);
    szFileName[sizeof(szFileName) - 1] = '\0';

	piFile = g_OpenFile(szFileName);
	KGLOG_PROCESS_ERROR(piFile);

    uFileSize = piFile->Size();
    KGLOG_PROCESS_ERROR(uFileSize > 0);

    pbyBuffer = (BYTE*)KMemory::Alloc(uFileSize);
    KGLOG_PROCESS_ERROR(pbyBuffer);

	uReadSize = piFile->Read(pbyBuffer, (unsigned long)uFileSize);
	KGLOG_PROCESS_ERROR(uReadSize == uFileSize);

    pbyOffset = pbyBuffer;

    pHeader = (KDOODAD_FILE_HEADER*)pbyOffset;
    pbyOffset += sizeof(KDOODAD_FILE_HEADER);

    KGLOG_PROCESS_ERROR(pHeader->nVersion == eDoodadDataVersion1 || 
        pHeader->nVersion == eDoodadDataVersion2 || 
        pHeader->nVersion == eDoodadDataCurrentVersion
    );
    m_DataTable.clear();

    if (pHeader->nVersion == eDoodadDataVersion1)
    {
        KDOODAD_DATA DoodadData;

        KGLOG_PROCESS_ERROR(uFileSize == sizeof(KDOODAD_FILE_HEADER) + sizeof(KDOODAD_DATA_V1) * pHeader->nDoodadCount);        
        memset(&DoodadData, 0, sizeof(KDOODAD_DATA));

        for (int i = 0; i < pHeader->nDoodadCount; i++)
        {
            KDOODAD_DATA_V1* pDoodadDataV1 = (KDOODAD_DATA_V1*)pbyOffset;
            pDoodadDataV1->szName[sizeof(pDoodadDataV1->szName) - 1] = '\0';
            
            DoodadData.szNickName[0] = '\0';	// Version1没有NickName

            DoodadData.dwTemplateID     = pDoodadDataV1->dwTemplateID;
            DoodadData.byLevel          = pDoodadDataV1->byLevel;
            DoodadData.nX               = pDoodadDataV1->nX;
            DoodadData.nY               = pDoodadDataV1->nY;
            DoodadData.nZ               = pDoodadDataV1->nZ;
            DoodadData.dwScriptID       = pDoodadDataV1->dwScriptID;
            DoodadData.byFaceDirection  = pDoodadDataV1->byFaceDirection;
            DoodadData.dwDoodadReliveID = pDoodadDataV1->dwDoodadReliveID;
            DoodadData.wObstacleGroup   = pDoodadDataV1->wObstacleGroup;
            DoodadData.nProgressID      = INVALID_SCENE_PROGRESS_ID;

            m_DataTable.push_back(DoodadData);

            pbyOffset += sizeof(KDOODAD_DATA_V1);
        }
        
        goto Exit1;
    }
    
    KGLOG_PROCESS_ERROR(uFileSize == sizeof(KDOODAD_FILE_HEADER) + sizeof(KDOODAD_DATA) * pHeader->nDoodadCount);

    for (int i = 0; i < pHeader->nDoodadCount; i++)
    {
        KDOODAD_DATA* pDoodadData = (KDOODAD_DATA*)pbyOffset;
        pDoodadData->szNickName[sizeof(pDoodadData->szNickName) - 1] = '\0';

        if (pHeader->nVersion == eDoodadDataVersion2)   // Version2的Name并非NickName
        {
            pDoodadData->szNickName[0] = '\0';
        }

        m_DataTable.push_back(*pDoodadData);

        pbyOffset += sizeof(KDOODAD_DATA);
    }

Exit1:
    bResult = true;
Exit0:
    KG_COM_RELEASE(piFile);

    if (pbyBuffer)
    {
        KMemory::Free(pbyBuffer);
        pbyBuffer = NULL;
    }
    return bResult;
}

BOOL KDoodadFile::Pop(KDOODAD_DATA* pDoodadInfo)
{
    BOOL    bResult = false;
    KDOODAD_DATA_TABLE::iterator it = m_DataTable.begin();

    assert(pDoodadInfo);

    if (it != m_DataTable.end())
    {
        *pDoodadInfo = *it;
        m_DataTable.pop_front();
        bResult = true;
    }

    return bResult;
}

IDoodadFile* CreateDoodadFileInterface()
{
    return KMemory::New<KDoodadFile>();
}
