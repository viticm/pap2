//////////////////////////////////////////////////////////////////////////////////////
//
//  FileName    :   KG_QueueMISO.h 
//  Version     :   1.0
//  Creater     :   zhao chunfeng
//  Date        :   2006-2-21 15:16:47
//  Comment     :   Copy from bishop !
//
//////////////////////////////////////////////////////////////////////////////////////

#ifndef _KG_QUEUE_MISO_H_
#define _KG_QUEUE_MISO_H_   1

#include "KSUnknown.h"
#include "Engine/KMutex.h"
#include "KG_QueueDef.h"

//  KQueueMISO Multiple Input Single Output : Input need lock, output not need lock
template <unsigned uQueueSize>
class KQueueMISO  
{
public:
    KQueueMISO(): m_uHeadPos(0), m_uTailPos(0), m_ulDataID(0) { };
    int Destory();

    int push(IUnknown *piValue,      unsigned long *pulRetDataID);
    int pop(IUnknown **ppiRetValue,  unsigned long *pulRetDataID);
   

private:
    KQUEUE_NODE  m_Data[uQueueSize];
    unsigned m_uHeadPos;
    unsigned m_uTailPos;
    unsigned long m_ulDataID;
    
    KMutex   m_InLock;
};


template <unsigned uQueueSize>
inline int KQueueMISO<uQueueSize>::Destory()
{
    int nRetCode = false;
    IUnknown *piValue = NULL;
    unsigned long ulDataID = 0;

    while (true)
    {
        nRetCode = pop(&piValue, &ulDataID);
        if (!nRetCode)
            break;
        
        KG_COM_RELEASE(piValue);
    }
    
    return true;
}

// because Multiple Input then need lock
template <unsigned uQueueSize>
inline int KQueueMISO<uQueueSize>::push(IUnknown *piValue, unsigned long *pulRetDataID)
{
    int nResult = false;

    if (!piValue)
        return nResult; // error
        
    if (!pulRetDataID)
        return nResult;

    unsigned uTailPosNext = (m_uTailPos + 1) % uQueueSize;
    
    if (uTailPosNext == m_uHeadPos)     // if fast check full then error
        return nResult;

    m_InLock.Lock();
    piValue->AddRef();    

    uTailPosNext = (m_uTailPos + 1) % uQueueSize;
        
    KGLOG_PROCESS_ERROR(uTailPosNext != m_uHeadPos);   // if full then error
    
    m_Data[m_uTailPos].m_piValue = piValue;
    m_Data[m_uTailPos].m_piValue->AddRef();
    m_ulDataID++;
    m_Data[m_uTailPos].m_ulID    = m_ulDataID;
    
    m_uTailPos = uTailPosNext;
    
    *pulRetDataID = m_ulDataID;
    
    nResult = true;
Exit0:
    KG_COM_RELEASE(piValue);

    m_InLock.Unlock();
    
    return nResult;
}

// because Single Output then not need lock
template <unsigned uQueueSize>
inline int KQueueMISO<uQueueSize>::pop(IUnknown **ppiRetValue,  unsigned long *pulRetDataID)
{
    int nResult = false;
   
    KGLOG_PROCESS_ERROR(ppiRetValue); 
    KGLOG_PROCESS_ERROR(pulRetDataID);
            
    KG_PROCESS_ERROR(m_uHeadPos != m_uTailPos);   // if empty then error
    
    *ppiRetValue  = m_Data[m_uHeadPos].m_piValue;
    //(*ppiRetValue)->AddRef();
    //KG_COM_RELEASE(m_Data[m_uHeadPos].m_piValue);
    
    *pulRetDataID = m_Data[m_uHeadPos].m_ulID;
    
    m_uHeadPos   = (m_uHeadPos + 1) % uQueueSize;

    nResult      = true;
Exit0:

    return nResult;        
}

#endif  // _KG_QUEUE_MISO_H_

