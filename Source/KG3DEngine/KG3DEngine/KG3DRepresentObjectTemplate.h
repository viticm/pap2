////////////////////////////////////////////////////////////////////////////////
//
//  FileName  : KG3DRepresentObjectTemplate.h
//  Author    : ZhaoXinyu 
//  CopyRight : Kingsoft Corporation All rights Reserved
//  History   : 2005.9.21 0:36 Create
//  Comment   :
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _KG3DREPRESENTOBJECTTEMPLATE_H_
#define _KG3DREPRESENTOBJECTTEMPLATE_H_
#pragma once

#include "KG3DRepresetObjectPropertyModifior.h"
#include "KG3DObjectPropertyDefine.h"

// 物件属性 : 全基础属性
struct OBJECT_PROPERTY : public OBJECT_PROPERTY_DISPLAY, 
                         public OBJECT_PROPERTY_SOUND,
                         public OBJECT_PROPERTY_STATES,
                         public OBJECT_PROPERTY_ACTION,
                         public OBJECT_PROPERTY_ABILITIES,
                         public OBJECT_PROPERTY_TEXT
{
    OBJECT_PROPERTY()
    {
        OBJECT_PROPERTY_DISPLAY();
        OBJECT_PROPERTY_SOUND();
        OBJECT_PROPERTY_STATES();
        OBJECT_PROPERTY_ACTION();
        OBJECT_PROPERTY_ABILITIES();
        OBJECT_PROPERTY_TEXT();
    }
};

class KG3DRepresentObjectTemplate :
    public KG3DRepresetObjectPropertyModifior<OBJECT_PROPERTY>
{
public:
     virtual int LoadFromIniFile(const char pcszFileName[]);
    // 显示相关 
    /*   
    int SetModelFile(unsigned nIndex, const char pcszFileName[]);
    int SetReplaceableTextureFile(unsigned nIndex, const char pcszFileName[]);
    int SetSpecial(unsigned nIndex, const char pcszFileName[]);
    int SetMinimapText(unsigned nIndex, const char pcszText[]);

    int SetTintingColor(unsigned nIndex, const DWORD dwColor);
    int SetChangeColor(unsigned nIndex, const DWORD dwColor);
    int SetShadowColor(unsigned nIndex, const DWORD dwColor);
    int SetMinimapShowColor(unsigned nIndex, const DWORD dwColor);

    int SetDecayTime(unsigned nIndex, const DWORD dwTime);  
    int SetElevationRadius(unsigned nIndex, const float fHight);
    int SetInteractionSpecial(unsigned nIndex, const int nSpecial);
    int SetOrder(unsigned nIndex, const int nOrder);
    int SetVisabilityLevel(unsigned nIndex, const int nLevel);

    int SetIfHighLight(unsigned nIndex, const bool bHightLight);
    int SetIfAmbientLight(unsigned nIndex, const bool bAmbientLigth);
    int SetIfShadow(unsigned nIndex, const bool bShadow);
    int SetIfMinimapShow(unsigned nIndex, const bool bMinimapShow);
    int SetIfMinimapShowText(unsigned nIndex, const bool bMinimapShowText);
    int SetIfReplaceableTexture(unsigned nIndex, const bool bReplaceableTexture);
    int SetIfTranslucence(unsigned nIndex, const bool bTranslucence);
    int SetIfHighLightScene(unsigned nIndex, const bool bHighLightScene);

    // 声音相关
    int SetRevive(unsigned nIndex, const char pcszFileName[]);
    int SetDeath(unsigned nIndex, const char pcszFileName[]);
    int SetMove(unsigned nIndex, const char pcszFileName[]);
    int SetLooping(unsigned nIndex, const char pcszFileName[]);

    int SetLoopingTime(unsigned nIndex, const DWORD dwTime);
    int SetFadeOutRate(unsigned nIndex, const DWORD dwFadeOutRate);                           
    int SetFadeInRate(unsigned nIndex, const DWORD dFadeInRate);                            
    int SetAffectArea(unsigned nIndex, const DWORD dwAffectArea);                            
    int SetReserved(unsigned nIndex, const DWORD dwReserved);   

    // 状态相关
    int SetLife(unsigned nIndex, const float fLife);
    int SetLifeReplenish(unsigned nIndex, const float fLifeReplenish);                          
    int SetInsaideDefence(unsigned nIndex, const float fInsaideDefence);                        
    int SetOutsaideDefence(unsigned nIndex, const float fOutsaideDefence);  

    int SetAttackable(unsigned nIndex, const bool bAttackable);                             // 可否被攻击
    int SetSelectable(unsigned nIndex, const bool bSelectable);                         
    int SetUnitWalkable(unsigned nIndex, const bool bUnitWalkable);         
    int SetUnitCanPass(unsigned nIndex, const bool bUnitCanPass);   
    int SetBulletWalkable(unsigned nIndex, const bool bBulletWalkable);  
    int SetBulletCanPass(unsigned nIndex, const bool bBulletCanPass);
    int SetAutoPathing(unsigned nIndex, const bool bAutoPathing);
    int SetDropOnDeath(unsigned nIndex, const bool bDropOnDeath);  

    int SetPathingType(unsigned nIndex, const int nPathingType);
    int SetPathingHeight(unsigned nIndex, const float fPathingHeight);
    int SetDeathType(unsigned nIndex, const int nDeathType);
    int SetReviveDelay(unsigned nIndex, const float fReviveDelay);
    int SetReviveTime(unsigned nIndex, const float fReviveTime);                             
    int SetDropNumber(unsigned nIndex, const int nDropNumber);   
    int SetDropTable(
        unsigned nIndex, int nTableLen, const OBJECT_PDROP_INFO DropTable[]
    );  

    // 动作相关
    int SetDeathFile1(unsigned nIndex, const char pcszFileName[]);
    int SetDeathTime1(unsigned nIndex, const DWORD dwTime);                              
    int SetDeathFile2(unsigned nIndex, const char pcszFileName[]);                     
    int SetDeathTime2(unsigned nIndex, const DWORD dwTime);                                
   
    int SetReserveFile1(unsigned nIndex, const char pcszFileName[]);                  
    int SetReserveTime1(unsigned nIndex, const DWORD dwTime);                            
    int SetReserveFile2(unsigned nIndex, const char pcszFileName[]);                    
    int SetReserveTime2(unsigned nIndex, const DWORD dwTime);                                 
    int SetReserveFile3(unsigned nIndex, const char pcszFileName[]);                   
    int SetReserveTime3(unsigned nIndex, const DWORD dwTime);                             
    int SetReserveFile4(unsigned nIndex, const char pcszFileName[]);                      
    int SetReserveTime4(unsigned nIndex, const DWORD dwTime);                               
    int SetReserveFile5(unsigned nIndex, const char pcszFileName[]);                    
    int SetReserveTime5(unsigned nIndex, const DWORD dwTime);                               
    int SetReserveFile6(unsigned nIndex, const char pcszFileName[]);                    
    int SetReserveTime6(unsigned nIndex, const DWORD dwTime);    

    // 技能相关
    int SetnAbilitiesID(unsigned nIndex, const int nAbilitiesID);

    // 文字显示相关
    int SetName(unsigned nIndex, const char pcszText[]);     
    int SetSuffix(unsigned nIndex, const char pcszText[]);  
    int SetNote(unsigned nIndex, const char pcszText[]);  
*/
public:
	KG3DRepresentObjectTemplate(void);
	virtual ~KG3DRepresentObjectTemplate(void);
/*
public:
    vector<OBJECT_PROPERTY> m_vecObjectProperty;
    */
};

extern KG3DRepresentObjectTemplate g_cObjectTemplate;    

#endif //_KG3DREPRESENTOBJECTTEMPLATE_H_