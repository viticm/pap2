//////////////////////////////////////////////////////////////////////////////////////
//
//  FileName    :   KNpcFile.cpp
//  Version     :   1.0
//  Creater     :   Zhao Chunfeng
//  Date        :   2008-05-28 15:11:19
//  Comment     :   NPC file 
//
//////////////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "KNpcFile.h"
#include "Engine/KMemory.h"
#include "Global.h"

enum KNpcDataVersion
{
    eNpcDataVersion2        = 2,
    eNpcDataCurrentVersion  = 3
};

#pragma	pack(1)
struct KNPC_FILE_HEADER 
{
    int nVersion;
    int nNpcCount;
};

struct KNPC_DATA_V2
{
    char    szNickName[_NAME_LEN];
    DWORD   dwTemplateID;
    int     nX;
    int     nY;
    int     nZ;
    BYTE    byFaceDirection;
    BYTE    byKind;						// Npc类型，见CHARACTER_KIND
    DWORD   dwScriptID;
    DWORD   dwReliveID;
    DWORD   dwRandomID;                 // 随机分组ID，如果是0，表示不随机
    DWORD   dwAIType;
    DWORD   dwThreatLinkID;             // 仇恨链接的分组ID
    int     nPatrolPathID;              // 寻路路线ID，同ID的NPC自动分为一组
    int     nOrderIndex;                // NPC在队形中的位置
    DWORD   dwRepresentID;
    int     nProgressID;
    BYTE    byReserved[10];
};
#pragma pack()

KNpcFile::KNpcFile()
{
    m_uRefCount         = 1;
}

HRESULT KNpcFile::QueryInterface(REFIID riid, void** ppvObject)
{
    return E_FAIL;
}

ULONG   KNpcFile::AddRef()
{
    return ++m_uRefCount;
}

ULONG   KNpcFile::Release()
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

BOOL KNpcFile::Push(const KNPC_DATA& crNpcInfo)
{
    m_DataTable.insert(m_DataTable.begin(), crNpcInfo);
    return true;
}

BOOL KNpcFile::Save(const char cszName[])
{
    BOOL                bResult     = false;
    size_t              uDataLen    = 0;
    BYTE*               pbyBuffer   = NULL;
    BYTE*               pbyOffset   = NULL;
    KNPC_FILE_HEADER*   pHeader     = NULL;
    FILE*               pFile       = NULL;
    size_t              uWriteCount = 0;
    char                szFileName[MAX_PATH];

    assert(cszName);

    uDataLen = sizeof(KNPC_FILE_HEADER) + sizeof(KNPC_DATA) * m_DataTable.size();

    pbyBuffer = (BYTE*)KMemory::Alloc(uDataLen);
    KGLOG_PROCESS_ERROR(pbyBuffer);
    pbyOffset = pbyBuffer;

    pHeader = (KNPC_FILE_HEADER*)pbyOffset;
    pbyOffset += sizeof(KNPC_FILE_HEADER);

    pHeader->nVersion   = eNpcDataCurrentVersion;
    pHeader->nNpcCount  = (int)m_DataTable.size();
    
    for (KNPC_DATA_TABLE::iterator it = m_DataTable.begin(); it != m_DataTable.end(); ++it)
    {
        *(KNPC_DATA*)pbyOffset = *it;
        pbyOffset += sizeof(KNPC_DATA);
    }

	snprintf(szFileName, sizeof(szFileName), "%s/%s/%s.npc", MAP_DIR, cszName, cszName);
    szFileName[sizeof(szFileName) - 1] = '\0';

    pFile = fopen(szFileName, "wb");
    KGLOG_PROCESS_ERROR(pFile);

    uWriteCount = fwrite(pbyBuffer, uDataLen, 1, pFile);
    KGLOG_PROCESS_ERROR(uWriteCount == 1);

    KGLogPrintf(KGLOG_INFO, "Save logic file: %s, total npc: %u\n", szFileName, m_DataTable.size());

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

BOOL KNpcFile::Load(const char cszName[])
{
    BOOL                bResult     = false;
    IFile*              piFile      = NULL;
    size_t              uFileSize   = 0;
    size_t              uReadSize   = 0;
    BYTE*               pbyBuffer   = NULL;
    BYTE*               pbyOffset   = NULL;
    KNPC_FILE_HEADER*   pHeader     = NULL;
    char                szFileName[MAX_PATH];

    assert(cszName);

	snprintf(szFileName, sizeof(szFileName), "%s/%s/%s.npc", MAP_DIR, cszName, cszName);
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

    pHeader = (KNPC_FILE_HEADER*)pbyOffset;
    pbyOffset += sizeof(KNPC_FILE_HEADER);

    KGLOG_PROCESS_ERROR(pHeader->nVersion == eNpcDataVersion2 || pHeader->nVersion == eNpcDataCurrentVersion);

    m_DataTable.clear();

    if (pHeader->nVersion == eNpcDataVersion2)
    {
        KNPC_DATA NewNpcData;
        
        KGLOG_PROCESS_ERROR(uFileSize == sizeof(KNPC_FILE_HEADER) + sizeof(KNPC_DATA_V2) * pHeader->nNpcCount);
        memset(&NewNpcData, 0, sizeof(KNPC_DATA));

        for (int i = 0; i < pHeader->nNpcCount; i++)
        {
            KNPC_DATA_V2*   pNpcData = (KNPC_DATA_V2*)pbyOffset;

            pNpcData->szNickName[sizeof(pNpcData->szNickName) - 1] = '\0';

            strncpy(NewNpcData.szNickName, pNpcData->szNickName, sizeof(NewNpcData.szNickName));
            NewNpcData.szNickName[sizeof(NewNpcData.szNickName) - 1] = '\0';

            NewNpcData.dwTemplateID     = pNpcData->dwTemplateID;
            NewNpcData.nX               = pNpcData->nX;
            NewNpcData.nY               = pNpcData->nY;
            NewNpcData.nZ               = pNpcData->nZ;
            NewNpcData.byFaceDirection  = pNpcData->byFaceDirection;
            NewNpcData.byKind           = pNpcData->byKind;
            NewNpcData.dwScriptID       = pNpcData->dwScriptID;
            NewNpcData.dwReliveID       = pNpcData->dwReliveID;
            NewNpcData.dwRandomID       = pNpcData->dwRandomID;
            NewNpcData.dwAIType         = pNpcData->dwAIType;
            NewNpcData.dwThreatLinkID   = pNpcData->dwThreatLinkID;
            NewNpcData.nPatrolPathID    = pNpcData->nPatrolPathID;
            NewNpcData.nOrderIndex      = pNpcData->nOrderIndex;
            NewNpcData.dwRepresentID    = pNpcData->dwRepresentID;
            NewNpcData.nProgressID      = INVALID_SCENE_PROGRESS_ID;

            m_DataTable.push_back(NewNpcData);

            pbyOffset += sizeof(KNPC_DATA_V2);
        }
        
        goto Exit1;
    }
    
    KGLOG_PROCESS_ERROR(uFileSize == sizeof(KNPC_FILE_HEADER) + sizeof(KNPC_DATA) * pHeader->nNpcCount);
    for (int i = 0; i < pHeader->nNpcCount; i++)
    {
        KNPC_DATA* pNpcData = (KNPC_DATA*)pbyOffset;

        pNpcData->szNickName[sizeof(pNpcData->szNickName) - 1] = '\0';

        m_DataTable.push_back(*pNpcData);

        pbyOffset += sizeof(KNPC_DATA);
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

BOOL KNpcFile::Pop(KNPC_DATA* pNpcInfo)
{
    BOOL    bResult = false;
    KNPC_DATA_TABLE::iterator it = m_DataTable.begin();

    assert(pNpcInfo);

    if (it != m_DataTable.end())
    {
        *pNpcInfo = *it;
        m_DataTable.pop_front();
        bResult = true;
    }

    return bResult;
}

INpcFile* CreateNpcFileInterface()
{
    return KMemory::New<KNpcFile>();
}
