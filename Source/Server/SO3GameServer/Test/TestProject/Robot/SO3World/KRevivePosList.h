#ifndef _KREVIVE_POS_LIST_H
#define _KREVIVE_POS_LIST_H

#include <map>

class KPlayer;

struct KREVIVE_POSITION
{
    int   nIndex;
    DWORD dwMapID; 
    int   nX;
    int   nY;
    int   nZ;
    int   nCamp;
};

class KRevivePosList
{
public:
    BOOL Init();
    BOOL Unint();

    KREVIVE_POSITION* GetRevivePos(int nIndex);

private:
    BOOL LoadRevivePosList();

public:
    typedef multimap<DWORD, KREVIVE_POSITION> REVIVE_POSITIONS;
    REVIVE_POSITIONS m_RevivePositions;
};

#endif