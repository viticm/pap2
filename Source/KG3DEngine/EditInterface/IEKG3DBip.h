////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : IEKG3DBip.h
//  Version     : 1.0
//  Creator     : Xia Yong
//  Create Date : 2006-11-4 15:08:33
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _INCLUDE_IEKG3DBIP_H_
#define _INCLUDE_IEKG3DBIP_H_
#include "KG3DTypes.h"

struct BoneInfoData
{
    TCHAR strBoneName[MAX_BONE_NAME_LENGTH];
    DWORD ChildIndices[MAX_NUM_CHILD_BONE];
    DWORD dwIndex;
    DWORD dwParentIndex;
    DWORD dwNumChildBones;
};
interface IEKG3DBip
{
    virtual HRESULT GetBoneInfoDataPoint(BoneInfoData **ppBoneInfo) = 0;
    virtual HRESULT GetBaseBoneCount(DWORD *pdwCount) = 0;
    virtual HRESULT GetBaseBoneByPosition(DWORD dwPositon, INT *pnBaseBone) = 0;
};

#endif //_INCLUDE_IEKG3DBIP_H_
