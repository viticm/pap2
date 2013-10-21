#ifndef _KROLE_DEF_H_
#define _KROLE_DEF_H_

#include "SO3ProtocolBasic.h"

#define KG_ID_FIELD_NAME            "ID"
#define KG_NAME_FIELD_NAME          "RoleName"
#define KG_ACCOUNT_FIELD_NAME       "Account"
#define KG_BASE_INFO_FIELD_NAME     "BaseInfo"
#define KG_EXT_INFO_FIELD_NAME      "ExtInfo"
#define KG_LAST_MODIFY_FIELD_NAME   "LastModify"
#define KG_DELETE_TIME_FIELD_NAME   "DeleteTime"

#define MAX_QUEST_COUNT			    8192
#define MAX_ACCEPT_QUEST_COUNT	    25
#define MAX_DAILY_QUEST_COUNT	    15
#define MAX_ASSIST_QUEST_COUNT	    16

#define MAX_BUFF_REICPE_COUNT       1

#pragma pack(1)
struct KDB_ACCEPT_QUEST
{
	WORD        wQuestID;
	BYTE		byKillNpcCount[QUEST_PARAM_COUNT];
	int			nQuestValue[QUEST_PARAM_COUNT * 2];
	BYTE		byFailed;
	time_t		nLimitTime;
};

struct KDB_ASSIST_QUEST
{
    DWORD dwPlayerID;
    WORD  wQuestID;
};

struct KDB_DAILY_QUEST
{
    WORD wQuestID;
    time_t  nNextAcceptTime;
};

struct KDB_DAILY_QUEST_DATA
{
    BYTE            byCount;
    KDB_DAILY_QUEST DailyQuest[0];
};

struct KBUFF_RECIPE_KEY
{
    DWORD   dwID;
    int     nLevel;
    DWORD   dwRecipeKey[MAX_BUFF_REICPE_COUNT];
};

struct KBUFF_DB_DATA
{
    BYTE byCount;
    struct KBUFF_DB_ITEM 
    {
        KBUFF_RECIPE_KEY    RecipeKey;
        DWORD               dwSkillSrcID;
        BYTE                bySkillSrcLevel;
        BYTE                byStackNum;
        int                 nLeftFrame;
        int                 nCustomValue;
    } Items[0];
};
#pragma pack()
    
#endif // _KROLE_DEF_H_
