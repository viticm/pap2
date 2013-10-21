////////////////////////////////////////////////////////////////////////////////
//  Copyright(c) Kingsoft
//
//  FileName    : KG_GameStatistics.h
//  Version     : 1.0
//  Creator     : Hu Hao
//  Create Date : 2009-9-26  13:00:56
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _KG_GAME_STATISTICS_H_
#define _KG_GAME_STATISTICS_H_

#include "KG_GameDatabase.h"
#include "KG_PostDatabase.h"

class KG_GameStatistics
{
public:
    KG_GameStatistics();
    ~KG_GameStatistics();

    int Init(void *pvContext);
    int UnInit(void *pvContext);

    int Run();
private:
    typedef KG_TONG_DATA *_PTONG_DATA;
    typedef std::vector<KG_TONG_DATA *>    _PTONG_DATA_VECTOR;  // Used for stating top tongs.

    typedef KG_ROLE_DATA *_PROLE_DATA;
    typedef std::vector<KG_ROLE_DATA *>    _PROLE_DATA_VECTOR;  // Used for stating top roles.
private:
    int _LoadServerInfo(const char cszIniFileName[], KG_SERVER_INFO *pRetServerInfo);
    int _LoadMySQLDatabaseParameter(
        const char cszIniFileName[], const char cszSectionName[], KG_MYSQL_PARAM *pRetParam
    );    
    int _LoadEquipmentInformation(KG_EQUIPMENT_TYPE_MAP *pRetEquipmentTypeMap);
    int _LoadItemTabFile(const char cszTabFileName[], KG_EQUIPMENT_POINT_MAP *pRetEquipmentPointMap);

    int _StatTongEquipmentPoint(_PTONG_DATA_VECTOR *pPTongDataVector);

    int _StatRoleMoney(_PROLE_DATA_VECTOR *pPTongDataVector);
    int _StatRoleTrain(_PROLE_DATA_VECTOR *pPRoleDataVector);
    int _StatRoleRepute(_PROLE_DATA_VECTOR *pPRoleDataVector);
    int _StatRoleAchievement(_PROLE_DATA_VECTOR *pPRoleDataVector);
    int _StatRoleEquipmentPoint(_PROLE_DATA_VECTOR *pPRoleDataVector);

private:
    static bool _CompareTongEquipmentPoint(const _PTONG_DATA cLeft, const _PTONG_DATA cRight);
    
    static bool _CompareRoleMoney(const _PROLE_DATA cLeft, const _PROLE_DATA cRight);
    static bool _CompareRoleTrain(const _PROLE_DATA cLeft, const _PROLE_DATA cRight);
    static bool _CompareRoleRepute(const _PROLE_DATA cLeft, const _PROLE_DATA cRight);
    static bool _CompareRoleAchievement(const _PROLE_DATA cLeft, const _PROLE_DATA cRight);
    static bool _CompareRoleEquipmentPoint(const _PROLE_DATA cLeft, const _PROLE_DATA cRight);

private:
    KG_GameDatabase             m_GameDatabase;
    KG_PostDatabase             m_PostDatabase;
};

#endif  // _KG_GAME_STATISTICS_H_
