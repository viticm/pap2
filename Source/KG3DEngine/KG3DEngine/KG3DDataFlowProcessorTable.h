#pragma once
#include "KG3DDataFlowProcessor.h"

class KG3DDataFlowProcessorTable
{
protected:
	HRESULT UnInit();
	map<UINT,KG3DDataFlowProcessor*>m_mapProcessor;
	UINT m_uCurrentHandle;
	UINT GetCurrentHandle()
	{
		return m_uCurrentHandle++;
	}
public:
	HRESULT FindProcessor(KG3DDataFlowProcessor** ppProcessor,UINT uHandle);
	HRESULT ReleaseProcessor(UINT uHandle);
	HRESULT NewOneProcessor(UINT* puHandle,KG3DDataFlowProcessorType eType);
	HRESULT GetAllDataProcessor(vector<UINT>* pvecHandle);

public:
	KG3DDataFlowProcessorTable(void);
	virtual ~KG3DDataFlowProcessorTable(void);
};
