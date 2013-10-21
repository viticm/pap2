#ifndef _KCAMPMANAGER_H_
#define _KCAMPMANAGER_H_

#include "SO3GlobalDef.h"

#define CAMP_SCORE_IN_DB "camp_score"

struct KCampManager
{
    BOOL Init();
    void UnInit(){};

    void Activate();
    void AddScore(int nScore);

    int GetScore(){return m_nScore;}
    int GetLevel(){return m_nLevel;}

    void Reset();

    int  m_nActivePlayerCount[cTotal];
    
private:
    void Save();

    int     m_nScore;
    int     m_nLevel;   // ·¶Î§ÊÇ0µ½CAMP_LEVEL_COUNT-1
    BOOL    m_bUpdate;
    int     m_nLevelScore[CAMP_LEVEL_COUNT];
};
#endif
