////////////////////////////////////////////////////////////////////////////////
//  Copyright(c) Kingsoft
//
//  FileName    : KG_TongInfo.h
//  Version     : 1.0
//  Creator     : Hu Hao
//  Create Date : 2009-9-26  15:31:46
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _KG_TONG_INFO_H_
#define _KG_TONG_INFO_H_

#include <vector>
#include "KTongDef.h"
#include "KTongDB.h"

class KG_TongInfo
{
public:
    KG_TongInfo();
    ~KG_TongInfo();

    int Init(const unsigned char cbyInfo[], unsigned uInfoSize, void *pvContext);
    int UnInit(void *pvContext);

    int GetTongName(unsigned uBufferSize, char *pszRetBuffer) const;

    int GetMasterID(int *pnRetMasterID) const;

    int GetCampID(int *pnRetCampID) const;

    int GetMaxMemberCount(int *pnRetMaxMemberCount) const;

    int GetTotalMemberCount(int *pnRetTotalMemberCount) const;
    int GetMemberID(int nMemberIndex, int *pnRetMemberID) const;
private:
    int _LoadBaseInfo(const unsigned char cbyInfo[], unsigned uInfoSize);
    int _LoadRosterInfo(const unsigned char cbyInfo[], unsigned uInfoSize);
private:
    typedef std::vector<KTONG_DB_MEMBER_DATA> _KG_MEMBER_DATA_VECTOR;

private:
    KTONG_DB_BASEINFO       m_BaseInfo;
    _KG_MEMBER_DATA_VECTOR  m_MemberDataVector;    
};

#endif  // _KG_TONG_INFO_H_

