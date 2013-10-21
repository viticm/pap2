#ifndef CASE_ACTION_OBJECT_KRLRIDESMGR_H
#define CASE_ACTION_OBJECT_KRLRIDESMGR_H

class KRLRides;

class KRLRidesMgr
{
private:
	typedef stdext::hash_map<DWORD, KRLRides*> KPtrMap;

public:
	KRLRidesMgr();
	~KRLRidesMgr();

	BOOL Init();
	void Exit();
	BOOL Reset();
	BOOL Activate(double fTime, double fTimeLast, DWORD dwGameLoop, BOOL bFrame);

    HRESULT Append(DWORD dwRides, KRLRides* pRLRides);
	HRESULT Remove(DWORD dwRides);
	KRLRides* Lookup(DWORD dwRides);

private:
	KPtrMap	m_aRides;
};

#endif // CASE_ACTION_OBJECT_KRLRIDESMGR_H