#include "stdafx.h"
#include "SO3GlobalDef.h"
#include "Global.h"
#include "Luna.h"
#include "KTongDef.h"
#include "KMentorDef.h"

#define REGISTER_CONST(__VALUE__)	{#__VALUE__, __VALUE__},
#define REGISTER_CONST_LIST(__VALUE__)	{#__VALUE__, LUA_CONST_##__VALUE__},

KLuaConst LUA_TONG_OPERATION_INDEX[] =
{
    {"SPEAK",                       totSpeak},                      //	在帮会频道发言
    {"JOIN_MANAGER_CHANNEL",        totJoinManagerChannel},         //	加入官员频道，并可以查看频道发言
    {"SPEAK_IN_MANAGER_CHANNEL",    totSpeakInManagerChannel},      //	在官员频道发言
    {"CHECK_OPERATION_RECORD",      totCheckOperationRecord},       //	查看帮会操作记录
    {"MODIFY_ANNOUNCEMENT",         totModifyAnnouncement},         //	修改帮会公告
    {"MODIFY_ONLINE_MESSAGE",       totModifyOnlineMessage},        //	修改帮会上线提示
    {"MODIFY_INTRODUCTION",         totModifyIntroduction},         //	修改帮会介绍
    {"MODIFY_RULES",                totModifyRules},                //	修改帮会帮规
    {"MODIFY_MEMORABILIA",          totModifyMemorabilia},          //	修改帮会大事记
    {"DEVELOP_TECHNOLOGY",          totDevelopTechnology},          //  投资科技树
    {"PRODUCT_ITEM",                totProductItem},	            //  生产道具

    {"PUT_REPOSITORY_PAGE2",        totPutRepositoryPage2},         // 	往仓库内第二页存放物品
    {"PUT_REPOSITORY_PAGE3",        totPutRepositoryPage3},	        //  往仓库内第三页存放物品
    {"PUT_REPOSITORY_PAGE4",        totPutRepositoryPage4},	        //  往仓库内第四页存放物品
    {"PUT_REPOSITORY_PAGE5",        totPutRepositoryPage5},	        //  往仓库内第五页存放物品
    {"PUT_REPOSITORY_PAGE6",        totPutRepositoryPage6},	        //  往仓库内第六页存放物品
    {"PUT_REPOSITORY_PAGE7",        totPutRepositoryPage7},	        //  往仓库内第七页存放物品
    {"PUT_REPOSITORY_PAGE8",        totPutRepositoryPage8},	        //  往仓库内第八页存放物品
    {"PUT_REPOSITORY_PAGE9",        totPutRepositoryPage9},	        //  往仓库内第九页存放物品
    {"PUT_REPOSITORY_PAGE10",       totPutRepositoryPage10},        //  往仓库内第十页存放物品

    {"GET_FROM_REPOSITORY_PAGE2",   totGetFromRepositoryPage2},     //	从仓库内第二页取出物品
    {"GET_FROM_REPOSITORY_PAGE3",   totGetFromRepositoryPage3},     //	从仓库内第三页取出物品
    {"GET_FROM_REPOSITORY_PAGE4",   totGetFromRepositoryPage4},     //	从仓库内第四页取出物品
    {"GET_FROM_REPOSITORY_PAGE5",   totGetFromRepositoryPage5},     //	从仓库内第五页取出物品
    {"GET_FROM_REPOSITORY_PAGE6",   totGetFromRepositoryPage6},     //	从仓库内第六页取出物品
    {"GET_FROM_REPOSITORY_PAGE7",   totGetFromRepositoryPage7},     //	从仓库内第七页取出物品
    {"GET_FROM_REPOSITORY_PAGE8",   totGetFromRepositoryPage8},     //	从仓库内第八页取出物品
    {"GET_FROM_REPOSITORY_PAGE9",   totGetFromRepositoryPage9},     //	从仓库内第九页取出物品
    {"GET_FROM_REPOSITORY_PAGE10",  totGetFromRepositoryPage10},    //	从仓库内第十页取出物品

    {"MODIFY_CALENDAR",             totModifyCalendar},             //	增加/编辑/删除帮会相关的日历项
    {"MANAGE_TASK",                 totManageTask},                 //	发起/取消帮会任务
    {"MANAGE_VOTE",                 totManageVote},                 //	发起/取消投票
    {"ADD_TO_GROUP",                totAddToGroup},                 //	加人入某一用户组
    {"MOVE_FROM_GROUP",             totMoveFromGroup},              //	将人从某一用户组中移除
    {"MODIFY_WAGE",                 totModifyWage},                 //	编辑他人工资
    {"PAY_SALARY",                  totPaySalary},                  //	发工资
    {"MODIFY_MEMBER_REMARK",        totModifyMemberRemark},         //	编辑他人的备注
    {"MODIFY_GROUP_NAME",           totModifyGroupName},            //  修改组名

    {NULL,                          0}
};

KLuaConst LUA_TONG_HISTORY_TYPE[]  =
{
    {"MEMBERMANAGE",    ohtMemberManage},
    {"FUND",            ohtFund},
    {"TECH_TREE",       ohtTechTree},
    {"ACTIVITY",        ohtActivity},
    {"REPERTORY",       ohtRepertory},

    {NULL, 0}
};

KLuaConst LUA_MENTOR_RECORD_STATE[] = 
{
    {"INVALID",              emrsInvalid},
    {"NORMAL",               emrsNormal},
    {"MENTOR_BREAK",         emrsMBreak},
    {"APPRENTICE_BREAK",     emrsABreak},
    {"BROKEN",               emrsBroken},
    {"GRADUATING",           emrsGraduating},
    {"GRADUATED",            emrsGraduated},
    
    {NULL, 0}
};

KLuaConst LUA_MAP_TYPE[] =
{
    {"NORMAL_MAP",          emtNormalMap},
    {"DUNGEON",             emtDungeon},
    {"BATTLE_FIELD",        emtBattleField},
    {"BIRTH_MAP",           emtBirthMap},

    {NULL,                  0}
};

KLuaConst LUA_CAMP_TYPE[] =
{
    {"NEUTRAL",             cNeutral},
    {"GOOD",                cGood},
    {"EVIL",                cEvil},

    {NULL, 0}    
};

KLuaConstList g_LuaCenterConstList[] =
{
    {"TONG_OPERATION_INDEX",    LUA_TONG_OPERATION_INDEX},
    {"TONG_HISTORY_TYPE",       LUA_TONG_HISTORY_TYPE},
    {"MENTOR_RECORD_STATE",     LUA_MENTOR_RECORD_STATE},
    {"MAP_TYPE",                LUA_MAP_TYPE},
    {"CAMP_TYPE",               LUA_CAMP_TYPE},

    {NULL,                      0}
};


