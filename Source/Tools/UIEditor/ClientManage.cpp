#include "Stdafx.h"
#include "ClientManage.h"
#include "UiEditor.h"
#include "Ui.h"
#include "UiWndImageCommon.h"


KUiClientManage g_ClientManager;


KUiClientManage::KUiClientManage()
{
    m_szFrameName[0]    = '\0';
    m_szFocusWndName[0] = '\0';
    m_nBindScript       = false;
    m_nUpDataData       = true;
    m_nClientSelWnd     = false;
}

KUiClientManage::~KUiClientManage()
{

}

void KUiClientManage::Clear()
{
    g_pfnUIBehavior(PCB_UI_DELETE_FRAME, (WPARAM)m_szFrameName, 0);
    m_szFrameName[0] = '\0';
}

void KUiClientManage::EnableUpDataData(int nFlag)
{
    m_nUpDataData = nFlag;
}

void KUiClientManage::UpDataWndInClient(IIniFile *pIni)
{
    int nRetCode = false;

    char szFrame[128] = "";
    char szScriptFile[MAX_PATH] = "";
 
    //判断是否频繁调用,太频繁调用是因为调用Updatadata时产生的，这种情况只调用第一次。
    KG_PROCESS_ERROR(m_nUpDataData);

    KG_PROCESS_ERROR(pIni);

    nRetCode = SafeCheck(pIni);
    KG_PROCESS_ERROR(nRetCode);

    Clear();    
    nRetCode = KUiEditor::GetFrameWndName(pIni, szFrame);
    KG_PROCESS_ERROR(nRetCode);
    KG_PROCESS_ERROR(szFrame[0]);

	pIni->GetString(szFrame, "ScriptFile", "", szScriptFile, sizeof(szScriptFile));
    if(m_nBindScript)
		g_pfnUIBehavior(PCB_UI_CREATE_FRAME, (WPARAM)pIni, (LPARAM)szScriptFile);
	else
		g_pfnUIBehavior(PCB_UI_CREATE_FRAME, (WPARAM)pIni, (LPARAM)0);
    strcpy(m_szFrameName, szFrame);
    
Exit0:
    return;
}

void KUiClientManage::BindScript(int nBindFlag, IIniFile *pIni)
{
    m_nBindScript = nBindFlag;
    UpDataWndInClient(pIni);
}

int KUiClientManage::SafeCheck(IIniFile *pIni)
{
    int nResult = FALSE;
    int nRetCode = false;

    KG_PROCESS_ERROR(pIni);

    int nValue = 0;
    char szUnitName[128] = "";
    char szNextUnitName[128] = "";
    char szType[128] = "";
    UI_WND_TYPE eType = UI_WND_WINDOW;

    while (pIni->GetNextSection(szUnitName, szNextUnitName)) 
    {
        pIni->GetString(szNextUnitName, "._WndType", "", szType, sizeof(szType));
        g_GetWndType(szType, eType);

        switch(eType) 
        {
        case UI_WND_SCROLLBAR:
            {
                int nType      = 0;
                int nEnd       = 0;
                int nButtonLen = 0;
                int nFrame     = 0;
                char szButtonName[128] = "";
                pIni->GetInteger(szNextUnitName, "Type", 0, &nType);
                pIni->GetString(szNextUnitName, "ThumbButtonSectionName", "", szButtonName, sizeof(szButtonName));
                if (nType == 0) 
                {
                    pIni->GetInteger(szButtonName, "Width", 0, &nButtonLen);
                    if (nButtonLen <= 0) 
                    {
                        SIZE sizeImg = { 0, 0 };
                        char szImgPath[MAX_PATH] = "";
                        pIni->GetString(szButtonName, "Image", "", szImgPath, sizeof(szImgPath));
                        pIni->GetInteger(szButtonName, "Frame", -1, &nFrame);
                        KUiWndImageCommon::GetImageSize(szImgPath, nFrame, &sizeImg);
                        nButtonLen = sizeImg.cx;
                    }
                }
                else
                {
                    pIni->GetInteger(szButtonName, "Height", 0, &nButtonLen);
                    if (nButtonLen <= 0) 
                    {
                        SIZE sizeImg = { 0, 0 };
                        char szImgPath[MAX_PATH] = "";
                        pIni->GetString(szButtonName, "Image", "", szImgPath, sizeof(szImgPath));
                        pIni->GetInteger(szButtonName, "Frame", -1, &nFrame);
                        KUiWndImageCommon::GetImageSize(szImgPath, nFrame, &sizeImg);
                        nButtonLen = sizeImg.cy;
                    }
                }

                pIni->GetInteger(szNextUnitName, "SlideEnd", 0, &nEnd);

                if (nEnd < nButtonLen) 
                {
                    KUiEditor::ms_ErrorInfomation.bHasError  = TRUE;
                    sprintf(KUiEditor::ms_ErrorInfomation.szErrorMsg, "!!!Error: %s:%s", szNextUnitName, "滚动条属性属性->结束位置的值过小"); 
                    KG_PROCESS_ERROR(0);
                }
                
            }
        	break;
        case UI_WND_PAGESET:
            {
                int nPageCount = 0;
                int i = 0;
                char szPageName[128] = "";
                char szPageKeyName[128] = "";
                pIni->GetInteger(szNextUnitName, "PageCount", 0, &nPageCount);
                for (i = 0; i < nPageCount; i++) 
                {
                    sprintf(szPageKeyName, "Page_%d", i);
                    pIni->GetString(szNextUnitName, szPageKeyName, "", szPageName, sizeof(szPageName));
                    nRetCode = SafeCheckUnitKey(pIni, szNextUnitName, szPageKeyName, szPageName);
                    KG_PROCESS_ERROR(nRetCode);
                    sprintf(szPageKeyName, "CheckBox_%d", i);
                    pIni->GetString(szNextUnitName, szPageKeyName, "", szPageName, sizeof(szPageName));
                    nRetCode = SafeCheckUnitKey(pIni, szNextUnitName, szPageKeyName, szPageName);
                    KG_PROCESS_ERROR(nRetCode);
                }

            }
            break;
        default:
            break;
        }
        strcpy(szUnitName, szNextUnitName);
    }
    

    KUiEditor::ms_ErrorInfomation.bHasError = FALSE;

    nResult = TRUE;
Exit0:
    return nResult;
}


int KUiClientManage::SafeCheckUnitKey(IIniFile *pIni, LPCTSTR pszUnit, LPCTSTR pszKey, LPCTSTR pszKeyValue)
{
    int nResult = FALSE;
    KG_PROCESS_ERROR(pIni);
    KG_PROCESS_ERROR(pszUnit);

    if (!pszKeyValue[0]) 
    {
        KUiEditor::ms_ErrorInfomation.bHasError  = TRUE;
        sprintf(KUiEditor::ms_ErrorInfomation.szErrorMsg, "!!!Error: %s:%s:%s", pszUnit, pszKey, "值为空！"); 
        KG_PROCESS_ERROR(0);
    }
    else if (!pIni->IsSectionExist(pszKeyValue)) 
    {
        KUiEditor::ms_ErrorInfomation.bHasError  = TRUE;
        sprintf(KUiEditor::ms_ErrorInfomation.szErrorMsg, "!!!Error: %s:%s:%s", pszUnit, pszKey, " 不存在！"); 
        KG_PROCESS_ERROR(0);
    }

    nResult = TRUE;
Exit0:
    return nResult;

}

int KUiClientManage::GetPositonFromClient(IN LPCSTR pszSectionName, OUT int *pnX, OUT int *pnY)
{
    int nResult         = false;
    int nRetCode        = false;
    LRESULT lRetCode    = false;
    short nX            = 0;
    short nY            = 0;
    char szType[128];
    int nType = UI_WND_WINDOW;

    KG_PROCESS_ERROR(pszSectionName);
    KG_PROCESS_ERROR(pszSectionName[0]);
    KG_PROCESS_ERROR(g_Ui.GetIni()->IsSectionExist(pszSectionName));

    g_Ui.GetIni()->GetString(pszSectionName, "._WndType", "", szType, sizeof(szType));
    nType = g_GetWndType(szType);
    if (nType >= UI_ITEM_NULL && nType <= UI_ITEM_TREE_LEAFE)
    {
        char szTreePath[MAX_PATH];
        char szItemTreePath[MAX_PATH];
        char szWnd[128];

        szTreePath[0] = '\0';
        szItemTreePath[0] = '\0';
        szWnd[0] = '\0';

        g_Ui.GetItemOwnerWndName(szWnd, sizeof(szWnd), pszSectionName);
        g_Ui.GetUnitTreePath(szTreePath, sizeof(szTreePath), szWnd);
        g_Ui.GetItemTreePath(szItemTreePath, sizeof(szItemTreePath), pszSectionName);

        lRetCode = g_pfnUIBehavior(PCB_UI_WINDOW_GET_POS, (WPARAM)szTreePath, (LPARAM)szItemTreePath);
    }
    else
    {
        char szTreePath[MAX_PATH];
        g_Ui.GetUnitTreePath(szTreePath, sizeof(szTreePath), pszSectionName);

        lRetCode = g_pfnUIBehavior(PCB_UI_WINDOW_GET_POS, (WPARAM)szTreePath, (LPARAM)NULL);
    }

    KG_PROCESS_ERROR(lRetCode != 0xFFFFFFFF);
    
    nX = LOWORD(lRetCode);
    nY = HIWORD(lRetCode);
    if(pnX)
        *pnX = nX;
    if(pnY)
        *pnY = nY;

    nResult = true;
Exit0:
    return nResult;
}

int KUiClientManage::GetSizeFromClient(
    IN LPCSTR pszSectionName, OUT int *pnWidth, OUT int *pnHeight 
)
{
    int nResult         = false;
    int nRetCode        = false;
    LRESULT lRetCode    = false;
    short nWidth        = 0;
    short nHeight       = 0;
    char szType[128];
    int nType = UI_WND_WINDOW;

    KG_PROCESS_ERROR(pszSectionName);
    KG_PROCESS_ERROR(pszSectionName[0]);
    KG_PROCESS_ERROR(g_Ui.GetIni()->IsSectionExist(pszSectionName));

    g_Ui.GetIni()->GetString(pszSectionName, "._WndType", "", szType, sizeof(szType));
    nType = g_GetWndType(szType);
    if (nType >= UI_ITEM_NULL && nType <= UI_ITEM_TREE_LEAFE)
    {
        char szTreePath[MAX_PATH];
        szTreePath[0] = '\0';
        char szItemTreePath[MAX_PATH];
        szItemTreePath[0] = '\0';
        char szWnd[128];
        szWnd[0] = '\0';
        g_Ui.GetItemOwnerWndName(szWnd, sizeof(szWnd), pszSectionName);
        g_Ui.GetUnitTreePath(szTreePath, sizeof(szTreePath), szWnd);
        g_Ui.GetItemTreePath(szItemTreePath, sizeof(szItemTreePath), pszSectionName);
        lRetCode = g_pfnUIBehavior(
            PCB_UI_WINDOW_GET_SIZE, 
            (WPARAM)szTreePath, (LPARAM)szItemTreePath
        );
    }
    else
    {
        char szTreePath[MAX_PATH];
        g_Ui.GetUnitTreePath(szTreePath, sizeof(szTreePath), pszSectionName);
        lRetCode = g_pfnUIBehavior(
            PCB_UI_WINDOW_GET_SIZE, 
            (WPARAM)szTreePath, (LPARAM)NULL
        );
    }
    
    nWidth = LOWORD(lRetCode);
    nHeight = HIWORD(lRetCode);
    if(pnWidth)
        *pnWidth = nWidth;
    if(pnHeight)
        *pnHeight = nHeight;

    nResult = true;
Exit0:
    return nResult;
}

int KUiClientManage::GetAllItemSizeAndPos()
{
    int nResult = false;
    int nRetCode = false;
    char szSection[128];
    szSection[0] = '\0';
    char szkey[128];
    szkey[0] = '\0';
    int x, y, w, h;

    while (g_Ui.GetNextUnit(szSection))
    {
        nRetCode = g_Ui.GetIni()->GetInteger(szSection, "Left", 0, &x);
        if (nRetCode)
        {
            nRetCode = GetPositonFromClient(szSection, &x, &y);
            if (nRetCode)
            {
                g_Ui.GetIni()->WriteInteger(szSection, "Left", x);
                g_Ui.GetIni()->WriteInteger(szSection, "Top", y);
            }
        }
        nRetCode = g_Ui.GetIni()->GetInteger(szSection, "Width", 0, &w);
        if (nRetCode)
        {
            nRetCode = GetSizeFromClient(szSection, &w, &h);
            if (nRetCode)
            {
                g_Ui.GetIni()->WriteInteger(szSection, "Width", w);
                g_Ui.GetIni()->WriteInteger(szSection, "Height", h);
            }
        }
    }

    nResult = true;
//Exit0:
    return nResult;
}

int KUiClientManage::IsFocusWndChangedInClient(OUT char *pszChangedName)
{
    int nResult     = false;
    int nRetCode    = false;


    KG_PROCESS_ERROR(m_nClientSelWnd);

    g_pfnUIBehavior(
        PCB_UI_GET_FOCUS_WND_NAME, 
        (WPARAM)pszChangedName, (LPARAM)128
    );

    KG_PROCESS_ERROR(_tcsicmp(pszChangedName, m_szFocusWndName) != 0);

    strcpy(m_szFocusWndName, pszChangedName);

    nResult = true;
Exit0:
    return nResult;
}

int KUiClientManage::SetClientChooseWnd(int nFlag)
{
    m_nClientSelWnd = nFlag;

    return nFlag;
}

int KUiClientManage::ReloadImageFromFile()
{
	g_pfnUIBehavior(PCB_UI_RELOAD_ALL_IMAGE, (WPARAM)0, (LPARAM)0);
	return true;
}