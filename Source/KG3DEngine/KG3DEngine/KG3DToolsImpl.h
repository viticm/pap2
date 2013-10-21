#pragma once

#include "ietools.h"
#include "Engine/KMutex.h"

//这个相当于IConnectionPoint接口
template<class T>
class CKG3DConnectionPoint : public T
{
public:
	CKG3DConnectionPoint(){}
	~CKG3DConnectionPoint(){}

	virtual HRESULT UnadviseAll()
	{
		m_listSinks.clear();
		return S_OK;
	}
	virtual HRESULT Advise(IEKG3DSimpleSink* pNewSink)
	{
		m_lock.Lock();
		_ASSERTE(pNewSink);
		if (pNewSink)
		{
			m_listSinks.push_back(pNewSink);
		}
		m_lock.Unlock();
		return S_OK;
	}
	
	virtual HRESULT Unadvise(IEKG3DSimpleSink* const pNewSink)
	{
		m_lock.Lock();
		_ASSERTE(pNewSink);
		if (pNewSink)
		{
			m_listSinks.remove(pNewSink);
		}
		m_lock.Unlock();
		return S_OK;
	}
protected:
	virtual void FireEvent(INT nSinkID)
	{
		m_lock.Lock();
		list<IEKG3DSimpleSink*>::iterator it;

		for(it = m_listSinks.begin(); it != m_listSinks.end(); it ++)
		{
			IEKG3DSimpleSink* pSS = (*it);
			if(pSS)
				pSS->SinkFunc(nSinkID);
		}
		m_lock.Unlock();
	}
	virtual void FireEvent2(INT nSinkID,INT nsubSinkID,DWORD dwValue,PVOID pPoint)
	{
		m_lock.Lock();
		list<IEKG3DSimpleSink*>::iterator it;

		for(it = m_listSinks.begin(); it != m_listSinks.end(); it ++)
		{
			(*it)->SinkFunc2(nSinkID,nsubSinkID,dwValue,pPoint);
		}
		m_lock.Unlock();
	}
	virtual UINT	GetSinksCount()
	{
		return static_cast<UINT>(m_listSinks.size());
	}	//使用的函数可以根据这个，方便的跳过发消息的步骤
private:
	list<IEKG3DSimpleSink*> m_listSinks;
	KMutex m_lock;
};