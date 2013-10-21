#include "stdafx.h"
#include "KMentorTableData.h"
#include "KMentorDef.h"

KMentorTableData::KMentorTableData(void)
{
    memset(&m_KMentorTableData, 0, sizeof(m_KMentorTableData));
    m_KMentorTableData.nVersion = 1;
}

KMentorTableData::~KMentorTableData(void)
{

}

int KMentorTableData::PaserMentorInfo(unsigned char* pbyData, size_t uDataLen)
{
    int nResult     = false;
    int nRetCode    = false;

    KMentorRecordDB*        pRecord     = NULL;

    KG_PROCESS_ERROR(sizeof(KMentorRecordDB_Analyser) == sizeof(KMentorRecordDB));
    KG_PROCESS_ERROR(uDataLen == sizeof(KMentorRecordDB_Analyser));
    pRecord = (KMentorRecordDB*)pbyData;

    KG_PROCESS_ERROR(pRecord->nVersion == 1);

    m_KMentorTableData.nCreateTime      = pRecord->nCreateTime;     
    m_KMentorTableData.nMentorValue     = pRecord->nMentorValue;    
    m_KMentorTableData.nEndTime         = pRecord->nEndTime;        
    m_KMentorTableData.byState          = pRecord->byState;
    m_KMentorTableData.byOriginalALevel = pRecord->byOriginalALevel;

    nResult = true;
Exit0:
    return nResult; 
}
