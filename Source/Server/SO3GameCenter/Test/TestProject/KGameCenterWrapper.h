#pragma once
#include "Engine//KThread.h"
#include "..\..\KSO3GameCenterSettings.h"
#include <vector>
#if 1
#define  private public
#include "KSO3GameCenter.h"
#endif

#define  GC KGameCenterWrapper::Instance()
#define  GCSETTING KGameCenterWrapper::GCSettings()

class KGameCenterWrapper
{
public:
	~KGameCenterWrapper(void);
	static KGameCenterWrapper* Instance();
	bool Start();
	void Stop();
	int GetRolePartyID(DWORD dwRoleId);
	int GetPartyLeaderID(DWORD dwPartyID);
	static void GameCenterRunner(void* pSelf);
	static KSO3GameCenterSettings* GCSettings();
	void AccelerationMultipleGC(int nMultip);
	void AccelerationGC(int nGameLoop);
	void AccelerationGCTime(time_t uTime);
	std::vector<DWORD> GetHometownMapTable();
	bool AddInBlackList(DWORD dwID, time_t BlackTime);
	KBattleFieldManager::KBASE_INFO_TABLE& GetBattleList();
	bool CheckPermission(DWORD dwRoleID, DWORD dwMapID, int nCopyIndex);
	int GetBattleFieldSide(DWORD dwMapID, int nCopyIndex, DWORD dwRoleID);
	int CreateBattleField(DWORD dwMapID);
	void ManualActivate();
	KGPQ* GetPQ(DWORD dwPQ);
	void SaveFellowShipData(DWORD dwRoleID);
	bool GCRun();	
	KROLE_STATE RoleSate(DWORD dwRoleID);
	bool CanCreateMap(DWORD& dwMapID, int& nMapCopyIndex);
	KMutex* Mutex() {return &m_Lock;};
	KSO3GameCenter* Center(){return m_pGC;};
	bool CallScript(const char* pszScript);
	void AccelerationRemainderGC(int nMultip, int Remainder);
private:
	void GCActivate();
	KBattleFieldManager::KBASE_INFO_TABLE m_BattleList;
	KGameCenterWrapper(void);
	
	KThread m_Thread;
	 bool m_bRun;
	 KMutex m_Lock;
	bool m_GCInit;
	KSO3GameCenter* m_pGC;
	time_t m_timeAccelera;
};
