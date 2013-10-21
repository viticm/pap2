#include "stdafx.h"
#include "./iniwndbuilder.h"
#include "../wndwindow.h"
#include "../wndframe.h"
#include "../wndwindow.h"
#include "../wndframe.h"
#include "../wndedit.h"
#include "../wndbutton.h"
#include "../wndpageset.h"
#include "../wndcheckbox.h"
#include "../wndscene.h"
#include "../wndminimap.h"
#include "../wndtexture.h"
#include "../wndmovie.h"
#include "../wndwebpage.h"
#include "../wndnewscrollbar.h"
#include "../../KGUI.h"
#include "../../script/kscriptmgr.h"

namespace
{
	UI::KWndFrame* g_pWndRoot;
}

namespace UI
{
	static BOOL GetParentTreePathFromIni(IIniFile* pIni, LPCSTR szSectionName, char szTreePath[KWndStation::WND_TREE_PATH_LEN])
	{
		char szBuffer[WND_SECTION_NAME_LEN];
		typedef std::vector<std::string> KStringList;
		KStringList strWndNames;

		KGLOG_PROCESS_ERROR(pIni);
		KGLOG_PROCESS_ERROR(szSectionName);

		szTreePath[0] = '\0';

		strncpy(szBuffer, szSectionName, _countof(szBuffer));
		while (pIni->GetString(szBuffer, "._Parent", "", szBuffer, _countof(szBuffer)))
		{
			strWndNames.push_back(szBuffer);

			if (KWndStation::GetLayerIndexByName(szBuffer) != WL_UNKNOWN)
			{
				break;
			}
		}

		for (KStringList::reverse_iterator it = strWndNames.rbegin(); it != strWndNames.rend(); ++it)
		{
			LPCSTR szName = (*it).c_str();

			KGLOG_PROCESS_ERROR(strlen(szTreePath) + strlen(szName) + 1 < KWndStation::WND_TREE_PATH_LEN);

			strcat(szTreePath, szName);
			strcat(szTreePath, "/");
		}

		return TRUE;
Exit0:
		return FALSE;
	}

	// NOTE: 没有缩放，没有item。
	static BOOL InitRawWndWindow(KWndWindow* pWnd, IIniFile* pIni, LPCSTR szSectionName)
	{
		int nRetCode = false;
		int nWidth = 0;
		int nHeight = 0;
		int nLeft = 0;
		int nTop = 0;
		int	nValue0 = 0;
		int nValue1 = 0;
		int nWndLayer = 0;
		char szBuffer[MAX_PATH];
		static int nsTipIndex = 0;

		pWnd->SetName(szSectionName);

		pIni->GetInteger(szSectionName, "Width", 0, &nWidth);
		pIni->GetInteger(szSectionName, "Height", 0, &nHeight);
		pIni->GetInteger(szSectionName, "Left", 0, &nLeft);
		pIni->GetInteger(szSectionName, "Top", 0, &nTop);

		pWnd->SetSize((float)nWidth, (float)nHeight);
		pWnd->SetRelPos((float)nLeft, (float)nTop);

		pIni->GetInteger(szSectionName, "Disable",  0, &nValue0);
		pWnd->SetStyle(WND_S_DISABLE, nValue0);

		pIni->GetInteger(szSectionName, "Moveable", 0, &nValue0);
		pWnd->SetStyle(WND_S_MOVEABLE, nValue0);

		pIni->GetInteger(szSectionName, "DummyWnd", 0, &nValue0);
		pWnd->SetStyle(WND_S_SIZE_WITH_ALL_CHILD, nValue0);

		pIni->GetInteger(szSectionName, "MousePenetrable", 0, &nValue0);
		pWnd->SetStyle(WND_S_MOUSE_PENETRABLE, nValue0);

		pIni->GetInteger(szSectionName, "SelfHoldMouseHover", 0, &nValue0);
		pWnd->SetStyle(WND_S_SELF_HOLD_MOUSE_HOVER, nValue0);

		pIni->GetInteger(szSectionName, "DisableBringToTop", 0, &nValue0);
		pWnd->SetStyle(WND_S_DISABLE_BRING_TO_TOP, nValue0);

		pIni->GetInteger(szSectionName, "BreatheWhenHide", 0, &nValue0);
		pWnd->SetStyle(WND_S_BREAHTE_WHEN_HIDE, nValue0);
		

		pIni->GetString(szSectionName, "$Tip", "", szBuffer, _countof(szBuffer));
		if (szBuffer[0] != '\0')
		{
			int nIndex = -1;
			nValue0 = 0;
			pIni->GetInteger(szSectionName, "OrgTip", 0, &nValue0);
			if (nValue0)
			{
				char szTipName[32];
				sprintf(szTipName, "###WTipText_%d", nsTipIndex);
				++nsTipIndex;
				g_pUI->m_GlobalStrValuableMgr.RecodeValue(szBuffer);
				g_pUI->m_GlobalStrValuableMgr.Append(szTipName, szBuffer);
				nIndex = g_pUI->m_GlobalStrValuableMgr.GetIndexAt(szTipName);
			}
			else
			{
				nIndex = g_pUI->m_GlobalStrValuableMgr.GetIndexAt(szBuffer);
			}
			pIni->GetInteger(szSectionName, "ShowTipType", 0, &nValue0);
			pIni->GetInteger(szSectionName, "TipRichText", 0, &nValue1);

			pWnd->SetTip(nIndex, nValue0, nValue1);
		}

		if (pIni->GetInteger(szSectionName, "Alpha", 255, &nValue0))
			pWnd->SetSelfAlpha(nValue0);

		pIni->GetString(szSectionName, "AreaFile", "", szBuffer, _countof(szBuffer));
		if (szBuffer[0] != '\0')
		{
			FormatFilePath(szBuffer);
			pWnd->SetAreaTestFile(szBuffer);
		}

		pIni->GetString(szSectionName, "._Parent", "", szBuffer, _countof(szBuffer));
		nWndLayer = KWndStation::GetLayerIndexByName(szBuffer);
		if (nWndLayer != WL_UNKNOWN)
		{
			nRetCode = g_pUI->m_WndStation.AddFrame((KWndFrame*)CastWndWindow(pWnd, "WndFrame"), nWndLayer);
			KGLOG_PROCESS_ERROR(nRetCode);

			g_pWndRoot = (KWndFrame*)CastWndWindow(pWnd, "WndFrame");
			KGLOG_PROCESS_ERROR(g_pWndRoot);
		}
		else
		{
			KWndWindow* pParent = NULL;
			char szTreePath[KWndStation::WND_TREE_PATH_LEN];

			nRetCode = GetParentTreePathFromIni(pIni, szSectionName, szTreePath);
			KGLOG_PROCESS_ERROR(nRetCode);

			pParent = g_pUI->m_WndStation.SearchWindow(szTreePath);
			KGLOG_PROCESS_ERROR(pParent);

			nRetCode = pParent->AddChild(pWnd);
			KGLOG_PROCESS_ERROR(nRetCode);
		}

		return TRUE;
Exit0:
		return FALSE;
	}

	static BOOL InitWndWindow(KWndWindow* pWnd, IIniFile* pIni, LPCSTR szSectionName)
	{
		int nResult = false;
		int nRetCode = false;
		float fScale = 0.0f;

		nRetCode = InitRawWndWindow(pWnd, pIni, szSectionName);
		KG_PROCESS_ERROR(nRetCode);

		fScale = g_pUI->m_WndStation.GetUIScale();

		pWnd->SetRelPos(pWnd->GetRelX() * fScale, pWnd->GetRelY() * fScale);
		pWnd->Scale(fScale, fScale);

		pWnd->CreateItemHandle(pIni, szSectionName);

		nResult = true;
Exit0:
		return nResult;
	}

	static BOOL InitWndPage(KWndWindow* pWnd, IIniFile* pIni, LPCSTR szSectionName)
	{
		int nResult = false;
		int nRetCode = false;
		float fScale = 0.0f;
		int nPageCount = 0;
		KWndPageSet* pPageSet = NULL;
		LPCSTR szPageSetName = NULL;

		nRetCode = InitRawWndWindow(pWnd, pIni, szSectionName);
		KG_PROCESS_ERROR(nRetCode);

		fScale = g_pUI->m_WndStation.GetUIScale();

		pWnd->SetRelPos(pWnd->GetRelX() * fScale, pWnd->GetRelY() * fScale);
		pWnd->Scale(fScale, fScale);

		pWnd->CreateItemHandle(pIni, szSectionName);

		pPageSet = (KWndPageSet*)CastWndWindow(pWnd->GetParent(), "WndPageSet");
		KGLOG_PROCESS_ERROR(pPageSet);

		szPageSetName = pPageSet->GetName();
		KGLOG_PROCESS_ERROR(szPageSetName);

		pIni->GetInteger(szPageSetName, "PageCount", 0, &nPageCount);

		for (int nIndex = 0; nIndex < nPageCount; ++nIndex)
		{
			char szKeyName[WND_SECTION_NAME_LEN];
			char szPageName[WND_SECTION_NAME_LEN];

			_snprintf(szKeyName, _countof(szKeyName), "Page_%d", nIndex);
			pIni->GetString(szPageSetName, szKeyName, "", szPageName, _countof(szPageName));

			if (strcmp(szPageName, szSectionName) == 0)
			{
				KWndCheckBox* pWndCheckBox = NULL;
				char szCheckBoxName[WND_SECTION_NAME_LEN];

				_snprintf(szKeyName, _countof(szKeyName), "CheckBox_%d", nIndex);
				pIni->GetString(szPageSetName, szKeyName, "", szCheckBoxName, _countof(szCheckBoxName));

				pWndCheckBox = (KWndCheckBox*)CastWndWindow(g_pUI->m_WndStation.SearchWindow(pPageSet, szCheckBoxName), "WndCheckBox");
				KGLOG_PROCESS_ERROR(pWndCheckBox);

				pPageSet->AddPage(pWnd, pWndCheckBox);

				break;
			}
		}

		nResult = true;
Exit0:
		return nResult;
	}

	static BOOL InitWndFrame(KWndFrame* pWnd, IIniFile* pIni, LPCSTR szSectionName)
	{
		int nResult = false;
		int nRetCode = false;
		int nValue = 0;
		int nDragAreaX = 0;
		int nDragAreaY = 0;
		int nDragAreaWidth = 0;
		int nDragAreaHeight = 0;
		int nFollowMouseOffsetX = 0;
		int nFollowMouseOffsetY = 0;
		char szValue[128];
		float fScale = 0.0f;

		nRetCode = InitRawWndWindow(pWnd, pIni, szSectionName);
		KG_PROCESS_ERROR(nRetCode);

		pIni->GetInteger(szSectionName, "IsCustomDragable", 0, &nValue);
		pWnd->EnableDrag(nValue);

		pIni->GetInteger(szSectionName, "DragAreaLeft", 0, &nDragAreaX);
		pIni->GetInteger(szSectionName, "DragAreaTop", 0, &nDragAreaY);
		pIni->GetInteger(szSectionName, "DragAreaWidth", 0, &nDragAreaWidth);
		pIni->GetInteger(szSectionName, "DragAreaHeight", 0, &nDragAreaHeight);
		pWnd->SetDragArea((float)nDragAreaX, (float)nDragAreaY, (float)nDragAreaWidth, (float)nDragAreaHeight);

		pIni->GetInteger(szSectionName, "FollowMouse", 0, &nValue);
		pWnd->EnableFollowMouseMove(nValue);

		pIni->GetInteger(szSectionName, "FollowMouseOffsetX", 0, &nFollowMouseOffsetX);
		pIni->GetInteger(szSectionName, "FollowMouseOffsetY", 0, &nFollowMouseOffsetY);
		pWnd->SetFollowMouseOffset((float)nFollowMouseOffsetX, (float)nFollowMouseOffsetY);

		pIni->GetInteger(szSectionName, "EnableTabChangeFocus", 0, &nValue);
		pWnd->SetStatus(UI_WND_FRAME_TAB_CHANGE_FOCUS, nValue);

		pIni->GetInteger(szSectionName, "MouseFollowFocus", 0, &nValue);
		pWnd->SetStatus(UI_WND_FRAME_MOUSE_FOLLOW_FOCUS, nValue);

		pIni->GetInteger(szSectionName, "DisableBreath", 0, &nValue);
		pWnd->SetStatus(UI_WND_FRAME_DISABLE_BREATH, nValue);

		pIni->GetInteger(szSectionName, "ShowWhenHideUI", 0, &nValue);
		pWnd->SetStatus(UI_WND_FRAME_SHOW_WHEN_HIDE_UI, nValue);

		pIni->GetInteger(szSectionName, "RenderEvent", 0, &nValue);
		pWnd->SetStatus(UI_WND_FRAME_POST_RENDER_EVENT, nValue);

		pIni->GetInteger(szSectionName, "RenderSampling", 0, &nValue);
		pWnd->SetStatus(UI_WND_FRAME_RENDER_SAMPLING, nValue);

		if (pIni->GetInteger(szSectionName, "ItemCount", 0, &nValue))
		{
			for (int nIndex = 0; nIndex < nValue; ++nIndex) 
			{
				char szCurItemKey[WND_SECTION_NAME_LEN];
				_snprintf(szCurItemKey, WND_SECTION_NAME_LEN, "Item_%d", nIndex);
				pIni->GetString(szSectionName, szCurItemKey, "", szValue, _countof(szValue));
				if (szValue[0] != '\0')
				{
					pWnd->AppendFocus(szValue);
				}
			}
		}

		if (pIni->GetString(szSectionName, "ClassName", "", szValue, _countof(szValue)))
			pWnd->SetLuaClass(szValue);

		fScale = g_pUI->m_WndStation.GetUIScale();

		pWnd->SetRelPos(pWnd->GetRelX() * fScale, pWnd->GetRelY() * fScale);
		pWnd->Scale(fScale, fScale);

		pWnd->CreateItemHandle(pIni, szSectionName);

		nResult = true;
Exit0:
		return nResult;
	}

	static BOOL InitWndButton(KWndButton* pWnd, IIniFile* pIni, LPCSTR szSectionName)
	{
		int nResult = false;
		int nRetCode = false;
		int nValue = 0;
		int nNormal = 0;
		int nMouseOver = 0;
		int nMouseDown = 0;
		int nDisable = 0;
		char szImagePath[MAX_PATH];
		KWndNewScrollBar* pScrollBar = NULL;
		float fScale = 0.0f;

		nRetCode = InitRawWndWindow(pWnd, pIni, szSectionName);
		KG_PROCESS_ERROR(nRetCode);

		pIni->GetInteger(szSectionName, "ButtonDisable", 0, &nValue);
		pWnd->SetStatus(WNDBUTTON_ENABLE, !nValue);

		pIni->GetInteger(szSectionName, "NormalFont", -1, &nNormal);
		pIni->GetInteger(szSectionName, "MouseOverFont", -1, &nMouseOver);
		pIni->GetInteger(szSectionName, "MouseDownFont", -1, &nMouseDown);
		pIni->GetInteger(szSectionName, "DisableFont", -1, &nDisable);
		pWnd->SetFont(nNormal, nMouseOver, nMouseDown, nDisable);

		pIni->GetInteger(szSectionName, "NormalGroup", 0, &nNormal);
		pIni->GetInteger(szSectionName, "MouseOverGroup", 0, &nMouseOver);
		pIni->GetInteger(szSectionName, "MouseDownGroup", 0, &nMouseDown);
		pIni->GetInteger(szSectionName, "DisableGroup", 0, &nDisable);
		pIni->GetString(szSectionName, "Image", "" , szImagePath, _countof(szImagePath));
		FormatFilePath(szImagePath);
		pWnd->SetAnimation(nNormal, nMouseOver, nMouseDown, nDisable, szImagePath);

		pScrollBar = (KWndNewScrollBar*)CastWndWindow(pWnd->GetParent(), "WndNewScrollBar");
		if (pScrollBar)
			pScrollBar->BindSlideButton(pWnd);

		fScale = g_pUI->m_WndStation.GetUIScale();

		pWnd->SetRelPos(pWnd->GetRelX() * fScale, pWnd->GetRelY() * fScale);
		pWnd->Scale(fScale, fScale);

		pWnd->CreateItemHandle(pIni, szSectionName);

		pWnd->UpdataAnimateGroup();

		nResult = true;
Exit0:
		return nResult;
	}

	static BOOL InitWndPageSet(KWndPageSet* pWnd, IIniFile* pIni, LPCSTR szSectionName)
	{
		int nResult = false;
		int nRetCode = false;
		float fScale = 0.0f;

		nRetCode = InitRawWndWindow(pWnd, pIni, szSectionName);
		KGLOG_PROCESS_ERROR(nRetCode);

		fScale = g_pUI->m_WndStation.GetUIScale();

		pWnd->SetRelPos(pWnd->GetRelX() * fScale, pWnd->GetRelY() * fScale);
		pWnd->Scale(fScale, fScale);

		pWnd->CreateItemHandle(pIni, szSectionName);

		nResult = true;
Exit0:
		return nResult;
	}

	static BOOL InitWndCheckBox(KWndCheckBox* pWnd, IIniFile* pIni, LPCSTR szSectionName)
	{
		int nResult = false;
		int nRetCode = false;
		int nValue = 0;
		int nUnCheckAndEnable = 0;
		int nCheckAndEnable = 0;
		int nUnCheckAndDisable = 0;
		int nCheckAndDisable = 0;
		int nChecking = 0;
		int nUnChecking = 0;
		int nCheckedAndEnableWhenMouseOver = 0;
		int nUncheckedAndEnableWhenMouseOver = 0;
		int nCheckedAndDisableWhenMouseOver = 0;
		int nUncheckedAndDisableWhenMouseOver = 0;
		int nCheckFont = 0;
		int nUncheckFont = 0;
		int nMouseDownFont = 0;
		int nMouseOverFont = 0;
		int nDisableFont = 0;
		float fScale = 0.0f;
		char szImagePath[MAX_PATH];

		nRetCode = InitRawWndWindow(pWnd, pIni, szSectionName);
		KG_PROCESS_ERROR(nRetCode);

		pIni->GetInteger(szSectionName, "DisableCheck", 0, &nValue);
		pWnd->SetStatus(WNDCHECKBOX_ENABLE, !nValue);

		pIni->GetInteger(szSectionName, "CheckedWhenCreate", 0, &nValue);
		pWnd->SetStatus(WNDCHECKBOX_CHECKED, nValue);

		pIni->GetInteger(szSectionName, "RadioButton", 0, &nValue);
		pWnd->SetStatus(WNDCHECKBOX_RADIO_BUTTON, nValue);

		pIni->GetInteger(szSectionName, "CheckFont", -1, &nCheckFont);
		pIni->GetInteger(szSectionName, "UncheckFont", -1, &nUncheckFont);
		pIni->GetInteger(szSectionName, "MouseDownFont", -1, &nMouseDownFont);
		pIni->GetInteger(szSectionName, "MouseOverFont", -1, &nMouseOverFont);
		pIni->GetInteger(szSectionName, "DisableFont", -1, &nDisableFont);

		pWnd->SetFontScheme(nCheckFont, nUncheckFont, nMouseDownFont, nMouseOverFont, nDisableFont);

		// TODO: 名字修改
		pIni->GetInteger(szSectionName, "UnCheckAndEnable", 0, &nUnCheckAndEnable);
		pIni->GetInteger(szSectionName, "CheckAndEnable", 0, &nCheckAndEnable);
		pIni->GetInteger(szSectionName, "UnCheckAndDisable", 0, &nUnCheckAndDisable);
		pIni->GetInteger(szSectionName, "CheckAndDisable", 0, &nCheckAndDisable);
		pIni->GetInteger(szSectionName, "Checking", 0, &nChecking);
		pIni->GetInteger(szSectionName, "UnChecking", 0, &nUnChecking);
		pIni->GetInteger(szSectionName, "CheckedAndEnableWhenMouseOver", 0, &nCheckedAndEnableWhenMouseOver);
		pIni->GetInteger(szSectionName, "UnCheckedAndEnableWhenMouseOver", 0, &nUncheckedAndEnableWhenMouseOver);
		pIni->GetInteger(szSectionName, "CheckedAndDisableWhenMouseOver", 0, &nCheckedAndDisableWhenMouseOver);
		pIni->GetInteger(szSectionName, "UnCheckedAndDisableWhenMouseOver", 0, &nUncheckedAndDisableWhenMouseOver);

		pIni->GetString(szSectionName, "Image", "" , szImagePath, _countof(szImagePath));
		FormatFilePath(szImagePath);
		pWnd->SetAnimation(nUnCheckAndEnable, nCheckAndEnable, nUnCheckAndDisable, nCheckAndDisable, 
			nChecking, nUnChecking, nCheckedAndEnableWhenMouseOver, nUncheckedAndEnableWhenMouseOver, 
			nCheckedAndDisableWhenMouseOver, nUncheckedAndDisableWhenMouseOver, szImagePath);

		fScale = g_pUI->m_WndStation.GetUIScale();

		pWnd->SetRelPos(pWnd->GetRelX() * fScale, pWnd->GetRelY() * fScale);
		pWnd->Scale(fScale, fScale);

		pWnd->CreateItemHandle(pIni, szSectionName);

		pWnd->UpdataAnimateGroup();

		nResult = true;
Exit0:
		return nResult; 
	}

	static BOOL InitWndNewScrollBar(KWndNewScrollBar* pWnd, IIniFile* pIni, LPCSTR szSectionName)
	{
		int nResult = false;
		int nRetCode = false;
		int nValue = 0;
		float fScale = 0.0f;

		nRetCode = InitRawWndWindow(pWnd, pIni, szSectionName);
		KG_PROCESS_ERROR(nRetCode);

		pIni->GetInteger(szSectionName, "Type", 0, &nValue);
		pWnd->SetStatus(WNDNEWSCROLL_VERTICAL, nValue);

		pIni->GetInteger(szSectionName, "AutoHideSlideButton", 0, &nValue);
		pWnd->SetStatus(WNDNEWSCROLL_AUTO_HIDE_SLIDE, nValue);

		pIni->GetInteger(szSectionName, "AutoResizeSlideButton", 0, &nValue);
		pWnd->SetStatus(WNDNEWSCROLL_AUTO_RESIZE_SLIDE, nValue);

		pIni->GetInteger(szSectionName, "StepCount", 0, &nValue);
		pWnd->SetStepCount(nValue);

		pIni->GetInteger(szSectionName, "PageStepCount", 0, &nValue);
		pWnd->SetPageStepCount(nValue);

		fScale = g_pUI->m_WndStation.GetUIScale();

		pWnd->SetRelPos(pWnd->GetRelX() * fScale, pWnd->GetRelY() * fScale);
		pWnd->Scale(fScale, fScale);

		pWnd->CreateItemHandle(pIni, szSectionName);

		nResult = true;
Exit0:
		return nResult;
	}

	static BOOL InitWndEdit(KWndEdit* pWnd, IIniFile* pIni, LPCSTR szSectionName)
	{
		int nResult = false;
		int nRetCode = false;
		int nValue = 0;
		float fScale = 0.0f;
		char szValue[32] = { 0 };
		DWORD dwFocusBgColor = 0;
		DWORD dwSelectBgColor = 0;

		nRetCode = InitRawWndWindow(pWnd, pIni, szSectionName);
		KG_PROCESS_ERROR(nRetCode);

		pIni->GetInteger(szSectionName, "MultiLine", 0, &nValue);
		pWnd->SetStatus(WNDEDIT_ES_MULTI_LINE, nValue);

		pIni->GetInteger(szSectionName, "Password", 0, &nValue);
		pWnd->SetStatus(WNDEDIT_ES_MASK_CHARACTER, nValue);
		if (nValue)
			pWnd->SetStatus(WNDEDIT_ES_MULTI_LINE, FALSE);

		pIni->GetInteger(szSectionName, "Type", 0, &nValue);
		pWnd->SetStatus(WNDEDIT_ES_CHAR_TYPE_FILTER, FALSE);
		pWnd->SetType(nValue);

		pIni->GetInteger(szSectionName, "MaxLen", 0, &nValue);
		pWnd->SetLimit(nValue);

		pIni->GetInteger(szSectionName, "FontScheme", 0, &nValue);
		pWnd->SetFontScheme(nValue);

		pIni->GetInteger(szSectionName, "SelFontScheme", 0, &nValue);
		pWnd->SetSelectFontScheme(nValue);

		pIni->GetInteger(szSectionName, "CaretFontScheme", 0, &nValue);
		pWnd->SetCaretFontScheme(nValue);

		pIni->GetString(szSectionName, "FocusBgColor", "", szValue, _countof(szValue));
		if (szValue[0] != '\0')
		{
			int nColorScheme = g_pUI->m_ColorSchemeMgr.FindScheme(ATL::CA2W(szValue, GetCodePage()));
			if (g_pUI->m_ColorSchemeMgr.IsExistColor(nColorScheme))
				dwFocusBgColor = g_pUI->m_ColorSchemeMgr.GetColor(nColorScheme);
		}
		if (pIni->GetInteger(szSectionName, "FocusBgColorAlpha", 0, &nValue))
			dwFocusBgColor = AddAlpha(dwFocusBgColor, nValue);

		pWnd->SetFocusBgColor(dwFocusBgColor);

		pIni->GetString(szSectionName, "SelectBgColor", "", szValue, _countof(szValue));
		if (szValue[0] != '\0')
		{
			int nColorScheme = g_pUI->m_ColorSchemeMgr.FindScheme(ATL::CA2W(szValue, GetCodePage()));
			if (g_pUI->m_ColorSchemeMgr.IsExistColor(nColorScheme))
				dwSelectBgColor = g_pUI->m_ColorSchemeMgr.GetColor(nColorScheme);
		}
		if (pIni->GetInteger(szSectionName, "SelectBgColorAlpha", 0, &nValue))
			dwSelectBgColor = AddAlpha(dwSelectBgColor, nValue);

		pWnd->SetSelectBgColor(dwSelectBgColor);

		pIni->GetInteger(szSectionName, "FontSpacing", 0, &nValue);
		pWnd->SetFontSpacing((float)nValue);

		pIni->GetInteger(szSectionName, "RowSpacing", 0, &nValue);
		pWnd->SetRowSpacing((float)nValue);

		fScale = g_pUI->m_WndStation.GetUIScale();

		pWnd->SetRelPos(pWnd->GetRelX() * fScale, pWnd->GetRelY() * fScale);
		pWnd->Scale(fScale, fScale);

		pWnd->CreateCursorHandle();
		pWnd->CreateTextHandle();

		nResult = true;
Exit0:
		return nResult;
	}

	static BOOL InitWndScene(KWndScene* pWnd, IIniFile* pIni, LPCSTR szSectionName)
	{
		int nResult  = false;
		int nRetCode = false;
		int	nValue = 0;
		float fScale = 0.0f;

		nRetCode = InitRawWndWindow(pWnd, pIni, szSectionName);
		KG_PROCESS_ERROR(nRetCode);

		if (pIni->GetInteger(szSectionName, "DisableRenderTerrain", 0, &nValue))
			pWnd->EnableRenderTerrain(nValue == 0);

		if (pIni->GetInteger(szSectionName, "DisableRenderSkyBox", 0, &nValue))
			pWnd->EnableRenderSkyBox(nValue == 0);

		if (pIni->GetInteger(szSectionName, "EnableAlpha", 0, &nValue))
			pWnd->EnableAlpha(nValue != 0);

		if (pIni->GetInteger(szSectionName, "EnableFrameMove", 0, &nValue))
			pWnd->EnableFrameMove(nValue != 0);

		pWnd->UpdateSceneViewport();

		fScale = g_pUI->m_WndStation.GetUIScale();

		pWnd->SetRelPos(pWnd->GetRelX() * fScale, pWnd->GetRelY() * fScale);
		pWnd->Scale(fScale, fScale);

		pWnd->CreateItemHandle(pIni, szSectionName);

		nResult = true;
Exit0:
		return nResult;
	}

	static BOOL InitWndMinimap(KWndMinimap* pWnd, IIniFile* pIni, LPCSTR szSectionName)
	{
		int nResult = false;
		int nRetCode = false;
		int nValue = 0;
		float fScale = 0.0f;
		char szValue[MAX_PATH];

		nRetCode = InitRawWndWindow(pWnd, pIni, szSectionName);
		KG_PROCESS_ERROR(nRetCode);

		pIni->GetInteger(szSectionName, "MinimapType", 0, &nValue);
		pWnd->SetMinimapType(nValue);

		pIni->GetString(szSectionName, "image", "", szValue, _countof(szValue));
		FormatFilePath(szValue);
		pWnd->LoadPointImageFile(szValue);

		pIni->GetInteger(szSectionName, "selfframe", -1, &nValue);
		pWnd->SetSelfImageFrame(nValue);

		pIni->GetString(szSectionName, "defaulttexture", "", szValue, _countof(szValue));
		FormatFilePath(szValue);
		pWnd->LoadDefaultImageFile(szValue);

		pIni->GetString(szSectionName, "sharptexture", "", szValue, _countof(szValue));
		FormatFilePath(szValue);
		pWnd->LoadSharpImageFile(szValue);		

		fScale = g_pUI->m_WndStation.GetUIScale();

		pWnd->SetRelPos(pWnd->GetRelX() * fScale, pWnd->GetRelY() * fScale);
		pWnd->Scale(fScale, fScale);

		pWnd->CreateItemHandle(pIni, szSectionName);

		nResult = true;
Exit0:
		return nResult;
	}

	static BOOL InitWndTexture(KWndTexture* pWnd, IIniFile* pIni, LPCSTR szSectionName)
	{
		int nResult = false;
		int nRetCode = false;
		float fScale = 0.0f;

		nRetCode = InitRawWndWindow(pWnd, pIni, szSectionName);
		KGLOG_PROCESS_ERROR(nRetCode);

		fScale = g_pUI->m_WndStation.GetUIScale();

		pWnd->SetRelPos(pWnd->GetRelX() * fScale, pWnd->GetRelY() * fScale);
		pWnd->Scale(fScale, fScale);

		pWnd->CreateItemHandle(pIni, szSectionName);

		nResult = true;
Exit0:
		return nResult;
	}

	static BOOL InitWndMovie(KWndMovie* pWnd, IIniFile* pIni, LPCSTR szSectionName)
	{
		int nResult = false;
		int nRetCode = false;
		int	nValue = 0;
		float fScale = 0.0f;

		nRetCode = InitRawWndWindow(pWnd, pIni, szSectionName);
		KG_PROCESS_ERROR(nRetCode);

		pWnd->UpdateMovieViewport();

		fScale = g_pUI->m_WndStation.GetUIScale();

		pWnd->SetRelPos(pWnd->GetRelX() * fScale, pWnd->GetRelY() * fScale);
		pWnd->Scale(fScale, fScale);

		pWnd->CreateItemHandle(pIni, szSectionName);

		nResult = true;
Exit0:
		return nResult;
	}

	static BOOL InitWndWebPage(KWndWebPage* pWnd, IIniFile* pIni, LPCSTR szSectionName)
	{
		int nResult = false;
		int nRetCode = false;
		float fScale = 0.0f;
		char szValue[MAX_PATH];

		nRetCode = InitRawWndWindow(pWnd, pIni, szSectionName);
		KGLOG_PROCESS_ERROR(nRetCode);

		fScale = g_pUI->m_WndStation.GetUIScale();

		pWnd->SetRelPos(pWnd->GetRelX() * fScale, pWnd->GetRelY() * fScale);
		pWnd->Scale(fScale, fScale);

		pIni->GetString(szSectionName, "$URL", "about:blank", szValue, _countof(szValue));
		pWnd->Navigate(ATL::CA2W(szValue));

		pWnd->CreateItemHandle(pIni, szSectionName);

		nResult = true;
Exit0:
		return nResult;
	}

	static int CreateWndWindow(IIniFile* pIni, LPCSTR szSectionName)
	{
		int nRetCode = false;
		KWndWindow* pWnd = NULL;
		char szWndType[WND_TYPE_NAME_LEN];

		KGLOG_PROCESS_ERROR(szSectionName);
		KGLOG_PROCESS_ERROR(szSectionName[0] != '\0');

		pIni->GetString(szSectionName, "._WndType", "", szWndType, _countof(szWndType));
		KG_PROCESS_ERROR(szWndType[0] != '\0');

		if (strcmp(szWndType, "WndFrame") == 0)
		{
			pWnd = new(std::nothrow)KWndFrame;
			KGLOG_PROCESS_ERROR(pWnd);

			nRetCode = InitWndFrame((KWndFrame*)pWnd, pIni, szSectionName);
			KGLOG_PROCESS_ERROR(nRetCode);
		}
		else if (strcmp(szWndType, "WndWindow") == 0)
		{
			pWnd = new(std::nothrow)KWndWindow;
			KGLOG_PROCESS_ERROR(pWnd);

			nRetCode = InitWndWindow(pWnd, pIni, szSectionName);
			KGLOG_PROCESS_ERROR(nRetCode);
		}
		else if (strcmp(szWndType, "WndButton") == 0)
		{
			pWnd = new(std::nothrow)KWndButton;
			KGLOG_PROCESS_ERROR(pWnd);

			nRetCode = InitWndButton((KWndButton*)pWnd, pIni, szSectionName);
			KGLOG_PROCESS_ERROR(nRetCode);
		}
		else if (strcmp(szWndType, "WndPage") == 0)
		{
			pWnd = new(std::nothrow)KWndWindow;
			KGLOG_PROCESS_ERROR(pWnd);

			nRetCode = InitWndPage(pWnd, pIni, szSectionName);
			KGLOG_PROCESS_ERROR(nRetCode);
		}
		else if (strcmp(szWndType, "WndPageSet") == 0)
		{
			pWnd = new(std::nothrow)KWndPageSet;
			KGLOG_PROCESS_ERROR(pWnd);

			nRetCode = InitWndPageSet((KWndPageSet*)pWnd, pIni, szSectionName);
			KGLOG_PROCESS_ERROR(nRetCode);
		}
		else if (strcmp(szWndType, "WndCheckBox") == 0)
		{
			pWnd = new(std::nothrow)KWndCheckBox;
			KGLOG_PROCESS_ERROR(pWnd);

			nRetCode = InitWndCheckBox((KWndCheckBox*)pWnd, pIni, szSectionName);
			KGLOG_PROCESS_ERROR(nRetCode);
		}
		else if (strcmp(szWndType, "WndNewScrollBar") == 0)
		{
			pWnd = new(std::nothrow)KWndNewScrollBar;
			KGLOG_PROCESS_ERROR(pWnd);

			nRetCode = InitWndNewScrollBar((KWndNewScrollBar*)pWnd, pIni, szSectionName);
			KGLOG_PROCESS_ERROR(nRetCode);
		}
		else if (strcmp(szWndType, "WndEdit") == 0)
		{
			pWnd = new(std::nothrow)KWndEdit;
			KGLOG_PROCESS_ERROR(pWnd);

			nRetCode = InitWndEdit((KWndEdit*)pWnd, pIni, szSectionName);
			KGLOG_PROCESS_ERROR(nRetCode);
		}
		else if (strcmp(szWndType, "WndScene") == 0)
		{
			pWnd = new(std::nothrow)KWndScene;
			KGLOG_PROCESS_ERROR(pWnd);

			nRetCode = InitWndScene((KWndScene*)pWnd, pIni, szSectionName);
			KGLOG_PROCESS_ERROR(nRetCode);
		}
		else if (strcmp(szWndType, "WndMinimap") == 0)
		{
			pWnd = new(std::nothrow)KWndMinimap;
			KGLOG_PROCESS_ERROR(pWnd);

			nRetCode = InitWndMinimap((KWndMinimap*)pWnd, pIni, szSectionName);
			KGLOG_PROCESS_ERROR(nRetCode);
		}
		else if (strcmp(szWndType, "WndTexture") == 0)
		{
			pWnd = new(std::nothrow)KWndTexture;
			KGLOG_PROCESS_ERROR(pWnd);

			nRetCode = InitWndTexture((KWndTexture*)pWnd, pIni, szSectionName);
			KGLOG_PROCESS_ERROR(nRetCode);
		}
		else if (strcmp(szWndType, "WndMovie") == 0)
		{
			pWnd = new(std::nothrow)KWndMovie;
			KGLOG_PROCESS_ERROR(pWnd);

			nRetCode = InitWndMovie((KWndMovie*)pWnd, pIni, szSectionName);
			KGLOG_PROCESS_ERROR(nRetCode);
		}
		else if (strcmp(szWndType, "WndWebPage") == 0)
		{
			pWnd = new(std::nothrow)KWndWebPage;
			KGLOG_PROCESS_ERROR(pWnd);

			nRetCode = InitWndWebPage((KWndWebPage*)pWnd, pIni, szSectionName);
			KGLOG_PROCESS_ERROR(nRetCode);
		}

		return true;
Exit0:
		KGLogPrintf(KGLOG_ERR, "KGUI CreateWndWindow(%s) failed\n", szSectionName);
		if (pWnd)
			pWnd->Destroy(FALSE);
		return false;
	}

	KWndFrame* CreateWndFrame(IIniFile* pIni)
	{
		char szNextSection[WND_SECTION_NAME_LEN] = { 0 };

		g_pWndRoot = NULL;

		KGLOG_PROCESS_ERROR(pIni);

		while (pIni->GetNextSection(szNextSection, szNextSection))
			CreateWndWindow(pIni, szNextSection);

		return g_pWndRoot;
Exit0:
		return NULL;
	}

	KWndFrame* CreateWndFrame(LPCSTR szIniFile, LPCSTR szFrameName)
	{
		// TODO : 目前只支持，父窗口最先创建的创建模式，其他都不可能正常运行
		// TODO : 并且父窗口的SectionName不能相同
		// TODO : 因为一个配置文件里面，SectionName不能相同，因此可以在此子树按名字查找，
		// TODO : 这种查找模式只适合在树创建的时间
		IIniFile* pIni = NULL;
		KWndFrame* pWndFrame = NULL;

		ASSERT(szIniFile);
		ASSERT(szFrameName);

		pIni = g_pUI->m_Config.OpenConfigFileMaxMatching(szIniFile);
		KGLOG_PROCESS_ERROR(pIni);

		pWndFrame = CreateWndFrame(pIni);
		KGLOG_PROCESS_ERROR(pWndFrame);

		// 修改Frame的名字
		if (szFrameName[0] != '\0')
			pWndFrame->SetName(szFrameName);

		BindWndScript(pWndFrame);

		return pWndFrame;
Exit0:
		return NULL;
	}


	BOOL BindWndScript(KWndFrame* pWnd)
	{
		BOOL bRetCode = FALSE;

		bRetCode = pWnd != NULL;
		KGLOG_PROCESS_ERROR(bRetCode);

		// 绑定脚本
		bRetCode = g_pUI->m_Script.BindScript(pWnd);
		KG_PROCESS_ERROR(bRetCode);

		// 只提供父窗口的OnCreate调用
		pWnd->FireCreate();

		if (pWnd->IsVisible())
			pWnd->Show();

		return TRUE;
Exit0:
		return FALSE;
	}

}

