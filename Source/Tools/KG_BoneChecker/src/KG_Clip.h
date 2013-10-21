////////////////////////////////////////////////////////////////////////////////
//  Copyright(c) Kingsoft
//
//	FileName    :   KG_Clip.h
//	Creator     :   Liang Bo
//	Date        :   2009-2-24 16:30:20
//	Comment     :   
//
////////////////////////////////////////////////////////////////////////////////

#ifndef __KG_Clip_h__
#define __KG_Clip_h__

#include "KG_Bip.h"
#include <d3d9.h>
#include <d3dx9.h>

const DWORD MAX_BONE_NUM = 255;

class KG_Clip
{
public:

    KG_Clip();
    ~KG_Clip();

    int LoadFromFile(const char cszFileName[]);
    int SkeletonCompare(KG_Bip *pBip, FILE *fpResultFile);
    unsigned GetBoneNum() const;

private:
    int _Clear();
    int _SwapBoneAnimationData(unsigned uBone1, unsigned uBone2);
    int _SaveNewFile();
    unsigned _FindBoneIndex(const char cszBoneName[]);

private:
    struct RTS
    {
        D3DXVECTOR3 Translation;
        D3DXVECTOR3 Scale;
        D3DXQUATERNION Rotation;
        float Sign;
        D3DXQUATERNION SRotation;
    };

    enum enuClipType
    {
        ANIMATION_NONE                = 0,
        ANIMATION_VERTICES            = 1,
        ANIMATION_VERTICES_RELATIVE   = 2,
        ANIMATION_BONE                = 10,
        ANIMATION_BONE_RELATIVE       = 11,
        ANIMATION_BONE_16             = 12,
        ANIMATION_BONE_RTS            = 13,
        ANIMATION_COUNT,
        ANIMATION_FORCE_DWORD = 0xffffffff,
    };

    enum
    {
        MAX_CLIPDESC_LEN = 30,
        MAX_BONENAME_LEN = 30,
    };

#pragma pack(push,1)  
    struct AniFileHeaderInfo
    {
        DWORD dwMask;
        DWORD dwBlockLength;
        DWORD dwNumAnimations;
        DWORD dwType;
        char  szDesc[MAX_CLIPDESC_LEN];
        struct BoneAnimation
        {
            DWORD dwNumBones;
            DWORD dwNumFrames;
            float fFrameLength;
        } BoneAni;
    };
#pragma pack(pop)
    
    AniFileHeaderInfo *m_pAniFileHeader;
    char       *m_pAniFileData;
    char       *m_pszBoneName[MAX_BONE_NUM];
    RTS        *m_pAnimatedBoneRTS[MAX_BONE_NUM];
    string      m_sClipName;
    DWORD       m_dwBoneNum;
};

////////////////////////////////////////////////////////////////////////////////
int AnalyseAni(vector<string> &vecBipList);
int _GetAniList(const char cszBipFile[], vector<string> *pvecRetAniList);

#endif // __KG_Clip_h__