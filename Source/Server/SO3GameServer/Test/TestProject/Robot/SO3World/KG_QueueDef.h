//////////////////////////////////////////////////////////////////////////////////////
//
//  FileName    :   KG_QueueDel.h
//  Version     :   1.0
//  Creater     :   zhao chunfeng
//  Date        :   2006-2-21 15:16:47
//  Comment     :   Copy from bishop !
//
//////////////////////////////////////////////////////////////////////////////////////

#ifndef _KG_QUEUEDEL_H_
#define _KG_QUEUEDEL_H_     1

#include "KSUnknown.h"

typedef struct
{
    unsigned long    m_ulID;
    IUnknown        *m_piValue;
} KQUEUE_NODE;

#endif  // _KG_QUEUEDEL_H_

