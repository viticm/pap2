// Operation.h: interface for the KEditOperation class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_OPERATION_H__95BA3DCA_1B41_47D9_8A38_3FCF1CD41CB4__INCLUDED_)
#define AFX_OPERATION_H__95BA3DCA_1B41_47D9_8A38_3FCF1CD41CB4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

typedef class KEditOperation
{
public:
	string  m_scName;

public:
	virtual HRESULT Undo();
	virtual HRESULT Do();
	
	KEditOperation();
	virtual ~KEditOperation();

}* LPOPERATION;

class KEditOperationHistory
{
public:
	vector<LPOPERATION>m_vecOperation;

public:
	HRESULT EmptyHistory()
	{
		for(DWORD i=0;i<m_vecOperation.size();i++)
		{
			LPOPERATION pOp = m_vecOperation[i];
			SAFE_DELETE(pOp);
		}
		m_vecOperation.clear();
		return S_OK;
	}
	KEditOperationHistory();
	virtual ~KEditOperationHistory();

};

#endif // !defined(AFX_OPERATION_H__95BA3DCA_1B41_47D9_8A38_3FCF1CD41CB4__INCLUDED_)
