#ifndef _KPATROL_PATH_LIST_H_
#define _KPATROL_PATH_LIST_H_

#include <map>

class KPatrolPath;

class KPatrolPathList
{
public:
	KPatrolPathList(void);
	~KPatrolPathList(void);

	BOOL Init(void);
	BOOL UnInit(void);

	BOOL Load(DWORD dwMapID);
	BOOL Save(DWORD dwMapID, const char* szMapName);

	KPatrolPath* GetPatrolPath(DWORD dwMapID, DWORD dwPatrolPathID);
	int GetSize();

	KPatrolPath* AddPatrolPath(DWORD dwMapID, DWORD dwPatrolPathID, char* szTabFileName);

private:
	typedef std::map<int64_t, KPatrolPath> KPATROL_PATH_LIST;
	KPATROL_PATH_LIST m_mapPatrolPathList;
};
#endif	//_KPATROL_PATH_LIST_H_
