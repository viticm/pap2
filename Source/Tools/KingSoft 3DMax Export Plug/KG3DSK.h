#pragma once
#include "kg3dblacklist.h"

class KG3DSK
{
public:
	KG3DSK(void);
	~KG3DSK(void);
	BOOL CheckSKValidation(BOOL bWriteLog);
	HRESULT GetStandardSkeletonFromFile(const TCHAR* strFileName);
	int FindBone(const TCHAR* strBoneName);
	void OutputToFile(const TCHAR* strFileName);
private:
	void DeleteBoneNameArray(TCHAR** ppArray);
	TCHAR** GetChildBoneNameArray();
	int GetBoneInfoFromString(const TCHAR* strInput, BoneInfoData& Data, TCHAR** ppChildBones);
	void FillChildInfo(TCHAR** ppInput, BoneInfoData& Data);
	void EliminateBlackBone();
	void ClearUnusedBones();
	void RelinkChildToParent(int nIndex);
	
	std::vector<BoneInfoData> m_SK;
	std::vector<int> m_BlackListBones;
	KG3DBlackList   m_BlackList;
	
};
