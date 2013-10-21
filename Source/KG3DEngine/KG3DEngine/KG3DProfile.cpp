////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KG3DProfile.cpp
//  Version     : 1.0
//  Creator     : Wu Caizhong
//  Create Date : 2009/8/26 17:55:15
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "KG3DProfile.h"
#include "KThread.h"

unsigned g_uRecordID = 0;
const unsigned OUTPUT_COLUMN_WIDTH = 8;

////////////////////////////////////////////////////////////////////////////////
// not support multi thread recorder now
class KG3Profile_Recorder_Imp : public IKG3Profile_Recorder
{
public:
    KG3Profile_Recorder_Imp(const char cszName[]);
    ~KG3Profile_Recorder_Imp(void);

    void Start()
    {
        ::QueryPerformanceCounter(&m_llStart);
        m_nRecording = true;
    }
    void Stop()
    {
        LARGE_INTEGER llEnd;
        ::QueryPerformanceCounter(&llEnd);
        m_llRecordValue.QuadPart += (llEnd.QuadPart - m_llStart.QuadPart);
        ++m_uCount;
        m_nRecording = false;
    }

public:
    void Sample(
        const LARGE_INTEGER &llCurTime, 
        LARGE_INTEGER *pllRetRecordTime, 
        unsigned *puRetRecordCount
    )
    {
        if (m_nRecording)
        {
            pllRetRecordTime->QuadPart = m_llRecordValue.QuadPart + (llCurTime.QuadPart - m_llStart.QuadPart);
            m_llRecordValue.QuadPart = 0;

            m_llStart = llCurTime;
        }
        else
        {
            *pllRetRecordTime = m_llRecordValue;
            m_llRecordValue.QuadPart = 0;
        }
        *puRetRecordCount = m_uCount;
        m_uCount = 0;
    }
    const char  *GetName()  { return m_sName.c_str(); };

private:
    std::string m_sName;
    LARGE_INTEGER m_llStart;
    unsigned m_uCount;
    LARGE_INTEGER m_llRecordValue;
    int m_nRecording;
};
KG3Profile_Recorder_Imp::KG3Profile_Recorder_Imp(const char cszName[])
{
    ASSERT(cszName);
    m_sName = cszName;
    m_llStart.QuadPart = 0;
    m_uCount = 0;
    m_llRecordValue.QuadPart = 0;
    m_nRecording = false;
}
KG3Profile_Recorder_Imp::~KG3Profile_Recorder_Imp(void)
{
    ASSERT(!m_nRecording);
}


////////////////////////////////////////////////////////////////////////////////
class KG3DProfile_Manager_Imp : public KG3DProfile_Manager
{
public:
    KG3DProfile_Manager_Imp();
    ~KG3DProfile_Manager_Imp();
    HRESULT Init(int nEnableProfileFlag);
    HRESULT UnInit();

    IKG3Profile_Recorder *GetRecord(unsigned uUnionID, const char cszName[]);

private:
    static void _WorkThread(void *pvThis);
    int _ThreadFunction();
    int _Sample(FILE *fpOutput);
    int _OutputTitle(FILE *fpOutput);
private:
    int m_nEnableProfileFlag;

    KThread m_WorkThread;
    int m_nExitFlag;

    typedef std::map<unsigned, KG3Profile_Recorder_Imp *> RECORDER_MAP;
    RECORDER_MAP m_RecorderMap;
    LARGE_INTEGER m_llFirstSampleTime;
};
////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////
static KG3DProfile_Manager_Imp gs_Profile_Manager;
KG3DProfile_Manager &GetProfileManager()
{
    return gs_Profile_Manager; // cast from child class type to base class type
}
////////////////////////////////////////////////////////////////////////////////









////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
KG3DProfile_Manager_Imp::KG3DProfile_Manager_Imp(void)
{
    m_nEnableProfileFlag = false;
    m_llFirstSampleTime.QuadPart = 0;
}

////////////////////////////////////////////////////////////////////////////////
KG3DProfile_Manager_Imp::~KG3DProfile_Manager_Imp(void)
{
    RECORDER_MAP::iterator it;
    for (it = m_RecorderMap.begin(); it != m_RecorderMap.end(); ++it)
    {
        KG_DELETE(it->second);
    }
    m_RecorderMap.clear();
}
////////////////////////////////////////////////////////////////////////////////
HRESULT KG3DProfile_Manager_Imp::Init(int nEnableProfileFlag)
{
    HRESULT hrResult = E_FAIL;
    int nRetCode = false; 
    int nWorkThreadCreateFlag = false;

    m_nEnableProfileFlag = nEnableProfileFlag;

    KG_PROCESS_SUCCESS(!m_nEnableProfileFlag);

    nRetCode = m_WorkThread.Create(_WorkThread, this);
    KGLOG_PROCESS_ERROR(nRetCode);
    nWorkThreadCreateFlag = true;

Exit1:
    hrResult = S_OK;
Exit0:
    if (FAILED(hrResult))
    {
        m_nExitFlag = true;
        m_WorkThread.Destroy();
        nWorkThreadCreateFlag = false;
    }
    return hrResult;
}
////////////////////////////////////////////////////////////////////////////////
HRESULT KG3DProfile_Manager_Imp::UnInit()
{
    HRESULT hrResult = E_FAIL;

    if (m_nEnableProfileFlag)
    {
        m_nExitFlag = true;
        m_WorkThread.Destroy();
    }
    hrResult = S_OK;
//Exit0:
    return hrResult;
}

////////////////////////////////////////////////////////////////////////////////
IKG3Profile_Recorder *KG3DProfile_Manager_Imp::GetRecord(unsigned uUnionID, const char cszName[])
{
    IKG3Profile_Recorder *pRecorder = NULL;
    KG3Profile_Recorder_Imp *pNewRecorder = NULL;
    RECORDER_MAP::iterator itExist;

    ASSERT(cszName);

    KG_PROCESS_SUCCESS(!m_nEnableProfileFlag);

    itExist = m_RecorderMap.find(uUnionID);
    if (itExist != m_RecorderMap.end())
    {
        pRecorder = itExist->second;
    }
    else
    {
        RECORDER_MAP::_Pairib InsertResult;
        pNewRecorder = new KG3Profile_Recorder_Imp(cszName);
        KGLOG_PROCESS_ERROR(pNewRecorder);

        InsertResult = m_RecorderMap.insert(make_pair(uUnionID, pNewRecorder));
        ASSERT(InsertResult.second);

        pRecorder = pNewRecorder;
        pNewRecorder = NULL;
    }
Exit1:
Exit0:
    KG_DELETE(pNewRecorder);
    return pRecorder;
}

////////////////////////////////////////////////////////////////////////////////
void KG3DProfile_Manager_Imp::_WorkThread(void *pvThis)
{
    ASSERT(pvThis);
    ((KG3DProfile_Manager_Imp *)pvThis)->_ThreadFunction();
}
////////////////////////////////////////////////////////////////////////////////
int KG3DProfile_Manager_Imp::_ThreadFunction()
{
    int nResult  = false;
    int nRetCode = false;
    DWORD dwSampleTime = 0;
    int   nSleepTime = 0;
    DWORD dwBegigTime = 0;
    FILE *fpOutput = NULL;
    char szLogFileName[PATH_MAX];
    time_t tmtNow = 0;
    struct tm tmNow; 
    RECORDER_MAP::iterator it;
    size_t uColumnCount = 0;


    nSleepTime = 4000; // wait some time to skip engine init
    while (nSleepTime > 0)
    {
        KG_PROCESS_SUCCESS(m_nExitFlag);
        ::Sleep(100);
        nSleepTime -= 100;
    }

    tmtNow = time(NULL);
    localtime_r(&tmtNow, &tmNow);
    nRetCode = snprintf(
        szLogFileName, sizeof(szLogFileName) - 1,
        "Profile\\KG3DEngineProfile_%d%2.2d%2.2d_%2.2d%2.2d%2.2d.csv", 
        tmNow.tm_year + 1900,
        tmNow.tm_mon + 1,
        tmNow.tm_mday,
        tmNow.tm_hour,
        tmNow.tm_min,
        tmNow.tm_sec
     );
    ASSERT((nRetCode != -1) && (nRetCode < (sizeof(szLogFileName) - 1)));
    szLogFileName[sizeof(szLogFileName) - 1] = '\0';

    mkdir("Profile");

    fpOutput = fopen(szLogFileName, "w");
    KGLOG_PROCESS_ERROR(fpOutput); 

    ::QueryPerformanceCounter(&m_llFirstSampleTime);
    while (true)
    {
        nSleepTime = 1000 - dwSampleTime;
        while (nSleepTime > 0)
        {
            KG_PROCESS_SUCCESS(m_nExitFlag);
            ::Sleep(100);
            nSleepTime -= 100;
        }

        dwBegigTime = ::timeGetTime();
        if (uColumnCount != m_RecorderMap.size())
        {
            nRetCode = _OutputTitle(fpOutput);
            KGLOG_PROCESS_ERROR(nRetCode);
            uColumnCount = m_RecorderMap.size();
        }
        if (uColumnCount)
        {
            nRetCode = _Sample(fpOutput);
            //KGLOG_PROCESS_ERROR(nRetCode);
        }
        dwSampleTime = ::timeGetTime() - dwBegigTime;
    }
Exit1:
    nResult = true;
Exit0:
    if (fpOutput)
    {
        fclose(fpOutput);
        fpOutput = NULL;
    }
    return nResult;
}

int KG3DProfile_Manager_Imp::_Sample(FILE *fpOutput)
{
    int nResult  = false;
    LARGE_INTEGER llSampleTime;
    LARGE_INTEGER llFrequency;
    float fTimeInMS = 0.0f;
    float fAvgTimeInMS = 0.0f;
    LARGE_INTEGER llRecordTime;
    unsigned uRecordCount;
    RECORDER_MAP::iterator it;

    ASSERT(fpOutput);

    ::QueryPerformanceCounter(&llSampleTime);
    ::QueryPerformanceFrequency(&llFrequency);

    fTimeInMS = (float)((llSampleTime.QuadPart - m_llFirstSampleTime.QuadPart) * 1000.0 / llFrequency.QuadPart);
    fprintf(fpOutput, "%*.0f", -(int)OUTPUT_COLUMN_WIDTH, fTimeInMS);
    for (it = m_RecorderMap.begin(); it != m_RecorderMap.end(); ++it)
    {
        it->second->Sample(llSampleTime, &llRecordTime, &uRecordCount);
        fTimeInMS = (float)(llRecordTime.QuadPart * 1000.0 / llFrequency.QuadPart);
        fAvgTimeInMS = fTimeInMS;
        if (uRecordCount > 0)
            fAvgTimeInMS /= (float)uRecordCount;
        
        fprintf(
            fpOutput, 
            ",%*.1f,%*.1f,%*u", 
            -(int)OUTPUT_COLUMN_WIDTH, fTimeInMS, 
            -(int)OUTPUT_COLUMN_WIDTH, fAvgTimeInMS, 
            -(int)OUTPUT_COLUMN_WIDTH, uRecordCount
        );
    }
    fprintf(fpOutput, "\n");
    fflush(fpOutput);
    nResult = true;
//Exit0:
    return nResult;
}
////////////////////////////////////////////////////////////////////////////////
int KG3DProfile_Manager_Imp::_OutputTitle(FILE *fpOutput)
{
    int nResult  = false;
    RECORDER_MAP::iterator it;
    int nNeedMoreLine = true;
    struct COLUMN_TITLE
    {
        std::string sName;
        unsigned uRemainOutputLen;
        unsigned uNextOutputPos;
    };
    COLUMN_TITLE ColumnTitle;
    std::vector<COLUMN_TITLE> vecColumnTitle;

    ASSERT(fpOutput);

    ColumnTitle.sName = "Time";
    ColumnTitle.uRemainOutputLen = (unsigned)strlen("Time");
    ColumnTitle.uNextOutputPos = 0;
    vecColumnTitle.push_back(ColumnTitle);

    for (it = m_RecorderMap.begin(); it != m_RecorderMap.end(); ++it)
    {
        ColumnTitle.sName = it->second->GetName();
        ColumnTitle.sName += "(Total)";
        ColumnTitle.uRemainOutputLen = (unsigned)ColumnTitle.sName.length();
        ColumnTitle.uNextOutputPos = 0;
        vecColumnTitle.push_back(ColumnTitle);

        ColumnTitle.sName = it->second->GetName();
        ColumnTitle.sName += "(Avg)";
        ColumnTitle.uRemainOutputLen = (unsigned)ColumnTitle.sName.length();
        ColumnTitle.uNextOutputPos = 0;
        vecColumnTitle.push_back(ColumnTitle);

        ColumnTitle.sName = it->second->GetName();
        ColumnTitle.sName += "(Count)";
        ColumnTitle.uRemainOutputLen = (unsigned)ColumnTitle.sName.length();
        ColumnTitle.uNextOutputPos = 0;
        vecColumnTitle.push_back(ColumnTitle);
    }

    // output a title with more then one line
    while (nNeedMoreLine)
    {
        nNeedMoreLine = false;

        for (size_t i = 0; i < vecColumnTitle.size(); ++i)
        {
            COLUMN_TITLE *pColumnTitle = &(vecColumnTitle[i]);
            char szName[OUTPUT_COLUMN_WIDTH + 1];
            szName[0] = '\0';

            if (pColumnTitle->uRemainOutputLen > 0)
            {
                const char *pcszOutputPos  = pColumnTitle->sName.c_str() + pColumnTitle->uNextOutputPos;
                unsigned uOutputLen = pColumnTitle->uRemainOutputLen;
                if (uOutputLen > OUTPUT_COLUMN_WIDTH)
                {
                    uOutputLen = OUTPUT_COLUMN_WIDTH;
                    // break line between word
                    const char *pchLastChar = pcszOutputPos + uOutputLen - 1;
                    while (pchLastChar > pcszOutputPos)
                    {
                        if (!isalpha(*pchLastChar) && !isdigit(*pchLastChar))
                        {
                            uOutputLen = (unsigned)(pchLastChar - pcszOutputPos);
                            if (isspace(*pchLastChar))
                                ++uOutputLen; // is not space such as '(', to next line
                            break;
                        } 
                        --pchLastChar;
                    }    
                }

                memcpy(szName, pcszOutputPos, uOutputLen);
                szName[uOutputLen] = '\0';

                if (pColumnTitle->uRemainOutputLen > OUTPUT_COLUMN_WIDTH)
                    nNeedMoreLine = true;

                pColumnTitle->uRemainOutputLen -= uOutputLen;
                pColumnTitle->uNextOutputPos   += uOutputLen;
            }

            if (i == 0)
                fprintf(fpOutput, "%*s", -(int)OUTPUT_COLUMN_WIDTH, szName);
            else
                fprintf(fpOutput, ",%*s", -(int)OUTPUT_COLUMN_WIDTH, szName);
        }
        fprintf(fpOutput, "\n");
    }

    fflush(fpOutput);

    nResult = true;
//Exit0:
    return nResult;
}

////////////////////////////////////////////////////////////////////////////////


