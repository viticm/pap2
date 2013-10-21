#include "StdAfx.h"
#include <conio.h>
#include "KWorldSettings.h"
#include "KMonsterManager.h"

KMonsterManager g_MonsterManager;

KMonsterManager::KMonsterManager(void)
{    
    m_nMaxIndex = 0;
}

KMonsterManager::~KMonsterManager(void)
{
}

int KMonsterManager::Init()
{
    m_nMaxIndex = 0;
    return TRUE;
}

int KMonsterManager::UnInit()
{
    DestroyAllMonster();
    return TRUE;
}

int KMonsterManager::CreateMonster(int nCount)
{
    int nRetCode = FALSE;
    int nResult  = FALSE;
    int nExistCount = 0;
    int nRoleIndex  = 0;
    MONSTER_PARAM MonsterParam;
    
    nRoleIndex = ReadRoleIndex();
    SaveRoleIndex(nRoleIndex + nCount);

    strcpy(MonsterParam.szBishopIP, g_WorldSettings.m_RobotParam.szBishopIP);
    MonsterParam.nBishopPort = g_WorldSettings.m_RobotParam.nBishopPort;    
    strcpy(MonsterParam.szPassword, g_WorldSettings.m_RobotParam.szPassword);
    MonsterParam.nArrangement  = g_WorldSettings.m_RobotParam.nArrangement;
    MonsterParam.nRange        = g_WorldSettings.m_RobotParam.nRange;
    MonsterParam.nRunRate      = g_WorldSettings.m_RobotParam.nRunRate;
    MonsterParam.nReconnect    = g_WorldSettings.m_RobotParam.nReconnect;
    MonsterParam.nMonsterTotal = m_MonsterList.size() + nCount;
    
    printf("\nPress Q to cancel...\n");
    for (int nIndex = 0; nIndex < nCount; ++nIndex)
    {
        sprintf(
            MonsterParam.szAccountName, 
            "%s%d", 
            g_WorldSettings.m_RobotParam.szAccountNamePrefix, 
            m_nMaxIndex + g_WorldSettings.m_RobotParam.nStartIndex
        );
        MonsterParam.nSerialNumber = m_nMaxIndex + 1;
        MonsterParam.nCreateRoleSN = nRoleIndex + m_nMaxIndex;
        ++m_nMaxIndex;

        KMonster *pMonster = new KMonster();
        KG_PROCESS_ERROR(pMonster);

        nRetCode = pMonster->Init(MonsterParam);
        KG_PROCESS_ERROR(nRetCode);

        m_MonsterList.push_back(pMonster);

        printf("\rCreating Robot ...... %d%%", (nIndex + 1) * 100 / nCount);
        KGThread_Sleep(g_WorldSettings.m_RobotParam.nStartInterval);

        if (kbhit() && getch() == 'q')
            goto Exit0;
    }

    nResult = TRUE;
Exit0:
    return nResult;
}

int KMonsterManager::DestroyMonster(int nCount)
{
    int nMaxCount = m_MonsterList.size();
    MONSTER_LIST::iterator it = m_MonsterList.begin();

    if (m_MonsterList.empty())
    {
        printf("...未启动...\n");
    }

    nMaxCount = min(nMaxCount, nCount);

    for (int i = 0; i < nMaxCount; ++i)
    {
        if (it == m_MonsterList.end())
            break;

        KMonster *pMonster = *it;

        if (pMonster)
        {
            pMonster->UnInit();
            delete pMonster;
            pMonster = NULL;
        }

        it = m_MonsterList.erase(it);

        printf("\rDestroying Robot ...... %d%%", (i + 1) * 100 / nMaxCount);
    }

    return TRUE;
}

int KMonsterManager::DestroyAllMonster()
{
    int nCount = m_MonsterList.size();
    
    DestroyMonster(nCount);

    return TRUE;
}

int KMonsterManager::QueryAllMonsterState()
{    
    MONSTER_LIST::iterator it;
    int nIndex = 0;

    if (m_MonsterList.empty())
    {
        printf("...无法获取列表，未启动...\n");
        goto Exit0;
     }

    printf("序号\t帐号\t状态\t错误信息\n");
    for (it = m_MonsterList.begin(); it != m_MonsterList.end(); ++it)
    {
        KMonster *pMonster = *it;
        printf(
            "%d\t%s\t%s\t%s\n", 
            ++nIndex, 
            pMonster->m_MonsterParam.szAccountName,
            pMonster->GetStateDescription(),
            pMonster->GetFailDescription()
        );
    }

Exit0:
    return TRUE;
}

int KMonsterManager::StateStatistic()
{
    MONSTER_LIST::iterator it;
    int  nOnline = 0;
    int  nOffline = 0;
    int  nLogin = 0;

    for (it = m_MonsterList.begin(); it != m_MonsterList.end(); ++it)
    {
        KMonster *pMonster = *it;
        switch (pMonster->GetOnlineState())
        {
        case osGameserverOnline:
            nOnline++;
            break;
        case osGameserverOffline:
        case osBishopOffline:
            nOffline++;
            break;
        case osBishopOnline:
            nLogin++;
            break;
        default:
            break;
        }
    }
    printf(
        "\n\t|Online  |\t%d|\n"
          "\t|Offline |\t%d|\n"
          "\t|Login   |\t%d|\n",
        nOnline, nOffline, nLogin
    );

    return TRUE;
}

int KMonsterManager::ReadRoleIndex()
{
    int nRetCode = FALSE;
    int nResult  = 0;
    
    FILE *pFile = NULL;
    char *pszTmpStr = NULL;
    char szBuffer[256];
    szBuffer[0] = '\0';

    nRetCode = fopen_s(&pFile, ROLE_INDEX_FILE, "r");
    KG_PROCESS_ERROR(nRetCode == 0);

    pszTmpStr = fgets(szBuffer, sizeof(szBuffer), pFile);
    KG_PROCESS_ERROR(pszTmpStr);
    szBuffer[sizeof(szBuffer) - 1] = '\0';

    pszTmpStr = szBuffer;
    while (*pszTmpStr)
    {
        if (*pszTmpStr < '0' || *pszTmpStr > '9')
            goto Exit0;

        ++pszTmpStr;
    }

    nResult = atoi(szBuffer);
    KG_PROCESS_ERROR(nResult >= 0);

Exit0:
    if (pFile)
    {
        fclose(pFile);
        pFile = NULL;
    }    
    return nResult;
}

int KMonsterManager::SaveRoleIndex(int nIndex)
{
    int nRetCode = FALSE;
    int nResult  = 0;

    FILE *pFile = NULL;
    char szBuffer[256];
    szBuffer[0] = '\0';
    
    nRetCode = fopen_s(&pFile, ROLE_INDEX_FILE, "w");
    KG_PROCESS_ERROR(nRetCode == 0);

    sprintf(szBuffer, "%d", nIndex);

    fputs(szBuffer, pFile);
    fflush(pFile);

Exit0:
    if (pFile)
    {
        fclose(pFile);
        pFile = NULL;
    }
    return TRUE;;
}
