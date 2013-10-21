#pragma once
#include "KG3DInterface.h"
#include "KG3DTypes.h"
#include <list>
#include <map>
#include "IEOther.h"
#include "IEEditor.h"
#include "kg3dmodeltable.h"
#include "KG3DGraphicsTool.h"
#include "KG3DEngineManager.h"
#include "IEKG3DRecPlayer.h"

#define MAX_IDLIST_STRING_LENGTH 2500

using namespace std;

class KG3DRecPlayer;

class KG3DRecPlayerCommand
{
public:	
	KG3DRecPlayerCommand()
		:m_pRecPlayer(NULL)
	{

	}
	virtual HRESULT SetMaster(KG3DRecPlayer* pRecPlayer)
	{
		m_pRecPlayer = pRecPlayer;
		return S_OK;
	}
	virtual HRESULT LoadCommand(IIniFile* Ini,char* pSection){return S_OK;};
	virtual HRESULT ExcuteCommand(){return S_OK;};
	virtual ~KG3DRecPlayerCommand(){};
protected:
	KG3DRecPlayer* m_pRecPlayer;
};


class KG3DRecPlayer : public IERecPlayer
{
	friend class KG3DRecPlayerCommand;
	friend class KG3DSceneLoadFromFileCommand;
	friend class KG3DSceneRenderCommand;
	friend class KG3DSceneUnloadCommand;
	friend class KG3DActorLoadCommand;
	friend class KG3DActorUnloadCommand;
	friend class KG3DActorAddedToSceneCommand;
	friend class KG3DActorRemoveFromSceneCommand;
	friend class KG3DActorPlayAnimationCommand;
public:
	struct EventNode
	{
		KG3DRecPlayerCommand* m_pCommand;
		DWORD m_dwTime;
		DWORD m_dwID;
		EventNode()
		{
			m_pCommand = NULL;
			m_dwTime = 0;
			m_dwID = 0;
		}
		~EventNode()
		{
			SAFE_DELETE(m_pCommand);
		}
	};
	enum MasterType{TYPEMODEL,TYPESCENEEDITOR,TYPECAMERA,TYPESUBMODEL};
	enum PlayType{PLAYTYPEONCE,PLAYTYPELOOP};
public:
	virtual HRESULT Init(){return S_OK;};
	virtual HRESULT SaveRecordFile(LPCSTR szFileName){return S_OK;};
	virtual HRESULT UpDateFreeCamera(D3DXVECTOR3 vecDis, D3DXVECTOR2 pointMouseMove){return S_OK;};
	virtual BOOL IsMovieOut(){return TRUE;};
	virtual HRESULT StartMovieOut(){return S_OK;};
	virtual HRESULT StopMovieOut(){return S_OK;};

	virtual HRESULT FreeCamera(){return S_OK;};
	virtual BOOL IsFreeCamera(){return TRUE;};
	virtual HRESULT StepToEvent(DWORD dwStepToEvent){return S_OK;};

	virtual CameraType GetCameraType(){return CAMERA_TYPE_RECORD;};
	virtual void SetCameraType(CameraType Type){return;};

	virtual FLOAT GetAngle(){return 0;};
	virtual HRESULT SetAngle(FLOAT fAngle){return S_OK;};

	virtual DWORD GetBindID(){return 0;};
	virtual void SetBindID(DWORD dwID){return;};

	virtual PlayState GetPlayarState(){return PLAY_STATE_NONE;};

	virtual HRESULT GetFirstObj(DWORD *pdwID, RecObj** ppObj){return S_OK;};
	virtual HRESULT GetNextObj(DWORD *pdwID, RecObj** ppObj){return S_OK;};
	virtual HRESULT GetIDObj(DWORD dwID, RecObj** ppObj){return S_OK;};
	virtual int GetObjNum(){return 0;};

	virtual DWORD GetNowTime(){return 0;};
	virtual DWORD GetTotalTime(){return 0;};
	virtual HRESULT GetEventTime(EventRecord* pEvent, int& nTime){return S_OK;};

	virtual DWORD GetEventNum(){return 0;};
	virtual DWORD GetNowEventNum(){return 0;};

	KG3DRecPlayer();
	virtual ~KG3DRecPlayer();
	virtual HRESULT RePlay() ;
	virtual HRESULT PauseRePlay();
	virtual HRESULT StopRePlay();
	virtual HRESULT LoadRecordFile(LPCSTR szFileName);
	virtual HRESULT SetReplayWindow(HWND hWnd) ;
	virtual HRESULT FrameMoveRePlay();
	virtual BOOL    IsPlayFinished();
	virtual HRESULT GetLoadingProgress(float* pnLoadFilePer,float* pnStepToEventPer,int* pnLoadScenePer) ;
	virtual HRESULT OnResizeWindow(PRECT const pRect) ;

	virtual HRESULT STDMETHODCALLTYPE QueryInterface(const IID& iid, void** ppv);
	virtual ULONG   STDMETHODCALLTYPE AddRef();
	virtual ULONG   STDMETHODCALLTYPE Release();
	virtual BOOL    IsLoading() {return FALSE;};
	virtual BOOL    IsExiting() {return FALSE;};
private:
	KG3DRecPlayerCommand* CommandFactory(FunctionType eType);
	HRESULT UnInit();
	HRESULT MastersUnit();
	HRESULT EvnetListUnit();
	HRESULT AdjustStartTime();
	struct Master
	{
		MasterType mtType;
		PVOID pMaster;
	};
	PlayState m_PlayState;
	map<DWORD,Master> m_mapMasters;
	std::list<EventNode*> m_EventList;
	HWND m_hWnd;
	IKG3DScene* m_pCurrentScene;
	std::list<EventNode*>::iterator m_pCurrentEventIt;
	DWORD m_dwRecPlayStartTime;

	ULONG m_ulRefCount;
	DWORD m_dwLoopStartID;
	DWORD m_dwLoopEndID;
	PlayType m_PlayType;
	BOOL m_bCount;
	std::list<EventNode*>::iterator m_itLoopFrom;

};

class KG3DSceneLoadFromFileCommand : public KG3DRecPlayerCommand
{
public:
	virtual HRESULT LoadCommand(IIniFile* Ini,char* pSection)
	{
		HRESULT hr = E_FAIL;
		KG_PROCESS_ERROR(pSection);
		Ini->GetString(pSection,"Path","\0",m_cszFileName,MAX_PATH);
		Ini->GetInteger(pSection,"SceneID",0,(int*)(&m_dwSceneID));
		Ini->GetString(pSection,"AniPath","\0",m_cszAniName,MAX_PATH);
		hr = S_OK;
Exit0:
		return hr;
	}

	virtual HRESULT ExcuteCommand()
	{
		HRESULT hr = E_FAIL;
		KG3DSCENE_INITSTRUCT SceneInit;
		KG_PROCESS_ERROR(m_pRecPlayer->m_mapMasters.find(m_dwSceneID) == m_pRecPlayer->m_mapMasters.end());
		IKG3DScene* piScene = NULL;
		g_cEngineManager.NewOneScene(SCENETYPE_SCENEEDITOR, &piScene);
		KG_PROCESS_ERROR(piScene);
		piScene->LoadFromFileEx(m_cszFileName,0,0,(DWORD_PTR)&SceneInit,0);
		//((KG3DSceneSceneEditor *)piScene)->ForceLoadAllEntity();
		//m_pRecPlayer->m_pCurrentScene = piScene;
		KG3DRecPlayer::Master MyMaster;
		MyMaster.mtType = KG3DRecPlayer::TYPESCENEEDITOR;
		MyMaster.pMaster = (PVOID)(piScene);
		m_pRecPlayer->m_mapMasters[m_dwSceneID] = MyMaster;

		IEKG3DScene* pIEScene = static_cast<IEKG3DScene*>(piScene);
		pIEScene->LoadCameraAnimation(m_cszAniName);
		hr = S_OK;
Exit0:
		return hr;
	}
private:
	char m_cszFileName[MAX_PATH];
	DWORD m_dwSceneID;
	char m_cszAniName[MAX_PATH];
};


class KG3DSceneRenderCommand : public KG3DRecPlayerCommand
{
public:
	virtual HRESULT LoadCommand(IIniFile* Ini,char* pSection)
	{
		HRESULT hr = E_FAIL;
		KG_PROCESS_ERROR(pSection);
		Ini->GetString(pSection,"WindowName","\0",m_cszFileName,MAX_PATH);
		Ini->GetInteger(pSection,"RenderSceneID",0,(int*)(&m_dwSceneID));
		hr = S_OK;
Exit0:
		return hr;
	}

	virtual HRESULT ExcuteCommand()
	{
		HRESULT hr = E_FAIL;
		KG_PROCESS_ERROR(m_pRecPlayer->m_mapMasters.find(m_dwSceneID) != m_pRecPlayer->m_mapMasters.end());
		IKG3DScene* pSceneSceneEditor = static_cast<IKG3DScene*>(m_pRecPlayer->m_mapMasters[m_dwSceneID].pMaster);
		KG_PROCESS_ERROR(pSceneSceneEditor);
		int nWindowID = 0;
		if (m_pRecPlayer->m_hWnd != NULL)
		{
			pSceneSceneEditor->AddOutputWindow(m_cszFileName,m_pRecPlayer->m_hWnd,0,0,&nWindowID);
		}
		m_pRecPlayer->m_pCurrentScene = pSceneSceneEditor;
		IEKG3DScene* pIEScene = static_cast<IEKG3DScene*>(pSceneSceneEditor);
		pIEScene->StartPlayCamAni();
		hr = S_OK;
Exit0:
		return hr;
	}
private:
	char m_cszFileName[MAX_PATH];
	DWORD m_dwSceneID;
};

class KG3DSceneUnloadCommand : public KG3DRecPlayerCommand
{
public:
	virtual HRESULT LoadCommand(IIniFile* Ini,char* pSection)
	{
		HRESULT hr = E_FAIL;
		KG_PROCESS_ERROR(pSection);
		//Ini->GetString(pSection,"windowName","\0",m_cszFileName,MAX_PATH);
		Ini->GetInteger(pSection,"RemoveSceneID",0,(int*)(&m_dwSceneID));
		hr = S_OK;
Exit0:
		return hr;
	}

	virtual HRESULT ExcuteCommand()
	{
		HRESULT hr = E_FAIL;
		KG_PROCESS_ERROR(m_pRecPlayer->m_mapMasters.find(m_dwSceneID) != m_pRecPlayer->m_mapMasters.end());
		IKG3DScene* pSceneSceneEditor = static_cast<IKG3DScene*>(m_pRecPlayer->m_mapMasters[m_dwSceneID].pMaster);
		KG_PROCESS_ERROR(pSceneSceneEditor);
		IEKG3DScene* pIEScene = static_cast<IEKG3DScene*>(pSceneSceneEditor);
		pIEScene->StopPlayCamAni();
		if (m_pRecPlayer->m_pCurrentScene == pSceneSceneEditor)
			m_pRecPlayer->m_pCurrentScene = NULL;
		g_cEngineManager.RemoveScene(pSceneSceneEditor);
		m_pRecPlayer->m_mapMasters.erase(m_pRecPlayer->m_mapMasters.find(m_dwSceneID));	
		hr = S_OK;
Exit0:
		return hr;
	}
private:
	DWORD m_dwSceneID;
};

class KG3DActorLoadCommand : public KG3DRecPlayerCommand
{
public:
	const static int MAX_BINDSOCKETNAME = 40;
	struct BindInfo
	{
		char m_cszBindPath[MAX_PATH];
		char m_cszBindSocket[MAX_BINDSOCKETNAME];
	};
	virtual HRESULT LoadCommand(IIniFile* Ini,char* pSection)
	{
		m_listBindInfo.clear();
		int nBindNum = 0;
		char cszBindSection[MAX_PATH];
		char* pszIDList = NULL;

		HRESULT hr = E_FAIL;
		KG_PROCESS_ERROR(pSection);
		Ini->GetInteger(pSection,"ActorNum",1,&m_nActorNum);
		KG_PROCESS_ERROR(m_nActorNum > 0);
		//Ini->GetInteger(pSection,"ActorID",0,&m);
		m_pListID = new int[m_nActorNum];

		KG_PROCESS_ERROR(m_pListID);
		pszIDList = new char[MAX_IDLIST_STRING_LENGTH];
		KG_PROCESS_ERROR(pszIDList);

		//Ini->GetMultiInteger(pSection,"ActorID",m_pListID,m_nActorNum);
		Ini->GetString(pSection,"ActorID","\0",pszIDList,MAX_IDLIST_STRING_LENGTH);
		char* pStringHead = strtok(pszIDList,",");
		int i = 0;
		while (pStringHead && i < m_nActorNum)
		{
			m_pListID[i] = atoi(pStringHead);
			++i;
			pStringHead = strtok(NULL,",");
		}
		
		SAFE_DELETE_ARRAY(pszIDList);

		Ini->GetString(pSection,"ActorPath","\0",m_cszFileName,MAX_PATH);
		Ini->GetInteger(pSection,"BindNum",0,&nBindNum);
		for (int i = 0; i < nBindNum; i++)
		{
			BindInfo* pBindInfo = new BindInfo;
			sprintf(cszBindSection,"%s%d","BindPath",i);
			Ini->GetString(pSection,cszBindSection,"\0",pBindInfo->m_cszBindPath,MAX_PATH);
			sprintf(cszBindSection,"%s%d","BindSocket",i);
			Ini->GetString(pSection,cszBindSection,"\0",pBindInfo->m_cszBindSocket,MAX_BINDSOCKETNAME);
			m_listBindInfo.push_back(pBindInfo);
		}
		hr = S_OK;
Exit0:
		return hr;
	}
	virtual HRESULT ExcuteCommand()
	{
		HRESULT hr = E_FAIL;
		for (int i = 0; i < m_nActorNum;i++)
		{
			list<BindInfo*>::iterator it = m_listBindInfo.begin();
			KG_PROCESS_ERROR(m_pRecPlayer->m_mapMasters.find(m_pListID[i]) == m_pRecPlayer->m_mapMasters.end());
			IKG3DResourceBase* pModelST = NULL;
			g_cModelTable.LoadResourceFromFile(m_cszFileName,0,0,&pModelST);
			KG_PROCESS_ERROR(pModelST);
			KG3DRecPlayer::Master MyMaster;
			MyMaster.mtType = KG3DRecPlayer::TYPEMODEL;
			MyMaster.pMaster = (PVOID)(pModelST);
			m_pRecPlayer->m_mapMasters[m_pListID[i]] = MyMaster;
			IKG3DModel* pModel = static_cast<IKG3DModel*>(MyMaster.pMaster);
			KG_PROCESS_ERROR(pModel);
			while (it != m_listBindInfo.end())
			{
				IKG3DResourceBase* pModelBindResource = NULL;
				g_cModelTable.LoadResourceFromFile((*it)->m_cszBindPath,0,0,&pModelBindResource);
				KG_PROCESS_ERROR(pModelST);
				IKG3DModel* pModelBind = static_cast<IKG3DModel*>(pModelBindResource);
				KG_PROCESS_ERROR(pModelBind);
				pModelBind->BindToSocket(pModel,(*it)->m_cszBindSocket);
				it++;
			}
		}
		hr = S_OK;
Exit0:
		return hr;
	}

    virtual ~KG3DActorLoadCommand()
	{
		list<BindInfo*>::iterator it = m_listBindInfo.begin();
		while (it != m_listBindInfo.end())
		{
			SAFE_DELETE(*it);
			it++;
		}
		m_listBindInfo.clear();
		//delete[] m_pListID;
		SAFE_DELETE_ARRAY(m_pListID);
	}
private:
	char m_cszFileName[MAX_PATH];
	//DWORD m_dwModelID;
	int m_nActorNum;
	int* m_pListID;
	//vector<DWORD> m_vecModelID;
	list<BindInfo*> m_listBindInfo;

};

class KG3DActorUnloadCommand : public KG3DRecPlayerCommand
{
public:
	virtual HRESULT LoadCommand(IIniFile* Ini,char* pSection)
	{
		HRESULT hr = E_FAIL;
		char* pszIDList = NULL;
		Ini->GetInteger(pSection,"ActorNum",1,&m_nActorNum);
		KG_PROCESS_ERROR(m_nActorNum > 0);
		m_pListID = new int[m_nActorNum];
		KG_PROCESS_ERROR(m_pListID);
		pszIDList = new char[MAX_IDLIST_STRING_LENGTH];
		KG_PROCESS_ERROR(pszIDList);
		//Ini->GetMultiInteger(pSection,"ActorID",m_pListID,m_nActorNum);
		Ini->GetString(pSection,"ActorID","\0",pszIDList,MAX_IDLIST_STRING_LENGTH);
		char* pStringHead = strtok(pszIDList,",");
		int i = 0;
		while (pStringHead && i < m_nActorNum)
		{
			m_pListID[i] = atoi(pStringHead);
			++i;
			pStringHead = strtok(NULL,",");
		}
		SAFE_DELETE_ARRAY(pszIDList);
		hr = S_OK;
Exit0:
		return hr;
	}

	virtual HRESULT ExcuteCommand()
	{
		HRESULT hr = E_FAIL;
		for (int i = 0; i < m_nActorNum;i++)
		{
			KG_PROCESS_ERROR(m_pRecPlayer->m_mapMasters.find(m_pListID[i]) != m_pRecPlayer->m_mapMasters.end());
			IKG3DModel* pModel = static_cast<IKG3DModel*>(m_pRecPlayer->m_mapMasters[m_pListID[i]].pMaster);
			KG_PROCESS_ERROR(pModel);
			SAFE_RELEASE(pModel);
			m_pRecPlayer->m_mapMasters.erase(m_pRecPlayer->m_mapMasters.find(m_pListID[i]));	
		}
		hr = S_OK;
Exit0:
		return hr;
	}
	virtual ~KG3DActorUnloadCommand()
	{
		SAFE_DELETE_ARRAY(m_pListID);
	}
private:
	int m_nActorNum;
	int* m_pListID;
	//DWORD m_dwModelID;
};

class KG3DActorAddedToSceneCommand : public KG3DRecPlayerCommand
{
public:
	virtual HRESULT LoadCommand(IIniFile* Ini,char* pSection)
	{
		HRESULT hr = E_FAIL;
		char* pszIDList = NULL;
		KG_PROCESS_ERROR(pSection);
		Ini->GetInteger(pSection,"ActorNum",1,&m_nActorNum);
		KG_PROCESS_ERROR(m_nActorNum > 0);
		m_pListID = new int[m_nActorNum];
		KG_PROCESS_ERROR(m_pListID);
		pszIDList = new char[MAX_IDLIST_STRING_LENGTH];
		KG_PROCESS_ERROR(pszIDList);
		//Ini->GetMultiInteger(pSection,"ActorID",m_pListID,m_nActorNum);
		Ini->GetString(pSection,"ActorID","\0",pszIDList,MAX_IDLIST_STRING_LENGTH);
		char* pStringHead = strtok(pszIDList,",");
		int i = 0;
		while (pStringHead && i < m_nActorNum)
		{
			m_pListID[i] = atoi(pStringHead);
			++i;
			pStringHead = strtok(NULL,",");
		}
		SAFE_DELETE_ARRAY(pszIDList);

		Ini->GetInteger(pSection,"SceneID",0,(int*)(&m_dwSceneID));
		Ini->GetMultiFloat(pSection,"InitPos",(float*)&m_vecPos,3);
		hr = S_OK;
Exit0:
		return hr;
	}
	virtual HRESULT ExcuteCommand()
	{
		HRESULT hr = E_FAIL;
		KG_PROCESS_ERROR(m_pRecPlayer->m_mapMasters.find(m_dwSceneID) != m_pRecPlayer->m_mapMasters.end());
		IKG3DScene* pSceneSceneEditor = static_cast<IKG3DScene*>(m_pRecPlayer->m_mapMasters[m_dwSceneID].pMaster);
		KG_PROCESS_ERROR(pSceneSceneEditor);
		int nEL = (int)(sqrt((float)m_nActorNum));
		nEL++;
		for (int i = 0; i < m_nActorNum;i++)
		{
			D3DXVECTOR3 vecCulPos = m_vecPos;
			vecCulPos.z += i / nEL * 100;
			vecCulPos.x += i % nEL * 100;
			KG_PROCESS_ERROR(m_pRecPlayer->m_mapMasters.find(m_pListID[i]) != m_pRecPlayer->m_mapMasters.end());
			IKG3DModel* pModel = static_cast<IKG3DModel*>(m_pRecPlayer->m_mapMasters[m_pListID[i]].pMaster);
			KG_PROCESS_ERROR(pModel);
			pSceneSceneEditor->AddRenderEntity(pModel,0,TRUE,TRUE);
			pModel->SetTranslation(&vecCulPos);
		}
		hr = S_OK;
Exit0:
		return hr;
	}
	virtual ~KG3DActorAddedToSceneCommand()
	{
		SAFE_DELETE_ARRAY(m_pListID);
	}
private:
	DWORD m_dwSceneID;
	int m_nActorNum;
	int* m_pListID;
	D3DXVECTOR3 m_vecPos;
};

class KG3DActorRemoveFromSceneCommand : public KG3DRecPlayerCommand
{
public:
	virtual HRESULT LoadCommand(IIniFile* Ini,char* pSection)
	{
		HRESULT hr = E_FAIL;
		char* pszIDList = NULL;
		KG_PROCESS_ERROR(pSection);
		Ini->GetInteger(pSection,"ActorNum",1,&m_nActorNum);
		KG_PROCESS_ERROR(m_nActorNum > 0);
		m_pListID = new int[m_nActorNum];
		KG_PROCESS_ERROR(m_pListID);
		pszIDList = new char[MAX_IDLIST_STRING_LENGTH];
		KG_PROCESS_ERROR(pszIDList);
		//Ini->GetMultiInteger(pSection,"ActorID",m_pListID,m_nActorNum);
		Ini->GetString(pSection,"ActorID","\0",pszIDList,MAX_IDLIST_STRING_LENGTH);
		char* pStringHead = strtok(pszIDList,",");
		int i = 0;
		while (pStringHead && i < m_nActorNum)
		{
			m_pListID[i] = atoi(pStringHead);
			++i;
			pStringHead = strtok(NULL,",");
		}
		SAFE_DELETE_ARRAY(pszIDList);

		Ini->GetInteger(pSection,"SceneID",0,(int*)(&m_dwSceneID));
		hr = S_OK;
Exit0:
		return hr;
	}
	virtual HRESULT ExcuteCommand()
	{
		HRESULT hr = E_FAIL;
		KG_PROCESS_ERROR(m_pRecPlayer->m_mapMasters.find(m_dwSceneID) != m_pRecPlayer->m_mapMasters.end());
		IKG3DScene* pSceneSceneEditor = static_cast<IKG3DScene*>(m_pRecPlayer->m_mapMasters[m_dwSceneID].pMaster);
		KG_PROCESS_ERROR(pSceneSceneEditor);;
		for (int i = 0;i < m_nActorNum;i++)
		{
			KG_PROCESS_ERROR(m_pRecPlayer->m_mapMasters.find(m_pListID[i]) != m_pRecPlayer->m_mapMasters.end());
			IKG3DModel* pModel = static_cast<IKG3DModel*>(m_pRecPlayer->m_mapMasters[m_pListID[i]].pMaster);
			KG_PROCESS_ERROR(pModel);
			pSceneSceneEditor->RemoveRenderEntity(pModel,0,1);
		}
		hr = S_OK;
Exit0:
		return hr;
	}
	virtual ~KG3DActorRemoveFromSceneCommand()
	{
		SAFE_DELETE_ARRAY(m_pListID);
	}
private:
	DWORD m_dwSceneID;
	int m_nActorNum;
	int* m_pListID;
};

class KG3DActorPlayAnimationCommand : public KG3DRecPlayerCommand
{
public:
	virtual HRESULT LoadCommand(IIniFile* Ini,char* pSection)
	{		
		HRESULT hr = E_FAIL;
		char* pszIDList = NULL;
		KG_PROCESS_ERROR(pSection);
		Ini->GetInteger(pSection,"ActorNum",1,&m_nActorNum);
		KG_PROCESS_ERROR(m_nActorNum > 0);
		m_pListID = new int[m_nActorNum];
		KG_PROCESS_ERROR(m_pListID);
		pszIDList = new char[MAX_IDLIST_STRING_LENGTH];
		KG_PROCESS_ERROR(pszIDList);
		//Ini->GetMultiInteger(pSection,"ActorID",m_pListID,m_nActorNum);
		Ini->GetString(pSection,"ActorID","\0",pszIDList,MAX_IDLIST_STRING_LENGTH);
		char* pStringHead = strtok(pszIDList,",");
		int i = 0;
		while (pStringHead && i < m_nActorNum)
		{
			m_pListID[i] = atoi(pStringHead);
			++i;
			pStringHead = strtok(NULL,",");
		}
		SAFE_DELETE_ARRAY(pszIDList);

		Ini->GetString(pSection,"Path","\0",m_cszAnimationName,MAX_PATH);
		Ini->GetInteger(pSection,"PlayType",0,(int*)(&m_dwAnimationPlayType));
		Ini->GetInteger(pSection,"ControllerPriority",0,(int*)(&m_Priority));
		Ini->GetInteger(pSection,"OffsetTime",0,&m_nOffsetTime);
		Ini->GetFloat(pSection,"Speed",0,&m_fSpeed);
		Ini->GetInteger(pSection,"TweenIn",0,(int*)(&(m_TweenTime.dwTweenIn)));
		Ini->GetInteger(pSection,"TweenOut",0,(int*)(&(m_TweenTime.dwTweenOut)));
		Ini->GetFloat(pSection,"TweenWeigth",0,&(m_TweenTime.fTweenWeigth));
		hr = S_OK;
Exit0:
		return hr;
	}
	virtual HRESULT ExcuteCommand()
	{
		HRESULT hr = E_FAIL;
		for (int i = 0 ;i < m_nActorNum;i++)
		{
			KG_PROCESS_ERROR(m_pRecPlayer->m_mapMasters.find(m_pListID[i]) != m_pRecPlayer->m_mapMasters.end());
			IKG3DModel* pModel = static_cast<IKG3DModel*>(m_pRecPlayer->m_mapMasters[m_pListID[i]].pMaster);
			KG_PROCESS_ERROR(pModel);	
			pModel->PlayAnimation(m_dwAnimationPlayType,m_cszAnimationName,m_fSpeed/* *pfSpeed*/ ,m_nOffsetTime,(PVOID)(&m_TweenTime),NULL, m_Priority);
		}
		hr = S_OK;
Exit0:
		return hr;
	}

	virtual ~KG3DActorPlayAnimationCommand()
	{
		SAFE_DELETE_ARRAY(m_pListID);
	}
private:
	TweenTimeSpan m_TweenTime;
	int m_nActorNum;
	int* m_pListID;
	DWORD m_dwAnimationPlayType;
	char m_cszAnimationName[MAX_PATH];
	float m_fSpeed;
	int m_nOffsetTime;
	enuAnimationControllerPriority m_Priority;
};
