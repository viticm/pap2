#ifndef _KMAIL_BOX_H_
#define _KMAIL_BOX_H_

#include <vector>
#include <map>
#include "KMailDef.h"

#define KMAILBOX_MAX_MAIL_COUNT     4000        // 一个邮箱最多容纳的邮件数
#define KMAIL_WITHDRAW_CYCLE        (60 * 60)   
#define SYNC_MAIL_LIST_MAX_COUNT    32          // 一次同步邮件列表的最大数

#pragma	pack(1)
struct KMailBoxDBHead
{
    WORD    wVersion;
    DWORD   dwNextMailID;
    DWORD   dwLastGlobalMailIndex;
};
#pragma	pack()

struct KMailCount
{
    int nPlayerMail;
    int nSystemMail;
    int nAuctionMail;
    int nGmMsgMail;
    int nPlayerMsgMail;
    int nTotalCount;
};

struct KMailListInfo;

typedef std::vector<KMailListInfo, KMemory::KAllocator<KMailListInfo> > KMailList;

class KMailBox
{
public:
    KMailBox();
    ~KMailBox();

    void Clear();
    BOOL Load(BYTE* pbyData, size_t uDataLen);
    BOOL Save(BYTE* pbyBuffer, size_t uBufferSize, size_t* puUsedSize);
    BOOL SendMail(KMail* pMail, size_t uMailLen);
    BOOL SendGlobalMail(KMail* pMail);
    int  GetMailList(KMailList* pMailList, time_t nTimeNow, DWORD dwStartID);
    BOOL DeleteMail(DWORD dwMailID);
    KMail* GetMail(DWORD dwMailID);
    int  GetWithdrawMail(
        time_t nTimeNow, time_t nSurvivalTime,
        std::vector<KMail*, KMemory::KAllocator<KMail*> >* pMailList
    );
    BOOL DeleteAll();
    void GetMailCount(KMailCount* pMailCount);

public:
    DWORD       m_dwID;
    BOOL        m_bSaved;
    time_t      m_nBaseTime;
    DWORD       m_dwLastGlobalMailIndex;

private:
    void ResetBaseTime();
    void EliminateMail(int nType, int nCount);

private:
    DWORD      m_dwNextMailID;

    typedef KMemory::KAllocator<std::pair<DWORD, KMail*> > KMailTableAlloctor;
    typedef std::map<DWORD, KMail*, std::less<DWORD>, KMailTableAlloctor> KMailTable;
    KMailTable m_MailTable;
};

#endif

