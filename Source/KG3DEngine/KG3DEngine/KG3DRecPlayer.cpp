#include "StdAfx.h"
#include "KG3DSceneSceneEditor.h"
#include "KG3DResourcePacker.h"
#include "KG_InterlockedVariable.h"
#include "..\..\..\Include\IKG3DSoundShell.h"
#include "KG3DRecPlayer.h"
#include "KG3DGraphiceEngine.h"

KG3DRecPlayer::KG3DRecPlayer()
{
	m_PlayState = PLAY_STATE_NONE;;
	m_mapMasters.clear();
	m_EventList.clear();
	m_hWnd = NULL;
	m_pCurrentScene = NULL;
	m_pCurrentEventIt = m_EventList.end();
	m_dwRecPlayStartTime = 0;
	m_ulRefCount = 0;
	m_bCount = FALSE;
}
KG3DRecPlayer::~KG3DRecPlayer()
{
	UnInit();
}
HRESULT KG3DRecPlayer::RePlay() 
{
	HRESULT hResult = E_FAIL;
	if (m_PlayState == PLAY_STATE_NONE)
	{
		MastersUnit();
		m_PlayState = PLAY_STATE_PLAY;
		m_pCurrentScene   = NULL;
		m_dwRecPlayStartTime = timeGetTime();//g_cGraphicsTool.GetNowTime();
        m_pCurrentEventIt = m_EventList.begin();
		//IKG3DSoundShell *pSoundShell = g_cEngineManager.GetSound();
		//if (pSoundShell)
		//	pSoundShell->Lock();
		std::list<EventNode*>::iterator m_itLoopFrom = m_EventList.end();
	}
	else if (m_PlayState == PLAY_STATE_PAUSE)
	{
		AdjustStartTime();
		m_PlayState = PLAY_STATE_PLAY;
	}
	else
	{
		KG_PROCESS_ERROR(false);
	}
	hResult = S_OK;
Exit0:
	return hResult;
}
HRESULT KG3DRecPlayer::PauseRePlay()
{
	if (m_PlayState == PLAY_STATE_PLAY)
	{
		m_PlayState = PLAY_STATE_PAUSE;
		return S_OK;
	}
	return E_FAIL;
}

HRESULT KG3DRecPlayer::StopRePlay()
{
	MastersUnit();
	m_PlayState = PLAY_STATE_NONE;;
	m_mapMasters.clear();
	m_pCurrentScene = NULL;
	m_pCurrentEventIt = m_EventList.end();
	m_dwRecPlayStartTime = 0;
	m_bCount = FALSE;
	return S_OK;
}

HRESULT KG3DRecPlayer::LoadRecordFile(LPCSTR szFileName)
{
	HRESULT hrResult = E_FAIL;
	int nRetCode = false;
	TCHAR szSection[MAX_PATH];
	IIniFile *pIniFile = NULL;
	int nType = FT_NONE;
	int nEventNum = 0;
	
	pIniFile = g_OpenIniFile(szFileName);
	KGLOG_PROCESS_ERROR(pIniFile);
	
	UnInit();

	nRetCode = pIniFile->GetInteger("Global", "Num", 0, &nEventNum);
	KGLOG_PROCESS_ERROR(nRetCode);
	nRetCode = pIniFile->GetInteger("Global", "LoopStart", 0, (int*)(&m_dwLoopStartID));
	KGLOG_PROCESS_ERROR(nRetCode);
	nRetCode = pIniFile->GetInteger("Global", "LoopEnd", 0, (int*)(&m_dwLoopEndID));
	KGLOG_PROCESS_ERROR(nRetCode);
	nRetCode = pIniFile->GetInteger("Global", "Playtype", 0, (int*)(&m_PlayType));
	KGLOG_PROCESS_ERROR(nRetCode);

	szSection[0] = '\0';
	while (pIniFile->GetNextSection(szSection, szSection))
	{
		int id = atoi(szSection);
		if ((id <= 0) && (strcmp(szSection, "0") != 0))
			continue;
		EventNode* pNode = new EventNode();
		pNode->m_dwID = id;
		nRetCode = pIniFile->GetInteger(szSection, "Time", 0,(int*)(&(pNode->m_dwTime)));
		KGLOG_PROCESS_ERROR(nRetCode);
		
		nRetCode = pIniFile->GetInteger(szSection, "Type", FT_NONE, &nType);
		KGLOG_PROCESS_ERROR(nRetCode);
		KG3DRecPlayerCommand* pCommand = CommandFactory(*((FunctionType*)(&nType)));
		pCommand->LoadCommand(pIniFile,szSection);
		pNode->m_pCommand = pCommand;
		m_EventList.push_back(pNode);		
	}
	hrResult = S_OK;
Exit0:
	KG_COM_RELEASE(pIniFile);
	return hrResult;
}
HRESULT KG3DRecPlayer::SetReplayWindow(HWND hWnd) 
{	
	m_hWnd = hWnd;
	return S_OK;
}

HRESULT KG3DRecPlayer::FrameMoveRePlay()
{
	HRESULT hr = E_FAIL;
	if (m_PlayState == PLAY_STATE_PLAY)
	{
		DWORD dwNowTime = timeGetTime();//g_cGraphicsTool.GetNowTime();
		DWORD dwStepTime = dwNowTime - m_dwRecPlayStartTime;
		std::list<EventNode*>::iterator beginIt = m_EventList.begin();
		m_bCount = FALSE;
		while (
			(m_pCurrentEventIt != m_EventList.end()) &&
			((*m_pCurrentEventIt)->m_dwTime - (*beginIt)->m_dwTime <= dwStepTime)
			)
		{
			if ((*m_pCurrentEventIt)->m_pCommand)
				(*m_pCurrentEventIt)->m_pCommand->ExcuteCommand();

			if (m_PlayType == PLAYTYPELOOP)
			{
				if ((*m_pCurrentEventIt)->m_dwID == m_dwLoopStartID)
				{
						m_itLoopFrom = m_pCurrentEventIt;
						m_bCount = TRUE;
				}

				if ((*m_pCurrentEventIt)->m_dwID == m_dwLoopEndID)
				{
					m_pCurrentEventIt = m_itLoopFrom;
					m_dwRecPlayStartTime = timeGetTime();
					break;
				}
			}
			else if (m_PlayType == PLAYTYPEONCE)
			{
				if ((*m_pCurrentEventIt)->m_dwID == m_dwLoopStartID)
				{
					m_bCount = TRUE;
				}

				if ((*m_pCurrentEventIt)->m_dwID == m_dwLoopEndID)
				{
					m_bCount = FALSE;
				}		
			}
			++m_pCurrentEventIt;
		}
		if (m_pCurrentEventIt == m_EventList.end())
		{
			m_PlayState = PLAY_STATE_NONE;
		}
	}

	if (m_pCurrentScene != NULL && m_PlayState != PLAY_STATE_NONE)
	{
		KG3DCamera* pCamera = g_cGraphicsTool.GetCamera();

		if (pCamera)
		{
			D3DXVECTOR3 vPosition;
			pCamera->GetLookAtPosition(&vPosition);
			m_pCurrentScene->SetFocus(vPosition);
		}
		hr = g_cGraphicsEngine.CheckDevice();
		KG_COM_PROCESS_ERROR(hr);
		m_pCurrentScene->FrameMove();
		m_pCurrentScene->Render(0);
	}
	hr = S_OK;
Exit0:
	return hr;
}

BOOL KG3DRecPlayer::IsPlayFinished()
{
	return (m_pCurrentEventIt == m_EventList.end());
}
HRESULT KG3DRecPlayer::GetLoadingProgress(float* pnLoadFilePer,float* pnStepToEventPer,int* pnLoadScenePer) 
{
	return S_OK;
}
HRESULT KG3DRecPlayer::OnResizeWindow(PRECT const pRect) 
{
	HRESULT hr = E_FAIL;
	KG_PROCESS_ERROR(m_pCurrentScene);
	KG_COM_PROCESS_ERROR(m_pCurrentScene->OnResizeWindow(NULL));
	hr = S_OK;
Exit0:
	return hr;
}
KG3DRecPlayerCommand* KG3DRecPlayer::CommandFactory(FunctionType eType)
{
	KG3DRecPlayerCommand* pCommand = NULL;
	switch (eType)
	{
	case FT_KG3DSCENEEDITOR_LOADFROMFILE: 		
		pCommand =  new KG3DSceneLoadFromFileCommand();
		break;
	case FT_KG3DSCENEEDITOR_ADDOUTPUTWINDOW:
		pCommand =  new KG3DSceneRenderCommand();
		break;
	case FT_KG3DSCENEEDITOR_DESTRUCT:
		pCommand =  new KG3DSceneUnloadCommand();
		break;
	case FT_KG3DMODEL_LOADFROMFILE:
		pCommand =  new KG3DActorLoadCommand();
		break;
	case FT_KG3DMODEL_DESTRUCT:
		pCommand =  new KG3DActorUnloadCommand();
		break;
	case FT_KG3DSCENEEDITOR_ADDRENDERENTITY:
		pCommand =  new KG3DActorAddedToSceneCommand();
		break;
	case FT_KG3DSCENEEDITOR_REMOVERENDERENTITY:
		pCommand =  new KG3DActorRemoveFromSceneCommand();
		break;
	case FT_KG3DMODEL_PLAYANIMATION:
		pCommand =  new KG3DActorPlayAnimationCommand();
		break;
	default:
		pCommand = NULL;
	}
	if (pCommand != NULL)
	{
		pCommand->SetMaster(this);
	}
	return pCommand;
}

HRESULT KG3DRecPlayer::UnInit()
{
	MastersUnit();
	EvnetListUnit();
	return S_OK;
}

HRESULT KG3DRecPlayer::MastersUnit()
{
	map<DWORD,Master>::reverse_iterator itMasters = m_mapMasters.rbegin();
	while (itMasters != m_mapMasters.rend())
	{
		switch ((itMasters->second).mtType)
		{
		case TYPESCENEEDITOR:
			{
				IKG3DScene* pScene = (IKG3DScene*)((itMasters->second).pMaster);
				g_cEngineManager.RemoveScene(pScene);
				(itMasters->second).pMaster = NULL;
			}
			break;
		case TYPEMODEL:
			{
				IKG3DModel* pModel = (IKG3DModel*)((itMasters->second).pMaster);
				//SAFE_RELEASE(pModel);
				if (pModel)
				{
					SAFE_RELEASE(pModel);
				}
				pModel = NULL;
				(itMasters->second).pMaster = NULL;
			}
			break;
		case TYPECAMERA:
			{
				//camera为非指针，自行释放
			}
		default:
			break;
		}
		itMasters++;
	}
	m_mapMasters.clear();
	return S_OK;
}
HRESULT KG3DRecPlayer::EvnetListUnit()
{
	list<EventNode*>::iterator it    = m_EventList.begin();
	list<EventNode*>::iterator itEnd = m_EventList.end();
	while (it != itEnd)
	{
		SAFE_DELETE(*it)
		++it;
	}
	m_EventList.clear();
	return S_OK;
}

HRESULT KG3DRecPlayer::AdjustStartTime()
{
	std::list<EventNode*>::iterator beginIt = m_EventList.begin();
	if (m_EventList.end() != beginIt && m_pCurrentEventIt != m_EventList.end())
		m_dwRecPlayStartTime = /*g_cGraphicsTool.GetNowTime()*/timeGetTime() - ((*m_pCurrentEventIt)->m_dwTime - (*beginIt)->m_dwTime);
	return S_OK;
}

HRESULT __stdcall KG3DRecPlayer::QueryInterface(const IID& iid, void** ppv)
{
	return S_OK;
}

ULONG __stdcall KG3DRecPlayer::AddRef()
{
	return (ULONG)KG_InterlockedIncrement((long *)&m_ulRefCount);
}

ULONG __stdcall KG3DRecPlayer::Release()
{
	ULONG uNewRefCount = (ULONG)KG_InterlockedDecrement((long *)&m_ulRefCount);
	if (uNewRefCount == 0)
		delete this; 
	return uNewRefCount;
}
