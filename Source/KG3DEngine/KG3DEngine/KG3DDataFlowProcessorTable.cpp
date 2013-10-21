#include "StdAfx.h"
#include "KG3DDataFlowProcessorTable.h"
#include "KG3DDataFlowProcessor.h"

#ifdef _DEBUG
#define new DEBUG_NEW_3DENGINE
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//KG3DDataFlowProcessorTable g_cDataFlowProcessorTable;

KG3DDataFlowProcessorTable::KG3DDataFlowProcessorTable(void)
{
	m_uCurrentHandle = 1;
}

KG3DDataFlowProcessorTable::~KG3DDataFlowProcessorTable(void)
{
}

HRESULT KG3DDataFlowProcessorTable::ReleaseProcessor(UINT uHandle)
{
	map<UINT,KG3DDataFlowProcessor*>::iterator i = m_mapProcessor.find(uHandle);
	if(i==m_mapProcessor.end())
	{
		return E_FAIL;
	}
	else
	{
		KG3DDataFlowProcessor* pProcess = i->second;
		SAFE_DELETE(pProcess);
		m_mapProcessor.erase(i);
		return S_OK;
	}
	return S_OK;
}

HRESULT KG3DDataFlowProcessorTable::FindProcessor(KG3DDataFlowProcessor** ppProcessor,UINT uHandle)
{
	map<UINT,KG3DDataFlowProcessor*>::iterator i = m_mapProcessor.find(uHandle);
	if(i==m_mapProcessor.end())
	{
		return E_FAIL;
	}
	else
	{
		(*ppProcessor) = i->second;
		return S_OK;
	}
	return S_OK;
}

HRESULT KG3DDataFlowProcessorTable::NewOneProcessor(UINT* puHandle,KG3DDataFlowProcessorType eType)
{
	KG3DDataFlowProcessor* pProcessor = NULL;
	switch(eType)
	{
	case DATA_PROCESSOR_NONE:
		pProcessor = new KG3DDataFlowProcessor();
		KGLOG_PROCESS_ERROR(pProcessor);
		break;

	case DATA_PROCESSOR_DATACREATER:
		pProcessor = new KG3DDataFlowProcessorDataCreater();
		KGLOG_PROCESS_ERROR(pProcessor);
		break;
	case DATA_PROCESSOR_DATADESTROYER:
		pProcessor = new KG3DDataFlowProcessorDataDestroyer();
		KGLOG_PROCESS_ERROR(pProcessor);
	    break;
	case DATA_PROCESSOR_DATAMODIFY:
		pProcessor = new KG3DDataFlowProcessorDataModifier();
		KGLOG_PROCESS_ERROR(pProcessor);
		break;

	case DATA_PROCESSOR_ALGORITHM:
		pProcessor = new KG3DDataFlowProcessorAlgorithm();
		KGLOG_PROCESS_ERROR(pProcessor);
	    break;

	case DATA_PROCESSOR_SYSTEMINPUT:
		pProcessor = new KG3DDataFlowProcessorSystemInput();
		KGLOG_PROCESS_ERROR(pProcessor);
		break;
	case DATA_PROCESSOR_SYSTEMOUTPUT:
		pProcessor = new KG3DDataFlowProcessorSystemOutPut();
		KGLOG_PROCESS_ERROR(pProcessor);
		break;
	default:
		KGLOG_PROCESS_ERROR(0);
	    break;
	}
	
	pProcessor->SetProcessType( eType );
	pProcessor->SetTable(this);
	pProcessor->AddRef();

	UINT uHandleNew = GetCurrentHandle();
	pProcessor->SetHandle(uHandleNew);
	m_mapProcessor[uHandleNew] = pProcessor;
	(*puHandle) = uHandleNew;

	return S_OK;
Exit0:
	return E_FAIL;
}

HRESULT KG3DDataFlowProcessorTable::UnInit()
{
	map<UINT,KG3DDataFlowProcessor*>::iterator i = m_mapProcessor.begin();
	map<UINT,KG3DDataFlowProcessor*>::iterator iend = m_mapProcessor.end();
	while (i!=iend)
	{
		KG3DDataFlowProcessor* pProcessor = i->second;
		SAFE_DELETE(pProcessor);
		i++;
	}
	m_mapProcessor.clear();

	return S_OK;
}

HRESULT KG3DDataFlowProcessorTable::GetAllDataProcessor(vector<UINT>* pvecHandle)
{
	pvecHandle->clear();

	map<UINT,KG3DDataFlowProcessor*>::iterator i = m_mapProcessor.begin();
	map<UINT,KG3DDataFlowProcessor*>::iterator iend = m_mapProcessor.end();
	while (i!=iend)
	{
		KG3DDataFlowProcessor* pProcessor = i->second;
		pvecHandle->push_back(pProcessor->GetHandle());
		i++;
	}
	return S_OK;
}
