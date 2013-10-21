#pragma once

#include "engine.h"
#include "Ui.h"

struct KSHOWWNDNODE
{
    HTREEITEM	hItem;
    char        szName[128];
    char        szParentName[128];
    KSHOWWNDNODE *pChildNode;
    KSHOWWNDNODE *pParentNode;
    KSHOWWNDNODE *pNextBrotherNode;
    KSHOWWNDNODE *pPreBrotherNode;
};

class KUiShowWndTree
{
public:
    KUiShowWndTree();
    ~KUiShowWndTree();
    int BuildTree(IIniFile *pIni);
    int AddChild(KSHOWWNDNODE *pParent, KSHOWWNDNODE *PChild);
    KSHOWWNDNODE* AddNodeByName(IIniFile *pIni, const char *pszName);
    int AddBrother(KSHOWWNDNODE *pExistChild, KSHOWWNDNODE *pNewChild);
    //int MoveChild(KSHOWWNDNODE *pChild,KSHOWWNDNODE *pPreParent, KSHOWWNDNODE *pNewParent);
    int MoveUp(KSHOWWNDNODE *pNodeNow, KUi *pUi);
    int MoveDown(KSHOWWNDNODE *pNodeNow, KUi *pUi);

    int ClearTree();
    //int DeleteNode(KSHOWWNDNODE *pNode);
    KSHOWWNDNODE* IsNodeExist(const char *pszName);
    int GetNamebyHItem(HTREEITEM hItemNow, char *pszName);
    int BuildUpShowWnd(HWND hTree);
    KSHOWWNDNODE* GetTheNodeByPosition(const POINT &Pos,  HWND hTree);
    KSHOWWNDNODE* GetRoot() { return &m_Root; };
    int SelParent(HWND hParent, HWND hTree, KSHOWWNDNODE *pNode, char *pszParent);
    KSHOWWNDNODE* GetNodeByHItem(HTREEITEM hItemNow);
    int ExPandByHItem(HWND hTree, HTREEITEM hItemNow);
    int Expand(HWND hTree, KSHOWWNDNODE *pNode);
    int NomalrizeIni(KUi *pUi);
    
protected:
private:
    KSHOWWNDNODE m_Root;
};