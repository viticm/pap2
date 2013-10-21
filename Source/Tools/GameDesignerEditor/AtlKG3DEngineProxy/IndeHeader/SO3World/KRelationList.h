#ifndef _KRELATION_LIST_H_
#define _KRELATION_LIST_H_

#define RELATION_CAMP_FILE_NAME		"RelationCamp.tab"
#define RELATION_FORCE_FILE_NAME	"RelationForce.tab"
#define RELATION_ID					"ID"

class KRelationList
{
private:
	int		m_nSize;
	int**	m_nRelation;

public:
	KRelationList();
	virtual ~KRelationList();

	BOOL	Init(const char* szSettingFile);
	int		GetRelation(int nA, int nB);
	int		GetSize() { return m_nSize; }
};
#endif //_KRELATION_LIST_H_
