//////////////////////////////////////////////////////////////////////////////////////
//
//  FileName    :   KG_QueueSIMO.h 
//  Version     :   1.0
//  Creater     :   zhao chunfeng
//  Date        :   2006-2-21 15:16:47
//  Comment     :   Copy from bishop !
//
//////////////////////////////////////////////////////////////////////////////////////

#ifndef _KG_QUEUE_SIMO_H_
#define _KG_QUEUE_SIMO_H_  1

#include "KSUnknown.h"
#include "Engine/KMutex.h"
#include "KG_QueueDef.h"


//  KQueueSIMO Single Input Multiple Output : Input not need lock, output need lock
template <unsigned uQueueSize>
class KQueueSIMO  
{
public:
    KQueueSIMO(): m_uHeadPos(0), m_uTailPos(0), m_ulMaxOutDataID(0){ };
    int Destory();

    int push(IUnknown *piValue, unsigned long ulDataID);
    
    // return KG_E_NO_DATA when data is will be in queue in the future 
    HRESULT pop(IUnknown **ppiRetValue, unsigned long ulDataID);
   

private:
    KQUEUE_NODE m_Data[uQueueSize];
    unsigned m_uHeadPos;
    unsigned m_uTailPos;
    unsigned long m_ulMaxOutDataID;   // use for fast check if have data match DataID
    
    KMutex   m_OutLock;

    // delete oldest node
    int _PopDelete();
    
    // return Found Pos, return -1 when not found
    // must be call after lock
    int _BinSearchForPop(unsigned long ulDataID);
};

template <unsigned uQueueSize>
inline int KQueueSIMO<uQueueSize>::Destory()
{
    m_OutLock.Lock();
    while (m_uHeadPos != m_uTailPos)
    {
        // Release Data
        KG_COM_RELEASE(m_Data[m_uHeadPos].m_piValue);
                
        m_uHeadPos = (m_uHeadPos + 1) % uQueueSize;
    }
    m_OutLock.Unlock();
    
    
    return true;
}

// delete oldest node(s)
template <unsigned uQueueSize>
inline int KQueueSIMO<uQueueSize>::_PopDelete()
{
    int nResult         = false;

    m_OutLock.Lock();

    KG_PROCESS_SUCCESS(m_uHeadPos == m_uTailPos);   // if empty then sucess, because is pop delete
    // if not empty
    
    // Release Data
    KG_COM_RELEASE(m_Data[m_uHeadPos].m_piValue);
            
    m_uHeadPos = (m_uHeadPos + 1) % uQueueSize;

Exit1:
    nResult = true;    
//Exit0:
    m_OutLock.Unlock();

    return nResult;
}

// because Single Input then not need lock
template <unsigned uQueueSize>
inline int KQueueSIMO<uQueueSize>::push(IUnknown *piValue, unsigned long ulDataID)
{
    int nResult  = false;
    int nRetCode = false;
    
    unsigned uTailPosNext = 0;
    
    KGLOG_PROCESS_ERROR(piValue);

    piValue->AddRef();    

    KGLOG_PROCESS_ERROR(ulDataID > m_ulMaxOutDataID);    // if piData is too old

    uTailPosNext = (m_uTailPos + 1) % uQueueSize;
        
    if (uTailPosNext == m_uHeadPos)     // if full then delete at lease one node
    {
        // delete oldest node
        nRetCode = _PopDelete();
        KGLOG_PROCESS_ERROR(nRetCode);
        
        // check out queue can be insert
        ASSERT(uTailPosNext != m_uHeadPos);
    }
    
    m_Data[m_uTailPos].m_piValue = piValue;
    m_Data[m_uTailPos].m_piValue->AddRef();
    m_Data[m_uTailPos].m_ulID    = ulDataID;
    
    m_uTailPos           = uTailPosNext;
    
    m_ulMaxOutDataID = ulDataID;
      
    nResult = true;  
Exit0:
    KG_COM_RELEASE(piValue);
    
    return nResult;
}


// return Found Pos, return -1 when not found
// must be call after lock
template <unsigned uQueueSize>
inline int KQueueSIMO<uQueueSize>::_BinSearchForPop(unsigned long ulDataID)
{
    int nLow  = 0;
    int nHigh = 0;
    
    nLow  = 0;
    nHigh = ((int)(((m_uTailPos + uQueueSize) - m_uHeadPos) % uQueueSize)) - 1;


    while (nLow <= nHigh)
    {
        int nMid = (nLow + nHigh) / 2;
        int nMidPos = (m_uHeadPos + nMid) % uQueueSize;
        
        if (m_Data[nMidPos].m_ulID > ulDataID)
            nHigh = nMid - 1;
        else if (m_Data[nMidPos].m_ulID < ulDataID)
            nLow  = nMid + 1;
        else 
            return nMidPos;     //if found then return nMidPos
    }    
    
    return -1;  // if not found    
}

// return KG_E_NO_DATA when data is will be in queue in the future 
template <unsigned uQueueSize>
inline HRESULT KQueueSIMO<uQueueSize>::pop(IUnknown **ppiRetValue, unsigned long ulDataID)
{
    if (!ppiRetValue)
        return KG_E_FAIL;

    if (ulDataID > m_ulMaxOutDataID)  // if data will be in queue in the future
        return KG_E_NO_DATA;
    
    HRESULT hrResult = KG_E_FAIL;
    int nRetCode = false;
    unsigned uPos = 0;

    m_OutLock.Lock();

    KGLOG_PROCESS_ERROR(m_uHeadPos != m_uTailPos); // if empty then error
    
    nRetCode = _BinSearchForPop(ulDataID);
    KGLOG_PROCESS_ERROR(nRetCode != -1);

    uPos = nRetCode;
    
    // if found 
    ASSERT(ulDataID == m_Data[uPos].m_ulID);

    *ppiRetValue = m_Data[uPos].m_piValue;
    //(*ppiRetData)->AddRef();
    //KG_COM_RELEASE(m_Data[uPos].m_piData);
    
    while (uPos != m_uHeadPos)
    {
        unsigned uNextPos = (uPos + uQueueSize - 1) % uQueueSize;
        m_Data[uPos] = m_Data[uNextPos];
        
        uPos = uNextPos;
    }

    m_uHeadPos = (m_uHeadPos + 1) % uQueueSize;
    
    hrResult = KG_S_OK;    
Exit0:
    m_OutLock.Unlock();

    return hrResult;
}



#endif  // _KG_QUEUE_SIMO_H_

