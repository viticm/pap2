#pragma once
#include "UiWndData.h"

class IIniFile;

class KWndLeaf
{
	friend class KWndTree;

public:
	KWndLeaf();
	KWndLeaf(UI_WND_TYPE Type);
	~KWndLeaf();

	void	Init(IIniFile* pIni);
	int		GetPosFromDawn(int& x, int& y);
	void	SetPos(int x, int y);
	void	SetSize(int w, int h);
	bool	GetWndTreePath(LPTSTR szTreePath, int nLength);
    char *  GetName(){ return m_Name; }

private:
	void	MoveUp();
	int		CreateObject(IIniFile* pIni);
	void	ReleaseObject();
	void	AttachToHost();
	void	AddChild(KWndLeaf* pChild);
	void	SplitFromTree();
	void	Append(KWndLeaf* pLeaf);
    void    BindWindowObject(void *pObjiect);
    void    IgnoreWndObject() { m_bIgnoreWndObject = TRUE; };
	KWndLeaf* FindName(const char* pszName);
	KWndLeaf* GetNext() { return m_pNext; }

public:
	char		m_Name[128];

private:
	UI_WND_TYPE	m_Type;
	void*		m_pWndObject;
	KWndLeaf*	m_pPre;
	KWndLeaf*	m_pNext;
    BOOL        m_bIgnoreWndObject;
};

class KWndTree
{
public:
	KWndTree();
	~KWndTree();
	KWndLeaf*	CreateWnd(UI_WND_TYPE eType, const char* pszName, IIniFile* pIni);
	KWndLeaf*	FindWnd(const char* pszName);
	void		RemoveWnd(const char* pszName);
	void		UpdateShowWnd(const char* pszName, IIniFile* pIni);
	void		Clear();
	void		UpdateAllWndPosFormDawn();
	void		BuildTree();
	bool		GetWndTreePath(LPTSTR szTreePath, int nLength, LPCTSTR szName);
    int         UpdateShowWndWithBindSection(IIniFile* pIni);
    int         SafeCheck(IIniFile *pIni);
    int         SafeCheckUnitKey(IIniFile *pIni, LPCTSTR pszUnit, LPCTSTR pszKey, LPCTSTR pszKeyValue);
private:
	KWndLeaf	m_Root;
    KWndLeaf*    m_pRootWnd;
};

extern KWndTree	g_WndTree;
