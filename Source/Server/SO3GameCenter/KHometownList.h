//////////////////////////////////////////////////////////////////////////////////////
//
//  FileName    :   KHometownList.h
//  Version     :   1.0
//  Creater     :   Zhao ChunFeng
//  Date        :   2007-03-16 16:18:48
//  Comment     :   
//
//////////////////////////////////////////////////////////////////////////////////////

#ifndef _KHOMETOWNLIST_H_
#define _KHOMETOWNLIST_H_

#include <vector>
#include <algorithm>

typedef std::vector<DWORD> KHOMETOWN_MAP_TABLE;

struct KBornPointInfo
{
    DWORD   dwMapID;
    int     nRoleType;
    int     nPosX;
    int     nPosY;
    int     nPosZ;
    int     nDirection;
};

class KHowntownList
{
public:
    KHowntownList();

    BOOL Load();

    BOOL GetBornPointInfo(KBornPointInfo* pBornPointInfo, DWORD dwMapID, int nRoleType);

    BOOL IsMapInHowntownList(DWORD dwMapID)
    {
        KHOMETOWN_MAP_TABLE::iterator it = std::find(m_HometownMapTable.begin(), m_HometownMapTable.end(), dwMapID);
        return (it != m_HometownMapTable.end());
    }

    const KHOMETOWN_MAP_TABLE* GetHometownMapTable()
    {
        return &m_HometownMapTable;
    }

private:

    typedef std::vector<KBornPointInfo> KBORN_POINT_TABLE;

    KBORN_POINT_TABLE    m_BornPointTable;
    KHOMETOWN_MAP_TABLE  m_HometownMapTable;
};

#endif

