#include "KWin32.h"

#include "Windows.h"
#include "../../core/src/coreshell.h"
#include "../../core/src/GameDataDef.h"
#include "../../core/src/CoreObjGenreDef.h"
#include "Windows.h"
#include "KEngine.h"
#include "Elem/Wnds.h"
#include "Elem/MouseHover.h"
#include "UiCase/UiPlayerBar.h"
#include "UiCase/UiGame.h"
#include "UiCase/UiGetString.h"
#include "UiCase/UiStatus.h"
#include "UiCase/UiTrade.h"
#include "UiCase/UiSkills.h"
#include "UiCase/UiItem.h"
#include "UiCase/UiShop.h"
#include "UiCase/UiMsgSel.h"
#include "UiCase/UiMsgCentrePad.h"
#include "UiCase/UiInformation2.h"
#include "UiCase/UiSysMsgCentre.h"
#include "UiCase/UiTeamManage.h"
#include "UiCase/UiSelPlayerNearby.h"
#include "UiCase/UiChatCentre.h"
#include "UiCase/UiStoreBox.h"
#include "UiCase/UiTaskNote.h"
#include "UiCase/UiNewsMessage.h"
#include "UiCase/UiStrengthRank.h"
#include "UiCase/UiTongManager.h"
#include "UiCase/UiTongCreateSheet.h"
#include "UiCase/UiStall.h"
#include "UiCase/uiminimap.h"
#include "UiCase/UiTrembleItem.h"
#include "UiCase/UiKillerAllTask.h"
#include "UiCase/UiKillerOwnTask.h"
#include "UiCase/UiKillerCreateTask.h"
#include "UiCase/UiStallSetAdv.h"
#include "UiChatPhrase.h"
#include "../Login/Login.h"
#include "UiCase/UiParadeItem.h"
#include "UiCase/UiDiceWeaveBingo.h"
#include "UiCase/UiCityManager.h"
#include "KTongProtocol.h"
#include "../Plugin/SwordPluginCentre.h"
#include <time.h>
#include "malloc.h"
#include "../S3Client.h"
#include "UiShell.h"
#include "UiBase.h"
#include "../../Engine/Src/Text.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

////////////////////////////////////////////////////////////////////////////////

bool UiCloseWndsInGame(bool bAll);

extern iCoreShell*		g_pCoreShell;

void GameWorldTips(unsigned int uParam, int nParam);

//--------------------------------------------------------------------------
//	功能：接受游戏世界数据改变通知的函数
//--------------------------------------------------------------------------
void CoreDataChangedCallback(unsigned int uDataId, unsigned int uParam, int nParam)
{
	KUiTrade* pTradeBar = NULL;
	switch(uDataId)
	{
	case GDCNI_HOLD_OBJECT:
		Wnd_DragFinished();
		if (uParam && ((KUiObjAtRegion*)uParam)->Obj.uGenre != CGOG_NOTHING)
		{
			KUiDraggedObject Obj;
			Obj.uGenre = ((KUiObjAtRegion*)uParam)->Obj.uGenre;
			Obj.uId = ((KUiObjAtRegion*)uParam)->Obj.uId;
			Obj.DataW = ((KUiObjAtRegion*)uParam)->Region.Width;
			Obj.DataH = ((KUiObjAtRegion*)uParam)->Region.Height;
			Wnd_DragBegin(&Obj, DrawDraggingGameObjFunc);
		}
		break;
	case GDCNI_PLAYER_BASE_INFO:	//主角的一些不易变的数据
		{
			KUiStatus* pBar = KUiStatus::GetIfVisible();
			if (pBar)
				pBar->UpdateBaseData();

			KUiPlayerBaseInfo	Info;
			memset(&Info, 0, sizeof(KUiPlayerBaseInfo));
			g_pCoreShell->GetGameData(GDI_PLAYER_BASE_INFO, (int)&Info, 0);
			if (Info.nCurFaction >= 0)
				KUiMsgCentrePad::QueryAllChannel();
			else
			{
				KUiMsgCentrePad::CloseSelfChannel(KUiMsgCentrePad::ch_Faction);
				if (Info.nCurTong || Info.uTongUnionId)
				{
					KUiMsgCentrePad::QueryAllChannel();
				}
				if (Info.nCurTong == 0)
					KUiMsgCentrePad::CloseSelfChannel(KUiMsgCentrePad::ch_Tong);
				if (Info.uTongUnionId == 0)
					KUiMsgCentrePad::CloseSelfChannel(KUiMsgCentrePad::ch_Union);
			}
		}
		break;
	case GDCNI_PLAYER_RT_ATTRIBUTE:
		{
			KUiStatus* pBar = KUiStatus::GetIfVisible();
			if (pBar)
				pBar->UpdateData();
		}
		break;
	case GDCNI_PLAYER_IMMED_ITEMSKILL://主角的立即使用物品与武功
		if (uParam)
		{
			KUiGameObject* pObj = (KUiGameObject*)uParam;
			KUiPlayerBar* pBar = KUiPlayerBar::GetIfVisible();
			if (pBar)
			{
				if (nParam >= 0)
					pBar->UpdateItem(nParam, pObj->uGenre, pObj->uId);
				else
					pBar->UpdateSkill(-nParam - 1, pObj->uGenre, pObj->uId);
			}
		}
		break;
	case GDCNI_OBJECT_CHANGED:
		if (uParam)
		{
			KUiObjAtContRegion* pObject = (KUiObjAtContRegion*)uParam;
			if (pObject->eContainer == UOC_ITEM_TAKE_WITH)
			{
				pTradeBar = KUiTrade::GetIfVisible();
				if (pTradeBar)
				{
					pTradeBar->OnChangedTakewithItem((KUiObjAtRegion*)uParam, nParam);
				}
				else
				{
					KUiItem* pItemsBar = KUiItem::GetIfVisible();
					if (pItemsBar)
						pItemsBar->UpdateItem((KUiObjAtRegion*)uParam, nParam);
				}
			}
			else if (pObject->eContainer == UOC_EQUIPTMENT)
			{
				KUiStatus* pEquips = KUiStatus::GetIfVisible();
				if (pEquips)
					pEquips->UpdateEquip((KUiObjAtRegion*)uParam, nParam);
			}
			else if (pObject->eContainer == UOC_STORE_BOX)
			{
				KUiStoreBox* pStoreBox = KUiStoreBox::GetIfVisible();
				if (pStoreBox)
					pStoreBox->UpdateItem((KUiObjAtRegion*)uParam, nParam);
			}
			else if (pObject->eContainer == UOC_TO_BE_TRADE)
			{
				KUiTrade* pTrade = KUiTrade::GetIfVisible();
				if (pTrade)
					pTrade->OnSelfChangedItem(pObject, nParam);
			}
			else if (pObject->eContainer == UOC_IN_HAND)
			{
				Wnd_DragFinished();
				if (pObject->Obj.uGenre != CGOG_NOTHING)
				{
					KUiDraggedObject Obj;
					Obj.uGenre = pObject->Obj.uGenre;
					Obj.uId = pObject->Obj.uId;
					Obj.DataW = pObject->Region.Width;
					Obj.DataH = pObject->Region.Height;
					Wnd_DragBegin(&Obj, DrawDraggingGameObjFunc);
				}
			}
			else if (pObject->eContainer == UOC_IMMEDIA_ITEM)
			{
				KUiPlayerBar* pBar = KUiPlayerBar::GetIfVisible();
				if (pBar)
				{
					if (nParam)
						pBar->UpdateItem(pObject->Region.h, pObject->Obj.uGenre, pObject->Obj.uId);
					else
						pBar->UpdateItem(pObject->Region.h, CGOG_NOTHING, 0);
				}
			}
			else if (pObject->eContainer == UOC_IMMEDIA_SKILL)
			{
				KUiPlayerBar* pBar = KUiPlayerBar::GetIfVisible();
				if (pBar)
				{
					if (nParam)
						pBar->UpdateSkill(pObject->Region.h, pObject->Obj.uGenre, pObject->Obj.uId);
					else
						pBar->UpdateSkill(pObject->Region.h, CGOG_NOTHING, 0);
				}
			}
			else if(pObject->eContainer == UOC_SMITH)
			{
				KUiTrembleItem* pBar = KUiTrembleItem::GetIfVisible();
				if (pBar)
				{
					pBar->UpdateView((KUiObjAtRegion*)uParam, nParam);
				}
			}
		}
		break;
	case GDCNI_LIVE_SKILL_BASE:		//生活技能数值变化
		{
			KUiSkills* pPad = KUiSkills::GetIfVisible();
			if (pPad)
				pPad->UpdateLiveBaseData();
		}
		break;
	case GDCNI_FIGHT_SKILL_POINT:	//剩余战斗技能点数变化
		{
			KUiSkills* pPad = KUiSkills::GetIfVisible();
			if (pPad)
			{
				pPad->UpdateFightRemainPoint(nParam);
			}
		}
		break;
	case GDCNI_SKILL_CHANGE:			//新掌握了一个技能
		if (uParam)
		{
			KUiSkills::UpdateSkill((KUiSkillData*)uParam, nParam);
		}
		break;
/*	case GDCNI_PLAYER_LEADERSHIP:	//主角统帅能力相关的数据发生变化
		{
			KUiManage* pBar = KUiManage::GetIfVisible();
			if (pBar)
				pBar->UpdateLeaderData();
		}
		break;
*/
	case GDCNI_TRADE_START:
		if (uParam)
		{
			UiCloseWndsInGame(false);
			KUiTrade::OpenWindow((KUiPlayerItem*)uParam);
		}
		break;
	case GDCNI_TRADE_DESIRE_ITEM:	//对方增减想交易的物品
		pTradeBar = KUiTrade::GetIfVisible();
		if (pTradeBar)
			pTradeBar->OnOppositeChangedItem((KUiObjAtRegion*)uParam, nParam);
		break;
	case GDCNI_TRADE_OPER_DATA:		//交易状态变化
		pTradeBar = KUiTrade::GetIfVisible();
		if (pTradeBar)
			pTradeBar->UpdateOperData();
		break;
	case GDCNI_TRADE_END:			//交易结束
		pTradeBar = KUiTrade::GetIfVisible();
		if (pTradeBar)
			pTradeBar->UpdateTradeEnd(nParam);
		break;
	case GDCNI_NPC_TRADE:
		if (nParam)
			KUiShop::OpenWindow();
		else
			KUiShop::CloseWindow();
		break;
	case GDCNI_NPC_TRADE_ITEM:
		{
			KUiShop* pShop = KUiShop::GetIfVisible();
			if (pShop)
				pShop->UpdateData();
		}
		break;
	case GDCNI_QUESTION_CHOOSE:
		KUiMsgSel::OpenWindow((KUiQuestionAndAnswer*)uParam);
		break;
	case GDCNI_GAME_START:
		{
			g_LoginLogic.NotifyToStartGame();
			Wnd_GameSpaceHandleInput(true);
			KUiMsgCentrePad::ReleaseActivateChannelAll();
			KUiMsgCentrePad::QueryAllChannel();
			g_WndGameSpace.Trigger(KUiGameSpace::enumTT_GAME_START, 0, 0);
		}
		break;
	case GDCNI_SPEAK_WORDS:			//npc说话内容
	//uParam = (KUiInformationParam*) pWordDataList 指向KUiInformationParam数组
	//nParam = pWordDataList包含KUiInformationParam元素的数目
		if (uParam && nParam)
			g_UiInformation2.SpeakWords((KUiInformationParam*)uParam, nParam);
		break;
	case GDCNI_INFORMATION:
		if (uParam)
		{
			KUiInformationParam* pInformation = (KUiInformationParam*)uParam;
			KWndWindow*	pCaller = pInformation->bNeedConfirmNotify ? ((KWndWindow*)WND_GAMESPACE) : 0;
			UIMessageBox2(pInformation->sInformation, pInformation->nInforLen,
				pInformation->sConfirmText, pCaller, 0);
		}
		break;
	case GDCNI_CHAT_GROUP:
		KUiChatCentre::UpdateData(UICC_U_ALL, 0, 0);
		break;
	case GDCNI_CHAT_FRIEND:		//聊天好友发生变化
		KUiChatCentre::UpdateData(UICC_U_GROUP, 0, nParam);
		break;
	case GDCNI_CHAT_FRIEND_STATUS:	//聊天好友状态发生变化
		KUiChatCentre::UpdateData(UICC_U_FRIEND, uParam, nParam);
		break;
	case GDCNI_TEAM:
		{
			KUiTeamManage* pPad = KUiTeamManage::GetIfVisible();
			if (pPad)
				pPad->UpdateData((KUiPlayerTeam*)uParam);
			if (uParam)
				KUiMsgCentrePad::QueryAllChannel();
			else
				KUiMsgCentrePad::CloseSelfChannel(KUiMsgCentrePad::ch_Team);
		}
		break;
//	case GDCNI_TEAM_NEARBY_LIST:
//		KUiTeamManage::UpdateNearbyTeams((KUiTeamItem*)uParam, nParam);
//		break;
	case GDCNI_SWITCH_CURSOR:
		Wnd_SwitchCursor(nParam);
		break;
	case GDCNI_SYSTEM_MESSAGE:
		if (uParam)
			KUiSysMsgCentre::AMessageArrival((KSystemMessage*)uParam, (void*)nParam);
		break;
	case GDCNI_OPEN_STORE_BOX:
		KUiStoreBox::OpenWindow();
		KUiItem::OpenWindow();
		break;
	case GDCNI_SWITCHING_SCENEPLACE:
		if(!nParam)
		{
			if(g_pCoreShell)
			{
				KSceneMapInfo Info;
				KUiMiniMap::SetIsShowMapDetail(g_pCoreShell->SceneMapOperation(GSMOI_SCENE_MAP_INFO, (unsigned int)&Info, 0));
			}
			g_WndGameSpace.Trigger(KUiGameSpace::enumTT_SWITCH_SCENE_FINISH, uParam, 0);
		}
		break;
	// flying add this branch to deal with the task record from server.
	case GDCNI_MISSION_RECORD:
		if (uParam)
		{
			KMissionRecord* pRecord = (KMissionRecord*)uParam;
			KUiTaskNote::WakeUp(pRecord->sContent, pRecord->nContentLen, pRecord->uValue);
		}
		break;
	case GDCNI_PK_SETTING:
		break;
	case GDCNI_VIEW_PLAYERITEM:
		KUiParadeItem::OpenWindow((KUiPlayerItem*)uParam);
		break;
	case GDCNI_PLAYER_BRIEF_PROP:
		GameWorldTips(uParam, nParam);
		break;
	case GDCNI_NEWS_MESSAGE:		//新闻消息
		if (uParam)
			KUiNewsMessage::MessageArrival((KNewsMessage*)uParam, (SYSTEMTIME*)nParam);
		break;
	case GDCNII_RANK_INDEX_LIST_ARRIVE:
		KUiStrengthRank::OpenWindow();
		KUiStrengthRank::NewIndexArrive(uParam, (KRankIndex *)nParam);
		break;
	case GDCNII_RANK_INFORMATION_ARRIVE:
		KUiStrengthRank::NewRankArrive(uParam, (KRankMessage *)nParam);
		break;
	case GDCNI_TONG_INFO:
		KUiTongManager::TongInfoArrive((KUiPlayerRelationWithOther *)uParam, (KTongInfo *)nParam);
		break;
	case GDCNI_TONG_MEMBER_LIST:
		KUiTongManager::NewDataArrive((KUiGameObjectWithName *)uParam, (KTongMemberItem *)nParam);
		break;
	case GDCNI_TONG_ACTION_RESULT:
		KUiTongManager::ResponseResult((KUiGameObjectWithName *)uParam, nParam == 1);
		break;
	case GDCNI_OPEN_TONG_CREATE_SHEET:
		if(uParam)
		{
			KUiTongCreateSheet* pSheet = KUiTongCreateSheet::OpenWindow();
			if(pSheet)
				pSheet->SetUsage(nParam);
		}
		else
			KUiTongCreateSheet::CloseWindow();
		break;
	case GDCNI_UI_ACTION_NOTIFY:
		{
			switch(uParam)
			{
			case enumUA_CLEAR_BUY_QUEUE:
				{
					KUiShop *pShop = KUiShop::GetIfVisible();
					if(pShop)
					{
						pShop->ClearBuyList();
					}
				}
				break;
			case enumUA_BESET_UI_CLOSE:
				KUiTrembleItem::CloseWindow();
				break;
			case enumUA_BESET_CLEAR_ITEM:
				{
					KUiTrembleItem *pTremble = KUiTrembleItem::GetIfVisible();
					if(pTremble)
					{
						pTremble->Clear();
					}
				}
				break;
			case enumUA_OPEN_CITY_MANAGER:
				{
					//KUiCityManager *pCityBar = KUiCityManager::GetIfVisible();
					KUiCityManager *pCityBar = KUiCityManager::OpenWindow();
					if(pCityBar)
					{
						KUiGameObjectWithName *pCityParam = (KUiGameObjectWithName *)(nParam);
						pCityBar->UpdateCurrentCityData(pCityParam->szName, pCityParam->szString, pCityParam->nParam, pCityParam->uParam);
					}
				}
				break;
			}
		}
		break;
	case GDCNI_OPEN_BESET_BOX:
		KUiTrembleItem::CloseWindow();
		if(nParam)
		{
			KUiTrembleItem::OpenWindow();
		}
		break;
	case GDCNI_SWITCH_STALL:
		{
			KUiItem* pUiItemWnd = KUiItem::GetIfVisible();
			if (uParam)
			{
				if (pUiItemWnd == NULL)
					KUiItem::OpenWindow();
			}

			if (pUiItemWnd)
				pUiItemWnd->UpdateStallState(uParam);
		}
		break;
	case GDCNI_BUY_FROM_STALL:
		if(uParam)
		{
			KUiStall::OpenWindow();
			KUiItem::OpenWindow();
		}
		else
		{
			KUiStall::CloseWindow(true);
			KUiItem::CloseWindow(false);
		}
		break;
	case GDCNI_STALL_MISS_ITEM:
		if (uParam)
		{
			KUiStall* pUiStall = KUiStall::GetIfVisible();
			if (pUiStall)
			{
				pUiStall->RemoveItem((KUiObjAtRegion*)uParam);
			}
		}
		break;
	case GDCNI_STALL_INPUT_ADV:
		{
			int nStallState = g_pCoreShell->GetGameData(GDI_IS_STALL, -1, -1);
			if (nStallState == 0 && (!g_UiBase.GetStatus() || UIS_S_STALL_SETPRICE & g_UiBase.GetStatus()))
				KUiStallSetAdv::OpenWindow(true, true);
		}
		break;
	case GDCNI_CLOSE_STALL:
		if (KUiStall::GetIfVisible())
			KUiStall::CloseWindow(true);
		break;

	//动感色子老虎机上的大奖累计金额来了
	case GDCNI_DICE_WEAVE_BINGO_MONEY:
		{
			KUiDiceWeaveBingo *pDice = KUiDiceWeaveBingo::GetIfVisible();
			if(pDice)
			{
				pDice->UpdateUI(uParam);
			}
		}
		break;

	//动感色子老虎机的下注结果来了
	case GDCNI_DICE_WEAVE_DICE_NUMBER:
		{
			KUiDiceWeaveBingo *pDice = KUiDiceWeaveBingo::GetIfVisible();
			if(pDice)
			{
				pDice->DiceNumberIncome(uParam, nParam);
			}
		}
		break;
	case GDCNI_DICE_WEAVE_RESULT:
		{
			KUiDiceWeaveBingo *pDice = KUiDiceWeaveBingo::GetIfVisible();
			if(pDice)
			{
				pDice->ResultIncome(uParam, (char *)nParam);
			}
		}
		break;
	case GDCNI_DICE_WEAVE_OPEN:
		KUiDiceWeaveBingo::CloseWindow();
		if(uParam)
		{
			KUiDiceWeaveBingo::OpenWindow();
		}
		break;
	case GDCNI_KILLER_QUERY_TASK_RESULT:
		{
			KILLER_TASK* pTask = (KILLER_TASK*)uParam;
			KUiKillerAllTask* pAllTask = KUiKillerAllTask::GetIfVisible();
			KUiKillerOwnTask* pOwnTask = KUiKillerOwnTask::GetIfVisible();
			if (pTask->byPart == 0)
			{
				if (pTask->byType == 2)
				{
					if (NULL == pAllTask)
						pAllTask = KUiKillerAllTask::OpenWindow();
				}
				else if (pTask->byType == 0 || pTask->byType == 1)
				{
					if (NULL == pOwnTask)
						pOwnTask = KUiKillerOwnTask::OpenWindow();
				}
			}

			if (!pAllTask && pTask->byType == 2 && pTask->byPart == 2)
			{
				KSystemMessage	sMsg;
				sprintf(sMsg.szMessage, "%s", MSG_KILLER_NO_TASK);
				sMsg.eType = SMT_NORMAL;
				sMsg.byConfirmType = SMCT_NONE;
				sMsg.byPriority = 0;
				sMsg.byParamSize = 0;
				CoreDataChangedCallback(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
				break;
			}

			if (pTask->byType == 2 && pAllTask)
				pAllTask->ReceiveTask(pTask);
			else if ((pTask->byType == 0 || pTask->byType == 1) && pOwnTask)
				pOwnTask->ReceiveTask(pTask);
		}
		break;
	case GDCNI_KILLER_OPEN_CREATE_TASK:
		KUiKillerCreateTask::OpenWindow(0);
		break;
	case GDCNI_KILLER_OPEN_QUERY_WISEMAN:
		KUiGetString::OpenWindow("请输入对方名字:", "", &g_WndGameSpace, enumGSR_GET_ROLENAME_FOR_WISEMAN, 6, 32, TRUE);
		{
			KUiGetString* pSWnd = KUiGetString::GetIfVisible();
			if (pSWnd)
				((KWndWindow*)pSWnd)->SetPosition(340, 230);
		}
		break;
	case GDCNI_OPEN_COMMON_INPUT:
		{
			COMMON_INPUT* pCI = (COMMON_INPUT*)uParam;
			if (NULL == KUiGetString::GetIfVisible())	//不能同时打开两个
			{
				KUiGetString::OpenWindow(pCI->szTitle, pCI->bIsNumberInput ? NULL : pCI->szInitString,
						&g_WndGameSpace, pCI->bIsNumberInput ? enumGSR_GET_NUMBER_COMMON : enumGSR_GET_STRING_COMMON,
						pCI->bIsNumberInput ? pCI->nMinValue : pCI->nMinLen,
						pCI->bIsNumberInput ? pCI->nMaxValue : pCI->nMaxLen,
						TRUE, pCI->bIsNumberInput);
				KUiGetString* pSWnd = KUiGetString::GetIfVisible();
				if (pSWnd)
					((KWndWindow*)pSWnd)->SetPosition(340, 230);
			}
		}
		break;
	}
}

//////////////////////////////////////////////////////

void KClientCallback::CoreDataChanged(unsigned int uDataId, unsigned int uParam, int nParam)
{
	CoreDataChangedCallback(uDataId, uParam, nParam);
	g_SwordPluginCentre.CoreDataChanged(uDataId, uParam, nParam);
}

typedef std::map<std::string, std::string> BLACKLIST;
BLACKLIST g_BlackListUserNames;

#define BLACKLIST_UNITNAME	 "黑名单"

struct BlacklistNotify : public AddinNotify
{
	virtual int	RenameUnitGroup(const char* Unit, const char* Name, const char* NewName, const STRINGLIST& friends);
	virtual int	MoveUnitGroup(const char* Unit, const char* Name, const char* Name2, const STRINGLIST& friends);
	virtual int	DeleteUnitGroup(const char* Unit, const char* Name, const STRINGLIST& friends);
	virtual int	DeleteUnitMember(const char* Unit, const char* Name);
	virtual int	MoveUnitMember(const char* Unit, const char* Name, const char* Group);
	void	SendNotifyGroupFriend(const char* Unit, const std::string& group, const STRINGLIST& friends);
	void	SendNotifyDeleteFriend(const char* Unit, const char* Name);
};

BlacklistNotify s_LNotify;

void ClearBlackList()
{
	g_BlackListUserNames.clear();
}

void CreateBlackListUnit()
{
	ClearBlackList();
	KUiChatCentre::AddAddinUnit(BLACKLIST_UNITNAME, &s_LNotify);
}

void AddBlackList(const char* strName, const char* strGroup, bool bNotify)
{
	if (strName && strName[0] != 0 && strGroup)
	{
		g_BlackListUserNames[strName] = strGroup;

		if (g_BlackListUserNames.size() > 0)
		{
			int nUnit = KUiChatCentre::FindUnitIndex(BLACKLIST_UNITNAME);
			if (nUnit >= 0 && KUiChatCentre::IsMyFriend((char*)strName) == false)
			{
				if (KUiChatCentre::AddFriendInfo(nUnit, (char*)strName, (char*)strGroup))
				{
					if (bNotify)
					{
						char	szInfo[128];
						sprintf(szInfo, "%s 被加入黑名单。", strName);
						KUiMsgCentrePad::SystemMessageArrival(szInfo, strlen(szInfo));
					}
				}
			}
		}
	}
}

void RemoveBlackList(char* strName)
{
	if (strName && strName[0] != 0)
	{
		BLACKLIST::iterator i = g_BlackListUserNames.find(strName);
		if (i != g_BlackListUserNames.end())
		{
			g_BlackListUserNames.erase(i);

			int nUnit = KUiChatCentre::FindUnitIndex(BLACKLIST_UNITNAME);
			if (nUnit >= 0)
			{
				KUiChatCentre::DeleteFriend(nUnit, strName, true);
			}
		}
	}
}

bool IsInBlackName(char* strName)
{
	if (strName && strName[0] != 0)
	{
		return g_BlackListUserNames.find(strName) != g_BlackListUserNames.end();
	}

	return false;
}

int	SaveBlackListPrivateSetting(KIniFile* pFile, LPCSTR lpSection, int nStart)
{
	int i = nStart;
	char szKey[10];
	char szLine[256];
	BLACKLIST::iterator iU = g_BlackListUserNames.begin();
	while (iU != g_BlackListUserNames.end())
	{
		sprintf(szKey, "%d", i);
		sprintf(szLine, "AddBlackList(\"%s\", \"%s\",0)", iU->first.c_str(), iU->second.c_str());
		pFile->WriteString(lpSection, szKey, szLine);
		i++;
		iU++;
	}
	return i;
}

int	BlacklistNotify::RenameUnitGroup(const char* Unit, const char* Name, const char* NewName, const STRINGLIST& friends)
{
	AddinNotify::RenameUnitGroup(Unit, Name, NewName, friends);

	if (NewName && NewName[0])
		SendNotifyGroupFriend(Unit, std::string(NewName), friends);

	return 0;
}

int	BlacklistNotify::MoveUnitGroup(const char* Unit, const char* Name, const char* Name2, const STRINGLIST& friends)
{
	AddinNotify::MoveUnitGroup(Unit, Name, Name2, friends);

	if (Name2 && Name2[0])
		SendNotifyGroupFriend(Unit, std::string(Name2), friends);

	return 0;
}

int	BlacklistNotify::DeleteUnitGroup(const char* Unit, const char* Name, const STRINGLIST& friends)
{
	AddinNotify::DeleteUnitGroup(Unit, Name, friends);

	if (Name && Name[0])
		SendNotifyGroupFriend(Unit, "", friends);

	return 0;
}

int	BlacklistNotify::DeleteUnitMember(const char* Unit, const char* Name)
{
	AddinNotify::DeleteUnitMember(Unit, Name);

	SendNotifyDeleteFriend(Unit, Name);

	return 0;
}

int	BlacklistNotify::MoveUnitMember(const char* Unit, const char* Name, const char* Group)
{
	AddinNotify::MoveUnitMember(Unit, Name, Group);

	if (Name && Name[0])
	{
		STRINGLIST friends;
		friends.push_back(Name);
		SendNotifyGroupFriend(Unit, Group, friends);
	}

	return 0;
}

void BlacklistNotify::SendNotifyGroupFriend(const char* Unit, const std::string& group, const STRINGLIST& friends)
{
	if (strcmp(Unit, BLACKLIST_UNITNAME) == 0)
	{
		STRINGLIST::const_iterator i = friends.begin();
		while (i != friends.end())
		{
			g_BlackListUserNames[(*i)] = group;
			i++;
		}
	}
}

void BlacklistNotify::SendNotifyDeleteFriend(const char* Unit, const char* Name)
{
	if (strcmp(Unit, BLACKLIST_UNITNAME) == 0)
	{
		RemoveBlackList((char*)Name);
	}
}

///////////////////////////////////////////////////////////////////////////////

void KClientCallback::ChannelMessageArrival(DWORD nChannelID, char* szSendName, const char* pMsgBuff, unsigned short nMsgLength, bool bSucc)
{
	g_SwordPluginCentre.ChannelMessageArrival(nChannelID, szSendName, pMsgBuff, nMsgLength, bSucc);

	if (!bSucc)
	{
		char szInfo[256];
		int n = sprintf(szInfo, "抱歉, 频道还未开放,消息无法发到!");
		KUiMsgCentrePad::SystemMessageArrival(szInfo, n);
		return;
	}

	int nIndex = -1;
	if (nChannelID == -1)	//gm alias ID
	{
		//找到系统频道
		int nChannelDataCount = KUiMsgCentrePad::GetChannelCount();
		for (int n = 0; n < nChannelDataCount; n++)
		{
			if (KUiMsgCentrePad::IsChannelType(n, KUiMsgCentrePad::ch_GM))
			{
				nIndex = n;
				break;
			}
		}
		if (nIndex < 0)
			return;
		nChannelID = KUiMsgCentrePad::GetChannelID(nIndex);
		if (nChannelID == -1)
			return;
	}
	else
	{
		nIndex = KUiMsgCentrePad::GetChannelIndex(nChannelID);
		if (nIndex < 0)
			return;

		if (IsInBlackName(szSendName))
			return;
	}

	KUiMsgCentrePad::NewChannelMessageArrival(nChannelID, szSendName, pMsgBuff, nMsgLength);

	if (KUiMsgCentrePad::GetChannelSubscribe(nIndex) &&
		KUiMsgCentrePad::IsChannelType(nIndex, KUiMsgCentrePad::ch_Screen))
	{
		KUiPlayerItem SelectPlayer;
		int nKind = -1;
		if (g_pCoreShell->FindSpecialNPC(szSendName, &SelectPlayer, nKind) && nKind == kind_player)
		{
			g_pCoreShell->ChatSpecialPlayer(&SelectPlayer, pMsgBuff, nMsgLength);
		}
	}
}

void KClientCallback::MSNMessageArrival(char* szSourceName, char* szSendName, const char* pMsgBuff, unsigned short nMsgLength, MSG_FLAG eFlag)
{
	g_SwordPluginCentre.MSNMessageArrival(szSourceName, szSendName, pMsgBuff, nMsgLength, eFlag);

	if (eFlag != MSG_F_SUCCESSED)
	{
		char szInfo[256];
		const char* pszFormat = (eFlag == MSG_F_STORE) ?
			"%s不在游戏中，他(/她)在下次上线时才会收到您刚才发送的消息。" :
			"抱歉, %s 不在游戏中,消息无法发到!";
		int nLen = sprintf(szInfo, pszFormat, szSendName);
		KUiMsgCentrePad::SystemMessageArrival(szInfo, nLen);
		return;
	}

	if (IsInBlackName(szSourceName))
		return;

	bool		bNoAutoReply = false;
	tm			timeSend;
	timeSend.tm_year = 0;
	for (int i = 0; i < 2; i++)
	{
		if (nMsgLength >= 2 && pMsgBuff[0] == HAT_MSG_SPECIAL_CONTROL_PREFIX)
		{
			if (pMsgBuff[1] == CMSC_AUTOREPLY_MSG)
			{
				pMsgBuff   += 2;
				nMsgLength -= 2;
				bNoAutoReply = true;
			}
			else if (pMsgBuff[1] == CMSC_OFFLINE_MSG)
			{
				timeSend = *localtime((time_t*)(pMsgBuff + 2));
				timeSend.tm_year += 1900;
				timeSend.tm_mon += 1;
				pMsgBuff   += 2 + sizeof(time_t);
				nMsgLength -= 2 + sizeof(time_t);
				bNoAutoReply = true;
			}
			else
			{
				pMsgBuff--;
				nMsgLength--;
			}
		}
	}

	if (timeSend.tm_year == 0)
	{
		KUiMsgCentrePad::NewMSNMessageArrival(szSourceName, szSendName, pMsgBuff, nMsgLength);
	}
	else
	{
		char	szTime[32];
		int nTimeLen = sprintf(szTime, "发送于%d月%d日%d:%02d: ",
				timeSend.tm_mon, timeSend.tm_mday, timeSend.tm_hour, timeSend.tm_min);
		char* pExpandMsg = (char*)_alloca(nMsgLength + nTimeLen);
		if (pExpandMsg)
		{
			memcpy(pExpandMsg, szTime, nTimeLen);
			memcpy(pExpandMsg + nTimeLen, pMsgBuff, nMsgLength);
			KUiMsgCentrePad::NewMSNMessageArrival(szSourceName, szSendName, pExpandMsg, nMsgLength + nTimeLen);
		}
		else
		{
			KUiMsgCentrePad::NewMSNMessageArrival(szSourceName, szSendName, pMsgBuff, nMsgLength);
		}
	}
	FriendStatus(szSourceName, stateOnline);

	bool bSave = false;
	char szLogName[128];
	if (KUiPlayerBar::IsSelfName(szSourceName))	//是自己说的
	{
		bSave = KUiChatCentre::IsMyFriend(szSendName);	//是自己给朋友说的
		strncpy(szLogName, szSendName, 127);
		szLogName[127] = 0;
	}
	else
	{
		bSave = KUiChatCentre::IsMyFriend(szSourceName);	//是朋友给自己说的
		strncpy(szLogName, szSourceName, 127);
		szLogName[127] = 0;

		if (bNoAutoReply == false)
		{
			char	szAutoReply[256];
			int	nAutoReplyLen = KUiChatPhrase::GetAutoReply(szAutoReply, sizeof(szAutoReply), true);
			if (nAutoReplyLen > 0)
			{
				KUiPlayerBar::OnSendSomeoneMessage(szSourceName, szAutoReply, nAutoReplyLen);
			}
		}
	}

	if (bSave)
	{
		KFile* pFile = g_UiBase.OpenSaveSentenceFile(szLogName);
		if (pFile)
		{
			g_UiBase.SaveSentence(pFile, szSourceName, szSourceName,
				szSendName, szSendName, (char*)pMsgBuff, nMsgLength);

			g_UiBase.CloseSaveSentenceFile(pFile);
		}
	}

	//if (!KUiPlayerBar::IsSelfName(szSourceName))	//不是自己说的放到头顶
	//{
	//	KUiPlayerItem SelectPlayer;
	//	int nKind = -1;
	//	if (g_pCoreShell->FindSpecialNPC((char*)KUiPlayerBar::SelfName(), &SelectPlayer, nKind) && nKind == kind_player)
	//	{
	//		strncpy(SelectPlayer.Name, szSourceName, 32);	//为了显示别人的名字
	//		g_pCoreShell->ChatSpecialPlayer(&SelectPlayer, pMsgBuff, nMsgLength);
	//	}
	//}
}

void KClientCallback::NotifyChannel(const char* pszChannel, const char* pszName, DWORD channelid, bool bDisappear, BYTE cost)
{
	g_SwordPluginCentre.NotifyChannel(pszChannel, pszName, channelid, bDisappear, cost);
	if (bDisappear)
		KUiMsgCentrePad::FreeChannel(pszChannel);
	else if (channelid != (DWORD)(-1))
		KUiMsgCentrePad::OpenChannel(pszChannel, channelid, cost);
	else
		KUiMsgCentrePad::NewChannel(pszChannel, pszName);
}

void KClientCallback::FriendInvite(char* roleName)
{
	g_SwordPluginCentre.FriendInvite(roleName);

	if (roleName && roleName[0] != 0)
	{
		// 通知界面有人申请添加聊天好友
		KSystemMessage	sMsg;
		KUiPlayerItem	sPlayer;

		strcpy(sPlayer.Name, roleName);
		sPlayer.nIndex = -1;
		sPlayer.uId = 0;
		sPlayer.nData = 0;

		sprintf(sMsg.szMessage, MSG_CHAT_GET_FRIEND_APPLY, roleName);
		sMsg.eType = SMT_FRIEND;
		sMsg.byConfirmType = SMCT_UI_FRIEND_INVITE;
		sMsg.byPriority = 3;
		sMsg.byParamSize = sizeof(KUiPlayerItem);
		CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, (int)&sPlayer);
	}
}

void KClientCallback::AddFriend(char* roleName, BYTE answer)
{
	g_SwordPluginCentre.AddFriend(roleName, answer);

	if (roleName && roleName[0] != 0)
	{
		if (answer == answerAgree)
		{
			KSystemMessage	sMsg;
			KUiPlayerItem	sPlayer;

			memset(&sPlayer, 0, sizeof(KUiPlayerItem));
			strcpy(sPlayer.Name, roleName);
			sPlayer.uId = 0;
			sPlayer.nIndex = -1;

			sprintf(sMsg.szMessage, MSG_CHAT_ADD_FRIEND_SUCCESS, roleName);
			sMsg.eType = SMT_FRIEND;
			sMsg.byConfirmType = SMCT_UI_INTERVIEW;
			sMsg.byPriority = 2;
			sMsg.byParamSize = sizeof(KUiPlayerItem);
			CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, (int)&sPlayer);

			RemoveBlackList(roleName);

			int nUnit = KUiChatCentre::FindUnitIndexByRoleNameAtServerUnit(roleName);
			if (nUnit >= 0)
			{
				KUiChatCentre::DeleteFriend(nUnit, roleName, false);
			}

			KUiChatCentre::AddFriendInfo(0, roleName, "");
			KUiChatCentre::FriendStatus(0, roleName, stateOnline);
		}
		else if (answer == answerDisagree)
		{
			// 通知界面pRefuse->m_szName拒绝了他的交友申请
			KSystemMessage	sMsg;

			sprintf(sMsg.szMessage, MSG_CHAT_REFUSE_FRIEND, roleName);
			sMsg.eType = SMT_FRIEND;
			sMsg.byConfirmType = SMCT_CLICK;
			sMsg.byPriority = 1;
			sMsg.byParamSize = 0;
			CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
		}
		else if (answer == answerUnable)
		{
			// 通知界面添加某人为好友操作失败
			KSystemMessage	sMsg;
			sprintf(sMsg.szMessage, MSG_CHAT_ADD_FRIEND_FAIL, roleName);
			sMsg.eType = SMT_NORMAL;
			sMsg.byConfirmType = SMCT_NONE;
			sMsg.byPriority = 0;
			sMsg.byParamSize = 0;
			CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
		}
	}
}

void KClientCallback::FriendStatus(char* roleName, BYTE state)
{
	g_SwordPluginCentre.FriendStatus(roleName, state);

	if (roleName == NULL || roleName[0] == 0)
		return;

	int nUnit = KUiChatCentre::FindUnitIndexByRoleNameAtServerUnit(roleName);
	if (nUnit < 0)
		return;

	bool bNotify = false;
	if (state == stateDismiss || state == stateDelete)
	{
		KUiChatCentre::DeleteFriend(nUnit, roleName, false);
		bNotify = true;
	}
	else if (KUiChatCentre::FriendStatus(nUnit, roleName, state))
	{
		bNotify = true;
	}

	if (bNotify)
	{
		// 通知界面有好友上线
		KSystemMessage	sMsg;
		char szStringBuf[64];
		const char *szBuff = KUiChatCentre::QueryUnitNameByUnitIndex(nUnit);
		if (state == stateOnline)
		{
			sprintf(szStringBuf, MSG_CHAT_FRIEND_ONLINE, roleName);
		}
		else if (state == stateOffline)
		{
			sprintf(szStringBuf, MSG_CHAT_FRIEND_OFFLINE, roleName);
		}
		else if (state == stateDelete)
		{
			sprintf(szStringBuf, MSG_CHAT_FRIEND_DELETE, roleName);
		}
		else if (state == stateDismiss)
		{
			if (nUnit == 0)//FRIEND_INDEX
				sprintf(szStringBuf, MSG_CHAT_DISMISS_FRIEND, roleName);
			else if (nUnit == 1)//BROTHER_INDEX
				sprintf(szStringBuf, MSG_CHAT_DISMISS_BROTHER, roleName);
			else if (nUnit == 3)//MATE_INDEX
				sprintf(szStringBuf, MSG_CHAT_DISMISS_MATE, roleName);
			else
				sprintf(szStringBuf, MSG_CHAT_DISMISS_RELATION, roleName);
		}
		else
			return;
		if(nUnit == 2/*ENEMY_INDEX*/ && szBuff && szBuff[0])
		    sprintf(sMsg.szMessage, "［%s］%s", szBuff, szStringBuf);
		else
			strcpy(sMsg.szMessage, szStringBuf);

		sMsg.eType = SMT_NORMAL;
		sMsg.byConfirmType = SMCT_NONE;
		sMsg.byPriority = 0;
		sMsg.byParamSize = 0;
		CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
	}
}

void KClientCallback::FriendInfo(char* roleName, char* unitName, char* groupName, BYTE state)
{
	g_SwordPluginCentre.FriendInfo(roleName, unitName, groupName, state);

	if (roleName && roleName[0] != 0 && unitName && groupName)
	{
		int nUnit = 0;
		if (unitName[0])
			nUnit = KUiChatCentre::FindUnitIndex(unitName);
		if (nUnit >= 0)
		{
			int nUnitOld = KUiChatCentre::FindUnitIndexByRoleNameAtServerUnit(roleName);
			if (nUnitOld >= 0 && nUnitOld != nUnit)
			{
				KUiChatCentre::DeleteFriend(nUnitOld, roleName, false);
			}

			KUiChatCentre::AddFriendInfo(nUnit, roleName, groupName);
			KUiChatCentre::FriendStatus(nUnit, roleName, state);
		}
	}
}

void KClientCallback::AddPeople(char* unitName, char* roleName)
{
	g_SwordPluginCentre.AddPeople(unitName, roleName);

	if (roleName && roleName[0] != 0 && unitName)
	{
		int nUnit = KUiChatCentre::FindUnitIndex(unitName);
		if (nUnit >= 0)
		{
			int nUnitOld = KUiChatCentre::FindUnitIndexByRoleNameAtServerUnit(roleName);
			if (nUnitOld >= 0 && nUnitOld != nUnit)
			{
				KUiChatCentre::DeleteFriend(nUnitOld, roleName, false);
			}

			KUiChatCentre::AddFriendInfo(nUnit, roleName, "");
			KUiChatCentre::FriendStatus(nUnit, roleName, stateOnline);
		}
	}
}

#define LEVEL_TIPS_INI "\\Ui\\杂烩.ini"
//--------------------------------------------------------------------------
//	功能：等级的贴士
//--------------------------------------------------------------------------
void LevelTips(int nNewLevel)
{
	char szMsg[256], szBuf[16];
	KIniFile Ini;
	int nMsgLen;

	if(Ini.Load(LEVEL_TIPS_INI))
	{
		Ini.GetString("LevelUpTips", itoa(nNewLevel, szBuf, 10), "", szMsg, sizeof(szMsg));

		nMsgLen = TEncodeText(szMsg, strlen(szMsg));

		if(nMsgLen)
		{
		    KUiMsgCentrePad::SystemMessageArrival(szMsg, nMsgLen);
		}
	}
}

//--------------------------------------------------------------------------
//	功能：门派的贴士
//--------------------------------------------------------------------------
void FactionTips(int nbJoin)
{
	char szMsg[256] = "";
	KIniFile Ini;
	int nMsgLen;

	if(Ini.Load(LEVEL_TIPS_INI))
	{
		if (nbJoin)
			Ini.GetString("FactionTips", "Join", "", szMsg, sizeof(szMsg));

		nMsgLen = TEncodeText(szMsg, strlen(szMsg));

		if(nMsgLen)
		{
		    KUiMsgCentrePad::SystemMessageArrival(szMsg, nMsgLen);
		}
	}
}

//--------------------------------------------------------------------------
//	功能：贴士老大
//--------------------------------------------------------------------------
void GameWorldTips(unsigned int uParam, int nParam)
{
	if(uParam == PBP_LEVEL)
		LevelTips(nParam);
	else if(uParam == PBP_FACTION)
		FactionTips(nParam);
}
