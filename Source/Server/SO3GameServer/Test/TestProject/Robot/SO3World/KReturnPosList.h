#ifndef _KRETURN_POS_LIST_H_
#define _KRETURN_POS_LIST_H_

#define MAX_RETURN_POS	1024

struct KReturnPos
{
	DWORD	dwMapID;
	int		nX;
	int		nY;
	int		nZ;
};

class KReturnPosList
{
public:
	KReturnPosList(void);
	~KReturnPosList(void);

	BOOL Init(void);

	KReturnPos* GetReturnPos(int nIndex);

private:
	KReturnPos m_ReturnPosList[MAX_RETURN_POS];
};

#endif	//_KRETURN_POS_LIST_H_