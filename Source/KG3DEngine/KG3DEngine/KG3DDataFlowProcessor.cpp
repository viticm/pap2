#include "StdAfx.h"
#include "KG3DDataFlowProcessor.h"
#include "KG3DDataFlowProcessorTable.h"
#include "KG3DSceneDataFlowEditor.h"
#include "KG3DDataFlowDataObject.h"
#include "KG3DDataFlowDataObjectTable.h"
#include "KG3DDataFlowArc.h"
#include "KG3DDataFlowDataDefine.h"
#include "KG3DRepresentObjectTable.h"
#include "KG3DRepresentObjectDataFlowProcessor.h"
#include "KG3DDataFlowArcTable.h"
#include "KG3DRepresentObjectDataFlowArc.h"

#ifdef _DEBUG
#define new DEBUG_NEW_3DENGINE
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

KG3DDataFlowProcessor::KG3DDataFlowProcessor(void)
{
	m_ulRefCount = 0;
	m_eProcessorType = DATA_PROCESSOR_NONE;
	m_lpTable = NULL;
	m_uHandle = 0;
	m_lpEditor = NULL;
}

KG3DDataFlowProcessor::~KG3DDataFlowProcessor(void)
{
}

HRESULT KG3DDataFlowProcessor::SetProcessType(KG3DDataFlowProcessorType eType)
{
	m_eProcessorType = eType;
	return S_OK;
}

ULONG KG3DDataFlowProcessor::AddRef()
{
	return (ULONG)KG_InterlockedIncrement((long *)&m_ulRefCount);
}

ULONG KG3DDataFlowProcessor::Release()
{
    ULONG uNewRefCount = (ULONG)KG_InterlockedDecrement((long *)&m_ulRefCount);
    if (uNewRefCount == 0)
        m_lpTable->ReleaseProcessor(m_uHandle);
    return uNewRefCount;
}

HRESULT KG3DDataFlowProcessor::DoProcess()//处理数据
{
	return S_OK;
}

KG3DDataFlowProcessorType KG3DDataFlowProcessor::GetProcessType()
{
	return m_eProcessorType;
}

DWORD KG3DDataFlowProcessor::GetNumInputNode()
{
	return (DWORD)m_vecInputNode.size();
}

DWORD KG3DDataFlowProcessor::GetNumOutPutNode()
{
	return (DWORD)m_vecOutputNode.size();
}

HRESULT KG3DDataFlowProcessor::Init(KG3DSceneDataFlowEditor* pEditor)
{
	m_lpEditor = pEditor;

	return S_OK;
}

HRESULT KG3DDataFlowProcessor::SetInputNodeType(DWORD dwIndex,UINT uDataDefine,DWORD dwContainerType)
{
	KGLOG_PROCESS_ERROR( dwIndex < (DWORD) m_vecInputNode.size());

	ConnectionNode& vNode = m_vecInputNode[dwIndex];

	if(!vNode.bDefineLock)
		vNode.uDataDefine = uDataDefine;
	if(!vNode.bContainerLock)
		vNode.dwContainerType = dwContainerType;

	return S_OK;
Exit0:
	return E_FAIL;
}

HRESULT KG3DDataFlowProcessor::GetInputNodeType(DWORD dwIndex,UINT* puDataDefine,DWORD* pdwContainerType,BOOL& bDefineLock,BOOL& bContainerLock)
{
	KGLOG_PROCESS_ERROR( dwIndex < (DWORD) m_vecInputNode.size());

	ConnectionNode& vNode = m_vecInputNode[dwIndex];

	(*puDataDefine) = vNode.uDataDefine;
	(*puDataDefine) = vNode.dwContainerType;

	return S_OK;
Exit0:
	return E_FAIL;
}

HRESULT KG3DDataFlowProcessor::SetOutputNodeType(DWORD dwIndex,UINT uDataDefine,DWORD dwContainerType)
{
	KGLOG_PROCESS_ERROR( dwIndex < (DWORD) m_vecOutputNode.size());

	ConnectionNode& vNode = m_vecOutputNode[dwIndex];

	if(!vNode.bDefineLock)
		vNode.uDataDefine = uDataDefine;
	if(!vNode.bContainerLock)
		vNode.dwContainerType = dwContainerType;

	return S_OK;
Exit0:
	return E_FAIL;
}

HRESULT KG3DDataFlowProcessor::GetOutputNodeType(DWORD dwIndex,UINT* puDataDefine,DWORD* pdwContainerType,BOOL& bDefineLock,BOOL& bContainerLock)
{
	KGLOG_PROCESS_ERROR( dwIndex < (DWORD) m_vecOutputNode.size());

	ConnectionNode& vNode = m_vecOutputNode[dwIndex];

	(*puDataDefine) = vNode.uDataDefine;
	(*puDataDefine) = vNode.dwContainerType;

	return S_OK;
Exit0:
	return E_FAIL;
}

HRESULT KG3DDataFlowProcessor::ConnectArcInputNode(DWORD dwIndex,UINT uArcHandle,KG3DDataFlowArc* pArc,DWORD dwArcConnectionIndex)
{
	KGLOG_PROCESS_ERROR( dwIndex < (DWORD) m_vecInputNode.size());

	ConnectionNode& vNode = m_vecInputNode[dwIndex];

	vNode.uArcHandle = uArcHandle;
	vNode.lpArc = pArc;
	vNode.dwArcConnectionIndex = dwArcConnectionIndex;

	return S_OK;
Exit0:
	return E_FAIL;
}

HRESULT KG3DDataFlowProcessor::ConnectArcOutputNode(DWORD dwIndex,UINT uArcHandle,KG3DDataFlowArc* pArc,DWORD dwArcConnectionIndex)
{
	KGLOG_PROCESS_ERROR( dwIndex < (DWORD) m_vecOutputNode.size());

	ConnectionNode& vNode = m_vecOutputNode[dwIndex];

	vNode.uArcHandle = uArcHandle;
	vNode.lpArc = pArc;
	vNode.dwArcConnectionIndex = dwArcConnectionIndex;

	return S_OK;
Exit0:
	return E_FAIL;
}


HRESULT KG3DDataFlowProcessorAlgorithm::Init(KG3DSceneDataFlowEditor* pEditor)
{
	ConnectionNode vNode;

	vNode.uDataDefine = pEditor->m_uDefineSystemKeyInput;
	vNode.dwContainerType = CONTAINER_LIST;
	vNode.bContainerLock = TRUE;
	vNode.bDefineLock = TRUE;
	vNode.bHasInnerNode = TRUE;
	m_vecInputNode.push_back(vNode);

	vNode.uDataDefine = pEditor->m_uDefineINT;
	vNode.dwContainerType = CONTAINER_LIST;
	vNode.bContainerLock = TRUE;
	vNode.bDefineLock = TRUE;
	vNode.bHasInnerNode = TRUE;
	m_vecInputNode.push_back(vNode);

	vNode.uDataDefine = pEditor->m_uDefineINT;
	vNode.dwContainerType = CONTAINER_LIST;
	vNode.bContainerLock = TRUE;
	vNode.bDefineLock = TRUE;
	vNode.bHasInnerNode = TRUE;
	m_vecOutputNode.push_back(vNode);

	return KG3DDataFlowProcessor::Init(pEditor);
}

HRESULT KG3DDataFlowProcessorSystemInput::Init(KG3DSceneDataFlowEditor* pEditor)
{
	ConnectionNode vNode;
	vNode.uDataDefine = pEditor->m_uDefineSystemKeyInput;
	vNode.dwContainerType = CONTAINER_LIST;
	vNode.bContainerLock = TRUE;
	vNode.bDefineLock = TRUE;
	m_vecOutputNode.push_back(vNode);

	return KG3DDataFlowProcessor::Init(pEditor);
}

HRESULT KG3DDataFlowProcessorSystemInput::ProcessRunMessage(UINT message, WPARAM wParam, LPARAM lParam)
{
	HRESULT hr = S_OK;

	if (message == WM_KEYDOWN)
	{
		if(m_vecOutputNode[0].lpArc)
		{
			KG3DDataFlowDataObject* pNewData = NULL;
			KG3DDataFlowDataDefine* pDefine = m_lpEditor->FindDataDefine(m_vecOutputNode[0].uDataDefine);
			KGLOG_PROCESS_ERROR(pDefine);

			hr = m_lpEditor->m_lpDataTable->NewOneData(&pNewData,pDefine);
			KGLOG_COM_PROCESS_ERROR(hr);

			pNewData->SetInt(1,(DWORD)wParam,pDefine);

			m_vecOutputNode[0].lpArc->InputDataList(pNewData);
		}
	}
	return S_OK;
Exit0:
	return E_FAIL;
}

HRESULT KG3DDataFlowProcessor::CreateInnerDataModifer()
{
	HRESULT hr = S_OK;

	ClearInnerObject();

	//////////////////////////////////////////////////////////////////////////
	for (int i=0;i<(int)m_vecInputNode.size();i++)
	{
		ConnectionNode& vNode = m_vecInputNode[i];
		if (vNode.bHasInnerNode)
		{
			KG3DDataFlowProcessorDataModifier* pModifier = NULL;
			UINT uNewProcessor;
			hr = NewOneInnerProcessor(&uNewProcessor,DATA_PROCESSOR_DATAMODIFY);
			KGLOG_COM_PROCESS_ERROR(hr);

			hr = m_lpEditor->m_lpProcessorTable->FindProcessor((KG3DDataFlowProcessor**)&pModifier,uNewProcessor);
			KGLOG_COM_PROCESS_ERROR(hr);

			KGLOG_PROCESS_ERROR(pModifier);

			pModifier->SetDataDefine(vNode.uDataDefine);
			pModifier->Init(m_lpEditor);
			pModifier->CreateConnectionNodes();

			vNode.uInnerNodeHandle = uNewProcessor;
		}
	}
	//////////////////////////////////////////////////////////////////////////
	for (int i=0;i<(int)m_vecOutputNode.size();i++)
	{
		ConnectionNode& vNode = m_vecOutputNode[i];
		if (vNode.bHasInnerNode)
		{
			KG3DDataFlowProcessorDataModifier* pModifier = NULL;
			UINT uNewProcessor;

			hr = NewOneInnerProcessor(&uNewProcessor,DATA_PROCESSOR_DATAMODIFY);
			KGLOG_COM_PROCESS_ERROR(hr);

			hr = m_lpEditor->m_lpProcessorTable->FindProcessor((KG3DDataFlowProcessor**)&pModifier,uNewProcessor);
			KGLOG_COM_PROCESS_ERROR(hr);

			KGLOG_PROCESS_ERROR(pModifier);

			pModifier->SetDataDefine(vNode.uDataDefine);
			pModifier->Init(m_lpEditor);
			pModifier->CreateConnectionNodes();

			vNode.uInnerNodeHandle = uNewProcessor;

		}
	}

	return S_OK;
Exit0:
	return E_FAIL;
}

HRESULT KG3DDataFlowProcessor::ClearInnerObject()
{
	//////////////////////////////////////////////////////////////////////////
	for (int i=0;i<(int)m_vecInputNode.size();i++)
	{
		ConnectionNode& vNode = m_vecInputNode[i];
		if (vNode.bHasInnerNode)
		{
			DeleteInnerProcessor(vNode.uInnerNodeHandle);
			vNode.uInnerNodeHandle = 0;
		}
	}
	//////////////////////////////////////////////////////////////////////////
	for (int i=0;i<(int)m_vecOutputNode.size();i++)
	{
		ConnectionNode& vNode = m_vecOutputNode[i];
		if (vNode.bHasInnerNode)
		{
			DeleteInnerProcessor(vNode.uInnerNodeHandle);
			vNode.uInnerNodeHandle = 0;
		}
	}
	return S_OK;
}

HRESULT KG3DDataFlowProcessorDataModifier::Init(KG3DSceneDataFlowEditor* pEditor)
{
	HRESULT hr = S_OK;
	hr = KG3DDataFlowProcessor::Init(pEditor);
	KGLOG_COM_PROCESS_ERROR(hr);

	return S_OK;
Exit0:
	return E_FAIL;
}

HRESULT KG3DDataFlowProcessorDataModifier::CreateConnectionNodes()
{
	KG3DDataFlowDataDefine* pDefine = NULL;
	m_vecInputNode.clear();
	m_vecOutputNode.clear();

	{
		ConnectionNode vNode;
		vNode.uDataDefine = m_uDataDefine;
		vNode.dwContainerType = CONTAINER_LIST;
		vNode.bContainerLock = TRUE;
		vNode.bDefineLock = TRUE;
		m_vecInputNode.push_back(vNode);
		m_vecOutputNode.push_back(vNode);
	}
	//////////////////////////////////////////////////////////////////////////
	pDefine = m_lpEditor->FindDataDefine(m_uDataDefine);
	KGLOG_PROCESS_ERROR(pDefine);
	for (DWORD i=0;i<pDefine->GetNumVariable();i++)
	{
		KG3DDATAFLOWVARIABLETYPE eType;
		TCHAR Name[MAX_PATH];
		pDefine->GetVariable(i,&eType,Name);

		ConnectionNode vNode;
		vNode.uDataDefine = m_lpEditor->m_uDefineINT;

		vNode.dwContainerType = CONTAINER_LIST;
		vNode.bContainerLock = TRUE;
		vNode.bDefineLock = TRUE;

		m_vecInputNode.push_back(vNode);
		m_vecOutputNode.push_back(vNode);
	}

	return S_OK;
Exit0:
	return E_FAIL;
}

HRESULT KG3DDataFlowProcessor::NewOneInnerProcessor(UINT* pHandle,DWORD dwType)
{
	HRESULT hr = S_OK;
	KG3DDataFlowProcessor* pProcess = NULL;
	KG3DRepresentObjectDataFlowProcessor* pObject = NULL;

	hr = m_lpEditor->m_lpProcessorTable->NewOneProcessor(pHandle,(KG3DDataFlowProcessorType)dwType);
	KGLOG_COM_PROCESS_ERROR(hr);

	hr = m_lpEditor->m_lpProcessorTable->FindProcessor(&pProcess,(*pHandle));
	KGLOG_COM_PROCESS_ERROR(hr);

	hr = g_GetObjectTable().NewOneResourse(REPRESENTOBJECT_DATAFLOWPROCESSOR,(IKG3DResourceBase**)&pObject);
	KGLOG_COM_PROCESS_ERROR(hr);

	pProcess->Init(m_lpEditor);
	pProcess->AddRef();
	pObject->SetProcessor(pProcess);
	pObject->ComputeBBox();
	{
		D3DXVECTOR3 V(0,0,0);
		V.x = m_mapInnerRepProcessor.size() * 500.0F;
		pObject->SetTranslation(&V);
	}

	m_mapInnerProcessor[(*pHandle)] = pProcess;
	pProcess->AddRef();
	m_mapInnerRepProcessor[(*pHandle)] = pObject;

	return S_OK;
Exit0:
	return E_FAIL;
}

HRESULT KG3DDataFlowProcessor::DeleteInnerProcessor(UINT uHandle)
{
	map<UINT,KG3DRepresentObjectDataFlowProcessor*>::iterator i = m_mapInnerRepProcessor.find(uHandle);
	if(i!=m_mapInnerRepProcessor.end())
	{
		KG3DRepresentObjectDataFlowProcessor* pProcessor = i->second;
		SAFE_RELEASE(pProcessor);
		m_mapInnerRepProcessor.erase(i);
	}
	map<UINT,KG3DDataFlowProcessor*>::iterator t = m_mapInnerProcessor.find(uHandle);
	if(t!=m_mapInnerProcessor.end())
	{
		KG3DDataFlowProcessor* pProcessor = t->second;
		SAFE_RELEASE(pProcessor);
		m_mapInnerProcessor.erase(t);
	}

	return m_lpEditor->m_lpProcessorTable->ReleaseProcessor(uHandle);

}

HRESULT KG3DDataFlowProcessor::NewOneInnerArc(UINT* pHandle)
{
	HRESULT hr = S_OK;
	KG3DDataFlowArc* pArc = NULL;
	KG3DRepresentObjectDataFlowArc* pObject = NULL;

	hr = m_lpEditor->m_lpArcTable->NewOneArc(pHandle);
	KGLOG_COM_PROCESS_ERROR(hr);

	hr = m_lpEditor->m_lpArcTable->FindArc(&pArc,*pHandle);
	KGLOG_COM_PROCESS_ERROR(hr);

	hr = g_GetObjectTable().NewOneResourse(REPRESENTOBJECT_DATAFLOWARC,(IKG3DResourceBase**)&pObject);
	KGLOG_COM_PROCESS_ERROR(hr);

	pObject->Init();
	pObject->SetArc(pArc);

	m_mapInnerArc[(*pHandle)] = pArc;
	m_mapInnerRepArc[(*pHandle)] = pObject;
	return S_OK;
Exit0:
	return E_FAIL;
}

HRESULT KG3DDataFlowProcessor::DeleteInnerArc(UINT uHandle)
{
	map<UINT,KG3DRepresentObjectDataFlowArc*>::iterator i = m_mapInnerRepArc.find(uHandle);//数据定义
	if(i!=m_mapInnerRepArc.end())
	{
		KG3DRepresentObjectDataFlowArc* pArc = i->second;
		SAFE_RELEASE(pArc);
		m_mapInnerRepArc.erase(i);
	}
	map<UINT,KG3DDataFlowArc*>::iterator t = m_mapInnerArc.find(uHandle);//数据定义
	if(t!=m_mapInnerArc.end())
	{
		m_mapInnerArc.erase(t);
	}
	return m_lpEditor->m_lpArcTable->ReleaseArc(uHandle);
}

HRESULT KG3DDataFlowProcessor::Refresh(KG3DSceneDataFlowEditor* pEditor)
{
	{
		map<UINT,KG3DRepresentObjectDataFlowProcessor*>::iterator i = m_mapInnerRepProcessor.begin();
		map<UINT,KG3DRepresentObjectDataFlowProcessor*>::iterator iend = m_mapInnerRepProcessor.end();
		while (i!=iend)
		{
			KG3DRepresentObjectDataFlowProcessor* pObject = i->second;
			pObject->Refresh(pEditor);
			pObject->ComputeBBox();
			i++;
		}
	}
	{
		map<UINT,KG3DRepresentObjectDataFlowArc*>::iterator i = m_mapInnerRepArc.begin();
		map<UINT,KG3DRepresentObjectDataFlowArc*>::iterator iend = m_mapInnerRepArc.end();
		while (i!=iend)
		{
			KG3DRepresentObjectDataFlowArc* pObject = i->second;
			pObject->Refresh(pEditor);
			i++;
		}
	}
	return S_OK;
}

KG3DDataFlowProcessorDataModifier* KG3DDataFlowProcessor::GetInputDataModifier(DWORD dwIndex)
{
	HRESULT hr = S_OK;

	KGLOG_PROCESS_ERROR( dwIndex < (DWORD) m_vecInputNode.size());

	ConnectionNode& vNode = m_vecInputNode[dwIndex];

	KGLOG_PROCESS_ERROR(vNode.bHasInnerNode);

	KG3DDataFlowProcessorDataModifier* pModify = NULL;
	hr = m_lpEditor->m_lpProcessorTable->FindProcessor((KG3DDataFlowProcessor**)&pModify,vNode.uInnerNodeHandle);
	KGLOG_COM_PROCESS_ERROR(hr);

	return pModify;

Exit0:
	return NULL;
}

KG3DDataFlowProcessorDataModifier* KG3DDataFlowProcessor::GetOutputDataModifier(DWORD dwIndex)
{
	HRESULT hr = S_OK;

	KGLOG_PROCESS_ERROR( dwIndex < (DWORD) m_vecOutputNode.size());

	ConnectionNode& vNode = m_vecOutputNode[dwIndex];

	KGLOG_PROCESS_ERROR(vNode.bHasInnerNode);

	KG3DDataFlowProcessorDataModifier* pModify = NULL;
	hr = m_lpEditor->m_lpProcessorTable->FindProcessor((KG3DDataFlowProcessor**)&pModify,vNode.uInnerNodeHandle);
	KGLOG_COM_PROCESS_ERROR(hr);

	return pModify;

Exit0:
	return NULL;
}
