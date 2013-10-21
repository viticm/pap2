#pragma once

#pragma warning(disable: 4996)
#include "kg3dboneset.h"
#include "kg3dmesh.h"

class KG3DBoneSetEx: public KG3DBoneSet
{
public:
	KG3DBoneSetEx(void);
	~KG3DBoneSetEx(void);

	struct BWItem
	{
		BONE_ID BoneID;
		float   fWeight;
	};

	HRESULT SetBonesWeight(BONE_ID* pBoneID, float fWeight, DWORD dwNumBones);
	HRESULT SetBoneWeight(const BONE_ID& BoneID, float fWeight, DWORD dwOption = 0);
	void NormalizeTable(float fWeight);
	/*HRESULT GetBoneWeight(const BONE_ID& BoneID, float& fWeight)
	{
		HRESULT hr = E_FAIL;
		if (m_WeightTable.count(BoneID))
		{
			fWeight = m_WeightTable[BoneID].fWeight;
			hr = S_OK;
		}
		return hr;
	}*/
	void GetAllBones(BONE_ID* pBoneID);
	HRESULT AddBone(const BONE_ID& BoneID, const BONE_ID& ParentBoneID = string(""), float fWeight = 1.0f);
	void RemoveBone(const BONE_ID& BoneID, DWORD dwOption, BOOL bRemoveChild = TRUE);

	HRESULT SaveToFile(FILE* pFile);

	HRESULT CheckSum(KG3DBoneSetEx* pTables, DWORD dwNumTables, BONE_ID* pErrorBones, DWORD& dwNumErrBones);

	void AttachToBone(const BONE_ID& BoneChild, const BONE_ID& BoneParent);
	void Detach(const BONE_ID& Bone);

	HRESULT GenerateSekeletonFromMesh(const KG3DMesh* pMesh, float fDefaultWeight = 1.0f);
private:
	void SetBoneWeightWithChild(const BONE_ID& BoneID, float fWeight);

};
#pragma warning(default: 4996)