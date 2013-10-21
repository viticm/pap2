#pragma once
#include "KG3DAnimationStruct.h"
class KG3DModel;
class KG3DModelST;

class KG3DBoneSet
{
public:
	const static DWORD m_dwID = 0x4257544c;//Ascii code for BWTL(Bone Weight TabLe)

	KG3DBoneSet();
	~KG3DBoneSet();
	
    HRESULT LoadFromFile(const TCHAR *strFileName);
    HRESULT SaveToFile(const TCHAR *strFileName);
	
    void SetBoneWeight(int nBoneIndex, float fWeight, BOOL bInherit);
	void SetBoneWeight(const TCHAR* strBoneName, float fWeight, BOOL bInherit);

	float GetBoneWeight(int nBoneIndex);
	float GetBoneWeight(const TCHAR* strBoneName);
	void GetChildInfo(int nIndex, const std::vector<int>** ppChildren);
	void GetChildInfo(const TCHAR* strBoneName, const std::vector<int>** ppChildren);
	DWORD GetNumBones();
	void Clone(const KG3DBoneSet& SrcBoneSet);
	HRESULT Create(KG3DModel* pModel);
    void SetCollector(KG3DAnimationUpdateExtraInfoCollector *pCollector);
    void GetBoneMatrix(int nIndex, D3DXMATRIX& matResult);

protected:
    HRESULT CreateModel(KG3DModel* pModel);
    HRESULT CreateModelST(KG3DModel* pModel);
	typedef string BONE_ID;

	const static int MAX_BONE_NAME = MAX_PATH;
	const static int MAX_NUM_CHILD_BONE = 10;
	

	struct BoneInfoItem
	{
        DWORD dwFilter;
		std::string strName;
		int nParentIndex;
		int nBoneIndex;
		std::vector<int> ChildIndices;
		float fWeight;
        BoneInfoItem();
	};

	struct BoneSetHeader
	{
		DWORD dwMask;
		DWORD dwVersion;
		DWORD dwNumBones;
		DWORD dwExtendBlock[10];
	};

	typedef std::map<string, int> MapTable;
	typedef std::map<string, int>::iterator MapTable_it;

	typedef std::vector<BoneInfoItem> WeightTable;
	typedef std::vector<BoneInfoItem>::iterator WeightTable_it;
	WeightTable m_WeightTable;
	MapTable    m_MapTable;

    struct BoneAnimationInfo
    {
        KG3DAnimationUpdateExtraInfoCollector* Collectors;
        std::vector<DWORD>                                  Flags;
    };
};