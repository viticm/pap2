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
//#include "IKG3DSoundShell.h"
#include "IEKG3DRecPlayer.h"

using namespace std;

class RecPlayer : public IERecPlayer
{
	friend class KCameraOptionDialog;
public:
	
	enum MasterType{TYPEMODEL,TYPESCENEEDITOR,TYPECAMERA,TYPESUBMODEL};
	enum PlayType{PLAYTYPE_BYFRAME,PLAYTYPE_BYTIME};
protected:	
	
    struct EventHead
    {
        DWORD dwFrameStartTime;
        DWORD dwFrameEndTime;
        DWORD dwFrameID;
        DWORD dwEventsCount;
        list<EventRecord*> ListEvent;
        EventHead()
        {
            dwFrameStartTime = 0;
            dwFrameEndTime   = 0;
            dwFrameID        = 0;
            dwEventsCount    = 0;

            ListEvent.clear();
        }
        ~EventHead()
        {
            ListEvent.clear();
        }

        void ListClear()
        {
            list<EventRecord*>::iterator it;
            EventRecord* pEvent = NULL;
            for (it = ListEvent.begin(); it != ListEvent.end(); ++it)
            {
                pEvent = *it;
                SAFE_DELETE(pEvent);
            }
        }
    };
    
    std::list<EventHead> m_ListEventHead;
    EventHead m_EventHead;
    DWORD m_dwCurrentEvents; // 记录当前到第几个事件

	struct Master
	{
		MasterType mtType;
		PVOID pMaster;
	};

	PlayState m_PlayState;
	map<DWORD,Master> m_mapMasters;
	EventRecord* m_pEvents;
    DWORD        m_dwEventsCount;
    EventRecord* m_pCurrentEvent;
    unsigned char* m_pbyRecordFileCache;
    struct RECORD_FILE_HEAD 
    {
        DWORD dwEventCount;
        DWORD dwRecordStartTime;
        DWORD dwRecordStartLineTime;
        DWORD dwRecordStopLineTime;
    };
    struct EVENT_RECORD_IN_FILE
    {
        DWORD        dwID;
        FunctionType EventFunction;
        DWORD        dwTime;
        DWORD        dwFrame;
        size_t       uParameters_DataSize;
        int          nParameters_Num;
    };
	//map<list<EventRecord>::iterator,list<EventRecord>::iterator> m_mapSmoothCamera;

	map<DWORD,RecObj> m_mapObj;

	HWND m_hWnd;
	IKG3DScene* m_pCurrentScene;
	DWORD m_dwRecordStartTime;
	DWORD m_dwRecordStartLineTime;
	DWORD m_dwRecordStopLineTime;

	DWORD m_dwRecPlayStartTime;
	DWORD m_dwRecPlayPauseTime;
	BOOL m_bIsFreeCamera;
	IKG3DCamera* m_pGhostCameraForFree;
	DWORD m_dwCameraID;
	FLOAT m_fPlaySpeed;
	HRESULT ProcessOneEvent(EventRecord *pEvent);
	HRESULT AdjustStartTime();

	DWORD m_dwBindID;
	BOOL m_bIsBindIDCreated;
	CameraType m_CameraType;
	FLOAT m_fAngle;

	DWORD m_dwCameraPastFromTime;
	DWORD m_dwCameraPastToTime;
	DWORD m_dwCameraNowFromTime;
	D3DXVECTOR3 m_vecFromPos;
	D3DXVECTOR3 m_vecToPos;

	BOOL m_bIsMovieing;
	//This maybe cause some disgust things.
	EventRecord* m_pEventMovieStart;
	EventRecord* m_pEventMovieStop;

	DWORD m_dwNowFrameCount;
	PlayType m_PlayType;

	IKG3DScene* m_pOldScene;
	int m_nOldWindowID;
	DWORD m_dwOldCameraID;

	//For smooth animation
	DWORD m_dwRecordLastFrameTime;
	DWORD m_dwNowLastFrameTime;
	float m_fSmoothSpeed;

	//For Progress
	float m_fLoadFilePer,m_fStepToEventPer;
	int m_nLoadScenePer;

    ULONG m_ulRefCount;
public:
    virtual HRESULT STDMETHODCALLTYPE QueryInterface(const IID& iid, void** ppv);
    virtual ULONG   STDMETHODCALLTYPE AddRef();
    virtual ULONG   STDMETHODCALLTYPE Release();

	RecPlayer(void);

	virtual HRESULT Init();
	virtual HRESULT UnInit();
	virtual HRESULT MastersUnit();

	virtual HRESULT SaveRecordFile(LPCSTR szFileName);
	virtual HRESULT LoadRecordFile(LPCSTR szFileName);

	virtual HRESULT SetReplayWindow(HWND hWnd);
	virtual HRESULT FrameMoveRePlay();
	virtual HRESULT RePlay();
	virtual HRESULT PauseRePlay();
	virtual HRESULT StopRePlay();
	virtual BOOL    IsPlayFinished();
	virtual HRESULT FreeCamera();
	virtual HRESULT UpDateFreeCamera(D3DXVECTOR3 vecDis,D3DXVECTOR2 pointMouseMove);

	virtual HRESULT GetLoadingProgress(float* pnLoadFilePer,float* pnStepToEventPer,int* pnLoadScenePer);
	HRESULT StepToEvent(DWORD dwStepToEvent);

	HRESULT OnResizeWindow(PRECT const pRect);
	PlayState GetPlayarState();
	BOOL IsFreeCamera();

	HRESULT StartMovieOut();
	HRESULT StopMovieOut();
	BOOL IsMovieOut();

	virtual HRESULT SetAngle(FLOAT fAngle);
	virtual CameraType GetCameraType();
	virtual void SetCameraType(CameraType Type);
	virtual FLOAT GetAngle();
	
	virtual DWORD GetBindID();
	virtual void SetBindID(DWORD dwID);
	
	virtual ~RecPlayer(void);
	virtual HRESULT GetFirstObj(DWORD *pdwID, RecObj** ppObj);
	virtual HRESULT GetNextObj(DWORD *pdwID, RecObj** ppObj);
    virtual HRESULT GetIDObj(DWORD dwID, RecObj** ppObj);
    virtual int GetObjNum();
	
	HRESULT CameraSmoothSave(LPCSTR szFileName);
    virtual DWORD GetNowTime();
    virtual DWORD GetTotalTime();
    virtual HRESULT GetEventTime(EventRecord* pEvent, int& nTime);
	virtual BOOL    IsLoading() {return FALSE;}
	virtual BOOL    IsExiting() {return FALSE;}

public:
    virtual HRESULT Play();
private:
    BOOL m_bChanged;
    list<EventHead>::iterator    m_CurrentOutIt;
    list<EventRecord*>::iterator m_CurrentIter;
    EventRecord* m_pStartLineEvent;
    EventRecord* m_pStopLineEvent;

    void DeleteHead();

private:
	map<DWORD,RecObj>::iterator m_itCurrent;

public:
    virtual DWORD GetEventNum();
    virtual DWORD GetNowEventNum();
};

IRecPlayer* g_GetNewRecPlayer();