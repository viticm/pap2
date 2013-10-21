////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : IKG3DObjectPropertyModifior.h
//  Version     : 1.0
//  Author      : ZhaoXinyu (Chiyer)
//  CopyRight   : Kingsoft Corporation All rights Reserved
//  History     : 2005-10-10 16:21:30
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _IKG3DOBJECTPROPERTYMODIFIOR_H_ 
#define _IKG3DOBJECTPROPERTYMODIFIOR_H_

#include "KG3DTypes.h"

class IKG3DObjectPropertyModifior
{
public:
    // 基础属性 ////////////////////////////////////////////////////////////////
    virtual char* GetObjectFileName(unsigned nIndex){return NULL;}

    // 显示相关 

    virtual int SetModelFile(unsigned nIndex, const char pcszFileName[]){return true;}
    virtual int SetReplaceableTextureFile(unsigned nIndex, const char pcszFileName[]){return true;}
    virtual int SetSpecial(unsigned nIndex, const char pcszFileName[]){return true;}
    virtual int SetMinimapText(unsigned nIndex, const char pcszText[]){return true;}

    virtual int SetTintingColor(unsigned nIndex, const DWORD dwColor){return true;}
    virtual int SetChangeColor(unsigned nIndex, const DWORD dwColor){return true;}
    virtual int SetShadowColor(unsigned nIndex, const DWORD dwColor){return true;}
    virtual int SetMinimapShowColor(unsigned nIndex, const DWORD dwColor){return true;}

    virtual int SetDecayTime(unsigned nIndex, const DWORD dwTime){return true;}  
    virtual int SetElevationRadius(unsigned nIndex, const float fHight){return true;}
    virtual int SetInteractionSpecial(unsigned nIndex, const int nSpecial){return true;}
    virtual int SetOrder(unsigned nIndex, const int nOrder){return true;}
    virtual int SetVisabilityLevel(unsigned nIndex, const int nLevel){return true;}

    virtual int SetIfHighLight(unsigned nIndex, const bool bHightLight){return true;}
    virtual int SetIfAmbientLight(unsigned nIndex, const bool bAmbientLigth){return true;}
    virtual int SetIfShadow(unsigned nIndex, const bool bShadow){return true;}
    virtual int SetIfMinimapShow(unsigned nIndex, const bool bMinimapShow){return true;}
    virtual int SetIfMinimapShowText(unsigned nIndex, const bool bMinimapShowText){return true;}
    virtual int SetIfReplaceableTexture(unsigned nIndex, const bool bReplaceableTexture){return true;}
    virtual int SetIfTranslucence(unsigned nIndex, const bool bTranslucence){return true;}
    virtual int SetIfHighLightScene(unsigned nIndex, const bool bHighLightScene){return true;}

    // 声音相关
    virtual int SetRevive(unsigned nIndex, const char pcszFileName[]){return true;}
    virtual int SetDeath(unsigned nIndex, const char pcszFileName[]){return true;}
    virtual int SetMove(unsigned nIndex, const char pcszFileName[]){return true;}
    virtual int SetLooping(unsigned nIndex, const char pcszFileName[]){return true;}

    virtual int SetLoopingTime(unsigned nIndex, const DWORD dwTime){return true;}
    virtual int SetFadeOutRate(unsigned nIndex, const DWORD dwFadeOutRate){return true;}                           
    virtual int SetFadeInRate(unsigned nIndex, const DWORD dFadeInRate){return true;}                            
    virtual int SetAffectArea(unsigned nIndex, const DWORD dwAffectArea){return true;}                            
    virtual int SetReserved(unsigned nIndex, const DWORD dwReserved){return true;}   

    // 状态相关
    virtual int SetLife(unsigned nIndex, const float fLife){return true;}
    virtual int SetLifeReplenish(unsigned nIndex, const float fLifeReplenish){return true;}                          
    virtual int SetInsaideDefence(unsigned nIndex, const float fInsaideDefence){return true;}                        
    virtual int SetOutsaideDefence(unsigned nIndex, const float fOutsaideDefence){return true;}  

    virtual int SetAttackable(unsigned nIndex, const bool bAttackable){return true;}                             // 可否被攻击
    virtual int SetSelectable(unsigned nIndex, const bool bSelectable){return true;}                         
    virtual int SetUnitWalkable(unsigned nIndex, const bool bUnitWalkable){return true;}         
    virtual int SetUnitCanPass(unsigned nIndex, const bool bUnitCanPass){return true;}   
    virtual int SetBulletWalkable(unsigned nIndex, const bool bBulletWalkable){return true;}  
    virtual int SetBulletCanPass(unsigned nIndex, const bool bBulletCanPass){return true;}
    virtual int SetAutoPathing(unsigned nIndex, const bool bAutoPathing){return true;}
    virtual int SetDropOnDeath(unsigned nIndex, const bool bDropOnDeath){return true;}  

    virtual int SetPathingType(unsigned nIndex, const int nPathingType){return true;}
    virtual int SetPathingHeight(unsigned nIndex, const float fPathingHeight){return true;}
    virtual int SetDeathType(unsigned nIndex, const int nDeathType){return true;}
    virtual int SetReviveDelay(unsigned nIndex, const float fReviveDelay){return true;}
    virtual int SetReviveTime(unsigned nIndex, const float fReviveTime){return true;}                             
    virtual int SetDropNumber(unsigned nIndex, const int nDropNumber){return true;}   
    virtual int SetDropTable(
        unsigned nIndex, int nTableLen, const OBJECT_PDROP_INFO DropTable[]
    ){return true;};  

    // 动作相关
    virtual int SetDeathFile1(unsigned nIndex, const char pcszFileName[]){return true;}
    virtual int SetDeathTime1(unsigned nIndex, const DWORD dwTime){return true;}                              
    virtual int SetDeathFile2(unsigned nIndex, const char pcszFileName[]){return true;}                     
    virtual int SetDeathTime2(unsigned nIndex, const DWORD dwTime){return true;}                                

    virtual int SetReserveFile1(unsigned nIndex, const char pcszFileName[]){return true;}                  
    virtual int SetReserveTime1(unsigned nIndex, const DWORD dwTime){return true;}                            
    virtual int SetReserveFile2(unsigned nIndex, const char pcszFileName[]){return true;}                    
    virtual int SetReserveTime2(unsigned nIndex, const DWORD dwTime){return true;}                                 
    virtual int SetReserveFile3(unsigned nIndex, const char pcszFileName[]){return true;}                   
    virtual int SetReserveTime3(unsigned nIndex, const DWORD dwTime){return true;}                             
    virtual int SetReserveFile4(unsigned nIndex, const char pcszFileName[]){return true;}                      
    virtual int SetReserveTime4(unsigned nIndex, const DWORD dwTime){return true;}                               
    virtual int SetReserveFile5(unsigned nIndex, const char pcszFileName[]){return true;}                    
    virtual int SetReserveTime5(unsigned nIndex, const DWORD dwTime){return true;}                               
    virtual int SetReserveFile6(unsigned nIndex, const char pcszFileName[]){return true;}                    
    virtual int SetReserveTime6(unsigned nIndex, const DWORD dwTime){return true;}    

    // 技能相关
    virtual int SetnAbilitiesID(unsigned nIndex, const int nAbilitiesID){return true;}

    // 文字显示相关
    virtual int SetName(unsigned nIndex, const char pcszText[]){return true;}     
    virtual int SetSuffix(unsigned nIndex, const char pcszText[]){return true;}  
    virtual int SetNote(unsigned nIndex, const char pcszText[]){return true;}  
    virtual int SetType(unsigned nIndex, const int nType){return true;}
public:
    IKG3DObjectPropertyModifior(){}
    ~IKG3DObjectPropertyModifior(){}

};

#endif // _IKG3DOBJECTPROPERTYMODIFIOR_H_