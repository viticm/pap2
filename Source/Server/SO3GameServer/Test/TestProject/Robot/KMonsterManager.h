#ifndef _KMONSTER_MANAGER_H_
#define _KMONSTER_MANAGER_H_

#include <list>
#include "KMonster.h"

#define ROLE_INDEX_FILE "index.dat"

class KMonsterManager
{
public:
    KMonsterManager(void);
    ~KMonsterManager(void);

    int Init();
    int UnInit();

    int CreateMonster(int nCount);
    int DestroyMonster(int nCount);
    int DestroyAllMonster();

public:
    int QueryAllMonsterState();
    int StateStatistic();

private:
    int ReadRoleIndex();
    int SaveRoleIndex(int nIndex);

private:    
    typedef std::list<KMonster *> MONSTER_LIST;
    MONSTER_LIST m_MonsterList;
    int m_nMaxIndex;
};

extern KMonsterManager g_MonsterManager;

#endif //_KMONSTER_MANAGER_H_