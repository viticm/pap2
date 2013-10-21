#include "StdAfx.h"
#include "KG3DRecorder.h"
#include "KG3DModelTable.h"
#include "KG3DSceneSceneEditor.h"
#include "KG3DModelST.h"
#include "KG3DSceneTable.h"
KG3DRecorder g_cRecorder;
DWORD KG3DRecorder::sdwMasterID = 1;

#ifdef _DEBUG
#define new DEBUG_NEW_3DENGINE
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

KG3DRecorder::Parameters::Parameters()
{
	nNum = 0;
	size_tDataSize = 0;
	pData = NULL; 
}

KG3DRecorder::KG3DRecorder(void)
{
	m_RecordState = RECORD_STATE_NONE;
	m_dwRecordStartTime = 0;
	m_bIsLock = FALSE;
	m_dwRecordStartLineTime = 0;
	m_dwRecordStopLineTime = 0;
	m_szSavePathName[0] = '\0' ;
    m_bIsRecordBegin = FALSE;
}


KG3DRecorder::~KG3DRecorder(void)
{
	UnInit();
}	

HRESULT KG3DRecorder::Lock()
{
	m_bIsLock = TRUE;
	return S_OK;
}
HRESULT KG3DRecorder::UnLock()
{
	m_bIsLock = FALSE;
	return S_OK;
}

BOOL KG3DRecorder::IsLock()
{
	return m_bIsLock;
}

BOOL KG3DRecorder::IsRecord()
{
	return m_RecordState == RECORD_STATE_REC;
}
HRESULT KG3DRecorder::Init()
{
	//Record();
	return S_OK;
}

HRESULT KG3DRecorder::MastersUnit()
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

HRESULT KG3DRecorder::UnInit()
{
	MastersUnit();
	list<EventRecord>::iterator itRecord;
	for (itRecord = m_listEvents.begin(); itRecord != m_listEvents.end(); ++itRecord)
	{
        KG_DELETE_ARRAY(itRecord->AParameters.pData);
	}
	m_listEvents.clear();
	return S_OK;
}

HRESULT KG3DRecorder::Record()
{
	UnInit();
	//GetCurrentSceneState();
	sdwMasterID = 1;
	m_dwRecordStartTime = timeGetTime();//GetCurrentTime();
	m_RecordState = RECORD_STATE_REC;
    m_bIsRecordBegin = FALSE;
	return S_OK;
}

HRESULT KG3DRecorder::StopRec()
{
	m_RecordState = RECORD_STATE_NONE;
	//SaveRecordFile()
	return S_OK;
}

HRESULT KG3DRecorder::RemoveItem(EventRecord* pEvent, BOOL &bNeedAdd)
{
    HRESULT hResult = E_FAIL;
    bNeedAdd = FALSE;

    KG_PROCESS_ERROR(pEvent);

    switch (pEvent->EventFunction)
    {
    case FT_KG3DSCENEEDITOR_DESTRUCT:
        {
            list<EventRecord>::iterator it;
            EventRecord eventRecord;
            list<DWORD> listCamera;
            listCamera.clear();

            for (it = m_listEvents.begin(); it != m_listEvents.end();)
            {
                eventRecord = *it;
                if (eventRecord.dwID == pEvent->dwID)
                {
                    if (eventRecord.EventFunction == FT_KG3DSCENEEDITOR_ADDOUTPUTWINDOW)
                    {
                        DWORD* pdwCameraID = (DWORD*)(&(eventRecord.AParameters.pData[eventRecord.AParameters.nFrom[1]]));
                        KG_PROCESS_ERROR(pdwCameraID);
                        listCamera.push_back(*pdwCameraID);
                    }
                    it = m_listEvents.erase(it);
                }
                else
                {
                    ++it;
                }
            }

            list<DWORD>::iterator itCamera;
            for (itCamera = listCamera.begin(); itCamera != listCamera.end(); ++itCamera)
            {
                DWORD dwCameraID = *itCamera;
                for (it = m_listEvents.begin(); it != m_listEvents.end();)
                {
                    eventRecord = *it;
                    if (eventRecord.dwID == dwCameraID)
                    {
                        it = m_listEvents.erase(it);
                    }
                    else
                    {
                        ++it;
                    }
                }
            }
        }    
        break;
    case FT_KG3DMODEL_DESTRUCT:
        {
            list<EventRecord>::iterator it;
            for (it = m_listEvents.begin(); it != m_listEvents.end();)
            {
                EventRecord eventRecord = *it;
                if (eventRecord.dwID == pEvent->dwID)
                {
                    it = m_listEvents.erase(it);
                }
                else
                {
                    ++it;
                }
            }
        }
        break;
    case FT_KG3DMODEL_BINDTOBONE:
    case FT_KG3DMODEL_BINDTOSOCKET:
    case FT_KG3DMODEL_ATTACHMODEL:
    case FT_KG3DMODEL_PLAYANIMATION:
    case FT_KG3DMODEL_PALYSPLITANIMATION:
    case FT_KG3DSCENEEDITOR_ADDRENDERENTITY:
        bNeedAdd = TRUE;
        break;
    case FT_KG3DSCENEEDITOR_REMOVERENDERENTITY:
        {
            list<EventRecord>::iterator it;
            DWORD* pdwEntityRemoveID = (DWORD*)(&(pEvent->AParameters.pData[pEvent->AParameters.nFrom[0]]));
            for (it = m_listEvents.begin(); it != m_listEvents.end(); ++it)
            {
                EventRecord eventRecord = *it;
                if (eventRecord.dwID == pEvent->dwID && 
                    eventRecord.EventFunction == FT_KG3DSCENEEDITOR_ADDRENDERENTITY)
                {
                    DWORD* pdwEntityAddID = (DWORD*)(&(eventRecord.AParameters.pData[eventRecord.AParameters.nFrom[0]]));
                    KG_PROCESS_ERROR(pdwEntityAddID);
                    if (*pdwEntityAddID == *pdwEntityRemoveID)
                    {
                        m_listEvents.erase(it);
                        break;
                    }
                }
            }
        }
        break;
    case FT_KG3DMODEL_DETCHMODEL:
        {
            list<EventRecord>::iterator it;
            DWORD* pdwDetchID = (DWORD*)(&(pEvent->AParameters.pData[pEvent->AParameters.nFrom[0]]));
            for (it = m_listEvents.begin(); it != m_listEvents.end(); ++it)
            {
                EventRecord eventRecord = *it;
                if (eventRecord.dwID == pEvent->dwID && 
                    eventRecord.EventFunction == FT_KG3DMODEL_ATTACHMODEL)
                {
                    DWORD* pdwAttachID = (DWORD*)(&(eventRecord.AParameters.pData[eventRecord.AParameters.nFrom[0]]));
                    if (*pdwAttachID == *pdwDetchID)
                    {
                        m_listEvents.erase(it);
                        break;
                    }
                }
            }
        }
        break;
    case FT_KG3DSCENEEDITOR_ADDOUTPUTWINDOW:
        {
            list<EventRecord>::iterator it;
            DWORD dwCameraID = 0;
            BOOL  bFind = FALSE;
            EventRecord eventRecord;

            for (it = m_listEvents.begin(); it != m_listEvents.end(); ++it)
            {
                eventRecord = *it;
                if (eventRecord.dwID == pEvent->dwID && 
                    eventRecord.EventFunction == pEvent->EventFunction)
                {
                    DWORD* pdwCameraID = (DWORD*)(&(eventRecord.AParameters.pData[eventRecord.AParameters.nFrom[1]]));
                    KG_PROCESS_ERROR(pdwCameraID);
                    dwCameraID = *pdwCameraID;
                    bFind = TRUE;
                    m_listEvents.erase(it);
                    break;
                }
            }
            if (bFind)
            {
                for (it = m_listEvents.begin(); it != m_listEvents.end();)
                {
                    eventRecord = *it;
                    if (eventRecord.dwID == dwCameraID)
                    {
                        it = m_listEvents.erase(it);
                    }   
                    else
                    {
                        ++it;
                    }
                }
            }
            bNeedAdd = TRUE;
        }
        break;
    default:
        {
            list<EventRecord>::iterator it;
            for (it = m_listEvents.begin(); it != m_listEvents.end(); ++it)
            {
                EventRecord eventRecord = *it;
                if (eventRecord.dwID == pEvent->dwID && 
                    eventRecord.EventFunction == pEvent->EventFunction)
                {
                    m_listEvents.erase(it);
                    break;
                }
            }
            bNeedAdd = TRUE;
        }
        break;
    }
    hResult = S_OK;
Exit0:
    return hResult;
}

HRESULT KG3DRecorder::RecordEvent(DWORD dwID,FunctionType EventFunction,DWORD dwTime, Parameters& AParameters)
{
    BOOL bNeedAdd = FALSE;
    EventRecord eventRecord;
    eventRecord.dwID = dwID;
    eventRecord.EventFunction = EventFunction;
    eventRecord.dwTime = timeGetTime();//dwTime;
    eventRecord.dwFrame = g_dwRenderCount;
    eventRecord.AParameters = AParameters;

    if (!m_bIsRecordBegin)
    {
        RemoveItem(&eventRecord, bNeedAdd);
    }
    if (m_bIsRecordBegin || bNeedAdd)
    {
        m_listEvents.push_back(eventRecord);
    }
	return S_OK;
}


HRESULT KG3DRecorder::SaveIniRecordFile(string szFileName)
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

HRESULT KG3DRecorder::SaveRecordFile(string szFileName)
{
	HRESULT hr = E_FAIL;
	unsigned long uSize = 0;

	list<EventRecord>::iterator itRecord = m_listEvents.begin();

	IFile* pFile = g_OpenFile(szFileName.c_str(),false,true);
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
		uSize = pFile->Write(&(itRecord->dwFrame),sizeof(DWORD));
		KG_PROCESS_ERROR(uSize == sizeof(DWORD));
		uSize = pFile->Write(&(itRecord->AParameters.size_tDataSize),sizeof(size_t));
		KG_PROCESS_ERROR(uSize == sizeof(size_t));
		uSize = pFile->Write(&(itRecord->AParameters.nNum),sizeof(int));
		KG_PROCESS_ERROR(uSize == sizeof(int));
		uSize = pFile->Write(itRecord->AParameters.nFrom,(itRecord->AParameters.nNum) * sizeof(int));
		KG_PROCESS_ERROR(uSize == (itRecord->AParameters.nNum) * sizeof(int));
		uSize = pFile->Write(itRecord->AParameters.pData, (unsigned long)itRecord->AParameters.size_tDataSize);
		KG_PROCESS_ERROR(uSize == itRecord->AParameters.size_tDataSize);
		itRecord++;
	}
	pFile->Close();
	hr = S_OK;

Exit0:
	KG_COM_RELEASE(pFile);
	return hr;
}

HRESULT  KG3DRecorder::DelByID(DWORD dwID)
{
	list<EventRecord>::iterator itRecord = m_listEvents.begin();
	while (itRecord != m_listEvents.end())
	{
		if (itRecord->dwID == dwID)
		{
			delete[](itRecord->AParameters.pData);
			itRecord = m_listEvents.erase(itRecord);
		}
		else
			itRecord++;
	}
	return S_OK;
}

HRESULT  KG3DRecorder::GetCurrentSceneState()
{
	return S_OK;
}

DWORD KG3DRecorder::GetMasterID()
{
	return sdwMasterID++;
}

HRESULT KG3DRecorder::StartRecordEvent(LPCTSTR szFolder,unsigned uMode)
{
	HRESULT hr = E_FAIL;
	if (IsRecord())
	{
        m_bIsRecordBegin = TRUE;
		m_dwRecordStartLineTime = (DWORD)m_listEvents.size();
		_tcsncpy(m_szSavePathName, szFolder, _countof(m_szSavePathName));
		hr = S_OK;
	}
	return hr;
}
HRESULT KG3DRecorder::StopRecordEvent()
{
	HRESULT hr = E_FAIL;
	if (IsRecord())
	{
		hr = g_cRecorder.SaveRecordFile(m_szSavePathName);
	}
	return hr;
}