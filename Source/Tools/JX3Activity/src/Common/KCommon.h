/***************************************************
Filename   :   KCommon.h
CreatedBy  :   Monkey(Hou Rui)
CreatedOn  :   2008年2月12日
Description:   角色分析DLL提供给外部的接口
**************************************************/

#ifndef KCOMMON_H
#define KCOMMON_H

#ifdef JX3ROLEANALYSER_EXPORTS
#ifdef WIN32
#define DLLDECL __declspec(dllexport)
#else
#define DLLDECL
#endif

#else

#ifdef WIN32
#define DLLDECL __declspec(dllexport)
#else
#define DLLDECL
#endif

#endif

#ifdef WIN32
#include <windows.h>
#endif

typedef void* PARSEHANDLE;

#ifdef __cplusplus
extern "C" {
#endif

//资源清理函数，角色信息接口使用完必须调用本函数
DLLDECL void EndRoleParse(PARSEHANDLE hHandle);
//资源清理函数，邮件信息接口使用完必须调用本函数
DLLDECL void EndMailBoxParse(PARSEHANDLE hHandle);
//资源清理函数，邮件信息接口使用完必须调用本函数
DLLDECL void EndFellowshipParse(PARSEHANDLE hHandle);
//资源清理函数，寄卖行信息接口使用完必须调用本函数
DLLDECL void EndAuctionParse(PARSEHANDLE hHandle);
//资源清理函数，帮会信息接口使用完必须调用本函数
DLLDECL void EndTongParse(PARSEHANDLE hHandle);
//资源清理函数，数据统计接口使用完必须调用本函数
DLLDECL void EndStatDataParse(PARSEHANDLE hHandle);

//******************************************************************************
// 以下为角色基本信息获取接口
//******************************************************************************
//解析角色基本数据的基础函数，以下的Get接口使用前必须调用本函数
DLLDECL PARSEHANDLE ParseRoleBaseData(unsigned char* pbyData, size_t uDataLen);
//获取角色类型
DLLDECL int GetRoleType(PARSEHANDLE hParser, char& cRoleType);
//获取角色所在地图及当前坐标
DLLDECL int GetRolePosition(
                            PARSEHANDLE hParser, unsigned long& dwMapID, int& nMapCopyIndex,
                            int& nX, int& nY, int& nZ, unsigned char& byFaceDirection
);
//获取角色上一个进入点
DLLDECL int GetLastEntry(
                         PARSEHANDLE hParser, unsigned long& dwMapID, int& nMapCopyIndex,
                         int& nX, int& nY, int& nZ, unsigned char& byFaceDirection
);
//获取角色等级
DLLDECL int GetRoleLevel(PARSEHANDLE hParser, int& nRoleLevel);
//获取角色表现ID
DLLDECL int GetRepresentId(PARSEHANDLE hParser, unsigned short** pdwRepresentId);
//获取角色上次存盘时间
DLLDECL int GetLastSaveTime(PARSEHANDLE hParser, time_t& nLastSaveTime);
//获取角色上次登录时间
DLLDECL int GetLastLoginTime(PARSEHANDLE hParser, time_t& nLastLoginTime);
//获取角色总的游戏时间
DLLDECL int GetTotalGameTime(PARSEHANDLE hParser, time_t& nTotalGameTime);
//获取角色的阵营
DLLDECL int GetCamp(PARSEHANDLE hParser, int& nCamp);
//获取角色的势力
DLLDECL int GetForceID(PARSEHANDLE hParser, unsigned long& dwForceID);
//获取角色创建时间
DLLDECL int GetCreateTime(PARSEHANDLE hParser, time_t& nCreateTime);

//解析角色扩展数据的基础函数，以下的Get接口使用前必须调用本函数
DLLDECL PARSEHANDLE ParseRoleExtData(unsigned char* pbyData, size_t uDataLen);
//******************************************************************************
// 以下为技能信息获取接口
//******************************************************************************
//获取角色装备内功的ID
DLLDECL int GetMountKungfuID(PARSEHANDLE hParser, unsigned long& dwMountKungfuID);
//获取角色装备内功的Level
DLLDECL int GetMountKungfuLevel(PARSEHANDLE hParser, unsigned long& dwMountKungfuLevel);
//获取角色的技能数量
DLLDECL int GetSkillCount(PARSEHANDLE hParser, int& nSkillCount);
//获取角色的技能信息(这里是返回单个技能信息，若要获取角色技能列表的话，需要循环调用本函数)
//参数说明: unsigned long dwSkillIndex  技能索引号, unsigned short& wSkillID 技能ID, unsigned char& byMaxLevel 技能最高等级, unsigned long& dwExp 技能当前经验值
DLLDECL int GetSkillItem(PARSEHANDLE hParser, unsigned long dwSkillIndex, unsigned short& wSkillID, unsigned char& byMaxLevel, unsigned long& dwExp);


//******************************************************************************
// 以下为秘笈信息获取接口
//******************************************************************************
//获取角色的秘笈数量
DLLDECL int GetSkillRecipeCount(PARSEHANDLE hParser, int& nkillRecipeCount);
//获取角色的秘笈信息(这里是返回单个秘笈信息，若要获取角色秘笈列表的话，需要循环调用本函数)
//参数说明: unsigned long dwSkillRecipeIndex 秘笈索引号, unsigned long& dwRecipeKey 秘笈编号, unsigned char& byActive 秘笈是否激活
DLLDECL int GetSkillRecipe(PARSEHANDLE hParser, unsigned long dwSkillRecipeIndex, unsigned long& dwRecipeKey, unsigned char& byActive);


//******************************************************************************
// 以下为物品信息获取接口
//******************************************************************************
//获取角色的金钱数量
DLLDECL int GetRoleMoney(PARSEHANDLE hParser, unsigned long& dwMoney);
//获取角色的可用背包数量
DLLDECL int GetRoleEnabledBankPackageCount(PARSEHANDLE hParser, unsigned short& wEnabledBankPackageCount);
//获取角色的物品数量
DLLDECL int GetRoleItemCount(PARSEHANDLE hParser, unsigned short& wItemCount);
//获取角色的物品信息(这里是返回单个物品信息，若要获取角色所有物品列表的话，需要循环调用本函数)
//参数说明:unsigned long dwItemIndex 物品索引号, unsigned char& byBox 所在背包,
//  unsigned char& byPos 所在位置, unsigned char& byVersion 物品版本号, unsigned char& byTabType 物品类型,
//  unsigned short& wTabIndex 物品在所在类型中的索引, unsigned char& byBind 是否绑定, unsigned short& wDurability 耐久度,
//  time_t& nGenTime 物品生成时间 int& nIsEqui 是否为装备物品, unsigned long& dwRandSeed 装备随机种子, unsigned short& wLeftEnchantTime 剩余附魔时间, unsigned char& byColorID 颜色,
//  unsigned short& wPermanentEnchant 永久附魔ID, unsigned short& wTemporaryEnchant 临时附魔ID,
//  unsigned short& wMount1 装备镶嵌ID, unsigned short& wMount2 装备镶嵌ID, unsigned short& wMount3 装备镶嵌ID, unsigned short& wMount4 装备镶嵌ID
//  当nIsEqui为true时,在nIsEqui后面的参数才有意义.
DLLDECL int GetRoleItemInfo(
                            PARSEHANDLE hParser, unsigned long dwItemIndex, unsigned char& byBox, 
                            unsigned char& byPos, unsigned char& byVersion, unsigned char& byTabType, 
                            unsigned short& wTabIndex, unsigned char& byBind, unsigned short& wDurability, time_t& nGenTime,
                            int& nIsEqui, unsigned long& dwRandSeed, unsigned short& wLeftEnchantTime, unsigned char& byColorID,
                            unsigned short& wPermanentEnchant, unsigned short& wTemporaryEnchant,
                            unsigned short& wMount1, unsigned short& wMount2, unsigned short& wMount3, unsigned short& wMount4
);


//******************************************************************************
// 以下为任务信息获取接口
//******************************************************************************
//获取角色的任务状态(这里是角色单个任务的状态，共4000个任务，若要获取角色所有任务状态的话，需要循环调用本函数)
DLLDECL int GetRoleQuestState(PARSEHANDLE hParser, unsigned long dwQuestIndex, unsigned char& dwQuestState);
//获取角色当前接受任务的数量
DLLDECL int GetRoleAcceptQuestCount(PARSEHANDLE hParser, unsigned char& dwAcceptQuestCount);
//获取角色当前接受任务的信息(这里是返回单个任务信息，若要获取角色所有任务信息的话，需要循环调用本函数)
//参数说明:unsigned long dwQuestIndex 任务索引号, unsigned short& wQuestID 任务ID,
//  unsigned char& byFailed 是否失败, time_t& nLimitTime 限定时间,
//  unsigned char** byKillNpcCount 杀npc数目(传入必须大于等于4个unsigned char的空间), int& nKillNPCMaxKind 杀npc数目数组大小,
//  int** nQuestValue 任务变量(传入必须大于等于8个int的空间), int& nnQuestValueMaxKind 任务变量数组大小
DLLDECL int GetRoleQuestInfo(
                             PARSEHANDLE hParser, unsigned long dwQuestIndex, unsigned short& wQuestID,
                             unsigned char& byFailed, time_t& nLimitTime,
                             unsigned char** byKillNpcCount, int& nKillNPCMaxKind,
                             int** nQuestValue, int& nnQuestValueMaxKind
);

//获取角色当前协助任务的数量
DLLDECL int GetRoleAssistQuestCount(PARSEHANDLE hParser, unsigned char& dwAssistQuestCount);
//获取角色每天能接受的协助任务的上限
DLLDECL int GetRoleAssistDailyCount(PARSEHANDLE hParser, unsigned char& dwAssistDailyCount);
//获取角色协助任务的信息(这里是返回单个协助任务信息，若要获取角色所有协助任务信息的话，需要循环调用本函数)
//参数说明:unsigned long dwAssistQuestIndex 协助任务索引, unsigned long& dwPlayerID 协助玩家ID, unsigned short& dwQuestID 任务ID
DLLDECL int GetRoleAssistQuestInfo(PARSEHANDLE hParser, unsigned long dwAssistQuestIndex, unsigned long& dwPlayerID, unsigned short& dwQuestID);
//获取角色当前每日任务的数量
DLLDECL int GetRoleDailyQuestCount(PARSEHANDLE hParser, unsigned char& byDailyQuestCount);
//获取角色当前每日任务的信息(这里是返回单个任务信息，若要获取角色所有任务信息的话，需要循环调用本函数)
//参数说明:unsigned long dwQuestIndex 索引号, unsigned short& wQuestID 任务ID, int& nNextAcceptTime 下次可以接每日任务的时间
DLLDECL int GetRoleDailyQuestInfo(PARSEHANDLE hParser, unsigned long dwQuestIndex, unsigned short& wQuestID, int& nNextAcceptTime);


//******************************************************************************
// 以下为生活技能信息获取接口
//******************************************************************************
//获取角色当前学习的生活技能的数量
DLLDECL int GetRoleProfessionCount(PARSEHANDLE hParser, unsigned char& byCount);
//获取角色生活技能的信息(这里是返回单个生活技能信息，若要获取角色所有生活技能信息的话，需要循环调用本函数)
//参数说明:unsigned long dwProfessionIndex, 生活技能索引
//  unsigned char& byProfessionID 生活技能ID, unsigned char& byMaxLevel 生活技能最高等级,
//  unsigned char& byCurrentLevel 当前等级, unsigned short& wProficiency 熟练度,
//  unsigned char& byBranchID 生活技能的分支ID
DLLDECL int GetRoleProfessionInfo(
                                  PARSEHANDLE hParser, unsigned long dwProfessionIndex,
                                  unsigned char& byProfessionID, unsigned char& byMaxLevel,
                                  unsigned char& byCurrentLevel, unsigned short& wProficiency,
                                  unsigned char& byBranchID
);


//******************************************************************************
// 以下为角色生活技能秘籍信息获取接口
//******************************************************************************
//获取角色是否学习某一生活技能的某一秘籍(注：这里的生活技能ID：dwCraftID从1算起)
//参数说明:unsigned long dwRecipeIndex 秘笈索引号, int& bIsLearned 是否已经学习
DLLDECL int GetRoleRecipeListOfCraft(PARSEHANDLE hParser, unsigned long dwCraftID, unsigned long dwRecipeIndex, int& bIsLearned);


//******************************************************************************
// 以下为角色状态信息获取接口
//******************************************************************************
//获取角色当前移动状态
DLLDECL int GetRoleMoveState(PARSEHANDLE hParser, unsigned char& byMoveState);
//获取角色当前所在交通路径
DLLDECL int GetRoleCurrentTrack(PARSEHANDLE hParser, unsigned short& wCurrentTrack);
//获取角色当前在交通系统中的移动帧数
DLLDECL int GetRoleMoveFrameCounter(PARSEHANDLE hParser, int& nMoveFrameCounter);
//获取角色来自的交通结点编号
DLLDECL int GetRoleFromFlyNode(PARSEHANDLE hParser, unsigned short& wFromFlyNode);
//获取角色目的城市编号
DLLDECL int GetRoleTargetCityID(PARSEHANDLE hParser, unsigned short& wTargetCityID);
//获取角色当前经验值
DLLDECL int GetRoleExperience(PARSEHANDLE hParser, int& nExperience);
//获取角色当前生命值
DLLDECL int GetRoleCurrentLife(PARSEHANDLE hParser, int& nCurrentLife);
//获取角色当前内力值
DLLDECL int GetRoleCurrentMana(PARSEHANDLE hParser, int& nCurrentMana);
//获取角色当前精力值
DLLDECL int GetRoleCurrentStamina(PARSEHANDLE hParser, int& nCurrentStamina);
//获取角色当前体力值
DLLDECL int GetRoleCurrentThew(PARSEHANDLE hParser, int& nCurrentThew);
//获取角色当前潜能点
DLLDECL int GetRoleCurrentTrainValue(PARSEHANDLE hParser, int& nCurrentTrainValue);
//获取角色已经用掉的潜能点
DLLDECL int GetRoleUsedTrainValue(PARSEHANDLE hParser, int& nUsedTrainValue);
//获取角色今天已经增加的潜能点
DLLDECL int GetRoleAddTrainTimeInToday(PARSEHANDLE hParser, int& nAddTrainTimeInToday);
//获取角色的PK状态
DLLDECL int GetRolePKState(PARSEHANDLE hParser, unsigned char& byPKState);
//获取角色屠杀结束的剩余时间(单位：秒)
DLLDECL int GetRoleCloseSlayLeftTime(PARSEHANDLE hParser, unsigned short& wCloseSlayLeftTime);
//获取角色剩余的复活帧数
DLLDECL int GetRoleLeftReviveFrame(PARSEHANDLE hParser, unsigned short& wLeftReviveFrame);
//获取角色上次原地复活时间
DLLDECL int GetRoleLastSituReviveTime(PARSEHANDLE hParser, time_t& nLastSituReviveTime);
//获取角色一定时间内原地复活次数
DLLDECL int GetRoleSituReviveCount(PARSEHANDLE hParser, unsigned char& bySituReviveCount);
//获取杀死角色的角色ID
DLLDECL int GetRoleKillerID(PARSEHANDLE hParser, unsigned long& dwKillerID);
//获取角色当前的杀气值
DLLDECL int GetRoleCurrentKillPoint(PARSEHANDLE hParser, unsigned short& wCurrentKillPoint);
//获取角色当前的威望值
DLLDECL int GetRoleCurrentPrestige(PARSEHANDLE hParser, int& nCurrentPrestige);
//获取角色贡献度
DLLDECL int GetContribution(PARSEHANDLE hParser, int& nContribution);
//获取角色限制等级
DLLDECL int GetMaxLevel(PARSEHANDLE hParser, int& nMaxLevel);
//获取角色隐藏帽子状态
DLLDECL int GetHideHat(PARSEHANDLE hParser, int& nHideHat);
//获取角色的禁言时间
DLLDECL int GetBanTime(PARSEHANDLE hParser, time_t& nBanTime);

//******************************************************************************
// 以下为角色Buff信息获取接口
//******************************************************************************
//获取角色Buff数量
DLLDECL int GetRoleBuffCount(PARSEHANDLE hParser, unsigned char& byCount);

//获取角色Buff信息(这里是返回单个Buff信息，若要获取角色所有Buff信息的话，需要循环调用本函数)
//参数说明:unsigned long BuffIndex buff的索引号, unsigned long& dwID buff的ID号, 
//int& nLevel 等级, unsigned long& dwRecipeKey 秘笈编号, unsigned long& dwSkillSrcID 技能释放者的角色ID,
//unsigned char& nSkillSrcLevel 技能释放者的等级, unsigned char& byStackNum buff叠加数量, int& nLeftFrame buff的剩余有效帧数
DLLDECL int GetRoleBuffInfo(
                            PARSEHANDLE hParser, unsigned long BuffIndex, unsigned long& dwID,
                            int& nLevel, unsigned long& dwRecipeKey, unsigned long& dwSkillSrcID,
                            unsigned char& nSkillSrcLevel, unsigned char& byStackNum, int& nLeftFrame
);


//******************************************************************************
// 以下为角色PQ信息获取接口
//******************************************************************************
//获取角色PQ数量
DLLDECL int GetRolePQCount(PARSEHANDLE hParser, unsigned short& wCount);
//获取角色PQ信息(这里是返回单个PQ信息，若要获取角色所有PQ信息的话，需要循环调用本函数)
DLLDECL int GetRolePQInfo(PARSEHANDLE hParser, unsigned long PQIndex, unsigned long& dwID);

//******************************************************************************
// 以下为角色英雄信息获取接口
//******************************************************************************
//获取角色英雄副本标记
DLLDECL int GetRoleHeroFlag(PARSEHANDLE hParser, int& bHeroFlag);
//获取角色是否有进入某个英雄副本的权限
//参数说明:unsigned long dwMapID 场景ID, int& bOpenFlag 开启标记
DLLDECL int GetRoleHeroMapCopyOpenFlag(PARSEHANDLE hParser, unsigned long dwMapID, int& bOpenFlag);


//******************************************************************************
// 以下为角色成就数据获取接口
//******************************************************************************
//获取角色成就积分
//参数说明:int& nPoint  成就积分
DLLDECL int GetRoleAchievementPoint(PARSEHANDLE hParser, int& nPoint);
//获取角色游戏完成度
//参数说明:int& nRecord  游戏完成度
DLLDECL int GetRoleAchievementRecord(PARSEHANDLE hParser, int& nRecord);
//获取角色指定成就项的计数数目
//参数说明:int nID 成就项ID, int& nCount 数目
DLLDECL int GetRoleAchievementCounterCount(PARSEHANDLE hParser, int nID, int& nCount);
//获取角色指定项成就是否完成
//参数说明:int nID 成就项ID, int& nIsAchieve 完成标记
DLLDECL int GetRoleAchievementAchieveStateFormID(PARSEHANDLE hParser, int nID, int& nIsAchieve);

//******************************************************************************
// 以下为角色称号数据获取接口
//******************************************************************************
//获取角色当前前缀
//参数说明:int& nCurrentPrefix 当前前缀
DLLDECL int GetRoleDesignationCurrentPrefix(PARSEHANDLE hParser, int& nCurrentPrefix);
//获取角色当前后缀
//参数说明:int& nCurrentPostfix 当前后缀
DLLDECL int GetRoleDesignationCurrentPostfix(PARSEHANDLE hParser, int& nCurrentPostfix);
//获取角色辈份编号
//参数说明:int& nGenerationIndex 辈份编号
DLLDECL int GetRoleDesignationGenerationIndex(PARSEHANDLE hParser, int& nGenerationIndex);
//获取角色门派中的名字编号
//参数说明:int& nNameInForceIndex 门派中的名字编号
DLLDECL int GetRoleDesignationNameInForceIndex(PARSEHANDLE hParser, int& nNameInForceIndex);
//参数说明:int& nDisplayFlag是否显示
DLLDECL int GetRoleDesignationDisplayFlag(PARSEHANDLE hParser, int& nDisplayFlag);
//获取玩家是否获得某个前缀
//参数说明:int nPrefix 编号, int& nIsPrefixAcquired 是否获得
DLLDECL int GetRoleDesignationIsPrefixAcquiredFormPrefix(PARSEHANDLE hParser, int nPrefix, int& nIsPrefixAcquired);
//获取玩家是否获得某个后缀
//参数说明:int nPostfix 编号, int& nIsPostfixAcquired是否获得
DLLDECL int GetRoleDesignationIsPostfixAcquiredFormPostfix(PARSEHANDLE hParser, int nPostfix, int& nIsPostfixAcquired);

//******************************************************************************
// 以下为角色和谐社会发生器获取接口
//******************************************************************************
//获取角色获取任务物品掉落相关信息
//参数说明:unsigned long dwID 任务id, int nItemPosIndex 物品位置, int& nHadFind 是否找到, unsigned long& dwFrustration 挫折值, int& nCount 挫折数目
DLLDECL int GetRoleQuestDropInfo(PARSEHANDLE hParser, unsigned long dwID, int nItemPosIndex, int& nHadFind, unsigned long& dwFrustration, int& nCount);
//获取角色打通经脉相关信息
//参数说明:unsigned long dwSkillID  skillID号, unsigned long dwSkillLevel skill等级, int& nHadFind 是否找到, unsigned long& dwFrustration 挫折值, int& nCount 挫折数目
DLLDECL int GetVenationInfo(PARSEHANDLE hParser, unsigned long dwSkillID, unsigned long dwSkillLevel, int& nHadFind, unsigned long& dwFrustration, int& nCount);

//******************************************************************************
// 以下为角色MailBox信息获取接口
//******************************************************************************
//解析角色邮件数据的基础函数，以下的Get接口使用前必须调用本函数
DLLDECL PARSEHANDLE ParseMailBoxInfoData(unsigned char* pbyData, size_t uDataLen);

//获取角色邮件数量
DLLDECL int GetMailCount(PARSEHANDLE hParser, int& nMailCount);

//获取一封邮件基本信息(这里是返回单个邮件信息，若要获取角色所有邮件基本信息的话，需要循环调用本函数)
// 参数说明: unsigned long dwMailIndex 邮件索引, 
//unsigned long& dwMailID 邮件ID,
//char szSenderName[]  传入的发信人字符串指针, int nSenderNameSize 发信人字符串大小, 外部定义的szSenderName字符串存储空间大小, 要大于32,
//char szTitle[] 传入的信件标题字符串指针, int nTitleSize 信件标题字符串大小, 外部定义的szTitle字符串存储空间大小, 要大于64,             
//time_t& nReceiveTime 接收时间,
//time_t& nGetAttachmentLock 附件是否加锁, 
//unsigned char& byFlags 邮件类型(0x01为未读,0x02为金钱,0x04为文字,0x08为物品,0x10为系统邮件)(按二进制位进行解析,判断二进制标志位),比如：是否读过，是金钱、文字、物品、系统邮件中的某一种或多种
DLLDECL int GetMailBaseInfo(PARSEHANDLE hParser, unsigned long dwMailIndex,     
                            unsigned long& dwMailID,    
                            char szSenderName[], int nSenderNameSize, 
                            char szTitle[], int nTitleSize,
                            time_t& nReceiveTime,
                            time_t& nGetAttachmentLock, 
                            unsigned char& byFlags
);

//获取一封邮件扩展信息(这里是返回单个邮件信息，若要获取角色所有邮件扩展信息的话，需要循环调用本函数,倘若存在物品,只返回物品KCOMMON_ITEM_DATA部分)
//参数说明: unsigned long dwMailIndex, 邮件索引
//  int& nMoney 金钱,
//  char szText[] 传入的用于邮件文字内容的字符串指针, int nTextSize 传入的用于邮件文字内容的字符串的大小,要大于700,
//  int& nHadItem 是否存在物品,倘若存在,之后的物品参数才会起用.
//  unsigned char& byVersion 物品版本, unsigned char& byTabType 物品类型,
//  unsigned short& wTabIndex 物品所在类型中的索引, unsigned char& byBind 是否绑定,
//  unsigned short& wDurability 物品耐久度, time_t& nGenTime 物品生成时间
//  int& nIsEqui, unsigned long& dwRandSeed, 
//  unsigned short& wLeftEnchantTime, unsigned char& byColorID,
//  unsigned short& wPermanentEnchant, unsigned short& wTemporaryEnchant,
//  unsigned short& wMount1, unsigned short& wMount2, unsigned short& wMount3, unsigned short& wMount4
DLLDECL int GetMailExtInfo(PARSEHANDLE hParser, unsigned long dwMailIndex, 
                           int& nMoney,        
                           char szText[], int nTextSize,       
                           int& nHadItem,
                           unsigned char& byVersion, unsigned char& byTabType, 
                           unsigned short& wTabIndex, unsigned char& byBind,
                           unsigned short& wDurability, time_t& nGenTime, 
                           int& nIsEqui, unsigned long& dwRandSeed, 
                           unsigned short& wLeftEnchantTime, unsigned char& byColorID,
                           unsigned short& wPermanentEnchant, unsigned short& wTemporaryEnchant,
                           unsigned short& wMount1, unsigned short& wMount2, unsigned short& wMount3, unsigned short& wMount4
);

//******************************************************************************
// 以下为角色好友信息获取接口
//******************************************************************************
//解析角色好友数据的基础函数，以下的Get接口使用前必须调用本函数
DLLDECL PARSEHANDLE ParseFellowShipData(unsigned char* pbyData, size_t uDataLen);
//获取角色好友数量
DLLDECL int GetFellowShipCount(PARSEHANDLE hParser, int& nFellowShipCount);
//获取一个好友基本信息(这里是返回单个好友信息，若要获取角色所有好友基本信息的话，需要循环调用本函数)
//参数说明:int nFellowShipIndex,  好友索引号
//  unsigned long& dwAlliedPlayerID 好友ID, int& nAttraction 好感度, 
//  int& nGroupID 分组ID, int& nMarried 结婚标记, int& nBrother 兄弟标记, int& nWhisperDailyCount 每天密聊次数,
//  int& nDuelDailyCount 每天决斗次数, char szRemark[] 传入的用于保存备注字符串的指针, int nRemarkSize 传入的用于保存备注字符串的大小,要大于32
DLLDECL int GetFellowshipInfo(PARSEHANDLE hParser, int nFellowShipIndex, 
                              unsigned long& dwAlliedPlayerID, int& nAttraction,
                              int& nGroupID, int& nMarried, int& nBrother, int& nWhisperDailyCount,
                              int& nDuelDailyCount, char szRemark[], int nRemarkSize
);

//获取角色仇人数量
DLLDECL int GetFoeCount(PARSEHANDLE hParser, int& nFoeCount);
//获取一个仇人ID信息(这里是返回单个仇人ID，若要获取角色所有仇人ID的话，需要循环调用本函数)
//参数说明: int nFoeIndex 仇人索引号, unsigned long& dwAlliedPlayerID 仇人角色ID
DLLDECL int GetFoeID(PARSEHANDLE hParser, int nFoeIndex, unsigned long& dwAlliedPlayerID);
//获取角色黑名单数量
DLLDECL int GetBlackListCount(PARSEHANDLE hParser, int& nBlackListCount);//new+
//获取一个黑名单ID信息(这里是返回单个黑名单ID，若要获取角色所有黑名单ID的话，需要循环调用本函数)
//参数说明: int nBlackListIndex 黑名单索引号, unsigned long& dwAlliedPlayerID 黑名单角色ID
DLLDECL int GetBlackListID(PARSEHANDLE hParser, int nBlackListIndex, unsigned long& dwAlliedPlayerID);


//******************************************************************************
// 以下为寄卖行信息获取接口
//******************************************************************************
//解析寄卖行物品数据的基础函数，以下的Get接口使用前必须调用本函数
DLLDECL PARSEHANDLE PaserAuctionItemData(unsigned char* pbyData, size_t uDataLen);
//获取寄卖行寄卖物品信息,只返回物品KCOMMON_ITEM_DATA部分
// 参数说明: unsigned char& byVersion 物品版本, unsigned char& byTabType 物品类型,
//  unsigned short& wTabIndex 物品所在类型中的索引, unsigned char& byBind 是否绑定, unsigned short& wDurability 物品耐久度,
//  time_t& nGenTime 物品生成时间 int& nIsEqui 是否为装备, unsigned long& dwRandSeed 装备随机种子, 
// unsigned short& wLeftEnchantTime 剩余附魔时间, unsigned char& byColorID 颜色,
// unsigned short& wPermanentEnchant 永久附魔ID, unsigned short& wTemporaryEnchant 临时附魔ID,
// unsigned short& wMount1 装备镶嵌ID, unsigned short& wMount2 装备镶嵌ID, unsigned short& wMount3 装备镶嵌ID, unsigned short& wMount4 装备镶嵌ID
// 当nIsEqui为true时,在nIsEqui后面的参数才有意义.
DLLDECL int GetAuctionItemInfo(
                               PARSEHANDLE hParser, unsigned char& byVersion, unsigned char& byTabType, 
                               unsigned short& wTabIndex, unsigned char& byBind, unsigned short& wDurability, 
                               time_t& nGenTime, int& nIsEqui, unsigned long& dwRandSeed, 
                               unsigned long& dwLeftEnchantTime, unsigned char& byColorID,
                               unsigned short& wPermanentEnchant, unsigned short& wTemporaryEnchant,
                               unsigned short& wMount1, unsigned short& wMount2, unsigned short& wMount3, unsigned short& wMount4
);


//******************************************************************************
// 以下为帮会信息获取接口
//******************************************************************************
//解析帮会数据的基础函数，以下的Get接口使用前必须调用本函数
DLLDECL PARSEHANDLE PaserTongData(unsigned char* pbyData, size_t uDataLen);

//获取帮会的基础信息
//参数说明:char szName[] 传入的保存帮会名称字符串指针, int nNameSize 传入的保存帮会名称字符串大小, 要大于32
//unsigned long& dwMaster 帮主角色ID, int& nFund 资金, int& nLevel 等级 , int& nDevelopmentPoint 发展点,
//unsigned short& wMaxMemberCount 成员上限, unsigned char& byTongState 帮会状态( 1为尚在筹备期, 2为正常状态, 3为濒临淘汰), time_t& nStateTimer 状态到期时间点(如果处于正常状态,此参数无效),
//unsigned char& byCamp 阵营
DLLDECL int GetTongBaseInfo(PARSEHANDLE hParser, char szName[], int nNameSize, 
                            unsigned long& dwMaster, int& nFund, int& nLevel, int& nDevelopmentPoint,
                            unsigned short& wMaxMemberCount, unsigned char& byTongState, time_t& nStateTimer,
                            unsigned char& byCamp
);

//获取帮会的扩展信息
//参数说明:char szAnnouncement[] 传入用于保存帮会公告的字符串指针, int nAnnouncementSize 传入用于保存帮会公告的字符串的大小,要大于128,
//  char szOnlineMessage[] 传入用于保存帮会上线提示的字符串指针, int nOnlineMessage 传入用于保存帮会上线提示的字符串的大小,要大于128,                     
//  char szIntroduction[] 传入用于保存帮会介绍的字符串指针, int nIntroduction 传入用于保存帮会介绍的字符串的大小,要大于128, 
//  char szRules[] 传入用于保存帮会帮规的字符串指针, int nRules 传入用于保存帮会帮规的字符串的大小,要大于1024,                                   
DLLDECL int GetTongExInfo(PARSEHANDLE hParser, char szAnnouncement[], int nAnnouncementSize, 
                            char szOnlineMessage[], int nOnlineMessage, 
                            char szIntroduction[], int nIntroduction, 
                            char szRules[], int nRules
);

//获取帮会成员数目
DLLDECL int GetTongMemberCount(PARSEHANDLE hParser, int& nCount);

//获取一个帮会成员信息(这里是返回单个帮会成员，若要所有帮会成员的话，需要循环调用本函数)
//参数说明:int nMemberIndex 成员索引号, unsigned long& dwID 成员角色ID, 
//  int& nGroupID 所属成员组, time_t& nJoinTime 加入时间, time_t& nLastOfflineTime 最后在线时间,
//  char szRemark[] 传入用于保存备注的字符串指针, int nRemarkSize 传入用于保存备注的字符串大小,要大于32
//  int& nSalary // 工资储蓄(银币)
DLLDECL int GetTongMemberInfo(PARSEHANDLE hParser, int nMemberIndex, unsigned long& dwID, 
                              int& nGroupID, time_t& nJoinTime, time_t& nLastOfflineTime,
                              char szRemark[], int nRemarkSize, int& nSalary 
);

//******************************************************************************
// 以下为数据统计信息获取接口
//******************************************************************************
//解析数据统计数据的基础函数，以下的Get接口使用前必须调用本函数
DLLDECL PARSEHANDLE PaserStatData(unsigned char* pbyData, size_t uDataLen);

//获得使用到的ID最大值
//参数说明: nMaxUsedID 使用到的ID最大值
DLLDECL int GetStatDataMaxUsedID(PARSEHANDLE hParser, int& nMaxUsedID);

//获得指定ID中的数值
//参数说明: int nID ID号, long long& nValue数值
DLLDECL int GetStatDataValueFromID(PARSEHANDLE hParser, int nID, long long& nValue);

#ifdef __cplusplus
}
#endif


#endif
