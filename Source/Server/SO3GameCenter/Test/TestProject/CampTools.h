#pragma once

class KGameServerSimulator;
struct  CampTools
{
	bool SetCamp(DWORD dwPlayerID, BYTE bCamp);
	
	int AddScore(int nScore);
	
	void GetLevelScore(int* pArray );

	int GetScore();
	int GetLevel();

	void SetScore(int nScore);
	void  SetLevel(int nLevel);
	KGameServerSimulator* m_pGS;
};
