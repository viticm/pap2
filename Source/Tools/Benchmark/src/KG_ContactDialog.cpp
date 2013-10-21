#include "StdAfx.h"
#include "KG_ContactDialog.h"
#include <commctrl.h>
#include <string>

std::string g_strContactInfo;
int  g_nGetContactInfo = false;

int _Trim(char szStringVal[], int nInputSize)
{
    int nResult = false;
    int nIndex = 0;

    KG_PROCESS_ERROR(nInputSize >= 0);

    for (int i = nInputSize - 1; i >= 0; --i)
    {
        if (szStringVal[i] != ' ')
            break;

        szStringVal[i] = '\0';
        nInputSize --;
    }
    
    nIndex = -1;
    for (int i = 0; i < nInputSize; ++i)
    {
        if (szStringVal[i] != ' ')
        {
            nIndex = i;
            break;
        }
    }

    if (nIndex > 0)
    {
        int i = 0;
        for (NULL; nIndex < nInputSize; ++nIndex, ++i)
        {
            szStringVal[i] = szStringVal[nIndex];
        }
        szStringVal[i] = '\0';
    }

    nResult = true;
Exit0:
    return nResult;
}

int _GetContactInfo(HWND hDlg)
{
    int nResult = false;
    int nRetCode = false;
    int nHadContactInfo = false;
    char szName[64];
    char szEmail[256];
    char szQQ[16];
    char szPhone[32];
    
    g_nGetContactInfo = false;
    g_strContactInfo.clear();

    nRetCode = GetDlgItemText(hDlg, IDC_EDIT_NAME, szName, sizeof(szName));
    if (nRetCode == 0)
    {
        MessageBox(hDlg, "请输入您的姓名.", "Error", MB_ICONINFORMATION);
        goto Exit0;
    }

    nRetCode = _Trim(szName, (int)strlen(szName));
    KGLOG_PROCESS_ERROR(nRetCode);

    nRetCode = (int)strlen(szName);
    if (nRetCode >= 32) // out of 32 bytes
    {
        MessageBox(hDlg, "您输入的姓名太长了, 姓名最大长度为: 英文为32个字符, 汉字为16个.", "Error", MB_ICONINFORMATION);
        goto Exit0;
    }

    g_strContactInfo += szName;

    g_strContactInfo.push_back(',');
    nRetCode = GetDlgItemText(hDlg, IDC_EDIT_PHONE, szPhone, sizeof(szPhone));
    if (nRetCode > 0)
    {
        nRetCode = _Trim(szPhone, (int)strlen(szPhone));
        KGLOG_PROCESS_ERROR(nRetCode);

        nRetCode = (int)strlen(szPhone);
        if (nRetCode >= 20) // out of 20 bytes
        {
            MessageBox(hDlg, "您输入的电话号码太长了, 电话号码的最大字符数为20.", "Error", MB_ICONINFORMATION);
            goto Exit0;
        }

        g_strContactInfo += szPhone;

        nHadContactInfo = true;
    }

    g_strContactInfo.push_back(',');
    nRetCode = GetDlgItemText(hDlg, IDC_EDIT_EMAIL, szEmail, sizeof(szEmail));
    if (nRetCode > 0)
    {
        nRetCode = _Trim(szEmail, (int)strlen(szEmail));
        KGLOG_PROCESS_ERROR(nRetCode);

        nRetCode = (int)strlen(szEmail);
        if (nRetCode >= 128) // out of 128 bytes
        {
            MessageBox(hDlg, "您输入的Email太长了, Email的最大字符数为128.", "Error", MB_ICONINFORMATION);
            goto Exit0;
        }
    
        g_strContactInfo += szEmail;

        nHadContactInfo = true;
    }

    g_strContactInfo.push_back(',');
    nRetCode = GetDlgItemText(hDlg, IDC_EDIT_QQ, szQQ, sizeof(szQQ));
    if (nRetCode > 0)
    {
        nRetCode = _Trim(szQQ, (int)strlen(szQQ));
        KGLOG_PROCESS_ERROR(nRetCode);

        nRetCode = (int)strlen(szQQ);
        if (nRetCode >= 12) // out of 12 bytes
        {
            MessageBox(hDlg, "您输入的QQ太长了, QQ的最大字符数为12.", "Error", MB_ICONINFORMATION);
            goto Exit0;
        }

        g_strContactInfo += szQQ;

        nHadContactInfo = true;
    }

    if (!nHadContactInfo)
    {
        MessageBox(hDlg, "请输入任意一种联系方式.", "Error", MB_ICONINFORMATION);
        goto Exit0;
    }

    g_nGetContactInfo = nHadContactInfo;

    nResult = true;
Exit0:
    if (!g_nGetContactInfo)
    {
        g_strContactInfo.clear();
    }

    return nResult;
}

BOOL CALLBACK ContactDlgProc(HWND hDlg, UINT message,WPARAM wParam, LPARAM lParam)
{
    int nRetCode = false;

    KG_USE_ARGUMENT(lParam);

	switch (message)
	{
	case WM_INITDIALOG:
		return TRUE ;

	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDOK:
            nRetCode = _GetContactInfo(hDlg);
            if (nRetCode)
            {
                EndDialog(hDlg, TRUE);
            }
			return TRUE;

        case IDCANCEL:
            EndDialog(hDlg, TRUE);
			return TRUE;
		}
		break;
	}
	return  FALSE;
}

////////////////////////////////////////////////////////////////////////////////
int ShowContactDialog(char szContactInfo[], int nSize)
{
    int nResult  = false;
    int nRetCode = false;
    HINSTANCE hInstance;
    INITCOMMONCONTROLSEX InitCtrls;

    KGLOG_PROCESS_ERROR(nSize > 0);

    hInstance = (HINSTANCE)::GetModuleHandle(NULL);
    KGLOG_PROCESS_ERROR(hInstance);

    // InitCommonControlsEx() is required on Windows XP if an application
    // manifest specifies use of ComCtl32.dll version 6 or later to enable
    // visual styles.  Otherwise, any window creation will fail.
    InitCtrls.dwSize = sizeof(InitCtrls);
    // Set this to include all the common control classes you want to use
    // in your application.
    InitCtrls.dwICC = ICC_WIN95_CLASSES;
    nRetCode = InitCommonControlsEx(&InitCtrls);
    KGLOG_PROCESS_ERROR(nRetCode);

    g_strContactInfo.clear();

    nRetCode = (int)DialogBox(hInstance, MAKEINTRESOURCE(IDD_CONTACT), NULL, ContactDlgProc);
    KGLOG_PROCESS_ERROR(nRetCode == TRUE);

    strncpy(szContactInfo, g_strContactInfo.c_str(), nSize);

    szContactInfo[nSize - 1] = '\0';

    nResult = true;
Exit0:
    return nResult;
}
