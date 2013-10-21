#include "Stdafx.h"
#include "UiShowWndTree.h"
#include "KGPublic.h"

KUiShowWndTree::~KUiShowWndTree()
{
    ClearTree();
}

KUiShowWndTree::KUiShowWndTree()
{
    ZeroMemory(&m_Root, sizeof(m_Root));
}

int KUiShowWndTree::BuildTree(IIniFile *pIni)
{
    int nResult = FALSE;
    KG_PROCESS_ERROR(pIni);

    char szUnit[128] = "";
    char szNextUnit[128] = "";
    char szParentName[128] = "";
    char szKeyValue[128] = "";
    BOOL bHasRootWnd = FALSE;
    while (pIni->GetNextSection(szUnit, szNextUnit)) 
    {
        pIni->GetString(szNextUnit, "._WndType", "", szKeyValue, sizeof(szKeyValue));
        if (_tcsicmp(szKeyValue, g_GetWndTypeKey(UI_WND_FRAME)) == 0) 
        {
            pIni->GetString(szNextUnit, "._Parent", "", szParentName, sizeof(szParentName));
            _ASSERT(g_IsStringFrameLayer(szParentName));
            bHasRootWnd = TRUE;
            break;
        }
        strcpy(szUnit, szNextUnit);
    }
    
    KG_PROCESS_ERROR(bHasRootWnd);

    ClearTree();

    ZeroMemory(&m_Root, sizeof(m_Root));
    strcpy(m_Root.szName, szNextUnit);
    strcpy(m_Root.szParentName, szParentName);
   
    szUnit[0] = 0;
    szNextUnit[0] = 0;
    while (pIni->GetNextSection(szUnit, szNextUnit)) 
    {
        if (!strcmp(szNextUnit, m_Root.szName)) 
        {
            strcpy(szUnit, szNextUnit);
            continue;
        }
        AddNodeByName(pIni, szNextUnit);
        strcpy(szUnit, szNextUnit);
    }


    nResult = TRUE;
Exit0:
    return nResult;
}

KSHOWWNDNODE* KUiShowWndTree::AddNodeByName(IIniFile *pIni, const char *pszName)
{
    KSHOWWNDNODE* pResult = NULL;
    KG_PROCESS_ERROR(pIni);
    KG_PROCESS_ERROR(pszName && pszName[0]);
    KG_PROCESS_ERROR(strcmp(pszName, m_Root.szName));

    pResult = IsNodeExist(pszName);
    if (pResult) 
    {
        KG_PROCESS_ERROR(0);
    }

    char szParentName[128] = "";
    pIni->GetString(pszName, "._Parent", "", szParentName, sizeof(szParentName));

    KSHOWWNDNODE *pParentNode = IsNodeExist(szParentName);
    if (!pParentNode)
    {
        pParentNode = AddNodeByName(pIni, szParentName);
        KG_PROCESS_ERROR(pParentNode);
    }

    KSHOWWNDNODE *pNode = new KSHOWWNDNODE;
    ZeroMemory(pNode, sizeof(KSHOWWNDNODE));
    strcpy(pNode->szName, pszName);
    strcpy(pNode->szParentName, szParentName);
    AddChild(pParentNode, pNode);

    pResult= pNode;
Exit0:
    return pResult;
}

int KUiShowWndTree::AddChild(KSHOWWNDNODE *pParent, KSHOWWNDNODE *PChild)
{
    int nResult = FALSE;
    KG_PROCESS_ERROR(pParent && PChild);

    if (pParent->pChildNode) 
    {
        AddBrother(pParent->pChildNode, PChild);
    }
    else
    {
        pParent->pChildNode      = PChild;
        PChild->pParentNode      = pParent;
        PChild->pNextBrotherNode = NULL;
        PChild->pPreBrotherNode  = NULL;
        strcpy(PChild->szParentName, pParent->szName);
    }

    nResult = TRUE;
Exit0:
    return nResult;
}

int KUiShowWndTree::AddBrother(KSHOWWNDNODE *pExistChild, KSHOWWNDNODE *pNewChild)
{
    int nResult = FALSE;
    KG_PROCESS_ERROR(pExistChild && pNewChild);

    KSHOWWNDNODE *pEndNode = pExistChild;
    while (pEndNode->pNextBrotherNode) 
    {
        pEndNode = pEndNode->pNextBrotherNode;
    }

    pEndNode->pNextBrotherNode  = pNewChild;
    pNewChild->pPreBrotherNode  = pEndNode;
    pNewChild->pNextBrotherNode = NULL;
    pNewChild->pParentNode      = pExistChild->pParentNode;
    strcpy(pNewChild->szParentName, pExistChild->szParentName);

    nResult = TRUE;
Exit0:
    return nResult;
}

KSHOWWNDNODE* KUiShowWndTree::IsNodeExist(const char *pszName)
{
    KSHOWWNDNODE* pResult = NULL;
    KG_PROCESS_ERROR(pszName && pszName[0]);

    if (!strcmp(pszName, m_Root.szName)) 
    {
        pResult = &m_Root;
        KG_PROCESS_ERROR(0);
    }

    KSHOWWNDNODE *pNow = m_Root.pChildNode;
    KG_PROCESS_ERROR(pNow);

    BOOL bChildFinished = FALSE;
    do
    {
        if (bChildFinished) 
        {
            if(pNow->pNextBrotherNode)
            {
                pNow = pNow->pNextBrotherNode;
                bChildFinished = FALSE;
            }
            else
            {
                pNow = pNow->pParentNode;
                bChildFinished = TRUE;
            }
        }
        else
        {
            if (!strcmp(pNow->szName, pszName))
            {
                pResult = pNow;
                break;
            }

            if(pNow->pChildNode)
            {
                pNow = pNow->pChildNode;
                bChildFinished = FALSE;            
            }
            else
            {
                if (pNow->pNextBrotherNode) 
                {
                    pNow = pNow->pNextBrotherNode;
                    bChildFinished = FALSE;
                }
                else
                {
                    pNow = pNow->pParentNode;
                    bChildFinished = TRUE;
                }
            
            }
        }

        if (pNow == &m_Root) 
        {
            break;
        }
    }while (TRUE);

Exit0:
    return pResult;

}

int KUiShowWndTree::ClearTree()
{
    int nResult = FALSE;
    KSHOWWNDNODE *pNow = m_Root.pChildNode;
    KSHOWWNDNODE *pDel = NULL;
    KG_PROCESS_ERROR(pNow);

    do
    {
        pDel = pNow;
        pNow = pNow->pChildNode ? pNow->pChildNode : (pNow->pNextBrotherNode ? pNow->pNextBrotherNode : pNow->pParentNode);
        if (!pDel->pChildNode) 
        {
            (pDel->pParentNode)->pChildNode = NULL;
            delete pDel;
        }

        if (pNow == &m_Root) 
        {
            break;
        } 

    }while (TRUE);


    nResult = TRUE;
Exit0:
    ZeroMemory(&m_Root, sizeof(KSHOWWNDNODE));
    return nResult;
}

int KUiShowWndTree::BuildUpShowWnd(HWND hTree)
{
    int nResult = FALSE;
    KG_PROCESS_ERROR(hTree);

    TreeView_DeleteAllItems(hTree);
    
    KG_PROCESS_ERROR(m_Root.szName && m_Root.szName[0]);

    char szRootName[256];
    sprintf(szRootName, "%s( %s )", m_Root.szName, m_Root.szParentName);

    TV_INSERTSTRUCT stTVIS;
	stTVIS.hParent		= NULL;
	stTVIS.hInsertAfter = TVI_ROOT;
	stTVIS.item.mask	= TVIF_TEXT;
	stTVIS.item.pszText = szRootName;
    m_Root.hItem = TreeView_InsertItem( hTree, &stTVIS);
    
    KSHOWWNDNODE *pNow = m_Root.pChildNode;
    KG_PROCESS_ERROR(pNow);

    BOOL bChildFinished = FALSE;
    do
    {
        if (bChildFinished) 
        {
            if(pNow->pNextBrotherNode)
            {
                pNow = pNow->pNextBrotherNode;
                bChildFinished = FALSE;
            }
            else
            {
                pNow = pNow->pParentNode;
                bChildFinished = TRUE;
            }
        }
        else
        {
            if (pNow->hItem == NULL)
            {
	            stTVIS.hParent	= (pNow->pParentNode)->hItem;
	            stTVIS.hInsertAfter = TVI_LAST;
	            stTVIS.item.mask	= TVIF_TEXT;
	            stTVIS.item.pszText = pNow->szName;
                pNow->hItem = TreeView_InsertItem( hTree, &stTVIS);
            }

            if(pNow->pChildNode)
            {
                pNow = pNow->pChildNode;
                bChildFinished = FALSE;            
            }
            else
            {
                if (pNow->pNextBrotherNode) 
                {
                    pNow = pNow->pNextBrotherNode;
                    bChildFinished = FALSE;
                }
                else
                {
                    pNow = pNow->pParentNode;
                    bChildFinished = TRUE;
                }
            
            }
        }

        if (pNow == &m_Root) 
        {
            break;
        }
    }while (TRUE);

    nResult = TRUE;
Exit0:
    return nResult;

}

int KUiShowWndTree::GetNamebyHItem(HTREEITEM hItemNow, char *pszName)
{
    int nResult = FALSE;
    KG_PROCESS_ERROR(hItemNow);
    KG_PROCESS_ERROR(pszName);
    KG_PROCESS_ERROR(m_Root.szName);

    if (hItemNow == m_Root.hItem) 
    {
        strcpy(pszName, m_Root.szName);
        nResult = TRUE;
        KG_PROCESS_ERROR(0);
    }

    KSHOWWNDNODE *pNow = m_Root.pChildNode;
    KG_PROCESS_ERROR(pNow);

    BOOL bChildFinished = FALSE;
    do
    {
        if (bChildFinished) 
        {
            if(pNow->pNextBrotherNode)
            {
                pNow = pNow->pNextBrotherNode;
                bChildFinished = FALSE;
            }
            else
            {
                pNow = pNow->pParentNode;
                bChildFinished = TRUE;
            }
        }
        else
        {
            if (pNow->hItem == hItemNow)
            {
                strcpy(pszName, pNow->szName);
                nResult = TRUE;
                break;
            }

            if(pNow->pChildNode)
            {
                pNow = pNow->pChildNode;
                bChildFinished = FALSE;            
            }
            else
            {
                if (pNow->pNextBrotherNode) 
                {
                    pNow = pNow->pNextBrotherNode;
                    bChildFinished = FALSE;
                }
                else
                {
                    pNow = pNow->pParentNode;
                    bChildFinished = TRUE;
                }
            
            }
        }

        if (pNow == &m_Root) 
        {
            break;
        }
    }while (TRUE);

Exit0:
    return nResult;

}

KSHOWWNDNODE* KUiShowWndTree::GetTheNodeByPosition(const POINT &Pos, HWND hTree)
{
    KSHOWWNDNODE *pResult = NULL;
    int nRetCode = false;
    KG_PROCESS_ERROR(hTree);
    RECT ItemRect;
    nRetCode = TreeView_GetItemRect(hTree, m_Root.hItem, &ItemRect, TRUE);
    KG_PROCESS_ERROR(nRetCode);
     if (PtInRect(&ItemRect, Pos)) 
    {
        pResult = &m_Root;
        KG_PROCESS_ERROR(0);
    }

    KSHOWWNDNODE *pNow = m_Root.pChildNode;
    KG_PROCESS_ERROR(pNow);

    BOOL bChildFinished = FALSE;
    do
    {
        if (bChildFinished) 
        {
            if(pNow->pNextBrotherNode)
            {
                pNow = pNow->pNextBrotherNode;
                bChildFinished = FALSE;
            }
            else
            {
                pNow = pNow->pParentNode;
                bChildFinished = TRUE;
            }
        }
        else
        {
            nRetCode = TreeView_GetItemRect(hTree, pNow->hItem, &ItemRect, TRUE);
            KG_PROCESS_ERROR(nRetCode);
            if (PtInRect(&ItemRect, Pos)) 
            {
                pResult = pNow;
                break;
            }

            if(pNow->pChildNode)
            {
                pNow = pNow->pChildNode;
                bChildFinished = FALSE;            
            }
            else
            {
                if (pNow->pNextBrotherNode) 
                {
                    pNow = pNow->pNextBrotherNode;
                    bChildFinished = FALSE;
                }
                else
                {
                    pNow = pNow->pParentNode;
                    bChildFinished = TRUE;
                }
            
            }
        }

        if (pNow == &m_Root) 
        {
            break;
        }
    }while (TRUE);


Exit0:
    return pResult;

}

//遍历算法有问题。

int KUiShowWndTree::SelParent(HWND hParent, HWND hTree, KSHOWWNDNODE *pNode, char *pszParent)
{
    int nResult = FALSE;
    KG_PROCESS_ERROR(hTree);
    KG_PROCESS_ERROR(pNode);


    int nMaxID = TreeView_GetCount(hTree) + 4;
    int nStartID = 10000;
    int nEndID   = nStartID;
 	HMENU hMenu = CreatePopupMenu();
	KG_PROCESS_ERROR(hMenu);

    struct MENUNAME  
    {
        char szName[128];
    };

    MENUNAME *pMenuItem = new MENUNAME[nMaxID];
    KG_PROCESS_ERROR(pMenuItem);

    if (pNode == &m_Root) 
    {
        int i;
        for (i = LAYER_BEGIN; i < LAYER_END; i++) 
        {
	        AppendMenu(hMenu, MF_ENABLED |MF_STRING, nEndID, g_GetFrameLayerLayoutString(i));
            strcpy(pMenuItem[nEndID -nStartID].szName, g_GetFrameLayerName(i));
            nEndID++;
        }
    }
    else
    {
        AppendMenu(hMenu, MF_ENABLED |MF_STRING, nEndID, m_Root.szName);
        strcpy(pMenuItem[nEndID -nStartID].szName, m_Root.szName);
        nEndID++;

        KSHOWWNDNODE *pNow = m_Root.pChildNode;
        KG_PROCESS_ERROR(pNow);

        BOOL bChildFinished = FALSE;
        do
        {
            if (bChildFinished) 
            {
                if(pNow->pNextBrotherNode)
                {
                    pNow = pNow->pNextBrotherNode;
                    bChildFinished = FALSE;
                }
                else
                {
                    pNow = pNow->pParentNode;
                    bChildFinished = TRUE;
                }
            }
            else
            {
                if (pNow != pNode)
                {
                    BOOL bNeedAdd = TRUE;
                    KSHOWWNDNODE *pParent = pNow->pParentNode;
                    while (pParent) 
                    {
                        if(pParent == pNode)
                        {
                            bNeedAdd = FALSE;
                            break;
                        }
                        pParent = pParent->pParentNode;
                    }
                    
                    if (bNeedAdd) 
                    {
	                    AppendMenu(hMenu, MF_ENABLED |MF_STRING, nEndID, pNow->szName);
                        strcpy(pMenuItem[nEndID - nStartID].szName, pNow->szName);
                        nEndID++;
                    }
                }

                if(pNow->pChildNode)
                {
                    pNow = pNow->pChildNode;
                    bChildFinished = FALSE;            
                }
                else
                {
                    if (pNow->pNextBrotherNode) 
                    {
                        pNow = pNow->pNextBrotherNode;
                        bChildFinished = FALSE;
                    }
                    else
                    {
                        pNow = pNow->pParentNode;
                        bChildFinished = TRUE;
                    }
                
                }
            }

            if (pNow == &m_Root) 
            {
                break;
            }
        }while (TRUE);
    }

	POINT Pos;
	GetCursorPos(&Pos);
	int nSel = TrackPopupMenu(hMenu, TPM_RETURNCMD | TPM_LEFTBUTTON | TPM_NONOTIFY,
		Pos.x, Pos.y, 0, hParent, NULL);
	DestroyMenu(hMenu);
    hMenu = NULL;
	
    KG_PROCESS_ERROR(nSel >= nStartID && nSel < nEndID);

    strcpy(pszParent, pMenuItem[nSel - nStartID].szName);

    nResult = TRUE;
Exit0:
    if (hMenu) 
    {
	    DestroyMenu(hMenu);        
    }
    if (pMenuItem) 
    {
        delete[] pMenuItem;
        pMenuItem = NULL;
    }
    return nResult;

}

KSHOWWNDNODE* KUiShowWndTree::GetNodeByHItem(HTREEITEM hItemNow)
{
    KSHOWWNDNODE* pResult = NULL;
    KG_PROCESS_ERROR(hItemNow);
    KG_PROCESS_ERROR(m_Root.szName);

    if (hItemNow == m_Root.hItem) 
    {
        pResult = &m_Root;
        KG_PROCESS_ERROR(0);
    }

    KSHOWWNDNODE *pNow = m_Root.pChildNode;
    KG_PROCESS_ERROR(pNow);

    BOOL bChildFinished = FALSE;
    do
    {
        if (bChildFinished) 
        {
            if(pNow->pNextBrotherNode)
            {
                pNow = pNow->pNextBrotherNode;
                bChildFinished = FALSE;
            }
            else
            {
                pNow = pNow->pParentNode;
                bChildFinished = TRUE;
            }
        }
        else
        {
            if (pNow->hItem == hItemNow)
            {
                pResult = pNow;
                break;
            }

            if(pNow->pChildNode)
            {
                pNow = pNow->pChildNode;
                bChildFinished = FALSE;            
            }
            else
            {
                if (pNow->pNextBrotherNode) 
                {
                    pNow = pNow->pNextBrotherNode;
                    bChildFinished = FALSE;
                }
                else
                {
                    pNow = pNow->pParentNode;
                    bChildFinished = TRUE;
                }
            
            }
        }

        if (pNow == &m_Root) 
        {
            break;
        }
    }while (TRUE);

Exit0:
    return pResult;

}

int KUiShowWndTree::ExPandByHItem(HWND hTree, HTREEITEM hItemNow)
{
    int nResult = false;
    int nRetCode = false;
    KSHOWWNDNODE *pNow = GetNodeByHItem(hItemNow);
    KG_PROCESS_ERROR(pNow);

    nRetCode = Expand(hTree, pNow);
    KG_PROCESS_ERROR(nRetCode);

    nResult = false;
Exit0:
    return nResult;
}

int KUiShowWndTree::Expand(HWND hTree, KSHOWWNDNODE *pNode)
{
    int nResult = false;
    KSHOWWNDNODE *pParent = NULL;
    KG_PROCESS_ERROR(pNode);
    
    TreeView_Expand(hTree, pNode->hItem, TVM_EXPAND);
    pParent = pNode->pParentNode;
    while (pParent)
    {
        TreeView_Expand(hTree, pParent->hItem, TVM_EXPAND);
        pParent = pParent->pParentNode;
    }

    nResult = true;
Exit0:
    return nResult;
}


int KUiShowWndTree::NomalrizeIni(KUi *pUi)
{
    int nResult = FALSE;
    KG_PROCESS_ERROR(pUi);
    KG_PROCESS_ERROR(m_Root.szName && m_Root.szName[0]);

    pUi->MoveUnitToEnd(m_Root.szName);
       
    KSHOWWNDNODE *pNow = m_Root.pChildNode;
    KG_PROCESS_ERROR(pNow);

    BOOL bChildFinished = FALSE;
    do
    {
        if (bChildFinished) 
        {
            if(pNow->pNextBrotherNode)
            {
                pNow = pNow->pNextBrotherNode;
                bChildFinished = FALSE;
            }
            else
            {
                pNow = pNow->pParentNode;
                bChildFinished = TRUE;
            }
        }
        else
        {
             pUi->MoveUnitToEnd(pNow->szName);

            if(pNow->pChildNode)
            {
                pNow = pNow->pChildNode;
                bChildFinished = FALSE;            
            }
            else
            {
                if (pNow->pNextBrotherNode) 
                {
                    pNow = pNow->pNextBrotherNode;
                    bChildFinished = FALSE;
                }
                else
                {
                    pNow = pNow->pParentNode;
                    bChildFinished = TRUE;
                }
            
            }
        }

        if (pNow == &m_Root) 
        {
            break;
        }
    }while (TRUE);



    nResult = TRUE;
Exit0:
    return nResult;
}


int KUiShowWndTree::MoveUp(KSHOWWNDNODE *pNodeNow, KUi *pUi)
{
    int nResult = FALSE;

    KG_PROCESS_ERROR(pNodeNow);

    KG_PROCESS_ERROR(pNodeNow->pPreBrotherNode);
    (pNodeNow->pPreBrotherNode)->pNextBrotherNode = pNodeNow->pNextBrotherNode;

    if (pNodeNow->pNextBrotherNode) 
    {
        (pNodeNow->pNextBrotherNode)->pPreBrotherNode = pNodeNow->pPreBrotherNode;
    }

    pNodeNow->pNextBrotherNode = pNodeNow->pPreBrotherNode;
    pNodeNow->pPreBrotherNode  = (pNodeNow->pPreBrotherNode)->pPreBrotherNode;

    (pNodeNow->pNextBrotherNode)->pPreBrotherNode = pNodeNow;
    if (pNodeNow->pPreBrotherNode) 
    {
        (pNodeNow->pPreBrotherNode)->pNextBrotherNode = pNodeNow;
    }

    //如果移到最顶端
    if (!pNodeNow->pPreBrotherNode) 
    {
        if (pNodeNow->pParentNode) 
        {
            (pNodeNow->pParentNode)->pChildNode = pNodeNow;
        }      
    }

    NomalrizeIni(pUi);

    nResult = TRUE;
Exit0:
    return nResult;

}

int KUiShowWndTree::MoveDown(KSHOWWNDNODE *pNodeNow, KUi *pUi)
{
    int nResult = FALSE;

    KG_PROCESS_ERROR(pNodeNow);

    KG_PROCESS_ERROR(pNodeNow->pNextBrotherNode);

    //如果是最顶端
    if (!pNodeNow->pPreBrotherNode) 
    {
        if (pNodeNow->pParentNode) 
        {
            (pNodeNow->pParentNode)->pChildNode = pNodeNow->pNextBrotherNode;
        }
    }

    (pNodeNow->pNextBrotherNode)->pPreBrotherNode = pNodeNow->pPreBrotherNode;

    if (pNodeNow->pPreBrotherNode) 
    {
        (pNodeNow->pPreBrotherNode)->pNextBrotherNode = pNodeNow->pNextBrotherNode;
    }

    pNodeNow->pPreBrotherNode = pNodeNow->pNextBrotherNode;
    pNodeNow->pNextBrotherNode = (pNodeNow->pNextBrotherNode)->pNextBrotherNode;
    
    (pNodeNow->pPreBrotherNode)->pNextBrotherNode = pNodeNow;
    if (pNodeNow->pNextBrotherNode) 
    {
        (pNodeNow->pNextBrotherNode)->pPreBrotherNode = pNodeNow;
    }

    NomalrizeIni(pUi);

    nResult = TRUE;
Exit0:
    return nResult;

}


