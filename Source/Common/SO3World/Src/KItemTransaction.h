////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KItemTransaction.h
//  Version     : 1.0
//  Creator     : XiaYong
//  Create Date : 2008-8-15 12:49:26
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _INCLUDE_KITEMTRANSACTION_H_
#define _INCLUDE_KITEMTRANSACTION_H_
#ifdef _SERVER
////////////////////////////////////////////////////////////////////////////////

#include "SO3Result.h"
#include "Global.h"
#include <vector>
////////////////////////////////////////////////////////////////////////////////

class KItem;
class KPlayer;
class KItemTransaction
{
public:
    KItemTransaction();
    ~KItemTransaction();

    void Start(KPlayer* pPlayer);
    void Commit();
    void Rollback();
    
    ADD_ITEM_RESULT_CODE AddItem(KItem* pItem);
    ITEM_RESULT_CODE     CostItem(DWORD dwBox, DWORD dwPos, int nDestroyAmount = 1);

private:
    void OnAddItem(DWORD dwBox, DWORD dwPos);
    void OnUpdateItemAmount(DWORD dwBox, DWORD dwPos, int nAddAmount);
    void OnDestroyItem(DWORD dwBox, DWORD dwPos, KItem* pItem);
    BOOL CheckItemMaxExistAmount(KItem* pAddItem);
    void Reset();

private:
    enum KOPERATION_TYPE
    {
        otInvalid,
        otAddItem,
        otUpdateItem,
        otDestroyItem
    };
    
    struct KOPERATION_INFO 
    {
        KOPERATION_TYPE eType;
        DWORD           dwBox;
        DWORD           dwPos;
        int             nAddAmount;
        KItem*          pItem;
    };

    typedef std::vector<KOPERATION_INFO> VEC_OPERATION_INFO;

    VEC_OPERATION_INFO  m_Operation;
    KInventory*         m_pBox;     // 指向玩家的BOX起始地址
    KPlayer*            m_pPlayer;
};

//////////////////////////////////////////////////////////////////////////
#endif
#endif //_INCLUDE_KITEMTRANSACTION_H_
