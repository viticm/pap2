#ifndef _KFIGHT_FLAG_LIST_H_
#define _KFIGHT_FLAG_LIST_H_

#define DEFAULT_FLAG	"DefaultFlag"
#define DEFAULT_REPUTE	"DefaultRepute"
#define DEFAULT_LEVEL	"DefaultLevel"
#define	FLAG_IS_LOCKED	"IsLocked"

typedef struct tagForceDefault
{
	BOOL		bDefaultFlag;
	BOOL		bLockedFlag;
	int			nLevel;
	int			nRepute;
} FORCE_DEFAULT;

class KFightFlagList
{
private:
	int				m_nSize;
	FORCE_DEFAULT*	m_ForceDefault;
public:
	KFightFlagList();
	virtual ~KFightFlagList();
	BOOL			Init();
	int				GetSize() { return m_nSize; }
	FORCE_DEFAULT*	GetDefaultRepute(DWORD dwForceID);
};
#endif
