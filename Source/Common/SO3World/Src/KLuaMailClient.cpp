#include "stdafx.h"
#include "KMailClient.h"
#include "KSO3World.h"
#include "Engine/KTextFilter.h"
#include "KPlayer.h"
#include "KItem.h"
#include "KPlayerClient.h"
#include <algorithm>

#ifdef _CLIENT

#include "SO3UI/SO3GameWorldUIHandler.h"

int KMailClient::LuaSendMail(Lua_State* L)
{
    int         nResult                      = mrcFailed;
    int         nRetCode                     = 0;
    DWORD       dwNpcID                      = ERROR_ID;
    int         nRequestID                   = 0;
    KPlayer*    pPlayer                      = NULL;
    KNpc*       pNpc                         = NULL;
    char*       pDestName                    = NULL;
    DWORD       dwBox[KMAIL_MAX_ITEM_COUNT]  = { ibInvalid };
    DWORD       dwX[KMAIL_MAX_ITEM_COUNT]    = { 0 };
    int         nPrice[KMAIL_MAX_ITEM_COUNT] = { 0 };
    int         nMoney                       = 0;
    char*       pszMailTitle                 = NULL;
    char*       pszText                      = NULL;
    size_t      uTextLen                     = 0;
    int         nPlayerMoney                 = 0;
    BOOL        bItem                        = false;
	KItem*		pItem						 = NULL;

    for (int i = 0; i < KMAIL_MAX_ITEM_COUNT; i++)
    {
        dwBox[i] = ibInvalid;
    }

    nRetCode = Lua_GetTopIndex(L);
    KGLOG_PROCESS_ERROR(nRetCode == 7);

    nRequestID   = (int)Lua_ValueToNumber(L, 1);
    dwNpcID      = (DWORD)Lua_ValueToNumber(L, 2);

    pPlayer = g_pSO3World->m_PlayerSet.GetObj(g_pSO3World->m_dwClientPlayerID);
    KGLOG_PROCESS_ERROR(pPlayer);

    pNpc    = g_pSO3World->m_NpcSet.GetObj(dwNpcID);
    KGLOG_PROCESS_ERROR(pNpc);

    nRetCode = g_InRange(pPlayer, pNpc, COMMON_PLAYER_OPERATION_DISTANCE);
    KG_PROCESS_ERROR_RET_CODE(nRetCode, mrcTooFarAway);

    pDestName    = (char*)Lua_ValueToString(L, 3);
    pszMailTitle = (char*)Lua_ValueToString(L, 4);
    pszText      = (char*)Lua_ValueToString(L, 5);
	nMoney       = (int)Lua_ValueToNumber(L, 6);
	if (Lua_IsTable(L, 7))
	{
		for (int i = 0; i < KMAIL_MAX_ITEM_COUNT; ++i)
		{
			Lua_PushNumber(L, i + 1);
			Lua_GetTable(L, -2);
			if (Lua_IsTable(L, -1))
			{
				Lua_PushString(L, "dwBox");
				Lua_GetTable(L, -2);
				dwBox[i] = (DWORD)Lua_ValueToNumber(L, -1);
				Lua_Pop(L, 1);

				Lua_PushString(L, "dwX");
				Lua_GetTable(L, -2);
				dwX[i] = (DWORD)Lua_ValueToNumber(L, -1);
				Lua_Pop(L, 1);

				Lua_PushString(L, "nPrice");
				Lua_GetTable(L, -2);
				nPrice[i] = (int)Lua_ValueToNumber(L, -1);
				Lua_Pop(L, 1);

				KGLOG_PROCESS_ERROR(nPrice >= 0);

                nPrice[i] = 0; // 货到付款功能暂时不开放

				int nBoxType = pPlayer->m_ItemList.GetBoxType(dwBox[i]);
				KG_PROCESS_ERROR_RET_CODE(nBoxType == ivtPackage, mrcItemNotInPackage);

				pItem = pPlayer->m_ItemList.GetItem(dwBox[i], dwX[i]);
				KGLOG_PROCESS_ERROR(pItem);

				KG_PROCESS_ERROR_RET_CODE(!pItem->IsBind(), mrcItemBeBind);
                assert(pItem->m_pItemInfo);
                KG_PROCESS_ERROR_RET_CODE(pItem->m_pItemInfo->nExistType == ketPermanent, mrcTimeLimitItem);
			}
			Lua_Pop(L, 1);
		}
	}

    KGLOG_PROCESS_ERROR(pDestName);
    KGLOG_PROCESS_ERROR(pszMailTitle);
    KGLOG_PROCESS_ERROR(pszText);
    KGLOG_PROCESS_ERROR(nMoney >= 0);

    uTextLen = strlen(pszText) + 1;
    KG_PROCESS_ERROR(uTextLen <= MAX_MAIL_TEXT_LEN);

    nPlayerMoney = pPlayer->m_ItemList.GetMoney();
    KG_PROCESS_ERROR_RET_CODE(nPlayerMoney >= (nMoney + KMAIL_COST), mrcNotEnoughMoney);

    nRetCode = g_PlayerClient.DoSendMailRequest(
        dwNpcID, nRequestID, pDestName, pszMailTitle, nMoney, (BYTE*)pszText, uTextLen, dwBox, dwX, nPrice
    );
    KGLOG_PROCESS_ERROR(nRetCode);

    nResult = mrcSucceed;
Exit0:
    if (nResult != mrcSucceed)
    {
        if (g_pGameWorldUIHandler)
        {
            KUIEventMailRespond MailRespond = { nResult };
            g_pGameWorldUIHandler->OnMailRespond(MailRespond);
        }
    }

    return 0;
}

int KMailClient::LuaDeleteMail(Lua_State* L)
{
    int                     nRetCode    = false;
    DWORD                   dwMailID    = 0;
	KMailInfo*				pMailInfo   = NULL;
    KMailTable::iterator	it;

    nRetCode = Lua_GetTopIndex(L);
    KGLOG_PROCESS_ERROR(nRetCode == 1);

    dwMailID = (int)Lua_ValueToNumber(L, 1);

    it = m_MailTable.find(dwMailID);
    KGLOG_PROCESS_ERROR(it != m_MailTable.end());

	pMailInfo = it->second;
	assert(pMailInfo);

    if (pMailInfo->bItemFlag) // 如果邮件中有未取的道具，拒绝删除
    {
        if (g_pGameWorldUIHandler)
        {
            KUIEventMailRespond MailRespond;
            MailRespond.nRespondCode = mrcDeleteRefused;
            g_pGameWorldUIHandler->OnMailRespond(MailRespond);
        }

        goto Exit0;
    }

	delete pMailInfo;
    
    m_MailTable.erase(it);

    nRetCode = g_PlayerClient.DoDeleteMail(dwMailID);
    KGLOG_PROCESS_ERROR(nRetCode);

Exit0:
    return 0;
}

int KMailClient::LuaCountMail(Lua_State* L)
{
    CountMail();

    Lua_PushNumber(L, m_nUnreadCount);
    Lua_PushNumber(L, m_nTotalCount);
    Lua_PushNumber(L, m_nSysUnreadCount);
    Lua_PushNumber(L, m_nSysCount);

    return 4;
}

int KMailClient::LuaApplyMailList(Lua_State* L)
{
    if (!m_bMailListApplying)
    {
        Clear();

        g_PlayerClient.DoGetMailListRequest(0);

        m_bMailListApplying = true;
    }

    return 0;
}

int KMailClient::LuaGetMailList(Lua_State* L)
{
    int nResult     = 0;
    int nRetCode    = 0;
    int nCount      = 0;
	const char * pcszFilter = NULL;

    nRetCode = Lua_GetTopIndex(L);
    KGLOG_PROCESS_ERROR(nRetCode == 1);

	pcszFilter = Lua_ValueToString(L, 1);
	KGLOG_PROCESS_ERROR(pcszFilter);

    Lua_NewTable(L);

	if (strcmp(pcszFilter, "player") == 0)
	{
		for (KMailTable::iterator it = m_MailTable.begin(); it != m_MailTable.end(); ++it)
		{
			KMailInfo*  pMailInfo   = it->second;
			if (pMailInfo && pMailInfo->byType == eMailType_Player)
			{
				Lua_PushNumber(L, ++nCount);
				Lua_PushNumber(L, (int)pMailInfo->dwMailID);
				Lua_SetTable(L, -3);
			}
		}
	}
	else if (strcmp(pcszFilter, "auction") == 0)
	{
		for (KMailTable::iterator it = m_MailTable.begin(); it != m_MailTable.end(); ++it)
		{
			KMailInfo*  pMailInfo   = it->second;
			if (pMailInfo && pMailInfo->byType == eMailType_Auction)
			{
				Lua_PushNumber(L, ++nCount);
				Lua_PushNumber(L, (int)pMailInfo->dwMailID);
				Lua_SetTable(L, -3);
			}
		}
	}
	else if (strcmp(pcszFilter, "system") == 0)
	{
		for (KMailTable::iterator it = m_MailTable.begin(); it != m_MailTable.end(); ++it)
		{
			KMailInfo*  pMailInfo   = it->second;
			if (pMailInfo && pMailInfo->byType == eMailType_System)
			{
				Lua_PushNumber(L, ++nCount);
				Lua_PushNumber(L, (int)pMailInfo->dwMailID);
				Lua_SetTable(L, -3);
			}
		}
	}
	else if (strcmp(pcszFilter, "attachments") == 0)
	{
		for (KMailTable::iterator it = m_MailTable.begin(); it != m_MailTable.end(); ++it)
		{
			KMailInfo*  pMailInfo   = it->second;
			if (pMailInfo && pMailInfo->byType != eMailType_GmMessage && 
				pMailInfo->byType != eMailType_PlayerMessage &&
				(pMailInfo->bItemFlag || pMailInfo->bMoneyFlag))
			{
				Lua_PushNumber(L, ++nCount);
				Lua_PushNumber(L, (int)pMailInfo->dwMailID);
				Lua_SetTable(L, -3);
			}
		}
	}
	else if (strcmp(pcszFilter, "empty") == 0)
	{
		for (KMailTable::iterator it = m_MailTable.begin(); it != m_MailTable.end(); ++it)
		{
			KMailInfo*  pMailInfo   = it->second;
			if (pMailInfo && pMailInfo->byType != eMailType_GmMessage && 
				pMailInfo->byType != eMailType_PlayerMessage &&
				!pMailInfo->bItemFlag && !pMailInfo->bMoneyFlag)
			{
				Lua_PushNumber(L, ++nCount);
				Lua_PushNumber(L, (int)pMailInfo->dwMailID);
				Lua_SetTable(L, -3);
			}
		}
	}
	else if (strcmp(pcszFilter, "read") == 0)
	{
		for (KMailTable::iterator it = m_MailTable.begin(); it != m_MailTable.end(); ++it)
		{
			KMailInfo*  pMailInfo   = it->second;
			if (pMailInfo && pMailInfo->byType != eMailType_GmMessage && 
				pMailInfo->byType != eMailType_PlayerMessage && pMailInfo->bReadFlag)
			{
				Lua_PushNumber(L, ++nCount);
				Lua_PushNumber(L, (int)pMailInfo->dwMailID);
				Lua_SetTable(L, -3);
			}
		}
	}
	else if (strcmp(pcszFilter, "unread") == 0)
	{
		for (KMailTable::iterator it = m_MailTable.begin(); it != m_MailTable.end(); ++it)
		{
			KMailInfo*  pMailInfo   = it->second;
			if (pMailInfo && pMailInfo->byType != eMailType_GmMessage && 
				pMailInfo->byType != eMailType_PlayerMessage && !pMailInfo->bReadFlag)
			{
				Lua_PushNumber(L, ++nCount);
				Lua_PushNumber(L, (int)pMailInfo->dwMailID);
				Lua_SetTable(L, -3);
			}
		}
	}
	else
	{
		for (KMailTable::iterator it = m_MailTable.begin(); it != m_MailTable.end(); ++it)
		{
			KMailInfo*  pMailInfo   = it->second;
			if (pMailInfo && pMailInfo->byType != eMailType_GmMessage && pMailInfo->byType != eMailType_PlayerMessage)
			{
				Lua_PushNumber(L, ++nCount);
				Lua_PushNumber(L, (int)pMailInfo->dwMailID);
				Lua_SetTable(L, -3);
			}
		}
	}


    nResult = 1;
Exit0:
    return nResult;
}

int KMailClient::LuaGetMailInfo(Lua_State* L)
{
	int                     nResult     = 0;
    int                     nRetCode    = 0;
    DWORD                   dwNpcID     = 0;
    DWORD                   dwMailID    = 0;
    KMailInfo*              pMailInfo   = NULL;
    KMailTable::iterator    it;

    nRetCode = Lua_GetTopIndex(L);
    KGLOG_PROCESS_ERROR(nRetCode == 1);

    dwMailID = (DWORD)Lua_ValueToNumber(L, 1);
    KGLOG_PROCESS_ERROR(dwMailID);

    it = m_MailTable.find(dwMailID);
    KGLOG_PROCESS_ERROR(it != m_MailTable.end());

    pMailInfo = it->second;
    KGLOG_PROCESS_ERROR(pMailInfo);

	pMailInfo->LuaGetObj(L);

    nResult = 1;
Exit0:
    return nResult;
}

int KMailClient::LuaReturnMail(Lua_State* L)
{
    int                     nRetCode    = 0;
    DWORD                   dwMailID    = 0;
    KMailInfo*              pMailInfo   = NULL;
    KMailTable::iterator    it;

    nRetCode = Lua_GetTopIndex(L);
    KGLOG_PROCESS_ERROR(nRetCode == 1);

    dwMailID = (DWORD)Lua_ValueToNumber(L, 1);
    KGLOG_PROCESS_ERROR(dwMailID);

    it = m_MailTable.find(dwMailID);
    KGLOG_PROCESS_ERROR(it != m_MailTable.end());

    pMailInfo = it->second;
    assert(pMailInfo);

    KGLOG_PROCESS_ERROR(pMailInfo->byType == eMailType_Player);

    for (int i = 0; i < KMAIL_MAX_ITEM_COUNT; i++)
    {
        if (pMailInfo->dwItemID[i] == ERROR_ID)
            continue;

        KItem* pItem = g_pSO3World->m_ItemSet.GetObj(pMailInfo->dwItemID[i]);

        if (pItem)
        {
            g_pSO3World->m_ItemManager.FreeItem(pItem);
            pItem = NULL;
        }
    }

    delete pMailInfo;
    m_MailTable.erase(it);

    nRetCode = g_PlayerClient.DoReturnMail(dwMailID);
    KGLOG_PROCESS_ERROR(nRetCode);

Exit0:
    return 0;
}

int KMailClient::LuaGetGmMessage(Lua_State* L)
{
    int         nResult     = 0;
    int         nTopIndex   = 0;
    DWORD       dwMailID    = 0;
    KMailInfo*  pMailInfo   = NULL;
    KMailTable::iterator it;

    nTopIndex = Lua_GetTopIndex(L);
    KGLOG_PROCESS_ERROR(nTopIndex == 1);

    dwMailID = (DWORD)Lua_ValueToNumber(L, 1);
    KGLOG_PROCESS_ERROR(dwMailID);

    it = m_MailTable.find(dwMailID);
    KGLOG_PROCESS_ERROR(it != m_MailTable.end());

    pMailInfo = it->second;
    KGLOG_PROCESS_ERROR(pMailInfo);
    KGLOG_PROCESS_ERROR(pMailInfo->byType == eMailType_GmMessage);
    KGLOG_PROCESS_ERROR(pMailInfo->bGotContentFlag);

    Lua_PushString(L, pMailInfo->szSenderName);
    Lua_PushString(L, pMailInfo->szTitle);
    Lua_PushNumber(L, pMailInfo->nSendTime);
    Lua_PushString(L, (char*)pMailInfo->byText);

    nResult = 4;
Exit0:
    return nResult;
}

int KMailClient::LuaGetPlayerMessage(Lua_State* L)
{
    int         nResult     = 0;
    int         nTopIndex   = 0;
    int         nIndex      = 0;
    DWORD       dwMailID    = 0;
    KMailInfo*  pMailInfo   = NULL;
    BYTE*       pbyMsgData  = NULL;
    BYTE*       pbyMsgEnd   = NULL;
    KMailTable::iterator it;

    nTopIndex = Lua_GetTopIndex(L);
    KGLOG_PROCESS_ERROR(nTopIndex == 1);

    dwMailID = (DWORD)Lua_ValueToNumber(L, 1);
    KGLOG_PROCESS_ERROR(dwMailID);

    it = m_MailTable.find(dwMailID);
    KGLOG_PROCESS_ERROR(it != m_MailTable.end());

    pMailInfo = it->second;
    KGLOG_PROCESS_ERROR(pMailInfo);
    KGLOG_PROCESS_ERROR(pMailInfo->byType == eMailType_PlayerMessage);
    KGLOG_PROCESS_ERROR(pMailInfo->bGotContentFlag);

    Lua_PushString(L, pMailInfo->szSenderName);
    Lua_PushString(L, pMailInfo->szTitle);
    Lua_PushNumber(L, pMailInfo->nSendTime);
    
    pbyMsgData = pMailInfo->byText;
    pbyMsgEnd  = pMailInfo->byText + pMailInfo->uTextLen;

    Lua_NewTable(L);
    nTopIndex = Lua_GetTopIndex(L);

    while (pbyMsgData < pbyMsgEnd)
    {
        BYTE byBlockType = (BYTE)*pbyMsgData;
        pbyMsgData += sizeof(BYTE);

        switch(byBlockType)
        {
        case tdbText:
            {
                int    nTextTable = 0;
                int    nRetCode   = 0;
                size_t uTextLen   = 0;
                char*  pszText    = (char*)pbyMsgData;
                KGLOG_PROCESS_ERROR(pszText);

                uTextLen = strlen(pszText);

                nRetCode = g_pSO3World->m_piTextFilter->Replace(pszText);
                KGLOG_PROCESS_ERROR(nRetCode);

                Lua_PushNumber(L, ++nIndex);
                Lua_NewTable(L);
                nTextTable = Lua_GetTopIndex(L);

                Lua_PushString(L, "type");
                Lua_PushString(L, "text");
                Lua_SetTable(L, nTextTable);

                Lua_PushString(L, "text");
                Lua_PushString(L, pszText);
                Lua_SetTable(L, nTextTable);

                Lua_SetTable(L, nTopIndex);

                pbyMsgData += (uTextLen + sizeof('\0'));
            }
            break;

        case tdbLinkName:
            {
                int    nLinkNameTable = 0;
                size_t uNameLen       = 0;
                char*  pszName        = (char*)pbyMsgData;
                KGLOG_PROCESS_ERROR(pszName);

                uNameLen = strlen(pszName);

                Lua_PushNumber(L, ++nIndex);
                Lua_NewTable(L);
                nLinkNameTable = Lua_GetTopIndex(L);

                Lua_PushString(L, "type");
                Lua_PushString(L, "name");
                Lua_SetTable(L, nLinkNameTable);

                Lua_PushString(L, "name");
                Lua_PushString(L, pszName);
                Lua_SetTable(L, nLinkNameTable);

                Lua_SetTable(L, nTopIndex);

                pbyMsgData += (uNameLen + sizeof('\0'));
            }
            break;

        case tdbLinkItem:
            {
                int     nLinkItemTable  = 0;
                DWORD   dwItemID        = 0;
                int     nItemID         = 0;
                KItem*  pDestItem       = NULL;
                struct  TALK_SYNC_ITEM* pTalkSyncItem = (TALK_SYNC_ITEM*)pbyMsgData;

                pDestItem = g_pSO3World->m_ItemManager.GenerateItemFromBinaryData(ERROR_ID, pTalkSyncItem->byData, pTalkSyncItem->uDataSize);
                KGLOG_PROCESS_ERROR(pDestItem);

                dwItemID = pDestItem->m_dwID;

                //If queue is full, then delete the first one.
                if (g_pSO3World->m_LinkItemQue.size() >= CLIENT_TALK_LINK_ITEM_BUFFER_SIZE)
                {
                    KItem* pDeleteItem = g_pSO3World->m_LinkItemQue.front();
                    if (pDeleteItem)
                    {
                        pDeleteItem->UnInit();
                        g_pSO3World->m_ItemSet.Unregister(pDeleteItem);
                        KMemory::Delete(pDeleteItem);
                        pDeleteItem = NULL;
                    }
                    g_pSO3World->m_LinkItemQue.pop_front();
                }

                g_pSO3World->m_LinkItemQue.push_back(pDestItem);
                pDestItem = NULL;

                Lua_PushNumber(L, ++nIndex);
                Lua_NewTable(L);
                nLinkItemTable = Lua_GetTopIndex(L);

                Lua_PushString(L, "type");
                Lua_PushString(L, "item");
                Lua_SetTable(L, nLinkItemTable);

                nItemID = (int)dwItemID;
                Lua_PushString(L, "item");
                Lua_PushNumber(L, nItemID);
                Lua_SetTable(L, nLinkItemTable);

                Lua_SetTable(L, nTopIndex);

                pbyMsgData += (sizeof(TALK_SYNC_ITEM) + pTalkSyncItem->uDataSize);
            }
            break;

        case tdbLinkItemInfo:
            {
                int		                nLinkItemInfoTable	= 0;
                TALK_SYNC_ITEM_INFO*    pTalkSyncItemInfo   = (TALK_SYNC_ITEM_INFO*)pbyMsgData;

                Lua_PushNumber(L, ++nIndex);
                Lua_NewTable(L);
                nLinkItemInfoTable = Lua_GetTopIndex(L);

                Lua_PushString(L, "type");
                Lua_PushString(L, "iteminfo");
                Lua_SetTable(L, nLinkItemInfoTable);

                Lua_PushString(L, "version");
                Lua_PushNumber(L, pTalkSyncItemInfo->nVersion);
                Lua_SetTable(L, nLinkItemInfoTable);

                Lua_PushString(L, "tabtype");
                Lua_PushNumber(L, pTalkSyncItemInfo->dwTabType);
                Lua_SetTable(L, nLinkItemInfoTable);

                Lua_PushString(L, "index");
                Lua_PushNumber(L, pTalkSyncItemInfo->dwIndex);
                Lua_SetTable(L, nLinkItemInfoTable);

                Lua_SetTable(L, nTopIndex);

                pbyMsgData += sizeof(TALK_SYNC_ITEM_INFO);
            }
            break;

        case tdbLinkQuest:
            {
                int		            nLinkQuestTable	= 0;
                TALK_SYNC_QUEST*    pTalkSyncQuest  = (TALK_SYNC_QUEST*)pbyMsgData;

                Lua_PushNumber(L, ++nIndex);
                Lua_NewTable(L);
                nLinkQuestTable = Lua_GetTopIndex(L);

                Lua_PushString(L, "type");
                Lua_PushString(L, "quest");
                Lua_SetTable(L, nLinkQuestTable);

                Lua_PushString(L, "questid");
                Lua_PushNumber(L, pTalkSyncQuest->dwQuestID);
                Lua_SetTable(L, nLinkQuestTable);

                Lua_SetTable(L, nTopIndex);

                pbyMsgData += sizeof(TALK_SYNC_QUEST);
            }
            break;

        case tdbLinkRecipe:
            {
                int		            nLinkRecipeTable    = 0;
                TALK_SYNC_RECIPE*   pTalkSyncRecipe     = (TALK_SYNC_RECIPE*)pbyMsgData;

                Lua_PushNumber(L, ++nIndex);
                Lua_NewTable(L);
                nLinkRecipeTable = Lua_GetTopIndex(L);

                Lua_PushString(L, "type");
                Lua_PushString(L, "recipe");
                Lua_SetTable(L, nLinkRecipeTable);

                Lua_PushString(L, "craftid");
                Lua_PushNumber(L, pTalkSyncRecipe->dwCraftID);
                Lua_SetTable(L, nLinkRecipeTable);

                Lua_PushString(L, "recipeid");
                Lua_PushNumber(L, pTalkSyncRecipe->dwRecipeID);
                Lua_SetTable(L, nLinkRecipeTable);

                Lua_SetTable(L, nTopIndex);

                pbyMsgData += sizeof(TALK_SYNC_RECIPE);
            }
            break;

        case tdbLinkEnchant:
            {
                int		            nLinkEnchantTable	= 0;
                TALK_SYNC_ENCHANT*  pTalkSyncEnchant    = (TALK_SYNC_ENCHANT*)pbyMsgData;

                Lua_PushNumber(L, ++nIndex);
                Lua_NewTable(L);
                nLinkEnchantTable = Lua_GetTopIndex(L);

                Lua_PushString(L, "type");
                Lua_PushString(L, "enchant");
                Lua_SetTable(L, nLinkEnchantTable);

                Lua_PushString(L, "proid");
                Lua_PushNumber(L, pTalkSyncEnchant->dwProID);
                Lua_SetTable(L, nLinkEnchantTable);

                Lua_PushString(L, "craftid");
                Lua_PushNumber(L, pTalkSyncEnchant->dwCraftID);
                Lua_SetTable(L, nLinkEnchantTable);

                Lua_PushString(L, "recipeid");
                Lua_PushNumber(L, pTalkSyncEnchant->dwRecipeID);
                Lua_SetTable(L, nLinkEnchantTable);

                Lua_SetTable(L, nTopIndex);

                pbyMsgData += sizeof(TALK_SYNC_ENCHANT);
            }
            break;

        case tdbLinkSkill:
            {
                int		            nLinkSkillTable		= 0;
                KSKILL_RECIPE_KEY*  pSkillKey           = (KSKILL_RECIPE_KEY*)pbyMsgData;
                char                szRecipeKey[32];

                Lua_PushNumber(L, ++nIndex);
                Lua_NewTable(L);
                nLinkSkillTable = Lua_GetTopIndex(L);

                Lua_PushString(L, "type");
                Lua_PushString(L, "skill");
                Lua_SetTable(L, nLinkSkillTable);

                Lua_PushString(L, "skill_id");
                Lua_PushNumber(L, pSkillKey->dwSkillID);
                Lua_SetTable(L, nLinkSkillTable);

                Lua_PushString(L, "skill_level");
                Lua_PushNumber(L, pSkillKey->dwSkillLevel);
                Lua_SetTable(L, nLinkSkillTable);

                for (int i = 0; i < MAX_SKILL_REICPE_COUNT; i++)
                {
                    snprintf(szRecipeKey, sizeof(szRecipeKey), "recipe%d", i + 1);
                    szRecipeKey[sizeof(szRecipeKey) - 1] = 0;

                    Lua_PushString(L, szRecipeKey);
                    Lua_PushNumber(L, pSkillKey->dwRecipeKey[i]);
                    Lua_SetTable(L, nLinkSkillTable);
                }

                Lua_SetTable(L, nTopIndex);

                pbyMsgData += sizeof(KSKILL_RECIPE_KEY);
            }
            break;

        case tdbLinkSkillRecipe:
            {
                int		                nLinkSkillRecipeTable   = 0;
                TALK_SYNC_SKILL_RECIPE* pTalkSyncSkillRecipe    = (TALK_SYNC_SKILL_RECIPE*)pbyMsgData;

                Lua_PushNumber(L, ++nIndex);
                Lua_NewTable(L);
                nLinkSkillRecipeTable = Lua_GetTopIndex(L);

                Lua_PushString(L, "type");
                Lua_PushString(L, "skillrecipe");
                Lua_SetTable(L, nLinkSkillRecipeTable);

                Lua_PushString(L, "id");
                Lua_PushNumber(L, pTalkSyncSkillRecipe->dwID);
                Lua_SetTable(L, nLinkSkillRecipeTable);

                Lua_PushString(L, "level");
                Lua_PushNumber(L, pTalkSyncSkillRecipe->dwLevel);
                Lua_SetTable(L, nLinkSkillRecipeTable);

                Lua_SetTable(L, nTopIndex);

                pbyMsgData += sizeof(TALK_SYNC_SKILL_RECIPE);
            }
            break;
        case tdbEmotion:
            {
                Lua_PushNumber(L, ++nIndex);
                Lua_NewTable(L);

                Lua_PushString(L, "type");
                Lua_PushString(L, "emotion");
                Lua_SetTable(L, -3);

                Lua_SetTable(L, nTopIndex);
            }
            break;
        case tdbLinkBook:
            {
                int nBookTable = 0;
                TALK_SYNC_BOOK *pTalkSyncBook = (TALK_SYNC_BOOK*)pbyMsgData;

                Lua_PushNumber(L, ++nIndex);
                Lua_NewTable(L);
                nBookTable =  Lua_GetTopIndex(L);

                Lua_PushString(L, "type");
                Lua_PushString(L, "book");
                Lua_SetTable(L, nBookTable);

                Lua_PushString(L, "version");
                Lua_PushNumber(L, pTalkSyncBook->ItemInfo.nVersion);
                Lua_SetTable(L, nBookTable);

                Lua_PushString(L, "tabtype");
                Lua_PushNumber(L, pTalkSyncBook->ItemInfo.dwTabType);
                Lua_SetTable(L, nBookTable);

                Lua_PushString(L, "index");
                Lua_PushNumber(L, pTalkSyncBook->ItemInfo.dwIndex);
                Lua_SetTable(L, nBookTable);

                Lua_PushString(L, "bookinfo");
                Lua_PushNumber(L,  pTalkSyncBook->nBookRecipeID);
                Lua_SetTable(L, nBookTable);

                Lua_SetTable(L, nTopIndex);

                pbyMsgData += sizeof(TALK_SYNC_BOOK);
            }
            break;
        case tdbLinkAchievement:
            {
                int nAchievementTable = 0;
                TALK_SYNC_ACHIEVEMENT *pTalkSyncAchievement = (TALK_SYNC_ACHIEVEMENT*)pbyMsgData;

                Lua_PushNumber(L, ++nIndex);
                Lua_NewTable(L);
                nAchievementTable =  Lua_GetTopIndex(L);

                Lua_PushString(L, "type");
                Lua_PushString(L, "achievement");
                Lua_SetTable(L, nAchievementTable);

                Lua_PushString(L, "id");
                Lua_PushNumber(L, pTalkSyncAchievement->dwAchievementID);
                Lua_SetTable(L, nAchievementTable);

                Lua_SetTable(L, nTopIndex);
            
                pbyMsgData += sizeof(TALK_SYNC_ACHIEVEMENT);
            }
            break;
        case tdbLinkDesignation:
            {
                TALK_SYNC_DESIGNATION * pTalkSyncDesignation = (TALK_SYNC_DESIGNATION*)pbyMsgData;

                Lua_PushNumber(L, ++nIndex);
                Lua_NewTable(L);

                Lua_PushString(L, "type");
                Lua_PushString(L, "designation");
                Lua_SetTable(L, -3);

                Lua_PushString(L, "id");
                Lua_PushNumber(L, pTalkSyncDesignation->dwDesignationID);
                Lua_SetTable(L, -3);

                Lua_PushString(L, "prefix");
                Lua_PushBoolean(L, pTalkSyncDesignation->dwPrefix);
                Lua_SetTable(L, -3);

                Lua_SetTable(L, nTopIndex);

                pbyMsgData += sizeof(TALK_SYNC_DESIGNATION);
            }
            break;
        case tdbEventLink:
            {
               int    nLinkTable = 0;
               size_t uNameLen   = 0;
               size_t uLinkLen   = 0;
               size_t uTotalLen  = 0;
               char   *pszName   = NULL;
               char   *pszLink   = NULL; 

               pszName = (char *)pbyMsgData;
               KGLOG_PROCESS_ERROR(pszName);
               uNameLen = strlen(pszName);
               uTotalLen += (uNameLen + sizeof('\0'));
                
               pszLink = (char *)(pbyMsgData + uTotalLen);
               KGLOG_PROCESS_ERROR(pszLink);
               uLinkLen = strlen(pszLink);
               uTotalLen += (uLinkLen + sizeof('\0'));

               Lua_PushNumber(L, ++nIndex);
               Lua_NewTable(L);

               nLinkTable = Lua_GetTopIndex(L);

               Lua_PushString(L, "type");
               Lua_PushString(L, "eventlink");
               Lua_SetTable(L, nLinkTable);

               Lua_PushString(L, "name");
               Lua_PushString(L, pszName);
               Lua_SetTable(L, nLinkTable);

               Lua_PushString(L, "linkinfo");
               Lua_PushString(L, pszLink);
               Lua_SetTable(L, nLinkTable);
               
               Lua_SetTable(L, nTopIndex);
               pbyMsgData += uTotalLen;
            }
            break;
        default:
            KGLogPrintf(KGLOG_ERR, "Unexpected talk data: %d\n", byBlockType);
            goto Exit0;
        }
    }

    nResult = 4;
Exit0:
    return nResult;
}

int KMailInfo::LuaGetLeftTime(Lua_State* L)
{
    Lua_PushNumber(L, GetLeftSecond(g_pSO3World->m_Settings.m_ConstList.nMailSurvivalTime));
	return 1;
}


int KMailInfo::LuaGetType(Lua_State* L)
{
    Lua_PushNumber(L, (int)byType);
	return 1;
}

int KMailInfo::LuaRequestContent(Lua_State* L)
{
    int                     nRetCode    = 0;
    DWORD                   dwNpcID     = 0;
    KPlayer*                pPlayer     = NULL;
    KNpc*                   pNpc        = NULL;

    nRetCode = Lua_GetTopIndex(L);
    KGLOG_PROCESS_ERROR(nRetCode == 1);

    dwNpcID  = (DWORD)Lua_ValueToNumber(L, 1);
    KGLOG_PROCESS_ERROR(dwNpcID);

    pPlayer = g_pSO3World->m_PlayerSet.GetObj(g_pSO3World->m_dwClientPlayerID);
    KGLOG_PROCESS_ERROR(pPlayer);

    pNpc    = g_pSO3World->m_NpcSet.GetObj(dwNpcID);
    KGLOG_PROCESS_ERROR(pNpc);

    nRetCode = g_InRange(pPlayer, pNpc, COMMON_PLAYER_OPERATION_DISTANCE);
    if (!nRetCode)
    {
        if (g_pGameWorldUIHandler)
        {
            KUIEventMailRespond MailRespond = { (int)mrcTooFarAway };
            g_pGameWorldUIHandler->OnMailRespond(MailRespond);
        }

        goto Exit0;
    }

    if (!bGotContentFlag)
        g_PlayerClient.DoQueryMailContent(dwNpcID, dwMailID, byType);

Exit0:
	return 0;
}

int KMailInfo::LuaGetText(Lua_State* L)
{
	if (bGotContentFlag)
		Lua_PushString(L, (char*)byText);
	else
		Lua_PushString(L, "");
	return 1;
}

int KMailInfo::LuaGetItem(Lua_State* L)
{
    int                     nRetCode    = 0;
    int						nIndex		= 0;
	KItem*					pItem		= NULL;

    nRetCode = Lua_GetTopIndex(L);
    KGLOG_PROCESS_ERROR(nRetCode == 1);

    nIndex  = (int)Lua_ValueToNumber(L, 1);
    KGLOG_PROCESS_ERROR(nIndex >= 0 && nIndex < KMAIL_MAX_ITEM_COUNT);

	KG_PROCESS_ERROR(bGotContentFlag);
	KG_PROCESS_ERROR(dwItemID[nIndex] != ERROR_ID);

	pItem = g_pSO3World->m_ItemSet.GetObj(dwItemID[nIndex]);
	KGLOG_PROCESS_ERROR(pItem);

Exit0:
	if (pItem)
	{
		pItem->LuaGetObj(L);
		Lua_PushNumber(L, nItemPrice[nIndex]);
	}
	else
	{
		Lua_PushNil(L);
		Lua_PushNumber(L, 0);
	}
	return 2;
}

int KMailInfo::LuaTakeMoney(Lua_State* L)
{
	if(bGotContentFlag && nMoney > 0)
		g_PlayerClient.DoAcquireMailMoneyRequest(dwMailID);
	return 0;
}

int KMailInfo::LuaTakeItem(Lua_State* L)
{
	int nIndex = 0;
    int nRetCode = Lua_GetTopIndex(L);
    KGLOG_PROCESS_ERROR(nRetCode == 1);

    nIndex  = (int)Lua_ValueToNumber(L, 1);
    KGLOG_PROCESS_ERROR(nIndex >= 0 && nIndex < KMAIL_MAX_ITEM_COUNT);

	if(bGotContentFlag && dwItemID[nIndex] != ERROR_ID)
		g_PlayerClient.DoAcquireMailItemRequest(dwMailID, nIndex);

Exit0:
	return 0;
}

int KMailInfo::LuaRead(Lua_State* L)
{
	if (!bReadFlag)
	{
		bReadFlag = true;
		g_PlayerClient.DoSetMailRead(dwMailID);
	}
	return 0;
}

int KMailInfo::LuaGetSendTime(Lua_State* L)
{
	Lua_PushNumber(L, nSendTime);
	return 1;
}

int KMailInfo::LuaGetRecvTime(Lua_State* L)
{
	Lua_PushNumber(L, nRecvTime);
	return 1;
}

DEFINE_LUA_CLASS_BEGIN(KMailClient)

    REGISTER_LUA_FUNC(KMailClient, SendMail)
    REGISTER_LUA_FUNC(KMailClient, DeleteMail)
    REGISTER_LUA_FUNC(KMailClient, CountMail)
    REGISTER_LUA_FUNC(KMailClient, ApplyMailList)
    REGISTER_LUA_FUNC(KMailClient, GetMailList)
    REGISTER_LUA_FUNC(KMailClient, ReturnMail)
    REGISTER_LUA_FUNC(KMailClient, GetGmMessage)
    REGISTER_LUA_FUNC(KMailClient, GetPlayerMessage)
	REGISTER_LUA_FUNC(KMailClient, GetMailInfo)

DEFINE_LUA_CLASS_END(KMailClient)

DEFINE_LUA_CLASS_BEGIN(KMailInfo)

	REGISTER_LUA_DWORD_READONLY(KMailInfo, MailID)

	REGISTER_LUA_STRING_READONLY(KMailInfo, SenderName)
	REGISTER_LUA_STRING_READONLY(KMailInfo, Title)

	REGISTER_LUA_BOOL_READONLY(KMailInfo, ReadFlag)
	REGISTER_LUA_BOOL_READONLY(KMailInfo, MoneyFlag)
	REGISTER_LUA_BOOL_READONLY(KMailInfo, ItemFlag)

	REGISTER_LUA_BOOL_READONLY(KMailInfo, GotContentFlag)

	REGISTER_LUA_INTEGER_READONLY(KMailInfo, Money)

	REGISTER_LUA_FUNC(KMailInfo, Read)
	REGISTER_LUA_FUNC(KMailInfo, GetLeftTime)
	REGISTER_LUA_FUNC(KMailInfo, GetType)
	REGISTER_LUA_FUNC(KMailInfo, RequestContent)
	REGISTER_LUA_FUNC(KMailInfo, GetText)
	REGISTER_LUA_FUNC(KMailInfo, GetItem)
	REGISTER_LUA_FUNC(KMailInfo, TakeMoney)
	REGISTER_LUA_FUNC(KMailInfo, TakeItem)

DEFINE_LUA_CLASS_END(KMailInfo)

#endif // _CLIENT

