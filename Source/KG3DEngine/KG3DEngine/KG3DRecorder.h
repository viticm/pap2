#pragma once

#include "../../../Include/KG3DEngine/KG3DTypes.h"

class KG3DRecorder
{
public:
	struct Parameters
	{
		static const int sMaxNum = 10;
		int nNum;
		int nFrom[sMaxNum];
		size_t size_tDataSize;
		char* pData; 
		Parameters();
	};

protected:
	enum MasterType{TYPEMODEL,TYPESCENEEDITOR,TYPECAMERA};
	enum RecordState{RECORD_STATE_REC,RECORD_STATE_NONE};

	struct EventRecord
	{
		DWORD dwID;
		FunctionType EventFunction;
		DWORD dwTime;
		DWORD dwFrame;
		Parameters AParameters;
	};

	struct Master
	{
		MasterType mtType;
		PVOID pMaster;
	};

	RecordState m_RecordState;
	map<DWORD,Master> m_mapMasters;
	list<EventRecord> m_listEvents;
	static DWORD sdwMasterID;
	DWORD m_dwRecordStartTime;
	DWORD m_dwRecordStartLineTime;
	DWORD m_dwRecordStopLineTime;
	BOOL m_bIsLock;
	BOOL m_bIsFreeCamera;
	TCHAR m_szSavePathName[MAX_PATH];
    BOOL  m_bIsRecordBegin;
    HRESULT RemoveItem(EventRecord* pEvent, BOOL &bNeedAdd);

public:
	KG3DRecorder(void);
	
	virtual HRESULT Init();
	virtual HRESULT UnInit();
	virtual HRESULT MastersUnit();
	
	virtual HRESULT StartRecordEvent(LPCTSTR szFolder,unsigned uMode);
	virtual HRESULT StopRecordEvent();
	virtual HRESULT SaveRecordFile(string szFileName);
	HRESULT SaveIniRecordFile(string szFileName);
	HRESULT GetCurrentSceneState();
	BOOL IsRecord();
	HRESULT Record();
	HRESULT StopRec();
	HRESULT RecordEvent(DWORD dwID,FunctionType EventFunction,DWORD dwTime, Parameters& AParameters);
	HRESULT DelByID(DWORD dwID);

	HRESULT Lock();
	HRESULT UnLock();
	BOOL IsLock();
	static DWORD GetMasterID();
	virtual ~KG3DRecorder(void);
};
extern KG3DRecorder g_cRecorder;