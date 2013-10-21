#pragma once
#include "UiWndData.h"

enum FRAMELAYERTYPE
{
    LAYER_BEGIN,
    LAYER_LOWEST = LAYER_BEGIN,
	LAYER_LOWEST1,
	LAYER_LOWEST2,
    LAYER_NORMAL,
	LAYER_NORMAL1,
	LAYER_NORMAL2,
    LAYER_TOPMOST,
	LAYER_TOPMOST1,
	LAYER_TOPMOST2,
	LAYER_TEXTURE,
    LAYER_END,
    LAYER_UNKNOWN = LAYER_END,

};

FRAMELAYERTYPE g_GetFrameLayerType(LPCTSTR pszLayer);
LPCTSTR        g_GetFrameLayerName(int nLayer);
LPCTSTR        g_GetFrameLayerLayoutString(int nLayer);
BOOL           g_IsStringFrameLayer(LPCTSTR pszName);


class KUi
{
public:
	KUi();
	~KUi();

	void	SetModified(){ m_Modified = true; }
	int		IsModified() { return m_Modified; }
	IIniFile*	GetIni(int bModify = false);
	int		Load(const char* pszIni);
	int		Save(const char* pszIni);
	void	Clear();
	int		New();
	int		GetUnitBaseData(const char* pszUnit, KUiWndWindowData& data);
	int		SetUnitBaseData(const char* pszUnit, const KUiWndWindowData& data);
	int		GetNextUnit(char* pszUnit);
	void	RemoveUnit(const char* pszUnit);
	int		RenameUnit(const char* pszUnit, const char* pNewName);
	int		ChangeUnitParent(const char* pszUnit, const char* pNewParent);
    int     SafeChangeUnitParent(const char* pszUnit, const char* pNewParent);
    BOOL    IsUnitMyForefather(LPCTSTR pszMe, LPCTSTR pszMaybeForeFather);
    BOOL    IsUnitMyFather(LPCTSTR pszMe, LPCTSTR pszSection);
    int     GetFrameUnit(char *pszFrameUnit);
    int     MoveUnitToEnd(const char *pszUnit);
    int     MoveUnit(const char *pszUnit, const char *pszTarget, BOOL bbehind = TRUE);
    //将pszUnit移动到pszTarget的前面bbehind = FALSE或后面bbehind = TRUE.
    int     GetUnitTreePath(OUT LPTSTR pszTreePath, IN int nlength, IN LPCTSTR pszUnitName);
    int     GetItemTreePath(OUT char *pszTreePath, IN int nLength, IN const char *pszItemName);
    int     GetItemOwnerWndName(OUT char *pszWnd, IN int nLength, IN const char *pszItemName);
    int     IsWndHasHandle(const char *pszWnd);

private:
private:
	int		  m_Modified;
	char	  m_szIniFile[MAX_PATH];
	IIniFile* m_pIniFile;
};

extern KUi	g_Ui;
