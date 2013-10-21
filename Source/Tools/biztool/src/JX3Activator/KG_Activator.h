////////////////////////////////////////////////////////////////////////////////
//  Copyright(c) Kingsoft
//
//  FileName    : KG_Activator.h
//  Version     : 1.0
//  Creator     : Hu Hao
//  Create Date : 2009-5-31 14:17:10
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _KG_ACTIVATOR_H_
#define _KG_ACTIVATOR_H_

#include "KG_HttpClient.h"
#include "KG_MainDialog.h"
#include "KG_Step1Dialog.h"
#include "KG_Step2Dialog.h"
#include "KG_Step4Dialog.h"
#include "KG_Step5Dialog.h"
#include "KG_Step6Dialog.h"
#include "KG_Step7Dialog.h"
#include "KG_Step8Dialog.h"

enum { KG_ACTIVATOR_POST_INTERVAL = 2500 };
enum { KG_ACTIVATOR_POST_RETRY_COUNT = 10 };
enum { KG_RETRY_INTERVAL = 20 };    // 保证一个用户20秒之内只能连一次服务器，减少服务器负载

enum KG_ACTIVATOR_STEP
{
    KG_ACTIVATOR_STEP_AGREEMENT,
    KG_ACTIVATOR_STEP_USERINFO,
    KG_ACTIVATOR_STEP_ACCOUNT,
};

class KG_Activator
{
public:
    KG_Activator();
    ~KG_Activator();

    int Run();

    int InitChildDialog();
    int UpdateVerifyImage();

    int Back();
    int Next();
    int Cancel();
    int Quit();

private:
    typedef std::map<std::string, std::string> KG_POST_PARAM_MAP;

private:
    int _GetHostIP();
    int _GetUserInfo();
    int _GetAccountInfo();

    int _CheckResult();

    int _GetMachineInfo();

    int _PostDataString(
        const char cszServer[],
        const char cszObject[],
        int *pnResponseCode,
        std::string *pstrRetResponseInfo
    );
private:
    KG_MainDialog           m_MainDialog;
    KG_Step1Dialog          m_AgreementDialog;
    KG_Step4Dialog          m_UserInfoDialog;
    KG_Step2Dialog          m_AccountInfoDialog;
    KG_Step5Dialog          m_PassDialog;
    KG_Step6Dialog          m_FailDialog;
    KG_Step7Dialog          m_TodayOverDialog;
    KG_Step8Dialog          m_AllOverDialog;

    std::string             m_strHost;
    KG_ACTIVATOR_STEP       m_eCurrentStep;
    time_t                  m_lLastPostErrorTime;
    KG_POST_PARAM_MAP       m_PostParamMap;
};

#endif // _KG_ACTIVATOR_H_