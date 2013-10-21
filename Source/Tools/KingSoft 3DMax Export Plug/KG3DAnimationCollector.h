#pragma once

#include "KG3DAnimationSupporter.h"

class KG3DAnimationCollector
{
protected:
	struct AnimationData
	{
		TimeValue t;
		void* pData;
	};

public:
	KG3DAnimationCollector(void);
	virtual ~KG3DAnimationCollector(void);
	virtual void Attach(KG3DAnimationSupporter* pSupporter);
	virtual void Dettach();
	virtual void GetFrameData(const TimeValue& t) = 0;
	virtual void GetNodeData(INode* pNode, const TimeValue& tTime, void* pData) = 0;
	virtual size_t GetNodeDataLength() = 0;

	const static DWORD m_dwMask = 0x414E494D;
protected:
	virtual void CleanUp();
	KG3DAnimationSupporter* m_pSupporter;
	std::vector<AnimationData> m_Data;
};