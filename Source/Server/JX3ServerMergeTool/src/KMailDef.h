#ifndef _KMAIL_DEF_H_
#define _KMAIL_DEF_H_

enum 
{
    MAILSYS_VERSION_V3 = 3,
    MAILSYS_VERSION_V4 = 4,
    MAILSYS_VERSION = 5
};

#define KMAIL_MAX_ITEM_COUNT 8
#define MAIL_TITLE_LEN       64
#define MAX_MAIL_TEXT_LEN    400
#define SYSTEM_MAIL_SENDER   "[SYSTEM]"

enum KMAIL_TYPE
{
    eMailType_Player = 0,
    eMailType_System,
    eMailType_Auction,
    eMailType_GmMessage,
    eMailType_PlayerMessage,
    eMailType_Total
};

#pragma	pack(1)
struct KMailBoxDBHead
{
    WORD    wVersion;
    DWORD   dwNextMailID;
    DWORD   dwLastGlobalMailIndex;
};

struct KMailItemDesc 
{  
    bool   bAcquired;   // true 表示已经分发给玩家了 
    BYTE   byDataLen;
    int    nPrice;      // 货到付款的道具价格
};

struct KMail
{
    DWORD           dwMailID;
    BYTE            byType;
    bool            bRead;
    DWORD           dwSenderID;
    char            szSenderName[_NAME_LEN];
    char            szTitle[MAIL_TITLE_LEN];
    time_t          nSendTime;
    time_t          nRecvTime;
    time_t          nLockTime;
    int             nMoney;
    WORD            wTextLen;
    KMailItemDesc   ItemDesc[KMAIL_MAX_ITEM_COUNT];
    BYTE            byData[0];
};

#pragma	pack()

#endif // _K_MAIL_DEF_H_
