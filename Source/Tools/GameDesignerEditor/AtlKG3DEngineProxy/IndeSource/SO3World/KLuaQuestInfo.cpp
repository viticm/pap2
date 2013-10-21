#include "stdafx.h"
#include "KQuestInfoList.h"

int KQuestInfo::LuaGetHortation(Lua_State* L)
{
	int nHave = false;
	int nTopIndex = Lua_GetTopIndex(L);

	Lua_NewTable(L);
	int nTable = Lua_GetTopIndex(L);

	if (nPresentMoney)
	{
		Lua_PushString(L, "money");
		Lua_PushNumber(L, nPresentMoney);
		Lua_SetTable(L, nTable);
		nHave = true;
	}

    if (nPresentSkill)
    {
        Lua_PushString(L, "skill");
        Lua_PushNumber(L, nPresentSkill);
        Lua_SetTable(L, nTable);
        nHave = true;
    }

	if (nPresentPrestige)
	{
		Lua_PushString(L, "prestige");
		Lua_PushNumber(L, nPresentPrestige);
		Lua_SetTable(L, nTable);
		nHave = true;
	}

	if (nPresentContribution)
	{
		Lua_PushString(L, "contribution");
		Lua_PushNumber(L, nPresentContribution);
		Lua_SetTable(L, nTable);
		nHave = true;
	}

	int nNotSet = true;
	int nRelIndex = 1;
	for (int i = 0; i < QUEST_PARAM_COUNT; ++i)
	{
		if (dwAffectForceID[i] != 0 && nAffectForceValue[i] != 0)
		{
			if (nNotSet)
			{
				Lua_PushString(L, "reputation");
				Lua_NewTable(L);
				nHave = true;
				nNotSet = false;
			}

			Lua_PushNumber(L, nRelIndex);
			Lua_NewTable(L);
			++nRelIndex;

			Lua_PushString(L, "force");
			Lua_PushNumber(L, dwAffectForceID[i]);
			Lua_SetTable(L, -3);

			Lua_PushString(L, "value");
			Lua_PushNumber(L, nAffectForceValue[i]);
			Lua_SetTable(L, -3);

			Lua_SetTable(L, -3);
		}
	}
	if (!nNotSet)
	{
		Lua_SetTable(L, nTable);
	}

	nNotSet = true;
	nRelIndex = 1;

    if (bHidePresent) 
    {   // 隐藏显示,为制定的道具图标暂时定为Type5,Index2931,Count1
        if (nNotSet)
        {
            Lua_PushString(L, "itemgroup1");
            Lua_NewTable(L);

            Lua_PushString(L, "all");
            Lua_PushBoolean(L, bPresentAll[0]);
            Lua_SetTable(L, -3);
            nHave = true;
            nNotSet = false;
        }

        Lua_PushNumber(L, nRelIndex);
        Lua_NewTable(L);
        ++nRelIndex;

        Lua_PushString(L, "selectindex");
        Lua_PushNumber(L, 0);
        Lua_SetTable(L, -3);

        Lua_PushString(L, "count");
        Lua_PushNumber(L, 1);
        Lua_SetTable(L, -3);

        Lua_PushString(L, "version");
        Lua_PushNumber(L, CURRENT_ITEM_VERSION);
        Lua_SetTable(L, -3);

        Lua_PushString(L, "type");
        Lua_PushNumber(L, 5);
        Lua_SetTable(L, -3);

        Lua_PushString(L, "index");
        Lua_PushNumber(L, 2931);
        Lua_SetTable(L, -3);

        Lua_SetTable(L, -3);
    }
    else
    {
        for (int i = 0; i < QUEST_PARAM_COUNT; ++i)
        {
            if (dwPresentItemAmount[i])
            {
                if (nNotSet)
                {
                    Lua_PushString(L, "itemgroup1");
                    Lua_NewTable(L);

                    Lua_PushString(L, "all");
                    Lua_PushBoolean(L, bPresentAll[0]);
                    Lua_SetTable(L, -3);
                    nHave = true;
                    nNotSet = false;
                }

                Lua_PushNumber(L, nRelIndex);
                Lua_NewTable(L);
                ++nRelIndex;

                Lua_PushString(L, "selectindex");
                Lua_PushNumber(L, i);
                Lua_SetTable(L, -3);

                Lua_PushString(L, "count");
                Lua_PushNumber(L, dwPresentItemAmount[i]);
                Lua_SetTable(L, -3);

                Lua_PushString(L, "version");
                Lua_PushNumber(L, CURRENT_ITEM_VERSION);
                Lua_SetTable(L, -3);

                Lua_PushString(L, "type");
                Lua_PushNumber(L, dwPresentItemType[i]);
                Lua_SetTable(L, -3);

                Lua_PushString(L, "index");
                Lua_PushNumber(L, dwPresentItemIndex[i]);
                Lua_SetTable(L, -3);

                Lua_SetTable(L, -3);
            }
        }
    }
	if (!nNotSet)
	{
		Lua_SetTable(L, nTable);
	}

	nNotSet = true;
	nRelIndex = 1;
	for (int i = QUEST_PARAM_COUNT; i < QUEST_PARAM_COUNT * 2; ++i)
	{
		if (dwPresentItemAmount[i])
		{
			if (nNotSet)
			{
				Lua_PushString(L, "itemgroup2");
				Lua_NewTable(L);

				Lua_PushString(L, "all");
				Lua_PushBoolean(L, bPresentAll[1]);
				Lua_SetTable(L, -3);
				nHave = true;
				nNotSet = false;
			}

			Lua_PushNumber(L, nRelIndex);
			Lua_NewTable(L);
			++nRelIndex;

			Lua_PushString(L, "selectindex");
			Lua_PushNumber(L, i);
			Lua_SetTable(L, -3);

			Lua_PushString(L, "count");
			Lua_PushNumber(L, dwPresentItemAmount[i]);
			Lua_SetTable(L, -3);

			Lua_PushString(L, "version");
			Lua_PushNumber(L, CURRENT_ITEM_VERSION);
			Lua_SetTable(L, -3);

			Lua_PushString(L, "type");
			Lua_PushNumber(L, dwPresentItemType[i]);
			Lua_SetTable(L, -3);

			Lua_PushString(L, "index");
			Lua_PushNumber(L, dwPresentItemIndex[i]);
			Lua_SetTable(L, -3);

			Lua_SetTable(L, -3);
		}
	}
	if (!nNotSet)
	{
		Lua_SetTable(L, nTable);
	}

	if (nHave)
	{
		assert(nTopIndex + 1 == Lua_GetTopIndex(L));
		return 1;
	}
	else
	{
		Lua_SetTopIndex(L, nTopIndex);
		return 0;
	}
}

DEFINE_LUA_CLASS_BEGIN(KQuestInfo)
	REGISTER_LUA_STRING(KQuestInfo, Name)
	REGISTER_LUA_STRING(KQuestInfo, QuestClass)
	REGISTER_LUA_BOOL(KQuestInfo, Share)					//是否可共享
	REGISTER_LUA_BOOL(KQuestInfo, Repeat)					//是否可重复
	REGISTER_LUA_BOOL(KQuestInfo, Assist)					//是否可协助
	REGISTER_LUA_BOOL(KQuestInfo, Accept)					//是否先接才能交
	REGISTER_LUA_INTEGER(KQuestInfo, Level)					//任务等级

	REGISTER_LUA_STRING(KQuestInfo, QuestDiff)				//任务难度描述

	REGISTER_LUA_DWORD(KQuestInfo, StartMapID)				//接任务Npc所在地图的ID
	REGISTER_LUA_DWORD(KQuestInfo, StartNpcTemplateID)		//接任务Npc的ID
	REGISTER_LUA_DWORD(KQuestInfo, StartItemType)			//接任务Item类型
	REGISTER_LUA_DWORD(KQuestInfo, StartItemIndex)			//接任务Item类型

#ifdef _CLIENT
	REGISTER_LUA_STRING(KQuestInfo, Objective)			
	REGISTER_LUA_STRING(KQuestInfo, Description)
	REGISTER_LUA_INTEGER(KQuestInfo, StartGossipType)		//接任务闲谈类型
	REGISTER_LUA_STRING(KQuestInfo, StartGossip1)			//接任务闲谈内容
	REGISTER_LUA_STRING(KQuestInfo, StartGossip2)			//接任务闲谈内容
	REGISTER_LUA_STRING(KQuestInfo, StartGossip3)			//接任务闲谈内容
	REGISTER_LUA_STRING(KQuestInfo, StartGossip4)			//接任务闲谈内容
#endif

    REGISTER_LUA_DWORD(KQuestInfo, RequireSchoolMask)

    REGISTER_LUA_BOOL(KQuestInfo, RequireReputeAll)
    REGISTER_LUA_DWORD(KQuestInfo, RequireForceID1)
    REGISTER_LUA_INTEGER(KQuestInfo, ReputeLevelMin1)
    REGISTER_LUA_INTEGER(KQuestInfo, ReputeLevelMax1)
    REGISTER_LUA_DWORD(KQuestInfo, RequireForceID2)
    REGISTER_LUA_INTEGER(KQuestInfo, ReputeLevelMin2)
    REGISTER_LUA_INTEGER(KQuestInfo, ReputeLevelMax2)
    REGISTER_LUA_DWORD(KQuestInfo, RequireForceID3)
    REGISTER_LUA_INTEGER(KQuestInfo, ReputeLevelMin3)
    REGISTER_LUA_INTEGER(KQuestInfo, ReputeLevelMax3)
    REGISTER_LUA_DWORD(KQuestInfo, RequireForceID4)
    REGISTER_LUA_INTEGER(KQuestInfo, ReputeLevelMin4)
    REGISTER_LUA_INTEGER(KQuestInfo, ReputeLevelMax4)

    REGISTER_LUA_INTEGER(KQuestInfo, MinLevel)			//接任务最小等级需求
	REGISTER_LUA_INTEGER(KQuestInfo, MaxLevel)			//接任务最大等级需求
	REGISTER_LUA_INTEGER(KQuestInfo, RequireGender)		//接任务的性别需求
	REGISTER_LUA_INTEGER(KQuestInfo, RequireRoleType)	//接任务的体型需求
	REGISTER_LUA_DWORD(KQuestInfo, CoolDownID)			//CoolDown计时器ID
	REGISTER_LUA_BOOL(KQuestInfo, PrequestLogic)
	REGISTER_LUA_DWORD(KQuestInfo, PrequestID1)			//前置任务ID
	REGISTER_LUA_DWORD(KQuestInfo, PrequestID2)			//前置任务ID
	REGISTER_LUA_DWORD(KQuestInfo, PrequestID3)			//前置任务ID
	REGISTER_LUA_DWORD(KQuestInfo, PrequestID4)			//前置任务ID
	REGISTER_LUA_INTEGER(KQuestInfo, StartTime)			//开始时间
	REGISTER_LUA_INTEGER(KQuestInfo, EndTime)			//结束时间
	REGISTER_LUA_DWORD(KQuestInfo, SubsequenceID)		//直接后继任务ID   

	REGISTER_LUA_DWORD(KQuestInfo, RequireSkillID)		//需求技能
	REGISTER_LUA_DWORD(KQuestInfo, MinSkillLevel)		//需求技能最小等级

	REGISTER_LUA_DWORD(KQuestInfo, RequireLifeSkillID)		//需求技能
	REGISTER_LUA_DWORD(KQuestInfo, MinLifeSkillLevel)		//需求技能最小等级

	REGISTER_LUA_DWORD(KQuestInfo, StartRequireItemType1)	//交任务时所需道具的类型
	REGISTER_LUA_DWORD(KQuestInfo, StartRequireItemIndex1)	//交任务时所需道具的类型
	REGISTER_LUA_DWORD(KQuestInfo, StartRequireItemAmount1)	//交任务时所需道具的数量
	REGISTER_LUA_DWORD(KQuestInfo, StartRequireItemType2)	//交任务时所需道具的类型
	REGISTER_LUA_DWORD(KQuestInfo, StartRequireItemIndex2)	//交任务时所需道具的类型
	REGISTER_LUA_DWORD(KQuestInfo, StartRequireItemAmount2)	//交任务时所需道具的数量
	REGISTER_LUA_DWORD(KQuestInfo, StartRequireItemType3)	//交任务时所需道具的类型
	REGISTER_LUA_DWORD(KQuestInfo, StartRequireItemIndex3)	//交任务时所需道具的类型
	REGISTER_LUA_DWORD(KQuestInfo, StartRequireItemAmount3)	//交任务时所需道具的数量
	REGISTER_LUA_DWORD(KQuestInfo, StartRequireItemType4)	//交任务时所需道具的类型
	REGISTER_LUA_DWORD(KQuestInfo, StartRequireItemIndex4)	//交任务时所需道具的类型
	REGISTER_LUA_DWORD(KQuestInfo, StartRequireItemAmount4)	//交任务时所需道具的数量

	REGISTER_LUA_DWORD(KQuestInfo, OfferItemType1)			//接任务时提供道具的类型
	REGISTER_LUA_DWORD(KQuestInfo, OfferItemIndex1)			//接任务时提供道具的类型
	REGISTER_LUA_DWORD(KQuestInfo, OfferItemAmount1)		//接任务时提供道具的数量
	REGISTER_LUA_DWORD(KQuestInfo, OfferItemType2)			//接任务时提供道具的类型
	REGISTER_LUA_DWORD(KQuestInfo, OfferItemIndex2)			//接任务时提供道具的类型
	REGISTER_LUA_DWORD(KQuestInfo, OfferItemAmount2)		//接任务时提供道具的数量
	REGISTER_LUA_DWORD(KQuestInfo, OfferItemType3)			//接任务时提供道具的类型
	REGISTER_LUA_DWORD(KQuestInfo, OfferItemIndex3)			//接任务时提供道具的类型
	REGISTER_LUA_DWORD(KQuestInfo, OfferItemAmount3)		//接任务时提供道具的数量
	REGISTER_LUA_DWORD(KQuestInfo, OfferItemType4)			//接任务时提供道具的类型
	REGISTER_LUA_DWORD(KQuestInfo, OfferItemIndex4)			//接任务时提供道具的类型
	REGISTER_LUA_DWORD(KQuestInfo, OfferItemAmount4)		//接任务时提供道具的数量

	REGISTER_LUA_DWORD(KQuestInfo, EndMapID)				//交任务Npc所在的地图ID
	REGISTER_LUA_DWORD(KQuestInfo, EndNpcTemplateID)		//交任务Npc的ID
    REGISTER_LUA_DWORD(KQuestInfo, EndDoodadTemplateID)     //交任务Doodad的ID
	REGISTER_LUA_INTEGER(KQuestInfo, EndItemType)			//接任务Item类型
	REGISTER_LUA_INTEGER(KQuestInfo, EndItemIndex)			//接任务Item类型

#ifdef _CLIENT
	REGISTER_LUA_STRING(KQuestInfo, DunningDialogue)
	REGISTER_LUA_STRING(KQuestInfo, UnfinishedDialogue)
	REGISTER_LUA_STRING(KQuestInfo, FinishedDialogue)
	REGISTER_LUA_INTEGER(KQuestInfo, EndGossipType)			//交任务闲谈类型
	REGISTER_LUA_STRING(KQuestInfo, EndGossip1)				//交任务闲谈内容
	REGISTER_LUA_STRING(KQuestInfo, EndGossip2)				//交任务闲谈内容
	REGISTER_LUA_STRING(KQuestInfo, EndGossip3)				//交任务闲谈内容
	REGISTER_LUA_STRING(KQuestInfo, EndGossip4)				//交任务闲谈内容
#endif

	REGISTER_LUA_INTEGER(KQuestInfo, FinishTime)			//完成时间限制

	REGISTER_LUA_DWORD(KQuestInfo, EndRequireItemType1)		//交任务时所需道具的类型
	REGISTER_LUA_DWORD(KQuestInfo, EndRequireItemIndex1)	//交任务时所需道具的类型
	REGISTER_LUA_DWORD(KQuestInfo, EndRequireItemAmount1)	//交任务时所需道具的数量
	REGISTER_LUA_BOOL(KQuestInfo, IsDeleteEndRequireItem1)  //是否删除道具
	REGISTER_LUA_DWORD(KQuestInfo, DropItemNpcTemplateID1)		//掉落交任务道具的Npc模板ID
	REGISTER_LUA_DWORD(KQuestInfo, DropItemDoodadTemplateID1)	//掉落交任务道具的doodad模板ID
	REGISTER_LUA_DWORD(KQuestInfo, DropItemRate1)			//掉落任务道具的几率
	REGISTER_LUA_BOOL(KQuestInfo, DropForEach1)				//是否每个人掉一个

	REGISTER_LUA_DWORD(KQuestInfo, EndRequireItemType2)		//交任务时所需道具的类型
	REGISTER_LUA_DWORD(KQuestInfo, EndRequireItemIndex2)	//交任务时所需道具的类型
	REGISTER_LUA_DWORD(KQuestInfo, EndRequireItemAmount2)	//交任务时所需道具的数量
	REGISTER_LUA_BOOL(KQuestInfo, IsDeleteEndRequireItem2)  //是否删除道具
	REGISTER_LUA_DWORD(KQuestInfo, DropItemNpcTemplateID2)		//掉落交任务道具的Npc模板ID
	REGISTER_LUA_DWORD(KQuestInfo, DropItemDoodadTemplateID2)	//掉落交任务道具的doodad模板ID
	REGISTER_LUA_DWORD(KQuestInfo, DropItemRate2)			//掉落任务道具的几率
	REGISTER_LUA_BOOL(KQuestInfo, DropForEach2)				//是否每个人掉一个

	REGISTER_LUA_DWORD(KQuestInfo, EndRequireItemType3)		//交任务时所需道具的类型
	REGISTER_LUA_DWORD(KQuestInfo, EndRequireItemIndex3)	//交任务时所需道具的类型
	REGISTER_LUA_DWORD(KQuestInfo, EndRequireItemAmount3)	//交任务时所需道具的数量
	REGISTER_LUA_BOOL(KQuestInfo, IsDeleteEndRequireItem3)  //是否删除道具
	REGISTER_LUA_DWORD(KQuestInfo, DropItemNpcTemplateID3)		//掉落交任务道具的Npc模板ID
	REGISTER_LUA_DWORD(KQuestInfo, DropItemDoodadTemplateID3)	//掉落交任务道具的doodad模板ID
	REGISTER_LUA_DWORD(KQuestInfo, DropItemRate3)			//掉落任务道具的几率
	REGISTER_LUA_BOOL(KQuestInfo, DropForEach3)				//是否每个人掉一个

	REGISTER_LUA_DWORD(KQuestInfo, EndRequireItemType4)		//交任务时所需道具的类型
	REGISTER_LUA_DWORD(KQuestInfo, EndRequireItemIndex4)	//交任务时所需道具的类型
	REGISTER_LUA_DWORD(KQuestInfo, EndRequireItemAmount4)	//交任务时所需道具的数量
	REGISTER_LUA_BOOL(KQuestInfo, IsDeleteEndRequireItem4)  //是否删除道具
	REGISTER_LUA_DWORD(KQuestInfo, DropItemNpcTemplateID4)		//掉落交任务道具的Npc模板ID
	REGISTER_LUA_DWORD(KQuestInfo, DropItemDoodadTemplateID4)	//掉落交任务道具的doodad模板ID
	REGISTER_LUA_DWORD(KQuestInfo, DropItemRate4)			//掉落任务道具的几率
	REGISTER_LUA_BOOL(KQuestInfo, DropForEach4)				//是否每个人掉一个

    REGISTER_LUA_DWORD(KQuestInfo, EndRequireItemType5)		//交任务时所需道具的类型
	REGISTER_LUA_DWORD(KQuestInfo, EndRequireItemIndex5)	//交任务时所需道具的类型
	REGISTER_LUA_DWORD(KQuestInfo, EndRequireItemAmount5)	//交任务时所需道具的数量
	REGISTER_LUA_BOOL(KQuestInfo, IsDeleteEndRequireItem5)  //是否删除道具
	REGISTER_LUA_DWORD(KQuestInfo, DropItemNpcTemplateID5)		//掉落交任务道具的Npc模板ID
	REGISTER_LUA_DWORD(KQuestInfo, DropItemDoodadTemplateID5)	//掉落交任务道具的doodad模板ID
	REGISTER_LUA_DWORD(KQuestInfo, DropItemRate5)			//掉落任务道具的几率
	REGISTER_LUA_BOOL(KQuestInfo, DropForEach5)				//是否每个人掉一个

    REGISTER_LUA_DWORD(KQuestInfo, EndRequireItemType6)		//交任务时所需道具的类型
	REGISTER_LUA_DWORD(KQuestInfo, EndRequireItemIndex6)	//交任务时所需道具的类型
	REGISTER_LUA_DWORD(KQuestInfo, EndRequireItemAmount6)	//交任务时所需道具的数量
	REGISTER_LUA_BOOL(KQuestInfo, IsDeleteEndRequireItem6)  //是否删除道具
	REGISTER_LUA_DWORD(KQuestInfo, DropItemNpcTemplateID6)		//掉落交任务道具的Npc模板ID
	REGISTER_LUA_DWORD(KQuestInfo, DropItemDoodadTemplateID6)	//掉落交任务道具的doodad模板ID
	REGISTER_LUA_DWORD(KQuestInfo, DropItemRate6)			//掉落任务道具的几率
	REGISTER_LUA_BOOL(KQuestInfo, DropForEach6)				//是否每个人掉一个

    REGISTER_LUA_DWORD(KQuestInfo, EndRequireItemType7)		//交任务时所需道具的类型
	REGISTER_LUA_DWORD(KQuestInfo, EndRequireItemIndex7)	//交任务时所需道具的类型
	REGISTER_LUA_DWORD(KQuestInfo, EndRequireItemAmount7)	//交任务时所需道具的数量
	REGISTER_LUA_BOOL(KQuestInfo, IsDeleteEndRequireItem7)  //是否删除道具
	REGISTER_LUA_DWORD(KQuestInfo, DropItemNpcTemplateID7)		//掉落交任务道具的Npc模板ID
	REGISTER_LUA_DWORD(KQuestInfo, DropItemDoodadTemplateID7)	//掉落交任务道具的doodad模板ID
	REGISTER_LUA_DWORD(KQuestInfo, DropItemRate7)			//掉落任务道具的几率
	REGISTER_LUA_BOOL(KQuestInfo, DropForEach7)				//是否每个人掉一个

    REGISTER_LUA_DWORD(KQuestInfo, EndRequireItemType8)		//交任务时所需道具的类型
	REGISTER_LUA_DWORD(KQuestInfo, EndRequireItemIndex8)	//交任务时所需道具的类型
	REGISTER_LUA_DWORD(KQuestInfo, EndRequireItemAmount8)	//交任务时所需道具的数量
	REGISTER_LUA_BOOL(KQuestInfo, IsDeleteEndRequireItem8)  //是否删除道具
	REGISTER_LUA_DWORD(KQuestInfo, DropItemNpcTemplateID8)		//掉落交任务道具的Npc模板ID
	REGISTER_LUA_DWORD(KQuestInfo, DropItemDoodadTemplateID8)	//掉落交任务道具的doodad模板ID
	REGISTER_LUA_DWORD(KQuestInfo, DropItemRate8)			//掉落任务道具的几率
	REGISTER_LUA_BOOL(KQuestInfo, DropForEach8)				//是否每个人掉一个

	REGISTER_LUA_INTEGER(KQuestInfo, CostMoney)				//消耗的金钱

	REGISTER_LUA_DWORD(KQuestInfo, KillNpcTemplateID1)		//交任务时所需杀死的Npc模板ID
	REGISTER_LUA_DWORD(KQuestInfo, KillNpcAmount1)		//交任务时所需杀死的Npc数量
	REGISTER_LUA_DWORD(KQuestInfo, KillNpcTemplateID2)		//交任务时所需杀死的Npc模板ID
	REGISTER_LUA_DWORD(KQuestInfo, KillNpcAmount2)		//交任务时所需杀死的Npc数量
	REGISTER_LUA_DWORD(KQuestInfo, KillNpcTemplateID3)		//交任务时所需杀死的Npc模板ID
	REGISTER_LUA_DWORD(KQuestInfo, KillNpcAmount3)		//交任务时所需杀死的Npc数量
	REGISTER_LUA_DWORD(KQuestInfo, KillNpcTemplateID4)		//交任务时所需杀死的Npc模板ID
	REGISTER_LUA_DWORD(KQuestInfo, KillNpcAmount4)		//交任务时所需杀死的Npc数量

	REGISTER_LUA_INTEGER(KQuestInfo, QuestValue1)			//任务变量
	REGISTER_LUA_INTEGER(KQuestInfo, QuestValue2)			//任务变量
	REGISTER_LUA_INTEGER(KQuestInfo, QuestValue3)			//任务变量
	REGISTER_LUA_INTEGER(KQuestInfo, QuestValue4)			//任务变量
	REGISTER_LUA_INTEGER(KQuestInfo, QuestValue5)			//任务变量
	REGISTER_LUA_INTEGER(KQuestInfo, QuestValue6)			//任务变量
	REGISTER_LUA_INTEGER(KQuestInfo, QuestValue7)			//任务变量
	REGISTER_LUA_INTEGER(KQuestInfo, QuestValue8)			//任务变量

#ifdef _CLIENT
	REGISTER_LUA_STRING(KQuestInfo, QuestValueStr1)
	REGISTER_LUA_STRING(KQuestInfo, QuestValueStr2)
	REGISTER_LUA_STRING(KQuestInfo, QuestValueStr3)
	REGISTER_LUA_STRING(KQuestInfo, QuestValueStr4)
	REGISTER_LUA_STRING(KQuestInfo, QuestValueStr5)
	REGISTER_LUA_STRING(KQuestInfo, QuestValueStr6)
	REGISTER_LUA_STRING(KQuestInfo, QuestValueStr7)
	REGISTER_LUA_STRING(KQuestInfo, QuestValueStr8)
#endif

	REGISTER_LUA_INTEGER(KQuestInfo, HidePresent)

	REGISTER_LUA_INTEGER(KQuestInfo, PresentAll1)		//第一组(KQuestInfo, 1-4)道具是否全给或者只给一件
	REGISTER_LUA_INTEGER(KQuestInfo, PresentAll2)		//第二组(KQuestInfo, 5-8)道具是否全给或者只给一件

	REGISTER_LUA_DWORD(KQuestInfo, PresentItemType1)	//交任务时奖励道具的类型
	REGISTER_LUA_DWORD(KQuestInfo, PresentItemIndex1)	//交任务时奖励道具的类型
	REGISTER_LUA_DWORD(KQuestInfo, PresentItemAmount1)	//交任务时奖励道具的数量
	REGISTER_LUA_DWORD(KQuestInfo, PresentItemType2)	//交任务时奖励道具的类型
	REGISTER_LUA_DWORD(KQuestInfo, PresentItemIndex2)	//交任务时奖励道具的类型
	REGISTER_LUA_DWORD(KQuestInfo, PresentItemAmount2)	//交任务时奖励道具的数量
	REGISTER_LUA_DWORD(KQuestInfo, PresentItemType3)	//交任务时奖励道具的类型
	REGISTER_LUA_DWORD(KQuestInfo, PresentItemIndex3)	//交任务时奖励道具的类型
	REGISTER_LUA_DWORD(KQuestInfo, PresentItemAmount3)	//交任务时奖励道具的数量
	REGISTER_LUA_DWORD(KQuestInfo, PresentItemType4)	//交任务时奖励道具的类型
	REGISTER_LUA_DWORD(KQuestInfo, PresentItemIndex4)	//交任务时奖励道具的类型
	REGISTER_LUA_DWORD(KQuestInfo, PresentItemAmount4)	//交任务时奖励道具的数量
	REGISTER_LUA_DWORD(KQuestInfo, PresentItemType5)	//交任务时奖励道具的类型
	REGISTER_LUA_DWORD(KQuestInfo, PresentItemIndex5)	//交任务时奖励道具的类型
	REGISTER_LUA_DWORD(KQuestInfo, PresentItemAmount5)	//交任务时奖励道具的数量
	REGISTER_LUA_DWORD(KQuestInfo, PresentItemType6)	//交任务时奖励道具的类型
	REGISTER_LUA_DWORD(KQuestInfo, PresentItemIndex6)	//交任务时奖励道具的类型
	REGISTER_LUA_DWORD(KQuestInfo, PresentItemAmount6)	//交任务时奖励道具的数量
	REGISTER_LUA_DWORD(KQuestInfo, PresentItemType7)	//交任务时奖励道具的类型
	REGISTER_LUA_DWORD(KQuestInfo, PresentItemIndex7)	//交任务时奖励道具的类型
	REGISTER_LUA_DWORD(KQuestInfo, PresentItemAmount7)	//交任务时奖励道具的数量
	REGISTER_LUA_DWORD(KQuestInfo, PresentItemType8)	//交任务时奖励道具的类型
	REGISTER_LUA_DWORD(KQuestInfo, PresentItemIndex8)	//交任务时奖励道具的类型
	REGISTER_LUA_DWORD(KQuestInfo, PresentItemAmount8)	//交任务时奖励道具的数量

	REGISTER_LUA_INTEGER(KQuestInfo, PresentExp)			//交任务时奖励的经验
	REGISTER_LUA_INTEGER(KQuestInfo, PresentMoney)			//交任务时奖励的金钱数量
    REGISTER_LUA_INTEGER(KQuestInfo, PresentSkill)

    REGISTER_LUA_INTEGER(KQuestInfo, PresentAssistThew)
    REGISTER_LUA_INTEGER(KQuestInfo, PresentAssistStamina)
    REGISTER_LUA_INTEGER(KQuestInfo, PresentAssistFriendship)

	REGISTER_LUA_DWORD(KQuestInfo, AffectForceID1)			//交任务影响的势力
	REGISTER_LUA_INTEGER(KQuestInfo, AffectForceValue1)		//交任务影响的势力友好度
	REGISTER_LUA_DWORD(KQuestInfo, AffectForceID2)			//交任务影响的势力
	REGISTER_LUA_INTEGER(KQuestInfo, AffectForceValue2)		//交任务影响的势力友好度
	REGISTER_LUA_DWORD(KQuestInfo, AffectForceID3)			//交任务影响的势力
	REGISTER_LUA_INTEGER(KQuestInfo, AffectForceValue3)		//交任务影响的势力友好度
	REGISTER_LUA_DWORD(KQuestInfo, AffectForceID4)			//交任务影响的势力
	REGISTER_LUA_INTEGER(KQuestInfo, AffectForceValue4)		//交任务影响的势力友好度

	REGISTER_LUA_INTEGER(KQuestInfo, RepeatCutPercent)		//重复做任务时奖励的削减百分比

    REGISTER_LUA_INTEGER(KQuestInfo, RequireCampMask)
    REGISTER_LUA_INTEGER(KQuestInfo, PresentPrestige)
    REGISTER_LUA_INTEGER(KQuestInfo, PresentContribution)
	
    REGISTER_LUA_FUNC(KQuestInfo, GetHortation)

DEFINE_LUA_CLASS_END(KQuestInfo)
