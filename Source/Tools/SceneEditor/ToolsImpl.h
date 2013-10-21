#pragma once

#include "../EditInterface/IETools.h"

class KConnectionPoint : public IEKG3DConnectionPoint
{
public:
	KConnectionPoint(){}
	~KConnectionPoint(){}

	virtual HRESULT Advise(IEKG3DSimpleSink* pNewSink)
	{
		_ASSERTE(pNewSink);
		if (pNewSink)
		{
			m_listSinks.push_back(pNewSink);
		}
		return S_OK;
	}
	
	virtual HRESULT Unadvise(IEKG3DSimpleSink* const pNewSink)
	{
		_ASSERTE(pNewSink);
		if (pNewSink)
		{
			m_listSinks.remove(pNewSink);
		}
		return S_OK;
	}
protected:
	virtual void FireEvent(INT nSinkID,INT nsubSinkID,DWORD dwValue,PVOID pPoint)
	{
		list<IEKG3DSimpleSink*>::iterator it;

		for(it = m_listSinks.begin(); it != m_listSinks.end(); it ++)
		{
			(*it)->SinkFunc(nSinkID,nsubSinkID,dwValue,pPoint);
		}
	}
private:
	list<IEKG3DSimpleSink*> m_listSinks;
};