///////////////////////////////////////////////////////////////
// Copyright(c) Kingsoft
// 
// FileName :   KSO3RoleDBUpdater.h
// Creator  :   liuzhibiao
// Date     :   2009-6-8
// Comment  :   v0.1
//              2009-6-8	Create0.1
//             
///////////////////////////////////////////////////////////////

#ifndef _KSO3ROLEDBUPDATER_H
#define _KSO3ROLEDBUPDATER_H

#include "KG_SO3RoleDBUpdaterDef.h"

#include "Engine/EngineBase.h"
#include "Engine/FileType.h"
#include "kgpublic.h"
#include "kgd_interface.h"
#include "KRoleDBDataDef.h"
#include "luna.h"
#include "KSkillList.h"
#include "KSkillRecipeList.h"
#include "KItem.h"
#include "KRecipeList.h"
#include "KQuestInfoList.h"
#include "KQuestList.h"
#include "KProfessionList.h"
#include "Engine/Random.h"
#include "KBuffList.h"
#include "KReputation.h"
#include "KCDTimerList.h"
#include "KGPQ.h"

#define MAIL_TITLE_LEN_OLD 32

#pragma pack(1)

struct KRoleDataBlock
{
    BYTE	byBlockType;
    WORD	wDataLen;
    BYTE	byReserved;
};

struct KRoleDBData
{
    DWORD		        dwDataLen;
    DWORD		        dwCRC;
    WORD		        wVersion;
    WORD				wBlockCount;
    KRoleDataBlock		BlockList[0];
};

struct MAILBOX_DB_EX
{
    WORD    wVersion;
    DWORD   dwNextMailID;
};

struct MAIL_DATA_OLD
{
    DWORD           dwMailID;
    char            szSenderName[ROLE_NAME_LEN];
    char            szTitle[MAIL_TITLE_LEN_OLD];
    time_t          nReceiveTime;
    time_t          nGetAttachmentLock;
    BYTE            byFlags;
    KMAIL_CONTENT   Content;
};

struct MAIL_DATA
{
    DWORD           dwMailID;
    char            szSenderName[ROLE_NAME_LEN];
    char            szTitle[MAIL_TITLE_LEN];
    time_t          nReceiveTime;
    time_t          nGetAttachmentLock;
    BYTE            byFlags;
    KMAIL_CONTENT   Content;
};

struct ItemData 
{
    DWORD m_dwMoney;
    WORD  m_wEnabledBankPackageCount;
    WORD  m_wItemCount;

    struct KITEM_DB_HEADEREX 
    {
        BYTE byBox;
        BYTE byPos;
        BYTE byDataLen;
        union
        {
            KCOMMON_ITEM_DATA commonEquipData;
            KCUSTOM_EQUI_DATA customEquipData;
            KTRAIN_EQUIP_DATA trainEquipData;
        };
    } ItemArray[0];
};

struct QuestData 
{
    BYTE              m_dwQuestState[MAX_QUEST_COUNT];
    BYTE              m_AcceptQuestCount;
    KDB_ACCEPT_QUEST  m_AcceptQuest[MAX_ACCEPT_QUEST_COUNT];
    BYTE              m_AssistQuestCount;
    BYTE              m_AssistDailyCount;
    KDB_ASSIST_QUEST  m_AssistQuest[MAX_ASSIST_QUEST_COUNT];
    BYTE              m_DailyQuestCount;
    KDB_DAILY_QUEST   m_DailyQuest[MAX_ACCEPT_QUEST_COUNT];
};

struct UserPreferences 
{
    int  m_nSize;
    BYTE m_byUserPreferences[MAX_USER_PREFERENCES_LEN];
};

struct BookList 
{
    int  m_nSize;
    BYTE m_byBookList[MAX_READ_BOOK_ID];
};

struct KROLE_POSITION_DB_OLD
{
    DWORD	dwMapID;
    int64_t	nMapCopyIndex;
    int		nX;
    int		nY;
    int		nZ;
    BYTE    byFaceDirection;
};

struct KBaseInfo_Old
{
    char                    cRoleType;  // see ROLE_TYPE;
    KROLE_POSITION_DB_OLD   CurrentPos;
    KROLE_POSITION_DB_OLD   LastEntry;
    BYTE	                byLevel;
    BYTE                    byForceID;
    WORD                    wRepresentId[perRepresentCount];
    time_t	                nLastSaveTime;
    time_t                  nLastLoginTime;
    time_t                  nTotalGameTime;
    time_t                  nBanTime;
};
struct KROLE_STATE_INFO_OLD
{
    BYTE    byMoveState;
    WORD    wCurrentTrack;
    int     nMoveFrameCounter;
    WORD    wFromFlyNode;
    WORD    wTargetCityID;

    int     nExperience;

    int		nCurrentLife;				
    int		nCurrentMana;				
    int     nCurrentStamina;
    int     nCurrentThew;

    int     nAddTrainTimeInToday;
    int     nCurrentTrainValue;
    int     nUsedTrainValue;
    WORD    wReserved;

    BYTE    byPKState;
    WORD    wCloseSlayLeftTime; // µ•Œª£∫√Î

    WORD    wLeftReviveFrame;
    time_t  nLastSituReviveTime;
    BYTE    bySituReviveCount;
    DWORD   dwKillerID;

    WORD    wCurrentKillPoint;
    time_t  nLastKillPointReduceTime;
};

struct KBUFF_DB_DATA_OLD
{
    BYTE byCount;
    struct KBUFF_DB_ITEM 
    {
        KBUFF_RECIPE_KEY    RecipeKey;
        DWORD               dwSkillSrcID;
        BYTE                bySkillSrcLevel;
        BYTE                byStackNum;
        int                 nLeftFrame;
    } Items[0];
};

struct KFORCE_DB_DATA_OLD
{
    BYTE    byCamp;//--
    WORD    wCount;
    struct FORECE_INFO 
    {
        BYTE    byForce;
        BYTE    byFightFlag;
        BYTE    byLevel;
        short   shRepute;
        BYTE    byShow;
    }   ForceInfo[0];
};

struct KSO3Role
{
    char                               m_szRoleName[256];
    char                               m_szAccountName[256];
    DWORD                              m_dwRoleID;
    KBaseInfo_Old                      m_BaseInfo;
    KDB_SKILL_DATA*                    m_pSkillData;
    KG_SKILL_RECIPE_LIST_DB_DATA_V2*   m_pSkillRecipeList;
    ItemData*                          m_pItemData;
    QuestData                          m_QuestData;
    KProfessionDBData*                 m_pProfessionData;
    BYTE                               m_byRecipeState[MAX_CRAFT_COUNT][MAX_RECIPE_ID];
    //KRecipeDBData*                     m_pRecipeData;
    KROLE_STATE_INFO_OLD               m_StateInfo;
    KBUFF_DB_DATA_OLD*                 m_pBuffDBData;
    KFORCE_DB_DATA_OLD*                m_pForceDBData;
    UserPreferences                    m_UserPreferences;
    BookList                           m_BookList;
    COOLDOWN_LIST_DB_DATA*             m_pCooldownListDBData;
    KROLE_ROAD_INFO*                   m_pRoleRoadInfo;
    KGPQ_DB_LIST*                      m_pPQList;
    bool                               m_bHadExtInfo;
    IKG_Buffer*                        m_piPackage;
    size_t                             m_uPackageUseSize;
    bool                               m_bHadUpdateBaseInfo;
    bool                               m_bHadUpdateExtInfo;
    char                               m_szRoleTime[64];
};

#pragma pack()

class KSO3RoleDBTools;

class KSO3RoleDBUpdater
{
public:
    int Init();
    int Run();
    void UnInit();

    int UpdateBaseInfo(KRoleBaseInfo* pBaseInfo_old);
    int UpdateExtInfo(BYTE* pbyData, size_t uDataLen);
    int ResetRoleAllInfo();
    int ConverMailBoxInfo(unsigned long ulMailBoxTableID, BYTE* pbyData, unsigned long ulDataLen);
    int ResetMailBoxInfo();
    int GetMailBoxInfoSize(unsigned& refuSize);
    int InsertToNewDBRole(char szTableName[], char szTimeFileName[]);
    int InsertToNewDBMail(DWORD dwID);
    int GetRoleAllInfoToCompare(BYTE* byBaseInfoData, unsigned long ulBaseInfoSize, BYTE* byExtInfoData, unsigned long ulExtInfoSize, char szTableName[], char szTimeFileName[], DWORD dwID);

private:
    int _FillConnectConfig(const char cszConnectConfigFileName[]);
    int ConverSkillRecipeList(BYTE* pbyData, size_t uDataLen);
    int ConverSkillList(BYTE* pbyData, size_t uDataLen);
    int ConverItemList(BYTE* pbyData, size_t uDataLen);
    int ConverQuestList(BYTE* pbyData, size_t uDataLen);
    int ConverProfessionList(BYTE* pbyData, size_t uDataLen);
    int ConverRecipeList(BYTE* pbyData, size_t uDataLen);
    int ConverStateInfo(BYTE* pbyData, size_t uDataLen);
    int ConverBuffList(BYTE* pbyData, size_t uDataLen);
    int ConverRepute(BYTE* pbyData, size_t uDataLen);
    int ConverUserPreferences(BYTE* pbyData, size_t uDataLen);
    int ConverBookState(BYTE* pbyData, size_t uDataLen);
    int ConverCoolDownTimer(BYTE* pbyData, size_t uDataLen);
    int ConverRoadOpenList(BYTE* pbyData, size_t uDataLen);
    int ConverCustomData(BYTE* pbyData, size_t uDataLen);
    int ConverVisitedMap(BYTE* pbyData, size_t uDataLen);
    int ConverPQList(BYTE* pbyData, size_t uDataLen);
    

    BOOL SaveStateInfo(size_t* puUsedSize, BYTE* pbyBuffer, size_t uBufferSize);
    BOOL SaveBuffList(size_t* puUsedSize, BYTE* pbyBuffer, size_t uBufferSize);
    BOOL SaveReputeList(size_t* puUsedSize, BYTE* pbyBuffer, size_t uBufferSize);
    
public:
    KG_ROLEDBCONVERSION_CONFIG  m_RoleDBConfig;
    KG_ROLEDBCONVERSION_CONFIG  m_NewRoleDBConfig;
    KG_ROLEDBCONVERSION_CONFIG  m_CompareRoleDBConfig;
    KSO3Role                    m_SO3RoleAllInfo;
    IKG_Buffer*                 m_piMailBoxInfoPackage;
    int                         m_nMailInfoMaxSize;
    int                         m_nUpdateToLocal;
    char                        m_szMailBoxTime[64];

private:
    size_t                      m_uMailInfoSize;
    KSO3RoleDBTools*            m_pSO3RoleDBTool;
    KSO3RoleDBTools*            m_pNewSO3RoleDBTool;
    KSO3RoleDBTools*            m_pCompareSoleDBTool;
};

#endif  // _KSO3ROLEDBUPDATER_H