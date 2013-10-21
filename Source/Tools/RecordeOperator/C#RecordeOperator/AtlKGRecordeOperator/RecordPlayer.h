// RecordPlayer.h : Declaration of the CRecordPlayer

#pragma once
#include "AtlKGRecordeOperator.h"
#include "resource.h"       // main symbols
#include "Type.h"
#include <comsvcs.h>
#include <map>
#include "IEKG3DRecPlayer.h"
// CRecordPlayer

class ATL_NO_VTABLE CRecordPlayer :
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CRecordPlayer, &CLSID_RecordPlayer>,
	public IRecordPlayer
{
public:
	CRecordPlayer()
	{
        m_piRecord = NULL;
        m_dwEventsCount = 0;
        m_nStepValue = -1;
        m_pRecObj = NULL;
        m_pCurrentEvent = NULL;
        m_mapStringType.clear();
	}

    ~CRecordPlayer()
    {
        if (m_piRecord)
        {
            delete m_piRecord;
            m_piRecord = NULL;
        }
        m_dwEventsCount = 0;
        m_nStepValue = -1;
    }

	DECLARE_PROTECT_FINAL_CONSTRUCT()

	HRESULT FinalConstruct()
	{
		return S_OK;
	}

	void FinalRelease()
	{
	}

DECLARE_REGISTRY_RESOURCEID(IDR_RECORDPLAYER)

DECLARE_NOT_AGGREGATABLE(CRecordPlayer)

BEGIN_COM_MAP(CRecordPlayer)
	COM_INTERFACE_ENTRY(IRecordPlayer)
END_COM_MAP()

private:
    IERecPlayer* m_piRecord;
    DWORD m_dwEventsCount;
    int   m_nStepValue;
    RecObj* m_pRecObj;
    EventRecord* m_pCurrentEvent;
    std::map<string, FunctionType> m_mapStringType;

    int SetRecord(IERecPlayer* piRecord)
    {
        m_piRecord = piRecord;
        return S_OK;
    };

    HRESULT GetDesByID(string& strDes, FunctionType Ftype);
    HRESULT GetFuncByDes(string strDes,FunctionType &Ftype);
    HRESULT InitMap();
// IRecordPlayer
public:
    STDMETHOD(RePlay)(int* pRetResult);
    STDMETHOD(PauseRePlay)(int* pRetResult);
    STDMETHOD(StopRePlay)(void);
    STDMETHOD(LoadRecordFile)(BSTR pstrFileName);
    STDMETHOD(SetReplayWindow)(ULONG uhWnd);
    STDMETHOD(FrameMoveRePlay)(void);
    STDMETHOD(IsPlayFinished)(int* pbFinished);
    STDMETHOD(GetLoadingProgress)(FLOAT* pnLoadFilePer, FLOAT* pnStepToEventPer, INT* pnLoadScenePer);
    STDMETHOD(SaveRecordFile)(BSTR pstrFileName);
    STDMETHOD(SetOption)(int nWidth, int nHeight);
    STDMETHOD(FreeCamera)(void);
    STDMETHOD(IsFreeCamera)(int* pnRetResult);
    STDMETHOD(GetCameraType)(void);
    STDMETHOD(GetObjSize)(int* pnSize);
    STDMETHOD(GetFirstObj)(int* pnID, BSTR* pstrRetName, int* pnType);
    STDMETHOD(GetNextObj)(int* pnID, BSTR* pstrRetName, int* pnType);
    STDMETHOD(InitTree)(void);
    STDMETHOD(GetFirstEvent)(BSTR* pnFunction, int* pnInfo);
    STDMETHOD(GetNextEvent)(BSTR* pnFunction, int* pnInfo);
    STDMETHOD(GetInformation)(int pnInfo, BSTR* pstrLabel, BSTR* pstrTextBox, int* pnNum);
    STDMETHOD(AddEvent)(int nNum, BSTR strName, BSTR strValue);
    STDMETHOD(RemoveEvent)(int nInfo);
    STDMETHOD(GetNowTime)(int* pnNowTime);
    STDMETHOD(GetTotalTime)(int* pnTotalTime);
    STDMETHOD(GetPlayState)(int* pPlayState);
    STDMETHOD(GetObjInfo)(int nID, int* pnSuccess);
    STDMETHOD(GetFirstEventTime)(int* pnTime, int* pnInfo);
    STDMETHOD(GetNextEventTime)(int* pnTime, int* pnInfo);
    STDMETHOD(GetEventFunction)(int pnInfo, BSTR* pstrFunction);
};

OBJECT_ENTRY_AUTO(__uuidof(RecordPlayer), CRecordPlayer)
