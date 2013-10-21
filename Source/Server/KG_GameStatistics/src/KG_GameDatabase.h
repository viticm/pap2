////////////////////////////////////////////////////////////////////////////////
//  Copyright(c) Kingsoft
//
//  FileName    : KG_GameDatabase.h
//  Version     : 1.0
//  Creator     : Hu Hao
//  Create Date : 2009-12-11  14:52:13
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////


#ifndef _KG_GAME_DATABASE_H_
#define _KG_GAME_DATABASE_H_

#include <list>
#include <map>
#include "KG_MySQLDatabase.h"
#include "KG_TongInfo.h"
#include "KG_RoleInfo.h"

////////////////////////////////////////////////////////////////////////////////
struct KG_TONG_DATA
{
    int     nTongID;            // 基本信息：帮会ID
    char    szTongName[64];     // 基本信息：帮会名称
    int     nMasterID;          // 基本信息：帮主ID
    char    szMasterName[64];   // 基本信息：帮主名称
    int     nTongCampID;        // 基本信息：帮会所在阵营ID
    int     nMaxMemberCount;    // 基本信息：帮会成员上限人数
    float   fEquipmentPoint;    // 统计信息：帮会所有成员的装备点数总值
};
typedef std::list<KG_TONG_DATA>         KG_TONG_DATA_LIST;

struct KG_ROLE_DATA
{
    int     nRoleID;            // 基本信息：角色ID
    char    szRoleName[64];     // 基本信息：角色姓名
    int     nTongID;            // 基本信息：角色所在帮会ID
    int     nTongMasterID;      // 基本信息：角色所在帮会帮主ID
    int     nRoleLevel;         // 基本信息：角色等级
    int     nRoleForceID;       // 基本信息：角色门派ID
    int     nRoleCampID;        // 基本信息：角色所在阵营ID
    int     nTotalMoney;        // 统计信息：角色拥有金钱数
    int     nTotalTrain;        // 统计信息：角色拥有修为：丹田修改值 + 全身所有奇穴值总和
    int     nTotalRepute;       // 统计信息：角色所有势力声望总值
    int     nAchievementRecord; // 统计信息：角色拥有成就点数
    float   fEquipmentPoint;    // 统计信息：角色装备在身上的所有装备点数总值
}; 
typedef std::list<KG_ROLE_DATA>       KG_ROLE_DATA_LIST;

struct KG_GAME_STAT_TONG_DATA
{
    int     nTongID;        
    int     nMasterID;      
    int     nTongCampID;    
    int     nMaxMemberCount;
    int     nValue;             // 排名数值
    int     nRank;              // 排名名次
};
typedef std::list<KG_GAME_STAT_TONG_DATA> KG_GAME_STAT_TONG_DATA_LIST;

struct KG_GAME_STAT_ROLE_DATA
{
    int     nRoleID;       
    int     nTongID;       
    int     nTongMasterID; 
    int     nRoleLevel;    
    int     nRoleForceID;  
    int     nRoleCampID;   
    int     nValue;             // 排名数值
    int     nRank;              // 排名名次
};
typedef std::list<KG_GAME_STAT_ROLE_DATA> KG_GAME_STAT_ROLE_DATA_LIST;

typedef std::map<int, float>                  KG_EQUIPMENT_POINT_MAP; // Map with the ID with equipment point.
typedef std::map<int, KG_EQUIPMENT_POINT_MAP> KG_EQUIPMENT_TYPE_MAP;  // Map with equipment type with _EQUIPMENT_POINT_MAP.
////////////////////////////////////////////////////////////////////////////////
class KG_GameDatabase
{
public:
    KG_GameDatabase();
    ~KG_GameDatabase();

    int Init(
        const KG_MYSQL_PARAM &crMySQLParam, const KG_EQUIPMENT_TYPE_MAP &crEquipmentTypeMap,
        void *pvContext
    );
    int UnInit(void *pvContext);

    int GetTongAndRoleDataList(
        KG_TONG_DATA_LIST *pRetTongDataList, KG_ROLE_DATA_LIST *pRetRoleDataList
    );

    int InsertTongStatData(
        const char cszKeyName[], const KG_GAME_STAT_TONG_DATA_LIST &crTongDataList
    );

    int InsertRoleStatData(
        const char cszKeyName[], const KG_GAME_STAT_ROLE_DATA_LIST &crRoleDataList
    );
private:
    typedef std::map<int, KG_TONG_DATA *> _ROLE_TONG_MAP;   // Map the role ID with the belonged tong.
private:
    int _GetTongInformation(KG_TONG_DATA_LIST *pRetTongDataList);
    
    int _GetRoleInformation(KG_ROLE_DATA_LIST *pRetRoleDataList);

    int _ParseTongData(
        int nTongID, const unsigned char cbyTongData[], unsigned uTongDataSize,
        KG_TONG_DATA *pRetTongData
    );

    int _ParseRoleData(
        int nRoleID, const char cszRoleName[],
        const unsigned char cbyBaseInfo[], unsigned uBaseInfoSize,
        const unsigned char cbyExtInfo[], unsigned uExtInfoSize,
        KG_ROLE_DATA *pRetRoleData
    );

    int _GetRoleTotalTrain(const KG_RoleInfo &crRoleInfo, int *pnRetTrain);
    int _GetRoleTotalRepute(const KG_RoleInfo &crRoleInfo, int *pnRetRepute);
    int _GetRoleEquipmentPoint(const KG_RoleInfo &crRoleInfo, float *pfRetLevel);

    int _InsertRankResult(
        const char cszKeyName[], const unsigned char cbyValueData[], unsigned uDataSize
    );
private:
    MYSQL                  *m_pMySQL;
    KG_EQUIPMENT_TYPE_MAP   m_EquipmentTypeMap;
    _ROLE_TONG_MAP          m_RoleTongMap;
};

#endif  // _KG_GAME_DATABASE_H_