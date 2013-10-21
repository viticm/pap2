// RecordeOperatorDoc.h : interface of the CRecordeOperatorDoc class
//


#pragma once
#include "IRecPlayer.h"
class IKG3DScene;
class CRecordeOperatorDoc : public CDocument
{
protected: // create from serialization only
	CRecordeOperatorDoc();
	DECLARE_DYNCREATE(CRecordeOperatorDoc)

// Attributes
public:

// Operations
public:

// Overrides
public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
	virtual BOOL OnOpenDocument(LPCTSTR lpszPathName);
	virtual void OnCloseDocument();
	HRESULT SetRecord(IERecPlayer* pRecord)
	{
		m_lpPlayer = pRecord;
		return S_OK;
	}
	IERecPlayer* GetRecord()
	{
		return m_lpPlayer;
	}

// Implementation
public:
	virtual ~CRecordeOperatorDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	IERecPlayer* m_lpPlayer;
// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()
};


