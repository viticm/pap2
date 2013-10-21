#include "StdAfx.h"
#include "RecPlayer.h"
#include "IEOther.h"
#include "IEEditor.h"


IRecPlayer* g_GetNewRecPlayer()
{
	return new RecPlayer;
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
	m_dwCurrentEventNum = 0;
	m_fPlaySpeed = 1;
	m_dwRecordStartLineTime = 0;
	m_dwRecordStopLineTime = 0;

	m_dwBindID = 42;
	m_bIsBindIDCreated = FALSE;
	m_CameraType = CAMERA_TYPE_RECORD;
	m_fAngle = M_PI;

	m_dwCameraPastFromTime = 0;
	m_dwCameraPastToTime = 0;
	m_dwCameraNowFromTime = 0;
	m_vecFromPos = D3DXVECTOR3(0,0,0);
	m_vecToPos = D3DXVECTOR3(0,0,0);

	m_bIsMovieing = FALSE;
	m_pEventMovieStart = NULL;
	m_pEventMovieStop = NULL;
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


HRESULT RecPlayer::Init()
{
	list<EventRecord>::iterator it = m_listEvents.begin();
	while (it != m_listEvents.end())
	{
		switch (it->EventFunction)
		{
		case FT_NONE:
			break;
		case FT_KG3DMODEL_LOADFROMFILE:
		case FT_KG3DSCENEEDITOR_LOADFROMFILE:
			{
				KG_PROCESS_ERROR(m_mapObj.find(it->dwID) == m_mapObj.end());
				RecObj MyObj;
				MyObj.pEventHead = &(*it);
				MyObj.pCurrentEvent = &(*it);
				it->pNextEvent = NULL;

				TrueType* ttType = (TrueType*)(&(it->AParameters.pData[it->AParameters.nFrom[0]]));
				KG_PROCESS_ERROR(ttType);
				MyObj.ObjType = *ttType;
				char* cszFileName = (char*)(&(it->AParameters.pData[it->AParameters.nFrom[1]]));
				KG_PROCESS_ERROR(cszFileName);
				MyObj.ObjName = cszFileName;

				m_mapObj[it->dwID] = MyObj;

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
				KG_PROCESS_ERROR(m_mapObj.find(it->dwID) != m_mapObj.end());
				KG_PROCESS_ERROR(m_mapObj[it->dwID].pCurrentEvent);
				(m_mapObj[it->dwID].pCurrentEvent)->pNextEvent = &(*it);
				it->pNextEvent = NULL;
				m_mapObj[it->dwID].pCurrentEvent = &(*it);
			}
			break;
	
		case FT_KG3DSCENEEDITOR_ADDOUTPUTWINDOW:
			{				
				KG_PROCESS_ERROR(m_mapObj.find(it->dwID) != m_mapObj.end());
				KG_PROCESS_ERROR(m_mapObj[it->dwID].pCurrentEvent);
				(m_mapObj[it->dwID].pCurrentEvent)->pNextEvent = &(*it);
				it->pNextEvent = NULL;

				DWORD* pdwCameraID = (DWORD*)(&(it->AParameters.pData[it->AParameters.nFrom[1]])); 
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
				KG_PROCESS_ERROR(m_mapObj.find(it->dwID) != m_mapObj.end());
				if (m_mapObj[it->dwID].pEventHead == NULL)
				{
					m_mapObj[it->dwID].pEventHead = &(*it);
					m_mapObj[it->dwID].pCurrentEvent = &(*it);
					it->pNextEvent = NULL;
				}
				else
				{
					KG_PROCESS_ERROR(m_mapObj[it->dwID].pCurrentEvent);
					(m_mapObj[it->dwID].pCurrentEvent)->pNextEvent = &(*it);
					it->pNextEvent = NULL;
					m_mapObj[it->dwID].pCurrentEvent = &(*it);
				}				
			}
			break;		
		default:
			break;
		}
Exit0:
		it++;
	}
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
				SAFE_RELEASE(pScene);
			}
			break;
		case TYPEMODEL:
			{
				IKG3DModel* pModel = (IKG3DModel*)((itMasters->second).pMaster);
				SAFE_RELEASE(pModel);
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
	list<EventRecord>::iterator itRecord = m_listEvents.begin();
	while (itRecord != m_listEvents.end())
	{
		delete[](itRecord->AParameters.pData);
		itRecord++;
	}
	m_listEvents.clear();
	SAFE_DELETE(m_pGhostCameraForFree);
	return S_OK;
}


HRESULT RecPlayer::SaveIniRecordFile(string szFileName)
{
	int nCount = 0;
	TCHAR SectionName[MAX_PATH];
	IIniFile* pFile = g_OpenIniFile(szFileName.c_str(),false,true);
	if (!pFile)
		return E_FAIL;
	pFile->WriteInteger("MAIN","NumEvent",(int)m_listEvents.size());
	pFile->WriteInteger("MAIN","RecordStartTime",(int)m_dwRecordStartTime);
	list<EventRecord>::iterator itRecord = m_listEvents.begin();
	while (itRecord != m_listEvents.end())
	{
		wsprintf(SectionName,"Event%d",nCount);
		pFile->WriteInteger(SectionName,"ID",itRecord->dwID);
		pFile->WriteInteger(SectionName,"Function",(int)itRecord->EventFunction);
		pFile->WriteInteger(SectionName,"Time",itRecord->dwTime);
		pFile->WriteInteger(SectionName,"ParametersLength",(int)(itRecord->AParameters.size_tDataSize));
		pFile->WriteInteger(SectionName,"ParametersNum",(int)(itRecord->AParameters.nNum));
		pFile->WriteMultiInteger(SectionName,"ParametersFrom",itRecord->AParameters.nFrom,(int)(itRecord->AParameters.nNum));
		//pFile->WriteStruct(SectionName,"ParametersStruct",&(itRecord->AParameters),sizeof(Parameters));
		if (itRecord->AParameters.size_tDataSize != 0)
			pFile->WriteStruct(SectionName,"ParameData",itRecord->AParameters.pData,(int)(itRecord->AParameters.size_tDataSize));
		itRecord++;
		nCount++;
	}
	pFile->Save(szFileName.c_str());
	pFile->Release();
	return S_OK;
}
HRESULT RecPlayer::LoadIniRecordFile(string szFileName)
{
	int nNumEvent = 0;
	TCHAR SectionName[MAX_PATH];
	IIniFile* pFile = g_OpenIniFile(szFileName.c_str(),false,false);
	if (!pFile)
		return E_FAIL;
	pFile->GetInteger("MAIN","NumEvent",0,&nNumEvent);
	pFile->GetInteger("MAIN","RecordStartTime",0,(int*)(&m_dwRecordStartTime));
	for (int i = 0;i < nNumEvent;i++)
	{
		wsprintf(SectionName,"Event%d",i);
		EventRecord eventRecord;
		int nID = 0;
		pFile->GetInteger(SectionName,"ID",0,&nID);
		eventRecord.dwID = nID;
		int nFunctionType = 0;
		pFile->GetInteger(SectionName,"Function",0,&nFunctionType);
		eventRecord.EventFunction = (FunctionType)nFunctionType;
		int nTime = 0;
		pFile->GetInteger(SectionName,"Time",0,&nTime);
		eventRecord.dwTime = nTime;
		int nSize = 0;
		pFile->GetInteger(SectionName,"ParametersLength",0,&nSize);
		eventRecord.AParameters.size_tDataSize = nSize;
		int nNum = 0;
		pFile->GetInteger(SectionName,"ParametersNum",0,&nNum);
		eventRecord.AParameters.nNum = nNum;
		pFile->GetMultiInteger(SectionName,"ParametersFrom",(int*)(&eventRecord.AParameters.nFrom),nNum);
		char* pData = new char[nSize];
		pFile->GetStruct(SectionName,"ParameData",pData,nSize);
		eventRecord.AParameters.pData = pData;
		m_listEvents.push_back(eventRecord);
	}
	pFile->Release();
	return S_OK;
}

HRESULT RecPlayer::SaveRecordFile(LPCSTR szFileName)
{
	HRESULT hr = E_FAIL;
	unsigned long uSize = 0;
	list<EventRecord>::iterator itRecord = m_listEvents.begin();
	KG_PROCESS_ERROR(szFileName);
	
	IFile* pFile = g_OpenFile(szFileName,false,true);
	KG_PROCESS_ERROR(pFile);
	
	int nNumEvent = (int)m_listEvents.size();
	uSize = pFile->Write(&nNumEvent,sizeof(int));
	KG_PROCESS_ERROR(uSize == sizeof(int));
	uSize = pFile->Write(&m_dwRecordStartTime,sizeof(DWORD));
	KG_PROCESS_ERROR(uSize == sizeof(DWORD));
	uSize = pFile->Write(&m_dwRecordStartLineTime,sizeof(DWORD));
	KG_PROCESS_ERROR(uSize == sizeof(DWORD));
	uSize = pFile->Write(&m_dwRecordStopLineTime,sizeof(DWORD));
	KG_PROCESS_ERROR(uSize == sizeof(DWORD));
	while (itRecord != m_listEvents.end())
	{
		uSize = pFile->Write(&(itRecord->dwID),sizeof(DWORD));
		KG_PROCESS_ERROR(uSize == sizeof(DWORD));
		uSize = pFile->Write(&(itRecord->EventFunction),sizeof(FunctionType));
		KG_PROCESS_ERROR(uSize == sizeof(FunctionType));
		uSize = pFile->Write(&(itRecord->dwTime),sizeof(DWORD));
		KG_PROCESS_ERROR(uSize == sizeof(DWORD));
		uSize = pFile->Write(&(itRecord->AParameters.size_tDataSize),sizeof(size_t));
		KG_PROCESS_ERROR(uSize == sizeof(size_t));
		uSize = pFile->Write(&(itRecord->AParameters.nNum),sizeof(int));
		KG_PROCESS_ERROR(uSize == sizeof(int));
		uSize = pFile->Write(itRecord->AParameters.nFrom,(itRecord->AParameters.nNum) * sizeof(int));
		KG_PROCESS_ERROR(uSize == (itRecord->AParameters.nNum) * sizeof(int));
		uSize = pFile->Write(itRecord->AParameters.pData,itRecord->AParameters.size_tDataSize);
		KG_PROCESS_ERROR(uSize == itRecord->AParameters.size_tDataSize);
		itRecord++;
	}
	pFile->Close();
	hr = S_OK;

Exit0:
	SAFE_RELEASE(pFile);
	return hr;
}
HRESULT RecPlayer::LoadRecordFile(LPCSTR szFileName)
{
	HRESULT hr = E_FAIL;
	unsigned long uSize = 0;
	IFile* pFile= g_OpenFile(szFileName,false,false);
	KG_PROCESS_ERROR(pFile);
	int nNumEvent = 0;
	uSize = pFile->Read(&nNumEvent,sizeof(int));
	KG_PROCESS_ERROR(uSize == sizeof(int));
	uSize = pFile->Read(&m_dwRecordStartTime,sizeof(DWORD));
	KG_PROCESS_ERROR(uSize == sizeof(DWORD));
	uSize = pFile->Read(&m_dwRecordStartLineTime,sizeof(DWORD));
	KG_PROCESS_ERROR(uSize == sizeof(DWORD));
	uSize = pFile->Read(&m_dwRecordStopLineTime,sizeof(DWORD));
	KG_PROCESS_ERROR(uSize == sizeof(DWORD));
	for (int i = 0;i < nNumEvent;i++)
	{
		EventRecord eventRecord;
		uSize = pFile->Read(&(eventRecord.dwID),sizeof(DWORD));
		KG_PROCESS_ERROR(uSize == sizeof(DWORD));
		uSize = pFile->Read(&(eventRecord.EventFunction),sizeof(FunctionType));
		KG_PROCESS_ERROR(uSize == sizeof(FunctionType));
		uSize = pFile->Read(&(eventRecord.dwTime),sizeof(DWORD));
		KG_PROCESS_ERROR(uSize == sizeof(DWORD));
		uSize = pFile->Read(&(eventRecord.AParameters.size_tDataSize),sizeof(size_t));
		KG_PROCESS_ERROR(uSize == sizeof(size_t));
		uSize = pFile->Read(&(eventRecord.AParameters.nNum),sizeof(int));
		KG_PROCESS_ERROR(uSize == sizeof(int));
		uSize = pFile->Read(eventRecord.AParameters.nFrom,eventRecord.AParameters.nNum * sizeof(int));
		KG_PROCESS_ERROR(uSize == eventRecord.AParameters.nNum * sizeof(int));
		char* pData = new char[eventRecord.AParameters.size_tDataSize];
		uSize = pFile->Read(pData,eventRecord.AParameters.size_tDataSize);
		KG_PROCESS_ERROR(uSize == eventRecord.AParameters.size_tDataSize);
		eventRecord.AParameters.pData = pData;
		m_listEvents.push_back(eventRecord);
	}
	pFile->Close();
	hr = S_OK;
	Init();
Exit0:
	SAFE_RELEASE(pFile);
	return hr;
}

DWORD RecPlayer::GetEventNum()
{
	return m_listEvents.size();
}

int RecPlayer::GetObjNum()
{
	return static_cast<int>(m_mapObj.size());
}

RecPlayer::PlayState RecPlayer::GetPlayarState()
{
	return m_PlayState;
}

BOOL RecPlayer::IsFreeCamera()
{
	return m_bIsFreeCamera;
}

DWORD RecPlayer::GetNowEventNum()
{
	if (m_dwCurrentEventNum <= m_listEvents.size())
		return m_dwCurrentEventNum;
	else
		return m_listEvents.size();
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

HRESULT RecPlayer::ProcessOneEvent(list<EventRecord>::iterator& it)
{
	HRESULT hr = E_FAIL;
	if (g_movieOption.bKeep)
	{
		if (&(*it) == m_pEventMovieStart)
		{
			StartMovieOut();
		}
		if (&(*it) == m_pEventMovieStop)
		{
			StopMovieOut();
		}
	}
	switch (it->EventFunction)
	{
	case FT_NONE:
		break;
	case FT_KG3DMODEL_LOADFROMFILE:
		{
			KG_PROCESS_ERROR(m_mapMasters.find(it->dwID) == m_mapMasters.end());
			IKG3DResourceBase* pModelST = NULL;
			char* cszFileName = (char*)(&(it->AParameters.pData[it->AParameters.nFrom[1]]));
			KG_PROCESS_ERROR(cszFileName);
			unsigned* puFileNameHash = (unsigned*)(&(it->AParameters.pData[it->AParameters.nFrom[2]])); 
			KG_PROCESS_ERROR(puFileNameHash);
			unsigned* puOption = (unsigned*)(&(it->AParameters.pData[it->AParameters.nFrom[3]]));
			KG_PROCESS_ERROR(puOption);
			IKG3DResourceManager* pModelTable = g_p3DEngine->Get3DModelTable();
			KG_PROCESS_ERROR(pModelTable);
			pModelTable->LoadResourceFromFile(cszFileName,*puFileNameHash,*puOption,&pModelST);
			KG_PROCESS_ERROR(pModelST);
			Master MyMaster;
			MyMaster.mtType = TYPEMODEL;
			MyMaster.pMaster = (PVOID)(pModelST);
			m_mapMasters[it->dwID] = MyMaster;
			if (it->dwID == m_dwBindID)
				m_bIsBindIDCreated = TRUE;
		}
		break;
	case FT_KG3DMODEL_DESTRUCT:
		{
			KG_PROCESS_ERROR(m_mapMasters.find(it->dwID) != m_mapMasters.end());
			IKG3DModel* pModel = static_cast<IKG3DModel*>(m_mapMasters[it->dwID].pMaster);
			SAFE_RELEASE(pModel);
			m_mapMasters.erase(m_mapMasters.find(it->dwID));
		}
		break;
	case FT_KG3DMODEL_SETTRANSLATION:
		{
			KG_PROCESS_ERROR(m_mapMasters.find(it->dwID) != m_mapMasters.end());
			IKG3DModel* pModel = static_cast<IKG3DModel*>(m_mapMasters[it->dwID].pMaster);
			KG_PROCESS_ERROR(pModel);
			D3DXVECTOR3* pvecValue = (D3DXVECTOR3*)(&(it->AParameters.pData[it->AParameters.nFrom[0]]));
			KG_PROCESS_ERROR(pvecValue);
			pModel->SetTranslation(pvecValue);
			if (m_CameraType == CAMERA_TYPE_BIND && it->dwID == m_dwBindID)
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
			KG_PROCESS_ERROR(m_mapMasters.find(it->dwID) != m_mapMasters.end());
			IKG3DModel* pModel = static_cast<IKG3DModel*>(m_mapMasters[it->dwID].pMaster);
			KG_PROCESS_ERROR(pModel);
			D3DXQUATERNION* pvecValue = (D3DXQUATERNION*)(&(it->AParameters.pData[it->AParameters.nFrom[0]]));
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
			KG_PROCESS_ERROR(m_mapMasters.find(it->dwID) != m_mapMasters.end());
			IKG3DModel* pModel = static_cast<IKG3DModel*>(m_mapMasters[it->dwID].pMaster);
			KG_PROCESS_ERROR(pModel);
			D3DXVECTOR3* pvecValue = (D3DXVECTOR3*)(&(it->AParameters.pData[it->AParameters.nFrom[0]]));
			KG_PROCESS_ERROR(pvecValue);
			pModel->SetRotationCenter(pvecValue);
		}
		break;
	case FT_KG3DMODEL_SETSCALING:
		{
			KG_PROCESS_ERROR(m_mapMasters.find(it->dwID) != m_mapMasters.end());
			IKG3DModel* pModel = static_cast<IKG3DModel*>(m_mapMasters[it->dwID].pMaster);
			KG_PROCESS_ERROR(pModel);
			D3DXVECTOR3* pvecValue = (D3DXVECTOR3*)(&(it->AParameters.pData[it->AParameters.nFrom[0]]));
			KG_PROCESS_ERROR(pvecValue);
			pModel->SetScaling(pvecValue);
		}
		break;
	case FT_KG3DMODEL_SETSCALINGCENTER:
		{
			KG_PROCESS_ERROR(m_mapMasters.find(it->dwID) != m_mapMasters.end());
			IKG3DModel* pModel = static_cast<IKG3DModel*>(m_mapMasters[it->dwID].pMaster);
			KG_PROCESS_ERROR(pModel);
			D3DXVECTOR3* pvecValue = (D3DXVECTOR3*)(&(it->AParameters.pData[it->AParameters.nFrom[0]]));
			KG_PROCESS_ERROR(pvecValue);
			pModel->SetScalingCenter(pvecValue);
		}
		break;
	case FT_KG3DMODEL_SETSCALINGROTATION:
		{
			KG_PROCESS_ERROR(m_mapMasters.find(it->dwID) != m_mapMasters.end());
			IKG3DModel* pModel = static_cast<IKG3DModel*>(m_mapMasters[it->dwID].pMaster);
			KG_PROCESS_ERROR(pModel);
			D3DXQUATERNION* pvecValue = (D3DXQUATERNION*)(&(it->AParameters.pData[it->AParameters.nFrom[0]]));
			KG_PROCESS_ERROR(pvecValue);
			pModel->SetScalingRotation(pvecValue);
		}
		break;
	case FT_KG3DMODEL_RESETTRANSFORM:
		{
			KG_PROCESS_ERROR(m_mapMasters.find(it->dwID) != m_mapMasters.end());
			IKG3DModel* pModel = static_cast<IKG3DModel*>(m_mapMasters[it->dwID].pMaster);
			KG_PROCESS_ERROR(pModel);
			pModel->ResetTransform();
		}
		break;
	case FT_KG3DMODEL_BINDTOSOCKET:
		{
			KG_PROCESS_ERROR(m_mapMasters.find(it->dwID) != m_mapMasters.end());
			IKG3DModel* pModel = static_cast<IKG3DModel*>(m_mapMasters[it->dwID].pMaster);
			KG_PROCESS_ERROR(pModel);
			DWORD* pdwModelID = (DWORD*)(&(it->AParameters.pData[it->AParameters.nFrom[0]]));
			KG_PROCESS_ERROR(pdwModelID);
			KG_PROCESS_ERROR(m_mapMasters.find(*pdwModelID) != m_mapMasters.end());
			IKG3DModel* pBindModel = static_cast<IKG3DModel*>(m_mapMasters[*pdwModelID].pMaster);
			KG_PROCESS_ERROR(pBindModel);
			char* cszScoketName = (char*)(&(it->AParameters.pData[it->AParameters.nFrom[1]]));
			KG_PROCESS_ERROR(cszScoketName);	
			pModel->BindToSocket(pBindModel,cszScoketName);
		}
		break;
	case FT_KG3DMODEL_BINDTOBONE:
		{
			KG_PROCESS_ERROR(m_mapMasters.find(it->dwID) != m_mapMasters.end());
			IKG3DModel* pModel = static_cast<IKG3DModel*>(m_mapMasters[it->dwID].pMaster);
			KG_PROCESS_ERROR(pModel);
			DWORD* pdwModelID = (DWORD*)(&(it->AParameters.pData[it->AParameters.nFrom[0]]));
			KG_PROCESS_ERROR(pdwModelID);
			KG_PROCESS_ERROR(m_mapMasters.find(*pdwModelID) != m_mapMasters.end());
			IKG3DModel* pBindModel = static_cast<IKG3DModel*>(m_mapMasters[*pdwModelID].pMaster);
			KG_PROCESS_ERROR(pBindModel);
			char* cszBoneName = (char*)(&(it->AParameters.pData[it->AParameters.nFrom[1]]));
			KG_PROCESS_ERROR(cszBoneName);						
			pModel->BindToBone(pBindModel,cszBoneName);
		}
		break;
	case FT_KG3DMODEL_ATTACHMODEL:
		{
			KG_PROCESS_ERROR(m_mapMasters.find(it->dwID) != m_mapMasters.end());
			IKG3DModel* pModel = static_cast<IKG3DModel*>(m_mapMasters[it->dwID].pMaster);
			KG_PROCESS_ERROR(pModel);
			DWORD* pdwModelID = (DWORD*)(&(it->AParameters.pData[it->AParameters.nFrom[0]]));
			KG_PROCESS_ERROR(pdwModelID);
			KG_PROCESS_ERROR(m_mapMasters.find(*pdwModelID) != m_mapMasters.end());
			IKG3DModel* pAttachModel = static_cast<IKG3DModel*>(m_mapMasters[*pdwModelID].pMaster);
			KG_PROCESS_ERROR(pAttachModel);				
			pModel->AttachModel(pAttachModel);
		}
		break;
	case FT_KG3DMODEL_DETCHMODEL:
		{
			KG_PROCESS_ERROR(m_mapMasters.find(it->dwID) != m_mapMasters.end());
			IKG3DModel* pModel = static_cast<IKG3DModel*>(m_mapMasters[it->dwID].pMaster);
			KG_PROCESS_ERROR(pModel);
			DWORD* pdwModelID = (DWORD*)(&(it->AParameters.pData[it->AParameters.nFrom[0]]));
			KG_PROCESS_ERROR(pdwModelID);
			KG_PROCESS_ERROR(m_mapMasters.find(*pdwModelID) != m_mapMasters.end());
			IKG3DModel* pAttachModel = static_cast<IKG3DModel*>(m_mapMasters[*pdwModelID].pMaster);
			KG_PROCESS_ERROR(pAttachModel);				
			pModel->DetachModel(pAttachModel);
		}
		break;
	case FT_KG3DMODEL_PLAYANIMATION:
		{
			KG_PROCESS_ERROR(m_mapMasters.find(it->dwID) != m_mapMasters.end());
			IKG3DModel* pModel = static_cast<IKG3DModel*>(m_mapMasters[it->dwID].pMaster);
			KG_PROCESS_ERROR(pModel);
			DWORD* pdwPlayType = (DWORD*)(&(it->AParameters.pData[it->AParameters.nFrom[0]]));
			KG_PROCESS_ERROR(pdwPlayType);
			char* cszFileName = (char*)(&(it->AParameters.pData[it->AParameters.nFrom[1]]));
			KG_PROCESS_ERROR(cszFileName);
			FLOAT* pfSpeed = (FLOAT*)(&(it->AParameters.pData[it->AParameters.nFrom[2]])); 
			KG_PROCESS_ERROR(pfSpeed);
			FLOAT* pfStartTime = (FLOAT*)(&(it->AParameters.pData[it->AParameters.nFrom[3]]));
			KG_PROCESS_ERROR(pfStartTime);
			pModel->PlayAnimation(*pdwPlayType,cszFileName,*pfSpeed,*pfStartTime,NULL,NULL);

		}
		break;
	case FT_KG3DMODEL_PALYSPLITANIMATION:
		{
			KG_PROCESS_ERROR(m_mapMasters.find(it->dwID) != m_mapMasters.end());
			IKG3DModel* pModel = static_cast<IKG3DModel*>(m_mapMasters[it->dwID].pMaster);
			KG_PROCESS_ERROR(pModel);
			char* strAnimationName = (char*)(&(it->AParameters.pData[it->AParameters.nFrom[0]]));
			KG_PROCESS_ERROR(strAnimationName);
			DWORD* pdwSplitType = (DWORD*)(&(it->AParameters.pData[it->AParameters.nFrom[1]]));
			KG_PROCESS_ERROR(pdwSplitType);
			BOOL* pbKeepOld = (BOOL*)(&(it->AParameters.pData[it->AParameters.nFrom[2]])); 
			KG_PROCESS_ERROR(pbKeepOld);
			DWORD* pdwExtraInfo = (DWORD*)(&(it->AParameters.pData[it->AParameters.nFrom[3]]));
			KG_PROCESS_ERROR(pdwExtraInfo);
			DWORD* pdwPlayType = (DWORD*)(&(it->AParameters.pData[it->AParameters.nFrom[4]]));
			KG_PROCESS_ERROR(pdwPlayType);
			FLOAT* pfSpeed = (FLOAT*)(&(it->AParameters.pData[it->AParameters.nFrom[5]]));
			KG_PROCESS_ERROR(pfSpeed);
			FLOAT* pfStartTime = (FLOAT*)(&(it->AParameters.pData[it->AParameters.nFrom[6]]));
			KG_PROCESS_ERROR(pfStartTime);
			pModel->PlaySplitAnimation(strAnimationName,*pdwSplitType,*pbKeepOld,*pdwExtraInfo,*pdwPlayType,*pfSpeed,*pfStartTime,NULL,NULL);
		}
		break;
	case FT_KG3DMODEL_LOADMATERIALFROMFILE:
		{
			KG_PROCESS_ERROR(m_mapMasters.find(it->dwID) != m_mapMasters.end());
			IKG3DModel* pModel = static_cast<IKG3DModel*>(m_mapMasters[it->dwID].pMaster);
			KG_PROCESS_ERROR(pModel);
			char* cszFileName = (char*)(&(it->AParameters.pData[it->AParameters.nFrom[0]]));
			KG_PROCESS_ERROR(cszFileName);
			DWORD* pOption = (DWORD*)(&(it->AParameters.pData[it->AParameters.nFrom[1]])); 
			KG_PROCESS_ERROR(pOption);
			pModel->LoadMaterialFromFile(cszFileName,*pOption);
		}
		break;
	case FT_KG3DMODEL_SETALPHA:
		{
			//float fAlpha, BOOL bUseSpecail
			KG_PROCESS_ERROR(m_mapMasters.find(it->dwID) != m_mapMasters.end());
			IKG3DModel* pModel = static_cast<IKG3DModel*>(m_mapMasters[it->dwID].pMaster);
			KG_PROCESS_ERROR(pModel);
			float* pfAlpha = (float*)(&(it->AParameters.pData[it->AParameters.nFrom[0]]));
			KG_PROCESS_ERROR(pfAlpha);
			BOOL* pbUseSpecail = (BOOL*)(&(it->AParameters.pData[it->AParameters.nFrom[1]])); 
			KG_PROCESS_ERROR(pbUseSpecail);
			pModel->SetAlpha(*pfAlpha,*pbUseSpecail);
			//pModel->SetAlpha(1,FALSE);
		}
		break;
	case FT_KG3DMODEL_SETCALLBACKRENDER:
		{
			KG_PROCESS_ERROR(m_mapMasters.find(it->dwID) != m_mapMasters.end());
			IKG3DModel* pModel = static_cast<IKG3DModel*>(m_mapMasters[it->dwID].pMaster);
			KG_PROCESS_ERROR(pModel);
			pModel->SetAlpha(0,FALSE);
		}
		break;
	case FT_KG3DSCENEEDITOR_DESTRUCT:
		{
			KG_PROCESS_ERROR(m_mapMasters.find(it->dwID) != m_mapMasters.end());
			IKG3DScene* pSceneSceneEditor = static_cast<IKG3DScene*>(m_mapMasters[it->dwID].pMaster);
			if (m_pCurrentScene == pSceneSceneEditor)
				m_pCurrentScene = NULL;
			g_p3DEngine->RemoveScene(pSceneSceneEditor);
			pSceneSceneEditor = NULL;
			m_mapMasters.erase(m_mapMasters.find(it->dwID));	
		}
		break;
	case FT_KG3DSCENEEDITOR_ADDRENDERENTITY:
		{
			KG_PROCESS_ERROR(m_mapMasters.find(it->dwID) != m_mapMasters.end());
			IKG3DScene* pSceneSceneEditor = static_cast<IKG3DScene*>(m_mapMasters[it->dwID].pMaster);
			KG_PROCESS_ERROR(pSceneSceneEditor);
			DWORD* pdwEntityID = (DWORD*)(&(it->AParameters.pData[it->AParameters.nFrom[0]]));
			KG_PROCESS_ERROR(pdwEntityID);
			KG_PROCESS_ERROR(m_mapMasters.find(*pdwEntityID) != m_mapMasters.end());
			IKG3DModel* pModel = static_cast<IKG3DModel*>(m_mapMasters[*pdwEntityID].pMaster);
			KG_PROCESS_ERROR(pModel);
			unsigned* puClass = (unsigned*)(&(it->AParameters.pData[it->AParameters.nFrom[1]]));
			KG_PROCESS_ERROR(puClass);
			BOOL* pbSelectable = (BOOL*)(&(it->AParameters.pData[it->AParameters.nFrom[2]]));
			KG_PROCESS_ERROR(pbSelectable);
			pSceneSceneEditor->AddRenderEntity(pModel,*puClass,*pbSelectable);
		}
		break;
	case FT_KG3DSCENEEDITOR_REMOVERENDERENTITY:
		{
			KG_PROCESS_ERROR(m_mapMasters.find(it->dwID) != m_mapMasters.end());
			IKG3DScene* pSceneSceneEditor = static_cast<IKG3DScene*>(m_mapMasters[it->dwID].pMaster);
			KG_PROCESS_ERROR(pSceneSceneEditor);
			DWORD* pdwEntityID = (DWORD*)(&(it->AParameters.pData[it->AParameters.nFrom[0]]));
			KG_PROCESS_ERROR(pdwEntityID);
			KG_PROCESS_ERROR(m_mapMasters.find(*pdwEntityID) != m_mapMasters.end());
			IKG3DModel* pModel = static_cast<IKG3DModel*>(m_mapMasters[*pdwEntityID].pMaster);
			KG_PROCESS_ERROR(pModel);
			unsigned* puClass = (unsigned*)(&(it->AParameters.pData[it->AParameters.nFrom[1]]));
			KG_PROCESS_ERROR(puClass);
			BOOL* pbSelectable = (BOOL*)(&(it->AParameters.pData[it->AParameters.nFrom[2]]));
			KG_PROCESS_ERROR(pbSelectable);
			pSceneSceneEditor->RemoveRenderEntity(pModel,*puClass,*pbSelectable);
		}
		break;
	case FT_KG3DSCENEEDITOR_LOADFROMFILE:
		{
			KG_PROCESS_ERROR(m_mapMasters.find(it->dwID) == m_mapMasters.end());
			char* cszFileName = (char*)(&(it->AParameters.pData[it->AParameters.nFrom[1]]));
			KG_PROCESS_ERROR(cszFileName);
			unsigned* puFileNameHash = (unsigned*)(&(it->AParameters.pData[it->AParameters.nFrom[2]])); 
			KG_PROCESS_ERROR(puFileNameHash);
			unsigned* puOption = (unsigned*)(&(it->AParameters.pData[it->AParameters.nFrom[3]]));
			KG_PROCESS_ERROR(puOption);
			IKG3DScene* piScene = NULL;
			g_p3DEngine->NewOneScene(SCENETYPE_SCENEEDITOR, &piScene);
			KG_PROCESS_ERROR(piScene);
			piScene->LoadFromFile(cszFileName,*puFileNameHash,*puOption);
			m_pCurrentScene = piScene;
			Master MyMaster;
			MyMaster.mtType = TYPESCENEEDITOR;
			MyMaster.pMaster = (PVOID)(piScene);
			m_mapMasters[it->dwID] = MyMaster;
		}
		break;
	case FT_KG3DSCENEEDITOR_ADDOUTPUTWINDOW:
		{
			KG_PROCESS_ERROR(m_mapMasters.find(it->dwID) != m_mapMasters.end());
			IKG3DScene* pSceneSceneEditor = static_cast<IKG3DScene*>(m_mapMasters[it->dwID].pMaster);
			KG_PROCESS_ERROR(pSceneSceneEditor);
			char* cszName = (char*)(&(it->AParameters.pData[it->AParameters.nFrom[0]]));
			KG_PROCESS_ERROR(cszName);
			DWORD* pdwCameraID = (DWORD*)(&(it->AParameters.pData[it->AParameters.nFrom[1]])); 
			KG_PROCESS_ERROR(pdwCameraID);						
			int nWindowID = 0;
			if (m_hWnd != NULL)
			{
				pSceneSceneEditor->AddOutputWindow(cszName,m_hWnd,0,0,&nWindowID);
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
			KG_PROCESS_ERROR(m_mapMasters.find(it->dwID) != m_mapMasters.end());
			IKG3DCamera* pCamera = static_cast<IKG3DCamera*>(m_mapMasters[it->dwID].pMaster);
			KG_PROCESS_ERROR(pCamera);
			TrackCameraInfo* pTrackCameraInfo = (TrackCameraInfo*)(&(it->AParameters.pData[it->AParameters.nFrom[0]]));
			KG_PROCESS_ERROR(pTrackCameraInfo);
			pCamera->SetTrackInfo(*pTrackCameraInfo);
		}
		break;
	case FT_KG3DCAMERA_UPDATECAMERA:
		{
			if (m_CameraType == CAMERA_TYPE_RECORD || (m_CameraType == CAMERA_TYPE_BIND && !m_bIsBindIDCreated))		
			{
				KG_PROCESS_ERROR(m_mapMasters.find(it->dwID) != m_mapMasters.end());
				IKG3DCamera* pCamera = static_cast<IKG3DCamera*>(m_mapMasters[it->dwID].pMaster);
				KG_PROCESS_ERROR(pCamera);
				D3DXVECTOR3* pvecPosition = (D3DXVECTOR3*)(&(it->AParameters.pData[it->AParameters.nFrom[0]]));
				KG_PROCESS_ERROR(pvecPosition);
				D3DXVECTOR3* pvecAxesPos = (D3DXVECTOR3*)(&(it->AParameters.pData[it->AParameters.nFrom[1]]));
				KG_PROCESS_ERROR(pvecAxesPos);
				float* pfLookAtOffset = (float*)(&(it->AParameters.pData[it->AParameters.nFrom[2]]));
				KG_PROCESS_ERROR(pfLookAtOffset);
				float* pfTargetMoveSpeed = (float*)(&(it->AParameters.pData[it->AParameters.nFrom[3]]));
				KG_PROCESS_ERROR(pfTargetMoveSpeed);
				pCamera->UpdateCamera(*pvecPosition,*pvecAxesPos,*pfLookAtOffset,*pfTargetMoveSpeed);
			}
			else if (m_CameraType == CAMERA_TYPE_BINDPLAYER)
			{
				D3DXVECTOR3* pvecAxesPos = (D3DXVECTOR3*)(&(it->AParameters.pData[it->AParameters.nFrom[1]]));
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
				m_dwCameraPastFromTime = it->dwTime;
				m_dwCameraNowFromTime =  g_p3DTools->GetNowTime();//GetCurrentTime();
				m_vecFromPos = vecPos;
				int nStep = 0;
				EventRecord* pToEvent = it->pNextEvent;
				while (nStep < 10 && pToEvent != NULL && pToEvent->EventFunction != FT_KG3DCAMERA_UPDATECAMERA)
				{
					pToEvent = pToEvent->pNextEvent;
					nStep++;
				}
				if (nStep < 10 && pToEvent != NULL)
				{					
					D3DXVECTOR3* pvecAxesPos = (D3DXVECTOR3*)(&(pToEvent->AParameters.pData[it->AParameters.nFrom[1]]));
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
				KG_PROCESS_ERROR(m_mapMasters.find(it->dwID) != m_mapMasters.end());
				IKG3DCamera* pCamera = static_cast<IKG3DCamera*>(m_mapMasters[it->dwID].pMaster);
				KG_PROCESS_ERROR(pCamera);
				D3DXVECTOR3* pvecPos = (D3DXVECTOR3*)(&(it->AParameters.pData[it->AParameters.nFrom[0]]));
				KG_PROCESS_ERROR(pvecPos);
				if (pvecPos->y > 0)
					pCamera->SetPosition(*pvecPos);	
			}
		}
		break;
	case FT_KG3DCAMERA_SETLOOKATPOSITION:
		{
			if (m_CameraType == CAMERA_TYPE_RECORD || (m_CameraType == CAMERA_TYPE_BIND && !m_bIsBindIDCreated))		
			{
				KG_PROCESS_ERROR(m_mapMasters.find(it->dwID) != m_mapMasters.end());
				IKG3DCamera* pCamera = static_cast<IKG3DCamera*>(m_mapMasters[it->dwID].pMaster);
				KG_PROCESS_ERROR(pCamera);
				D3DXVECTOR3* pvecPos = (D3DXVECTOR3*)(&(it->AParameters.pData[it->AParameters.nFrom[0]]));
				KG_PROCESS_ERROR(pvecPos);
				if (pvecPos->y > 0)
					pCamera->SetLookAtPosition(*pvecPos);
			}
		}
		break;
	case FT_KG3DCAMERA_SETUPDIRECTION:
		{
			if (m_CameraType == CAMERA_TYPE_RECORD || (m_CameraType == CAMERA_TYPE_BIND && !m_bIsBindIDCreated))		
			{
				KG_PROCESS_ERROR(m_mapMasters.find(it->dwID) != m_mapMasters.end());
				IKG3DCamera* pCamera = static_cast<IKG3DCamera*>(m_mapMasters[it->dwID].pMaster);
				KG_PROCESS_ERROR(pCamera);
				D3DXVECTOR3* pvecUp = (D3DXVECTOR3*)(&(it->AParameters.pData[it->AParameters.nFrom[0]]));
				KG_PROCESS_ERROR(pvecUp);
				if (pvecUp->y > 0)
					pCamera->SetUpDirection(*pvecUp);
			}
		}
		break;
	case FT_KG3DCAMERA_SETGLOBALPERSPECTIVE:
		{
			//float fFovy, float fAspect, float fzNear, float fzFar
			//KG_PROCESS_ERROR(m_mapMasters.find(it->dwID) != m_mapMasters.end());
			//IKG3DCamera* pCamera = static_cast<IKG3DCamera*>(m_mapMasters[it->dwID].pMaster);
			//KG_PROCESS_ERROR(pCamera);
			//float* pfFovy = (float*)(&(it->AParameters.pData[it->AParameters.nFrom[0]]));
			//KG_PROCESS_ERROR(pfFovy);
			//float* pfAspect = (float*)(&(it->AParameters.pData[it->AParameters.nFrom[1]]));
			//KG_PROCESS_ERROR(pfAspect);
			//float* pfzNear = (float*)(&(it->AParameters.pData[it->AParameters.nFrom[2]]));
			//KG_PROCESS_ERROR(pfzNear);
			//float* pfzFar = (float*)(&(it->AParameters.pData[it->AParameters.nFrom[3]]));
			//KG_PROCESS_ERROR(pfzFar);
			//pCamera->SetGlobalPerspective(*pfFovy,*pfAspect,*pfzNear,*pfzFar);
		}
		break;
	case FT_KG3DCAMERA_SETGLOBALORTHOGONAL:
		{
			//float fWidth, float fHeight, float fzNear, float fzFar
			//KG_PROCESS_ERROR(m_mapMasters.find(it->dwID) != m_mapMasters.end());
			//IKG3DCamera* pCamera = static_cast<IKG3DCamera*>(m_mapMasters[it->dwID].pMaster);
			//KG_PROCESS_ERROR(pCamera);
			//float* pfWidth = (float*)(&(it->AParameters.pData[it->AParameters.nFrom[0]]));
			//KG_PROCESS_ERROR(pfWidth);
			//float* pfHeight = (float*)(&(it->AParameters.pData[it->AParameters.nFrom[1]]));
			//KG_PROCESS_ERROR(pfHeight);
			//float* pfzNear = (float*)(&(it->AParameters.pData[it->AParameters.nFrom[2]]));
			//KG_PROCESS_ERROR(pfzNear);
			//float* pfzFar = (float*)(&(it->AParameters.pData[it->AParameters.nFrom[3]]));
			//KG_PROCESS_ERROR(pfzFar);
			//pCamera->SetGlobalOrthogonal(*pfWidth,*pfHeight,*pfzNear,*pfzFar);
		}
		break;
	default:
		break;
	}
	hr = S_OK;
Exit0:
	return hr;
}
HRESULT RecPlayer::FrameMoveRePlay()
{
	if (m_PlayState == PLAY_STATE_PLAY)
	{

		DWORD dwNowTime = g_p3DTools->GetNowTime();//GetCurrentTime();
		DWORD dwStepTime = dwNowTime - m_dwRecPlayStartTime;
		list<EventRecord>::iterator it = m_CurrentEvent;
		while ( it != m_listEvents.end() && (it->dwTime - m_dwRecordStartTime <= dwStepTime * m_fPlaySpeed))
		{
			ProcessOneEvent(it);
			it++;
			m_dwCurrentEventNum++;
		}

		if (it == m_listEvents.end())
		{
			m_PlayState = PLAY_STATE_NONE;
		}
		m_CurrentEvent = it;
		//for camera
		if (m_CameraType == CAMERA_TYPE_BINDPLAYER && m_dwCameraPastToTime != 0)
		{
			DWORD dwNowCameraTime = g_p3DTools->GetNowTime();//GetCurrentTime();	
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
		if (m_pCurrentScene != NULL)
		{
			
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
	m_dwRecPlayStartTime = /*GetCurrentTime()*/ g_p3DTools->GetNowTime() - (m_CurrentEvent->dwTime - m_dwRecordStartTime);
	return S_OK;
}

HRESULT RecPlayer::StepToEvent(DWORD dwStepToEvent)
{
	HRESULT hr = E_FAIL;
	KG_PROCESS_ERROR(m_PlayState == PLAY_STATE_PLAY);

	if (dwStepToEvent > m_dwCurrentEventNum)
	{
		list<EventRecord>::iterator it = m_CurrentEvent;
		while ( it != m_listEvents.end() && (m_dwCurrentEventNum < dwStepToEvent))
		{
			ProcessOneEvent(it);
			it++;
			m_dwCurrentEventNum++;
		}
		m_CurrentEvent = it;
		AdjustStartTime();
	}
	else if (dwStepToEvent < m_dwCurrentEventNum)
	{
		KG_PROCESS_ERROR(dwStepToEvent > m_dwRecordStartLineTime);
		StopRePlay();
		RePlay();
		KG_PROCESS_ERROR(dwStepToEvent > m_dwCurrentEventNum);
		StepToEvent(dwStepToEvent);
	}
	hr = S_OK;
Exit0:
	return hr;
}
HRESULT RecPlayer::RePlay()
{
	if (m_PlayState == PLAY_STATE_NONE)
	{
		MastersUnit();
		m_PlayState = PLAY_STATE_PLAY;
		m_bIsBindIDCreated = FALSE;
		m_dwCurrentEventNum = 0;
		m_pCurrentScene = NULL;
		m_CurrentEvent = m_listEvents.begin();
		m_dwRecPlayStartTime = g_p3DTools->GetNowTime();//GetCurrentTime();

		m_dwRecordStartLineTime = 0;
		StepToEvent(m_dwRecordStartLineTime);
		return S_OK;
	}
	else if (m_PlayState == PLAY_STATE_PAUSE)
	{
		AdjustStartTime();
		m_PlayState = PLAY_STATE_PLAY;
		return S_OK;
	}
	return E_FAIL;
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
	if (m_PlayState == PLAY_STATE_PLAY)	
	{
		m_PlayState = PLAY_STATE_NONE;
		m_bIsBindIDCreated = FALSE;
		MastersUnit();
		m_dwCurrentEventNum = 0;
		m_pCurrentScene = NULL;
		return S_OK;
	}
	return E_FAIL;
}


HRESULT RecPlayer::StartMovieOut()
{
	HRESULT hr = E_FAIL;
	KG_PROCESS_ERROR(!m_bIsMovieing);
	IEKG3DSceneSceneEditor* pIEEditor = dynamic_cast<IEKG3DSceneSceneEditor*>(m_pCurrentScene);
	KG_PROCESS_ERROR(pIEEditor);
	hr = pIEEditor->BeginMovieREC(g_movieOption.Size, g_movieOption.fInterval, 
								  g_movieOption.FiterType,g_movieOption.ImageType,g_movieOption.nAutoScale);

	if (hr == S_OK)
	{
		m_bIsMovieing = TRUE;
		m_pEventMovieStart = &(*m_CurrentEvent);
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
		m_pEventMovieStop = &(*m_CurrentEvent);
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