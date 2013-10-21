////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : IEKG3DRecPlayer.h
//  Version     : 1.0
//  Creator     : Shen Liqiao
//  Create Date : 2009-5-28 9:47:37
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _INCLUDE_IEKG3DRECPLAYER_H_
#define _INCLUDE_IEKG3DRECPLAYER_H_
#include "KG3DTypes.h"
////////////////////////////////////////////////////////////////////////////////
struct IERecPlayer : public IRecPlayer
{
	enum CameraType
	{
		CAMERA_TYPE_RECORD,
		CAMERA_TYPE_BIND,
		CAMERA_TYPE_BINDPLAYER,
		CAMERA_TYPE_COUNT,
		CAMERA_TYPE_FORCE_DWORD = 0xffffffff,
	};

	enum PlayState
	{
		PLAY_STATE_PLAY,
		PLAY_STATE_NONE,
		PLAY_STATE_PAUSE
	};

	virtual HRESULT Init() = 0;
	virtual HRESULT SaveRecordFile(LPCSTR szFileName) = 0;
	virtual HRESULT UpDateFreeCamera(D3DXVECTOR3 vecDis, D3DXVECTOR2 pointMouseMove) = 0;
	virtual HRESULT FrameMoveRePlay() = 0;
	virtual BOOL IsMovieOut() = 0;
	virtual HRESULT StartMovieOut() = 0;
	virtual HRESULT StopMovieOut() = 0;

	virtual HRESULT FreeCamera() = 0;
	virtual BOOL IsFreeCamera() = 0;
	virtual HRESULT StepToEvent(DWORD dwStepToEvent) = 0;

	virtual CameraType GetCameraType() = 0;
	virtual void SetCameraType(CameraType Type) = 0;

	virtual FLOAT GetAngle() = 0;
	virtual HRESULT SetAngle(FLOAT fAngle) = 0;

	virtual DWORD GetBindID() = 0;
	virtual void SetBindID(DWORD dwID) = 0;

	virtual PlayState GetPlayarState() = 0;

	virtual HRESULT GetFirstObj(DWORD *pdwID, RecObj** ppObj) = 0;
	virtual HRESULT GetNextObj(DWORD *pdwID, RecObj** ppObj) = 0;
	virtual HRESULT GetIDObj(DWORD dwID, RecObj** ppObj) = 0;
	virtual int GetObjNum() = 0;

	virtual DWORD GetNowTime() = 0;
	virtual DWORD GetTotalTime() = 0;
	virtual HRESULT GetEventTime(EventRecord* pEvent, int& nTime) = 0;
	virtual ~IERecPlayer(){}

	virtual DWORD GetEventNum() = 0;
	virtual DWORD GetNowEventNum() = 0;
};


#endif //_INCLUDE_IEKG3DRECPLAYER_H_
