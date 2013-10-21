////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KG3DProfile.h
//  Version     : 1.0
//  Creator     : Wu Caizhong
//  Create Date : 2009/8/26 17:55:06
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _INCLUDE_KG3DPROFILE_H_
#define _INCLUDE_KG3DPROFILE_H_

////////////////////////////////////////////////////////////////////////////////
class IKG3Profile_Recorder
{
public:
    virtual void Start() = 0;
    virtual void Stop() = 0;
};

class KG3DProfile_Manager
{
public:
    virtual HRESULT Init(int nEnableProfileFlag) = 0;
    virtual HRESULT UnInit() = 0;

    virtual IKG3Profile_Recorder *GetRecord(unsigned uUnionID, const char cszName[]) = 0;
};

KG3DProfile_Manager &GetProfileManager();



////////////////////////////////////////////////////////////////////////////////
class KG3Profile_RecorderHelper
{
public:
    KG3Profile_RecorderHelper(unsigned uUnionID, const char cszName[])
    {
        m_piRecorder = GetProfileManager().GetRecord(uUnionID, cszName);
        if (m_piRecorder)
            m_piRecorder->Start();
    }
    ~KG3Profile_RecorderHelper()
    {
        if (m_piRecorder)
            m_piRecorder->Stop();
    }
private:
    IKG3Profile_Recorder *m_piRecorder;
};
//从MSDN学的，必须要用另外一个宏UNIQUE_NAME2转一下才能生成唯一名称，看上去似乎是在欺骗编译器
#define UNIQUE_NAME2(x,y) x##y
#define UNIQUE_NAME(x,y) UNIQUE_NAME2(x,y)

// __COUNTER__ only unique in one file
#define KG3PROFILE_DURATION(szName)                                 \
extern unsigned g_uRecordID;                                        \
static unsigned UNIQUE_NAME(s_uRecordID, __LINE__) = ++g_uRecordID; \
KG3Profile_RecorderHelper UNIQUE_NAME(Profile, __LINE__)(           \
    UNIQUE_NAME(s_uRecordID, __LINE__), szName                      \
);

////////////////////////////////////////////////////////////////////////////////

#endif //_INCLUDE_KG3DPROFILE_H_
