#pragma once
#include "KG3DInterface.h"
#include "KG3DTypes.h"
#include "KGMemDateList.h"
#include <list>
#include <map>
using namespace std;

class RecPlayer : public IERecPlayer
{
	friend class KCameraOptionDialog;
public:
	
	enum MasterType{TYPEMODEL,TYPESCENEEDITOR,TYPECAMERA};
protected:
	//////For ListCtrl
	struct GlobleInfo
	{
		int nObjNum;
		int nEventNum;
	};
	GlobleInfo m_GlobleInfo;
protected:	
	
	struct Master
	{
		MasterType mtType;
		PVOID pMaster;
	};

	PlayState m_PlayState;
	map<DWORD,Master> m_mapMasters;
	list<EventRecord> m_listEvents;
	//map<list<EventRecord>::iterator,list<EventRecord>::iterator> m_mapSmoothCamera;

	map<DWORD,RecObj> m_mapObj;

	HWND m_hWnd;
	IKG3DScene* m_pCurrentScene;
	DWORD m_dwRecordStartTime;
	DWORD m_dwRecordStartLineTime;
	DWORD m_dwRecordStopLineTime;

	DWORD m_dwRecPlayStartTime;
	DWORD m_dwRecPlayPauseTime;
	list<EventRecord>::iterator m_CurrentEvent;
	BOOL m_bIsFreeCamera;
	IKG3DCamera* m_pGhostCameraForFree;
	DWORD m_dwCameraID;
	DWORD m_dwCurrentEventNum;
	FLOAT m_fPlaySpeed;
	HRESULT ProcessOneEvent(list<EventRecord>::iterator& it);
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

public:
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

	virtual HRESULT FreeCamera();
	virtual HRESULT UpDateFreeCamera(D3DXVECTOR3 vecDis,D3DXVECTOR2 pointMouseMove);

	virtual DWORD GetEventNum();
	virtual DWORD GetNowEventNum();

	virtual int GetObjNum();
	HRESULT StepToEvent(DWORD dwStepToEvent);

	HRESULT OnResizeWindow(PRECT const pRect);
	PlayState GetPlayarState();
	BOOL IsFreeCamera();
	HRESULT SaveIniRecordFile(string szFileName);
	HRESULT LoadIniRecordFile(string szFileName);

	HRESULT FillListTree(CTreeCtrl& ListTree);
	HRESULT FillEvent(HTREEITEM& hItem,CTreeCtrl& ListTree);
	HRESULT FillListCtrl(int nDeeps,DWORD_PTR pPoint,KGMemDateList& listctrlObj);

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

private:
	map<DWORD,RecObj>::iterator m_itCurrent;
};

IRecPlayer* g_GetNewRecPlayer();