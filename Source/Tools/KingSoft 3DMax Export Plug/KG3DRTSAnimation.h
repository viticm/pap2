#pragma once
#include "kg3danimationcollector.h"

class KG3DRelinkTable;

class KG3DRTSAnimation : public KG3DAnimationCollector
{
	struct RTSInfo
	{
		D3DXVECTOR3 Translation;
		D3DXVECTOR3 Scale;
		D3DXQUATERNION Rotation;
		float Sign;
		D3DXQUATERNION SRotation;
	};

#pragma pack(push, 1)
	struct AnimationHeader
	{
		DWORD dwMask;
		DWORD dwBlockLength;
		DWORD dwNumAnimations;
		AnimationType dwAnimationType;
		TCHAR strAnimationName[30];
		AnimationHeader();
	};

	
	struct BoneRTSHeader
	{
		DWORD dwNumBones;
		DWORD dwNumFrames;
		float fFrameLength;
		//TCHAR strBoneNames[30][dwNumBones];
		//RTSInfo BoneMatices[dwNumBones * dwNumFrames];
	};
#pragma pack(pop)

	

public:
	KG3DRTSAnimation(void);
	virtual ~KG3DRTSAnimation(void);
	virtual void Attach(KG3DAnimationSupporter* pSupporter);
	virtual void Dettach();
	virtual void GetNodeData(INode* pNode, const TimeValue& tTime, void* pData);
	virtual void GetFrameData(const TimeValue& t);
	
	HRESULT SaveToFile(const TCHAR* strFileName);
protected:
	void ExchangeMatToRTS(const Matrix3& matSrc, RTSInfo& rtsDest);
	virtual size_t GetNodeDataLength();
	KG3DRelinkTable* m_pRelinkTable;
};
