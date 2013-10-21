#ifndef CASE_ACTION_OBJECT_KRLSCENE_MGR_H
#define CASE_ACTION_OBJECT_KRLSCENE_MGR_H

class KRLScene;
class IKG3DRegionInfoManager;

typedef std::vector<KRLScene*>	KRLScenePtrArray;

class KRLSceneMgr
{
private:
	typedef std::map<DWORD, IKG3DRegionInfoManager*> K3DRegionMgrMap;

public:
	KRLSceneMgr();
	~KRLSceneMgr();

	BOOL Init();
	void Exit();
	BOOL Reset();
	BOOL Activate(double fTime, double fTimeLast, DWORD dwGameLoop, BOOL bFrame);

	HRESULT Append(DWORD dwScene, KRLScene* pRLScene);
	HRESULT Remove(DWORD dwScene);
	KRLScene* Lookup(DWORD dwScene);

	DWORD GetRegionData(DWORD dwScene, D3DXVECTOR3 vPosition);
	DWORD GetRegionDataByMapID(DWORD dwMapID, D3DXVECTOR3 vPosition);

public:
	DWORD m_dwClientPlayerRegionInfo;
    D3DXVECTOR3 m_vClientPlayerRegionPosition;

	KRLScenePtrArray m_apScene;
	K3DRegionMgrMap	m_a3DRegionMgr;
};

#endif // CASE_ACTION_OBJECT_KRLSCENE_MGR_H