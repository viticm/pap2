#ifndef _KMAILOPERATION_H_ 
#define _KMAILOPERATION_H_ 

enum KMAIL_OPERATION_TYPE
{
    emoInvalid = -1,

    emoSendMail,
    emoGetMailList,
    emoDeleteMail,
    emoQueryMailContent,
    emoAcquireMailMoney,
    emoAcquireMailItem,
    emoReleaseMailMoney,
    emoReleaseMailItem,
    emoUnLockMail,
    emoSetMailRead,
    emoUserReturnMail,
    emoWithdrawMail,
    emoDeleteAll,
    emoLoadMailCount,

    emoTotal
};

struct KMAIL_OPERATION_HEAD
{
    BYTE    byType;
    DWORD   dwMailBoxID;  // ” œ‰IDæÕ «ÕÊº“ID
    size_t  uSize;
};

struct KMAIL_SEND_MAIL_OPERATION : KMAIL_OPERATION_HEAD
{
    BYTE    byRequestID;
    BYTE    byData[0];    // KMail
};

struct KMAIL_GET_MAIL_LIST_OPERATION : KMAIL_OPERATION_HEAD 
{
    DWORD   dwStartID;
};

struct KMAIL_DELETE_MAIL_OPERATION : KMAIL_OPERATION_HEAD 
{
    DWORD   dwMailID;
};

struct KMAIL_QUERY_MAIL_CONTENT_OPERATION : KMAIL_OPERATION_HEAD
{
    DWORD       dwMailID;
    KMAIL_TYPE  eMailType;
};

struct KMAIL_ACQUIRE_MAIL_MONEY_OPERATION : KMAIL_OPERATION_HEAD 
{
    DWORD   dwMailID;
};

struct KMAIL_ACQUIRE_MAIL_ITEM_OPERATION : KMAIL_OPERATION_HEAD 
{
    DWORD   dwMailID;
    int     nIndex;
};

struct KMAIL_RELEASE_MAIL_MONEY_OPERATION : KMAIL_OPERATION_HEAD 
{
    DWORD   dwMailID;
};

struct KMAIL_RELEASE_MAIL_ITEM_OPERATION : KMAIL_OPERATION_HEAD 
{
    DWORD   dwMailID;
    int     nIndex;
};

struct KMAIL_UNLOCK_MAIL_OPERATION : KMAIL_OPERATION_HEAD 
{
    DWORD   dwMailID;
};

struct KMAIL_SET_MAIL_READ_OPERATION : KMAIL_OPERATION_HEAD 
{
    DWORD   dwMailID;
};

struct KMAIL_USER_RETURN_MAIL_OPERATION : KMAIL_OPERATION_HEAD
{
    DWORD   dwMailID;
};

struct KMAIL_WITHDRAW_MAIL_OPERATION : KMAIL_OPERATION_HEAD 
{
};

struct KMAIL_DELETE_ALL_OPERATION : KMAIL_OPERATION_HEAD 
{
};

struct KMAIL_LOAD_COUNT_OPERATION : KMAIL_OPERATION_HEAD 
{
};
#endif
