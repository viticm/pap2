#ifndef CASE_ACTION_OBJECT_KRLDOODADMGR_H
#define CASE_ACTION_OBJECT_KRLDOODADMGR_H

class KRLDoodad;

class KRLDoodadMgr
{
public:
	typedef stdext::hash_map<DWORD, KRLDoodad*>	KPtrMap;

public:
	KRLDoodadMgr();
	~KRLDoodadMgr();

	BOOL Init();
	void Exit();
	BOOL Reset();
	BOOL Activate(double fTime, double fTimeLast, DWORD dwGameLoop, BOOL bFrame);

	HRESULT Append(DWORD dwDoodad, KRLDoodad* pRLDoodad);
	HRESULT Remove(DWORD dwDoodad);
	KRLDoodad* Lookup(DWORD dwDoodad);

public:
	KPtrMap	m_aDoodad;
};

#endif // CASE_ACTION_OBJECT_KRLDOODADMGR_H