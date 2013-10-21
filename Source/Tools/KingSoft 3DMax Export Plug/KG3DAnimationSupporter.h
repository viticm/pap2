#pragma once

class KG3DAnimationCollector;

class KG3DAnimationSupporter
{
public:
	KG3DAnimationSupporter(void);
	virtual ~KG3DAnimationSupporter(void);
	virtual int GetNumNodes() = 0;
	virtual INode* GetNode(unsigned int nIndex) = 0;
	virtual void* GetExtraInfo() = 0;
};
