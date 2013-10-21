#ifndef CASE_ACTION_OBJECT_KRLREMOTECHARACTERMGR_H
#define CASE_ACTION_OBJECT_KRLREMOTECHARACTERMGR_H

class KRLRemoteCharacter;

class KRLRemoteCharacterMgr
{
public:
    typedef stdext::hash_map<DWORD, KRLRemoteCharacter*> KPtrMap;

public:
    KRLRemoteCharacterMgr();
    ~KRLRemoteCharacterMgr();

	BOOL Init();
	void Exit();
	BOOL Reset();
	BOOL Activate(double fTime, double fTimeLast, DWORD dwGameLoop, BOOL bFrame);

    HRESULT Append(DWORD dwCharacter, KRLRemoteCharacter* pRLRemoteCharacter);
    HRESULT Remove(DWORD dwCharacter);
    KRLRemoteCharacter* Lookup(DWORD dwCharacter);

public:
    KPtrMap	m_aCharacter;
};

#endif // CASE_ACTION_OBJECT_KRLREMOTECHARACTERMGR_H