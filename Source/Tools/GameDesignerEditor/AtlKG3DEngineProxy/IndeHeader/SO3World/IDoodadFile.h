//////////////////////////////////////////////////////////////////////////////////////
//
//  FileName    :   IDoodadFile.h
//  Version     :   1.0
//  Creater     :   Zhao Chunfeng
//  Date        :   2008-05-29 06:56:19
//  Comment     :   Doodad file interface
//
//////////////////////////////////////////////////////////////////////////////////////
#ifndef _IDOODADFILE_H_
#define _IDOODADFILE_H_

#include "KSUnknown.h"

#pragma	pack(1)
struct KDOODAD_DATA
{
	char	szNickName[_NAME_LEN];
	DWORD	dwTemplateID;
	BYTE	byLevel;
	int		nX;
	int		nY;
	int		nZ;
	DWORD	dwScriptID;
	BYTE	byFaceDirection;	
	DWORD	dwDoodadReliveID;
    WORD    wObstacleGroup;
    int     nProgressID;
	BYTE	byReserved[22];
};
#pragma pack()

struct IDoodadFile : IUnknown
{
    virtual BOOL Push(const KDOODAD_DATA& crDoodadInfo) = 0;
    virtual BOOL Save(const char cszName[]) = 0;

    virtual BOOL Load(const char cszName[]) = 0;
    virtual BOOL Pop(KDOODAD_DATA* pDoodadInfo) = 0;

    virtual int  GetCount() = 0;
};

IDoodadFile* CreateDoodadFileInterface();

#endif
