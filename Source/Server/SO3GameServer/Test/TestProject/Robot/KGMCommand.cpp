#include "stdafx.h"
#include "KGMCommand.h"
#include "KMonster.h"
#include "KPlayerClient.h"

#define GM_COMMAND_BUFFER_SIZE (256)

int KGMCommand::SetPosition(KMonster *pMonster, int nDestX, int nDestY, int nDestZ)
{
    int nRetCode = FALSE;
    char szGM[GM_COMMAND_BUFFER_SIZE];

    ASSERT(pMonster);

    snprintf(szGM, sizeof(szGM), "player.SetPosition(%d, %d, %d)", nDestX, nDestY, nDestZ);
    szGM[sizeof(szGM) - 1] = '\0';

    nRetCode = pMonster->m_PlayerClient.DoGmCommand(szGM);
    return nRetCode;
}

int KGMCommand::AddExp(KMonster *pMonster, int nExp)
{
    int nRetCode = FALSE;
    char szGM[GM_COMMAND_BUFFER_SIZE];

    ASSERT(pMonster);

    snprintf(szGM, sizeof(szGM), "player.AddExp(%d)", nExp);
    szGM[sizeof(szGM) - 1] = '\0';

    nRetCode = pMonster->m_PlayerClient.DoGmCommand(szGM);
    return nRetCode;
}

int KGMCommand::LearnSkill(KMonster *pMonster, DWORD dwSkillID)
{
    int nRetCode = FALSE;
    char szGM[GM_COMMAND_BUFFER_SIZE];

    ASSERT(pMonster);

    snprintf(szGM, sizeof(szGM), "player.LearnSkill(%d)", dwSkillID);
    szGM[sizeof(szGM) - 1] = '\0';

    nRetCode = pMonster->m_PlayerClient.DoGmCommand(szGM);
    return nRetCode;
}

int KGMCommand::AddItem(KMonster *pMonster, DWORD dwTabType, DWORD dwIndex, int nStackNum)
{
    int nRetCode = FALSE;
    char szGM[GM_COMMAND_BUFFER_SIZE];

    ASSERT(pMonster);

    snprintf(szGM, sizeof(szGM), "player.AddItem(%u, %d, %d)", dwTabType, dwIndex, nStackNum);
    szGM[sizeof(szGM) - 1] = '\0';

    nRetCode = pMonster->m_PlayerClient.DoGmCommand(szGM);
    return nRetCode;
}