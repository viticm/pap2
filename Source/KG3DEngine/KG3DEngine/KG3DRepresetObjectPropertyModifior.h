////////////////////////////////////////////////////////////////////////////////
//
//  FileName  : KG3DRepresetObjectPropertyModifior.h
//  Author    : ZhaoXinyu (Chiyer)
//  CopyRight : Kingsoft Corporation All rights Reserved
//  History   : 2005.9.23 12:06 Create
//  Comment   : Object Property Modifiy Template
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _KG3DREPRESETOBJECTPROPERTYMODIFIOR_H_
#define _KG3DREPRESETOBJECTPROPERTYMODIFIOR_H_
#pragma once

//#include "stdafx.h"
//#include "FileType.h"
#include "KG3DObjectPropertyDefine.h"
#include "IKG3DObjectPropertyModifior.h"

extern TCHAR g_Def_WorkDirectory[256];//执行文件目录

template <class TResource>
class KG3DRepresetObjectPropertyModifior : public IKG3DObjectPropertyModifior
{
    friend TResource;

public:
    virtual char* GetObjectFileName(unsigned nIndex)
    {
        return m_vecObjectProperty[nIndex]->szFile;
    }

    virtual int LoadFromIniFile(const char pcszFileName[])
    {
        char szFileName[MAX_PATH];

		ASSERT(FALSE);
		sprintf(NULL, "test");	// for must error when release
				
		snprintf( szFileName, sizeof(szFileName), "%s", &pcszFileName[1] );
        IIniFile* pFile = g_OpenIniFile(szFileName);
        
        if (!pFile)
        {
            return false;
        }
        
        TResource* lpResource = new TResource();
       
        //TCHAR szWorkDirectory[256];  
        char szTemp[MAX_TEXT];
        //char szPath[MAX_PATH];
        int pColor[4];
        int nBool;

        // 标示相关

        strncpy(
            lpResource->szFile, szFileName, sizeof(lpResource->szFile)
        );
        lpResource->szFile[sizeof( lpResource->szFile) - 1] = '\0';

        pFile->GetString(
            "caption", "szName",
            lpResource->szName , 
            lpResource->szName , 
            MAX_TEXT
        );

        pFile->GetString(
            "caption", "szSuffix", 
            lpResource->szSuffix , 
            lpResource->szSuffix , 
            MAX_TEXT
        );

        pFile->GetString(
            "caption", "szNote", 
            lpResource->szNote , 
            lpResource->szNote ,
            MAX_TEXT
        );

        pFile->GetString(
            "caption", "nType", 
            "Default",  szTemp,
            MAX_TEXT
        );

        if (_strnicmp(szTemp, "NPC", sizeof(szTemp)) == 0)
        {
            lpResource->nType = REPRESENTOBJECT_NPC; 
        }
        else if (_strnicmp(szTemp, "道具", sizeof(szTemp)) == 0)
        {
            lpResource->nType = REPRESENTOBJECT_PROP; 
        }
        else if (_strnicmp(szTemp, "魔法", sizeof(szTemp)) == 0)
        {
            lpResource->nType = REPRESENTOBJECT_MAGIC; 
        }
        else if (_strnicmp(szTemp, "特效", sizeof(szTemp)) == 0)
        {
            lpResource->nType = REPRESENTOBJECT_EFFECT; 
        }
        else if (_strnicmp(szTemp, "技能", sizeof(szTemp)) == 0)
        {
            lpResource->nType = REPRESENTOBJECT_SKILL; 
        }
        else 
        {
            lpResource->nType = REPRESENTOBJECT_DEFAULT; 
        }

        // 显示相关

        pFile->GetString(
            "display", "szModelFile", 
            lpResource->szModelFile , 
            lpResource->szModelFile ,
            //szPath, szPath,
            MAX_PATH
        );
        //strncpy(szWorkDirectory, g_Work_Directory, sizeof(szWorkDirectory));
        //strncat(szWorkDirectory, szPath)

        pFile->GetString(
            "display", "szReplaceableTextureFile", 
            lpResource->szReplaceableTextureFile , 
            lpResource->szReplaceableTextureFile , 
            MAX_PATH
        );

        pFile->GetString(
            "display", "szSpecial", 
            lpResource->szSpecial , 
            lpResource->szSpecial ,
            MAX_PATH
        );

        pFile->GetString(
            "display", "szMinimapText",
            lpResource->szMinimapText , 
            lpResource->szMinimapText ,
            MAX_TEXT
        );
        
        pFile->GetMultiInteger("display", "dwTintingColor", pColor, 4);
        lpResource->dwTintingColor = 
            D3DCOLOR_ARGB(pColor[0], pColor[1], pColor[2], pColor[3]);

        pFile->GetMultiInteger("display", "dwChangeColor", pColor, 4);
        lpResource->dwChangeColor = 
            D3DCOLOR_ARGB(pColor[0], pColor[1], pColor[2], pColor[3]);

        pFile->GetMultiInteger("display", "dwShadowColor", pColor, 4);
        lpResource->dwShadowColor = 
            D3DCOLOR_ARGB(pColor[0], pColor[1], pColor[2], pColor[3]);

        pFile->GetMultiInteger("display", "dwMinimapShowColor", pColor, 4);
        lpResource->dwMinimapShowColor = 
            D3DCOLOR_ARGB(pColor[0], pColor[1], pColor[2], pColor[3]);
       
        pFile->GetInteger(
            "display", "dwDecayTime", 
            lpResource->dwDecayTime , 
            &lpResource->dwDecayTime
         );

        pFile->GetFloat(
            "display", "fElevationRadius", 
            lpResource->fElevationRadius ,
            &lpResource->fElevationRadius
        );

        pFile->GetInteger(
            "display", "nInteractionSpecial", 
            lpResource->nInteractionSpecial , 
            &lpResource->nInteractionSpecial
        );

        pFile->GetInteger(
            "display", "nOrder", 
            lpResource->nOrder , 
            &lpResource->nOrder
        );

        pFile->GetInteger(
            "display", "nVisabilityLevel", 
            lpResource->nVisabilityLevel , 
            &lpResource->nVisabilityLevel
        );
    
        pFile->GetInteger("display", "bHighLight", 1, &nBool);
        nBool == 1 ? 
             lpResource->bHighLight = true :
             lpResource->bHighLight = false;

        pFile->GetInteger("display", "bAmbientLight", 1, &nBool);
        nBool == 1 ? 
            lpResource->bAmbientLight = true : 
            lpResource->bAmbientLight = false;

        pFile->GetInteger("display", "bShadow", 1, &nBool);
        nBool == 1 ? 
            lpResource->bShadow = true : 
            lpResource->bShadow = false;

        pFile->GetInteger("display", "bMinimapShow", 1, &nBool);
        nBool == 1 ? 
            lpResource->bMinimapShow = true : 
            lpResource->bMinimapShow = false;

        pFile->GetInteger("display", "bMinimapShowText", 1, &nBool);
        nBool == 1 ? 
            lpResource->bMinimapShowText = true : 
            lpResource->bMinimapShowText = false;

        pFile->GetInteger("display", "bReplaceableTexture", 1, &nBool);
        nBool == 1 ? 
            lpResource->bReplaceableTexture = true :
            lpResource->bReplaceableTexture = false;

        pFile->GetInteger("display", "bTranslucence", 1, &nBool);
        nBool == 1 ? 
            lpResource->bTranslucence = true :
            lpResource->bTranslucence = false;

        pFile->GetInteger("display", "bHighLightScene", 1, &nBool);
        nBool == 1 ? 
            lpResource->bHighLightScene = true : 
            lpResource->bHighLightScene = false;

        // 声音相关

        pFile->GetString(
            "sound", "szRevive" ,
            lpResource->szRevive, 
            lpResource->szRevive, 
            MAX_PATH
        );

        pFile->GetString(
            "sound", "szDeath" ,
            lpResource->szDeath, 
            lpResource->szDeath, 
            MAX_PATH
        );

        pFile->GetString(
            "sound", "szMove" ,
            lpResource->szMove, 
            lpResource->szMove, 
            MAX_PATH
        );

        pFile->GetString(
            "sound", "szLooping" ,
            lpResource->szLooping, 
            lpResource->szLooping, 
            MAX_PATH
        );

        pFile->GetInteger(
            "sound", "dwLoopingTime" , 
            lpResource->dwLoopingTime, 
            &lpResource->dwLoopingTime
        );

        pFile->GetInteger(
            "sound", "dwFadeOutRate" , 
            lpResource->dwFadeOutRate, 
            &lpResource->dwFadeOutRate
        );

        pFile->GetInteger(
            "sound", "dwFadeInRate" , 
            lpResource->dwFadeInRate, 
            &lpResource->dwFadeInRate
        );

        pFile->GetInteger(
            "sound", "dwAffectArea" , 
            lpResource->dwAffectArea, 
            &lpResource->dwAffectArea
        );


        // 状态相关
        pFile->GetFloat(
            "states", "fLife", 
            lpResource->fLife,
            &lpResource->fLife
        );

        pFile->GetFloat(
            "states", "fLifeReplenish", 
            lpResource->fLifeReplenish,
            &lpResource->fLifeReplenish
        );

        pFile->GetFloat(
            "states", "fInsaideDefence", 
            lpResource->fInsaideDefence,
            &lpResource->fInsaideDefence
        );

        pFile->GetFloat(
            "states", "fOutsaideDefence", 
            lpResource->fOutsaideDefence,
            &lpResource->fOutsaideDefence
        );

        pFile->GetBool("states", "bAttackable", &nBool);
        nBool == 1 ? 
            lpResource->bAttackable = true :
            lpResource->bAttackable = false;

        pFile->GetBool("states", "bSelectable", &nBool);
        nBool == 1 ? 
            lpResource->bSelectable = true :
            lpResource->bSelectable = false;

        pFile->GetBool("states", "bUnitWalkable", &nBool);
        nBool == 1 ? 
            lpResource->bUnitWalkable = true :
            lpResource->bUnitWalkable = false;

        pFile->GetBool("states", "bUnitCanPass", &nBool);
        nBool == 1 ? 
            lpResource->bUnitCanPass = true :
            lpResource->bUnitCanPass = false;

        pFile->GetBool("states", "bBulletWalkable", &nBool);
        nBool == 1 ? 
            lpResource->bBulletWalkable = true :
            lpResource->bBulletWalkable = false;

        pFile->GetBool("states", "bBulletCanPass", &nBool);
        nBool == 1 ? 
            lpResource->bBulletCanPass = true :
            lpResource->bBulletCanPass = false;

        pFile->GetBool("states", "bAutoPathing", &nBool);
        nBool == 1 ? 
            lpResource->bAutoPathing = true :
            lpResource->bAutoPathing = false;

        pFile->GetBool("states", "bDropOnDeath", &nBool);
        nBool == 1 ? 
            lpResource->bDropOnDeath = true :
            lpResource->bDropOnDeath = false;

        pFile->GetInteger(
            "states", "nPathingType", 
            lpResource->nPathingType, 
            &lpResource->nPathingType
         );

        pFile->GetFloat(
            "states", "fPathingHeight", 
            lpResource->fPathingHeight,
            &lpResource->fPathingHeight
        );

        pFile->GetInteger(
            "states", "nDeathType", 
            lpResource->nDeathType, 
            &lpResource->nDeathType
         );

        pFile->GetFloat(
            "states", "fReviveDelay", 
            lpResource->fReviveDelay,
            &lpResource->fReviveDelay
        );

         pFile->GetFloat(
            "states", "fReviveTime", 
            lpResource->fReviveTime, 
            &lpResource->fReviveTime
         );

        pFile->GetInteger(
            "states", "nDropNumber", 
            lpResource->nDropNumber,
            &lpResource->nDropNumber
        );

        // 动作相关
        
        // 技能相关
        
        // 加入到属性摸模版表
        m_vecObjectProperty.push_back(lpResource);

        pFile->Release();

        return true; 
    }

    virtual int IniProperTemplate()
    {
        IniProperTemplate(m_szConfigurationFilesName);
        return true;
    }

    virtual int IniProperTemplate(const char pcszRootPath[])
    {
        WIN32_FIND_DATA fd;
        memset(&fd, 0, sizeof(WIN32_FIND_DATA));

        HANDLE hSearch;
        char szFilePathName[MAX_PATH];
        char szTempPath[MAX_PATH];
        memset(szFilePathName, 0, sizeof(szFilePathName));
        memset(szTempPath, 0, sizeof(szTempPath));

        strncpy(szFilePathName, pcszRootPath, sizeof(szFilePathName));
        szFilePathName[sizeof(szFilePathName) - 1] = '\0';

        bool bSearchFinished = false;
        if(szFilePathName[strlen(szFilePathName) -1] != '\\')
        {
            strncat(
                szFilePathName, "\\", sizeof(szFilePathName) - strlen(szFilePathName)
            );
            szFilePathName[sizeof(szFilePathName) - 1] = '\0';
        }

        strncat(
            szFilePathName, "*", sizeof(szFilePathName) - strlen(szFilePathName)
        );
        szFilePathName[sizeof(szFilePathName) - 1] = '\0';

        hSearch = FindFirstFile(szFilePathName, &fd);

        if( 
            (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) &&
            strcmp(fd.cFileName, ".")  != 0 && 
            strcmp(fd.cFileName, "..") != 0 
        )       
        {
            strncpy(szTempPath, pcszRootPath, sizeof(szTempPath));
            strncat(
                szTempPath, fd.cFileName, sizeof(szTempPath) - strlen(szTempPath)
            );
            strncat(szTempPath, "\\", sizeof(szTempPath) - strlen(szTempPath));
            szTempPath[sizeof(szTempPath) - 1] = '\0';
            IniProperTemplate(szTempPath);
        }
        else if( 
            strcmp(fd.cFileName, ".")  != 0 &&
            strcmp(fd.cFileName, "..") != 0
        )
        {
            if (0 == _stricmp(&fd.cFileName[strlen(fd.cFileName) - 4], ".ini"))
            {
                char szTemp[MAX_PATH];
                strcpy(szTemp, pcszRootPath);
                strcat(szTemp, fd.cFileName);
                LoadFromIniFile(szTemp);
            }
        }

        while(!bSearchFinished)
        {
            if(FindNextFile(hSearch, &fd))
            {
                if(
                    (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) &&
                    strcmp(fd.cFileName, ".")  != 0 &&
                    strcmp(fd.cFileName, "..") != 0
                )       
                {
                    strncpy(szTempPath, pcszRootPath, sizeof(szTempPath));
                    strncat(
                        szTempPath, fd.cFileName, sizeof(szTempPath) - strlen(szTempPath)
                    );
                    strncat(szTempPath, "\\", sizeof(szTempPath) - strlen(szTempPath));
                    szTempPath[sizeof(szTempPath) - 1] = '\0';
                    IniProperTemplate(szTempPath);
                }
                else if(
                    strcmp(fd.cFileName, ".")  != 0 &&
                    strcmp(fd.cFileName, "..") != 0
                )
                {
                    if (0 == _stricmp(&fd.cFileName[strlen(fd.cFileName) - 4], ".ini"))
                    {
                        char szTemp[MAX_PATH];
                        strcpy(szTemp, pcszRootPath);
                        strcat(szTemp, fd.cFileName);
                        LoadFromIniFile(szTemp);
                    }
                }
            }
            else
            {
                if(GetLastError() == ERROR_NO_MORE_FILES)          
                {
                    bSearchFinished = true;
                }
                else
                {
                    bSearchFinished = true;                       
                }
            }
        }
        return true;
    }
    
    //  物件属性设置 : 显示相关
    int SetModelFile(unsigned nIndex, const char pcszFileName[])
    {
        if (nIndex >= m_vecObjectProperty.size())
        {
            return false;
        }

        strncpy(
            m_vecObjectProperty[nIndex]->szModelFile, pcszFileName,
            sizeof(m_vecObjectProperty[nIndex]->szModelFile)
        );
        m_vecObjectProperty[nIndex]->szModelFile[
            sizeof(m_vecObjectProperty[nIndex]->szModelFile) - 1
        ] = '\0';

        return true;
    }

    int SetReplaceableTextureFile(unsigned nIndex, const char pcszFileName[])
    {
        if (nIndex >= m_vecObjectProperty.size())
        {
            return false;
        }

        strncpy(
            m_vecObjectProperty[nIndex]->szReplaceableTextureFile, pcszFileName,
            sizeof(m_vecObjectProperty[nIndex]->szReplaceableTextureFile)
        );
        m_vecObjectProperty[nIndex]->szReplaceableTextureFile[
            sizeof(m_vecObjectProperty[nIndex]->szReplaceableTextureFile) - 1
        ] = '\0';

        return true;
    }

    int SetSpecial(unsigned nIndex, const char pcszFileName[])
    {
        if (nIndex >= m_vecObjectProperty.size())
        {
            return false;
        }

        strncpy(
            m_vecObjectProperty[nIndex]->szSpecial, pcszFileName,
            sizeof(m_vecObjectProperty[nIndex]->szSpecial)
        );
        m_vecObjectProperty[nIndex]->szSpecial[
            sizeof(m_vecObjectProperty[nIndex]->szSpecial) - 1
        ] = '\0';

        return true;
    }

    int SetMinimapText(unsigned nIndex, const char pcszText[])
    {
        if (nIndex >= m_vecObjectProperty.size())
        {
            return false;
        }

        strncpy(
            m_vecObjectProperty[nIndex]->szMinimapText, pcszText,
            sizeof(m_vecObjectProperty[nIndex]->szMinimapText)
        );
        m_vecObjectProperty[nIndex]->szMinimapText[
            sizeof(m_vecObjectProperty[nIndex]->szMinimapText) - 1
        ] = '\0';

        return true;
    }

    int SetTintingColor(unsigned nIndex, const DWORD dwColor)
    {
        if (nIndex >= m_vecObjectProperty.size())
        {
            return false;
        }

        m_vecObjectProperty[nIndex]->dwTintingColor = dwColor;

        return true;
    }

    int SetChangeColor(unsigned nIndex, const DWORD dwColor)
    {
        if (nIndex >= m_vecObjectProperty.size())
        {
            return false;
        }

        m_vecObjectProperty[nIndex]->dwChangeColor = dwColor;

        return true;
    }

    int SetShadowColor(unsigned nIndex, const DWORD dwColor)
    {
        if (nIndex >= m_vecObjectProperty.size())
        {
            return false;
        }

        m_vecObjectProperty[nIndex]->dwShadowColor = dwColor;

        return true;
    }

    int SetMinimapShowColor( unsigned nIndex, const DWORD dwColor)
    {
        if (nIndex >= m_vecObjectProperty.size())
        {
            return false;
        }

        m_vecObjectProperty[nIndex]->dwMinimapShowColor = dwColor;

        return true;
    }

    int SetDecayTime(unsigned nIndex, const int dwTime)
    {
        if (nIndex >= m_vecObjectProperty.size())
        {
            return false;
        }

        m_vecObjectProperty[nIndex]->dwDecayTime = dwTime;

        return true;
    }

    int SetElevationRadius(unsigned nIndex, const float fHight)
    {
        if (nIndex >= m_vecObjectProperty.size())
        {
            return false;
        }

        m_vecObjectProperty[nIndex]->fElevationRadius = fHight;

        return true;
    }

    int SetInteractionSpecial(unsigned nIndex, const int nSpecial)
    {
        if (nIndex >= m_vecObjectProperty.size())
        {
            return false;
        }

        m_vecObjectProperty[nIndex]->nInteractionSpecial = nSpecial;

        return true;
    }

    int SetOrder(unsigned nIndex, const int nOrder)
    {
        if (nIndex >= m_vecObjectProperty.size())
        {
            return false;
        }

        m_vecObjectProperty[nIndex]->nOrder = nOrder;

        return true;
    }

    int SetVisabilityLevel(unsigned nIndex, const int nLevel)
    {
        if (nIndex >= m_vecObjectProperty.size())
        {
            return false;
        }

        m_vecObjectProperty[nIndex]->nVisabilityLevel = nLevel;

        return true;
    }

    int SetIfHighLight(unsigned nIndex, const bool bHightLight)
    {
        if (nIndex >= m_vecObjectProperty.size())
        {
            return false;
        }

        m_vecObjectProperty[nIndex]->bHighLight = bHightLight;

        return true;
    }

    int SetIfAmbientLight(unsigned nIndex, const bool bAmbientLigth)
    {
        if (nIndex >= m_vecObjectProperty.size())
        {
            return false;
        }

        m_vecObjectProperty[nIndex]->bAmbientLight = bAmbientLigth;

        return true;
    }

    int SetIfShadow(unsigned nIndex, const bool bShadow)
    {
        if (nIndex >= m_vecObjectProperty.size())
        {
            return false;
        }

        m_vecObjectProperty[nIndex]->bShadow = bShadow;

        return true;
    }

    int SetIfMinimapShow(unsigned nIndex, const bool bMinimapShow)
    {
        if (nIndex >= m_vecObjectProperty.size())
        {
            return false;
        }

        m_vecObjectProperty[nIndex]->bMinimapShow = bMinimapShow;

        return true;
    }

    int SetIfMinimapShowText(unsigned nIndex, const bool bMinimapShowText)
    {
        if (nIndex >= m_vecObjectProperty.size())
        {
            return false;
        }

        m_vecObjectProperty[nIndex]->bMinimapShowText = bMinimapShowText;

        return true;
    }

    int SetIfReplaceableTexture(unsigned nIndex, const bool bReplaceableTexture)
    {
        if (nIndex >= m_vecObjectProperty.size())
        {
            return false;
        }

        m_vecObjectProperty[nIndex]->bReplaceableTexture = bReplaceableTexture;

        return true;
    }

    int SetIfTranslucence(unsigned nIndex, const bool bTranslucence)
    {
        if (nIndex >= m_vecObjectProperty.size())
        {
            return false;
        }

        m_vecObjectProperty[nIndex]->bTranslucence = bTranslucence;

        return true;
    }

    int SetIfHighLightScene(unsigned nIndex, const bool bHighLightScene)
    {
        if (nIndex >= m_vecObjectProperty.size())
        {
            return false;
        }

        m_vecObjectProperty[nIndex]->bHighLightScene = bHighLightScene;

        return true;
    }

    //  物件属性设置 : 声音相关
    int SetRevive(unsigned nIndex, const char pcszFileName[])
    {
        if (nIndex >= m_vecObjectProperty.size())
        {
            return false;
        }

        strncpy(
            m_vecObjectProperty[nIndex]->szRevive, pcszFileName,
            sizeof(m_vecObjectProperty[nIndex]->szRevive)
        );
        m_vecObjectProperty[nIndex]->szRevive[
            sizeof(m_vecObjectProperty[nIndex]->szRevive) - 1
        ] = '\0';

        return true;
    }

    int SetDeath(unsigned nIndex, const char pcszFileName[])
    {
        if (nIndex >= m_vecObjectProperty.size())
        {
            return false;
        }

        strncpy(
            m_vecObjectProperty[nIndex]->szDeath, pcszFileName,
            sizeof(m_vecObjectProperty[nIndex]->szDeath)
        );
        m_vecObjectProperty[nIndex]->szDeath[
            sizeof(m_vecObjectProperty[nIndex]->szDeath) - 1
        ] = '\0';

        return true;
    }

    int SetMove(unsigned nIndex, const char pcszFileName[])
    {
        if (nIndex >= m_vecObjectProperty.size())
        {
            return false;
        }

        strncpy(
            m_vecObjectProperty[nIndex]->szMove, pcszFileName,
            sizeof(m_vecObjectProperty[nIndex]->szMove)
        );
        m_vecObjectProperty[nIndex]->szMove[
            sizeof(m_vecObjectProperty[nIndex]->szMove) - 1
        ] = '\0';

        return true;
    }

    int SetLooping(unsigned nIndex, const char pcszFileName[])
    {
        if (nIndex >= m_vecObjectProperty.size())
        {
            return false;
        }

        strncpy(
            m_vecObjectProperty[nIndex]->szLooping, pcszFileName,
            sizeof(m_vecObjectProperty[nIndex]->szLooping)
        );
        m_vecObjectProperty[nIndex]->szLooping[
            sizeof(m_vecObjectProperty[nIndex]->szLooping) - 1
        ] = '\0';

        return true;
    }


    int SetLoopingTime(unsigned nIndex, const int dwTime)
    {
        if (nIndex >= m_vecObjectProperty.size())
        {
            return false;
        }

        m_vecObjectProperty[nIndex]->dwLoopingTime = dwTime;

        return true;
    }

    int SetFadeOutRate(unsigned nIndex, const int dwFadeOutRate)
    {
        if (nIndex >= m_vecObjectProperty.size())
        {
            return false;
        }

        m_vecObjectProperty[nIndex]->dwFadeOutRate = dwFadeOutRate;

        return true;
    }

    int SetFadeInRate(unsigned nIndex, const DWORD dFadeInRate)
    {
        if (nIndex >= m_vecObjectProperty.size())
        {
            return false;
        }

        m_vecObjectProperty[nIndex]->dwFadeInRate = dFadeInRate;

        return true;
    }

    int SetAffectArea(unsigned nIndex, const int dwAffectArea)
    {
        if (nIndex >= m_vecObjectProperty.size())
        {
            return false;
        }

        m_vecObjectProperty[nIndex]->dwAffectArea = dwAffectArea;

        return true;
    }

    int SetReserved(unsigned nIndex, const int dwReserved)
    {
        if (nIndex >= m_vecObjectProperty.size())
        {
            return false;
        }

        m_vecObjectProperty[nIndex]->dwReserved = dwReserved;

        return true;
    }

    //  物件属性设置 :  状态相关
    int SetLife(unsigned nIndex, const float fLife)
    {
        if (nIndex >= m_vecObjectProperty.size())
        {
            return false;
        }

        m_vecObjectProperty[nIndex]->fLife = fLife;

        return true;
    }

    int SetLifeReplenish(unsigned nIndex, const float fLifeReplenish)
    {
        if (nIndex >= m_vecObjectProperty.size())
        {
            return false;
        }

        m_vecObjectProperty[nIndex]->fLifeReplenish = fLifeReplenish;

        return true;
    }

    int SetInsaideDefence(unsigned nIndex, const float fInsaideDefence)
    {
        if (nIndex >= m_vecObjectProperty.size())
        {
            return false;
        }

        m_vecObjectProperty[nIndex]->fInsaideDefence = fInsaideDefence;

        return true;
    }

    int SetOutsaideDefence(unsigned nIndex, const float fOutsaideDefence)
    {
        if (nIndex >= m_vecObjectProperty.size())
        {
            return false;
        }

        m_vecObjectProperty[nIndex]->fOutsaideDefence = fOutsaideDefence;

        return true;
    }

    int SetAttackable(unsigned nIndex, const bool bAttackable)       
    {
        if (nIndex >= m_vecObjectProperty.size())
        {
            return false;
        }

        m_vecObjectProperty[nIndex]->bAttackable = bAttackable;

        return true;
    }

    int SetSelectable(unsigned nIndex, const bool bSelectable)
    {
        if (nIndex >= m_vecObjectProperty.size())
        {
            return false;
        }

        m_vecObjectProperty[nIndex]->bSelectable = bSelectable;

        return true;
    }

    int SetUnitWalkable(unsigned nIndex, const bool bUnitWalkable)
    {
        if (nIndex >= m_vecObjectProperty.size())
        {
            return false;
        }

        m_vecObjectProperty[nIndex]->bUnitWalkable = bUnitWalkable;

        return true;
    }

    int SetUnitCanPass(unsigned nIndex, const bool bUnitCanPass)
    {
        if (nIndex >= m_vecObjectProperty.size())
        {
            return false;
        }

        m_vecObjectProperty[nIndex]->bUnitCanPass = bUnitCanPass;

        return true;
    }

    int SetBulletWalkable(unsigned nIndex, const bool bBulletWalkable)
    {
        if (nIndex >= m_vecObjectProperty.size())
        {
            return false;
        }

        m_vecObjectProperty[nIndex]->bBulletWalkable = bBulletWalkable;

        return true;
    }

    int SetBulletCanPass(unsigned nIndex, const bool bBulletCanPass)
    {
        if (nIndex >= m_vecObjectProperty.size())
        {
            return false;
        }

        m_vecObjectProperty[nIndex]->bBulletCanPass = bBulletCanPass;

        return true;
    }

    int SetAutoPathing(unsigned nIndex, const bool bAutoPathing)
    {
        if (nIndex >= m_vecObjectProperty.size())
        {
            return false;
        }

        m_vecObjectProperty[nIndex]->bAutoPathing = bAutoPathing;

        return true;
    }

    int SetDropOnDeath(unsigned nIndex, const bool bDropOnDeath)
    {
        if (nIndex >= m_vecObjectProperty.size())
        {
            return false;
        }

        m_vecObjectProperty[nIndex]->bDropOnDeath = bDropOnDeath;

        return true;
    }

    int SetPathingType(unsigned nIndex, const int nPathingType)
    {
        if (nIndex >= m_vecObjectProperty.size())
        {
            return false;
        }

        m_vecObjectProperty[nIndex]->nPathingType = nPathingType;

        return true;
    }

    int SetPathingHeight(unsigned nIndex, const float fPathingHeight)
    {
        if (nIndex >= m_vecObjectProperty.size())
        {
            return false;
        }

        m_vecObjectProperty[nIndex]->fPathingHeight = fPathingHeight;

        return true;
    }

    int SetDeathType(unsigned nIndex, const int nDeathType)
    {
        if (nIndex >= m_vecObjectProperty.size())
        {
            return false;
        }

        m_vecObjectProperty[nIndex]->nDeathType = nDeathType;

        return true;
    }

    int SetReviveDelay(unsigned nIndex, const float fReviveDelay)
    {
        if (nIndex >= m_vecObjectProperty.size())
        {
            return false;
        }

        m_vecObjectProperty[nIndex]->fReviveDelay = fReviveDelay;

        return true;
    }

    int SetReviveTime(unsigned nIndex, const float fReviveTime)
    {
        if (nIndex >= m_vecObjectProperty.size())
        {
            return false;
        }

        m_vecObjectProperty[nIndex]->fReviveTime = fReviveTime;

        return true;
    }

    int SetDropNumber(unsigned nIndex, const int nDropNumber)
    {
        if (nIndex >= m_vecObjectProperty.size())
        {
            return false;
        }

        m_vecObjectProperty[nIndex]->nDropNumber = nDropNumber;

        return true;
    }

    int SetDropTable(
        unsigned nIndex, int nTableLen, const OBJECT_PDROP_INFO DropTable[]
    )
    {
        if (
            nIndex    >= m_vecObjectProperty.size() || 
            nTableLen != m_vecObjectProperty[nIndex]->nDropNumber
            )
        {
            return false;
        }

        for (int i = 0; i <  nTableLen; i++)
        {
            m_vecObjectProperty[nIndex]->DropTable[i].nID     = DropTable[i].nID;
            m_vecObjectProperty[nIndex]->DropTable[i].nNumber = DropTable[i].nNumber;
        }

        return true;
    }

    //  物件属性设置 : 动作相关
    int SetDeathFile1(unsigned nIndex, const char pcszFileName[])
    {
        if (nIndex >= m_vecObjectProperty.size())
        {
            return false;
        }

        strncpy(
            m_vecObjectProperty[nIndex]->szDeathFile1, pcszFileName,
            sizeof(m_vecObjectProperty[nIndex]->szDeathFile1)
        );
        m_vecObjectProperty[nIndex]->szDeathFile1[
            sizeof(m_vecObjectProperty[nIndex]->szDeathFile1) - 1
        ] = '\0';

        return true;
    }

    int SetDeathTime1(unsigned nIndex, const int dwTime)
    {
        if (nIndex >= m_vecObjectProperty.size())
        {
            return false;
        }

        m_vecObjectProperty[nIndex]->dwDeathTime1 = dwTime;

        return true;
    }

    int SetDeathFile2(unsigned nIndex, const char pcszFileName[])
    {
        if (nIndex >= m_vecObjectProperty.size())
        {
            return false;
        }

        strncpy(
            m_vecObjectProperty[nIndex]->szDeathFile2, pcszFileName,
            sizeof(m_vecObjectProperty[nIndex]->szDeathFile2)
        );
        m_vecObjectProperty[nIndex]->szDeathFile2[
            sizeof(m_vecObjectProperty[nIndex]->szDeathFile2) - 1
        ] = '\0';

        return true;
    }

    int SetDeathTime2(unsigned nIndex, const int dwTime)
    {
        if (nIndex >= m_vecObjectProperty.size())
        {
            return false;
        }

        m_vecObjectProperty[nIndex]->dwDeathTime2 = dwTime;

        return true;
    }

    int SetReserveFile1(unsigned nIndex, const char pcszFileName[])
    {
        if (nIndex >= m_vecObjectProperty.size())
        {
            return false;
        }

        strncpy(
            m_vecObjectProperty[nIndex]->szReserveFile1, pcszFileName,
            sizeof(m_vecObjectProperty[nIndex]->szReserveFile1)
        );
        m_vecObjectProperty[nIndex]->szReserveFile1[
            sizeof(m_vecObjectProperty[nIndex]->szReserveFile1) - 1
        ] = '\0';

        return true;
    }

    int SetReserveTime1(unsigned nIndex, const int dwTime)
    {
        if (nIndex >= m_vecObjectProperty.size())
        {
            return false;
        }

        m_vecObjectProperty[nIndex]->dwReserveTime1 = dwTime;

        return true;
    }

    int SetReserveFile2(unsigned nIndex, const char pcszFileName[])
    {
        if (nIndex >= m_vecObjectProperty.size())
        {
            return false;
        }

        strncpy(
            m_vecObjectProperty[nIndex]->szReserveFile2, pcszFileName,
            sizeof(m_vecObjectProperty[nIndex]->szReserveFile2)
        );
        m_vecObjectProperty[nIndex]->szReserveFile2[
            sizeof(m_vecObjectProperty[nIndex]->szReserveFile2) - 1
        ] = '\0';

        return true;
    }

    int SetReserveTime2(unsigned nIndex, const int dwTime)
    {
        if (nIndex >= m_vecObjectProperty.size())
        {
            return false;
        }

        m_vecObjectProperty[nIndex]->dwReserveTime2 = dwTime;

        return true;
    }

    int SetReserveFile3(unsigned nIndex, const char pcszFileName[])
    {
        if (nIndex >= m_vecObjectProperty.size())
        {
            return false;
        }

        strncpy(
            m_vecObjectProperty[nIndex]->szReserveFile3, pcszFileName,
            sizeof(m_vecObjectProperty[nIndex]->szReserveFile3)
        );
        m_vecObjectProperty[nIndex]->szReserveFile3[
            sizeof(m_vecObjectProperty[nIndex]->szReserveFile3) - 1
        ] = '\0';

        return true;
    }

    int SetReserveTime3(unsigned nIndex, const int dwTime)
    {
        if (nIndex >= m_vecObjectProperty.size())
        {
            return false;
        }

        m_vecObjectProperty[nIndex]->dwReserveTime3 = dwTime;

        return true;
    }

    int SetReserveFile4(unsigned nIndex, const char pcszFileName[])
    {
        if (nIndex >= m_vecObjectProperty.size())
        {
            return false;
        }

        strncpy(
            m_vecObjectProperty[nIndex]->szReserveFile4, pcszFileName,
            sizeof(m_vecObjectProperty[nIndex]->szReserveFile4)
        );
        m_vecObjectProperty[nIndex]->szReserveFile4[
            sizeof(m_vecObjectProperty[nIndex]->szReserveFile4) - 1
        ] = '\0';

        return true;
    }

    int SetReserveTime4(unsigned nIndex, const int dwTime)
    {
        if (nIndex >= m_vecObjectProperty.size())
        {
            return false;
        }

        m_vecObjectProperty[nIndex]->dwReserveTime4 = dwTime;

        return true;
    }

    int SetReserveFile5(unsigned nIndex, const char pcszFileName[])
    {
        if (nIndex >= m_vecObjectProperty.size())
        {
            return false;
        }

        strncpy(
            m_vecObjectProperty[nIndex]->szReserveFile5, pcszFileName,
            sizeof(m_vecObjectProperty[nIndex]->szReserveFile5)
        );
        m_vecObjectProperty[nIndex]->szReserveFile5[
            sizeof(m_vecObjectProperty[nIndex]->szReserveFile5) - 1
        ] = '\0';

        return true;
    }

    int SetReserveTime5(unsigned nIndex, const int dwTime)
    {
        if (nIndex >= m_vecObjectProperty.size())
        {
            return false;
        }

        m_vecObjectProperty[nIndex]->dwReserveTime5 = dwTime;

        return true;
    }

    int SetReserveFile6(unsigned nIndex, const char pcszFileName[])
    {
        if (nIndex >= m_vecObjectProperty.size())
        {
            return false;
        }

        strncpy(
            m_vecObjectProperty[nIndex]->szReserveFile6, pcszFileName,
            sizeof(m_vecObjectProperty[nIndex]->szReserveFile6)
        );
        m_vecObjectProperty[nIndex]->szReserveFile6[
            sizeof(m_vecObjectProperty[nIndex]->szReserveFile6) - 1
        ] = '\0';

        return true;
    }

    int SetReserveTime6(unsigned nIndex, const int dwTime)
    {
        if (nIndex >= m_vecObjectProperty.size())
        {
            return false;
        }

        m_vecObjectProperty[nIndex]->dwReserveTime6 = dwTime;

        return true;
    }

    //  物件属性设置 : 技能相关
    int SetnAbilitiesID(unsigned nIndex, const int nAbilitiesID)
    {
        if (nIndex >= m_vecObjectProperty.size())
        {
            return false;
        }

        m_vecObjectProperty[nIndex]->nAbilitiesID = nAbilitiesID;

        return true;

    }

    //  物件属性设置 : 文字显示相关
    int SetName(unsigned nIndex, const char pcszText[])
    {
        if (nIndex >= m_vecObjectProperty.size())
        {
            return false;
        }

        strncpy(
            m_vecObjectProperty[nIndex]->szName, pcszText,
            sizeof(m_vecObjectProperty[nIndex]->szName)
        );
        m_vecObjectProperty[nIndex]->szName[
            sizeof(m_vecObjectProperty[nIndex]->szName) - 1
        ] = '\0';

        return true;
    }

    int SetSuffix(unsigned nIndex, const char pcszText[])
    {
        if (nIndex >= m_vecObjectProperty.size())
        {
            return false;
        }

        strncpy(
            m_vecObjectProperty[nIndex]->szSuffix, pcszText,
            sizeof(m_vecObjectProperty[nIndex]->szSuffix)
        );
        m_vecObjectProperty[nIndex]->szSuffix[
            sizeof(m_vecObjectProperty[nIndex]->szSuffix) - 1
        ] = '\0';

        return true;
    }

    int SetNote(unsigned nIndex, const char pcszText[])
    {
        if (nIndex >= m_vecObjectProperty.size())
        {
            return false;
        }

        strncpy(
            m_vecObjectProperty[nIndex]->szNote, pcszText,
            sizeof(m_vecObjectProperty[nIndex]->szNote)
        );
        m_vecObjectProperty[nIndex]->szNote[
            sizeof(m_vecObjectProperty[nIndex]->szNote) - 1
        ] = '\0';

        return true;
    }

    int SetType(unsigned nIndex, const int nType)
    {
        if (nIndex >= m_vecObjectProperty.size())
        {
            return false;
        }

        m_vecObjectProperty[nIndex]->nType = nType;

        return true;
    }

public:
    KG3DRepresetObjectPropertyModifior()
    {
        memset(m_szConfigurationFilesName, 0, sizeof(m_szConfigurationFilesName));    
    }
    ~KG3DRepresetObjectPropertyModifior()
    {
    }

public:
    vector<TResource*> m_vecObjectProperty;
    char m_szConfigurationFilesName[MAX_PATH];
};

#endif //_KG3DREPRESETOBJECTPROPERTYMODIFIOR_H_