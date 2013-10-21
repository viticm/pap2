////////////////////////////////////////////////////////////////////////////////
//
//  FileName  : KG3DRepresentNPCTemplate.h
//  Author    : ZhaoXinyu (Chiyer)
//  CopyRight : Kingsoft Corporation All rights Reserved
//  History   : 2005.9.23 23:36 Create
//  Comment   : NPC Property Modifiy Template
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _KG3DREPRESETNPCTEMPLATE_H_
#define _KG3DREPRESETNPCTEMPLATE_H_

#include "KG3DRepresentObjectTemplate.h"
#pragma once

// NPC Property
struct NPC_PROPERTY : public OBJECT_PROPERTY
{
    int     nNpcType;
    int     nNpcID;

    NPC_PROPERTY()
    {
        OBJECT_PROPERTY();
        nNpcType = 0;
        nNpcID   = 0;
     }
};

class KG3DRepresentNPCTemplate :
    public KG3DRepresetObjectPropertyModifior<NPC_PROPERTY>
{
public:
    virtual int LoadFromIniFile(const char pcszFileName[]);
    int SetNpcType(unsigned nIndex, const int nNpcType)
    {
        if (nIndex >= m_vecObjectProperty.size())
        {
            return false;
        }
        m_vecObjectProperty[nIndex]->nNpcType = nNpcType;
        return true;
    }

    int SetNpcID(unsigned nIndex, const int nNpcID)
    {
        if (nIndex >= m_vecObjectProperty.size())
        {
            return false;
        }
        m_vecObjectProperty[nIndex]->nNpcID = nNpcID;
        return true;
    }

public:
	KG3DRepresentNPCTemplate(void);
	virtual ~KG3DRepresentNPCTemplate(void);
};

extern KG3DRepresentNPCTemplate g_cNpeTemplate;        

//////////////////////////////////////////////////////////////////////////




#endif // _KG3DREPRESETNPCTEMPLATE_H_
