#include <windows.h>
#include <tchar.h>
#include "WndTree.h"
#include "UiEditor.h"
#include "Ui.h"
#include "Engine.h"
#include "UiWndImageCommon.h"

KWndTree	g_WndTree;

KWndLeaf::KWndLeaf()
{
	m_Name[0] = 0;
	m_pWndObject = NULL;
	m_pPre = m_pNext = NULL;
	m_Type = UI_WND_WINDOW;
    m_bIgnoreWndObject = FALSE;
}

KWndLeaf::KWndLeaf(UI_WND_TYPE Type)
{
	m_Name[0] = 0;
	m_pWndObject = NULL;
	m_pPre = m_pNext = NULL;
	m_Type = Type;
    m_bIgnoreWndObject = FALSE;
}

KWndLeaf::~KWndLeaf()
{
	if (m_pPre)
		m_pPre->m_pNext = m_pNext;
	if (m_pNext)
		m_pNext->m_pPre = m_pPre;
	m_pPre = m_pNext = NULL;
    if (!m_bIgnoreWndObject) 
    {
        ReleaseObject();
    }
}

void KWndLeaf::MoveUp()
{
	if (m_pNext)
	{
		m_pPre->m_pNext = m_pNext;
		m_pNext->m_pPre = m_pPre;
		m_pNext = m_pNext->m_pNext;
		m_pPre = m_pNext;
		m_pPre->m_pNext = this;
		m_pNext->m_pPre = this;
	}
}

void KWndLeaf::Init(IIniFile* pIni)
{
	if (!m_pWndObject || !pIni || !m_Name[0])
		return;

	Plugin::KClientUIInitWndParam	Param;
	Param.pIniFile = pIni;
	_tcsncpy(Param.szSection, m_Name, sizeof(Param.szSection));
	g_pfnClientBehavior(Plugin::IPluginShell::PCB_UI_INIT_WINDOW, (WPARAM)m_pWndObject, (LPARAM)&Param);
}

void KWndLeaf::SetPos(int x, int y)
{
	if (m_pWndObject)
	{
		g_pfnClientBehavior(Plugin::IPluginShell::PCB_UI_WINDOW_POS, (WPARAM)m_pWndObject, MAKELPARAM(x, y));
	}
}

int KWndLeaf::GetPosFromDawn(int& x, int& y)
{
	x = y = 0;
	if (m_pWndObject)
	{
		LRESULT l = g_pfnClientBehavior(Plugin::IPluginShell::PCB_UI_WINDOW_GET_POS, (WPARAM)m_pWndObject, 0);
		x = LOWORD(l);
		y = HIWORD(l);
		return true;
	}
	return false;
}

void KWndLeaf::SetSize(int w, int h)
{
	if (m_pWndObject)

	{
		g_pfnClientBehavior(Plugin::IPluginShell::PCB_UI_WINDOW_SIZE, (WPARAM)m_pWndObject, MAKELPARAM(w, h));
	}
}

void KWndLeaf::Append(KWndLeaf* pLeaf)
{
	KWndLeaf* pEnd = this;
	while(pEnd->m_pNext)
		pEnd = pEnd->m_pNext;
	pEnd->m_pNext = pLeaf;
	pLeaf->m_pPre = pEnd;
}

KWndLeaf* KWndLeaf::FindName(const char* pszName)
{
	KWndLeaf* pWnd = this;
	while(pWnd)
	{
		if (!strcmp(pWnd->m_Name, pszName))
			break;
		pWnd = pWnd->m_pNext;
	}
	return pWnd;
}

void KWndLeaf::ReleaseObject()
{
	if (m_pWndObject)
	{
		g_pfnClientBehavior(Plugin::IPluginShell::PCB_UI_DELETE_WINDOW, (WPARAM)m_pWndObject, 0);
		m_pWndObject = NULL;
	}
}

int	KWndLeaf::CreateObject(IIniFile* pIni)
{
	ReleaseObject();

	m_pWndObject = (PVOID)g_pfnClientBehavior(Plugin::IPluginShell::PCB_UI_CREATE_WINDOW, (WPARAM)pIni, (LPARAM)m_Name);

	return (m_pWndObject != NULL);
}

void KWndLeaf::AttachToHost()
{
	if (m_pWndObject)
	{
		g_pfnClientBehavior(Plugin::IPluginShell::PCB_UI_WINDOW_ATTACH,(WPARAM)m_pWndObject, 0);
	}
}

void KWndLeaf::AddChild(KWndLeaf* pChild)
{
	if (m_pWndObject && pChild->m_pWndObject)
	{
		g_pfnClientBehavior(Plugin::IPluginShell::PCB_UI_WINDOW_ADD_CHILD, (WPARAM)m_pWndObject, (LPARAM)pChild->m_pWndObject);
	}
}

void KWndLeaf::SplitFromTree()
{
}

bool KWndLeaf::GetWndTreePath(LPTSTR szTreePath, int nLength)
{
	if (szTreePath == NULL || nLength <= 0)
		return false;

	if (m_pWndObject == NULL)
		return false;

	Plugin::KClientUIString sString;
	sString.szBuffer = szTreePath;
	sString.nLength = nLength;

	if (!g_pfnClientBehavior(Plugin::IPluginShell::PCB_UI_GET_WND_TREE_PATH, (WPARAM)m_pWndObject, (LPARAM)&sString))
		return false;

	return true;
}

void KWndLeaf::BindWindowObject(void *pObjiect)
{
    if (pObjiect) 
    {
        m_pWndObject = pObjiect;
    }
}


KWndTree::KWndTree()
{
    m_pRootWnd = NULL;
    ZeroMemory(&m_Root, sizeof(KWndLeaf));
}

KWndTree::~KWndTree()
{
}

KWndLeaf*	KWndTree::FindWnd(const char* pszName)
{
	if (pszName && pszName[0])
		return m_Root.FindName(pszName);
	return NULL;
}

void	KWndTree::RemoveWnd(const char* pszName)
{
	KWndLeaf* pWnd = FindWnd(pszName);
	if (pWnd)
		delete pWnd;
}

KWndLeaf*	KWndTree::CreateWnd(UI_WND_TYPE eType, const char* pszName, IIniFile* pIni)
{
	if (!pszName || !pszName[0])
		return NULL;

	KWndLeaf* pNew = new KWndLeaf(eType);
	while (pNew)
	{
		strcpy(pNew->m_Name, pszName);
		if(!pNew->CreateObject(pIni))
			break;
		pNew->Init(pIni);
		m_Root.Append(pNew);
		return pNew;
	};
	if (pNew)
		delete pNew;
	return NULL;
}

int KWndTree::UpdateShowWndWithBindSection(IIniFile* pIni)
{
    int nRetCode = FALSE;
    int nResult  = FALSE;

    int nWndCount = 0;
    char szWndName[128];
    szWndName[0] = 0;
    char szNextWndName[128];
    szNextWndName[0] = 0;
    char szTypeName[128];
    szTypeName[0] = 0;
    char szParentName[128];
    szParentName[0] = 0;
    char szSpetialParentUnit[128] = "";
    int i = 0;
    struct WNDBASEDATA 
    {
        char szName[128];
        UI_WND_TYPE nType;
    };
    KWndLeaf **ppNewLeafTable = NULL;
    WNDBASEDATA *pBaseData = NULL;
    void     **ppLeafObjectTable = NULL;
    int      nHasTopParent = FALSE;


    //判断是否频繁调用,太频繁调用是因为调用Updatadata时产生的，这种情况只调用第一次。
    static DWORD dwLastTime = 0;
    DWORD dwTimeNow = GetTickCount();
    if (dwTimeNow - dwLastTime < 100) 
    {
        dwLastTime = dwTimeNow;
        KG_PROCESS_ERROR(0);
    }
    else
    {
        dwLastTime = dwTimeNow;
    }


    //安全检查
    nRetCode = SafeCheck(pIni);
    KG_PROCESS_ERROR(nRetCode);

    //清除树里的所有内容。
    Clear();

    //得到ini下所有的窗口名和窗口数目。
    szWndName[0] = 0;
    szNextWndName[0] = 0;
    nWndCount = 0;
    while (pIni->GetNextSection(szWndName, szNextWndName)) 
    {
        nWndCount++;
        strcpy(szWndName, szNextWndName); 
    }

    if (nWndCount != 0) 
        pBaseData = new WNDBASEDATA[nWndCount];
    KG_PROCESS_ERROR(pBaseData);

    szWndName[0] = 0;
    szNextWndName[0] = 0;
    szTypeName[0] = 0;
    i = 0;
    while (pIni->GetNextSection(szWndName, szNextWndName)) 
    {
        strcpy(pBaseData[i].szName, szNextWndName);
        pIni->GetString(szNextWndName, "._WndType", "", szTypeName, sizeof(szTypeName));
        g_GetWndType(szTypeName, pBaseData[i].nType);
        if (pBaseData[i].nType == UI_WND_FRAME) 
        {
            KG_PROCESS_ERROR(nHasTopParent == FALSE);
            nHasTopParent = TRUE;
            strcpy(szSpetialParentUnit, szNextWndName);
        }
        i++;
        strcpy(szWndName, szNextWndName); 
    }
    KG_PROCESS_ERROR(nHasTopParent);

    //建立一个用于保存客户端返回所有窗口指针的指针数组。
    ppNewLeafTable = new KWndLeaf* [nWndCount];
    KG_PROCESS_ERROR(ppNewLeafTable);

    for (i = 0; i < nWndCount; i++) 
    {
        ppNewLeafTable[i] = new KWndLeaf(pBaseData[i].nType);
        KG_PROCESS_ERROR(ppNewLeafTable[i]);
        strcpy(ppNewLeafTable[i]->m_Name, pBaseData[i].szName);
    }
    
    ppLeafObjectTable = new void* [nWndCount];
    KG_PROCESS_ERROR(ppLeafObjectTable);

    //通知客户端建立所有显示窗口，并把窗口指针保存在上面建立的指针数组里面。
 	nRetCode = g_pfnClientBehavior(Plugin::IPluginShell::PCB_UI_BIND_WINDOW, (WPARAM)ppLeafObjectTable, (LPARAM)pIni);  
    KG_PROCESS_ERROR(nRetCode);


    //把所有窗口添加到root里面。找出m_pRootWnd
    for (i = 0; i < nWndCount; i++) 
    {
        ppNewLeafTable[i]->BindWindowObject(ppLeafObjectTable[i]);
         m_Root.Append(ppNewLeafTable[i]);
    }
    m_pRootWnd = FindWnd(szSpetialParentUnit);

    nRetCode = TRUE;
    nResult = TRUE;
Exit0:
    if (ppNewLeafTable) 
    {
        delete[] ppNewLeafTable;
        ppNewLeafTable = NULL;
    }
    if ((nRetCode == FALSE) && (ppNewLeafTable != NULL)) 
    {
        for (i = 0; i < nWndCount; i++) 
        {
            if (ppNewLeafTable[i]) 
            {
                delete ppNewLeafTable[i];
                ppNewLeafTable[i] = NULL;
            }
        }
    }
    if (ppNewLeafTable != NULL) 
    {
        delete[] ppNewLeafTable;
    }
    if (pBaseData != NULL) 
    {
        delete[] pBaseData;
    }

    return nResult;
}

int KWndTree::SafeCheck(IIniFile *pIni)
{
    int nResult = FALSE;
    int nRetCode = FALSE;

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
                    sprintf(szPageKeyName, "Button_%d", i);
                    pIni->GetString(szNextUnitName, szPageKeyName, "", szPageName, sizeof(szPageName));
                    nRetCode = SafeCheckUnitKey(pIni, szNextUnitName, szPageKeyName, szPageName);
                    KG_PROCESS_ERROR(nRetCode);
                }

            }
            break;
        case UI_WND_SCROLLMESSAGELISTBOX:
        case UI_WND_SCROLLTREELIST:
        case UI_WND_SCROLLRICHTEXT:
            {
                char szBuffer[128] = "";
                pIni->GetString(szNextUnitName, "ScrollerSectionName", "", szBuffer, sizeof(szBuffer));
                nRetCode = SafeCheckUnitKey(pIni, szNextUnitName, "滚动条", szBuffer);
                KG_PROCESS_ERROR(nRetCode);

                pIni->GetString(szNextUnitName, "ScrolleeSectionName", "", szBuffer, sizeof(szBuffer));
                nRetCode = SafeCheckUnitKey(pIni, szNextUnitName, "被滚动窗口", szBuffer);
                KG_PROCESS_ERROR(nRetCode);
            }
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


int KWndTree::SafeCheckUnitKey(IIniFile *pIni, LPCTSTR pszUnit, LPCTSTR pszKey, LPCTSTR pszKeyValue)
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

void KWndTree::Clear()
{
	KWndLeaf* pWnd = NULL;

	while(pWnd = m_Root.GetNext())
	{   
        if (pWnd == m_pRootWnd) 
        {
            void *pObject = NULL;
            g_pfnClientBehavior(Plugin::IPluginShell::PCB_UI_GET_ROOT_WND_PTR, (WPARAM)m_pRootWnd->m_Name, (LPARAM)&pObject);  
            if (pObject) 
            {
                g_pfnClientBehavior(Plugin::IPluginShell::PCB_UI_DELETE_WINDOW, (WPARAM)pObject, 0);  
            }

        }

        pWnd->IgnoreWndObject();
		delete pWnd;
	}
}

void KWndTree::UpdateShowWnd(const char* pszName, IIniFile* pIni)
{
	KWndLeaf* pWnd = FindWnd(pszName);

	pWnd->Init(pIni);

}

void KWndTree::UpdateAllWndPosFormDawn()
{
	if (!g_Ui.GetIni())
		return;
	KWndLeaf* pWnd = &m_Root;
	while ((pWnd = pWnd->GetNext()) != NULL)             
	{
		int	x, y;
		if (pWnd->GetPosFromDawn(x, y))
		{
			g_Ui.GetIni(true)->WriteInteger(pWnd->m_Name, "Left", x);
			g_Ui.GetIni()->WriteInteger(pWnd->m_Name, "Top", y);
		}
	};
}

void KWndTree::BuildTree()
{
	if (!g_Ui.GetIni())
		return;
	KWndLeaf	*pWnd = &m_Root, *pMain = NULL;
	bool bFirstMain = true;

	while(pWnd = pWnd->GetNext())
	{
		char	szParent[64];
		g_Ui.GetIni()->GetString(pWnd->m_Name, "._Parent", "", szParent, sizeof(szParent));
		if (szParent[0])
		{
			KWndLeaf* pParentWnd = FindWnd(szParent);
			if (pParentWnd)
			{
				pParentWnd->AddChild(pWnd);
			}
			else if (bFirstMain)
			{
				pWnd->AttachToHost();
				pMain = pWnd;
				bFirstMain = false;
			}
		}
		else if (pMain)
		{
			pMain->AddChild(pWnd);
		}
	}
}


bool KWndTree::GetWndTreePath(LPTSTR szTreePath, int nLength, LPCTSTR szName)
{
	if (szName == NULL || szName[0] == _T('\0'))
		return false;

	KWndLeaf* pLeaf = FindWnd(szName);
	if (pLeaf == NULL)
		return false;

	if (!pLeaf->GetWndTreePath(szTreePath, nLength))
		return false;

	return true;
}

