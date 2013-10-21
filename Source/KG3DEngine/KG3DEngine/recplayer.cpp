#include "StdAfx.h"
#include "RecPlayer.h"
#include "KG3DSceneSceneEditor.h"
#include "KG3DResourcePacker.h"
#include "KG_InterlockedVariable.h"
#include "..\..\..\Include\IKG3DSoundShell.h"
#include "KG3DRecPlayer.h"

extern HRESULT g_MultiThreadBoneAniRemove(KG3DModel* pModel);

IRecPlayer* g_GetNewRecPlayer()
{
	return new KG3DRecPlayer;
}

RecPlayer::RecPlayer(void)
{
	m_PlayState = PLAY_STATE_NONE;
	m_dwRecordStartTime = 0;
	m_dwRecPlayStartTime = 0;
	m_hWnd = NULL;
	m_pCurrentScene = NULL;
	m_dwRecPlayPauseTime = 0;
	m_bIsFreeCamera = FALSE;
	m_dwCameraID = 0;
	m_pGhostCameraForFree = NULL;
	
    m_pEvents = NULL;
    m_dwEventsCount = 0;
    m_pCurrentEvent = NULL;
    m_pbyRecordFileCache = NULL;
	
    m_fPlaySpeed = 1;
	m_dwRecordStartLineTime = 0;
	m_dwRecordStopLineTime = 0;

	m_dwBindID = 42;
	m_bIsBindIDCreated = FALSE;
	m_CameraType = CAMERA_TYPE_RECORD;
	m_fAngle = D3DX_PI;

	m_dwCameraPastFromTime = 0;
	m_dwCameraPastToTime = 0;
	m_dwCameraNowFromTime = 0;
	m_vecFromPos = D3DXVECTOR3(0,0,0);
	m_vecToPos = D3DXVECTOR3(0,0,0);

	m_bIsMovieing = FALSE;
	m_pEventMovieStart = NULL;
	m_pEventMovieStop = NULL;

	m_dwNowFrameCount = 0;
	m_PlayType = PLAYTYPE_BYTIME;

	m_pOldScene = NULL;
	m_nOldWindowID = 0;
	m_dwOldCameraID = 0;

	m_dwRecordLastFrameTime = 0;
	m_dwNowLastFrameTime = 0;
	m_fSmoothSpeed = 1;

	m_fLoadFilePer = 0;
	m_fStepToEventPer = 0;
	m_nLoadScenePer = 0;

    m_ListEventHead.clear();
    m_dwCurrentEvents = 0;

    m_ulRefCount = 0;

    m_bChanged = false;
}

HRESULT __stdcall RecPlayer::QueryInterface(const IID& iid, void** ppv)
{
    return S_OK;
}

ULONG __stdcall RecPlayer::AddRef()
{
    return (ULONG)KG_InterlockedIncrement((long *)&m_ulRefCount);
}

ULONG __stdcall RecPlayer::Release()
{
    ULONG uNewRefCount = (ULONG)KG_InterlockedDecrement((long *)&m_ulRefCount);
    if (uNewRefCount == 0)
        delete this; 
    return uNewRefCount;
}

HRESULT RecPlayer::SetReplayWindow(HWND hWnd)
{
	m_hWnd = hWnd;
	return S_OK;
}

RecPlayer::~RecPlayer(void)
{
	UnInit();
}	

// This Function Cause Heap Corruption, Make It No Use Now,I Will Check It Carefully Later.
HRESULT RecPlayer::Init()
{
    EventHead Head;
    EventRecord *pEvent = NULL;
    list<EventHead>::iterator itOutBegin = m_ListEventHead.begin();
    list<EventHead>::iterator itOutEnd   = m_ListEventHead.end();
    list<EventRecord*>::iterator itInBegin;
    list<EventRecord*>::iterator itInEnd;

    m_mapObj.clear();

    for (; itOutBegin != itOutEnd; ++itOutBegin)
    {
        Head = *itOutBegin;
        itInBegin = Head.ListEvent.begin();
        itInEnd   = Head.ListEvent.end();
        for (; itInBegin != itInEnd; ++itInBegin)
        {
            pEvent = *itInBegin;
            switch (pEvent->EventFunction)
            {
    		case FT_NONE:
    			break;
    		case FT_KG3DMODEL_LOADFROMFILE:
    		case FT_KG3DSCENEEDITOR_LOADFROMFILE:
    			{
    				KG_PROCESS_ERROR(m_mapObj.find(pEvent->dwID) == m_mapObj.end());
    				RecObj MyObj;
    				MyObj.pEventHead = &(*pEvent);
    				MyObj.pCurrentEvent = &(*pEvent);
    				pEvent->pNextEvent = NULL;
    
    				TrueType* ttType = (TrueType*)(&(pEvent->AParameters.pData[pEvent->AParameters.nFrom[0]]));
    				KG_PROCESS_ERROR(ttType);
    				MyObj.ObjType = *ttType;
    				char* cszFileName = (char*)(&(pEvent->AParameters.pData[pEvent->AParameters.nFrom[1]]));
    				KG_PROCESS_ERROR(cszFileName);
    				MyObj.ObjName = cszFileName;
    
    				m_mapObj[pEvent->dwID] = MyObj;
    			}
    			break;
    		case FT_KG3DMODEL_SETTRANSLATION:		
    		case FT_KG3DMODEL_SETROTATION:		
    		case FT_KG3DMODEL_SETROTATIONCENTER:		
    		case FT_KG3DMODEL_SETSCALING:	
    		case FT_KG3DMODEL_SETSCALINGCENTER:		
    		case FT_KG3DMODEL_SETSCALINGROTATION:		
    		case FT_KG3DMODEL_RESETTRANSFORM:
    		case FT_KG3DMODEL_BINDTOSOCKET:	
    		case FT_KG3DMODEL_BINDTOBONE:		
    		case FT_KG3DMODEL_ATTACHMODEL:		
    		case FT_KG3DMODEL_DETCHMODEL:		
    		case FT_KG3DMODEL_PLAYANIMATION:		
    		case FT_KG3DMODEL_PALYSPLITANIMATION:	
    		case FT_KG3DMODEL_LOADMATERIALFROMFILE:		
    		case FT_KG3DMODEL_SETALPHA:		
    		case FT_KG3DMODEL_SETCALLBACKRENDER:
    		case FT_KG3DMODEL_DESTRUCT:
    
    
    		case FT_KG3DSCENEEDITOR_DESTRUCT:
    		case FT_KG3DSCENEEDITOR_ADDRENDERENTITY:	
    		case FT_KG3DSCENEEDITOR_REMOVERENDERENTITY:
    			{
    				KG_PROCESS_ERROR(m_mapObj.find(pEvent->dwID) != m_mapObj.end());
    				KG_PROCESS_ERROR(m_mapObj[pEvent->dwID].pCurrentEvent);
    				(m_mapObj[pEvent->dwID].pCurrentEvent)->pNextEvent = &(*pEvent);
    				pEvent->pNextEvent = NULL;
    				m_mapObj[pEvent->dwID].pCurrentEvent = &(*pEvent);
    			}
    			break;
    	
    		case FT_KG3DSCENEEDITOR_ADDOUTPUTWINDOW:
    			{				
    				KG_PROCESS_ERROR(m_mapObj.find(pEvent->dwID) != m_mapObj.end());
    				KG_PROCESS_ERROR(m_mapObj[pEvent->dwID].pCurrentEvent);
    				(m_mapObj[pEvent->dwID].pCurrentEvent)->pNextEvent = &(*pEvent);
    				pEvent->pNextEvent = NULL;
    				m_mapObj[pEvent->dwID].pCurrentEvent = &(*pEvent);
    
    				DWORD* pdwCameraID = (DWORD*)(&(pEvent->AParameters.pData[pEvent->AParameters.nFrom[1]])); 
    				KG_PROCESS_ERROR(pdwCameraID);	
    				KG_PROCESS_ERROR(m_mapObj.find(*pdwCameraID) == m_mapObj.end());
    
    				RecObj MyObj;
    				MyObj.pEventHead = NULL;
    				MyObj.pCurrentEvent = NULL;
    				MyObj.ObjType = TT_TRACKCAMERA;
    				MyObj.ObjName = "TrackCamera";

    				m_mapObj[*pdwCameraID] = MyObj;
    			}
    			break;
    		case FT_KG3DCAMERA_UPDATECAMERA:	
    		case FT_KG3DCAMERA_SETPOSITION:		
    		case FT_KG3DCAMERA_SETLOOKATPOSITION:		
    		case FT_KG3DCAMERA_SETUPDIRECTION:		
    		case FT_KG3DCAMERA_SETGLOBALPERSPECTIVE:		
    		case FT_KG3DCAMERA_SETGLOBALORTHOGONAL:
    		case FT_KG3DCAMERA_SETTRACKINFO:
    			{
    				KG_PROCESS_ERROR(m_mapObj.find(pEvent->dwID) != m_mapObj.end());
    				if (m_mapObj[pEvent->dwID].pEventHead == NULL)
    				{
    					m_mapObj[pEvent->dwID].pEventHead = &(*pEvent);
    					m_mapObj[pEvent->dwID].pCurrentEvent = &(*pEvent);
    					pEvent->pNextEvent = NULL;
    				}
    				else
    				{
    					KG_PROCESS_ERROR(m_mapObj[pEvent->dwID].pCurrentEvent);
    					(m_mapObj[pEvent->dwID].pCurrentEvent)->pNextEvent = &(*pEvent);
    					pEvent->pNextEvent = NULL;
    					m_mapObj[pEvent->dwID].pCurrentEvent = &(*pEvent);
    				}
    			}
    			break;		
    		default:
    			break;
            }
        }
    }

Exit0:
	return S_OK;
}

HRESULT RecPlayer::MastersUnit()
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

HRESULT RecPlayer::UnInit()
{
	MastersUnit();
    DeleteHead();
    KG_DELETE_ARRAY(m_pbyRecordFileCache);
	SAFE_DELETE(m_pGhostCameraForFree);
	m_pCurrentScene = NULL;
	return S_OK;
}

HRESULT RecPlayer::SaveRecordFile(LPCSTR szFileName)
{
	HRESULT hResult = E_FAIL;
	unsigned long uSize = 0;
	IFile *pFile = NULL;
    EventRecord *pEvent = NULL;
    EventHead Head;
    list<EventHead>::iterator itOutBegin = m_ListEventHead.begin();
    list<EventHead>::iterator itOutEnd   = m_ListEventHead.end();
    list<EventRecord*>::iterator itInBegin;
    list<EventRecord*>::iterator itInEnd;

    KG_PROCESS_ERROR(szFileName);
	
	pFile = g_OpenFile(szFileName,false,true);
	KG_PROCESS_ERROR(pFile);
	
	uSize = pFile->Write(&m_dwEventsCount,sizeof(int));
	KG_PROCESS_ERROR(uSize == sizeof(int));
	uSize = pFile->Write(&m_dwRecordStartTime,sizeof(DWORD));
	KG_PROCESS_ERROR(uSize == sizeof(DWORD));
	uSize = pFile->Write(&m_dwRecordStartLineTime,sizeof(DWORD));
	KG_PROCESS_ERROR(uSize == sizeof(DWORD));
	uSize = pFile->Write(&m_dwRecordStopLineTime,sizeof(DWORD));
	KG_PROCESS_ERROR(uSize == sizeof(DWORD));

    for (; itOutBegin != itOutEnd; ++itOutBegin)
	{
        Head = *itOutBegin;
        itInBegin = Head.ListEvent.begin();
        itInEnd   = Head.ListEvent.end();
        for (; itInBegin != itInEnd; ++itInBegin)
        {
            pEvent = *itInBegin;
		    uSize = pFile->Write(&(pEvent->dwID),sizeof(DWORD));
		    KG_PROCESS_ERROR(uSize == sizeof(DWORD));
		    uSize = pFile->Write(&(pEvent->EventFunction),sizeof(FunctionType));
		    KG_PROCESS_ERROR(uSize == sizeof(FunctionType));
		    uSize = pFile->Write(&(pEvent->dwTime),sizeof(DWORD));
		    KG_PROCESS_ERROR(uSize == sizeof(DWORD));
		    uSize = pFile->Write(&(pEvent->dwFrame),sizeof(DWORD));
		    KG_PROCESS_ERROR(uSize == sizeof(DWORD));
		    uSize = pFile->Write(&(pEvent->AParameters.size_tDataSize),sizeof(size_t));
		    KG_PROCESS_ERROR(uSize == sizeof(size_t));
		    uSize = pFile->Write(&(pEvent->AParameters.nNum),sizeof(int));
		    KG_PROCESS_ERROR(uSize == sizeof(int));
		    uSize = pFile->Write(pEvent->AParameters.nFrom,(pEvent->AParameters.nNum) * sizeof(int));
		    KG_PROCESS_ERROR(uSize == (pEvent->AParameters.nNum) * sizeof(int));
		    uSize = pFile->Write(pEvent->AParameters.pData,(unsigned long)(pEvent->AParameters.size_tDataSize));
		    KG_PROCESS_ERROR(uSize == pEvent->AParameters.size_tDataSize);
        }
	}
	hResult = S_OK;
Exit0:
	KG_COM_RELEASE(pFile);
	return hResult;
}
HRESULT RecPlayer::LoadRecordFile(LPCSTR szFileName)
{
	HRESULT hResult = E_FAIL;
    IFile* pFile = NULL;
    unsigned long ulFileSize = 0;
    unsigned long ulReadSize = 0;
    unsigned char *pbyReadPos     = NULL;
    unsigned char *pbyReadEndPos  = NULL;
    EventRecord *pEvent           = NULL;
    RECORD_FILE_HEAD *pFileHead   = NULL;
    EVENT_RECORD_IN_FILE *pRecord = NULL;
    EventHead Head;
	
	m_fLoadFilePer = 0;
	m_fStepToEventPer = 0;
	m_nLoadScenePer = 0;

	KGLOG_PROCESS_ERROR(szFileName);

	UnInit();

    pFile= g_OpenFile(szFileName,false,false);
	KG_PROCESS_ERROR(pFile);

    ulFileSize = pFile->Size();
    KG_PROCESS_ERROR(ulFileSize > 0);
    
    ASSERT(!m_pbyRecordFileCache);

    m_pbyRecordFileCache = new unsigned char[ulFileSize];
    KGLOG_PROCESS_ERROR(m_pbyRecordFileCache);
    
    ulReadSize = pFile->Read(m_pbyRecordFileCache, ulFileSize);
    KGLOG_PROCESS_ERROR(ulReadSize == ulFileSize);

    pbyReadPos    = m_pbyRecordFileCache;
    pbyReadEndPos = m_pbyRecordFileCache + ulFileSize;

    ulReadSize = sizeof(*pFileHead);
    KGLOG_PROCESS_ERROR(pbyReadPos + ulReadSize <= pbyReadEndPos);
    pFileHead = (RECORD_FILE_HEAD *)pbyReadPos;
    m_dwEventsCount         = pFileHead->dwEventCount;
    m_dwRecordStartTime     = pFileHead->dwRecordStartTime;
    m_dwRecordStartLineTime = pFileHead->dwRecordStartLineTime;
    // m_dwRecordStartLineTime = 0;
    m_dwRecordStopLineTime  = pFileHead->dwRecordStopLineTime;

    pbyReadPos += ulReadSize;
    
    KG_PROCESS_SUCCESS(m_dwEventsCount == 0);

	DWORD dwReadedEventCount = 0;
    while (dwReadedEventCount < m_dwEventsCount)
	{
        pEvent = new EventRecord();
        KG_PROCESS_ERROR(pEvent);

        ulReadSize = sizeof(*pRecord);
        KGLOG_PROCESS_ERROR(pbyReadPos + ulReadSize <= pbyReadEndPos);
        pRecord = (EVENT_RECORD_IN_FILE *)pbyReadPos;
        pEvent->dwID            = pRecord->dwID;
        pEvent->EventFunction   = pRecord->EventFunction;
        pEvent->dwTime          = pRecord->dwTime;
        pEvent->dwFrame         = pRecord->dwFrame;
        pEvent->AParameters.size_tDataSize = pRecord->uParameters_DataSize;
        pEvent->AParameters.nNum           = pRecord->nParameters_Num;
        pEvent->AParameters.pData = NULL;
        pEvent->pNextEvent        = NULL;
        pbyReadPos += ulReadSize;

        ulReadSize = pEvent->AParameters.nNum * sizeof(int);
        KGLOG_PROCESS_ERROR(pbyReadPos + ulReadSize <= pbyReadEndPos);
        memcpy(pEvent->AParameters.nFrom, pbyReadPos, ulReadSize);
        pbyReadPos += ulReadSize;

        ulReadSize = (unsigned long)pEvent->AParameters.size_tDataSize;
        if (ulReadSize > 0)
        {
            KGLOG_PROCESS_ERROR(pbyReadPos + ulReadSize <= pbyReadEndPos);
            pEvent->AParameters.pData = (char *)pbyReadPos; // not need new buffer,use cache
            pbyReadPos += ulReadSize;
        }
		dwReadedEventCount++;
		m_fLoadFilePer = ((float)dwReadedEventCount) / ((float)(m_dwEventsCount)) * 100;
        if (Head.ListEvent.empty())
        {
            Head.dwFrameStartTime = pEvent->dwTime;
            Head.dwFrameEndTime   = pEvent->dwTime;
            Head.dwFrameID        = pEvent->dwFrame;
            Head.dwEventsCount    = 1;
            Head.ListEvent.clear();
            Head.ListEvent.push_back(pEvent);
        }
        else
        {
            if (Head.dwFrameID == pEvent->dwFrame)
            {
                Head.dwFrameEndTime = pEvent->dwTime;
                Head.dwEventsCount++;
                Head.ListEvent.push_back(pEvent);
            }
            else
            {
                m_ListEventHead.push_back(Head);

                Head.dwFrameStartTime = pEvent->dwTime;
                Head.dwFrameEndTime   = pEvent->dwTime;
                Head.dwFrameID        = pEvent->dwFrame;
                Head.dwEventsCount    = 1;
                Head.ListEvent.clear();
                Head.ListEvent.push_back(pEvent);
            }
        }

        if (dwReadedEventCount == m_dwRecordStartLineTime)
        {
            m_pStartLineEvent = pEvent;
        }

        if (dwReadedEventCount == m_dwEventsCount - 1)
        {
            m_pStopLineEvent = pEvent;
        }
	}
    m_ListEventHead.push_back(Head);

Exit1:
    hResult = S_OK;
Exit0:
    if (FAILED(hResult))
    {
        DeleteHead();
		KG_DELETE_ARRAY(m_pbyRecordFileCache);
    }
	KG_COM_RELEASE(pFile);
	return hResult;
}

void RecPlayer::DeleteHead()
{
    list<EventHead>::iterator it    = m_ListEventHead.begin();
    list<EventHead>::iterator itEnd = m_ListEventHead.end();
    while (it != itEnd)
    {
        (*it).ListClear();
        ++it;
    }
    m_ListEventHead.clear();
}

RecPlayer::PlayState RecPlayer::GetPlayarState()
{
	return m_PlayState;
}

BOOL RecPlayer::IsFreeCamera()
{
	return m_bIsFreeCamera;
}

DWORD RecPlayer::GetNowTime()
{
    DWORD dwResult = 0;

    if (m_dwCurrentEvents == m_dwEventsCount)
    {
        dwResult = GetTotalTime();
    }
    else if (m_dwCurrentEvents == 0)
    {
        dwResult = 0;
    }
    else
    {
        if (m_pCurrentEvent->dwTime < m_pStartLineEvent->dwTime)
            dwResult = 0;
        else
            dwResult = (m_pCurrentEvent->dwTime - m_pStartLineEvent->dwTime) / 1000; // 转换到秒
    }
    return dwResult;
}

DWORD RecPlayer::GetTotalTime()
{
    ASSERT(m_pStopLineEvent);
    ASSERT(m_pStartLineEvent);
    ASSERT(m_pStartLineEvent->dwTime < m_pStopLineEvent->dwTime);

    return (m_pStopLineEvent->dwTime - m_pStartLineEvent->dwTime) / 1000;
}

HRESULT RecPlayer::OnResizeWindow(PRECT const pRect)
{
	HRESULT hr = E_FAIL;
	KG_PROCESS_ERROR(m_pCurrentScene);
	KG_COM_PROCESS_ERROR(m_pCurrentScene->OnResizeWindow(NULL));
	hr = S_OK;
Exit0:
	return hr;
}

HRESULT RecPlayer::ProcessOneEvent(EventRecord *pEvent)
{
	HRESULT hr = E_FAIL;
    
    ASSERT(pEvent);

	if (g_cEngineManager.GetRecordOption()->bKeep)
	{
		if (pEvent == m_pEventMovieStart)
		{
			StartMovieOut();
		}
		if (pEvent == m_pEventMovieStop)
		{
			StopMovieOut();
		}
	}
	switch (pEvent->EventFunction)
	{
	case FT_NONE:
		break;
	case FT_KG3DMODEL_LOADFROMFILE:
		{
			KG_PROCESS_ERROR(m_mapMasters.find(pEvent->dwID) == m_mapMasters.end());
			IKG3DResourceBase* pModelST = NULL;
            KGLOG_PROCESS_ERROR(pEvent->AParameters.pData);
			char* cszFileName = (char*)(&(pEvent->AParameters.pData[pEvent->AParameters.nFrom[1]]));
			KG_PROCESS_ERROR(cszFileName);
			unsigned* puFileNameHash = (unsigned*)(&(pEvent->AParameters.pData[pEvent->AParameters.nFrom[2]])); 
			KG_PROCESS_ERROR(puFileNameHash);
			unsigned* puOption = (unsigned*)(&(pEvent->AParameters.pData[pEvent->AParameters.nFrom[3]]));
			KG_PROCESS_ERROR(puOption);
			g_cModelTable.LoadResourceFromFile(cszFileName,*puFileNameHash,*puOption,&pModelST);
			KG_PROCESS_ERROR(pModelST);
			Master MyMaster;
			MyMaster.mtType = TYPEMODEL;
			MyMaster.pMaster = (PVOID)(pModelST);
			m_mapMasters[pEvent->dwID] = MyMaster;
			if (pEvent->dwID == m_dwBindID)
				m_bIsBindIDCreated = TRUE;
		}
		break;
	case FT_KG3DMODEL_DESTRUCT:
		{
			KG_PROCESS_ERROR(m_mapMasters.find(pEvent->dwID) != m_mapMasters.end());
			IKG3DModel* pModel = static_cast<IKG3DModel*>(m_mapMasters[pEvent->dwID].pMaster);
			SAFE_RELEASE(pModel);
			m_mapMasters.erase(m_mapMasters.find(pEvent->dwID));
		}
		break;
	case FT_KG3DMODEL_SETTRANSLATION:
		{
			KG_PROCESS_ERROR(m_mapMasters.find(pEvent->dwID) != m_mapMasters.end());
			IKG3DModel* pModel = static_cast<IKG3DModel*>(m_mapMasters[pEvent->dwID].pMaster);
			KG_PROCESS_ERROR(pModel);
			KGLOG_PROCESS_ERROR(pEvent->AParameters.pData);
			D3DXVECTOR3* pvecValue = (D3DXVECTOR3*)(&(pEvent->AParameters.pData[pEvent->AParameters.nFrom[0]]));
			KG_PROCESS_ERROR(pvecValue);
			pModel->SetTranslation(pvecValue);
			if (m_CameraType == CAMERA_TYPE_BIND && pEvent->dwID == m_dwBindID)
			{
				D3DXMATRIX matStepRotion;
				D3DXVECTOR3 vecCameraDir,vecLookAt;
				D3DXVECTOR3 vecPos = *pvecValue;
				vecPos.y += 200;
				KG_PROCESS_ERROR(m_mapMasters.find(m_dwCameraID) != m_mapMasters.end());
				IKG3DCamera* pCamera = static_cast<IKG3DCamera*>(m_mapMasters[m_dwCameraID].pMaster);
				KG_PROCESS_ERROR(pCamera);
				pCamera->SetPosition(vecPos);
			
				D3DXMatrixRotationAxis(
					&matStepRotion,
					&D3DXVECTOR3(0,1,0),
					m_fAngle
				);
				vecCameraDir = D3DXVECTOR3(0,0,100);
				D3DXVec3TransformNormal(
					&vecCameraDir,
					&vecCameraDir,
					&matStepRotion
					);
				vecLookAt = vecPos + vecCameraDir;
				pCamera->SetLookAtPosition(vecLookAt);
			}
		}
		break;
	case FT_KG3DMODEL_SETROTATION:
		{
			KG_PROCESS_ERROR(m_mapMasters.find(pEvent->dwID) != m_mapMasters.end());
			IKG3DModel* pModel = static_cast<IKG3DModel*>(m_mapMasters[pEvent->dwID].pMaster);
			KG_PROCESS_ERROR(pModel);
			KGLOG_PROCESS_ERROR(pEvent->AParameters.pData);
			D3DXQUATERNION* pvecValue = (D3DXQUATERNION*)(&(pEvent->AParameters.pData[pEvent->AParameters.nFrom[0]]));
			KG_PROCESS_ERROR(pvecValue);
			pModel->SetRotation(pvecValue);
			//if (m_CameraType == CAMERA_TYPE_BIND && it->dwID == m_dwBindID)
			//{
			//	D3DXVECTOR3 vecCameraPos,vecLookAt,vecCameraDir;
			//	D3DXQUATERNION quaRotion = *pvecValue;
			//	D3DXVECTOR3 vecRotion;
			//	//pModel->GetRotationCenter(&vecRotion);
			//	D3DXMATRIX matRotion,matStepRotion;
			//	D3DXMatrixTransformation(
			//		&matRotion,
			//		NULL,
			//		NULL,
			//		NULL,
			//		NULL,
			//		&quaRotion,
			//		NULL
			//		);
			//	D3DXMatrixRotationAxis(
			//		&matStepRotion,
			//		&D3DXVECTOR3(0,1,0),
			//		m_fAngle
			//	);
			//	D3DXMatrixMultiply(
			//		&matRotion,
			//		&matRotion,
			//		&matStepRotion
			//		);
			//	KG_PROCESS_ERROR(m_mapMasters.find(m_dwCameraID) != m_mapMasters.end());
			//	IKG3DCamera* pCamera = static_cast<IKG3DCamera*>(m_mapMasters[m_dwCameraID].pMaster);
			//	KG_PROCESS_ERROR(pCamera);
			//	pCamera->GetPosition(&vecCameraPos);
			//	pCamera->GetLookAtPosition(&vecLookAt);
			//	//vecCameraDir = vecLookAt - vecCameraPos;
			//	vecCameraDir = D3DXVECTOR3(0,0,100);
			//	D3DXVec3TransformNormal(
			//		&vecCameraDir,
			//		&vecCameraDir,
			//		&matRotion
			//		);
			//	vecLookAt = vecCameraPos + vecCameraDir;
			//	pCamera->SetLookAtPosition(vecLookAt);
			//}
		}
		break;
	case FT_KG3DMODEL_SETROTATIONCENTER:
		{
			KG_PROCESS_ERROR(m_mapMasters.find(pEvent->dwID) != m_mapMasters.end());
			IKG3DModel* pModel = static_cast<IKG3DModel*>(m_mapMasters[pEvent->dwID].pMaster);
			KG_PROCESS_ERROR(pModel);
			KGLOG_PROCESS_ERROR(pEvent->AParameters.pData);
			D3DXVECTOR3* pvecValue = (D3DXVECTOR3*)(&(pEvent->AParameters.pData[pEvent->AParameters.nFrom[0]]));
			KG_PROCESS_ERROR(pvecValue);
			pModel->SetRotationCenter(pvecValue);
		}
		break;
	case FT_KG3DMODEL_SETSCALING:
		{
			KG_PROCESS_ERROR(m_mapMasters.find(pEvent->dwID) != m_mapMasters.end());
			IKG3DModel* pModel = static_cast<IKG3DModel*>(m_mapMasters[pEvent->dwID].pMaster);
			KG_PROCESS_ERROR(pModel);
			KGLOG_PROCESS_ERROR(pEvent->AParameters.pData);
			D3DXVECTOR3* pvecValue = (D3DXVECTOR3*)(&(pEvent->AParameters.pData[pEvent->AParameters.nFrom[0]]));
			KG_PROCESS_ERROR(pvecValue);
			pModel->SetScaling(pvecValue);
		}
		break;
	case FT_KG3DMODEL_SETSCALINGCENTER:
		{
			KG_PROCESS_ERROR(m_mapMasters.find(pEvent->dwID) != m_mapMasters.end());
			IKG3DModel* pModel = static_cast<IKG3DModel*>(m_mapMasters[pEvent->dwID].pMaster);
			KG_PROCESS_ERROR(pModel);
			KGLOG_PROCESS_ERROR(pEvent->AParameters.pData);
			D3DXVECTOR3* pvecValue = (D3DXVECTOR3*)(&(pEvent->AParameters.pData[pEvent->AParameters.nFrom[0]]));
			KG_PROCESS_ERROR(pvecValue);
			pModel->SetScalingCenter(pvecValue);
		}
		break;
	case FT_KG3DMODEL_SETSCALINGROTATION:
		{
			KG_PROCESS_ERROR(m_mapMasters.find(pEvent->dwID) != m_mapMasters.end());
			IKG3DModel* pModel = static_cast<IKG3DModel*>(m_mapMasters[pEvent->dwID].pMaster);
			KG_PROCESS_ERROR(pModel);
			KGLOG_PROCESS_ERROR(pEvent->AParameters.pData);
			D3DXQUATERNION* pvecValue = (D3DXQUATERNION*)(&(pEvent->AParameters.pData[pEvent->AParameters.nFrom[0]]));
			KG_PROCESS_ERROR(pvecValue);
			pModel->SetScalingRotation(pvecValue);
		}
		break;
	case FT_KG3DMODEL_RESETTRANSFORM:
		{
			KG_PROCESS_ERROR(m_mapMasters.find(pEvent->dwID) != m_mapMasters.end());
			IKG3DModel* pModel = static_cast<IKG3DModel*>(m_mapMasters[pEvent->dwID].pMaster);
			KG_PROCESS_ERROR(pModel);
			pModel->ResetTransform();
		}
		break;
	case FT_KG3DMODEL_BINDTOSOCKET:
		{
			KG_PROCESS_ERROR(m_mapMasters.find(pEvent->dwID) != m_mapMasters.end());
			IKG3DModel* pModel = static_cast<IKG3DModel*>(m_mapMasters[pEvent->dwID].pMaster);
			KG_PROCESS_ERROR(pModel);
			KGLOG_PROCESS_ERROR(pEvent->AParameters.pData);
			DWORD* pdwModelID = (DWORD*)(&(pEvent->AParameters.pData[pEvent->AParameters.nFrom[0]]));
			KG_PROCESS_ERROR(pdwModelID);
			KG_PROCESS_ERROR(m_mapMasters.find(*pdwModelID) != m_mapMasters.end());
			IKG3DModel* pBindModel = static_cast<IKG3DModel*>(m_mapMasters[*pdwModelID].pMaster);
			KG_PROCESS_ERROR(pBindModel);
			char* cszScoketName = (char*)(&(pEvent->AParameters.pData[pEvent->AParameters.nFrom[1]]));
			KG_PROCESS_ERROR(cszScoketName);	
			pModel->BindToSocket(pBindModel,cszScoketName);
		}
		break;
	case FT_KG3DMODEL_BINDTOBONE:
		{
			KG_PROCESS_ERROR(m_mapMasters.find(pEvent->dwID) != m_mapMasters.end());
			IKG3DModel* pModel = static_cast<IKG3DModel*>(m_mapMasters[pEvent->dwID].pMaster);
			KG_PROCESS_ERROR(pModel);
			KGLOG_PROCESS_ERROR(pEvent->AParameters.pData);
			DWORD* pdwModelID = (DWORD*)(&(pEvent->AParameters.pData[pEvent->AParameters.nFrom[0]]));
			KG_PROCESS_ERROR(pdwModelID);
			KG_PROCESS_ERROR(m_mapMasters.find(*pdwModelID) != m_mapMasters.end());
			IKG3DModel* pBindModel = static_cast<IKG3DModel*>(m_mapMasters[*pdwModelID].pMaster);
			KG_PROCESS_ERROR(pBindModel);
			char* cszBoneName = (char*)(&(pEvent->AParameters.pData[pEvent->AParameters.nFrom[1]]));
			KG_PROCESS_ERROR(cszBoneName);						
			pModel->BindToBone(pBindModel,cszBoneName);
		}
		break;
	case FT_KG3DMODEL_ATTACHMODEL:
		{
			KG_PROCESS_ERROR(m_mapMasters.find(pEvent->dwID) != m_mapMasters.end());
			IKG3DModel* pModel = static_cast<IKG3DModel*>(m_mapMasters[pEvent->dwID].pMaster);
			KG_PROCESS_ERROR(pModel);
			KGLOG_PROCESS_ERROR(pEvent->AParameters.pData);
			DWORD* pdwModelID = (DWORD*)(&(pEvent->AParameters.pData[pEvent->AParameters.nFrom[0]]));
			KG_PROCESS_ERROR(pdwModelID);
			KG_PROCESS_ERROR(m_mapMasters.find(*pdwModelID) != m_mapMasters.end());
			IKG3DModel* pAttachModel = static_cast<IKG3DModel*>(m_mapMasters[*pdwModelID].pMaster);
			KG_PROCESS_ERROR(pAttachModel);				
			pModel->AttachModel(pAttachModel);
			//m_mapMasters[*pdwModelID].mtType = TYPESUBMODEL;
		}
		break;
	case FT_KG3DMODEL_DETCHMODEL:
		{
			KG_PROCESS_ERROR(m_mapMasters.find(pEvent->dwID) != m_mapMasters.end());
			IKG3DModel* pModel = static_cast<IKG3DModel*>(m_mapMasters[pEvent->dwID].pMaster);
			KG_PROCESS_ERROR(pModel);
			KGLOG_PROCESS_ERROR(pEvent->AParameters.pData);
			DWORD* pdwModelID = (DWORD*)(&(pEvent->AParameters.pData[pEvent->AParameters.nFrom[0]]));
			KG_PROCESS_ERROR(pdwModelID);
			KG_PROCESS_ERROR(m_mapMasters.find(*pdwModelID) != m_mapMasters.end());
			IKG3DModel* pAttachModel = static_cast<IKG3DModel*>(m_mapMasters[*pdwModelID].pMaster);
			KG_PROCESS_ERROR(pAttachModel);				
			pModel->DetachModel(pAttachModel);
			//m_mapMasters[*pdwModelID].mtType = TYPEMODEL;
		}
		break;
	case FT_KG3DMODEL_PLAYANIMATION:
		{
			KG_PROCESS_ERROR(m_mapMasters.find(pEvent->dwID) != m_mapMasters.end());
			IKG3DModel* pModel = static_cast<IKG3DModel*>(m_mapMasters[pEvent->dwID].pMaster);
			KG_PROCESS_ERROR(pModel);
			KGLOG_PROCESS_ERROR(pEvent->AParameters.pData);
			DWORD* pdwPlayType = (DWORD*)(&(pEvent->AParameters.pData[pEvent->AParameters.nFrom[0]]));
			KG_PROCESS_ERROR(pdwPlayType);
			char* cszFileName = (char*)(&(pEvent->AParameters.pData[pEvent->AParameters.nFrom[1]]));
			// KG_PROCESS_ERROR(cszFileName);
			FLOAT* pfSpeed = (FLOAT*)(&(pEvent->AParameters.pData[pEvent->AParameters.nFrom[2]])); 
			KG_PROCESS_ERROR(pfSpeed);
			int* pnOffsetTime = (int*)(&(pEvent->AParameters.pData[pEvent->AParameters.nFrom[3]]));
			KG_PROCESS_ERROR(pnOffsetTime);
			pModel->PlayAnimation(*pdwPlayType,cszFileName,m_fPlaySpeed/* *pfSpeed*/ ,*pnOffsetTime,NULL,NULL, ANICTL_PRIMARY);
		}
		break;
	case FT_KG3DMODEL_PALYSPLITANIMATION:
		{
			KG_PROCESS_ERROR(m_mapMasters.find(pEvent->dwID) != m_mapMasters.end());
			IKG3DModel* pModel = static_cast<IKG3DModel*>(m_mapMasters[pEvent->dwID].pMaster);
			KG_PROCESS_ERROR(pModel);
			KGLOG_PROCESS_ERROR(pEvent->AParameters.pData);
			char* strAnimationName = (char*)(&(pEvent->AParameters.pData[pEvent->AParameters.nFrom[0]]));
			KG_PROCESS_ERROR(strAnimationName);
			DWORD* pdwSplitType = (DWORD*)(&(pEvent->AParameters.pData[pEvent->AParameters.nFrom[1]]));
			KG_PROCESS_ERROR(pdwSplitType);
			DWORD* pdwExtraInfo = (DWORD*)(&(pEvent->AParameters.pData[pEvent->AParameters.nFrom[2]]));
			KG_PROCESS_ERROR(pdwExtraInfo);
			DWORD* pdwPlayType = (DWORD*)(&(pEvent->AParameters.pData[pEvent->AParameters.nFrom[3]]));
			KG_PROCESS_ERROR(pdwPlayType);
			FLOAT* pfSpeed = (FLOAT*)(&(pEvent->AParameters.pData[pEvent->AParameters.nFrom[4]]));
			KG_PROCESS_ERROR(pfSpeed);
			int* pnOffsetTime = (int*)(&(pEvent->AParameters.pData[pEvent->AParameters.nFrom[5]]));
			KG_PROCESS_ERROR(pnOffsetTime);
			pModel->PlaySplitAnimation(strAnimationName,*pdwSplitType,*pdwExtraInfo,*pdwPlayType,m_fPlaySpeed/* *pfSpeed*/,*pnOffsetTime,NULL,NULL, ANICTL_PRIMARY);
		}
		break;
	case FT_KG3DMODEL_LOADMATERIALFROMFILE:
		{
			KG_PROCESS_ERROR(m_mapMasters.find(pEvent->dwID) != m_mapMasters.end());
			IKG3DModel* pModel = static_cast<IKG3DModel*>(m_mapMasters[pEvent->dwID].pMaster);
			KG_PROCESS_ERROR(pModel);
			KGLOG_PROCESS_ERROR(pEvent->AParameters.pData);
			char* cszFileName = (char*)(&(pEvent->AParameters.pData[pEvent->AParameters.nFrom[0]]));
			KG_PROCESS_ERROR(cszFileName);
			DWORD* pOption = (DWORD*)(&(pEvent->AParameters.pData[pEvent->AParameters.nFrom[1]])); 
			KG_PROCESS_ERROR(pOption);
			pModel->LoadMaterialFromFile(cszFileName,*pOption);
		}
		break;
	case FT_KG3DMODEL_SETALPHA:
		{
			//float fAlpha, BOOL bUseSpecail
			KG_PROCESS_ERROR(m_mapMasters.find(pEvent->dwID) != m_mapMasters.end());
			IKG3DModel* pModel = static_cast<IKG3DModel*>(m_mapMasters[pEvent->dwID].pMaster);
			KG_PROCESS_ERROR(pModel);
			KGLOG_PROCESS_ERROR(pEvent->AParameters.pData);
			float* pfAlpha = (float*)(&(pEvent->AParameters.pData[pEvent->AParameters.nFrom[0]]));
			KG_PROCESS_ERROR(pfAlpha);
			BOOL* pbUseSpecail = (BOOL*)(&(pEvent->AParameters.pData[pEvent->AParameters.nFrom[1]])); 
			KG_PROCESS_ERROR(pbUseSpecail);
			pModel->SetAlpha(*pfAlpha,*pbUseSpecail);
			//pModel->SetAlpha(1,FALSE);
		}
		break;
	case FT_KG3DMODEL_SETCALLBACKRENDER:
		{
			KG_PROCESS_ERROR(m_mapMasters.find(pEvent->dwID) != m_mapMasters.end());
			IKG3DModel* pModel = static_cast<IKG3DModel*>(m_mapMasters[pEvent->dwID].pMaster);
			KG_PROCESS_ERROR(pModel);
			pModel->SetAlpha(0,FALSE);
		}
		break;
	case FT_KG3DSCENEEDITOR_DESTRUCT:
		{
			KG_PROCESS_ERROR(m_mapMasters.find(pEvent->dwID) != m_mapMasters.end());
			IKG3DScene* pSceneSceneEditor = static_cast<IKG3DScene*>(m_mapMasters[pEvent->dwID].pMaster);
			if (m_pCurrentScene == pSceneSceneEditor)
				m_pCurrentScene = NULL;
			g_cEngineManager.RemoveScene(pSceneSceneEditor);
			pSceneSceneEditor = NULL;
			m_mapMasters.erase(m_mapMasters.find(pEvent->dwID));	
		}
		break;
	case FT_KG3DSCENEEDITOR_ADDRENDERENTITY:
		{
			KG_PROCESS_ERROR(m_mapMasters.find(pEvent->dwID) != m_mapMasters.end());
			IKG3DScene* pSceneSceneEditor = static_cast<IKG3DScene*>(m_mapMasters[pEvent->dwID].pMaster);
			KG_PROCESS_ERROR(pSceneSceneEditor);
			KGLOG_PROCESS_ERROR(pEvent->AParameters.pData);
			DWORD* pdwEntityID = (DWORD*)(&(pEvent->AParameters.pData[pEvent->AParameters.nFrom[0]]));
			KG_PROCESS_ERROR(pdwEntityID);
			KG_PROCESS_ERROR(m_mapMasters.find(*pdwEntityID) != m_mapMasters.end());
			IKG3DModel* pModel = static_cast<IKG3DModel*>(m_mapMasters[*pdwEntityID].pMaster);
			KG_PROCESS_ERROR(pModel);
			unsigned* puClass = (unsigned*)(&(pEvent->AParameters.pData[pEvent->AParameters.nFrom[1]]));
			KG_PROCESS_ERROR(puClass);
			BOOL* pbSelectable = (BOOL*)(&(pEvent->AParameters.pData[pEvent->AParameters.nFrom[2]]));
			KG_PROCESS_ERROR(pbSelectable);
			pSceneSceneEditor->AddRenderEntity(pModel,*puClass,*pbSelectable,TRUE);
		}
		break;
	case FT_KG3DSCENEEDITOR_REMOVERENDERENTITY:
		{
			KG_PROCESS_ERROR(m_mapMasters.find(pEvent->dwID) != m_mapMasters.end());
			IKG3DScene* pSceneSceneEditor = static_cast<IKG3DScene*>(m_mapMasters[pEvent->dwID].pMaster);
			KG_PROCESS_ERROR(pSceneSceneEditor);
			KGLOG_PROCESS_ERROR(pEvent->AParameters.pData);
			DWORD* pdwEntityID = (DWORD*)(&(pEvent->AParameters.pData[pEvent->AParameters.nFrom[0]]));
			KG_PROCESS_ERROR(pdwEntityID);
			KG_PROCESS_ERROR(m_mapMasters.find(*pdwEntityID) != m_mapMasters.end());
			IKG3DModel* pModel = static_cast<IKG3DModel*>(m_mapMasters[*pdwEntityID].pMaster);
			KG_PROCESS_ERROR(pModel);
			unsigned* puClass = (unsigned*)(&(pEvent->AParameters.pData[pEvent->AParameters.nFrom[1]]));
			KG_PROCESS_ERROR(puClass);
			BOOL* pbSelectable = (BOOL*)(&(pEvent->AParameters.pData[pEvent->AParameters.nFrom[2]]));
			KG_PROCESS_ERROR(pbSelectable);
			pSceneSceneEditor->RemoveRenderEntity(pModel,*puClass,*pbSelectable);
		}
		break;
	case FT_KG3DSCENEEDITOR_LOADFROMFILE:
		{
			KG_PROCESS_ERROR(m_mapMasters.find(pEvent->dwID) == m_mapMasters.end());
			KGLOG_PROCESS_ERROR(pEvent->AParameters.pData);
			char* cszFileName = (char*)(&(pEvent->AParameters.pData[pEvent->AParameters.nFrom[1]]));
			KG_PROCESS_ERROR(cszFileName);
			unsigned* puFileNameHash = (unsigned*)(&(pEvent->AParameters.pData[pEvent->AParameters.nFrom[2]])); 
			KG_PROCESS_ERROR(puFileNameHash);
			unsigned* puOption = (unsigned*)(&(pEvent->AParameters.pData[pEvent->AParameters.nFrom[3]]));
			KG_PROCESS_ERROR(puOption);
			IKG3DScene* piScene = NULL;
			g_cEngineManager.NewOneScene(SCENETYPE_SCENEEDITOR, &piScene);
			KG_PROCESS_ERROR(piScene);
			piScene->LoadFromFile(cszFileName,*puFileNameHash,*puOption);
            //((KG3DSceneSceneEditor *)piScene)->ForceLoadAllEntity();
			m_pCurrentScene = piScene;
			Master MyMaster;
			MyMaster.mtType = TYPESCENEEDITOR;
			MyMaster.pMaster = (PVOID)(piScene);
			m_mapMasters[pEvent->dwID] = MyMaster;
		}
		break;
	case FT_KG3DSCENEEDITOR_ADDOUTPUTWINDOW:
		{
			KG_PROCESS_ERROR(m_mapMasters.find(pEvent->dwID) != m_mapMasters.end());
			IKG3DScene* pSceneSceneEditor = static_cast<IKG3DScene*>(m_mapMasters[pEvent->dwID].pMaster);
			KG_PROCESS_ERROR(pSceneSceneEditor);
			KGLOG_PROCESS_ERROR(pEvent->AParameters.pData);
			char* cszName = (char*)(&(pEvent->AParameters.pData[pEvent->AParameters.nFrom[0]]));
			KG_PROCESS_ERROR(cszName);
			DWORD* pdwCameraID = (DWORD*)(&(pEvent->AParameters.pData[pEvent->AParameters.nFrom[1]])); 
			KG_PROCESS_ERROR(pdwCameraID);						
			int nWindowID = 0;
			if (m_hWnd != NULL)
			{
				
				//if (m_pOldScene)
				//{
				//	m_pOldScene->RemoveOutputWindow(m_nOldWindowID);
				//	m_mapMasters[m_dwOldCameraID].pMaster = NULL;
				//}

				pSceneSceneEditor->AddOutputWindow(cszName,m_hWnd,0,0,&nWindowID);
				//m_pOldScene = pSceneSceneEditor;
				//m_nOldWindowID = nWindowID;
				m_dwOldCameraID = *pdwCameraID;
				//m_pCurrentScene = (IKG3DScene*)pSceneSceneEditor;
				IKG3DCamera* pCamera = pSceneSceneEditor->GetCurrentCamera();
				//RECT rect;
				//::GetClientRect(m_hWnd,&rect);
				pSceneSceneEditor->OnResizeWindow(NULL);
				Master MyMaster;
				MyMaster.mtType = TYPECAMERA;
				MyMaster.pMaster = (PVOID)(pCamera);
				m_mapMasters[*pdwCameraID] = MyMaster;			
				m_dwCameraID = *pdwCameraID;
			}
		}
		break;
	case FT_KG3DCAMERA_SETTRACKINFO:
		{
			KG_PROCESS_ERROR(m_mapMasters.find(pEvent->dwID) != m_mapMasters.end());
			IKG3DCamera* pCamera = static_cast<IKG3DCamera*>(m_mapMasters[pEvent->dwID].pMaster);
			KG_PROCESS_ERROR(pCamera);
			KGLOG_PROCESS_ERROR(pEvent->AParameters.pData);
			TrackCameraInfo* pTrackCameraInfo = (TrackCameraInfo*)(&(pEvent->AParameters.pData[pEvent->AParameters.nFrom[0]]));
			KG_PROCESS_ERROR(pTrackCameraInfo);
			pCamera->SetTrackInfo(*pTrackCameraInfo);
		}
		break;
	case FT_KG3DCAMERA_UPDATECAMERA:
		{
			if (m_CameraType == CAMERA_TYPE_RECORD || (m_CameraType == CAMERA_TYPE_BIND && !m_bIsBindIDCreated))		
			{
				KG_PROCESS_ERROR(m_mapMasters.find(pEvent->dwID) != m_mapMasters.end());
				IKG3DCamera* pCamera = static_cast<IKG3DCamera*>(m_mapMasters[pEvent->dwID].pMaster);
				KG_PROCESS_ERROR(pCamera);
				KGLOG_PROCESS_ERROR(pEvent->AParameters.pData);
				D3DXVECTOR3* pvecPosition = (D3DXVECTOR3*)(&(pEvent->AParameters.pData[pEvent->AParameters.nFrom[0]]));
				KG_PROCESS_ERROR(pvecPosition);
				D3DXVECTOR3* pvecAxesPos = (D3DXVECTOR3*)(&(pEvent->AParameters.pData[pEvent->AParameters.nFrom[1]]));
				KG_PROCESS_ERROR(pvecAxesPos);
				float* pfLookAtOffset = (float*)(&(pEvent->AParameters.pData[pEvent->AParameters.nFrom[2]]));
				KG_PROCESS_ERROR(pfLookAtOffset);
				float* pfTargetMoveSpeed = (float*)(&(pEvent->AParameters.pData[pEvent->AParameters.nFrom[3]]));
				KG_PROCESS_ERROR(pfTargetMoveSpeed);
				pCamera->UpdateCamera(*pvecPosition,*pvecAxesPos,*pfLookAtOffset,*pfTargetMoveSpeed);
			}
			else if (m_CameraType == CAMERA_TYPE_BINDPLAYER)
			{
				D3DXVECTOR3* pvecAxesPos = (D3DXVECTOR3*)(&(pEvent->AParameters.pData[pEvent->AParameters.nFrom[1]]));
				KG_PROCESS_ERROR(pvecAxesPos);
				
				D3DXMATRIX matStepRotion;
				D3DXVECTOR3 vecCameraDir,vecLookAt;
				D3DXVECTOR3 vecPos = *pvecAxesPos;
				vecPos.y += 200;
				KG_PROCESS_ERROR(m_mapMasters.find(m_dwCameraID) != m_mapMasters.end());
				IKG3DCamera* pCamera = static_cast<IKG3DCamera*>(m_mapMasters[m_dwCameraID].pMaster);
				KG_PROCESS_ERROR(pCamera);
				pCamera->SetPosition(vecPos);

				D3DXMatrixRotationAxis(
					&matStepRotion,
					&D3DXVECTOR3(0,1,0),
					m_fAngle
					);
				vecCameraDir = D3DXVECTOR3(0,0,100);
				D3DXVec3TransformNormal(
					&vecCameraDir,
					&vecCameraDir,
					&matStepRotion
					);
				vecLookAt = vecPos + vecCameraDir;
				pCamera->SetLookAtPosition(vecLookAt);

				//for smooth camera
				m_dwCameraPastFromTime = pEvent->dwTime;
				m_dwCameraNowFromTime =  g_cGraphicsTool.GetNowTime();
				m_vecFromPos = vecPos;
				int nStep = 0;
				EventRecord* pToEvent = pEvent->pNextEvent;
				while (nStep < 10 && pToEvent != NULL && pToEvent->EventFunction != FT_KG3DCAMERA_UPDATECAMERA)
				{
					pToEvent = pToEvent->pNextEvent;
					nStep++;
				}
				if (nStep < 10 && pToEvent != NULL)
				{					
					D3DXVECTOR3* pvecAxesPos = (D3DXVECTOR3*)(&(pToEvent->AParameters.pData[pEvent->AParameters.nFrom[1]]));
					if (pvecAxesPos)
					{
						m_dwCameraPastToTime = pToEvent->dwTime;
						m_vecToPos = *pvecAxesPos;
						m_vecToPos.y += 200;
					}
					else
					{
						m_dwCameraPastToTime = 0;
						m_vecToPos = D3DXVECTOR3(0,0,0);
					}
				}
				else
				{
					m_dwCameraPastToTime = 0;
					m_vecToPos = D3DXVECTOR3(0,0,0);
				}
			}
		}
		break;
	case FT_KG3DCAMERA_SETPOSITION:
		{
			if (m_CameraType == CAMERA_TYPE_RECORD || (m_CameraType == CAMERA_TYPE_BIND && !m_bIsBindIDCreated))		
			{
				KG_PROCESS_ERROR(m_mapMasters.find(pEvent->dwID) != m_mapMasters.end());
				IKG3DCamera* pCamera = static_cast<IKG3DCamera*>(m_mapMasters[pEvent->dwID].pMaster);
				KG_PROCESS_ERROR(pCamera);
				KGLOG_PROCESS_ERROR(pEvent->AParameters.pData);
				D3DXVECTOR3* pvecPos = (D3DXVECTOR3*)(&(pEvent->AParameters.pData[pEvent->AParameters.nFrom[0]]));
				KG_PROCESS_ERROR(pvecPos);
				//if (pvecPos->y > 0)
				pCamera->SetPosition(*pvecPos);	
			}
		}
		break;
	case FT_KG3DCAMERA_SETLOOKATPOSITION:
		{
			if (m_CameraType == CAMERA_TYPE_RECORD || (m_CameraType == CAMERA_TYPE_BIND && !m_bIsBindIDCreated))		
			{
				KG_PROCESS_ERROR(m_mapMasters.find(pEvent->dwID) != m_mapMasters.end());
				IKG3DCamera* pCamera = static_cast<IKG3DCamera*>(m_mapMasters[pEvent->dwID].pMaster);
				KG_PROCESS_ERROR(pCamera);
				KGLOG_PROCESS_ERROR(pEvent->AParameters.pData);
				D3DXVECTOR3* pvecPos = (D3DXVECTOR3*)(&(pEvent->AParameters.pData[pEvent->AParameters.nFrom[0]]));
				KG_PROCESS_ERROR(pvecPos);
				//if (pvecPos->y > 0)
				pCamera->SetLookAtPosition(*pvecPos);
			}
		}
		break;
	case FT_KG3DCAMERA_SETUPDIRECTION:
		{
			if (m_CameraType == CAMERA_TYPE_RECORD || (m_CameraType == CAMERA_TYPE_BIND && !m_bIsBindIDCreated))		
			{
				KG_PROCESS_ERROR(m_mapMasters.find(pEvent->dwID) != m_mapMasters.end());
				IKG3DCamera* pCamera = static_cast<IKG3DCamera*>(m_mapMasters[pEvent->dwID].pMaster);
				KG_PROCESS_ERROR(pCamera);
				KGLOG_PROCESS_ERROR(pEvent->AParameters.pData);
				D3DXVECTOR3* pvecUp = (D3DXVECTOR3*)(&(pEvent->AParameters.pData[pEvent->AParameters.nFrom[0]]));
				KG_PROCESS_ERROR(pvecUp);
				//if (pvecUp->y > 0)
				pCamera->SetUpDirection(*pvecUp);
			}
		}
		break;
	//case FT_KG3DCAMERA_SETGLOBALPERSPECTIVE:
	//	{
	//		//float fFovy, float fAspect, float fzNear, float fzFar;
	//		KG_PROCESS_ERROR(m_mapMasters.find(pEvent->dwID) != m_mapMasters.end());
	//		IKG3DCamera* pCamera = static_cast<IKG3DCamera*>(m_mapMasters[pEvent->dwID].pMaster);
	//		KG_PROCESS_ERROR(pCamera);
	//		KGLOG_PROCESS_ERROR(pEvent->AParameters.pData);
	//		float* pfFovy = (float*)(&(pEvent->AParameters.pData[pEvent->AParameters.nFrom[0]]));
	//		KG_PROCESS_ERROR(pfFovy);
	//		float* pfAspect = (float*)(&(pEvent->AParameters.pData[pEvent->AParameters.nFrom[1]]));
	//		KG_PROCESS_ERROR(pfAspect);
	//		float* pfzNear = (float*)(&(pEvent->AParameters.pData[pEvent->AParameters.nFrom[2]]));
	//		KG_PROCESS_ERROR(pfzNear);
	//		float* pfzFar = (float*)(&(pEvent->AParameters.pData[pEvent->AParameters.nFrom[3]]));
	//		KG_PROCESS_ERROR(pfzFar);
	//		pCamera->SetGlobalPerspective(*pfFovy,*pfAspect,*pfzNear,*pfzFar);
	//	}
	//	break;
	//case FT_KG3DCAMERA_SETGLOBALORTHOGONAL:
	//	{
	//		//float fWidth, float fHeight, float fzNear, float fzFar;
	//		KG_PROCESS_ERROR(m_mapMasters.find(pEvent->dwID) != m_mapMasters.end());
	//		IKG3DCamera* pCamera = static_cast<IKG3DCamera*>(m_mapMasters[pEvent->dwID].pMaster);
	//		KG_PROCESS_ERROR(pCamera);
	//		KGLOG_PROCESS_ERROR(pEvent->AParameters.pData);
	//		float* pfWidth = (float*)(&(pEvent->AParameters.pData[pEvent->AParameters.nFrom[0]]));
	//		KG_PROCESS_ERROR(pfWidth);
	//		float* pfHeight = (float*)(&(pEvent->AParameters.pData[pEvent->AParameters.nFrom[1]]));
	//		KG_PROCESS_ERROR(pfHeight);
	//		float* pfzNear = (float*)(&(pEvent->AParameters.pData[pEvent->AParameters.nFrom[2]]));
	//		KG_PROCESS_ERROR(pfzNear);
	//		float* pfzFar = (float*)(&(pEvent->AParameters.pData[pEvent->AParameters.nFrom[3]]));
	//		KG_PROCESS_ERROR(pfzFar);
	//		pCamera->SetGlobalOrthogonal(*pfWidth,*pfHeight,*pfzNear,*pfzFar);
	//	}
	//	break;

	//case FT_KG3DSOUND_PLAYBGMUSIC:
	//	{
	//		//virtual HRESULT PlayBgMusic(int nType, LPCSTR FileName, INT nPan, float fVolume) = 0;
	//		KGLOG_PROCESS_ERROR(pEvent->AParameters.pData);
	//		int* pnType = (int*)(&(pEvent->AParameters.pData[pEvent->AParameters.nFrom[0]]));
	//		KG_PROCESS_ERROR(pnType);
	//		char* pFileName = (char*)(&(pEvent->AParameters.pData[pEvent->AParameters.nFrom[1]]));
	//		KG_PROCESS_ERROR(pFileName);
	//		INT* pnPan = (INT*)(&(pEvent->AParameters.pData[pEvent->AParameters.nFrom[2]]));
	//		KG_PROCESS_ERROR(pnPan);
	//		float* pfVolume = (float*)(&(pEvent->AParameters.pData[pEvent->AParameters.nFrom[3]])); 
	//		KG_PROCESS_ERROR(pfVolume);
	//		IKG3DSoundShell *pSoundShell = g_cEngineManager.GetSound();
	//		if (!pSoundShell)
	//			return E_FAIL;
	//		pSoundShell->PlayBgMusic(*pnType,pFileName,*pnPan,*pfVolume);
	//	}
	//	break;
	//case FT_KG3DSOUND_STOPBGMUSIC:
	//	{
	//		IKG3DSoundShell *pSoundShell = g_cEngineManager.GetSound();
	//		if (!pSoundShell)
	//			return E_FAIL;
	//		pSoundShell->StopBgMusic();
	//	}
	//	break;
	//case FT_KG3DSOUND_SETBGMUSICVOLUME:
	//	{
	//		//virtual HRESULT SetBgMusicVolume(float fVolume) = 0;
	//		KGLOG_PROCESS_ERROR(pEvent->AParameters.pData);
	//		float* pfVolume = (float*)(&(pEvent->AParameters.pData[pEvent->AParameters.nFrom[0]])); 
	//		KG_PROCESS_ERROR(pfVolume);
	//		IKG3DSoundShell *pSoundShell = g_cEngineManager.GetSound();
	//		if (!pSoundShell)
	//			return E_FAIL;
	//		pSoundShell->SetBgMusicVolume(*pfVolume);
	//	}
	//	break;
	//case FT_KG3DSOUND_SETBGMUSICLOOP:
	//	{
	//		//virtual HRESULT	SetBgMusicLoop(BOOL bLoop) = 0;
	//		KGLOG_PROCESS_ERROR(pEvent->AParameters.pData);
	//		BOOL* pbLoop = (BOOL*)(&(pEvent->AParameters.pData[pEvent->AParameters.nFrom[0]])); 
	//		KG_PROCESS_ERROR(pbLoop);
	//		IKG3DSoundShell *pSoundShell = g_cEngineManager.GetSound();
	//		if (!pSoundShell)
	//			return E_FAIL;
	//		pSoundShell->SetBgMusicLoop(*pbLoop);
	//	}
	//	break;
	default:
		break;
	}
	hr = S_OK;
Exit0:
	return hr;
}

BOOL RecPlayer::IsPlayFinished()
{
	return (m_dwCurrentEvents == m_dwEventsCount);
}

HRESULT RecPlayer::FrameMoveRePlay()
{
    if (m_PlayState == PLAY_STATE_PLAY)
	{
		DWORD dwNowTime = g_cGraphicsTool.GetNowTime();
		DWORD dwStepTime = dwNowTime - m_dwRecPlayStartTime;
		if (m_PlayType == PLAYTYPE_BYTIME)
		{
            while (
                (m_dwCurrentEvents < m_dwEventsCount) &&
                (m_pCurrentEvent->dwTime - m_dwRecordStartTime <= dwStepTime * m_fPlaySpeed)
            )
			{
				ProcessOneEvent(m_pCurrentEvent);
                m_dwCurrentEvents++;
                ++m_CurrentIter;
                if (m_CurrentIter == m_EventHead.ListEvent.end())
                {
                    ++m_CurrentOutIt;
                    if (m_CurrentOutIt == m_ListEventHead.end())
                        break;
                    m_EventHead   = *m_CurrentOutIt;
                    m_CurrentIter = m_EventHead.ListEvent.begin();
                }
                m_pCurrentEvent = *m_CurrentIter;
			}
		}
		else if (m_PlayType == PLAYTYPE_BYFRAME)
		{
			m_dwNowFrameCount = m_pCurrentEvent->dwFrame;
			while (m_dwCurrentEvents < m_dwEventsCount)
			{
				ProcessOneEvent(m_pCurrentEvent);
                m_dwCurrentEvents++;
                ++m_CurrentIter;
                if (m_CurrentIter == m_EventHead.ListEvent.end())
                    break;
                m_pCurrentEvent = *m_CurrentIter;
			}

            if ((m_dwCurrentEvents < m_dwEventsCount) &&
                (m_CurrentIter == m_EventHead.ListEvent.end())
            )
            {
                ++m_CurrentOutIt;
                m_EventHead   = *m_CurrentOutIt;
                m_CurrentIter = m_EventHead.ListEvent.begin();
                m_pCurrentEvent = *m_CurrentIter;
            }
		}

		//For Animation
		if (m_PlayType == PLAYTYPE_BYFRAME && m_dwCurrentEvents < m_dwEventsCount)
		{
			if (m_dwRecordLastFrameTime == 0 || m_dwNowLastFrameTime == 0)
			{
				m_fSmoothSpeed = 1;
			}
			else
			{
				DWORD dwRecordTimePastByFrame = m_pCurrentEvent->dwTime - m_dwRecordLastFrameTime;
				DWORD dwNowTimePastByFrame = g_cGraphicsTool.GetNowTime() - m_dwNowLastFrameTime;
				if (dwNowTimePastByFrame == 0)
				{
					m_fPlaySpeed = 1;
				}
				else
				{
					m_fPlaySpeed = (float)(dwRecordTimePastByFrame) / (float)(dwNowTimePastByFrame);
                    if (m_fPlaySpeed < 0.0001)
                        m_fPlaySpeed = 0.00011f;
				}
			}
			m_dwRecordLastFrameTime = m_pCurrentEvent->dwTime;
			m_dwNowLastFrameTime = g_cGraphicsTool.GetNowTime();
		}
	
		if (m_dwCurrentEvents == m_dwEventsCount)
		{
			m_PlayState = PLAY_STATE_NONE;
            m_pCurrentEvent = NULL;
		}
		//for camera
		if (m_CameraType == CAMERA_TYPE_BINDPLAYER && m_dwCameraPastToTime != 0)
		{
			DWORD dwNowCameraTime = g_cGraphicsTool.GetNowTime();	
			DWORD dwDisTime = m_dwCameraPastToTime - m_dwCameraPastFromTime;
			if (dwNowCameraTime - m_dwCameraNowFromTime < dwDisTime)
			{
				D3DXVECTOR3 vecDis = m_vecToPos - m_vecFromPos;
				vecDis = vecDis * ((float)(dwNowCameraTime - m_dwCameraNowFromTime) / (float)dwDisTime);
				vecDis = vecDis + m_vecFromPos;

				if (m_mapMasters.find(m_dwCameraID) != m_mapMasters.end())
				{
					IKG3DCamera* pCamera = static_cast<IKG3DCamera*>(m_mapMasters[m_dwCameraID].pMaster);
					D3DXMATRIX matStepRotion;
					D3DXVECTOR3 vecCameraDir,vecLookAt;
					if (pCamera)
					{
						pCamera->SetPosition(vecDis);

						D3DXMatrixRotationAxis(
							&matStepRotion,
							&D3DXVECTOR3(0,1,0),
							m_fAngle
							);
						vecCameraDir = D3DXVECTOR3(0,0,100);
						D3DXVec3TransformNormal(
							&vecCameraDir,
							&vecCameraDir,
							&matStepRotion
							);
						vecLookAt = vecDis + vecCameraDir;
						pCamera->SetLookAtPosition(vecLookAt);
					}
				}

			}

		}
	}

    if (m_pCurrentScene != NULL && m_PlayState != PLAY_STATE_NONE)
	{
		m_pCurrentScene->FrameMove();
		m_pCurrentScene->Render(0);
	}

	return S_OK;
}

HRESULT RecPlayer::UpDateFreeCamera(D3DXVECTOR3 vecDis,D3DXVECTOR2 pointMouseMove)
{
	HRESULT hr = E_FAIL;
	D3DXVECTOR3 vec3CameraPos,vec3CameraLookAtPos,vec3ViewDir,vec3UpDir,vec3Value;
	D3DXMATRIX matView,matInvView;

	D3DXQUATERNION quaGoGoGo;
	D3DXMATRIX matRotation;
	D3DXVECTOR3 vecNewLookAt;

	KG_PROCESS_ERROR(m_bIsFreeCamera);
	KG_PROCESS_ERROR(m_pCurrentScene);
	IKG3DCamera* pCamera = m_pCurrentScene->GetCurrentCamera();	

	KG_PROCESS_ERROR(pCamera);
	pCamera->GetPosition(&vec3CameraPos);
	pCamera->GetLookAtPosition(&vec3CameraLookAtPos);
	pCamera->GetUpDirection(&vec3UpDir);
	vec3ViewDir = vec3CameraLookAtPos - vec3CameraPos;
	D3DXVec3Normalize(&vec3ViewDir,&vec3ViewDir);
	pCamera->SetPosition(vec3CameraPos + vecDis.x * vec3ViewDir);
	
	pCamera->GetPosition(&vec3CameraPos);
	D3DXVec3Normalize(&vec3UpDir,&vec3UpDir);
	D3DXVec3Cross(&vec3Value,&vec3ViewDir,&vec3UpDir);
	D3DXVec3Normalize(&vec3Value,&vec3Value);
	pCamera->SetPosition(vec3CameraPos + vecDis.z * vec3Value);
	pCamera->SetLookAtPosition(vec3CameraLookAtPos + vecDis.z * vec3Value);

	pCamera->GetLookAtPosition(&vec3CameraLookAtPos);
	D3DXQuaternionRotationYawPitchRoll(&quaGoGoGo,-pointMouseMove.x * 2,0,-pointMouseMove.y * 2);
	D3DXMatrixTransformation(&matRotation,NULL,NULL,NULL,&vec3CameraPos,&quaGoGoGo,NULL);
	D3DXVec3TransformCoord(&vecNewLookAt,&vec3CameraLookAtPos,&matRotation);
	pCamera->SetLookAtPosition(vecNewLookAt);
	
	hr = S_OK;
Exit0:	
	return hr;
}


HRESULT RecPlayer::FreeCamera()
{
	HRESULT hr = E_FAIL;
	if (m_bIsFreeCamera)
	{
		KG_PROCESS_ERROR(m_pCurrentScene);
		KG_PROCESS_ERROR(m_pGhostCameraForFree);
		IKG3DCamera* pCamera = m_pCurrentScene->GetCurrentCamera();	
		KG_PROCESS_ERROR(m_pGhostCameraForFree);
		KG_COM_PROCESS_ERROR(m_pGhostCameraForFree->Copy(pCamera));
		//TrackCameraInfo trackInfo;
		//pCamera->GetTrackInfo(&trackInfo);
		//trackInfo.bEnableTrack = TRUE;
		//trackInfo.nUseObstructdAvert = TRUE;
		//trackInfo.nUseFlexibilitySys = TRUE;
		//pCamera->SetTrackInfo(trackInfo);
		KG_PROCESS_ERROR(m_mapMasters.find(m_dwCameraID) != m_mapMasters.end());
		m_mapMasters[m_dwCameraID].pMaster = pCamera;
		SAFE_DELETE(m_pGhostCameraForFree);
		m_bIsFreeCamera = FALSE;
	}
	else
	{
		KG_PROCESS_ERROR(m_pCurrentScene);
		IKG3DCamera* pCamera = m_pCurrentScene->GetCurrentCamera();
		KG_PROCESS_ERROR(pCamera);
		SAFE_DELETE(m_pGhostCameraForFree);
		m_pGhostCameraForFree = pCamera->GetNew();
		KG_PROCESS_ERROR(m_pGhostCameraForFree);
		KG_COM_PROCESS_ERROR(pCamera->Copy(m_pGhostCameraForFree));
	/*	TrackCameraInfo trackInfo;
		pCamera->GetTrackInfo(&trackInfo);
		trackInfo.bEnableTrack = FALSE;
		trackInfo.nUseObstructdAvert = FALSE;
		trackInfo.nUseFlexibilitySys = FALSE;
		pCamera->SetTrackInfo(trackInfo);*/
		KG_PROCESS_ERROR(m_mapMasters.find(m_dwCameraID) != m_mapMasters.end());
		m_mapMasters[m_dwCameraID].pMaster = m_pGhostCameraForFree;
		m_bIsFreeCamera = TRUE;
	}
	hr = S_OK;
Exit0:
	return hr;
}

HRESULT RecPlayer::AdjustStartTime()
{
	if (m_pCurrentEvent != NULL && m_dwCurrentEvents < m_dwEventsCount)
		m_dwRecPlayStartTime = g_cGraphicsTool.GetNowTime() - (m_pCurrentEvent->dwTime - m_dwRecordStartTime);
	return S_OK;
}

HRESULT RecPlayer::StepToEvent(DWORD dwStepToEvent)
{
	HRESULT hr = E_FAIL;
	KG_PROCESS_ERROR(m_PlayState == PLAY_STATE_PLAY);

    m_fStepToEventPer = 0;
	int nStepedEventCount = 0;

    if (dwStepToEvent > m_dwCurrentEvents)
	{
        while (m_dwCurrentEvents < m_dwEventsCount && (m_dwCurrentEvents < dwStepToEvent))
		{
			ProcessOneEvent(m_pCurrentEvent);
            m_dwCurrentEvents++;
            ++m_CurrentIter;
            if (m_CurrentIter == m_EventHead.ListEvent.end())
            {
                if (m_bChanged)
                {
                    m_EventHead.dwFrameEndTime = m_pCurrentEvent->dwTime;
                }
                ++m_CurrentOutIt;
                m_EventHead     = *m_CurrentOutIt;
                m_CurrentIter   = m_EventHead.ListEvent.begin();
                if (m_bChanged)
                    m_dwNowFrameCount++;
            }
            m_pCurrentEvent = *m_CurrentIter;

			nStepedEventCount++;
			m_fStepToEventPer = (float)(nStepedEventCount) / (float)(dwStepToEvent) * 100;

            if (m_bChanged)
            {
                m_EventHead.dwFrameID        = m_dwNowFrameCount;
                m_EventHead.dwFrameStartTime = g_cGraphicsTool.GetNowTime();
                m_pCurrentEvent->dwFrame = m_dwNowFrameCount;
                m_pCurrentEvent->dwTime = g_cGraphicsTool.GetNowTime();
            }
		}
		AdjustStartTime();
	}
	else if (m_dwCurrentEvents > dwStepToEvent)
	{
		KG_PROCESS_ERROR(dwStepToEvent > m_dwRecordStartLineTime);
		StopRePlay();
		RePlay();
		KG_PROCESS_ERROR(dwStepToEvent > m_dwCurrentEvents);
		StepToEvent(dwStepToEvent);
	}
	hr = S_OK;
Exit0:
	return hr;
}
HRESULT RecPlayer::RePlay()
{
    HRESULT hResult = E_FAIL;

    KG_PROCESS_SUCCESS(m_dwEventsCount == 0);

	if (m_PlayState == PLAY_STATE_NONE)
	{

		m_pOldScene = NULL;
		m_nOldWindowID = 0;
		m_dwOldCameraID = 0;
		MastersUnit();
		m_PlayState = PLAY_STATE_PLAY;
		m_bIsBindIDCreated = FALSE;
		m_pCurrentScene   = NULL;
        m_dwCurrentEvents = 0;

        m_CurrentOutIt  = m_ListEventHead.begin();
        m_EventHead     = *m_CurrentOutIt;

        m_CurrentIter   = m_EventHead.ListEvent.begin();
		m_pCurrentEvent = *m_CurrentIter;

		m_dwRecPlayStartTime = g_cGraphicsTool.GetNowTime();
		m_dwNowFrameCount = 0;

        if (m_bChanged)
        {
            m_EventHead.dwFrameID        = m_dwNowFrameCount;
            m_EventHead.dwFrameStartTime = g_cGraphicsTool.GetNowTime();
            m_pCurrentEvent->dwFrame = m_dwNowFrameCount;
            m_pCurrentEvent->dwTime  = g_cGraphicsTool.GetNowTime();
        }
		IKG3DSoundShell *pSoundShell = g_cEngineManager.GetSound();
		if (pSoundShell)
			pSoundShell->Lock();
		StepToEvent(m_dwRecordStartLineTime);
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

Exit1:
    hResult = S_OK;
Exit0:
    return hResult;
}

HRESULT RecPlayer::PauseRePlay()
{	
	if (m_PlayState == PLAY_STATE_PLAY)
	{
		m_PlayState = PLAY_STATE_PAUSE;
		return S_OK;
	}
	return E_FAIL;
}

HRESULT RecPlayer::StopRePlay()
{

    MastersUnit();

    m_pOldScene = NULL;
    m_nOldWindowID = 0;
    m_dwOldCameraID = 0;

    m_PlayState = PLAY_STATE_NONE;
    m_dwCurrentEvents = 0;
    m_pCurrentEvent   = NULL;
    m_bIsBindIDCreated = FALSE;
    m_pCurrentScene = NULL;
    m_dwNowFrameCount = 0;
	return S_OK;
}

HRESULT RecPlayer::StartMovieOut()
{
	HRESULT hr = E_FAIL;
    IEKG3DSceneSceneEditor* pIEEditor = NULL;
    RecordOption* pRecordOption = NULL;

	KG_PROCESS_ERROR(!m_bIsMovieing);

	pIEEditor = dynamic_cast<IEKG3DSceneSceneEditor*>(m_pCurrentScene);
	KG_PROCESS_ERROR(pIEEditor);

    pRecordOption = g_cEngineManager.GetRecordOption();
    KGLOG_PROCESS_ERROR(pRecordOption);

    hr = pIEEditor->BeginMovieREC(
        pRecordOption->Size, 
        pRecordOption->fInterval, 
        pRecordOption->FiterType,
        pRecordOption->ImageType,
        pRecordOption->nAutoScale);
	if (hr == S_OK)
	{
		m_bIsMovieing = TRUE;
		m_pEventMovieStart = m_pCurrentEvent;
	}

Exit0:
	return hr;
}

HRESULT RecPlayer::StopMovieOut()
{
	HRESULT hr = E_FAIL;
	KG_PROCESS_ERROR(m_bIsMovieing);
	IEKG3DSceneSceneEditor* pIEEditor = dynamic_cast<IEKG3DSceneSceneEditor*>(m_pCurrentScene);
	KG_PROCESS_ERROR(pIEEditor);
	hr = pIEEditor->EndMovieREC();
	if (hr == S_OK)
	{
		m_bIsMovieing = FALSE;
		m_pEventMovieStop = m_pCurrentEvent;
	}
Exit0:
	return hr;	
}

BOOL RecPlayer::IsMovieOut()
{
	return m_bIsMovieing;
}

HRESULT RecPlayer::SetAngle(FLOAT fAngle)
{
	HRESULT hr = E_FAIL;
	D3DXMATRIX matStepRotion;
	D3DXVECTOR3 vecCameraDir,vecLookAt,vecPos;
	m_fAngle = fAngle;
	if (m_CameraType == CAMERA_TYPE_BIND || m_CameraType == CAMERA_TYPE_BINDPLAYER)
	{	
		IKG3DCamera* pCamera = static_cast<IKG3DCamera*>(m_mapMasters[m_dwCameraID].pMaster);
		KG_PROCESS_ERROR(pCamera);
		pCamera->GetPosition(&vecPos);

		D3DXMatrixRotationAxis(
			&matStepRotion,
			&D3DXVECTOR3(0,1,0),
			m_fAngle
			);
		vecCameraDir = D3DXVECTOR3(0,0,100);
		D3DXVec3TransformNormal(
			&vecCameraDir,
			&vecCameraDir,
			&matStepRotion
			);
		vecLookAt = vecPos + vecCameraDir;
		pCamera->SetLookAtPosition(vecLookAt);
		//float fFovy,Aspect,fzNear,fzFar;
		//pCamera->GetGlobalPerspective(&fFovy,&Aspect,&fzNear,&fzFar);
		//pCamera->SetGlobalPerspective(fFovy,1.0,fzNear,fzFar);
	}	
	hr = S_OK;
Exit0:
	return hr;
}

IERecPlayer::CameraType RecPlayer::GetCameraType()
{
	return m_CameraType;
}

void RecPlayer::SetCameraType(CameraType Type)
{
	m_CameraType = Type;
}

FLOAT RecPlayer::GetAngle()
{
	return m_fAngle;
}

DWORD RecPlayer::GetBindID()
{
	return m_dwBindID;
}

void RecPlayer::SetBindID(DWORD dwID)
{
	m_dwBindID = dwID;
}

HRESULT RecPlayer::GetFirstObj(DWORD *pdwID, RecObj** ppObj)
{
	HRESULT hRetCode = E_FAIL;
	KG_PROCESS_ERROR(pdwID);
	KG_PROCESS_ERROR(ppObj);
	KG_PROCESS_ERROR(m_mapObj.size());
	
	m_itCurrent = m_mapObj.begin();
	*pdwID = m_itCurrent->first;
	*ppObj = &(m_itCurrent->second);

	m_itCurrent++;
	hRetCode = S_OK;
Exit0:
	return hRetCode;
}

HRESULT RecPlayer::GetNextObj(DWORD *pdwID, RecObj** ppObj)
{
	HRESULT hRetCode = E_FAIL;
	KG_PROCESS_ERROR(pdwID);
	KG_PROCESS_ERROR(ppObj);

	KG_PROCESS_ERROR(m_itCurrent != m_mapObj.end());
	*pdwID = m_itCurrent->first;
	*ppObj = &(m_itCurrent->second);
	m_itCurrent++;
	hRetCode = S_OK;
Exit0:
	return hRetCode;
}

int RecPlayer::GetObjNum()
{
    return static_cast<int>(m_mapObj.size());
}

HRESULT RecPlayer::GetIDObj(DWORD dwID, RecObj** ppObj)
{
    HRESULT hResult = E_FAIL;
    map<DWORD, RecObj>::iterator it;

    KG_PROCESS_ERROR(ppObj);
    it = m_mapObj.find(dwID);
    KG_PROCESS_ERROR(it != m_mapObj.end());
    *ppObj = &(it->second);

    hResult = S_OK;
Exit0:
    return hResult;
}

HRESULT RecPlayer::CameraSmoothSave(LPCSTR szFileName)
{
	 EventRecord* pEventEnd = m_pEvents + m_dwEventsCount;
	 EventRecord* pEvents = m_pEvents;
	 DWORD dwSampeStep = 1000;
	 int nPosSampleCout = 0;
	 D3DXVECTOR3 vecPosSample[4];
	 DWORD dwCurrentPosID = 0;
	 DWORD dwNowSampleTime = 0;
	 vector<EventRecord*> vecPosEventList;
	 while (pEvents != pEventEnd)
	 {
		 if (pEvents->EventFunction == FT_KG3DCAMERA_SETPOSITION)
		 {
			 if (dwCurrentPosID != pEvents->dwID)		
			 {
				 KGLOG_PROCESS_ERROR(pEvents->AParameters.pData);
				 D3DXVECTOR3* pvecPos = (D3DXVECTOR3*)(&(pEvents->AParameters.pData[pEvents->AParameters.nFrom[0]]));
				 KG_PROCESS_ERROR(pvecPos);
				 vecPosSample[0] = *pvecPos;
				 nPosSampleCout = 0;
				 nPosSampleCout++;
				 dwCurrentPosID = pEvents->dwID;
				 dwNowSampleTime = pEvents->dwTime;
				 vecPosEventList.clear();
			 }
			 else if (pEvents->dwTime - dwNowSampleTime > dwSampeStep) 
			 {
				 KGLOG_PROCESS_ERROR(pEvents->AParameters.pData);
				 D3DXVECTOR3* pvecPos = (D3DXVECTOR3*)(&(pEvents->AParameters.pData[pEvents->AParameters.nFrom[0]]));
				 KG_PROCESS_ERROR(pvecPos);
				 vecPosSample[nPosSampleCout] = *pvecPos;		
				 nPosSampleCout++;
				 dwNowSampleTime = pEvents->dwTime;
				 if (nPosSampleCout == 4)
				 {
                     DWORD dwStartTime = vecPosEventList[0]->dwTime;
                     DWORD dwLastTime = pEvents->dwTime;
                     for (size_t i = 0; i < vecPosEventList.size();i++)
                     {
                         float fInterValue = (float)(vecPosEventList[i]->dwTime - dwStartTime) / (float)(dwLastTime - dwStartTime);
                         float fInvInterValue = 1 - fInterValue;
                         D3DXVECTOR3 vecInterPos = vecPosSample[0] * fInvInterValue * fInvInterValue * fInvInterValue;
                         vecInterPos += vecPosSample[1] * 3.0f * fInterValue * fInvInterValue * fInvInterValue;
                         vecInterPos += vecPosSample[2] * 3.0f * fInterValue * fInterValue * fInvInterValue;
                         vecInterPos += vecPosSample[3] * fInterValue * fInterValue * fInterValue;
                         KGLOG_PROCESS_ERROR(vecPosEventList[i]->AParameters.pData);
                         D3DXVECTOR3* pvecPos = (D3DXVECTOR3*)(&(vecPosEventList[i]->AParameters.pData[vecPosEventList[i]->AParameters.nFrom[0]]));
                         KG_PROCESS_ERROR(pvecPos);
                         *pvecPos = vecInterPos;
                     }
                     vecPosSample[0] = vecPosSample[nPosSampleCout - 1];
                     nPosSampleCout = 0;
                     nPosSampleCout++;
                     vecPosEventList.clear();
				 }
			 }
			 vecPosEventList.push_back(pEvents);
		 }
		 pEvents++;
	 }
	
	 nPosSampleCout = 0;
	 dwNowSampleTime = 0;
	 dwCurrentPosID = 0;
	 vecPosEventList.clear();
	 pEvents = m_pEvents;
	 while (pEvents != pEventEnd)
	 {
		 if (pEvents->EventFunction == FT_KG3DCAMERA_SETLOOKATPOSITION)
		 {
			 if (dwCurrentPosID != pEvents->dwID)		
			 {
				 KGLOG_PROCESS_ERROR(pEvents->AParameters.pData);
				 D3DXVECTOR3* pvecPos = (D3DXVECTOR3*)(&(pEvents->AParameters.pData[pEvents->AParameters.nFrom[0]]));
				 KG_PROCESS_ERROR(pvecPos);
				 vecPosSample[0] = *pvecPos;
				 nPosSampleCout = 0;
				 nPosSampleCout++;
				 dwCurrentPosID = pEvents->dwID;
				 dwNowSampleTime = pEvents->dwTime;
				 vecPosEventList.clear();
			 }
			 else if (pEvents->dwTime - dwNowSampleTime > dwSampeStep) 
			 {
				 KGLOG_PROCESS_ERROR(pEvents->AParameters.pData);
				 D3DXVECTOR3* pvecPos = (D3DXVECTOR3*)(&(pEvents->AParameters.pData[pEvents->AParameters.nFrom[0]]));
				 KG_PROCESS_ERROR(pvecPos);
				 vecPosSample[nPosSampleCout] = *pvecPos;		
				 nPosSampleCout++;
				 dwNowSampleTime = pEvents->dwTime;
				 if (nPosSampleCout == 4)
				 {
					 DWORD dwStartTime = vecPosEventList[0]->dwTime;
					 DWORD dwLastTime = pEvents->dwTime;
					 for (size_t i = 0; i < vecPosEventList.size();i++)
					 {
						 float fInterValue = (float)(vecPosEventList[i]->dwTime - dwStartTime) / (float)(dwLastTime - dwStartTime);
						 float fInvInterValue = 1 - fInterValue;
						 D3DXVECTOR3 vecInterPos = vecPosSample[0] * fInvInterValue * fInvInterValue * fInvInterValue;
						 vecInterPos += vecPosSample[1] * 3.0f * fInterValue * fInvInterValue * fInvInterValue;
						 vecInterPos += vecPosSample[2] * 3.0f * fInterValue * fInterValue * fInvInterValue;
						 vecInterPos += vecPosSample[3] * fInterValue * fInterValue * fInterValue;
						 KGLOG_PROCESS_ERROR(vecPosEventList[i]->AParameters.pData);
						 D3DXVECTOR3* pvecPos = (D3DXVECTOR3*)(&(vecPosEventList[i]->AParameters.pData[vecPosEventList[i]->AParameters.nFrom[0]]));
						 KG_PROCESS_ERROR(pvecPos);
						 *pvecPos = vecInterPos;
					 }
					 vecPosSample[0] = vecPosSample[nPosSampleCout - 1];
					 nPosSampleCout = 0;
					 nPosSampleCout++;
					 vecPosEventList.clear();
				 }
			 }
			 vecPosEventList.push_back(pEvents);
		 }
		 pEvents++;
	 }
	if (szFileName != NULL) 
		SaveRecordFile(szFileName);
	return S_OK;
Exit0:
	return E_FAIL;
}


HRESULT RecPlayer::GetLoadingProgress(float* pnLoadFilePer,float* pnStepToEventPer,int* pnLoadScenePer)
{
	HRESULT hRetCode = E_FAIL;
	KG_PROCESS_ERROR(pnLoadFilePer);
	KG_PROCESS_ERROR(pnStepToEventPer);
	KG_PROCESS_ERROR(pnLoadScenePer);
	*pnLoadFilePer = m_fLoadFilePer;
	*pnStepToEventPer = m_fStepToEventPer;
	//if (m_pCurrentScene != NULL && m_pCurrentEvent != NULL && m_pCurrentEvent->EventFunction == FT_KG3DSCENEEDITOR_LOADFROMFILE)
	//	*pnLoadScenePer = (int)(m_pCurrentScene->GetLoadingProcess() * 100);
	//else
	*pnLoadScenePer = 0;
	hRetCode = S_OK;
Exit0:
	return hRetCode;
}

HRESULT RecPlayer::GetEventTime(EventRecord* pEvent, int& nTime)
{
    HRESULT hResult = E_FAIL;
    KG_PROCESS_ERROR(pEvent);
    ASSERT(m_pStartLineEvent);
        
    nTime = (int)(((double)pEvent->dwTime - (double)m_pStartLineEvent->dwTime) / 1000);

    hResult = S_OK;
Exit0:
    return hResult;
}

HRESULT RecPlayer::Play()
{
    RePlay();
    while (m_dwCurrentEvents < m_dwEventsCount)
    {
        ProcessOneEvent(m_pCurrentEvent);
        m_dwCurrentEvents++;
        ++m_CurrentIter;
        if (m_CurrentIter == m_EventHead.ListEvent.end())
        {
            m_EventHead.dwFrameEndTime = m_pCurrentEvent->dwTime;
            ++m_CurrentOutIt;
            if (m_CurrentOutIt == m_ListEventHead.end())
                break;
            m_EventHead   = *m_CurrentOutIt;
            m_CurrentIter = m_EventHead.ListEvent.begin();
            if (m_bChanged)
                m_dwNowFrameCount++;
        }
        m_pCurrentEvent = *m_CurrentIter;

        if (m_bChanged)
        {
            m_EventHead.dwFrameID        = m_dwNowFrameCount;
            m_EventHead.dwFrameStartTime = g_cGraphicsTool.GetNowTime();
            m_pCurrentEvent->dwFrame = m_dwNowFrameCount;
            m_pCurrentEvent->dwTime  = g_cGraphicsTool.GetNowTime();
        }
    }
    StopRePlay();
    
    return S_OK;
}

DWORD RecPlayer::GetNowEventNum()
{
    DWORD dwResult = 0;
    if (m_dwCurrentEvents < m_dwRecordStartLineTime)
        dwResult = 0;
    else
        dwResult = m_dwCurrentEvents - m_dwRecordStartLineTime;
    return dwResult;
}

DWORD RecPlayer::GetEventNum()
{
    ASSERT(m_dwEventsCount >= m_dwRecordStartLineTime);
    return m_dwEventsCount - m_dwRecordStartLineTime;
}