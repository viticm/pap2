#ifndef _DESIGNATION_H_
#define _DESIGNATION_H_

#include "Global.h"
#include "KCustomData.h"

#define MAX_DESIGNATION_FIX_ID      UCHAR_MAX
#define DESIGNATIONFIX_DATA_SIZE    (MAX_DESIGNATION_FIX_ID / CHAR_BIT + 1)

class KPlayer;

class KDesignation
{
public:
    KDesignation();
    ~KDesignation();

    BOOL Init(KPlayer* pPlayer);
	void UnInit();

    BOOL Save(size_t* puUsedSize, BYTE* pbyBuffer, size_t uBufferSize);
    BOOL Load(BYTE* pbyData, size_t uDataLen);

    BOOL SetCurrentDesignation(int nPrefix, int nPostfix, BOOL bDisplayFlag);
    BOOL AcquirePrefix(int nPrefix);
    BOOL AcquirePostfix(int nPostfix);
    BOOL RemovePrefix(int nPrefix);
    BOOL RemovePostfix(int nPostfix);
    BOOL IsPrefixAcquired(int nPrefix);
    BOOL IsPostfixAcquired(int nPostfix);

#ifdef _SERVER
    BOOL SetGeneration(int nGenerationIndex);
#endif

private:
    KCustomData<DESIGNATIONFIX_DATA_SIZE>  m_AcquiredPrefix;
    KCustomData<DESIGNATIONFIX_DATA_SIZE>  m_AcquiredPostfix;
    KPlayer*                               m_pPlayer;

#ifdef _SERVER
    BOOL BroadcastDesignationAnnounce(int nPrefix, int nPostfix, BYTE byType);
#endif

public:
    int     m_nCurrentPrefix;   // 前缀
    int     m_nCurrentPostfix;  // 后缀
    int     m_nGenerationIndex; // 辈分
    int     m_nBynameIndex;     // 绰号
    BOOL    m_bDisplayFlag;     // 是否显示辈分与绰号
};

#endif  //_DESIGNATION_H_
 
