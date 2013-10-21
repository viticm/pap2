#include "StdAfx.h"
#include "KG3DSceneDataFlowEditor.h"
#include "KG3DGraphicsTool.h"
#include "KG3DDataFlowDataDefine.h"
#include "KG3DDataFlowProcessorTable.h"
#include "KG3DRepresentObjectTable.h"
#include "KG3DRepresentObjectDataFlowProcessor.h"
#include "KG3DDataFlowArcTable.h"
#include "KG3DDataFlowArc.h"
#include "KG3DRepresentObjectDataFlowArc.h"
#include "KG3DDataFlowDataObjectTable.h"

#ifdef _DEBUG
#define new DEBUG_NEW_3DENGINE
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

KG3DSceneDataFlowEditor::KG3DSceneDataFlowEditor(void)
{
	m_uCurrentDataDefineIndex = 1;
	//////////////////////////////////////////////////////////////////////////
	m_bRunning = FALSE;

	m_uProcessorSystemKeyInput = 0;

	m_uDefineSystemKeyInput = 0;

	m_lpProcessorTable = new KG3DDataFlowProcessorTable;
	m_lpArcTable = new KG3DDataFlowArcTable;
	m_lpDataTable = new KG3DDataFlowDataObjectTable;

	m_uDefineINT = 0;
	m_uDefineDCOLOR = 0;
	m_uDefineFLOAT = 0;
	m_uDefineFLOAT2 = 0;
	m_uDefineFLOAT3 = 0;
	m_uDefineFLOAT4 = 0;
	m_uDefineCHAR32 = 0;
	m_uDefineCHAR128 = 0;
	m_uDefineCHAR256 = 0;
	m_uDefineWCHAR32 = 0;
	m_uDefineWCHAR128 = 0;
	m_uDefineWCHAR256 = 0;

	m_uCurrentSelectedProcessor = 0;

}

KG3DSceneDataFlowEditor::~KG3DSceneDataFlowEditor(void)
{
	ClearnUp();
}

HRESULT KG3DSceneDataFlowEditor::RenderForEachWindow(KG3DSceneOutputWnd& wndCur)
{
	KG3DSceneOutputWnd* pWnd = &wndCur;
	switch(pWnd->GetType())
	{
	case OUTPUTWND_DATAFLOWEDITOR_MAIN:
		RenderMainWindow(wndCur);
		break;
	case OUTPUTWND_DATAFLOWEDITOR_PREVIEW:
		RenderPreViewWindow(pWnd);
		break;
	case OUTPUTWND_DATAFLOWEDITOR_RUN:
		RenderRunWindow(pWnd);
		break;
	}
	return S_OK;
}

HRESULT KG3DSceneDataFlowEditor::RenderRunWindow(KG3DSceneOutputWnd* pWnd)
{
	RenderWindowBegin(pWnd,TRUE);

	g_cGraphicsTool.DrawGroundGrid(100,0xFF404040);

	RenderWindowEnd(pWnd);

	return S_OK;
}

HRESULT KG3DSceneDataFlowEditor::RenderMainWindow(KG3DSceneOutputWnd& wndCur)
{
	KG3DSceneOutputWnd* pWnd = &wndCur;
	HRESULT hr = S_OK;

	RenderWindowBegin(pWnd,TRUE);

	g_cGraphicsTool.DrawGroundGrid(100,0xFF404040);

	switch(this->GetEditState())
	{
	case EDITSTATE_MAIN:
		{
			{
				map<UINT,KG3DRepresentObjectDataFlowProcessor*>::iterator i = m_mapRepProcessor.begin();
				map<UINT,KG3DRepresentObjectDataFlowProcessor*>::iterator iend = m_mapRepProcessor.end();
				while (i!=iend)
				{
					KG3DRepresentObjectDataFlowProcessor* pObject = i->second;
					pObject->Render(0);
					i++;
				}
			}
			{
				map<UINT,KG3DRepresentObjectDataFlowArc*>::iterator i = m_mapRepArc.begin();
				map<UINT,KG3DRepresentObjectDataFlowArc*>::iterator iend = m_mapRepArc.end();
				while (i!=iend)
				{
					KG3DRepresentObjectDataFlowArc* pObject = i->second;
					pObject->Render(0);
					i++;
				}
			}
			break;
		}
	case EDITSTATE_PROCESSOR:
		{
			KG3DDataFlowProcessor* pProcessor = NULL;
			hr = m_lpProcessorTable->FindProcessor(&pProcessor,m_uCurrentSelectedProcessor);
			if (SUCCEEDED(hr))
			{
				{
					map<UINT,KG3DRepresentObjectDataFlowProcessor*>::iterator i = pProcessor->m_mapInnerRepProcessor.begin();
					map<UINT,KG3DRepresentObjectDataFlowProcessor*>::iterator iend = pProcessor->m_mapInnerRepProcessor.end();
					while (i!=iend)
					{
						KG3DRepresentObjectDataFlowProcessor* pObject = i->second;
						pObject->Render(0);
						i++;
					}
				}
				{
					map<UINT,KG3DRepresentObjectDataFlowArc*>::iterator i = pProcessor->m_mapInnerRepArc.begin();
					map<UINT,KG3DRepresentObjectDataFlowArc*>::iterator iend = pProcessor->m_mapInnerRepArc.end();
					while (i!=iend)
					{
						KG3DRepresentObjectDataFlowArc* pObject = i->second;
						pObject->Render(0);
						i++;
					}
				}
			}
		}
	}
	

	D3DXMATRIX Proj,View;
	Proj = pWnd->GetCamera().GetProjectionMatrix();
	View = pWnd->GetCamera().GetViewMatrix();

	RenderSelectedEntityMark(this->GetEditState(),pWnd->m_Viewport,Proj,View,TRUE);

	RenderWindowEnd(pWnd);
	return S_OK;
}

HRESULT KG3DSceneDataFlowEditor::RenderPreViewWindow(KG3DSceneOutputWnd* pWnd)
{
	RenderWindowBegin(pWnd,TRUE);

	RenderWindowEnd(pWnd);
	return S_OK;
}

HRESULT KG3DSceneDataFlowEditor::ClearnUp()
{
	{
		map<UINT,KG3DDataFlowDataDefine*>::iterator i = m_mapDataDefine.begin();//数据定义
		map<UINT,KG3DDataFlowDataDefine*>::iterator iend = m_mapDataDefine.end();
		while (i!=iend)
		{
			KG3DDataFlowDataDefine* pDefine = i->second;
			SAFE_DELETE(pDefine);
			i++;
		}
		m_mapDataDefine.clear();

	}



	//////////////////////////////////////////////////////////////////////////
	{
		map<UINT,KG3DRepresentObjectDataFlowProcessor*>::iterator i = m_mapRepProcessor.begin();
		map<UINT,KG3DRepresentObjectDataFlowProcessor*>::iterator iend = m_mapRepProcessor.end();
		while (i!=iend)
		{
			KG3DRepresentObjectDataFlowProcessor* pObject = i->second;
			SAFE_RELEASE(pObject);
			i++;
		}
		m_mapRepProcessor.clear();
	}
	//////////////////////////////////////////////////////////////////////////
	{
		map<UINT,KG3DRepresentObjectDataFlowArc*>::iterator i = m_mapRepArc.begin();
		map<UINT,KG3DRepresentObjectDataFlowArc*>::iterator iend = m_mapRepArc.end();
		while (i!=iend)
		{
			KG3DRepresentObjectDataFlowArc* pObject = i->second;
			SAFE_RELEASE(pObject);
			i++;
		}
		m_mapRepArc.clear();
	}

	ClearnUpProcessorInnerObjects();

	SAFE_DELETE(m_lpProcessorTable);
	SAFE_DELETE(m_lpArcTable);
	SAFE_DELETE(m_lpDataTable);
	return S_OK;
}

//////////////////////////////////////////////////////////////////////////
//针对数据定义的操作
HRESULT KG3DSceneDataFlowEditor::NewOneDataDefine(UINT* pHandle)
{
	KGLOG_PROCESS_ERROR(pHandle);

	KG3DDataFlowDataDefine* pNewDefine = new KG3DDataFlowDataDefine();
	KGLOG_PROCESS_ERROR(pNewDefine);

	UINT uNew = GetCurrentDataDefineHandle();

	m_mapDataDefine[uNew] = pNewDefine;
	(*pHandle) = uNew;

	pNewDefine->SetName("NoNameDataDefine");

	return S_OK;
Exit0:
	return E_FAIL;
}

HRESULT KG3DSceneDataFlowEditor::DeleteDataDefine(UINT uHandle)
{
	map<UINT,KG3DDataFlowDataDefine*>::iterator iFind = m_mapDataDefine.find(uHandle);//数据定义
	if (iFind == m_mapDataDefine.end())
	{
		return E_FAIL;
	}
	else
	{
		KG3DDataFlowDataDefine* pDefine = iFind->second;
		SAFE_DELETE(pDefine);
		m_mapDataDefine.erase(iFind);
		return S_OK;
	}
}

HRESULT KG3DSceneDataFlowEditor::SetDataDefineName(LPCTSTR pName,UINT uHandle)
{
	KG3DDataFlowDataDefine* pDefine = FindDataDefine(uHandle);
	KGLOG_PROCESS_ERROR(pDefine);

	return pDefine->SetName(pName);
Exit0:
	return E_FAIL;
}

HRESULT KG3DSceneDataFlowEditor::GetDataDefineName(LPCTSTR pName,UINT uHandle)
{
	KG3DDataFlowDataDefine* pDefine = FindDataDefine(uHandle);
	KGLOG_PROCESS_ERROR(pDefine);

	return pDefine->GetName(pName);
Exit0:
	return E_FAIL;
}


HRESULT KG3DSceneDataFlowEditor::SetNumVariableofDataDefine(DWORD dwNumVariable,UINT uHandle)
{
	KG3DDataFlowDataDefine* pDefine = FindDataDefine(uHandle);
	KGLOG_PROCESS_ERROR(pDefine);

	return pDefine->SetNumVariable(dwNumVariable);
Exit0:
	return E_FAIL;
}

HRESULT KG3DSceneDataFlowEditor::GetNumVariableofDataDefine(DWORD* pdwNumVariable,UINT uHandle)
{
	KG3DDataFlowDataDefine* pDefine = FindDataDefine(uHandle);
	KGLOG_PROCESS_ERROR(pDefine);

	(*pdwNumVariable) = pDefine->GetNumVariable();
	return S_OK;
Exit0:
	return E_FAIL;
}

HRESULT KG3DSceneDataFlowEditor::SetVariableofDataDefine(DWORD dwIndex,KG3DDATAFLOWVARIABLETYPE eType,LPCTSTR pName,UINT uHandle)
{
	KG3DDataFlowDataDefine* pDefine = FindDataDefine(uHandle);
	KGLOG_PROCESS_ERROR(pDefine);

	return pDefine->SetVariable(dwIndex,eType,pName);
Exit0:
	return E_FAIL;
}

HRESULT KG3DSceneDataFlowEditor::GetVariableofDataDefine(DWORD dwIndex,KG3DDATAFLOWVARIABLETYPE* peType,LPCTSTR pName,UINT uHandle)
{
	KG3DDataFlowDataDefine* pDefine = FindDataDefine(uHandle);
	KGLOG_PROCESS_ERROR(pDefine);

	return pDefine->GetVariable(dwIndex,peType,pName);
Exit0:
	return E_FAIL;
}

UINT KG3DSceneDataFlowEditor::GetCurrentDataDefineHandle()
{
	return m_uCurrentDataDefineIndex++;
}

KG3DDataFlowDataDefine* KG3DSceneDataFlowEditor::FindDataDefine(UINT uHandle)
{
	map<UINT,KG3DDataFlowDataDefine*>::iterator iFind = m_mapDataDefine.find(uHandle);//数据定义
	if (iFind == m_mapDataDefine.end())
	{
		return NULL;
	}
	else
	{
		return iFind->second;
	}
}

HRESULT KG3DSceneDataFlowEditor::GetAllDataDefine(vector<UINT>* pvecHandle)
{
	pvecHandle->clear();

	map<UINT,KG3DDataFlowDataDefine*>::iterator i = m_mapDataDefine.begin();//数据定义
	map<UINT,KG3DDataFlowDataDefine*>::iterator iend = m_mapDataDefine.end();
	while (i!=iend)
	{
		pvecHandle->push_back(i->first);
		i++;
	}

	return S_OK;
}

HRESULT KG3DSceneDataFlowEditor::GetNumVariableType(DWORD* pdwNumType)
{
	(*pdwNumType) = (DWORD)VARIABLETYPE_END;
	return S_OK;
}

HRESULT KG3DSceneDataFlowEditor::GetVariableType(KG3DDATAFLOWVARIABLETYPE* peType,LPCTSTR pTypeName,DWORD dwIndex)
{
	(*peType) = (KG3DDATAFLOWVARIABLETYPE)dwIndex;

	switch((KG3DDATAFLOWVARIABLETYPE)dwIndex)
	{
	case VARIABLETYPE_INT:
		wsprintf((LPTSTR)pTypeName,"整数");
		break;
	case VARIABLETYPE_DCOLOR:
		wsprintf((LPTSTR)pTypeName,"色彩");
		break;

	case VARIABLETYPE_FLOAT:
		wsprintf((LPTSTR)pTypeName,"实数");
		break;

	case VARIABLETYPE_FLOAT2:
		wsprintf((LPTSTR)pTypeName,"二维向量");
		break;

	case VARIABLETYPE_FLOAT3:
		wsprintf((LPTSTR)pTypeName,"三维向量");
		break;

	case VARIABLETYPE_FLOAT4:
		wsprintf((LPTSTR)pTypeName,"四维向量");
		break;

	case VARIABLETYPE_CHAR32:
		wsprintf((LPTSTR)pTypeName,"短字符串");
		break;

	case VARIABLETYPE_CHAR128:
		wsprintf((LPTSTR)pTypeName,"中字符串");
		break;

	case VARIABLETYPE_CHAR256:
		wsprintf((LPTSTR)pTypeName,"长字符串");

		break;

	case VARIABLETYPE_WCHAR32:
		wsprintf((LPTSTR)pTypeName,"通用短字符串");
		break;

	case VARIABLETYPE_WCHAR128:
		wsprintf((LPTSTR)pTypeName,"通用中字符串");
		break;

	case VARIABLETYPE_WCHAR256:
		wsprintf((LPTSTR)pTypeName,"通用长字符串");
		break;
	default:
		wsprintf((LPTSTR)pTypeName,"未知/错误的变量类型");
		return E_FAIL;
		break;
	}
	return S_OK;
}

HRESULT KG3DSceneDataFlowEditor::SaveDataDefineToFile(LPCTSTR pFileName)
{
	return S_OK;
}

HRESULT KG3DSceneDataFlowEditor::LoadDataDefineFromFile(LPCTSTR pFileName)
{
	return S_OK;
}

HRESULT KG3DSceneDataFlowEditor::NewOneDataProcessor(UINT* pHandle,DWORD dwType)
{
	HRESULT hr = S_OK;
	KG3DDataFlowProcessor* pProcess = NULL;
	KG3DRepresentObjectDataFlowProcessor* pObject = NULL;

	hr = m_lpProcessorTable->NewOneProcessor(pHandle,(KG3DDataFlowProcessorType)dwType);
	KGLOG_COM_PROCESS_ERROR(hr);

	hr = m_lpProcessorTable->FindProcessor(&pProcess,(*pHandle));
	KGLOG_COM_PROCESS_ERROR(hr);

	hr = g_GetObjectTable().NewOneResourse(REPRESENTOBJECT_DATAFLOWPROCESSOR,(IKG3DResourceBase**)&pObject);
	KGLOG_COM_PROCESS_ERROR(hr);

	pProcess->Init(this);
	pProcess->AddRef();
	pObject->SetProcessor(pProcess);
	pObject->ComputeBBox();
	{
		D3DXVECTOR3 V(0,0,0);
		V.x = m_mapRepProcessor.size() * 500.0F;
		pObject->SetTranslation(&V);
	}

	m_mapRepProcessor[(*pHandle)] = pObject;

	{
		/*KG3DSceneEntity Entity(SCENEENTITY_OBJECT,pObject);
		this->GetSelectionTool().SetSelectable(Entity, TRUE);*/
		this->IEInsertEntitySelectable(SCENEENTITY_OBJECT, pObject, TRUE);//<Selectable Refactor>
	}
	return S_OK;
Exit0:
	return E_FAIL;
}

HRESULT KG3DSceneDataFlowEditor::DeleteDataProcessor(UINT uHandle)
{
	map<UINT,KG3DRepresentObjectDataFlowProcessor*>::iterator i = m_mapRepProcessor.find(uHandle);
	if(i!=m_mapRepProcessor.end())
	{
		KG3DRepresentObjectDataFlowProcessor* pProcessor = i->second;
		SAFE_RELEASE(pProcessor);
		m_mapRepProcessor.erase(i);
	}

	return m_lpProcessorTable->ReleaseProcessor(uHandle);

}

HRESULT KG3DSceneDataFlowEditor::GetAllDataProcessorType(vector<DWORD>*pvecProcessorType)
{
	KGLOG_PROCESS_ERROR(pvecProcessorType);

	pvecProcessorType->clear();

	for (DWORD i=0;i<(DWORD)DATA_PROCESSOR_END;i++)
	{
		pvecProcessorType->push_back(i);
	}

	return S_OK;
Exit0:
	return E_FAIL;
}

HRESULT KG3DSceneDataFlowEditor::GetDataProcessorDescriptionByType(LPCTSTR pDesc,DWORD dwType)
{
	switch(dwType)
	{
	case DATA_PROCESSOR_NONE:
		wsprintf((LPTSTR)pDesc,"未知/错误的处理器类型");
		break;
	case DATA_PROCESSOR_DATACREATER:
		wsprintf((LPTSTR)pDesc,"数据创建处理器");
		break;
	case DATA_PROCESSOR_DATADESTROYER:
		wsprintf((LPTSTR)pDesc,"数据销毁处理器");
		break;
	case DATA_PROCESSOR_ALGORITHM:
		wsprintf((LPTSTR)pDesc,"计算处理器");
		break;
	case DATA_PROCESSOR_SYSTEMINPUT:
		wsprintf((LPTSTR)pDesc,"系统信号输入");
		break;
	case DATA_PROCESSOR_SYSTEMOUTPUT:
		wsprintf((LPTSTR)pDesc,"系统信号输出");
		break;

	default:
		wsprintf((LPTSTR)pDesc,"未知/错误的处理器类型");
		break;

	}
	return S_OK;
}

HRESULT KG3DSceneDataFlowEditor::GetAllDataProcessor(vector<UINT>* pvecHandle)
{
	return m_lpProcessorTable->GetAllDataProcessor(pvecHandle);
}

HRESULT KG3DSceneDataFlowEditor::Init()
{
	UINT uHandleP1 = 0;
	UINT uHandleP2 = 0;
	UINT uHandleA1 = 0;
	UINT uHandleA2 = 0;
	CreateDefaultDataDefine();


	NewOneDataProcessor(&uHandleP2,DATA_PROCESSOR_SYSTEMINPUT);
	m_uProcessorSystemKeyInput = uHandleP2;

	NewOneDataProcessor(&uHandleP1,DATA_PROCESSOR_ALGORITHM);

	NewOneArc(&uHandleA1);
	SetArcType(uHandleA1,m_uDefineSystemKeyInput,CONTAINER_LIST);
	SetNumArcInputConnection(uHandleA1,1);
	SetNumArcOutputConnection(uHandleA1,1);
	ConnectArcToProcessorOutput(uHandleA1,0,uHandleP2,0);
	ConnectArcToProcessorInput(uHandleA1,0,uHandleP1,0);

	NewOneArc(&uHandleA2);
	SetArcType(uHandleA2,m_uDefineINT,CONTAINER_SINGLE);
	SetNumArcInputConnection(uHandleA2,0);
	SetNumArcOutputConnection(uHandleA2,1);
	ConnectArcToProcessorInput(uHandleA2,0,uHandleP1,1);

	KG3DDataFlowProcessor* pProcessor = NULL;
	m_lpProcessorTable->FindProcessor(&pProcessor,uHandleP1);
	pProcessor->CreateInnerDataModifer();
	{
		UINT uInnerArc = 0;
		pProcessor->NewOneInnerArc(&uInnerArc);

		KG3DDataFlowProcessorDataModifier* pModifier1 = pProcessor->GetInputDataModifier(0);
		KG3DDataFlowProcessorDataModifier* pModifier2 = pProcessor->GetOutputDataModifier(0);
		
		SetArcType(uInnerArc,m_uDefineINT,CONTAINER_LIST);
		SetNumArcInputConnection(uInnerArc,1);
		SetNumArcOutputConnection(uInnerArc,1);
		ConnectArcToProcessorOutput(uInnerArc,0,pModifier1->GetHandle(),VK_UP + 1);
		ConnectArcToProcessorInput( uInnerArc,0,pModifier2->GetHandle(),0);
	}

	Refresh();

	return S_OK;
}

HRESULT KG3DSceneDataFlowEditor::CreateDefaultDataDefine()
{
	HRESULT hr = S_OK;

	UINT uHandle = 0;

	{
		hr = NewOneDataDefine(&uHandle);
		KGLOG_COM_PROCESS_ERROR(hr);
		m_uDefineINT = uHandle;

		hr = SetDataDefineName("INT",uHandle);
		KGLOG_COM_PROCESS_ERROR(hr);

		hr = SetNumVariableofDataDefine(1,uHandle);
		KGLOG_COM_PROCESS_ERROR(hr);

		hr = SetVariableofDataDefine(0,VARIABLETYPE_INT,"Variable",uHandle);
		KGLOG_COM_PROCESS_ERROR(hr);
	}
	{
		hr = NewOneDataDefine(&uHandle);
		KGLOG_COM_PROCESS_ERROR(hr);
		m_uDefineDCOLOR = uHandle;

		hr = SetDataDefineName("DWORD Color",uHandle);
		KGLOG_COM_PROCESS_ERROR(hr);

		hr = SetNumVariableofDataDefine(1,uHandle);
		KGLOG_COM_PROCESS_ERROR(hr);

		hr = SetVariableofDataDefine(0,VARIABLETYPE_DCOLOR,"Variable",uHandle);
		KGLOG_COM_PROCESS_ERROR(hr);
	}
	{
		hr = NewOneDataDefine(&uHandle);
		KGLOG_COM_PROCESS_ERROR(hr);
		m_uDefineFLOAT = uHandle;

		hr = SetDataDefineName("FLOAT",uHandle);
		KGLOG_COM_PROCESS_ERROR(hr);

		hr = SetNumVariableofDataDefine(1,uHandle);
		KGLOG_COM_PROCESS_ERROR(hr);

		hr = SetVariableofDataDefine(0,VARIABLETYPE_FLOAT,"Variable",uHandle);
		KGLOG_COM_PROCESS_ERROR(hr);
	}
	{
		hr = NewOneDataDefine(&uHandle);
		KGLOG_COM_PROCESS_ERROR(hr);
		m_uDefineFLOAT2 = uHandle;

		hr = SetDataDefineName("FLOAT2",uHandle);
		KGLOG_COM_PROCESS_ERROR(hr);

		hr = SetNumVariableofDataDefine(1,uHandle);
		KGLOG_COM_PROCESS_ERROR(hr);

		hr = SetVariableofDataDefine(0,VARIABLETYPE_FLOAT2,"Variable",uHandle);
		KGLOG_COM_PROCESS_ERROR(hr);
	}
	{
		hr = NewOneDataDefine(&uHandle);
		KGLOG_COM_PROCESS_ERROR(hr);
		m_uDefineFLOAT3 = uHandle;

		hr = SetDataDefineName("FLOAT3",uHandle);
		KGLOG_COM_PROCESS_ERROR(hr);

		hr = SetNumVariableofDataDefine(1,uHandle);
		KGLOG_COM_PROCESS_ERROR(hr);

		hr = SetVariableofDataDefine(0,VARIABLETYPE_FLOAT3,"Variable",uHandle);
		KGLOG_COM_PROCESS_ERROR(hr);
	}
	{
		hr = NewOneDataDefine(&uHandle);
		KGLOG_COM_PROCESS_ERROR(hr);
		m_uDefineFLOAT4 = uHandle;

		hr = SetDataDefineName("FLOAT4",uHandle);
		KGLOG_COM_PROCESS_ERROR(hr);

		hr = SetNumVariableofDataDefine(1,uHandle);
		KGLOG_COM_PROCESS_ERROR(hr);

		hr = SetVariableofDataDefine(0,VARIABLETYPE_FLOAT4,"Variable",uHandle);
		KGLOG_COM_PROCESS_ERROR(hr);
	}
	{
		hr = NewOneDataDefine(&uHandle);
		KGLOG_COM_PROCESS_ERROR(hr);
		m_uDefineCHAR32 = uHandle;

		hr = SetDataDefineName("Char32",uHandle);
		KGLOG_COM_PROCESS_ERROR(hr);

		hr = SetNumVariableofDataDefine(1,uHandle);
		KGLOG_COM_PROCESS_ERROR(hr);

		hr = SetVariableofDataDefine(0,VARIABLETYPE_CHAR32,"Variable",uHandle);
		KGLOG_COM_PROCESS_ERROR(hr);
	}
	{
		hr = NewOneDataDefine(&uHandle);
		KGLOG_COM_PROCESS_ERROR(hr);
		m_uDefineCHAR128 = uHandle;

		hr = SetDataDefineName("Char128",uHandle);
		KGLOG_COM_PROCESS_ERROR(hr);

		hr = SetNumVariableofDataDefine(1,uHandle);
		KGLOG_COM_PROCESS_ERROR(hr);

		hr = SetVariableofDataDefine(0,VARIABLETYPE_CHAR128,"Variable",uHandle);
		KGLOG_COM_PROCESS_ERROR(hr);
	}
	{
		hr = NewOneDataDefine(&uHandle);
		KGLOG_COM_PROCESS_ERROR(hr);
		m_uDefineCHAR256 = uHandle;

		hr = SetDataDefineName("Char256",uHandle);
		KGLOG_COM_PROCESS_ERROR(hr);

		hr = SetNumVariableofDataDefine(1,uHandle);
		KGLOG_COM_PROCESS_ERROR(hr);

		hr = SetVariableofDataDefine(0,VARIABLETYPE_CHAR256,"Variable",uHandle);
		KGLOG_COM_PROCESS_ERROR(hr);
	}
	{
		hr = NewOneDataDefine(&uHandle);
		KGLOG_COM_PROCESS_ERROR(hr);
		m_uDefineWCHAR32 = uHandle;

		hr = SetDataDefineName("WChar32",uHandle);
		KGLOG_COM_PROCESS_ERROR(hr);

		hr = SetNumVariableofDataDefine(1,uHandle);
		KGLOG_COM_PROCESS_ERROR(hr);

		hr = SetVariableofDataDefine(0,VARIABLETYPE_WCHAR32,"Variable",uHandle);
		KGLOG_COM_PROCESS_ERROR(hr);
	}
	{
		hr = NewOneDataDefine(&uHandle);
		KGLOG_COM_PROCESS_ERROR(hr);
		m_uDefineWCHAR128 = uHandle;

		hr = SetDataDefineName("WChar128",uHandle);
		KGLOG_COM_PROCESS_ERROR(hr);

		hr = SetNumVariableofDataDefine(1,uHandle);
		KGLOG_COM_PROCESS_ERROR(hr);

		hr = SetVariableofDataDefine(0,VARIABLETYPE_WCHAR128,"Variable",uHandle);
		KGLOG_COM_PROCESS_ERROR(hr);
	}
	{
		hr = NewOneDataDefine(&uHandle);
		KGLOG_COM_PROCESS_ERROR(hr);
		m_uDefineWCHAR256 = uHandle;

		hr = SetDataDefineName("WChar256",uHandle);
		KGLOG_COM_PROCESS_ERROR(hr);

		hr = SetNumVariableofDataDefine(1,uHandle);
		KGLOG_COM_PROCESS_ERROR(hr);

		hr = SetVariableofDataDefine(0,VARIABLETYPE_WCHAR256,"Variable",uHandle);
		KGLOG_COM_PROCESS_ERROR(hr);
	}

	CreateSystemKeyInputDefine();

	return S_OK;
Exit0:
	return E_FAIL;
}

HRESULT	KG3DSceneDataFlowEditor::CreateSystemKeyInputDefine()
{
	HRESULT hr = S_OK;
	UINT uHandle = 0;

	hr = NewOneDataDefine(&uHandle);
	KGLOG_COM_PROCESS_ERROR(hr);
	m_uDefineSystemKeyInput = uHandle;

	hr = SetDataDefineName("SystemKeyInput",uHandle);
	KGLOG_COM_PROCESS_ERROR(hr);

	hr = SetNumVariableofDataDefine(0xFF,uHandle);
	KGLOG_COM_PROCESS_ERROR(hr);

	{
		for (int i=0;i<0xFF;i++)
		{
			SetVariableofDataDefine(i,VARIABLETYPE_INT,"unassigned",uHandle);
		}
	}
	//////////////////////////////////////////////////////////////////////////
	SetVariableofDataDefine(0x01,VARIABLETYPE_INT,"VK_LBUTTON",uHandle);
	SetVariableofDataDefine(0x02,VARIABLETYPE_INT,"VK_RBUTTON",uHandle);
	SetVariableofDataDefine(0x03,VARIABLETYPE_INT,"VK_CANCEL",uHandle);
	SetVariableofDataDefine(0x04,VARIABLETYPE_INT,"VK_MBUTTON",uHandle);

	SetVariableofDataDefine(0x05,VARIABLETYPE_INT,"VK_XBUTTON1",uHandle);
	SetVariableofDataDefine(0x06,VARIABLETYPE_INT,"VK_XBUTTON2",uHandle);

	SetVariableofDataDefine(0x08,VARIABLETYPE_INT,"VK_BACK",uHandle);
	SetVariableofDataDefine(0x09,VARIABLETYPE_INT,"VK_TAB",uHandle);


	SetVariableofDataDefine(0x0C,VARIABLETYPE_INT,"VK_CLEAR",uHandle);
	SetVariableofDataDefine(0x0D,VARIABLETYPE_INT,"VK_RETURN",uHandle);

	SetVariableofDataDefine(0x10,VARIABLETYPE_INT,"VK_SHIFT",uHandle);
	SetVariableofDataDefine(0x11,VARIABLETYPE_INT,"VK_CONTROL",uHandle);
	SetVariableofDataDefine(0x12,VARIABLETYPE_INT,"VK_MENU",uHandle);
	SetVariableofDataDefine(0x13,VARIABLETYPE_INT,"VK_PAUSE",uHandle);
	SetVariableofDataDefine(0x14,VARIABLETYPE_INT,"VK_CAPITAL",uHandle);

	SetVariableofDataDefine(0x15,VARIABLETYPE_INT,"VK_KANA",uHandle);
	SetVariableofDataDefine(0x17,VARIABLETYPE_INT,"VK_JUNJA",uHandle);
	SetVariableofDataDefine(0x18,VARIABLETYPE_INT,"VK_FINAL",uHandle);
	SetVariableofDataDefine(0x19,VARIABLETYPE_INT,"VK_KANJI",uHandle);

	SetVariableofDataDefine(0x1B,VARIABLETYPE_INT,"VK_ESCAPE",uHandle);

	SetVariableofDataDefine(0x1C,VARIABLETYPE_INT,"VK_CONVERT",uHandle);
	SetVariableofDataDefine(0x1D,VARIABLETYPE_INT,"VK_NONCONVERT",uHandle);
	SetVariableofDataDefine(0x1E,VARIABLETYPE_INT,"VK_ACCEPT",uHandle);
	SetVariableofDataDefine(0x1F,VARIABLETYPE_INT,"VK_MODECHANGE",uHandle);

	SetVariableofDataDefine(0x20,VARIABLETYPE_INT,"VK_SPACE",uHandle);
	SetVariableofDataDefine(0x21,VARIABLETYPE_INT,"VK_PRIOR",uHandle);
	SetVariableofDataDefine(0x22,VARIABLETYPE_INT,"VK_NEXT",uHandle);
	SetVariableofDataDefine(0x23,VARIABLETYPE_INT,"VK_END",uHandle);
	SetVariableofDataDefine(0x24,VARIABLETYPE_INT,"VK_HOME",uHandle);
	SetVariableofDataDefine(0x25,VARIABLETYPE_INT,"VK_LEFT",uHandle);
	SetVariableofDataDefine(0x26,VARIABLETYPE_INT,"VK_UP",uHandle);
	SetVariableofDataDefine(0x27,VARIABLETYPE_INT,"VK_RIGHT",uHandle);
	SetVariableofDataDefine(0x28,VARIABLETYPE_INT,"VK_DOWN",uHandle);
	SetVariableofDataDefine(0x29,VARIABLETYPE_INT,"VK_SELECT",uHandle);
	SetVariableofDataDefine(0x2A,VARIABLETYPE_INT,"VK_PRINT",uHandle);
	SetVariableofDataDefine(0x2B,VARIABLETYPE_INT,"VK_EXECUTE",uHandle);
	SetVariableofDataDefine(0x2C,VARIABLETYPE_INT,"VK_SNAPSHOT",uHandle);
	SetVariableofDataDefine(0x2D,VARIABLETYPE_INT,"VK_INSERT",uHandle);
	SetVariableofDataDefine(0x2E,VARIABLETYPE_INT,"VK_DELETE",uHandle);
	SetVariableofDataDefine(0x2F,VARIABLETYPE_INT,"VK_HELP",uHandle);

	{
		for (int i=0;i<=9;i++)
		{
			TCHAR Name[256];
			wsprintf(Name,"VK_%d",i);
			SetVariableofDataDefine(0x30 + i,VARIABLETYPE_INT,Name,uHandle);
		}
	}
	{
		for (int i=0;i<=26;i++)
		{
			TCHAR Name[256];
			wsprintf(Name,"VK_%c",i+'A');
			SetVariableofDataDefine(0x41 + i,VARIABLETYPE_INT,Name,uHandle);
		}
	}

	SetVariableofDataDefine(0x5B,VARIABLETYPE_INT,"VK_LWIN",uHandle);
	SetVariableofDataDefine(0x5C,VARIABLETYPE_INT,"VK_RWIN",uHandle);
	SetVariableofDataDefine(0x5D,VARIABLETYPE_INT,"VK_APPS",uHandle);

	SetVariableofDataDefine(0x5F,VARIABLETYPE_INT,"VK_SLEEP",uHandle);

	SetVariableofDataDefine(0x60,VARIABLETYPE_INT,"VK_NUMPAD0",uHandle);
	SetVariableofDataDefine(0x61,VARIABLETYPE_INT,"VK_NUMPAD1",uHandle);
	SetVariableofDataDefine(0x62,VARIABLETYPE_INT,"VK_NUMPAD2",uHandle);
	SetVariableofDataDefine(0x63,VARIABLETYPE_INT,"VK_NUMPAD3",uHandle);
	SetVariableofDataDefine(0x64,VARIABLETYPE_INT,"VK_NUMPAD4",uHandle);
	SetVariableofDataDefine(0x65,VARIABLETYPE_INT,"VK_NUMPAD5",uHandle);
	SetVariableofDataDefine(0x66,VARIABLETYPE_INT,"VK_NUMPAD6",uHandle);
	SetVariableofDataDefine(0x67,VARIABLETYPE_INT,"VK_NUMPAD7",uHandle);
	SetVariableofDataDefine(0x68,VARIABLETYPE_INT,"VK_NUMPAD8",uHandle);
	SetVariableofDataDefine(0x69,VARIABLETYPE_INT,"VK_NUMPAD9",uHandle);
	SetVariableofDataDefine(0x6A,VARIABLETYPE_INT,"VK_MULTIPLY",uHandle);
	SetVariableofDataDefine(0x6B,VARIABLETYPE_INT,"VK_ADD",uHandle);
	SetVariableofDataDefine(0x6C,VARIABLETYPE_INT,"VK_SEPARATOR",uHandle);
	SetVariableofDataDefine(0x6D,VARIABLETYPE_INT,"VK_SUBTRACT",uHandle);
	SetVariableofDataDefine(0x6E,VARIABLETYPE_INT,"VK_DECIMAL",uHandle);
	SetVariableofDataDefine(0x6F,VARIABLETYPE_INT,"VK_DIVIDE",uHandle);

	SetVariableofDataDefine(0x70,VARIABLETYPE_INT,"VK_F1",uHandle);
	SetVariableofDataDefine(0x71,VARIABLETYPE_INT,"VK_F2",uHandle);
	SetVariableofDataDefine(0x72,VARIABLETYPE_INT,"VK_F3",uHandle);
	SetVariableofDataDefine(0x73,VARIABLETYPE_INT,"VK_F4",uHandle);
	SetVariableofDataDefine(0x74,VARIABLETYPE_INT,"VK_F5",uHandle);
	SetVariableofDataDefine(0x75,VARIABLETYPE_INT,"VK_F6",uHandle);
	SetVariableofDataDefine(0x76,VARIABLETYPE_INT,"VK_F7",uHandle);
	SetVariableofDataDefine(0x77,VARIABLETYPE_INT,"VK_F8",uHandle);
	SetVariableofDataDefine(0x78,VARIABLETYPE_INT,"VK_F9",uHandle);
	SetVariableofDataDefine(0x79,VARIABLETYPE_INT,"VK_F10",uHandle);
	SetVariableofDataDefine(0x7A,VARIABLETYPE_INT,"VK_F11",uHandle);
	SetVariableofDataDefine(0x7B,VARIABLETYPE_INT,"VK_F12",uHandle);
	SetVariableofDataDefine(0x7C,VARIABLETYPE_INT,"VK_F13",uHandle);
	SetVariableofDataDefine(0x7D,VARIABLETYPE_INT,"VK_F14",uHandle);
	SetVariableofDataDefine(0x7E,VARIABLETYPE_INT,"VK_F15",uHandle);
	SetVariableofDataDefine(0x7F,VARIABLETYPE_INT,"VK_F16",uHandle);
	SetVariableofDataDefine(0x80,VARIABLETYPE_INT,"VK_F17",uHandle);
	SetVariableofDataDefine(0x81,VARIABLETYPE_INT,"VK_F18",uHandle);
	SetVariableofDataDefine(0x82,VARIABLETYPE_INT,"VK_F19",uHandle);
	SetVariableofDataDefine(0x83,VARIABLETYPE_INT,"VK_F20",uHandle);
	SetVariableofDataDefine(0x84,VARIABLETYPE_INT,"VK_F21",uHandle);
	SetVariableofDataDefine(0x85,VARIABLETYPE_INT,"VK_F22",uHandle);
	SetVariableofDataDefine(0x86,VARIABLETYPE_INT,"VK_F23",uHandle);
	SetVariableofDataDefine(0x87,VARIABLETYPE_INT,"VK_F24",uHandle);


	SetVariableofDataDefine(0x90,VARIABLETYPE_INT,"VK_NUMLOCK",uHandle);
	SetVariableofDataDefine(0x91,VARIABLETYPE_INT,"VK_SCROLL",uHandle);

	SetVariableofDataDefine(0xA0,VARIABLETYPE_INT,"VK_LSHIFT",uHandle);
	SetVariableofDataDefine(0xA1,VARIABLETYPE_INT,"VK_RSHIFT",uHandle);
	SetVariableofDataDefine(0xA2,VARIABLETYPE_INT,"VK_LCONTROL",uHandle);
	SetVariableofDataDefine(0xA3,VARIABLETYPE_INT,"VK_RCONTROL",uHandle);
	SetVariableofDataDefine(0xA4,VARIABLETYPE_INT,"VK_LMENU",uHandle);
	SetVariableofDataDefine(0xA5,VARIABLETYPE_INT,"VK_RMENU",uHandle);

	hr = S_OK;
Exit0:
	return hr;
}

HRESULT KG3DSceneDataFlowEditor::NewOneArc(UINT* pHandle)
{
	HRESULT hr = S_OK;
	KG3DDataFlowArc* pArc = NULL;
	KG3DRepresentObjectDataFlowArc* pObject = NULL;

	hr = m_lpArcTable->NewOneArc(pHandle);
	KGLOG_COM_PROCESS_ERROR(hr);

	hr = m_lpArcTable->FindArc(&pArc,*pHandle);
	KGLOG_COM_PROCESS_ERROR(hr);

	hr = g_GetObjectTable().NewOneResourse(REPRESENTOBJECT_DATAFLOWARC,(IKG3DResourceBase**)&pObject);
	KGLOG_COM_PROCESS_ERROR(hr);

	pObject->Init();
	pObject->SetArc(pArc);

	m_mapRepArc[(*pHandle)] = pObject;
	return S_OK;
Exit0:
	return E_FAIL;
}

HRESULT KG3DSceneDataFlowEditor::DeleteArc(UINT uHandle)
{
	map<UINT,KG3DRepresentObjectDataFlowArc*>::iterator i = m_mapRepArc.find(uHandle);//数据定义
	if(i!=m_mapRepArc.end())
	{
		KG3DRepresentObjectDataFlowArc* pArc = i->second;
		SAFE_RELEASE(pArc);
		m_mapRepArc.erase(i);
	}
	return m_lpArcTable->ReleaseArc(uHandle);
}


HRESULT KG3DSceneDataFlowEditor::SetArcName(UINT uHandle,LPCTSTR pName)
{
	HRESULT hr = E_FAIL;
	KG3DDataFlowArc* pArc = NULL;
	hr = m_lpArcTable->FindArc(&pArc,uHandle);
	KGLOG_COM_PROCESS_ERROR(hr);

	return pArc->SetName(pName);

Exit0:
	return E_FAIL;
}

HRESULT KG3DSceneDataFlowEditor::GetArcName(UINT uHandle,LPCTSTR pName)
{
	HRESULT hr = E_FAIL;
	KG3DDataFlowArc* pArc = NULL;
	hr = m_lpArcTable->FindArc(&pArc,uHandle);
	KGLOG_COM_PROCESS_ERROR(hr);

	return pArc->GetName(pName);

Exit0:
	return E_FAIL;
}

HRESULT KG3DSceneDataFlowEditor::SetArcType(UINT uHandle,UINT uDataDefine,DWORD dwContainerType)
{
	HRESULT hr = E_FAIL;
	KG3DDataFlowArc* pArc = NULL;
	hr = m_lpArcTable->FindArc(&pArc,uHandle);
	KGLOG_COM_PROCESS_ERROR(hr);

	return pArc->SetType(uDataDefine,dwContainerType);

Exit0:
	return E_FAIL;
}

HRESULT KG3DSceneDataFlowEditor::GetArcType(UINT uHandle,UINT* puDataDefine,DWORD* pdwContainerType)
{
	HRESULT hr = E_FAIL;
	KG3DDataFlowArc* pArc = NULL;
	hr = m_lpArcTable->FindArc(&pArc,uHandle);
	KGLOG_COM_PROCESS_ERROR(hr);

	return pArc->GetType(puDataDefine,pdwContainerType);

Exit0:
	return E_FAIL;
}

HRESULT KG3DSceneDataFlowEditor::SetNumArcInputConnection(UINT uHandle,DWORD dwNum)
{
	HRESULT hr = E_FAIL;
	KG3DDataFlowArc* pArc = NULL;
	hr = m_lpArcTable->FindArc(&pArc,uHandle);
	KGLOG_COM_PROCESS_ERROR(hr);

	return pArc->SetNumInputConnection(dwNum);

Exit0:
	return E_FAIL;
}

HRESULT KG3DSceneDataFlowEditor::GetNumArcInputConnection(UINT uHandle,DWORD* pdwNum)
{
	HRESULT hr = E_FAIL;
	KG3DDataFlowArc* pArc = NULL;
	hr = m_lpArcTable->FindArc(&pArc,uHandle);
	KGLOG_COM_PROCESS_ERROR(hr);

	return pArc->GetNumInputConnection(pdwNum);

Exit0:
	return E_FAIL;
}

HRESULT KG3DSceneDataFlowEditor::SetNumArcOutputConnection(UINT uHandle,DWORD dwNum)
{
	HRESULT hr = E_FAIL;
	KG3DDataFlowArc* pArc = NULL;
	hr = m_lpArcTable->FindArc(&pArc,uHandle);
	KGLOG_COM_PROCESS_ERROR(hr);

	return pArc->SetNumOutputConnection(dwNum);

Exit0:
	return E_FAIL;
}

HRESULT KG3DSceneDataFlowEditor::GetNumArcOutputConnection(UINT uHandle,DWORD* pdwNum)
{
	HRESULT hr = E_FAIL;
	KG3DDataFlowArc* pArc = NULL;
	hr = m_lpArcTable->FindArc(&pArc,uHandle);
	KGLOG_COM_PROCESS_ERROR(hr);

	return pArc->GetNumOutputConnection(pdwNum);

Exit0:
	return E_FAIL;
}


HRESULT KG3DSceneDataFlowEditor::ConnectArcToProcessorOutput(UINT uArcHandle,DWORD dwArcConnectionIndex,UINT uProcessorHandle,DWORD dwProcessorConnectionIndex)
{
	HRESULT hr = E_FAIL;
	KG3DDataFlowArc* pArc = NULL;
	hr = m_lpArcTable->FindArc(&pArc,uArcHandle);
	KGLOG_COM_PROCESS_ERROR(hr);

	KG3DDataFlowProcessor* pProcessor = NULL;
	hr = m_lpProcessorTable->FindProcessor(&pProcessor,uProcessorHandle);
	if(pProcessor)
	{
		hr = pArc->SetInputConnection(dwArcConnectionIndex,uProcessorHandle,pProcessor,dwProcessorConnectionIndex);
		KGLOG_COM_PROCESS_ERROR(hr);

		hr = pProcessor->ConnectArcOutputNode(dwProcessorConnectionIndex,uArcHandle,pArc,dwArcConnectionIndex);
		KGLOG_COM_PROCESS_ERROR(hr);
	}
	else
	{
		hr = pArc->SetInputConnection(dwArcConnectionIndex,0,NULL,0);
		KGLOG_COM_PROCESS_ERROR(hr);
	}

	return S_OK;
Exit0:
	return E_FAIL;
}

HRESULT KG3DSceneDataFlowEditor::ConnectArcToProcessorInput(UINT uArcHandle,DWORD dwArcConnectionIndex,UINT uProcessorHandle,DWORD dwProcessorConnectionIndex)
{
	HRESULT hr = E_FAIL;
	KG3DDataFlowArc* pArc = NULL;
	hr = m_lpArcTable->FindArc(&pArc,uArcHandle);
	KGLOG_COM_PROCESS_ERROR(hr);

	KG3DDataFlowProcessor* pProcessor = NULL;
	hr = m_lpProcessorTable->FindProcessor(&pProcessor,uProcessorHandle);
	if(pProcessor)
	{
		hr = pArc->SetOutputConnection(dwArcConnectionIndex,uProcessorHandle,pProcessor,dwProcessorConnectionIndex);
		KGLOG_COM_PROCESS_ERROR(hr);

		hr = pProcessor->ConnectArcInputNode(dwProcessorConnectionIndex,uArcHandle,pArc,dwArcConnectionIndex);
		KGLOG_COM_PROCESS_ERROR(hr);
	}
	else
	{
		hr = pArc->SetOutputConnection(dwArcConnectionIndex,0,NULL,0);
		KGLOG_COM_PROCESS_ERROR(hr);
	}

	return S_OK;
Exit0:
	return E_FAIL;
}

HRESULT KG3DSceneDataFlowEditor::GetAllArc(vector<UINT>* pvecHandle)
{
	return m_lpArcTable->GetAllArc(pvecHandle);
}

HRESULT KG3DSceneDataFlowEditor::Refresh()
{

	{
		map<UINT,KG3DRepresentObjectDataFlowProcessor*>::iterator i = m_mapRepProcessor.begin();
		map<UINT,KG3DRepresentObjectDataFlowProcessor*>::iterator iend = m_mapRepProcessor.end();
		while (i!=iend)
		{
			KG3DRepresentObjectDataFlowProcessor* pObject = i->second;
			pObject->Refresh(this);
			pObject->ComputeBBox();
			i++;
		}
	}
	{
		map<UINT,KG3DRepresentObjectDataFlowArc*>::iterator i = m_mapRepArc.begin();
		map<UINT,KG3DRepresentObjectDataFlowArc*>::iterator iend = m_mapRepArc.end();
		while (i!=iend)
		{
			KG3DRepresentObjectDataFlowArc* pObject = i->second;
			pObject->Refresh(this);
			i++;
		}
	}
	return S_OK;
}

HRESULT KG3DSceneDataFlowEditor::FindRepProcessor(KG3DRepresentObjectDataFlowProcessor** ppObject,UINT uHandle)
{
	map<UINT,KG3DRepresentObjectDataFlowProcessor*>::iterator i = m_mapRepProcessor.find(uHandle);
	if (i!=m_mapRepProcessor.end())
	{
		(*ppObject) = i->second;
		return S_OK;
	}
	else
	{
		return E_FAIL;
	}
}

HRESULT KG3DSceneDataFlowEditor::FindRepArc(KG3DRepresentObjectDataFlowArc** ppObject,UINT uHandle)
{
	map<UINT,KG3DRepresentObjectDataFlowArc*>::iterator i = m_mapRepArc.find(uHandle);
	if (i!=m_mapRepArc.end())
	{
		(*ppObject) = i->second;
		return S_OK;
	}
	else
	{
		return E_FAIL;
	}
}

HRESULT KG3DSceneDataFlowEditor::SetProcessorInputNode(UINT uHandle,DWORD dwIndex,UINT uDataDefine,DWORD dwContainer)
{
	HRESULT hr = S_OK;
	KG3DDataFlowProcessor* pProcessor = NULL;
	hr = m_lpProcessorTable->FindProcessor(&pProcessor,uHandle);
	KGLOG_COM_PROCESS_ERROR(hr);

	return pProcessor->SetInputNodeType(dwIndex,uDataDefine,dwContainer);
Exit0:
	return E_FAIL;
}

HRESULT KG3DSceneDataFlowEditor::GetProcessorInputNode(UINT uHandle,DWORD dwIndex,UINT* puDataDefine,DWORD* pdwContainer,BOOL& bDefineLock,BOOL& bContainerLock)//获取连接点信息，后两个参数为确定是否数据定义固定和容器固定
{
	HRESULT hr = S_OK;
	KG3DDataFlowProcessor* pProcessor = NULL;
	hr = m_lpProcessorTable->FindProcessor(&pProcessor,uHandle);
	KGLOG_COM_PROCESS_ERROR(hr);

	return pProcessor->GetInputNodeType(dwIndex,puDataDefine,pdwContainer,bDefineLock,bContainerLock);
Exit0:
	return E_FAIL;
}

HRESULT KG3DSceneDataFlowEditor::SetProcessorOutputNode(UINT uHandle,DWORD dwIndex,UINT uDataDefine,DWORD dwContainer)
{
	HRESULT hr = S_OK;
	KG3DDataFlowProcessor* pProcessor = NULL;
	hr = m_lpProcessorTable->FindProcessor(&pProcessor,uHandle);
	KGLOG_COM_PROCESS_ERROR(hr);

	return pProcessor->SetOutputNodeType(dwIndex,uDataDefine,dwContainer);
Exit0:
	return E_FAIL;
}

HRESULT KG3DSceneDataFlowEditor::GetProcessorOutputNode(UINT uHandle,DWORD dwIndex,UINT* puDataDefine,DWORD* pdwContainer,BOOL& bDefineLock,BOOL& bContainerLock)//获取连接点信息，后两个参数为确定是否数据定义固定和容器固定
{
	HRESULT hr = S_OK;
	KG3DDataFlowProcessor* pProcessor = NULL;
	hr = m_lpProcessorTable->FindProcessor(&pProcessor,uHandle);
	KGLOG_COM_PROCESS_ERROR(hr);

	return pProcessor->GetOutputNodeType(dwIndex,puDataDefine,pdwContainer,bDefineLock,bContainerLock);
Exit0:
	return E_FAIL;
}

HRESULT KG3DSceneDataFlowEditor::ProcessRunMessage(UINT message, WPARAM wParam, LPARAM lParam)
{
	HRESULT hr = S_OK;
	if (!m_bRunning)
		return S_OK;

	if (message == WM_KEYDOWN)
	{
		if(m_uProcessorSystemKeyInput)
		{
			KG3DDataFlowProcessorSystemInput* pInput = NULL;
			hr = m_lpProcessorTable->FindProcessor((KG3DDataFlowProcessor**)&pInput,m_uProcessorSystemKeyInput);
			if(SUCCEEDED(hr))
			{
				pInput->ProcessRunMessage(message,wParam,lParam);
			}
		}
	}
	return S_OK;
}

HRESULT KG3DSceneDataFlowEditor::RunDataFlow(BOOL bRun)
{
	m_bRunning = bRun;
	return S_OK;
}

HRESULT KG3DSceneDataFlowEditor::FrameMove()
{
	return KG3DSceneEditorBase::FrameMove();
}

HRESULT KG3DSceneDataFlowEditor::ToggleEditCurrentSelectedProcessor(BOOL bEdit)
{
	if (bEdit)
	{
		KG3DSceneSelectionTool& selTool = this->GetSelectionTool();
		//KGLOG_PROCESS_ERROR(m_listSelectedEntity.size());
		KG_PROCESS_ERROR(0 != selTool.GetSelectionCount());

		//KG3DSceneEntity Entity = *m_listSelectedEntity.begin();

		selTool.GetSelectionAccessProxy().Begin();
		KG3DSceneEntity Entity = selTool.GetSelectionAccessProxy().GetCur();


		KGLOG_PROCESS_ERROR(Entity.GetType() == SCENEENTITY_OBJECT);
		KG3DRepresentObject* pObject = static_cast<KG3DRepresentObject*>(Entity.m_pPointer);
		KGLOG_PROCESS_ERROR(pObject);
		KGLOG_PROCESS_ERROR(pObject->GetType() == REPRESENTOBJECT_DATAFLOWPROCESSOR);
		KG3DRepresentObjectDataFlowProcessor* pRepProcessor = 
			static_cast<KG3DRepresentObjectDataFlowProcessor*>(pObject);
		KGLOG_PROCESS_ERROR(pRepProcessor);
		KG3DDataFlowProcessor* pProcessor = pRepProcessor->GetProcessor();
		m_uCurrentSelectedProcessor = pProcessor->GetHandle();

		//m_dwEditState = EDITSTATE_PROCESSOR;
		this->SetEditState(EDITSTATE_PROCESSOR);
		
		selTool.ClearSelection();
		selTool.ClearSelectable();

		ClearnUpProcessorInnerObjects();
		//////////////////////////////////////////////////////////////////////////
	}
	else
	{
		//m_dwEditState = EDITSTATE_MAIN;
		this->SetEditState(EDITSTATE_MAIN);

		/*m_listSelectedEntity.clear();
		m_listSelectableEntity.clear();*/
		
		KG3DSceneSelectionTool& selTool = GetSelectionTool();

		selTool.ClearSelection();
		selTool.ClearSelectable();

		ClearnUpProcessorInnerObjects();
		{
			map<UINT,KG3DRepresentObjectDataFlowProcessor*>::iterator i = m_mapRepProcessor.begin();
			map<UINT,KG3DRepresentObjectDataFlowProcessor*>::iterator iend = m_mapRepProcessor.end();
			while (i!=iend)
			{
				KG3DRepresentObjectDataFlowProcessor* pObject = i->second;
				/*KG3DSceneEntity Entity(SCENEENTITY_OBJECT,pObject);
				selTool.SetSelectable(Entity, TRUE);*/
				this->IEInsertEntitySelectable(SCENEENTITY_OBJECT, pObject, TRUE);//<Selectable Refactor>
				i++;
			}
		}
		{
			map<UINT,KG3DRepresentObjectDataFlowArc*>::iterator i = m_mapRepArc.begin();
			map<UINT,KG3DRepresentObjectDataFlowArc*>::iterator iend = m_mapRepArc.end();
			while (i!=iend)
			{
				KG3DRepresentObjectDataFlowArc* pObject = i->second;
				/*KG3DSceneEntity Entity(SCENEENTITY_OBJECT,pObject);
				selTool.SetSelectable(Entity, TRUE);*/
				this->IEInsertEntitySelectable(SCENEENTITY_OBJECT, pObject, TRUE);//<Selectable Refactor>
				i++;
			}
		}
	}

	return S_OK;
Exit0:
	return E_FAIL;
}

HRESULT KG3DSceneDataFlowEditor::ClearnUpProcessorInnerObjects()
{

	return S_OK;
}
