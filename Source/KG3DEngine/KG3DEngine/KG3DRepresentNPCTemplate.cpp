////////////////////////////////////////////////////////////////////////////////
//
//  FileName  : KG3DRepresentNPCTemplate.cpp
//  Author    : ZhaoXinyu 
//  CopyRight : Kingsoft Corporation All rights Reserved
//  History   : 2005.9.23 23:36 Create
//  Comment   :
//
////////////////////////////////////////////////////////////////////////////////
#include "StdAfx.h"
#include "kg3drepresentnpctemplate.h"


#ifdef _DEBUG
#define new DEBUG_NEW_3DENGINE
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

KG3DRepresentNPCTemplate   g_cNpeTemplate;      // NPC 属性表

KG3DRepresentNPCTemplate::KG3DRepresentNPCTemplate(void)
{
    strncpy(
        m_szConfigurationFilesName, ".\\物件\\NPC\\", 
        sizeof(m_szConfigurationFilesName)
    );
    m_szConfigurationFilesName[sizeof(m_szConfigurationFilesName) - 1] = '\0';
}

KG3DRepresentNPCTemplate::~KG3DRepresentNPCTemplate(void)
{
}

int KG3DRepresentNPCTemplate::LoadFromIniFile(const char pcszFileName[])
{
    KG3DRepresetObjectPropertyModifior<NPC_PROPERTY>
        ::LoadFromIniFile(pcszFileName);
    return TRUE;
}