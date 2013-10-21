#ifndef _KNPC_TEAM_LIST_H_
#define _KNPC_TEAM_LIST_H_

#include <map>
#include <vector>
#include "KNpcTeam.h"

struct KNpcTeamTemplate
{
	DWORD dwID;
	DWORD dwMapID;
	int nOrder;
};

class KNpcTeamList
{
public:
	KNpcTeamList(void); 
	~KNpcTeamList(void);

	BOOL Init();
	BOOL UnInit();

	BOOL Load(DWORD dwMapID);
	BOOL Save(DWORD dwMapID, const char* szMapName);

	//KNpcTeam* GetNpcTeam(DWORD dwMapID, DWORD dwIndex);
	//Ä£°åº¯Êý
	inline int GetSize();
	BOOL AddNpcTeam(DWORD dwMapId, DWORD dwTeamID, DWORD dwTeamOrder);
	KNpcTeamTemplate* GetNpcTeamTemplate(DWORD dwMapID, DWORD dwTeamID);
	BOOL GetNpcTeamTemplate(DWORD dwMapID, OUT std::vector<KNpcTeamTemplate>& vecNpcTeamArray);

private:
	typedef std::map<int64_t, KNpcTeamTemplate> KNPC_TEAM_TEMPLATE_LIST;
	KNPC_TEAM_TEMPLATE_LIST m_mapNpcTeamList;
};

inline int KNpcTeamList::GetSize()
{
	return (int)m_mapNpcTeamList.size();
}

#endif //_KNPC_TEAM_LIST_H_
