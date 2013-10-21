#include "Stdafx.h"
#include "Ui.h"
#include "UiEditor.h"
#include "UiWndData.h"

KUi	g_Ui;

static LPCTSTR gs_pszFrameLayerTable[LAYER_END][2] = 
{
    {"Lowest",			"最低层"    },
	{"Lowest1",			"最低层1"    },
	{"Lowest2",			"最低层2"    },
    {"Normal",			"中间层"    },
	{"Normal1",			"中间层1"    },
	{"Normal2",			"中间层2"    },
    {"Topmost",			"最高层"   },
	{"Topmost1",		"最高层1"   },
	{"Topmost2",		"最高层2"   },
    {"Texture",			"渲染到场景贴图"    },
};

FRAMELAYERTYPE g_GetFrameLayerType(LPCTSTR pszLayer)
{
    int i;
    for (i = LAYER_BEGIN; i < LAYER_END; i++) 
    {
        if (_tcsicmp(pszLayer, gs_pszFrameLayerTable[i][0]) == 0) 
            return FRAMELAYERTYPE(i);
    }
    
    return LAYER_UNKNOWN;
}

LPCTSTR g_GetFrameLayerName(int nLayer)
{
    if (nLayer < LAYER_BEGIN || nLayer >= LAYER_END) 
        return NULL;
    else
        return gs_pszFrameLayerTable[nLayer][0];
}

LPCTSTR g_GetFrameLayerLayoutString(int nLayer)
{
    if (nLayer < LAYER_BEGIN || nLayer >= LAYER_END) 
        return NULL;
    else
        return gs_pszFrameLayerTable[nLayer][1];
}

BOOL g_IsStringFrameLayer(LPCTSTR pszName)
{
    return g_GetFrameLayerType(pszName) != LAYER_UNKNOWN;
}



KUi::KUi()
{
	m_Modified = false;
	m_szIniFile[0] = 0;
    m_pIniFile = g_CreateIniFile();
}

KUi::~KUi()
{
	Clear();
}

int	KUi::Load(const char* pszIni)
{
	if (!pszIni || !pszIni[0])
		return false;
	Clear();

    m_pIniFile = g_OpenIniFile(pszIni);
    if (m_pIniFile == NULL)
		return false;
    KUiEditor::NormalizeIniData(m_pIniFile);

	strcpy(m_szIniFile, pszIni);
	return true;
}

int KUi::Save(const char* pszIni)
{
	if (!pszIni || !pszIni[0])
		return false;
	if (!m_Modified && !stricmp(pszIni, m_szIniFile))
		return true;
    if (m_pIniFile == NULL) 
        return false;
	if (!m_pIniFile->Save(pszIni))
		return false;
	Clear();
	Load(pszIni);
//	m_Modified = false;
//	strcpy(m_szIniFile, pszIni);
	return true;
}

int	KUi::New()
{
	Clear();
    if (m_pIniFile) 
    {
        m_pIniFile->Clear();
    }

	return 1;
}

void KUi::Clear()
{
	m_Modified = false;
	m_szIniFile[0] = 0;
    if (m_pIniFile) 
    {
	    m_pIniFile->Clear();
    }

}

int KUi::GetNextUnit(char* pszUnit)
{
	if (!pszUnit)
		return false;
	char szNextSection[128] = "";
    if (m_pIniFile == NULL)
        return false;
	while (m_pIniFile->GetNextSection(pszUnit, szNextSection))
	{
		strcpy(pszUnit, szNextSection);
		return true;
	}
	return false;
}

int	KUi::GetUnitBaseData(const char* pszUnit, KUiWndWindowData& data)
{
	if (!pszUnit || !pszUnit[0])
		return false;
    if (m_pIniFile == NULL) 
    return false;
	if (!m_pIniFile->IsSectionExist(pszUnit))
		return false;
	char	szType[32], szBuffer[64];
	m_pIniFile->GetString(pszUnit, "._WndType", "", szType, sizeof(szType));
	m_pIniFile->GetString(pszUnit, "._Comment", "", data.Comment, sizeof(data.Comment));
	
	if (!m_pIniFile->GetString(pszUnit, "._Parent", "", data.Parent, sizeof(data.Parent)))
	{
		char szSection[64] = "";
		m_pIniFile->GetNextSection("", szSection);
		if (strcmp(szSection, pszUnit))
			strcpy(data.Parent, szSection);
	}

	m_pIniFile->GetInteger(pszUnit, "Left", 0, &data.Left);
	m_pIniFile->GetInteger(pszUnit, "Top", 0, &data.Top);
	m_pIniFile->GetInteger(pszUnit, "Width", 0, &data.Width);
	m_pIniFile->GetInteger(pszUnit, "Height", 0, &data.Height);

	if (g_GetWndType(szType, data.Type))
		return true;
	if (m_pIniFile->GetString(pszUnit, "Image", "", szBuffer, sizeof(szBuffer)))
		data.Type = UI_WND_IMAGE;
	int	nValue;
	if (m_pIniFile->GetInteger(pszUnit, "Up", 0, &nValue) ||
		m_pIniFile->GetInteger(pszUnit, "Down", 0, &nValue))
	{
		data.Type = UI_WND_BUTTON;
	}
	else if ((m_pIniFile->GetInteger(pszUnit, "Min", 0, &nValue) && m_pIniFile->GetInteger(pszUnit, "Max", 0, &nValue)) ||
		(m_pIniFile->GetInteger(pszUnit, "SlideBegin", 0, &nValue) && m_pIniFile->GetInteger(pszUnit, "SlideEnd", 0, &nValue)))
	{
		data.Type = UI_WND_SCROLLBAR;
	}
	return true;
}

int	KUi::SetUnitBaseData(const char* pszUnit, const KUiWndWindowData& data)
{
	if (!pszUnit || !pszUnit[0])
		return false;
    if (m_pIniFile == NULL)
        return false;
	char	szType[32];
	strcpy(szType, g_GetWndTypeKey(data.Type));
	if (szType[0])
		m_pIniFile->WriteString(pszUnit, "._WndType", szType);
	else
		m_pIniFile->EraseKey(pszUnit, "._WndType");
	if (data.Comment[0])
		m_pIniFile->WriteString(pszUnit, "._Comment", data.Comment);
	else
		m_pIniFile->EraseKey(pszUnit, "._Comment");
	if (data.Parent[0])
		m_pIniFile->WriteString(pszUnit, "._Parent", data.Parent);
	else
		m_pIniFile->EraseKey(pszUnit, "._Parent");
	m_pIniFile->WriteInteger(pszUnit, "Left",  data.Left);
	m_pIniFile->WriteInteger(pszUnit, "Top",   data.Top);
	m_pIniFile->WriteInteger(pszUnit, "Width", data.Width);
	m_pIniFile->WriteInteger(pszUnit, "Height",data.Height);

	m_Modified = true;
	return true;
}

void KUi::RemoveUnit(const char* pszUnit)
{
	if (!pszUnit || !pszUnit[0])
		return;
    if (m_pIniFile == NULL)
        return;
	int	x, y, x2, y2;
	m_pIniFile->GetInteger(pszUnit, "Left", 0, &x);
	m_pIniFile->GetInteger(pszUnit, "Top", 0, &y);
	m_pIniFile->EraseSection(pszUnit);

	char	szSection[64] = "", szParent[64] = "";
	while(GetNextUnit(szSection))
	{
		m_pIniFile->GetString(szSection, "._Parent", "", szParent, sizeof(szParent));
		if (szParent[0] && !strcmp(szParent, pszUnit))
		{
			m_pIniFile->EraseKey(szSection, "._Parent");
			if (m_pIniFile->GetInteger(szSection, "Left", 0, &x2))
				m_pIniFile->WriteInteger(szSection, "Left", x2 + x);
			if (m_pIniFile->GetInteger(szSection, "Top", 0, &y2))
				m_pIniFile->WriteInteger(szSection, "Top", y2 + y);
		}
	}
	m_Modified = true;
}

int KUi::RenameUnit(const char* pszUnit, const char* pNewName)
{
    if (m_pIniFile == NULL)
        return false;
	if (m_pIniFile->IsSectionExist(pNewName) ||
		!m_pIniFile->IsSectionExist(pszUnit))
		return false;

    //记录当前位置
    char szNextUnitName[128] = "";
    BOOL bHaveNextUnit = FALSE;
    if (m_pIniFile->GetNextSection(pszUnit, szNextUnitName)) 
    {
        bHaveNextUnit = TRUE;
    }

	char	szPreKey[128] = "", szKey[128] = "", szBuffer[1024];
	while(m_pIniFile->GetNextKey(pszUnit, szPreKey, szKey))
	{
		m_pIniFile->GetString(pszUnit, szKey, "", szBuffer, sizeof(szBuffer));
		m_pIniFile->WriteString(pNewName, szKey, szBuffer);
		strcpy(szPreKey, szKey);
	}
	m_pIniFile->EraseSection(pszUnit);

	char szSection[128] = "", szParent[128] = "";

	while(GetNextUnit(szSection))
	{
		if (!strcmp(szSection, pNewName))
			continue;
		m_pIniFile->GetString(szSection, "._Parent", "", szParent, sizeof(szParent));
		if (szParent[0] && !strcmp(szParent, pszUnit))
			m_pIniFile->WriteString(szSection, "._Parent", pNewName);
	}

    if (bHaveNextUnit) 
    {
        MoveUnit(pNewName, szNextUnitName, FALSE);
    }

	m_Modified = true;
	return true;
}

IIniFile* KUi::GetIni(int bModify)
{
	if (bModify && m_pIniFile)
		m_Modified = true;
	return m_pIniFile;
}

int KUi::ChangeUnitParent(const char* pszUnit, const char* pNewParent)
{
    if (m_pIniFile == NULL) 
        return false;
	if (!m_pIniFile->IsSectionExist(pszUnit)) 
	{
		return false;
    }
    if (!pNewParent[0]) 
    {
        return false;
    }

    if (!m_pIniFile->IsSectionExist(pNewParent)) 
    {
        return false;
    }

	char	szOldParent[64];
	m_pIniFile->GetString(pszUnit, "._Parent", "", szOldParent, sizeof(szOldParent));
	POINT	OldPos, OldParentPos, NewParentPos;
	m_pIniFile->GetInteger(pszUnit, "Left", 0, (int*)&OldPos.x);
	m_pIniFile->GetInteger(pszUnit, "Top", 0, (int*)&OldPos.y);
	if (szOldParent[0])
	{
		m_pIniFile->GetInteger(szOldParent, "Left", 0, (int*)&OldParentPos.x);
		m_pIniFile->GetInteger(szOldParent, "Top", 0, (int*)&OldParentPos.y);
	}
	else
	{
		OldParentPos.x = OldParentPos.y = 0;
	}

	if (pNewParent[0])
	{
		m_pIniFile->GetInteger(pNewParent, "Left", 0, (int*)&NewParentPos.x);
		m_pIniFile->GetInteger(pNewParent, "Top", 0, (int*)&NewParentPos.y);
	}
	else
	{
		NewParentPos.x = NewParentPos.y = 0;
	}
	m_pIniFile->WriteString(pszUnit, "._Parent", pNewParent);
	m_pIniFile->WriteInteger(pszUnit, "Left", OldPos.x + OldParentPos.x - NewParentPos.x);
	m_pIniFile->WriteInteger(pszUnit, "Top", OldPos.y + OldParentPos.y - NewParentPos.y);
	m_Modified = true;
	return true;
}

int KUi::SafeChangeUnitParent(const char* pszUnit, const char* pNewParent)
{
    int nResult = FALSE;
    int nRetCode = false;
    KG_PROCESS_ERROR(m_pIniFile);
    KG_PROCESS_ERROR(pszUnit);
    nRetCode = m_pIniFile->IsSectionExist(pszUnit);
    KG_PROCESS_ERROR(nRetCode);
    KG_PROCESS_ERROR(pNewParent);
    KG_PROCESS_ERROR(pNewParent[0]);
    
    //如果pNewParent的窗口存在，直接更改父窗口，不需其他处理。
    if (m_pIniFile->IsSectionExist(pNewParent)) 
    {
        KG_PROCESS_ERROR(!g_IsStringFrameLayer(pNewParent));
        KG_PROCESS_ERROR(!IsUnitMyForefather(pNewParent, pszUnit));

        ChangeUnitParent(pszUnit, pNewParent);
        nResult = TRUE;
        KG_PROCESS_ERROR(0);
    }

    //如果这个Unit是Frame
    char szKeyValue[128] = "";
    m_pIniFile->GetString(pszUnit, "._WndType", "", szKeyValue, sizeof(szKeyValue));
    KG_PROCESS_ERROR(_tcsicmp(szKeyValue, g_GetWndTypeKey(UI_WND_FRAME)) == 0);
    KG_PROCESS_ERROR(g_IsStringFrameLayer(pNewParent));
    m_pIniFile->WriteString(pszUnit, "._Parent", pNewParent);
    
    nResult = TRUE;
Exit0:
    return nResult;
}

BOOL KUi::IsUnitMyForefather(LPCTSTR pszMe, LPCTSTR pszMaybeForeFather)
{
    int nResult = FALSE;
        
    KG_PROCESS_ERROR(pszMe && pszMe[0]);
    KG_PROCESS_ERROR(pszMaybeForeFather && pszMaybeForeFather[0]);
    KG_PROCESS_ERROR(m_pIniFile);

    char szParent[128] = "";
    strcpy(szParent, pszMe);
    while (m_pIniFile->IsSectionExist(szParent)) 
    {
        m_pIniFile->GetString(szParent, "._Parent", "", szParent, sizeof(szParent));

        if(_tcsicmp(szParent, pszMaybeForeFather) == 0)
        {
            nResult = TRUE;
            break;
        }
    }

Exit0:
    return nResult;
}

BOOL KUi::IsUnitMyFather(LPCTSTR pszMe, LPCTSTR pszSection)
{
    BOOL bResult = false;
    char szParent[128];
    KG_PROCESS_ERROR(pszMe && pszMe[0]);
    KG_PROCESS_ERROR(pszSection && pszSection[0]);
    KG_PROCESS_ERROR(m_pIniFile);
    KG_PROCESS_ERROR(m_pIniFile->IsSectionExist(pszMe));

    m_pIniFile->GetString(pszMe, "._Parent", "", szParent, sizeof(szParent));
    if (stricmp(szParent, pszSection) == 0)
        return TRUE;

Exit0:
    return FALSE;
}

int KUi::GetFrameUnit(char *pszFrameUnit)
{
    int nResult = FALSE;
    KG_PROCESS_ERROR(pszFrameUnit);
    KG_PROCESS_ERROR(m_pIniFile);

    char szUnit[128] = "";
    char szKeyValue[128] = "";
    while (GetNextUnit(szUnit)) 
    {
        m_pIniFile->GetString(szUnit, "._WndType", "", szKeyValue, sizeof(szKeyValue));
        if (_tcsicmp(szKeyValue, g_GetWndTypeKey(UI_WND_FRAME)) == 0) 
        {
            m_pIniFile->GetString(szUnit, "._Parent", "", szKeyValue, sizeof(szKeyValue));
            _ASSERT(g_IsStringFrameLayer(szKeyValue)); 

            strcpy(pszFrameUnit, szUnit);
            nResult = TRUE;
            KG_PROCESS_ERROR(0);
        }
    }

Exit0:
    return nResult;
}

int KUi::MoveUnit(const char *pszUnit, const char *pszTarget, BOOL bBehind /* = TRUE */)
{
    int nResult = FALSE;
    int nRetCode = false;
    KG_PROCESS_ERROR(pszUnit);
    KG_PROCESS_ERROR(pszTarget);
    nRetCode = m_pIniFile->IsSectionExist(pszUnit);
    KG_PROCESS_ERROR(nRetCode);
    nRetCode = m_pIniFile->IsSectionExist(pszTarget);
    KG_PROCESS_ERROR(nRetCode);

    if (!strcmp(pszUnit, pszTarget)) 
    {
        nResult = TRUE;
        KG_PROCESS_ERROR(0);
    }

    MoveUnitToEnd(pszUnit);

    char szUnitName[128];
    szUnitName[0] = 0;
    char szNextUnitName[128];
    szNextUnitName[0] = 0;
    if (bBehind) 
    {
        strcpy(szUnitName, pszTarget);
        m_pIniFile->GetNextSection(szUnitName, szNextUnitName);
        strcpy(szUnitName, szNextUnitName);
    }
    else
    {
        strcpy(szUnitName, pszTarget);
    }

    while ( m_pIniFile->GetNextSection(szUnitName, szNextUnitName)) 
    {
        if (!strcmp(szUnitName, pszUnit)) 
        {
            break;
        }
        MoveUnitToEnd(szUnitName);
        strcpy(szUnitName, szNextUnitName);
    }

    m_Modified = true;

    nResult = TRUE;
Exit0:
    return nResult;
    
}
int KUi::MoveUnitToEnd(const char *pszUnit)
{
    int nResult = FALSE;
    int nRetCode = false;
    KG_PROCESS_ERROR(pszUnit);
    KG_PROCESS_ERROR(pszUnit[0]);
    KG_PROCESS_ERROR(m_pIniFile);
    
    nRetCode = m_pIniFile->IsSectionExist(pszUnit);
    KG_PROCESS_ERROR(nRetCode);

    char szTempName[128] = "Wnd_MoveUnitToEnd_Temp_0";
    char szKeyName[128];
    szKeyName[0] = 0;
    char szNextKeyName[128];
    szNextKeyName[0] = 0;
    char szKeyValue[1024];
    szKeyValue[0] = 0;

    int i = 0;
    while (m_pIniFile->IsSectionExist(szTempName)) 
    {
        i++;
        sprintf(szTempName, "Wnd_MoveUnitToEnd_Temp_%d",i);
    }

    while (m_pIniFile->GetNextKey(pszUnit, szKeyName, szNextKeyName)) 
    {
        m_pIniFile->GetString(pszUnit, szNextKeyName, "", szKeyValue, sizeof(szKeyValue));
        m_pIniFile->WriteString(szTempName, szNextKeyName, szKeyValue);
        strcpy(szKeyName, szNextKeyName);
    }

    m_pIniFile->EraseSection(pszUnit);

    szKeyName[0] = 0;
    szNextKeyName[0] = 0;
    while (m_pIniFile->GetNextKey(szTempName, szKeyName, szNextKeyName)) 
    {
        m_pIniFile->GetString(szTempName, szNextKeyName, "", szKeyValue, sizeof(szKeyValue));
        m_pIniFile->WriteString(pszUnit, szNextKeyName, szKeyValue);
        strcpy(szKeyName, szNextKeyName);
    }
        
    m_pIniFile->EraseSection(szTempName);
    m_Modified = true;
    nResult = TRUE;
Exit0:
    return nResult;
}


int KUi::GetUnitTreePath(OUT LPTSTR pszTreePath, IN int nlength, IN LPCTSTR pszUnitName)
{
    int nResult             = false;
    int nRetCoed            = false;
    char szParentName[128]  = "";
    char szUnit[128]    = "";

    KG_PROCESS_ERROR(pszTreePath);
    KG_PROCESS_ERROR(pszUnitName);
    KG_PROCESS_ERROR(pszUnitName[0]);
    KG_PROCESS_ERROR(m_pIniFile);
    KG_PROCESS_ERROR(nlength > 0);
    KG_PROCESS_ERROR(m_pIniFile->IsSectionExist(pszUnitName));

    strncpy(pszTreePath, pszUnitName, nlength);
    szUnit[nlength - 1] = '\0';

    strncpy(szUnit, pszUnitName, sizeof(szUnit));
    szUnit[sizeof(szUnit) - 1] = '\0';
    while (m_pIniFile->IsSectionExist(szUnit)) 
    {
        m_pIniFile->GetString(szUnit, "._Parent", "", szParentName, sizeof(szParentName));
        
        KG_PROCESS_ERROR(int(strlen(szParentName) + strlen(pszTreePath)) < nlength);
        _ASSERT(int(strlen(szParentName) + strlen(pszTreePath)) < nlength && "TreePath to short!");
        //strncpy(pszTreePath + strlen(szParentName), pszTreePath, strlen(szParentName));
        memmove(pszTreePath + strlen(szParentName) + 1, pszTreePath, strlen(pszTreePath) + 1);
        memcpy(pszTreePath, szParentName, strlen(szParentName));
        pszTreePath[strlen(szParentName)] = '/';

        strncpy(szUnit, szParentName, sizeof(szUnit));
        szUnit[sizeof(szUnit) - 1] = '\0';
    }

    pszTreePath[nlength - 1] = '\0';
    
    nResult = true;
Exit0:
    return nResult;
}

int KUi::GetItemTreePath(OUT char *pszTreePath, IN int nLength, IN const char *pszItemName)
{
    int nResult             = false;
    int nRetCoed            = false;
    char szParentName[128];
    szParentName[0] = '\0';
    char szUnit[128];
    szUnit[0] = '\0';
    char szType[128];
    szType[0] = '\0';
    std::vector<_tstring> aFatherList;

    KG_PROCESS_ERROR(pszTreePath);
    KG_PROCESS_ERROR(pszItemName);
    KG_PROCESS_ERROR(pszItemName[0]);
    KG_PROCESS_ERROR(m_pIniFile);
    KG_PROCESS_ERROR(nLength > 0);
    KG_PROCESS_ERROR(m_pIniFile->IsSectionExist(pszItemName));

    strncpy(szUnit, pszItemName, sizeof(szUnit));
    szUnit[sizeof(szUnit) - 1] = '\0';

    aFatherList.push_back(szUnit);

    while (m_pIniFile->IsSectionExist(szUnit))
    {
        m_pIniFile->GetString(szUnit, "._Parent", "", szParentName, sizeof(szParentName));
        m_pIniFile->GetString(szParentName, "._WndType", "", szType, sizeof(szType));
        int nType = g_GetWndType(szType);
        if (nType < UI_ITEM_NULL || nType > UI_ITEM_TREE_LEAFE)
            break;

        aFatherList.push_back(szParentName);
        strncpy(szUnit, szParentName, sizeof(szUnit));
    }

    pszTreePath[0] = '\0';
    aFatherList.pop_back();
    for (int i = aFatherList.size() - 1; i >= 0; --i)
    {
        strncat(pszTreePath, aFatherList[i].c_str(), nLength);
        if (i)
            strncat(pszTreePath, "/", nLength);
    }
    pszTreePath[nLength - 1] = '\0';

    nResult = true;
Exit0:
    return nResult;
}

int KUi::GetItemOwnerWndName(OUT char *pszWnd, IN int nLength, IN const char *pszItemName)
{
    int nResult = false;
    char szFather[128];
    szFather[0] = '\0';
    char szType[128];
    szType[0] = '\0';
    int nType = UI_WND_WINDOW;

    KG_PROCESS_ERROR(pszWnd);
    KG_PROCESS_ERROR(pszItemName);
    KG_PROCESS_ERROR(m_pIniFile);

    strncpy(szFather, pszItemName, sizeof(szFather));
    while (m_pIniFile->IsSectionExist(szFather))
    {
        m_pIniFile->GetString(szFather, "._WndType", "", szType, sizeof(szType));
        nType = g_GetWndType(szType);
        if (nType < UI_ITEM_NULL || nType > UI_ITEM_TREE_LEAFE)
        {
            strncpy(pszWnd, szFather, nLength);
            pszWnd[nLength - 1] = '\0';
            KG_PROCESS_SUCCESS(true);
        }
        m_pIniFile->GetString(szFather, "._Parent", "", szFather, sizeof(szFather));
    }
    KG_PROCESS_ERROR(false);

Exit1:
    nResult = true;
Exit0:
    return nResult;
}

int KUi::IsWndHasHandle(const char *pszWnd)
{
    int nResult = false;
    char szSetcion[128];
    char szType[128];
    KG_PROCESS_ERROR(pszWnd);
    KG_PROCESS_ERROR(m_pIniFile);

    szSetcion[0] = '\0';
    while (GetNextUnit(szSetcion))
    {
        if (!IsUnitMyFather(szSetcion, pszWnd))
            continue;
        m_pIniFile->GetString(szSetcion, "._WndType", "", szType, sizeof(szType));
        if (stricmp(szType, "Handle") == 0)
            KG_PROCESS_SUCCESS(true);
    }

    KG_PROCESS_ERROR(false);

Exit1:
    nResult = true;
Exit0:
    return nResult;
}