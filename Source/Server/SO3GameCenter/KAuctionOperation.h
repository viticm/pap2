#ifndef _KAUCTION_OPERATION_H_
#define _KAUCTION_OPERATION_H_

enum KAUCTION_OPERATION_TYPE
{
    eaoInvalid = -1,

    eaoLookup,
    eaoBid,
    eaoSell,
    eaoCancel,

    eaoTotal
};

struct KAUCTION_OPERATION_HEAD
{
    BYTE    byType;
    DWORD   dwPlayerID;
    size_t  uSize;
};

struct KAUCTION_LOOKUP_OPERATION : KAUCTION_OPERATION_HEAD 
{
    BYTE                    byRequestID;
    KAUCTION_LOOKUP_PARAM   Param;
};

struct KAUCTION_BID_OPERATION : KAUCTION_OPERATION_HEAD 
{
    DWORD   dwSaleID;
    DWORD   dwCRC;
    int     nPrice;
};

struct KAUCTION_SELL_OPERATION : KAUCTION_OPERATION_HEAD
{
    char    szSaleName[SALE_NAME_LEN];
    char    szSellerName[ROLE_NAME_LEN];
    int     nAucGenre;
    int     nAucSub;
    int     nRequireLevel;
    int     nQuality;
    int     nStartPrice;
    int     nBuyItNowPrice;
    int     nCustodyCharges;
    time_t  nDurationTime;
    size_t  uDataLen;
    BYTE    byItemData[0];
};

struct KAUCTION_CANCEL_OPERATION : KAUCTION_OPERATION_HEAD 
{
    DWORD   dwSaleID;
};

#endif
