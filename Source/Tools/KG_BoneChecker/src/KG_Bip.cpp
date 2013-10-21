#include "stdafx.h"
#include "KG_Bip.h"
#include "KG_Clip.h"
#include <algorithm>

////////////////////////////////////////////////////////////////////////////////
inline int _GetLine(char **ppszBuffer, unsigned uLineBufferSize, char szRetLine[])
{
    int nResult  = false;
    char *pszLineBegin = NULL;
    char *pchLineEnd = NULL;
    unsigned uLineLength = 0;

    ASSERT(ppszBuffer);
    ASSERT(*ppszBuffer);
    ASSERT(uLineBufferSize > 0);
    ASSERT(szRetLine);
    KGLOG_PROCESS_ERROR(**ppszBuffer); // not a empty string

    pszLineBegin = *ppszBuffer;

    pchLineEnd = pszLineBegin;
    while (*pchLineEnd && (*pchLineEnd != '\r') && (*pchLineEnd != '\n'))
        ++pchLineEnd;

    uLineLength = (unsigned)(pchLineEnd - pszLineBegin);
    KGLOG_PROCESS_ERROR(uLineLength < uLineBufferSize);
    memcpy(szRetLine, pszLineBegin, uLineLength);
    szRetLine[uLineLength] = '\0';
   
    // seek to next line
    pszLineBegin = pchLineEnd;
    while (*pszLineBegin && (*pszLineBegin == '\r' || *pszLineBegin == '\n'))
        ++pszLineBegin;
    *ppszBuffer = pszLineBegin;

    nResult = true;
Exit0:
    return nResult;
}

////////////////////////////////////////////////////////////////////////////////
KG_Bip::KG_Bip()
{
    m_uNumBones = 0;
    m_sBipName = "";
    m_pSkeleton = NULL;
}

KG_Bip::~KG_Bip()
{
    KG_DELETE_ARRAY(m_pSkeleton);
}

int KG_Bip::LoadFromFile(const char cszFileName[], int nSortNameFlag)
{
    int nResult  = false;
    int nRetCode = false;
    IFile *piFile = NULL;
    unsigned uFileSize = 0;
    unsigned uReadSize = 0;
    char *pszFileBuffer = NULL;
    char *pszBuffer = NULL;
    char szLine[1024];
    unsigned uNumBaseBones = 0;

    KGLOG_PROCESS_ERROR(cszFileName);

    piFile = g_OpenFile(cszFileName);
    KGLOG_PROCESS_ERROR(piFile);

    uFileSize = piFile->Size();
    KGLOG_PROCESS_ERROR(uFileSize > 0);

    pszFileBuffer = new char[uFileSize + 1];
    KGLOG_PROCESS_ERROR(pszFileBuffer);

    uReadSize = piFile->Read(pszFileBuffer, uFileSize);
    KGLOG_PROCESS_ERROR(uReadSize == uFileSize);
    pszFileBuffer[uReadSize] = '\0';

    pszBuffer = pszFileBuffer;

    /// read bone num
    nRetCode = _GetLine(&pszBuffer, sizeof(szLine), szLine);
    KGLOG_PROCESS_ERROR(nRetCode);

    m_uNumBones = strtoul(szLine, NULL, 10);
    KGLOG_PROCESS_ERROR(m_uNumBones > 0);

    m_pSkeleton = new BoneInfoData[m_uNumBones];
    KGLOG_PROCESS_ERROR(m_pSkeleton);

    /// read bones
    for (unsigned i = 0; i < m_uNumBones; i++)
    {
        nRetCode = _GetLine(&pszBuffer, sizeof(szLine), szLine);
        KGLOG_PROCESS_ERROR(nRetCode);

        nRetCode = _LineToBoneInfo(szLine, nSortNameFlag, &m_pSkeleton[i]);
        KGLOG_PROCESS_ERROR(nRetCode);
    }

    if (nSortNameFlag)
    {
        sort(m_pSkeleton, m_pSkeleton + m_uNumBones, _BoneCmp);
    }

    for (unsigned i = 0; i < m_uNumBones; i++)
    {
        nRetCode = _CheckSkeleton(i);
        KGLOG_PROCESS_ERROR(nRetCode);
    }

    /// read base bone index
    nRetCode = _GetLine(&pszBuffer, sizeof(szLine), szLine);
    KGLOG_PROCESS_ERROR(nRetCode);

    uNumBaseBones = strtoul(szLine, NULL, 10);
    KGLOG_PROCESS_ERROR(uNumBaseBones > 0);

    for (unsigned i = 0; i < uNumBaseBones; i++)
    {
        nRetCode = _GetLine(&pszBuffer, sizeof(szLine), szLine);
        KGLOG_PROCESS_ERROR(nRetCode);

        unsigned uBaseBoneIndex = 0;
        uBaseBoneIndex = strtoul(szLine, NULL, 10);
        KGLOG_PROCESS_ERROR(uNumBaseBones > 0);

        m_vecBaseBones.push_back(uBaseBoneIndex);
    }

    m_sBipName = cszFileName;

    nResult = true;
Exit0:
    if (!nResult)
    {
        KG_DELETE_ARRAY(m_pSkeleton);
        KGLogPrintf(KGLOG_INFO, "bip load failed \"%s\"", cszFileName);
    }
    KG_DELETE_ARRAY(pszFileBuffer);
    KG_COM_RELEASE(piFile);
    return nResult;
}

int KG_Bip::CheckClip(const char cszClipName[], FILE *fpResultFile)
{
    int nResult  = false;
    int nRetCode = false;
    KG_Clip *pClip = NULL;
    unsigned uBoneNum = 0;

    KGLOG_PROCESS_ERROR(cszClipName);
    KGLOG_PROCESS_ERROR(fpResultFile);

    pClip = new KG_Clip;
    KGLOG_PROCESS_ERROR(pClip);

    nRetCode = pClip->LoadFromFile(cszClipName);
    KG_PROCESS_ERROR(nRetCode);

    uBoneNum = pClip->GetBoneNum();
    KGLOG_PROCESS_ERROR(uBoneNum > 0);

    if (uBoneNum != m_uNumBones)
    {
        nRetCode = fprintf(
            fpResultFile, 
            "======================\n"
            "ERROR !! bonenum dismatch! (%u %u) \n"
            "%s \n"
            "%s \n"
            "======================\n", 
            m_uNumBones, uBoneNum, m_sBipName.c_str(), cszClipName
        );
        KGLOG_PROCESS_ERROR(nRetCode > 0);

        KG_PROCESS_ERROR(false);
    }

    nRetCode = pClip->SkeletonCompare(this, fpResultFile);
    KG_PROCESS_ERROR(nRetCode);

    nResult = true;
Exit0:
    KG_DELETE(pClip);
    return nResult;
}

int KG_Bip::_LineToBoneInfo(
    const char szLineBuffer[], int nSortNameFlag, BoneInfoData *pRetBoneInfo
)
{
    int nResult  = false;
    char *pszLine = NULL;
    char *pszFind = NULL;
    unsigned uBoneNameLength = 0;
    unsigned uChildNum = 0;

    ASSERT(szLineBuffer);
    ASSERT(pRetBoneInfo);

    pszLine = (char *)szLineBuffer;

    pszFind = strstr(pszLine, "  ");
    if (pszFind)
        uBoneNameLength = (unsigned)(pszFind - pszLine);
    else
        uBoneNameLength = (unsigned)strlen(pszLine);

    KGLOG_PROCESS_ERROR(uBoneNameLength < sizeof(pRetBoneInfo->szBoneName));
    memcpy(pRetBoneInfo->szBoneName, pszLine, uBoneNameLength);
    pRetBoneInfo->szBoneName[uBoneNameLength] = '\0';

    pRetBoneInfo->uNumChildBones = 0;

    while (pszFind)
    {
        while (*pszFind == ' ') 
            pszFind++;

        if (*pszFind == '\0')
            break;

        char *pszEndOfChildName = strstr(pszFind, "  ");
        if (pszEndOfChildName)
            uBoneNameLength = (unsigned)(pszEndOfChildName - pszFind);
        else
            uBoneNameLength = (unsigned)strlen(pszFind);    //the last one

        KGLOG_PROCESS_ERROR(uBoneNameLength < sizeof(pRetBoneInfo->szChildName[uChildNum]));
        memcpy(pRetBoneInfo->szChildName[uChildNum], pszFind, uBoneNameLength);
        pRetBoneInfo->szChildName[uChildNum][uBoneNameLength] = '\0';
        ++uChildNum;

        pszFind = pszEndOfChildName;
    }

    pRetBoneInfo->uNumChildBones = uChildNum;

    if (nSortNameFlag)
    {
        qsort(
            pRetBoneInfo->szChildName, 
            pRetBoneInfo->uNumChildBones, 
            sizeof(pRetBoneInfo->szChildName[0]), 
            _StringCmp
        );
    }

    nResult = true;
Exit0:
    return nResult;
}

int KG_Bip::_CheckSkeleton(unsigned uIndex)
{
    int nResult = false;
    BoneInfoData *pInfo = NULL;
    unsigned uNumChild = 0;
    unsigned uBoneIndex = (unsigned)-1;

    ASSERT(uIndex < m_uNumBones);

    pInfo = &m_pSkeleton[uIndex];
    ASSERT(pInfo);

    uNumChild = pInfo->uNumChildBones;
    for (unsigned i = 0; i < uNumChild; i++)
    {
        uBoneIndex = _FindBoneIndex(pInfo->szChildName[i]);
        KGLOG_PROCESS_ERROR(uBoneIndex != (unsigned)-1);
    }

    nResult = true;
Exit0:
    return nResult;
}

unsigned KG_Bip::_FindBoneIndex(const char cszBoneName[])
{
    unsigned uResult = (unsigned)-1;

    KGLOG_PROCESS_ERROR(cszBoneName);

    for (unsigned i = 0; i < m_uNumBones; i++)
    {
        if (!strcmp(m_pSkeleton[i].szBoneName, cszBoneName))
            return i;
    }

Exit0:
    KGLogPrintf(KGLOG_INFO, "UnKnown Bone \"%s\"", cszBoneName);
    return uResult;
}

BoneInfoData* KG_Bip::GetSkeleton() const
{
    BoneInfoData* pRetSkeleton = NULL;

    KGLOG_PROCESS_ERROR(m_pSkeleton);
    pRetSkeleton = m_pSkeleton;

Exit0:
    return pRetSkeleton;
}

unsigned KG_Bip::GetNumBone() const
{
    return m_uNumBones;
}

const char* KG_Bip::GetBipName() const
{
    return m_sBipName.c_str();
}

const char* KG_Bip::GetBoneNameByIndex(unsigned uBoneIndex) const
{
    const char* pszRetBoneName = NULL;

    KGLOG_PROCESS_ERROR(uBoneIndex < m_uNumBones);

    ASSERT(m_pSkeleton);
    pszRetBoneName = m_pSkeleton[uBoneIndex].szBoneName;

Exit0:
    return pszRetBoneName;
}

////////////////////////////////////////////////////////////////////////////////
int AnalyseBip(vector<string> &vecBipName)
{
    int nResult  = false;
    int nRetCode = false;
    KG_Bip **ppBip = NULL;
    size_t uBipCount = 0;
    FILE *fpFile = NULL;
    char szResultFileName[MAX_PATH];
    time_t tmtNow = 0;
    struct tm tmNow; 

    uBipCount = vecBipName.size();

    ppBip = new KG_Bip*[uBipCount];
    KGLOG_PROCESS_ERROR(ppBip);
    memset(ppBip, 0, sizeof(KG_Bip *) * uBipCount);

    tmtNow = time(NULL);

    localtime_r(&tmtNow, &tmNow);

    nRetCode = snprintf(
        szResultFileName, sizeof(szResultFileName) - 1,
        "BipCheckResult_%d_%2.2d_%2.2d_%2.2d_%2.2d_%2.2d.txt",
        tmNow.tm_year + 1900, tmNow.tm_mon + 1, tmNow.tm_mday,
        tmNow.tm_hour, tmNow.tm_min, tmNow.tm_sec
    );
    ASSERT(nRetCode >0 && nRetCode < (sizeof(szResultFileName) - 1));
    szResultFileName[sizeof(szResultFileName) - 1] = '\0';

    fpFile = fopen(szResultFileName, "w");
    KGLOG_PROCESS_ERROR(fpFile);

    for (size_t i = 0; i < uBipCount; i++)
    {
        ppBip[i] = new KG_Bip;
        KGLOG_PROCESS_ERROR(ppBip[i]);

        nRetCode = ppBip[i]->LoadFromFile(vecBipName[i].c_str(), true);
        if (!nRetCode)
        {
            nRetCode = fprintf(
                fpFile, 
                "##ERROR \n"
                "Bip load failed \"%s\" \n", 
                vecBipName[i].c_str()
            );
            KGLOG_PROCESS_ERROR(nRetCode > 0);

            KG_DELETE(ppBip[i]);
        }
    }

    for (size_t i = 0; i < uBipCount; i++)
    {
        if (!ppBip[i])
            continue;

        unsigned uBoneCount1 = ppBip[i]->GetNumBone();
        for (size_t j = i + 1; j < uBipCount; j++)
        {
            if (!ppBip[j])
                continue;

            unsigned uBoneCount2 = ppBip[j]->GetNumBone();
            if (uBoneCount1 == uBoneCount2)
            {
                BoneInfoData *pSkeleton1 = ppBip[i]->GetSkeleton();
                BoneInfoData *pSkeleton2 = ppBip[j]->GetSkeleton();
                nRetCode = memcmp(pSkeleton1, pSkeleton2, sizeof(BoneInfoData) * uBoneCount1);
                if (nRetCode)
                    continue;
                
                nRetCode = fprintf(
                    fpFile,
                    "##WARNING same skeleton\n"
                    "%s\n"
                    "%s\n",
                    ppBip[i]->GetBipName(), ppBip[j]->GetBipName()
                );
                KGLOG_PROCESS_ERROR(nRetCode > 0);
            }
        }
    }

    nResult = true;
Exit0:
    if (fpFile)
    {
        fclose(fpFile);
        fpFile = NULL;
    }
    if (ppBip)
    {
        for (size_t i = 0; i < uBipCount; i++)
        {
            KG_DELETE(ppBip[i]);
        }
    }
    KG_DELETE_ARRAY(ppBip);

    return nResult;
}