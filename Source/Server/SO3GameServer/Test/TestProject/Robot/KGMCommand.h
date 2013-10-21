#ifndef _KGM_COMMAND_H_
#define _KGM_COMMAND_H_

class KMonster;

class KGMCommand
{
public:
    static int SetPosition(KMonster *pMonster, int nDestX, int nDestY, int nDestZ);
    static int AddExp(KMonster *pMonster, int nExp);
    static int LearnSkill(KMonster *pMonster, DWORD dwSkillID);
    static int AddItem(KMonster *pMonster, 	DWORD dwTabType, DWORD dwIndex, int nStackNum = 0);
};

#endif //_KGM_COMMAND_H_