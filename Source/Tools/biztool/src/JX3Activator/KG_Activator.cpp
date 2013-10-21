////////////////////////////////////////////////////////////////////////////////
//  Copyright(c) Kingsoft
//
//  FileName    : KG_Activator.cpp
//  Version     : 1.0
//  Creator     : Hu Hao
//  Create Date : 2009-5-31 14:52:18
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Resource.h"
#include "KG_Activator.h"
#include "SystemInfo.h"
#include "my_md5.h"
#include <atlrx.h>

////////////////////////////////////////////////////////////////////////////////
const char KG_POST_PARAM_SYS_INFO[]         = "h_sys_info";
const char KG_POST_PARAM_SYS_VERSION[]      = "h_sys_version";
const char KG_POST_PARAM_CPU_INFO[]         = "h_cpu";
const char KG_POST_PARAM_MEMORY_INFO[]      = "h_memory";
const char KG_POST_PARAM_VGA_INFO[]         = "h_vga";
const char KG_POST_PARAM_VGA_MEMORY[]       = "h_vga_memory";
const char KG_POST_PARAM_UUID[]             = "h_uuid";
const char KG_POST_PARAM_ENCRYPTED_CODE[]   = "h_code";
const char KG_POST_PARAM_PASSPORT[]         = "passport";
const char KG_POST_PARAM_PASSWORD[]         = "password";
const char KG_POST_PARAM_VERIFY_CODE[]      = "verifycode";
const char KG_POST_PARAM_PHONE[]            = "phone";
const char KG_POST_PARAM_EMAIL[]            = "email";
const char KG_POST_PARAM_QQ[]               = "qq";

////////////////////////////////////////////////////////////////////////////////
inline int _CheckRegEx(LPCTSTR szRE, LPCTSTR szInput)
{
    CAtlRegExp<> re;
    CAtlREMatchContext<> mc;

    if (szRE == NULL)
        return false;
    if (szInput == NULL)
        return false;

    re.Parse(szRE);
    return re.Match(szInput, &mc);
}

////////////////////////////////////////////////////////////////////////////////
KG_Activator::KG_Activator()
{
    m_lLastPostErrorTime = 0;
}

KG_Activator::~KG_Activator()
{
}

int KG_Activator::Run()
{
    m_MainDialog.DoModal();    
    return true;
}

int KG_Activator::InitChildDialog()
{
    int nResult  = false;
    int nRetCode = false;

    nRetCode = m_AgreementDialog.Create(IDD_STEP1, &m_MainDialog);
    KGLOG_PROCESS_ERROR(nRetCode);

    nRetCode = m_AccountInfoDialog.Create(IDD_STEP2, &m_MainDialog);
    KGLOG_PROCESS_ERROR(nRetCode);

    nRetCode = m_UserInfoDialog.Create(IDD_STEP4, &m_MainDialog);
    KGLOG_PROCESS_ERROR(nRetCode);

    nRetCode = m_PassDialog.Create(IDD_STEP5, &m_MainDialog);
    KGLOG_PROCESS_ERROR(nRetCode);

    nRetCode = m_FailDialog.Create(IDD_STEP6, &m_MainDialog);
    KGLOG_PROCESS_ERROR(nRetCode);

    nRetCode = m_TodayOverDialog.Create(IDD_STEP7, &m_MainDialog);
    KGLOG_PROCESS_ERROR(nRetCode);

    nRetCode = m_AllOverDialog.Create(IDD_STEP8, &m_MainDialog);
    KGLOG_PROCESS_ERROR(nRetCode);

    m_eCurrentStep = KG_ACTIVATOR_STEP_AGREEMENT;

    m_AgreementDialog.ShowWindow(SW_SHOW);
    m_AccountInfoDialog.ShowWindow(SW_HIDE);
    m_UserInfoDialog.ShowWindow(SW_HIDE);
    m_PassDialog.ShowWindow(SW_HIDE);
    m_FailDialog.ShowWindow(SW_HIDE);
    m_TodayOverDialog.ShowWindow(SW_HIDE);
    m_AllOverDialog.ShowWindow(SW_HIDE);
    
    nRetCode = _GetHostIP();
    KGLOG_PROCESS_ERROR(nRetCode);

    nResult = true;
Exit0:
    return nResult;
}

int KG_Activator::UpdateVerifyImage()
{
    int nResult  = false;
    int nRetCode = false;
    char szTempFilePath[MAX_PATH];
    std::string strImageFileName;
    unsigned char *pbyResponseData = NULL;
    DWORD dwResponseData = 0;

    nRetCode = GetTempPath(sizeof(szTempFilePath), szTempFilePath);
    KGLOG_PROCESS_ERROR(nRetCode);

    strImageFileName  = szTempFilePath;
    strImageFileName += "JX3Activator.png";

    nRetCode = KGU_DownloadHttpFile(
        m_strHost.c_str(), "/img.php", strImageFileName.c_str()
    );
    if (!nRetCode)
        strImageFileName.clear();

    nRetCode = m_AccountInfoDialog.UpdateVerifyImage(strImageFileName.c_str());
    KGLOG_PROCESS_ERROR(nRetCode);

    nResult = true;
Exit0:
    return nResult;
}

int KG_Activator::Back()
{
    switch (m_eCurrentStep)
    {
    case KG_ACTIVATOR_STEP_AGREEMENT:
        ASSERT(false);
        break;
    case KG_ACTIVATOR_STEP_USERINFO:
        m_eCurrentStep = KG_ACTIVATOR_STEP_AGREEMENT;
        m_UserInfoDialog.ShowWindow(SW_HIDE);
        m_AgreementDialog.ShowWindow(SW_SHOW);
        break;
    case KG_ACTIVATOR_STEP_ACCOUNT:
        m_eCurrentStep = KG_ACTIVATOR_STEP_USERINFO;
        m_AccountInfoDialog.ShowWindow(SW_HIDE);
        m_UserInfoDialog.ShowWindow(SW_SHOW);
        break;
    default:
        ASSERT(false);
        break;
    }

    return true;
}

int KG_Activator::Next()
{
    int nResult  = false;
    int nRetCode = false;
    time_t lCurrentTime = 0;

    switch (m_eCurrentStep)
    {
    case KG_ACTIVATOR_STEP_AGREEMENT:
        m_eCurrentStep = KG_ACTIVATOR_STEP_USERINFO;
        m_AgreementDialog.ShowWindow(SW_HIDE);
        m_UserInfoDialog.ShowWindow(SW_SHOW);
        break;
    case KG_ACTIVATOR_STEP_USERINFO:
        nRetCode = _GetUserInfo();
        if (!nRetCode)
            break;
             
        m_eCurrentStep = KG_ACTIVATOR_STEP_ACCOUNT;
        m_UserInfoDialog.ShowWindow(SW_HIDE);
        m_AccountInfoDialog.ShowWindow(SW_SHOW);
        UpdateVerifyImage();
        break;
    case KG_ACTIVATOR_STEP_ACCOUNT:
        lCurrentTime = time(NULL);
        if (lCurrentTime - m_lLastPostErrorTime <= KG_RETRY_INTERVAL)
        {
            Sleep(2000);
            MessageBox(m_MainDialog, "系统繁忙，请稍后重试", "提示", MB_OK);
            break;
        }

        nRetCode = _GetAccountInfo();
        if (!nRetCode)
            break;

        _CheckResult();
        break;
    default:
        break;
    }

    return true;
}

int KG_Activator::Cancel()
{
    int nResult  = false;
    int nRetCode = false;

    nRetCode = MessageBox(
        m_MainDialog, "确定退出申请", "提示", MB_OKCANCEL | MB_DEFBUTTON2
    );
    KG_PROCESS_SUCCESS(nRetCode == IDCANCEL);   // Indicate not cancel.

    nRetCode = Quit();
    KGLOG_PROCESS_ERROR(nRetCode);

Exit1:
    nResult = true;
Exit0:
    return nResult;
}

int KG_Activator::Quit()
{
    m_AllOverDialog.DestroyWindow();
    m_TodayOverDialog.DestroyWindow();
    m_FailDialog.DestroyWindow();
    m_PassDialog.DestroyWindow();
    m_UserInfoDialog.DestroyWindow();
    m_AccountInfoDialog.DestroyWindow();
    m_AgreementDialog.DestroyWindow();
    
    m_MainDialog.EndDialog(IDOK);
    
    return true;
}

int KG_Activator::_GetHostIP()
{
    int nResult  = false;
    int nRetCode = false;
    hostent* remoteHost;
    char szBuffer[128];
    struct in_addr ia;

    remoteHost = gethostbyname("tester.jx3.xoyo.com");
    KGLOG_PROCESS_ERROR(remoteHost);

    KGLOG_PROCESS_ERROR(remoteHost->h_addr_list);

    memcpy(&ia, remoteHost->h_addr_list[0], 4);

    nRetCode = _snprintf(
        szBuffer, sizeof(szBuffer), 
        "%d.%d.%d.%d", 
        ia.S_un.S_un_b.s_b1,
        ia.S_un.S_un_b.s_b2,
        ia.S_un.S_un_b.s_b3,
        ia.S_un.S_un_b.s_b4
    );
    KGLOG_PROCESS_ERROR(nRetCode < sizeof(szBuffer));

    m_strHost = szBuffer;

    nResult = true;
Exit0:
    return nResult;
}

int KG_Activator::_GetUserInfo()
{
    int nRetCode = false;
    std::string strValue;
    unsigned uValueLength = 0;

    strValue = m_UserInfoDialog.GetPhone();
    uValueLength = (unsigned)strValue.length();
    if (uValueLength == 0)
    {
        MessageBox(m_MainDialog, "请输入电话或手机", "提示", MB_OK);
        return false;
    }
    if (uValueLength < 6 || uValueLength > 16)
    {
        MessageBox(m_MainDialog, "电话或手机格式不正确", "提示", MB_OK);
        return false;
    }
    nRetCode = _CheckRegEx(_T("[0-9-]+"), strValue.c_str());
    if (!nRetCode)
    {
        MessageBox(m_MainDialog, "电话或手机格式不正确", "提示", MB_OK);
        return false;
    }
    m_PostParamMap[KG_POST_PARAM_PHONE] = strValue;

    strValue = m_UserInfoDialog.GetEmail();
    uValueLength = (unsigned)strValue.length();
    if (uValueLength == 0)
    {
        MessageBox(m_MainDialog, "请输入电子邮箱", "提示", MB_OK);
        return false;
    }
    if (uValueLength < 8 || uValueLength > 150)
    {
        MessageBox(m_MainDialog, "邮箱格式不正确", "提示", MB_OK);
        return false;
    }
    nRetCode = _CheckRegEx(_T("[a-zA-Z0-9_\\-\\.]+@[a-zA-Z0-9_\\-]+"), strValue.c_str());
    if (!nRetCode)
    {
        MessageBox(m_MainDialog, "电子邮箱格式不正确", "提示", MB_OK);
        return false;
    }
    m_PostParamMap[KG_POST_PARAM_EMAIL] = strValue;

    strValue = m_UserInfoDialog.GetQQ();
    uValueLength = (unsigned)strValue.length();
    if (uValueLength == 0)
    {
        MessageBox(m_MainDialog, "请输入QQ或MSN号码", "提示", MB_OK);
        return false;
    }
    nRetCode = _CheckRegEx("[0-9]+", strValue.c_str());
    if (nRetCode)
    {   // indicate QQ
        if (uValueLength < 5 || uValueLength > 11)
        {
            MessageBox(m_MainDialog, "QQ或MSN格式不正确", "提示", MB_OK);
            return false;
        }
    }
    else
    {   // indicate MSN
        nRetCode = _CheckRegEx(_T("[a-zA-Z0-9_\\-\\.]+@[a-zA-Z0-9_\\-]+"), strValue.c_str());
        if (!nRetCode)
        {
            MessageBox(m_MainDialog, "QQ或MSN格式不正确", "提示", MB_OK);
            return false;
        }
    }
    m_PostParamMap[KG_POST_PARAM_QQ] = strValue;

    return true;
}

int KG_Activator::_GetAccountInfo()
{
    std::string strValue;
    unsigned uValueLength = 0;
    char szEncodedPassword[64];

    strValue = m_AccountInfoDialog.GetPassport();
    uValueLength = (unsigned)strValue.length();
    if (uValueLength == 0)
    {
        MessageBox(m_MainDialog, "请填写通行证帐号", "提示", MB_OK);
        return false;
    }
    if (uValueLength > 32)
    {
        MessageBox(m_MainDialog, "通行证帐号长度过长", "提示", MB_OK);
        return false;
    }
    m_PostParamMap[KG_POST_PARAM_PASSPORT] = strValue;

    strValue = m_AccountInfoDialog.GetPassword();
    uValueLength = (unsigned)strValue.length();
    if (uValueLength == 0)
    {
        MessageBox(m_MainDialog, "请填写密码", "提示", MB_OK);
        return false;
    }
    StringMD5(strValue.c_str(), szEncodedPassword, sizeof(szEncodedPassword));
    m_PostParamMap[KG_POST_PARAM_PASSWORD] = szEncodedPassword;

    strValue = m_AccountInfoDialog.GetVerifyCode();
    uValueLength = (unsigned)strValue.length();
    if (uValueLength == 0)
    {
        MessageBox(m_MainDialog, "请填写验证码", "提示", MB_OK);
        return false;
    }
    if (uValueLength != 4)
    {
        MessageBox(m_MainDialog, "验证码填写错误", "提示", MB_OK);
        return false;
    }
    m_PostParamMap[KG_POST_PARAM_VERIFY_CODE] = strValue;
    
    return true;
}

int KG_Activator::_GetMachineInfo()
{
    HRESULT hRetCode = E_FAIL;
    CSystemInfo SystemInfo;
    CString strData;
    std::string strSystemInfo;
    std::string strSystemVersion;
    std::string strCPUInfo;
    std::string strMemoryInfo;
    std::string strVGAInfo;
    std::string strVGAMemory;
    std::string strUUID;
    std::string strEncryptedCode;
    std::string strCombination;

    // -------------------- Get machine information --------------------
    strData = "";
    hRetCode = SystemInfo.GetSystemVersion(strData);
    if (SUCCEEDED(hRetCode))
        strSystemInfo = (LPCTSTR)strData;
    else
        strSystemInfo = "unknown system";
    
    strData = "";
    hRetCode = SystemInfo.GetSystemId(strData);
    if (SUCCEEDED(hRetCode))
        strSystemVersion = (LPCTSTR)strData;
    else
        strSystemVersion = "0.0";

    strData = "";
    hRetCode = SystemInfo.GetSystemCpuInfo(strData);
    if (SUCCEEDED(hRetCode))
        strCPUInfo = (LPCTSTR)strData;
    else
        strCPUInfo = "unknown cpu";

    strData = "";
    hRetCode = SystemInfo.GetSystemMemInfo(strData);
    if (SUCCEEDED(hRetCode))
        strMemoryInfo = (LPCTSTR)strData;
    else
        strMemoryInfo = "2222";

    strData = "";
    hRetCode = SystemInfo.GetDisplayInfo(strData);
    if (SUCCEEDED(hRetCode))
        strVGAInfo = (LPCTSTR)strData;
    else
        strVGAInfo = "unknown vga";

    strData = "";
    hRetCode = SystemInfo.GetDisplayMem(strData);
    if (SUCCEEDED(hRetCode))
        strVGAMemory = (LPCTSTR)strData;
    else
        strVGAMemory = "255";

    strData = "";
    hRetCode = SystemInfo.GetHardDiskId(strData);
    strUUID = (LPCTSTR)strData;

    strCombination  = strSystemInfo;
    strCombination += '|';
    strCombination += strSystemVersion;
    strCombination += '|';
    strCombination += strCPUInfo;
    strCombination += '|';
    strCombination += strMemoryInfo;
    strCombination += '|';
    strCombination += strVGAInfo;
    strCombination += '|';
    strCombination += strVGAMemory;
    strCombination += '|';
    strCombination += strUUID;

    strData = "";
    hRetCode = SystemInfo.EncryptString(CString(strCombination.c_str()), strData);
    ASSERT(hRetCode);
    strEncryptedCode = (LPCTSTR)strData;
    // ------------------------------------------------------------

    m_PostParamMap[KG_POST_PARAM_SYS_INFO]    = strSystemInfo;
    m_PostParamMap[KG_POST_PARAM_SYS_VERSION] = strSystemVersion;
    m_PostParamMap[KG_POST_PARAM_CPU_INFO]    = strCPUInfo;
    m_PostParamMap[KG_POST_PARAM_MEMORY_INFO] = strMemoryInfo;
    m_PostParamMap[KG_POST_PARAM_VGA_INFO]    = strVGAInfo;
    m_PostParamMap[KG_POST_PARAM_VGA_MEMORY]  = strVGAMemory;
    m_PostParamMap[KG_POST_PARAM_UUID]        = strUUID;
    m_PostParamMap[KG_POST_PARAM_ENCRYPTED_CODE] = strEncryptedCode;
    
    return true;
}

int KG_Activator::_CheckResult()
{
    int nRetCode = false;
    int nResult = false;
    int nResultCode = 0;
    std::string strMessage;

    nRetCode = _GetMachineInfo();
    ASSERT(nRetCode);

    nRetCode = _PostDataString(
        m_strHost.c_str(),
        "/checkData.php",
        &nResultCode, 
        &strMessage
    );
    if (!nRetCode)
    {
        m_lLastPostErrorTime = time(NULL);      
        MessageBox(m_MainDialog, "系统繁忙，请稍后重试", "提示", MB_OK);
        KGLOG_PROCESS_ERROR(false);
    }

    switch (nResultCode)
    {
    case 1: // 此账号此次被成功激活
        m_AccountInfoDialog.ShowWindow(SW_HIDE);
        m_PassDialog.ShowWindow(SW_SHOW);
        break;
    case 2: // 配置不合格
        m_AccountInfoDialog.ShowWindow(SW_HIDE);
        m_FailDialog.ShowWindow(SW_SHOW);
        break;
    case 3: // 今日号发完
        m_AccountInfoDialog.ShowWindow(SW_HIDE);
        m_TodayOverDialog.ShowWindow(SW_SHOW);
        break;
    case 4: // 所有号发完
        m_AccountInfoDialog.ShowWindow(SW_HIDE);
        m_AllOverDialog.ShowWindow(SW_SHOW);
        break;
    case 5: // 此账号为防沉迷账号
        nRetCode = MessageBox(
            m_MainDialog, 
            "    对不起，您的通行证帐号为防沉迷帐号（即未绑定身份证或绑定的身份证未满18岁）。为了青少年的学业，我们谢绝未成年人。\r\n"
            "    如果您已满18岁，请点击“确定”，登陆后输入您的身份证信息",
            "提示",
            MB_OKCANCEL
        );
        if (nRetCode == IDOK)
            ShellExecute(0, NULL, _T("https://pass.kingsoft.com/"), NULL, NULL, SW_NORMAL);

        UpdateVerifyImage();
        break;     
    default:
        // 其他错误
        MessageBox(m_MainDialog, strMessage.c_str(), "提示", MB_OK);

        UpdateVerifyImage();
        break;
    }

// Exit1:
    nResult = true;
Exit0:
    return nResult;
}


int KG_Activator::_PostDataString(
    const char cszServer[],
    const char cszObject[],
    int *pnResponseCode,
    std::string *pstrRetResponseInfo
)
{
    int nResult  = false;
    int nRetCode = false;
    KG_HttpClient HttpClient;
    KG_POST_PARAM_MAP::iterator it;
    unsigned char *pbyResponseData = NULL;
    DWORD dwResponseDataSize = 0;

    ASSERT(cszServer);
    ASSERT(cszServer[0]);
    ASSERT(cszObject);
    ASSERT(cszObject[0]);
    ASSERT(pnResponseCode);
    ASSERT(pstrRetResponseInfo);

    for (it = m_PostParamMap.begin(); it != m_PostParamMap.end(); ++it)
    {
        nRetCode = HttpClient.AddPostParam(
            (*it).first.c_str(), (*it).second.c_str()
        );
        KGLOG_PROCESS_ERROR(nRetCode);           
    }

    nRetCode = HttpClient.RequestPost(
        "Mozilla/4.0 (compatible; MSIE 6.0; Windows NT 5.1)",
        cszServer,
        cszObject,
        "POST",
        INTERNET_DEFAULT_HTTP_PORT
    );
    KGLOG_PROCESS_ERROR(nRetCode);

    nRetCode = HttpClient.GetResponse(&pbyResponseData, &dwResponseDataSize);
    KGLOG_PROCESS_ERROR(nRetCode);

    *pnResponseCode = atoi((const char *)pbyResponseData);   
    *pstrRetResponseInfo = (const char*)pbyResponseData + 2;   

    nResult = true;
Exit0:
    return nResult;
}
