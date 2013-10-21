////////////////////////////////////////////////////////////////////////////////
//  Copyright(c) Kingsoft
//
//	FileName    :   KG_Bip.h
//	Creator     :   Liang Bo
//	Date        :   2009-2-23 10:11:43
//	Comment     :   
//
////////////////////////////////////////////////////////////////////////////////

#ifndef __KG_Bip_h__
#define __KG_Bip_h__

////////////////////////////////////////////////////////////////////////////////
const int MAX_BONE_NAME_LENGTH = 30;
const int MAX_NUM_CHILD_BONE = 30;

struct BoneInfoData
{
    char     szBoneName[MAX_BONE_NAME_LENGTH];
    char     szChildName[MAX_NUM_CHILD_BONE][MAX_BONE_NAME_LENGTH];
    unsigned uNumChildBones;
};

class KG_Bip
{
public:
    KG_Bip(); 
    ~KG_Bip();

    int LoadFromFile(const char cszFileName[], int nSortNameFlag);
    int CheckClip(const char cszClipName[], FILE *fpResultFile);
    unsigned GetNumBone() const;
    BoneInfoData* GetSkeleton() const;
    const char* GetBipName() const;
    const char* GetBoneNameByIndex(unsigned uBoneIndex) const;

private:
    int _LineToBoneInfo(
        const char szLineBuffer[], int nSortNameFlag, BoneInfoData *pRetBoneInfo
    );
    int _CheckSkeleton(unsigned uIndex);
    unsigned _FindBoneIndex(const char cszBoneName[]);

private:
    vector<unsigned> m_vecBaseBones;
    unsigned m_uNumBones;
    string m_sBipName;
    BoneInfoData* m_pSkeleton;

    static inline int _BoneCmp(const BoneInfoData &crBone1, const BoneInfoData &crBone2)
    {
        return strcmp(crBone1.szBoneName, crBone2.szBoneName) < 0;
    }

    static inline int _StringCmp(const void *cpv1, const void *cpv2)
    {
        return strcmp((const char *)cpv1, (const char *)cpv2);
    }
};

int AnalyseBip(vector<string> &vecBipList);

#endif // __KG_Bip_h__