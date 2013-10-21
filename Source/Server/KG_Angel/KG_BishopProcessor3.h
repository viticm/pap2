////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KG_BishopProcessor3.h
//  Version     : 1.0
//  Creator     : Wu Caizhong
//  Create Date : 2005-09-22 15:41:18
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _INCLUDE_KG_BISHOPPROCESSOR3_H_
#define _INCLUDE_KG_BISHOPPROCESSOR3_H_

////////////////////////////////////////////////////////////////////////////////
#include "KG_Angel.h"
#include "Engine/KG_InterlockedVariable.h"

class KG_BishopProcessor3 : public IKG_BishopProcessor
{
public:

    KG_BishopProcessor3() : m_ulRefCount(1) {}

    ////////////////////////////////////////////////////////////////////////////
    //                 interface function                                     //
    ////////////////////////////////////////////////////////////////////////////
    HRESULT STDMETHODCALLTYPE QueryInterface( 
        /* [in] */ REFIID riid,
        /* [iid_is][out] */ void **ppvObject
    )
    {
        KG_USE_ARGUMENT(riid);
        KG_USE_ARGUMENT(ppvObject);
        return KG_E_FAIL;
    }

    ULONG STDMETHODCALLTYPE AddRef(void)
    {
        return (unsigned long)KG_InterlockedIncrement((long *)&m_ulRefCount);
    }

    ULONG STDMETHODCALLTYPE Release(void)
    {
        long lCount = KG_InterlockedDecrement((long *)&m_ulRefCount);
        ASSERT(lCount >=0 );
        if (lCount == 0)
        {
            delete this;
        }
        return (unsigned long)lCount;
    }

    virtual int CheckRoleDataCRC(unsigned uBufferSize, void *pvRoleData);

    virtual char *GetRoleNameFromRoleData(unsigned uBufferSize,  void *pvRoleData);
   

    virtual int GetNormalRoleCountFromList(unsigned uBufferSize, void *pvRoleListData);
    virtual int GetFreezedRoleCountFromList(unsigned uBufferSize, void *pvRoleListData);

    virtual int GetNormalRoleNameFromList(
        unsigned uBufferSize, 
        void *pvRoleListData, 
        int nIndex, 
        char szRoleName[], 
        int nNameBufferSize
    );

    virtual int GetFreezedRoleNameFromList(
        unsigned uBufferSize, 
        void *pvRoleListData, 
        int nIndex, 
        char szRoleName[], 
        int nNameBufferSize
    );

    virtual int GetRoleNameFromPlayerCreateRoleParam(
        unsigned uParamSize, 
        void *pvCreateRoleParam,
        char szRoleName[],
        int nNameBufferSize
    );

    virtual int GetGoddessCreateRoleParamSize();

    virtual int GetGoddessCreateRoleParam(
        unsigned uGoddessParamSize, void *pvGoddessParam, // output
        unsigned uPlayerParamSize, void *pvPlayerCreateRoleParam, // input
        const char cszAccountName[]
    );
    

    virtual int OnSendRoleList(unsigned uBufferSize, void *pvRoleList);

    virtual int OnEnterGameRequest(unsigned uBufferSize, void *pvQueryPacket);    

    virtual int GetRoleDataFlag(
        unsigned uRoleDataBufferSize, void *pvRoleData,
        int  *pnUseRevive, int *pnRevivalID, int *pnEnterGameID 
    );

    // Set cUseRevive, cFightMode, reset CRC. 
    virtual int SetRoleDataFlag(   
        unsigned uRoleDataBufferSize, void *pvRoldData,
        char cUseRevive, char cFightMode 
    );

private:
    ~KG_BishopProcessor3() {};
    unsigned long m_ulRefCount;
};


#endif //_INCLUDE_KG_BISHOPPROCESSOR3_H_
