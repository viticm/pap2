#include "stdafx.h"
#include "KQuestInfoList.h"
#include "KSO3World.h"

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

    if (wPresentSkill)
    {
        Lua_PushString(L, "skill");
        Lua_PushNumber(L, wPresentSkill);
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

    if (nPresentTrain)
    {
        Lua_PushString(L, "presenttrain");
        Lua_PushNumber(L, nPresentTrain);
        Lua_SetTable(L, nTable);
        nHave = true;
    }

	int nNotSet = true;
	int nRelIndex = 1;
	for (int i = 0; i < QUEST_PARAM_COUNT; ++i)
	{
		if (byAffectForceID[i] != 0 && nAffectForceValue[i] != 0)
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
			Lua_PushNumber(L, byAffectForceID[i]);
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
        Lua_PushNumber(L, 0);
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
            if (wPresentItemAmount[i])
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
                Lua_PushNumber(L, wPresentItemAmount[i]);
                Lua_SetTable(L, -3);

                Lua_PushString(L, "version");
                Lua_PushNumber(L, 0);
                Lua_SetTable(L, -3);

                Lua_PushString(L, "type");
                Lua_PushNumber(L, byPresentItemType[i]);
                Lua_SetTable(L, -3);

                Lua_PushString(L, "index");
                Lua_PushNumber(L, wPresentItemIndex[i]);
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
		if (wPresentItemAmount[i])
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
			Lua_PushNumber(L, wPresentItemAmount[i]);
			Lua_SetTable(L, -3);

			Lua_PushString(L, "version");
			Lua_PushNumber(L, 0);
			Lua_SetTable(L, -3);

			Lua_PushString(L, "type");
			Lua_PushNumber(L, byPresentItemType[i]);
			Lua_SetTable(L, -3);

			Lua_PushString(L, "index");
			Lua_PushNumber(L, wPresentItemIndex[i]);
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
    REGISTER_LUA_DWORD(KQuestInfo, QuestClassID)

	REGISTER_LUA_BOOL(KQuestInfo, Share)					//是否可共享
	REGISTER_LUA_BOOL(KQuestInfo, Repeat)					//是否可重复
	REGISTER_LUA_BOOL(KQuestInfo, Assist)					//是否可协助
	REGISTER_LUA_BOOL(KQuestInfo, Accept)					//是否先接才能交
	REGISTER_LUA_INTEGER_READONLY(KQuestInfo, Level)					//任务等级

	REGISTER_LUA_DWORD_READONLY(KQuestInfo, StartMapID)		//接任务Npc所在地图的ID
	REGISTER_LUA_DWORD(KQuestInfo, StartNpcTemplateID)		//接任务Npc的ID
	REGISTER_LUA_DWORD_READONLY(KQuestInfo, StartItemType)			//接任务Item类型
	REGISTER_LUA_DWORD_READONLY(KQuestInfo, StartItemIndex)			//接任务Item类型

    REGISTER_LUA_DWORD(KQuestInfo, RequireSchoolMask)

    REGISTER_LUA_BOOL(KQuestInfo, RequireReputeAll)
    REGISTER_LUA_DWORD_READONLY(KQuestInfo, RequireForceID1)
    REGISTER_LUA_INTEGER_READONLY(KQuestInfo, ReputeLevelMin1)
    REGISTER_LUA_INTEGER_READONLY(KQuestInfo, ReputeLevelMax1)
    REGISTER_LUA_DWORD_READONLY(KQuestInfo, RequireForceID2)
    REGISTER_LUA_INTEGER_READONLY(KQuestInfo, ReputeLevelMin2)
    REGISTER_LUA_INTEGER_READONLY(KQuestInfo, ReputeLevelMax2)
    REGISTER_LUA_DWORD_READONLY(KQuestInfo, RequireForceID3)
    REGISTER_LUA_INTEGER_READONLY(KQuestInfo, ReputeLevelMin3)
    REGISTER_LUA_INTEGER_READONLY(KQuestInfo, ReputeLevelMax3)
    REGISTER_LUA_DWORD_READONLY(KQuestInfo, RequireForceID4)
    REGISTER_LUA_INTEGER_READONLY(KQuestInfo, ReputeLevelMin4)
    REGISTER_LUA_INTEGER_READONLY(KQuestInfo, ReputeLevelMax4)

    REGISTER_LUA_INTEGER_READONLY(KQuestInfo, MinLevel)			//接任务最小等级需求
	REGISTER_LUA_INTEGER_READONLY(KQuestInfo, MaxLevel)			//接任务最大等级需求
	REGISTER_LUA_INTEGER_READONLY(KQuestInfo, RequireGender)		//接任务的性别需求
	REGISTER_LUA_INTEGER_READONLY(KQuestInfo, RequireRoleType)	//接任务的体型需求
	REGISTER_LUA_DWORD(KQuestInfo, CoolDownID)			//CoolDown计时器ID
	REGISTER_LUA_BOOL(KQuestInfo, PrequestLogic)
	REGISTER_LUA_DWORD(KQuestInfo, PrequestID1)			//前置任务ID
	REGISTER_LUA_DWORD(KQuestInfo, PrequestID2)			//前置任务ID
	REGISTER_LUA_DWORD(KQuestInfo, PrequestID3)			//前置任务ID
	REGISTER_LUA_DWORD(KQuestInfo, PrequestID4)			//前置任务ID
	REGISTER_LUA_INTEGER(KQuestInfo, StartTime)			//开始时间
	REGISTER_LUA_INTEGER(KQuestInfo, EndTime)			//结束时间
	REGISTER_LUA_DWORD(KQuestInfo, SubsequenceID)		//直接后继任务ID   

	REGISTER_LUA_DWORD_READONLY(KQuestInfo, RequireSkillID)		//需求技能
	REGISTER_LUA_DWORD_READONLY(KQuestInfo, MinSkillLevel)		//需求技能最小等级

	REGISTER_LUA_DWORD_READONLY(KQuestInfo, RequireLifeSkillID)		//需求技能
	REGISTER_LUA_DWORD_READONLY(KQuestInfo, MinLifeSkillLevel)		//需求技能最小等级

	REGISTER_LUA_DWORD_READONLY(KQuestInfo, StartRequireItemType1)	//交任务时所需道具的类型
	REGISTER_LUA_DWORD_READONLY(KQuestInfo, StartRequireItemIndex1)	//交任务时所需道具的类型
	REGISTER_LUA_DWORD_READONLY(KQuestInfo, StartRequireItemAmount1)	//交任务时所需道具的数量
	REGISTER_LUA_DWORD_READONLY(KQuestInfo, StartRequireItemType2)	//交任务时所需道具的类型
	REGISTER_LUA_DWORD_READONLY(KQuestInfo, StartRequireItemIndex2)	//交任务时所需道具的类型
	REGISTER_LUA_DWORD_READONLY(KQuestInfo, StartRequireItemAmount2)	//交任务时所需道具的数量
	REGISTER_LUA_DWORD_READONLY(KQuestInfo, StartRequireItemType3)	//交任务时所需道具的类型
	REGISTER_LUA_DWORD_READONLY(KQuestInfo, StartRequireItemIndex3)	//交任务时所需道具的类型
	REGISTER_LUA_DWORD_READONLY(KQuestInfo, StartRequireItemAmount3)	//交任务时所需道具的数量
	REGISTER_LUA_DWORD_READONLY(KQuestInfo, StartRequireItemType4)	//交任务时所需道具的类型
	REGISTER_LUA_DWORD_READONLY(KQuestInfo, StartRequireItemIndex4)	//交任务时所需道具的类型
	REGISTER_LUA_DWORD_READONLY(KQuestInfo, StartRequireItemAmount4)	//交任务时所需道具的数量

	REGISTER_LUA_DWORD_READONLY(KQuestInfo, EndMapID)		//交任务Npc所在的地图ID
	REGISTER_LUA_DWORD(KQuestInfo, EndNpcTemplateID)		//交任务Npc的ID
    REGISTER_LUA_DWORD(KQuestInfo, EndDoodadTemplateID)     //交任务Doodad的ID
	REGISTER_LUA_INTEGER_READONLY(KQuestInfo, EndItemType)			//接任务Item类型
	REGISTER_LUA_INTEGER_READONLY(KQuestInfo, EndItemIndex)			//接任务Item类型

	REGISTER_LUA_INTEGER(KQuestInfo, FinishTime)			//完成时间限制

	REGISTER_LUA_DWORD_READONLY(KQuestInfo, EndRequireItemType1)	
	REGISTER_LUA_DWORD_READONLY(KQuestInfo, EndRequireItemIndex1)	
	REGISTER_LUA_DWORD_READONLY(KQuestInfo, EndRequireItemAmount1)	
	REGISTER_LUA_DWORD_READONLY(KQuestInfo, EndRequireItemType2)	
	REGISTER_LUA_DWORD_READONLY(KQuestInfo, EndRequireItemIndex2)	
	REGISTER_LUA_DWORD_READONLY(KQuestInfo, EndRequireItemAmount2)	
	REGISTER_LUA_DWORD_READONLY(KQuestInfo, EndRequireItemType3)	
	REGISTER_LUA_DWORD_READONLY(KQuestInfo, EndRequireItemIndex3)	
	REGISTER_LUA_DWORD_READONLY(KQuestInfo, EndRequireItemAmount3)	
	REGISTER_LUA_DWORD_READONLY(KQuestInfo, EndRequireItemType4)	
	REGISTER_LUA_DWORD_READONLY(KQuestInfo, EndRequireItemIndex4)	
	REGISTER_LUA_DWORD_READONLY(KQuestInfo, EndRequireItemAmount4)	
    REGISTER_LUA_DWORD_READONLY(KQuestInfo, EndRequireItemType5)	
	REGISTER_LUA_DWORD_READONLY(KQuestInfo, EndRequireItemIndex5)	
	REGISTER_LUA_DWORD_READONLY(KQuestInfo, EndRequireItemAmount5)	
    REGISTER_LUA_DWORD_READONLY(KQuestInfo, EndRequireItemType6)	
	REGISTER_LUA_DWORD_READONLY(KQuestInfo, EndRequireItemIndex6)	
	REGISTER_LUA_DWORD_READONLY(KQuestInfo, EndRequireItemAmount6)	
    REGISTER_LUA_DWORD_READONLY(KQuestInfo, EndRequireItemType7)	
	REGISTER_LUA_DWORD_READONLY(KQuestInfo, EndRequireItemIndex7)	
	REGISTER_LUA_DWORD_READONLY(KQuestInfo, EndRequireItemAmount7)	
    REGISTER_LUA_DWORD_READONLY(KQuestInfo, EndRequireItemType8)	
	REGISTER_LUA_DWORD_READONLY(KQuestInfo, EndRequireItemIndex8)	
	REGISTER_LUA_DWORD_READONLY(KQuestInfo, EndRequireItemAmount8)	

	REGISTER_LUA_INTEGER(KQuestInfo, CostMoney)				//消耗的金钱

	REGISTER_LUA_DWORD(KQuestInfo, KillNpcTemplateID1)		//交任务时所需杀死的Npc模板ID
	REGISTER_LUA_DWORD_READONLY(KQuestInfo, KillNpcAmount1)		//交任务时所需杀死的Npc数量
	REGISTER_LUA_DWORD(KQuestInfo, KillNpcTemplateID2)		//交任务时所需杀死的Npc模板ID
	REGISTER_LUA_DWORD_READONLY(KQuestInfo, KillNpcAmount2)		//交任务时所需杀死的Npc数量
	REGISTER_LUA_DWORD(KQuestInfo, KillNpcTemplateID3)		//交任务时所需杀死的Npc模板ID
	REGISTER_LUA_DWORD_READONLY(KQuestInfo, KillNpcAmount3)		//交任务时所需杀死的Npc数量
	REGISTER_LUA_DWORD(KQuestInfo, KillNpcTemplateID4)		//交任务时所需杀死的Npc模板ID
	REGISTER_LUA_DWORD_READONLY(KQuestInfo, KillNpcAmount4)		//交任务时所需杀死的Npc数量

	REGISTER_LUA_INTEGER(KQuestInfo, QuestValue1)			//任务变量
	REGISTER_LUA_INTEGER(KQuestInfo, QuestValue2)			//任务变量
	REGISTER_LUA_INTEGER(KQuestInfo, QuestValue3)			//任务变量
	REGISTER_LUA_INTEGER(KQuestInfo, QuestValue4)			//任务变量
	REGISTER_LUA_INTEGER(KQuestInfo, QuestValue5)			//任务变量
	REGISTER_LUA_INTEGER(KQuestInfo, QuestValue6)			//任务变量
	REGISTER_LUA_INTEGER(KQuestInfo, QuestValue7)			//任务变量
	REGISTER_LUA_INTEGER(KQuestInfo, QuestValue8)			//任务变量

	REGISTER_LUA_BOOL(KQuestInfo, HidePresent)

	REGISTER_LUA_INTEGER_READONLY(KQuestInfo, PresentAll1)		//第一组(KQuestInfo, 1-4)道具是否全给或者只给一件
	REGISTER_LUA_INTEGER_READONLY(KQuestInfo, PresentAll2)		//第二组(KQuestInfo, 5-8)道具是否全给或者只给一件

	REGISTER_LUA_DWORD_READONLY(KQuestInfo, PresentItemType1)	
	REGISTER_LUA_DWORD_READONLY(KQuestInfo, PresentItemIndex1)	
	REGISTER_LUA_DWORD_READONLY(KQuestInfo, PresentItemAmount1)	
	REGISTER_LUA_DWORD_READONLY(KQuestInfo, PresentItemType2)	
	REGISTER_LUA_DWORD_READONLY(KQuestInfo, PresentItemIndex2)	
	REGISTER_LUA_DWORD_READONLY(KQuestInfo, PresentItemAmount2)	
	REGISTER_LUA_DWORD_READONLY(KQuestInfo, PresentItemType3)	
	REGISTER_LUA_DWORD_READONLY(KQuestInfo, PresentItemIndex3)	
	REGISTER_LUA_DWORD_READONLY(KQuestInfo, PresentItemAmount3)	
	REGISTER_LUA_DWORD_READONLY(KQuestInfo, PresentItemType4)	
	REGISTER_LUA_DWORD_READONLY(KQuestInfo, PresentItemIndex4)	
	REGISTER_LUA_DWORD_READONLY(KQuestInfo, PresentItemAmount4)	
	REGISTER_LUA_DWORD_READONLY(KQuestInfo, PresentItemType5)	
	REGISTER_LUA_DWORD_READONLY(KQuestInfo, PresentItemIndex5)	
	REGISTER_LUA_DWORD_READONLY(KQuestInfo, PresentItemAmount5)	
	REGISTER_LUA_DWORD_READONLY(KQuestInfo, PresentItemType6)	
	REGISTER_LUA_DWORD_READONLY(KQuestInfo, PresentItemIndex6)	
	REGISTER_LUA_DWORD_READONLY(KQuestInfo, PresentItemAmount6)	
	REGISTER_LUA_DWORD_READONLY(KQuestInfo, PresentItemType7)	
	REGISTER_LUA_DWORD_READONLY(KQuestInfo, PresentItemIndex7)	
	REGISTER_LUA_DWORD_READONLY(KQuestInfo, PresentItemAmount7)	
	REGISTER_LUA_DWORD_READONLY(KQuestInfo, PresentItemType8)	
	REGISTER_LUA_DWORD_READONLY(KQuestInfo, PresentItemIndex8)	
	REGISTER_LUA_DWORD_READONLY(KQuestInfo, PresentItemAmount8)	

	REGISTER_LUA_INTEGER(KQuestInfo, PresentExp)			//交任务时奖励的经验
	REGISTER_LUA_INTEGER(KQuestInfo, PresentMoney)			//交任务时奖励的金钱数量
    REGISTER_LUA_INTEGER_READONLY(KQuestInfo, PresentSkill)

    REGISTER_LUA_INTEGER(KQuestInfo, PresentAssistThew)
    REGISTER_LUA_INTEGER(KQuestInfo, PresentAssistStamina)
    REGISTER_LUA_INTEGER(KQuestInfo, PresentAssistFriendship)

	REGISTER_LUA_DWORD_READONLY(KQuestInfo, AffectForceID1)			//交任务影响的势力
	REGISTER_LUA_INTEGER(KQuestInfo, AffectForceValue1)		//交任务影响的势力友好度
	REGISTER_LUA_DWORD_READONLY(KQuestInfo, AffectForceID2)			//交任务影响的势力
	REGISTER_LUA_INTEGER(KQuestInfo, AffectForceValue2)		//交任务影响的势力友好度
	REGISTER_LUA_DWORD_READONLY(KQuestInfo, AffectForceID3)			//交任务影响的势力
	REGISTER_LUA_INTEGER(KQuestInfo, AffectForceValue3)		//交任务影响的势力友好度
	REGISTER_LUA_DWORD_READONLY(KQuestInfo, AffectForceID4)			//交任务影响的势力
	REGISTER_LUA_INTEGER(KQuestInfo, AffectForceValue4)		//交任务影响的势力友好度

	REGISTER_LUA_INTEGER_READONLY(KQuestInfo, RepeatCutPercent)		//重复做任务时奖励的削减百分比

    REGISTER_LUA_INTEGER_READONLY(KQuestInfo, RequireCampMask)
    REGISTER_LUA_INTEGER(KQuestInfo, PresentPrestige)
    REGISTER_LUA_INTEGER(KQuestInfo, PresentContribution)
	
    REGISTER_LUA_FUNC(KQuestInfo, GetHortation)

DEFINE_LUA_CLASS_END(KQuestInfo)
