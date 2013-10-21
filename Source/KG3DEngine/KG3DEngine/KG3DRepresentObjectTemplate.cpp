////////////////////////////////////////////////////////////////////////////////
//
//  FileName  : KG3DRepresentObjectTemplate.h
//  Author    : ZhaoXinyu 
//  CopyRight : Kingsoft Corporation All rights Reserved
//  History   : 2005.9.21 0:36 Create
//  Comment   :
//
////////////////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "KG3drepresentobjecttemplate.h"

#ifdef _DEBUG
#define new DEBUG_NEW_3DENGINE
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

KG3DRepresentObjectTemplate g_cObjectTemplate;   

KG3DRepresentObjectTemplate::KG3DRepresentObjectTemplate(void)
{
    strncpy(
        m_szConfigurationFilesName, ".\\物件\\物件\\", 
        sizeof(m_szConfigurationFilesName)
    );
    m_szConfigurationFilesName[sizeof(m_szConfigurationFilesName) - 1] = '\0';
}

KG3DRepresentObjectTemplate::~KG3DRepresentObjectTemplate(void)
{
}

int KG3DRepresentObjectTemplate::LoadFromIniFile(const char pcszFileName[])
{
    KG3DRepresetObjectPropertyModifior<OBJECT_PROPERTY>
        ::LoadFromIniFile(pcszFileName);
    return TRUE;
}

////////////////////////////////////////////////////////////////////////////////
//  物件属性设置 : 显示相关
////////////////////////////////////////////////////////////////////////////////
/*
int KG3DRepresentObjectTemplate::SetModelFile(
    unsigned nIndex, const char pcszFileName[]
)
{
    if (nIndex >= m_vecObjectProperty.size())
    {
        return FALSE;
    }

    strncpy(
        m_vecObjectProperty[nIndex].szModelFile, pcszFileName,
        sizeof(m_vecObjectProperty[nIndex].szModelFile)
    );
    m_vecObjectProperty[nIndex].szModelFile[
        sizeof(m_vecObjectProperty[nIndex].szModelFile) - 1
    ] = '\0';

    return TRUE;
}

int KG3DRepresentObjectTemplate::SetReplaceableTextureFile(
    unsigned nIndex, const char pcszFileName[]
 )
 {
     if (nIndex >= m_vecObjectProperty.size())
     {
         return FALSE;
     }

     strncpy(
         m_vecObjectProperty[nIndex].szReplaceableTextureFile, pcszFileName,
         sizeof(m_vecObjectProperty[nIndex].szReplaceableTextureFile)
     );
     m_vecObjectProperty[nIndex].szReplaceableTextureFile[
         sizeof(m_vecObjectProperty[nIndex].szReplaceableTextureFile) - 1
     ] = '\0';

     return TRUE;
 }

 int KG3DRepresentObjectTemplate::SetSpecial(
     unsigned nIndex, const char pcszFileName[]
 )
 {
    if (nIndex >= m_vecObjectProperty.size())
    {
        return FALSE;
    }

    strncpy(
        m_vecObjectProperty[nIndex].szSpecial, pcszFileName,
        sizeof(m_vecObjectProperty[nIndex].szSpecial)
    );
    m_vecObjectProperty[nIndex].szSpecial[
        sizeof(m_vecObjectProperty[nIndex].szSpecial) - 1
    ] = '\0';

    return TRUE;
}

 int KG3DRepresentObjectTemplate::SetMinimapText(
         unsigned nIndex, const char pcszText[]
 )
 {
    if (nIndex >= m_vecObjectProperty.size())
    {
        return FALSE;
    }

    strncpy(
        m_vecObjectProperty[nIndex].szMinimapText, pcszText,
        sizeof(m_vecObjectProperty[nIndex].szMinimapText)
     );
    m_vecObjectProperty[nIndex].szMinimapText[
        sizeof(m_vecObjectProperty[nIndex].szMinimapText) - 1
    ] = '\0';

    return TRUE;
}

 int KG3DRepresentObjectTemplate::SetTintingColor(
     unsigned nIndex, const DWORD dwColor
 )
 {
     if (nIndex >= m_vecObjectProperty.size())
     {
         return FALSE;
     }

     m_vecObjectProperty[nIndex].dwTintingColor = dwColor;

     return TRUE;
 }

 int KG3DRepresentObjectTemplate::SetChangeColor(
     unsigned nIndex, const DWORD dwColor
 )
 {
     if (nIndex >= m_vecObjectProperty.size())
     {
         return FALSE;
     }

     m_vecObjectProperty[nIndex].dwChangeColor = dwColor;

     return TRUE;
 }

 int KG3DRepresentObjectTemplate::SetShadowColor(
     unsigned nIndex, const DWORD dwColor
 )
 {
     if (nIndex >= m_vecObjectProperty.size())
     {
         return FALSE;
     }

     m_vecObjectProperty[nIndex].dwShadowColor = dwColor;

     return TRUE;
 }

 int KG3DRepresentObjectTemplate::SetMinimapShowColor(
     unsigned nIndex, const DWORD dwColor
 )
 {
     if (nIndex >= m_vecObjectProperty.size())
     {
         return FALSE;
     }

     m_vecObjectProperty[nIndex].dwMinimapShowColor = dwColor;

     return TRUE;
 }

 int KG3DRepresentObjectTemplate::SetDecayTime(
     unsigned nIndex, const DWORD dwTime
 )
 {
     if (nIndex >= m_vecObjectProperty.size())
     {
         return FALSE;
     }

     m_vecObjectProperty[nIndex].dwDecayTime = dwTime;

     return TRUE;
 }

 int KG3DRepresentObjectTemplate::SetElevationRadius(
     unsigned nIndex, const float fHight
 )
 {
     if (nIndex >= m_vecObjectProperty.size())
     {
         return FALSE;
     }

     m_vecObjectProperty[nIndex].fElevationRadius = fHight;

     return TRUE;
 }

 int KG3DRepresentObjectTemplate::SetInteractionSpecial(
     unsigned nIndex, const int nSpecial
 )
 {
     if (nIndex >= m_vecObjectProperty.size())
     {
         return FALSE;
     }

     m_vecObjectProperty[nIndex].nInteractionSpecial = nSpecial;

     return TRUE;
 }

 int KG3DRepresentObjectTemplate::SetOrder(unsigned nIndex, const int nOrder)
 {
     if (nIndex >= m_vecObjectProperty.size())
     {
         return FALSE;
     }

     m_vecObjectProperty[nIndex].nOrder = nOrder;

     return TRUE;
 }

 int KG3DRepresentObjectTemplate::SetVisabilityLevel(
     unsigned nIndex, const int nLevel
 )
 {
     if (nIndex >= m_vecObjectProperty.size())
     {
         return FALSE;
     }

     m_vecObjectProperty[nIndex].nVisabilityLevel = nLevel;

     return TRUE;
 }

 int KG3DRepresentObjectTemplate::SetIfHighLight(
     unsigned nIndex, const bool bHightLight
 )
 {
     if (nIndex >= m_vecObjectProperty.size())
     {
         return FALSE;
     }

     m_vecObjectProperty[nIndex].bHighLight = bHightLight;

     return TRUE;
 }

 int KG3DRepresentObjectTemplate::SetIfAmbientLight(
     unsigned nIndex, const bool bAmbientLigth
 )
 {
     if (nIndex >= m_vecObjectProperty.size())
     {
         return FALSE;
     }

     m_vecObjectProperty[nIndex].bAmbientLight = bAmbientLigth;

     return TRUE;
 }

 int KG3DRepresentObjectTemplate::SetIfShadow(unsigned nIndex, const bool bShadow)
 {
     if (nIndex >= m_vecObjectProperty.size())
     {
         return FALSE;
     }

     m_vecObjectProperty[nIndex].bShadow = bShadow;

     return TRUE;
 }

 int KG3DRepresentObjectTemplate::SetIfMinimapShow(
     unsigned nIndex, const bool bMinimapShow
 )
 {
     if (nIndex >= m_vecObjectProperty.size())
     {
         return FALSE;
     }

     m_vecObjectProperty[nIndex].bMinimapShow = bMinimapShow;

     return TRUE;
 }

 int KG3DRepresentObjectTemplate::SetIfMinimapShowText(
     unsigned nIndex, const bool bMinimapShowText
 )
 {
     if (nIndex >= m_vecObjectProperty.size())
     {
         return FALSE;
     }

     m_vecObjectProperty[nIndex].bMinimapShowText = bMinimapShowText;

     return TRUE;
 }

 int KG3DRepresentObjectTemplate::SetIfReplaceableTexture(
     unsigned nIndex, const bool bReplaceableTexture
 )
 {
     if (nIndex >= m_vecObjectProperty.size())
     {
         return FALSE;
     }

     m_vecObjectProperty[nIndex].bReplaceableTexture = bReplaceableTexture;

     return TRUE;
 }

 int KG3DRepresentObjectTemplate::SetIfTranslucence(
     unsigned nIndex, const bool bTranslucence
 )
 {
     if (nIndex >= m_vecObjectProperty.size())
     {
         return FALSE;
     }

     m_vecObjectProperty[nIndex].bTranslucence = bTranslucence;

     return TRUE;
 }

 int KG3DRepresentObjectTemplate::SetIfHighLightScene(
     unsigned nIndex, const bool bHighLightScene
 )
 {
     if (nIndex >= m_vecObjectProperty.size())
     {
         return FALSE;
     }

     m_vecObjectProperty[nIndex].bHighLightScene = bHighLightScene;

     return TRUE;
 }

 ///////////////////////////////////////////////////////////////////////////////
 //  物件属性设置 : 声音相关
 ///////////////////////////////////////////////////////////////////////////////

 int KG3DRepresentObjectTemplate::SetRevive(
     unsigned nIndex, const char pcszFileName[]
 )
 {
     if (nIndex >= m_vecObjectProperty.size())
     {
         return FALSE;
     }

     strncpy(
         m_vecObjectProperty[nIndex].szRevive, pcszFileName,
         sizeof(m_vecObjectProperty[nIndex].szRevive)
     );
     m_vecObjectProperty[nIndex].szRevive[
         sizeof(m_vecObjectProperty[nIndex].szRevive) - 1
     ] = '\0';

     return TRUE;
 }

 int KG3DRepresentObjectTemplate::SetDeath(
     unsigned nIndex, const char pcszFileName[]
 )
 {
     if (nIndex >= m_vecObjectProperty.size())
     {
         return FALSE;
     }

     strncpy(
         m_vecObjectProperty[nIndex].szDeath, pcszFileName,
         sizeof(m_vecObjectProperty[nIndex].szDeath)
     );
     m_vecObjectProperty[nIndex].szDeath[
         sizeof(m_vecObjectProperty[nIndex].szDeath) - 1
     ] = '\0';

     return TRUE;
 }

 int KG3DRepresentObjectTemplate::SetMove(
     unsigned nIndex, const char pcszFileName[]
 )
 {
     if (nIndex >= m_vecObjectProperty.size())
     {
         return FALSE;
     }

     strncpy(
         m_vecObjectProperty[nIndex].szMove, pcszFileName,
         sizeof(m_vecObjectProperty[nIndex].szMove)
     );
     m_vecObjectProperty[nIndex].szMove[
         sizeof(m_vecObjectProperty[nIndex].szMove) - 1
     ] = '\0';

     return TRUE;
 }

 int KG3DRepresentObjectTemplate::SetLooping(
     unsigned nIndex, const char pcszFileName[]
 )
 {
     if (nIndex >= m_vecObjectProperty.size())
     {
         return FALSE;
     }

     strncpy(
         m_vecObjectProperty[nIndex].szLooping, pcszFileName,
         sizeof(m_vecObjectProperty[nIndex].szLooping)
         );
     m_vecObjectProperty[nIndex].szLooping[
         sizeof(m_vecObjectProperty[nIndex].szLooping) - 1
     ] = '\0';

     return TRUE;
 }


 int KG3DRepresentObjectTemplate::SetLoopingTime(
     unsigned nIndex, const DWORD dwTime
 )
 {
    if (nIndex >= m_vecObjectProperty.size())
    {
        return FALSE;
    }

    m_vecObjectProperty[nIndex].dwLoopingTime = dwTime;

    return TRUE;
 }

 int KG3DRepresentObjectTemplate::SetFadeOutRate(
     unsigned nIndex, const DWORD dwFadeOutRate
 )
 {
     if (nIndex >= m_vecObjectProperty.size())
     {
         return FALSE;
     }

     m_vecObjectProperty[nIndex].dwFadeOutRate = dwFadeOutRate;

     return TRUE;
 }

 int KG3DRepresentObjectTemplate::SetFadeInRate(
     unsigned nIndex, const DWORD dFadeInRate
 )
 {
     if (nIndex >= m_vecObjectProperty.size())
     {
         return FALSE;
     }

     m_vecObjectProperty[nIndex].dwFadeInRate = dFadeInRate;

     return TRUE;
 }

 int KG3DRepresentObjectTemplate::SetAffectArea(
     unsigned nIndex, const DWORD dwAffectArea
 )
 {
     if (nIndex >= m_vecObjectProperty.size())
     {
         return FALSE;
     }

     m_vecObjectProperty[nIndex].dwAffectArea = dwAffectArea;

     return TRUE;
 }

 int KG3DRepresentObjectTemplate::SetReserved(
     unsigned nIndex, const DWORD dwReserved
 )
 {
     if (nIndex >= m_vecObjectProperty.size())
     {
         return FALSE;
     }

     m_vecObjectProperty[nIndex].dwReserved = dwReserved;

     return TRUE;
 }

 ///////////////////////////////////////////////////////////////////////////////
 //  物件属性设置 :  状态相关
 ///////////////////////////////////////////////////////////////////////////////

 int KG3DRepresentObjectTemplate::SetLife(unsigned nIndex, const float fLife)
 {
     if (nIndex >= m_vecObjectProperty.size())
     {
         return FALSE;
     }
    
     m_vecObjectProperty[nIndex].fLife = fLife;

     return TRUE;
 }

 int KG3DRepresentObjectTemplate::SetLifeReplenish(
     unsigned nIndex, const float fLifeReplenish
 )
 {
     if (nIndex >= m_vecObjectProperty.size())
     {
         return FALSE;
     }

     m_vecObjectProperty[nIndex].fLifeReplenish = fLifeReplenish;

     return TRUE;
 }

 int KG3DRepresentObjectTemplate::SetInsaideDefence(
     unsigned nIndex, const float fInsaideDefence
 )
 {
     if (nIndex >= m_vecObjectProperty.size())
     {
         return FALSE;
     }

     m_vecObjectProperty[nIndex].fInsaideDefence = fInsaideDefence;

     return TRUE;
 }

 int KG3DRepresentObjectTemplate::SetOutsaideDefence(
     unsigned nIndex, const float fOutsaideDefence
 )
 {
     if (nIndex >= m_vecObjectProperty.size())
     {
         return FALSE;
     }

     m_vecObjectProperty[nIndex].fOutsaideDefence = fOutsaideDefence;

     return TRUE;
 }

 int KG3DRepresentObjectTemplate::SetAttackable(
     unsigned nIndex, const bool bAttackable
 )       
 {
     if (nIndex >= m_vecObjectProperty.size())
     {
         return FALSE;
     }

     m_vecObjectProperty[nIndex].bAttackable = bAttackable;

     return TRUE;
 }

 int KG3DRepresentObjectTemplate::SetSelectable(
     unsigned nIndex, const bool bSelectable
 )
 {
     if (nIndex >= m_vecObjectProperty.size())
     {
         return FALSE;
     }

     m_vecObjectProperty[nIndex].bSelectable = bSelectable;

     return TRUE;
 }

 int KG3DRepresentObjectTemplate::SetUnitWalkable(
     unsigned nIndex, const bool bUnitWalkable
 )
 {
     if (nIndex >= m_vecObjectProperty.size())
     {
         return FALSE;
     }

     m_vecObjectProperty[nIndex].bUnitWalkable = bUnitWalkable;

     return TRUE;
 }

 int KG3DRepresentObjectTemplate::SetUnitCanPass(
     unsigned nIndex, const bool bUnitCanPass
 )
 {
     if (nIndex >= m_vecObjectProperty.size())
     {
         return FALSE;
     }

     m_vecObjectProperty[nIndex].bUnitCanPass = bUnitCanPass;

     return TRUE;
 }

 int KG3DRepresentObjectTemplate::SetBulletWalkable(
     unsigned nIndex, const bool bBulletWalkable
 )
 {
     if (nIndex >= m_vecObjectProperty.size())
     {
         return FALSE;
     }

     m_vecObjectProperty[nIndex].bBulletWalkable = bBulletWalkable;

     return TRUE;
 }

 int KG3DRepresentObjectTemplate::SetBulletCanPass(
     unsigned nIndex, const bool bBulletCanPass
 )
 {
     if (nIndex >= m_vecObjectProperty.size())
     {
         return FALSE;
     }

     m_vecObjectProperty[nIndex].bBulletCanPass = bBulletCanPass;

     return TRUE;
 }

 int KG3DRepresentObjectTemplate::SetAutoPathing(
     unsigned nIndex, const bool bAutoPathing
 )
 {
     if (nIndex >= m_vecObjectProperty.size())
     {
         return FALSE;
     }

     m_vecObjectProperty[nIndex].bAutoPathing = bAutoPathing;

     return TRUE;
 }

 int KG3DRepresentObjectTemplate::SetDropOnDeath(
     unsigned nIndex, const bool bDropOnDeath
 )
 {
     if (nIndex >= m_vecObjectProperty.size())
     {
         return FALSE;
     }

     m_vecObjectProperty[nIndex].bDropOnDeath = bDropOnDeath;

     return TRUE;
 }

 int KG3DRepresentObjectTemplate::SetPathingType(
     unsigned nIndex, const int nPathingType
 )
 {
     if (nIndex >= m_vecObjectProperty.size())
     {
         return FALSE;
     }

     m_vecObjectProperty[nIndex].nPathingType = nPathingType;

     return TRUE;
 }

 int KG3DRepresentObjectTemplate::SetPathingHeight(
     unsigned nIndex, const float fPathingHeight
 )
 {
     if (nIndex >= m_vecObjectProperty.size())
     {
         return FALSE;
     }

     m_vecObjectProperty[nIndex].fPathingHeight = fPathingHeight;

     return TRUE;
 }

 int KG3DRepresentObjectTemplate::SetDeathType(
     unsigned nIndex, const int nDeathType
 )
 {
     if (nIndex >= m_vecObjectProperty.size())
     {
         return FALSE;
     }

     m_vecObjectProperty[nIndex].nDeathType = nDeathType;

     return TRUE;
 }

 int KG3DRepresentObjectTemplate::SetReviveDelay(
     unsigned nIndex, const float fReviveDelay
 )
 {
     if (nIndex >= m_vecObjectProperty.size())
     {
         return FALSE;
     }

     m_vecObjectProperty[nIndex].fReviveDelay = fReviveDelay;

     return TRUE;
 }

 int KG3DRepresentObjectTemplate::SetReviveTime(
     unsigned nIndex, const float fReviveTime
 )
 {
     if (nIndex >= m_vecObjectProperty.size())
     {
         return FALSE;
     }

     m_vecObjectProperty[nIndex].fReviveTime = fReviveTime;

     return TRUE;
 }

 int KG3DRepresentObjectTemplate::SetDropNumber(
     unsigned nIndex, const int nDropNumber
 )
 {
     if (nIndex >= m_vecObjectProperty.size())
     {
         return FALSE;
     }

     m_vecObjectProperty[nIndex].nDropNumber = nDropNumber;

     return TRUE;
 }

 int KG3DRepresentObjectTemplate::SetDropTable(
     unsigned nIndex, int nTableLen, const OBJECT_PDROP_INFO DropTable[]
 )
 {
     if (
         nIndex    >= m_vecObjectProperty.size() || 
         nTableLen != m_vecObjectProperty[nIndex].nDropNumber
     )
     {
         return FALSE;
     }

     for (int i = 0; i <  nTableLen; i++)
     {
         m_vecObjectProperty[nIndex].DropTable[i].nID     = DropTable[i].nID;
         m_vecObjectProperty[nIndex].DropTable[i].nNumber = DropTable[i].nNumber;
     }

     return TRUE;
 }

 ///////////////////////////////////////////////////////////////////////////////
 //  物件属性设置 : 动作相关
 ///////////////////////////////////////////////////////////////////////////////

 int KG3DRepresentObjectTemplate::SetDeathFile1(
     unsigned nIndex, const char pcszFileName[]
 )
 {
     if (nIndex >= m_vecObjectProperty.size())
     {
         return FALSE;
     }

     strncpy(
         m_vecObjectProperty[nIndex].szDeathFile1, pcszFileName,
         sizeof(m_vecObjectProperty[nIndex].szDeathFile1)
     );
     m_vecObjectProperty[nIndex].szDeathFile1[
         sizeof(m_vecObjectProperty[nIndex].szDeathFile1) - 1
     ] = '\0';

     return TRUE;
 }

 int KG3DRepresentObjectTemplate::SetDeathTime1(
     unsigned nIndex, const DWORD dwTime
 )
 {
     if (nIndex >= m_vecObjectProperty.size())
     {
         return FALSE;
     }

     m_vecObjectProperty[nIndex].dwDeathTime1 = dwTime;

     return TRUE;
 }

 int KG3DRepresentObjectTemplate::SetDeathFile2(
     unsigned nIndex, const char pcszFileName[]
 )
 {
     if (nIndex >= m_vecObjectProperty.size())
     {
         return FALSE;
     }

     strncpy(
         m_vecObjectProperty[nIndex].szDeathFile2, pcszFileName,
         sizeof(m_vecObjectProperty[nIndex].szDeathFile2)
     );
     m_vecObjectProperty[nIndex].szDeathFile2[
         sizeof(m_vecObjectProperty[nIndex].szDeathFile2) - 1
     ] = '\0';

     return TRUE;
 }

 int KG3DRepresentObjectTemplate::SetDeathTime2(
     unsigned nIndex, const DWORD dwTime
 )
 {
     if (nIndex >= m_vecObjectProperty.size())
     {
         return FALSE;
     }

     m_vecObjectProperty[nIndex].dwDeathTime2 = dwTime;

     return TRUE;
 }

 int KG3DRepresentObjectTemplate::SetReserveFile1(
     unsigned nIndex, const char pcszFileName[]
 )
 {
     if (nIndex >= m_vecObjectProperty.size())
     {
         return FALSE;
     }

     strncpy(
         m_vecObjectProperty[nIndex].szReserveFile1, pcszFileName,
         sizeof(m_vecObjectProperty[nIndex].szReserveFile1)
     );
     m_vecObjectProperty[nIndex].szReserveFile1[
         sizeof(m_vecObjectProperty[nIndex].szReserveFile1) - 1
     ] = '\0';

     return TRUE;
 }

 int KG3DRepresentObjectTemplate::SetReserveTime1(
     unsigned nIndex, const DWORD dwTime
 )
 {
     if (nIndex >= m_vecObjectProperty.size())
     {
         return FALSE;
     }

     m_vecObjectProperty[nIndex].dwReserveTime1 = dwTime;

     return TRUE;
 }

 int KG3DRepresentObjectTemplate::SetReserveFile2(
     unsigned nIndex, const char pcszFileName[]
 )
 {
     if (nIndex >= m_vecObjectProperty.size())
     {
         return FALSE;
     }

     strncpy(
         m_vecObjectProperty[nIndex].szReserveFile2, pcszFileName,
         sizeof(m_vecObjectProperty[nIndex].szReserveFile2)
     );
     m_vecObjectProperty[nIndex].szReserveFile2[
         sizeof(m_vecObjectProperty[nIndex].szReserveFile2) - 1
     ] = '\0';

     return TRUE;
 }

 int KG3DRepresentObjectTemplate::SetReserveTime2(
     unsigned nIndex, const DWORD dwTime
 )
 {
     if (nIndex >= m_vecObjectProperty.size())
     {
         return FALSE;
     }

     m_vecObjectProperty[nIndex].dwReserveTime2 = dwTime;

     return TRUE;
 }

 int KG3DRepresentObjectTemplate::SetReserveFile3(
     unsigned nIndex, const char pcszFileName[]
 )
 {
     if (nIndex >= m_vecObjectProperty.size())
     {
         return FALSE;
     }

     strncpy(
         m_vecObjectProperty[nIndex].szReserveFile3, pcszFileName,
         sizeof(m_vecObjectProperty[nIndex].szReserveFile3)
     );
     m_vecObjectProperty[nIndex].szReserveFile3[
         sizeof(m_vecObjectProperty[nIndex].szReserveFile3) - 1
     ] = '\0';

     return TRUE;
 }

 int KG3DRepresentObjectTemplate::SetReserveTime3(
     unsigned nIndex, const DWORD dwTime
 )
 {
     if (nIndex >= m_vecObjectProperty.size())
     {
         return FALSE;
     }

     m_vecObjectProperty[nIndex].dwReserveTime3 = dwTime;

     return TRUE;
 }

 int KG3DRepresentObjectTemplate::SetReserveFile4(
     unsigned nIndex, const char pcszFileName[]
 )
 {
     if (nIndex >= m_vecObjectProperty.size())
     {
         return FALSE;
     }

     strncpy(
         m_vecObjectProperty[nIndex].szReserveFile4, pcszFileName,
         sizeof(m_vecObjectProperty[nIndex].szReserveFile4)
     );
     m_vecObjectProperty[nIndex].szReserveFile4[
         sizeof(m_vecObjectProperty[nIndex].szReserveFile4) - 1
     ] = '\0';

     return TRUE;
 }

 int KG3DRepresentObjectTemplate::SetReserveTime4(
     unsigned nIndex, const DWORD dwTime
 )
 {
     if (nIndex >= m_vecObjectProperty.size())
     {
         return FALSE;
     }

     m_vecObjectProperty[nIndex].dwReserveTime4 = dwTime;

     return TRUE;
 }

 int KG3DRepresentObjectTemplate::SetReserveFile5(
     unsigned nIndex, const char pcszFileName[]
 )
 {
     if (nIndex >= m_vecObjectProperty.size())
     {
         return FALSE;
     }

     strncpy(
         m_vecObjectProperty[nIndex].szReserveFile5, pcszFileName,
         sizeof(m_vecObjectProperty[nIndex].szReserveFile5)
     );
     m_vecObjectProperty[nIndex].szReserveFile5[
         sizeof(m_vecObjectProperty[nIndex].szReserveFile5) - 1
     ] = '\0';

     return TRUE;
 }

 int KG3DRepresentObjectTemplate::SetReserveTime5(
     unsigned nIndex, const DWORD dwTime
 )
 {
     if (nIndex >= m_vecObjectProperty.size())
     {
         return FALSE;
     }

     m_vecObjectProperty[nIndex].dwReserveTime5 = dwTime;

     return TRUE;
 }

 int KG3DRepresentObjectTemplate::SetReserveFile6(
     unsigned nIndex, const char pcszFileName[]
 )
 {
     if (nIndex >= m_vecObjectProperty.size())
     {
         return FALSE;
     }

     strncpy(
         m_vecObjectProperty[nIndex].szReserveFile6, pcszFileName,
         sizeof(m_vecObjectProperty[nIndex].szReserveFile6)
         );
     m_vecObjectProperty[nIndex].szReserveFile6[
         sizeof(m_vecObjectProperty[nIndex].szReserveFile6) - 1
     ] = '\0';

     return TRUE;
 }

 int KG3DRepresentObjectTemplate::SetReserveTime6(
     unsigned nIndex, const DWORD dwTime
 )
 {
     if (nIndex >= m_vecObjectProperty.size())
     {
         return FALSE;
     }

     m_vecObjectProperty[nIndex].dwReserveTime6 = dwTime;

     return TRUE;
 }

 ///////////////////////////////////////////////////////////////////////////////
 //  物件属性设置 : 技能相关
 ///////////////////////////////////////////////////////////////////////////////
 
 int KG3DRepresentObjectTemplate::SetnAbilitiesID(
     unsigned nIndex, const int nAbilitiesID
 )
 {
     if (nIndex >= m_vecObjectProperty.size())
     {
         return FALSE;
     }

     m_vecObjectProperty[nIndex].nAbilitiesID = nAbilitiesID;

     return TRUE;
    
 }

 ///////////////////////////////////////////////////////////////////////////////
 //  物件属性设置 : 文字显示相关
 ///////////////////////////////////////////////////////////////////////////////
 
 int KG3DRepresentObjectTemplate::SetName(unsigned nIndex, const char pcszText[])
 {
     if (nIndex >= m_vecObjectProperty.size())
     {
         return FALSE;
     }

     strncpy(
         m_vecObjectProperty[nIndex].szName, pcszText,
         sizeof(m_vecObjectProperty[nIndex].szName)
     );
     m_vecObjectProperty[nIndex].szName[
         sizeof(m_vecObjectProperty[nIndex].szName) - 1
     ] = '\0';

     return TRUE;
 }

 int KG3DRepresentObjectTemplate::SetSuffix(unsigned nIndex, const char pcszText[])
 {
     if (nIndex >= m_vecObjectProperty.size())
     {
         return FALSE;
     }

     strncpy(
         m_vecObjectProperty[nIndex].szSuffix, pcszText,
         sizeof(m_vecObjectProperty[nIndex].szSuffix)
     );
     m_vecObjectProperty[nIndex].szSuffix[
         sizeof(m_vecObjectProperty[nIndex].szSuffix) - 1
     ] = '\0';

     return TRUE;
 }

 int KG3DRepresentObjectTemplate::SetNote(unsigned nIndex, const char pcszText[])
 {
     if (nIndex >= m_vecObjectProperty.size())
     {
         return FALSE;
     }

     strncpy(
         m_vecObjectProperty[nIndex].szNote, pcszText,
         sizeof(m_vecObjectProperty[nIndex].szNote)
     );
     m_vecObjectProperty[nIndex].szNote[
         sizeof(m_vecObjectProperty[nIndex].szNote) - 1
     ] = '\0';

     return TRUE;
 }
*/