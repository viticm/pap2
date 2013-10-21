#include "stdafx.h"
#include "KG_Clip.h"

const DWORD CLIP_FILE_MASK = 0x414E494D;
const DWORD CLIP_FILE_END_FLAG = 0xFFFFFFFF;

////////////////////////////////////////////////////////////////////////////////
KG_Clip::KG_Clip()
{
    m_dwBoneNum = 0;
    m_pAniFileData = NULL;
    m_pAniFileHeader = NULL;
    memset(m_pszBoneName, 0, sizeof(char *) * MAX_BONE_NUM);
    memset(m_pAnimatedBoneRTS, 0, sizeof(RTS *) * MAX_BONE_NUM);
}

KG_Clip::~KG_Clip()
{
    _Clear();
}

int KG_Clip::LoadFromFile(const char cszFileName[])
{
    int nResult = false;
    unsigned uFileSize = 0;
    unsigned uSizeRead = 0;
    IFile* piFile = NULL;
    //DWORD dwFileEndFlag = 0;
    unsigned uBufferSize = 0;
    DWORD dwNumFrames = 0;
    char *pszBoneNameBegin = NULL;
    RTS *pBoneRTSBegin = NULL;

    KGLOG_PROCESS_ERROR(cszFileName);

    piFile = g_OpenFile(cszFileName);
    KGLOG_PROCESS_ERROR(piFile);

    uFileSize = piFile->Size();
    KGLOG_PROCESS_ERROR(uFileSize > 0);

    m_pAniFileData = new char[uFileSize];
    KGLOG_PROCESS_ERROR(m_pAniFileData);

    uSizeRead = piFile->Read(m_pAniFileData, uFileSize);
    KGLOG_PROCESS_ERROR(uSizeRead == uFileSize);

    //dwFileEndFlag = *(DWORD *)(m_pAniFileData + uFileSize - sizeof(DWORD));
    //KGLOG_PROCESS_ERROR(dwFileEndFlag == CLIP_FILE_END_FLAG);

    uBufferSize = uFileSize;

    KGLOG_PROCESS_ERROR(uBufferSize > sizeof(AniFileHeaderInfo));
    m_pAniFileHeader = (AniFileHeaderInfo *)m_pAniFileData;

    KGLOG_PROCESS_ERROR(m_pAniFileHeader->dwMask == CLIP_FILE_MASK);

    switch (m_pAniFileHeader->dwType)
    {
    case ANIMATION_BONE_RTS:
        {
            m_dwBoneNum = m_pAniFileHeader->BoneAni.dwNumBones;
            KGLOG_PROCESS_ERROR(m_dwBoneNum < MAX_BONE_NUM);

            uBufferSize -= sizeof(AniFileHeaderInfo);

            KGLOG_PROCESS_ERROR(uBufferSize > m_dwBoneNum * MAX_BONENAME_LEN);
            pszBoneNameBegin = m_pAniFileData + sizeof(AniFileHeaderInfo);
            for (DWORD i = 0; i < m_dwBoneNum; i++)
            {
                m_pszBoneName[i] = pszBoneNameBegin;
                pszBoneNameBegin += MAX_BONENAME_LEN;
            }

            uBufferSize -= m_dwBoneNum * MAX_BONENAME_LEN;

            dwNumFrames = m_pAniFileHeader->BoneAni.dwNumFrames;

            // file may end without CLIP_FILE_END_FLAG, so use '>=' instead of '>'
            KGLOG_PROCESS_ERROR(uBufferSize >= sizeof(RTS) * dwNumFrames * m_dwBoneNum);
            pBoneRTSBegin = (RTS *)pszBoneNameBegin;
            for (DWORD i = 0; i < m_dwBoneNum; i++)
            {
                m_pAnimatedBoneRTS[i] = pBoneRTSBegin;
                pBoneRTSBegin += dwNumFrames;
            }

            break;
        }
    default:
        KGLogPrintf(KGLOG_INFO, "Ignore Animation Type %u", m_pAniFileHeader->dwType);
        KG_PROCESS_ERROR(false);
    }

    m_sClipName = cszFileName;

    nResult = true;
Exit0:
    if (!nResult)
    {
        KG_DELETE_ARRAY(m_pAniFileData);

        KGLogPrintf(KGLOG_INFO, "ani load failed %s", cszFileName);
    }
    KG_COM_RELEASE(piFile);
    return nResult;
}

int KG_Clip::SkeletonCompare(KG_Bip *pBip, FILE *fpResultFile)
{
    int nResult  = false;
    int nRetCode = false;
    const char *cpszBoneName = NULL;
    unsigned uBoneIndex = 0;
    unsigned uBipBone = 0;
    int nDismatchFlag = false;

    KGLOG_PROCESS_ERROR(pBip);
    KGLOG_PROCESS_ERROR(fpResultFile);

    uBipBone = pBip->GetNumBone();
    for (unsigned i = 0; i < uBipBone; i++)
    {
        cpszBoneName = pBip->GetBoneNameByIndex(i);
        KGLOG_PROCESS_ERROR(cpszBoneName);

        uBoneIndex = _FindBoneIndex(cpszBoneName);
        KGLOG_PROCESS_ERROR(uBoneIndex != -1);

        if (uBoneIndex != i)
        {
            nRetCode = _SwapBoneAnimationData(i, uBoneIndex);
            KGLOG_PROCESS_ERROR(nRetCode);

            nDismatchFlag = true;
        }
    }

    if (nDismatchFlag)
    {
        nRetCode = _SaveNewFile();
        KGLOG_PROCESS_ERROR(nRetCode);

        nRetCode = fprintf(fpResultFile, "Adjust! %s\n", m_sClipName.c_str());
        KGLOG_PROCESS_ERROR(nRetCode > 0);
    }

    nResult = true;
Exit0:
    return nResult;
}

unsigned KG_Clip::GetBoneNum() const
{
    unsigned uRetBoneNum = 0;

    KGLOG_PROCESS_ERROR(m_pAniFileHeader);

    uRetBoneNum = (unsigned)m_pAniFileHeader->BoneAni.dwNumBones;

Exit0:
    return uRetBoneNum;
}

int KG_Clip::_Clear()
{
    KG_DELETE_ARRAY(m_pAniFileData);

    return true;
}

unsigned KG_Clip::_FindBoneIndex(const char cszBoneName[])
{
    unsigned uResult = (unsigned)-1;

    KGLOG_PROCESS_ERROR(cszBoneName);

    for (DWORD i = 0; i < m_dwBoneNum; i++)
    {
        ASSERT(m_pszBoneName[i]);
        if (!_stricmp(m_pszBoneName[i], cszBoneName))
        {
            return i;
        }
    }

Exit0:
    return uResult;
}

int KG_Clip::_SwapBoneAnimationData(unsigned uBone1, unsigned uBone2)
{
    char *pszBoneNameTemp = NULL;
    RTS *pBoneRTSTemp = NULL;

    ASSERT(uBone1 != uBone2);

    ASSERT(m_pAnimatedBoneRTS[uBone1]);
    ASSERT(m_pAnimatedBoneRTS[uBone2]);
    pBoneRTSTemp = m_pAnimatedBoneRTS[uBone1];
    m_pAnimatedBoneRTS[uBone1] = m_pAnimatedBoneRTS[uBone2];
    m_pAnimatedBoneRTS[uBone2] = pBoneRTSTemp;

    ASSERT(m_pszBoneName[uBone1]);
    ASSERT(m_pszBoneName[uBone2]);
    pszBoneNameTemp = m_pszBoneName[uBone1];
    m_pszBoneName[uBone1] = m_pszBoneName[uBone2];
    m_pszBoneName[uBone2] = pszBoneNameTemp;

    return true;
}

int KG_Clip::_SaveNewFile()
{
    int nResult = false;
    size_t uWriteSize = 0;
    FILE* fpFile = NULL;
    DWORD dwNumFrames = 0;

    fpFile = fopen(m_sClipName.c_str(), "wb");
    KGLOG_PROCESS_ERROR(fpFile);

    ASSERT(m_pAniFileHeader);
    uWriteSize = fwrite(m_pAniFileHeader, sizeof(AniFileHeaderInfo), 1, fpFile);
    KGLOG_PROCESS_ERROR(uWriteSize == 1);

    for (DWORD i = 0; i < m_dwBoneNum; i++)
    {
        ASSERT(m_pszBoneName[i]);
        uWriteSize = fwrite(m_pszBoneName[i], MAX_BONENAME_LEN, 1, fpFile); 
        KGLOG_PROCESS_ERROR(uWriteSize == 1);
    }

    dwNumFrames = m_pAniFileHeader->BoneAni.dwNumFrames;
    for (DWORD i = 0; i < m_dwBoneNum; i++)
    {
        uWriteSize = fwrite(m_pAnimatedBoneRTS[i], sizeof(RTS) * dwNumFrames, 1, fpFile);
        KGLOG_PROCESS_ERROR(uWriteSize == 1);
    }

    uWriteSize = fwrite(&CLIP_FILE_END_FLAG, sizeof(DWORD), 1, fpFile);
    KGLOG_PROCESS_ERROR(uWriteSize == 1);

    nResult = true;
Exit0:
    if (fpFile)
    {
        fclose(fpFile);
        fpFile = NULL;
    }
    if (!nResult)
    {
        KGLogPrintf(KGLOG_INFO, "save new clip failed %s", m_sClipName.c_str());
    }
    return nResult;
}

////////////////////////////////////////////////////////////////////////////////
int _GetAniFilePath(const char cszBipFile[], unsigned uAniFilePathSize, char szAniFilePath[])
{
    int nResult  = false;
    char *pszFind = NULL;
    char *pchFind = NULL;
    char *pszBipFile = NULL;
    unsigned uAniFilePathLength = 0;

    ASSERT(cszBipFile);
    ASSERT(uAniFilePathSize > 0);
    ASSERT(szAniFilePath);

    pszBipFile = (char *)cszBipFile;

    pszFind = strstr(pszBipFile, "\\模型\\");
    if (pszFind)
    {
        uAniFilePathLength = (unsigned)(pszFind - pszBipFile);
        KGLOG_PROCESS_ERROR(uAniFilePathLength + sizeof("\\动作") <= uAniFilePathSize);
        memcpy(szAniFilePath, pszBipFile, uAniFilePathLength);
        strncpy(szAniFilePath + uAniFilePathLength, "\\动作", uAniFilePathSize - uAniFilePathLength);
        szAniFilePath[uAniFilePathSize - 1] = '\0';

        KG_PROCESS_SUCCESS(true);
    }

    pszFind = strstr(pszBipFile, "\\部件\\");
    if (pszFind)
    {
        uAniFilePathLength = (unsigned)(pszFind - pszBipFile);
        KGLOG_PROCESS_ERROR(uAniFilePathLength + sizeof("\\动作") <= uAniFilePathSize);
        memcpy(szAniFilePath, pszBipFile, uAniFilePathLength);
        strncpy(szAniFilePath + uAniFilePathLength, "\\动作", uAniFilePathSize - uAniFilePathLength);
        szAniFilePath[uAniFilePathSize - 1] = '\0';

        KG_PROCESS_SUCCESS(true);
    }

    pchFind = strrchr(pszBipFile, '\\');
    if (pchFind)
    {
        uAniFilePathLength = (unsigned)(pchFind - pszBipFile);
        KGLOG_PROCESS_ERROR(uAniFilePathLength <  uAniFilePathSize);
        memcpy(szAniFilePath, pszBipFile, uAniFilePathLength);
        szAniFilePath[uAniFilePathLength] = '\0';

        KG_PROCESS_SUCCESS(true);
    }

    KG_PROCESS_ERROR(false);

Exit1:
    nResult = true;
Exit0:
    return nResult;
}

int _GetAniList(const char cszBipFile[], vector<string> *pvecRetAniList)
{
    int nResult  = false;
    int nRetCode = false;
    char szCmd[MAX_PATH];
    char szAniFileName[MAX_PATH];
    FILE *fpFile = NULL;
    char szAniFilePath[MAX_PATH];

    ASSERT(cszBipFile);
    ASSERT(pvecRetAniList);

    nRetCode = _GetAniFilePath(cszBipFile, sizeof(szAniFilePath), szAniFilePath);
    KG_PROCESS_ERROR(nRetCode);

    nRetCode = snprintf(szCmd, sizeof(szCmd) - 1, "dir /s /b \"%s\\*.ani\" > temp_anilist.dat", szAniFilePath);
    ASSERT((nRetCode > 0) && (nRetCode <= sizeof(szCmd) - 1));
    szCmd[sizeof(szCmd) - 1] = '\0';

    nRetCode = system(szCmd);
    KG_PROCESS_ERROR(nRetCode == 0);

    fpFile = fopen("temp_anilist.dat", "r");
    KGLOG_PROCESS_ERROR(fpFile);

    while (true)
    {
        char *pRetCode = fgets(szAniFileName, sizeof(szAniFileName) - 1, fpFile);
        if (!pRetCode)
            break;
        szAniFileName[sizeof(szAniFileName) - 1] = '\0';

        size_t uLen = strlen(szAniFileName);
        ASSERT(uLen > 1);
        size_t i = uLen - 1;
        while (szAniFileName[i] == '\r' || szAniFileName[i] == '\n')
        {
            ASSERT(i > 0);
            --i;
        }
        szAniFileName[i + 1] = '\0';

        pvecRetAniList->push_back(szAniFileName);
    }

    nResult = true;
Exit0:
    if (fpFile)
    {
        fclose(fpFile);
        fpFile = NULL;
    }
    if (!nResult)
    {
        KGLogPrintf(KGLOG_INFO, "bip \"%s\" has none ani file", cszBipFile);
    }
    remove("temp_anilist.dat");
    return nResult;
}

int AnalyseAni(vector<string> &vecBipName)
{
    int nResult  = false;
    int nRetCode = false;
    vector<string> vecAniList;
    size_t uAniListSize = 0;
    size_t uBipListSize = 0;
    KG_Bip *pBip = NULL;
    FILE *fpFile = NULL;
    char szResultFileName[MAX_PATH];
    time_t tmtNow = 0;
    struct tm tmNow; 

    tmtNow = time(NULL);

    localtime_r(&tmtNow, &tmNow);

    nRetCode = snprintf(
        szResultFileName, sizeof(szResultFileName) - 1,
        "AniCheckResult_%d_%2.2d_%2.2d_%2.2d_%2.2d_%2.2d.txt",
        tmNow.tm_year + 1900, tmNow.tm_mon + 1, tmNow.tm_mday,
        tmNow.tm_hour, tmNow.tm_min, tmNow.tm_sec
    );
    ASSERT(nRetCode >0 && nRetCode < (sizeof(szResultFileName) - 1));
    szResultFileName[sizeof(szResultFileName) - 1] = '\0';

    fpFile = fopen(szResultFileName, "w");
    KGLOG_PROCESS_ERROR(fpFile);

    uBipListSize = vecBipName.size();
    for (size_t i = 0; i < uBipListSize; i++)
    {
        vecAniList.clear();
        nRetCode = _GetAniList(vecBipName[i].c_str(), &vecAniList);
        if (!nRetCode)
            continue;

        KG_DELETE(pBip);
        pBip = new KG_Bip;
        KGLOG_PROCESS_ERROR(pBip);

        nRetCode = pBip->LoadFromFile(vecBipName[i].c_str(), false);
        if (!nRetCode)
        {
            nRetCode = fprintf(fpFile, "Bip load failed \"%s\" \n", vecBipName[i].c_str());
            KGLOG_PROCESS_ERROR(nRetCode > 0);

            continue;
        }

        uAniListSize = vecAniList.size();
        for (size_t j = 0; j < uAniListSize; j++)
        {
            nRetCode = pBip->CheckClip(vecAniList[j].c_str(), fpFile);
            if (!nRetCode)
                continue;
        }
    }

    nResult = true;
Exit0:
    KG_DELETE(pBip);
    if (fpFile)
    {
        fclose(fpFile);
        fpFile = NULL;
    }
    return nResult;
}
