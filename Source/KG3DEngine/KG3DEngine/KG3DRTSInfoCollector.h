#pragma once

#include "KG3DAnimationStruct.h"

class KG3DRTSInfoCollector: public KG3DAnimationUpdateExtraInfoCollector
{
public:
	KG3DRTSInfoCollector(void);
	virtual ~KG3DRTSInfoCollector(void);
	virtual void SaveData(DWORD dwIndex, void *pData);
	virtual void* GetData();
	virtual DWORD GetSize();
	virtual void SetSize(DWORD dwSize);
	virtual void* GetData(DWORD dwIndex);
	virtual void Clear();
	virtual HRESULT CreateStore(KG3DModel *pModel);
	HRESULT CreateStore(KG3DBip *pBip);
	virtual void SetData(void* pData, size_t NumData);
private:
	RTS*  m_RTSData;
	DWORD m_dwSize;
};
