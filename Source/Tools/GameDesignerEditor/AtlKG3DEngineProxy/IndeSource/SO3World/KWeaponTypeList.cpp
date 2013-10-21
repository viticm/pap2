#include "stdafx.h"
#include <algorithm>
#include "ProtocolBasic.h"
#include "SO3GlobalDef.h"
#include "KWeaponTypeList.h"
#include "StrMap.h"
#include "KItem.h"
#include "KSkill.h"
using namespace std;

#define WEAPON_TYPE_TAB "WeaponType.tab"

BOOL KWeaponTypeList::Init()
{
    BOOL        bResult                 = false;
    BOOL        bRetCode                = false;
    char        szFileName[MAX_PATH];
    ITabFile*   piTabFile               = NULL;
    int         nHeight                 = 0;

    snprintf(szFileName, sizeof(szFileName), "%s/%s", SETTING_DIR, WEAPON_TYPE_TAB);
    szFileName[sizeof(szFileName) - 1] = '\0';

    piTabFile = g_OpenTabFile(szFileName);
    if (!piTabFile)
    {
        KGLogPrintf(KGLOG_ERR, "[KWeaponTypeList] Failed to open tab file : %s\n", szFileName);
        goto Exit0;
    }

    nHeight = piTabFile->GetHeight();

    for (int nLine = 2; nLine <= nHeight; nLine++)
    {
        bRetCode = LoadLine(piTabFile, nLine);
        KGLOG_PROCESS_ERROR(bRetCode);
    }

    bResult = true;
Exit0:
    KG_COM_RELEASE(piTabFile);
    if (!bResult)
    {
        m_TypeList.clear();
    }
    return bResult;
}

void KWeaponTypeList::UnInit()
{
    m_TypeList.clear();
}

BOOL KWeaponTypeList::CheckType(DWORD dwTypeID, int eDetailType)
{
    BOOL                            bResult             = false;
    KTYPE_LIST::iterator            ListIter;
    KDETAIL_TYPE_TABLE*             pDetailTypeTable    = NULL;
    KDETAIL_TYPE_TABLE::iterator    DetailIter;

    // 对于0类型不检查,定死了0表示"任意武器(包括没有武器的情况)"
    KG_PROCESS_SUCCESS(dwTypeID == 0);

    ListIter = m_TypeList.find(dwTypeID);
    KG_PROCESS_ERROR(ListIter != m_TypeList.end());

    pDetailTypeTable = &ListIter->second.DetailTypeTable;

    DetailIter = find(pDetailTypeTable->begin(), pDetailTypeTable->end(), eDetailType);
    KG_PROCESS_ERROR(DetailIter != pDetailTypeTable->end());

Exit1:
    bResult = true;
Exit0:
    return bResult;
}

// 根据技能对武器需求的ID查询该ID是近程还是远程武器
int KWeaponTypeList::GetWeaponGenre(DWORD dwTypeID)
{
    int                     nResult = swrtInvalid;
    KTYPE_LIST::iterator    ListIter;

    KG_PROCESS_ERROR(dwTypeID > 0);
   
    ListIter = m_TypeList.find(dwTypeID);
    if (ListIter != m_TypeList.end())
    {
        nResult = ListIter->second.eWeaponGenre;
    }

Exit0:
    return nResult;
}

BOOL KWeaponTypeList::LoadLine(ITabFile* piTabFile, int nLine)
{
    BOOL                                bResult         = false;
    int                                 nRetCode        = false;
    DWORD                               dwTypeID        = 0;
    KSKILL_WEAPON_REQUEST_TYPE          eWeaponGenre    = swrtInvalid;
    char                                szValue[_NAME_LEN];
    KWEAPON_REQUEST_INFO                WeaponRequestInfo;

    DECLARE_STRING_MAP_BEGIN(WEAPON_DETAIL_TYPE)
        REGISTER_MAP_VALUE(wdtWand)
        REGISTER_MAP_VALUE(wdtSpear)
        REGISTER_MAP_VALUE(wdtSword)
        REGISTER_MAP_VALUE(wdtFist)
        REGISTER_MAP_VALUE(wdtPen)
        REGISTER_MAP_VALUE(wdtDoubleWeapon)
        REGISTER_MAP_VALUE(wdtSlingShot)
        REGISTER_MAP_VALUE(wdtDart)
        REGISTER_MAP_VALUE(wdtMachDart)
    DECLARE_STRING_MAP_END(WEAPON_DETAIL_TYPE)

    DECLARE_STRING_MAP_BEGIN(KSKILL_WEAPON_REQUEST_TYPE)
        REGISTER_MAP_VALUE(swrtMeleeWeapon)
        REGISTER_MAP_VALUE(swrtRangeWeapon)
    DECLARE_STRING_MAP_END(KSKILL_WEAPON_REQUEST_TYPE)


    nRetCode = piTabFile->GetInteger(nLine, "ID", 0, (int*)&dwTypeID);
    KGLOG_PROCESS_ERROR(nRetCode > 0);
    KGLOG_PROCESS_ERROR(dwTypeID > 0); // 不能将TypeID填为0,因为0已经被约定了表示"任意类型"
    KGLOG_PROCESS_ERROR(dwTypeID == (DWORD)(nLine - 1));

    nRetCode = piTabFile->GetString(nLine, "WeaponGenre", "", szValue, sizeof(szValue));
    KGLOG_PROCESS_ERROR(nRetCode > 0);
    MAP_STRING_LOCAL(KSKILL_WEAPON_REQUEST_TYPE, szValue, eWeaponGenre);
    
    WeaponRequestInfo.eWeaponGenre = eWeaponGenre;

    for (int nIndex = 0; nIndex < 6; nIndex++)
    {
        WEAPON_DETAIL_TYPE  eWeaponDetailType = wdtWand;
        char                szSecName[_NAME_LEN];

        snprintf(szSecName, sizeof(szSecName), "Type%d", nIndex + 1);
        szSecName[sizeof(szSecName) - 1] = '\0';

        nRetCode = piTabFile->GetString(nLine, szSecName, "", szValue, sizeof(szValue));
        if (nRetCode < 0)
            continue;        
        KGLOG_PROCESS_ERROR(nRetCode > 0);

        MAP_STRING_LOCAL(WEAPON_DETAIL_TYPE, szValue, eWeaponDetailType);

        WeaponRequestInfo.DetailTypeTable.push_back(eWeaponDetailType);
    }

    nRetCode = (int)WeaponRequestInfo.DetailTypeTable.size();
    if (nRetCode > 0)
    {
        m_TypeList[dwTypeID] = WeaponRequestInfo;
    }

    bResult = true;
Exit0:
    return bResult;
}
