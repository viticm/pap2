#include "stdafx.h"
#include "./kwndclassscripttable.h"
#include "./kitemhandleclassscripttable.h"
#include "./kscriptmgr.h"
#include "./kscriptloader.h"
#include "../elem/wndwindow.h"
#include "../elem/wndedit.h"
#include "../elem/wndbutton.h"
#include "../elem/wndcheckbox.h"
#include "../elem/wndscene.h"
#include "../elem/wndmovie.h"
#include "../elem/wndtexture.h"
#include "../elem/wndwebpage.h"
#include "../elem/wndpageset.h"
#include "../elem/wndframe.h"
#include "../elem/wndminimap.h"
#include "../elem/autolocatewnd.h"
#include "../elem/wndnewscrollbar.h"
#include "../elem/Components/item.h"
#include "../elem/Components/itemhandle.h"
#include "../elem/Components/decoder.h"
#include "../elem/Components/itemeventmgr.h"
#include "../KGUI.h"

namespace UI
{
	void KWndClassScriptTable::Load(lua_State* L)
	{
		static luaL_reg const s_aCursorLibTable[] =
		{
			{ "Show",							LuaCursor_ShowCursor },
			{ "IsVisible",						LuaCursor_IsVisible },
			{ "Switch",							LuaCursor_SwitchCursor },
			{ "GetCurrentIndex",				LuaCursor_GetCurrentCursorIndex },
			{ "Restore",						LuaCursor_RestoreCursor },
			{ "IsExist",						LuaCursor_IsExistCursor },
			{ "Load",							LuaCursor_LoadCursor },
			{ "Unload",							LuaCursor_UnloadCursor },
			{ "LoadAll",						LuaCursor_LoadAllCursor },
			{ "UnloadAll",						LuaCursor_UnloadAllCursor },
			{ "GetPos",							LuaCursor_GetCursorPos },
			{ "SetPos",							LuaCursor_SetCursorPos },
			
			{ NULL,								NULL },
		};

		//--wnd库准备删除。
		static luaL_reg const s_aWndLibTable[] =
		{
			{ "ToggleWindow",					LuaStation_ToggleWindow },
			{ "OpenWindow",						LuaStation_OpenWindow },
			{ "CloseWindow",					LuaStation_CloseWindow },
            { "KeepInClient",					LuaStation_KeepWndInClient },
            { "SetPopUpMenuPos",				LuaStation_SetPopUpMenuPos },
            { "SetTipPosByRect",				LuaStation_SetTipPosByRect },
            { "SetTipPosByWnd",					LuaStation_SetTipPosByWnd },
			{ "LoadFrameList",					LuaStation_LoadFrameList },
			{ "UnloadFrameList",				LuaStation_UnloadFrameList },
			{ "AdjustFrameListPosition",		LuaStation_AdjustFrameListPosition },
			{ NULL,								NULL },
		};

		static luaL_reg const s_aStationLibTable[] =
		{
			{ "SetFocusWindow",					LuaStation_SetFocusWindow },
			{ "GetFocusWindow",					LuaStation_GetFocusWindow },
			{ "SetActiveFrame",					LuaStation_SetActiveFrame },
			{ "GetActiveFrame",					LuaStation_GetActiveFrame },
			{ "SetCapture",						LuaStation_SetCapture },
			{ "GetCapture",						LuaStation_GetCapture },
			{ "ReleaseCapture",					LuaStation_ReleaseCapture },

			{ "Lookup",							LuaStation_Lookup },
			
			{ "GetWindowPosition",				LuaStation_GetWindowPosition },
			{ "GetClientPosition",				LuaStation_GetClientPosition },

			{ "GetStandardClientSize",			LuaStation_GetStandardClientSize },
			{ "GetClientSize",					LuaStation_GetClientSize },
			{ "OriginalToAdjustPos",			LuaStation_OriginalToAdjustPos },
			{ "AdjustToOriginalPos",			LuaStation_AdjustToOriginalPos },
			{ "SetUIScale",						LuaStation_SetUIScale },
			{ "GetUIScale",						LuaStation_GetUIScale },
			{ "GetMaxUIScale",				    LuaStation_GetMaxUIScale },
			{ "IsFullScreen",					LuaStation_IsFullScreen },
			{ "IsPanauision",					LuaStation_IsPanauision },
			{ "IsExclusiveMode",				LuaStation_IsExclusiveMode },
			{ "IsVisible",						LuaStation_IsVisible },
			{ "Show",							LuaStation_Show },
			{ "Hide",							LuaStation_Hide },
			{ "Paint",							LuaStation_Paint },
			{ "GetScreenPos",					LuaStation_GetScreenPos },
			{ "GetMessagePos",					LuaStation_GetMessagePos },
			{ "GetMessageWheelDelta",			LuaStation_GetMessageWheelDelta },
			
			{ "GetMessageKey",					LuaStation_GetMessageKey },
			{ "GetIdleTime",					LuaStation_GetIdleTime },
			{ "ClearIdleTime",					LuaStation_ClearIdleTime },
			{ "IsInUserAction",					LuaStation_IsInUserAction },
			{ "GetMouseOverWindow",				LuaStation_GetMouseOverWindow},

			{ "ToggleWindow",					LuaStation_ToggleWindow },
			{ "OpenWindow",						LuaStation_OpenWindow },
			{ "CloseWindow",					LuaStation_CloseWindow },
			{ "KeepInClient",					LuaStation_KeepWndInClient },
			{ "SetPopUpMenuPos",				LuaStation_SetPopUpMenuPos },
			{ "SetTipPosByRect",				LuaStation_SetTipPosByRect },
			{ "SetTipPosByWnd",					LuaStation_SetTipPosByWnd },
			{ "LoadFrameList",					LuaStation_LoadFrameList },
			{ "UnloadFrameList",				LuaStation_UnloadFrameList },
			{ "AdjustFrameListPosition",		LuaStation_AdjustFrameListPosition },

			{ NULL,								NULL },
		};

		//-----------------------------------------------------------------------------------------------------------------------
		static luaL_reg const s_aWndWindowMetaTable[] =
		{
			{ "GetRelPos",							LuaWindow_GetRelPos },
			{ "GetAbsPos",							LuaWindow_GetAbsPos },
			{ "GetSize",							LuaWindow_GetSize },
            { "SetSize",							LuaWindow_SetSize },
			{ "IsVisible",							LuaWindow_IsVisible },
			{ "IsDisable",							LuaWindow_IsDisable },
			{ "SetRelPos",							LuaWindow_SetRelPos },
			{ "SetAbsPos",							LuaWindow_SetAbsPos },
			{ "SetCursorAbove",						LuaWindow_SetCursorAbove },
			{ "Enable",								LuaWindow_Enable },
			{ "Show",								LuaWindow_Show },
			{ "Hide",								LuaWindow_Hide },
			{ "ToggleVisible",						LuaWindow_ToggleVisible },
			{ "BringToTop",							LuaWindow_BringToTop },
            { "Scale",							    LuaWindow_Scale },
			{ "CreateItemHandle",					LuaWindow_CreateItemHandle },
			{ "ReleaseItemHandle",					LuaWindow_ReleaseItemHandle },
			{ "Lookup",							    LuaWindow_Lookup },
			{ "GetName",							LuaWindow_GetName },
			{ "SetName",							LuaWindow_SetName },
			{ "GetPrev",							LuaWindow_GetPrev },
			{ "GetNext",							LuaWindow_GetNext },
			{ "GetParent",							LuaWindow_GetParent },
			{ "GetRoot",							LuaWindow_GetRoot },
			{ "GetFirstChild",						LuaWindow_GetFirstChild },
			{ "CorrectPos",							LuaWindow_CorrectPos },
			{ "SetSizeWithAllChild",				LuaWindow_SetSizeWithAllChild },
			{ "SetMousePenetrable",					LuaWindow_SetMousePenetrable },
			{ "SetAlpha",							LuaWindow_SetAlpha },
			{ "SetSelfAlpha",						LuaWindow_SetSelfAlpha },
			{ "GetAlpha",							LuaWindow_GetAlpha },
			{ "GetType",							LuaWindow_GetType },
			{ "ChangeRelation",						LuaWindow_ChangeRelation },
			{ "RebuildEventArray",					LuaWindow_RebuildEventArray },
			{ "SetPoint",							LuaWindow_SetPoint },
			{ "SetAreaTestFile",					LuaWindow_SetAreaTestFile },
			{ "Destroy",							LuaWindow_Destroy},
			{ "GetTreePath",						LuaWindow_GetTreePath },
			{ "StartMoving",						LuaWindow_StartMoving },
			{ "EndMoving",							LuaWindow_EndMoving },

			{ "IsValid",							LuaWindow_IsValid },
//			{ "__newindex",							LuaWindow_NewIndex },
			{ "__eq",								LuaWindow_Equal },
			

			{ NULL,									NULL },
		};

		static luaL_reg const s_aWndEditMetaTable[] =
		{
			//------------WndWindow----------------------
			{ "GetRelPos",							LuaWindow_GetRelPos },
			{ "GetAbsPos",							LuaWindow_GetAbsPos },
			{ "GetSize",							LuaWindow_GetSize },
			{ "SetSize",							LuaWindow_SetSize },
			{ "IsVisible",							LuaWindow_IsVisible },
			{ "IsDisable",							LuaWindow_IsDisable },
			{ "SetRelPos",							LuaWindow_SetRelPos },
			{ "SetAbsPos",							LuaWindow_SetAbsPos },
			{ "SetCursorAbove",						LuaWindow_SetCursorAbove },
			{ "Enable",								LuaWindow_Enable },
			{ "Show",								LuaWindow_Show },
			{ "Hide",								LuaWindow_Hide },
			{ "ToggleVisible",						LuaWindow_ToggleVisible },
			{ "BringToTop",							LuaWindow_BringToTop },
			{ "Scale",							    LuaWindow_Scale },
			{ "CreateItemHandle",					LuaWindow_CreateItemHandle },
			{ "ReleaseItemHandle",					LuaWindow_ReleaseItemHandle },
			{ "Lookup",							    LuaWindow_Lookup },
			{ "GetName",							LuaWindow_GetName },
			{ "SetName",							LuaWindow_SetName },
			{ "GetPrev",							LuaWindow_GetPrev },
			{ "GetNext",							LuaWindow_GetNext },
			{ "GetParent",							LuaWindow_GetParent },
			{ "GetRoot",							LuaWindow_GetRoot },
			{ "GetFirstChild",						LuaWindow_GetFirstChild },
			{ "CorrectPos",							LuaWindow_CorrectPos },
			{ "SetSizeWithAllChild",				LuaWindow_SetSizeWithAllChild },
			{ "SetMousePenetrable",					LuaWindow_SetMousePenetrable },
			{ "SetAlpha",							LuaWindow_SetAlpha },
			{ "SetSelfAlpha",						LuaWindow_SetSelfAlpha },
			{ "GetAlpha",							LuaWindow_GetAlpha },
			{ "GetType",							LuaWindow_GetType },
			{ "IsValid",							LuaWindow_IsValid },
			{ "ChangeRelation",						LuaWindow_ChangeRelation },
			{ "RebuildEventArray",					LuaWindow_RebuildEventArray },
			{ "SetPoint",							LuaWindow_SetPoint },
			{ "SetAreaTestFile",					LuaWindow_SetAreaTestFile },
			{ "Destroy",							LuaWindow_Destroy},
			{ "GetTreePath",						LuaWindow_GetTreePath },
			{ "StartMoving",						LuaWindow_StartMoving },
			{ "EndMoving",							LuaWindow_EndMoving },

//			{ "__newindex",							LuaWindow_NewIndex },
			{ "__eq",								LuaWindow_Equal },

			{ "SetText",							LuaEdit_SetText },
			{ "GetText",							LuaEdit_GetText },
			{ "GetTextLength",						LuaEdit_GetTextLength },
			{ "ClearText",							LuaEdit_ClearText },
			{ "InsertObj",							LuaEdit_InsertObj },			
            { "GetTextStruct",                      LuaEdit_GetTextStruct },
			{ "SetType",							LuaEdit_SetType },
			{ "SetLimit",							LuaEdit_SetLimit },
			{ "GetLimit",							LuaEdit_GetLimit },
			{ "SetLimitMultiByte",					LuaEdit_SetLimitMultiByte },
			{ "IsLimitMultiByte",					LuaEdit_IsLimitMultiByte },
			{ "SelectAll",							LuaEdit_SelectAll },
			{ "CancelSelect",						LuaEdit_CancelSelect },
			{ "SetFontScheme",						LuaEdit_SetFontScheme },
			{ "GetFontScheme",						LuaEdit_GetFontScheme },
			{ "SetFontColor",						LuaEdit_SetFontColor },
			{ "InsertText",							LuaEdit_InsertText },
			{ "Backspace",							LuaEdit_Backspace },			
			{ "SetMultiLine",						LuaEdit_SetMultiLine },
			{ "IsMultiLine",						LuaEdit_IsMultiLine },
			{ "SetFontSpacing",						LuaEdit_SetFontSpacing },
			{ "SetRowSpacing",						LuaEdit_SetRowSpacing },
			{ "SetFocusBgColor",					LuaEdit_SetFocusBgColor },
			{ "SetSelectBgColor",					LuaEdit_SetSelectBgColor },
			{ "SetSelectFontScheme",				LuaEdit_SetSelectFontScheme },
			{ "SetCurSel",							LuaEdit_SetCurSel },

			{ NULL,									NULL },
		};

		static luaL_reg const s_aWndButtonMetaTable[] =
		{
			//------------WndWindow----------------------
			{ "GetRelPos",							LuaWindow_GetRelPos },
			{ "GetAbsPos",							LuaWindow_GetAbsPos },
			{ "GetSize",							LuaWindow_GetSize },
			{ "SetSize",							LuaWindow_SetSize },
			{ "IsVisible",							LuaWindow_IsVisible },
			{ "IsDisable",							LuaWindow_IsDisable },
			{ "SetRelPos",							LuaWindow_SetRelPos },
			{ "SetAbsPos",							LuaWindow_SetAbsPos },
			{ "SetCursorAbove",						LuaWindow_SetCursorAbove },
			{ "Enable",								LuaWindow_Enable },
			{ "Show",								LuaWindow_Show },
			{ "Hide",								LuaWindow_Hide },
			{ "ToggleVisible",						LuaWindow_ToggleVisible },
			{ "BringToTop",							LuaWindow_BringToTop },
			{ "Scale",							    LuaWindow_Scale },
			{ "CreateItemHandle",					LuaWindow_CreateItemHandle },
			{ "ReleaseItemHandle",					LuaWindow_ReleaseItemHandle },
			{ "Lookup",							    LuaWindow_Lookup },
			{ "GetName",							LuaWindow_GetName },
			{ "SetName",							LuaWindow_SetName },
			{ "GetPrev",							LuaWindow_GetPrev },
			{ "GetNext",							LuaWindow_GetNext },
			{ "GetParent",							LuaWindow_GetParent },
			{ "GetRoot",							LuaWindow_GetRoot },
			{ "GetFirstChild",						LuaWindow_GetFirstChild },
			{ "CorrectPos",							LuaWindow_CorrectPos},
			{ "SetSizeWithAllChild",				LuaWindow_SetSizeWithAllChild },
			{ "SetMousePenetrable",					LuaWindow_SetMousePenetrable },
			{ "SetAlpha",							LuaWindow_SetAlpha },
			{ "SetSelfAlpha",						LuaWindow_SetSelfAlpha },
			{ "GetAlpha",							LuaWindow_GetAlpha },
			{ "GetType",							LuaWindow_GetType },
			{ "IsValid",							LuaWindow_IsValid },
			{ "ChangeRelation",						LuaWindow_ChangeRelation },
			{ "RebuildEventArray",					LuaWindow_RebuildEventArray },
			{ "SetPoint",							LuaWindow_SetPoint },
			{ "SetAreaTestFile",					LuaWindow_SetAreaTestFile },
			{ "Destroy",							LuaWindow_Destroy},
			{ "GetTreePath",						LuaWindow_GetTreePath },
			{ "StartMoving",						LuaWindow_StartMoving },
			{ "EndMoving",							LuaWindow_EndMoving },

//			{ "__newindex",							LuaWindow_NewIndex },
			{ "__eq",								LuaWindow_Equal },

			//------------WndButton----------------------
			{ "IsEnabled",							LuaButton_IsEnabled },
			{ "Enable",								LuaButton_Enable },
			{ "SetAnimateGroupNormal",				LuaButton_SetAnimateGroupNormal },
			{ "SetAnimateGroupMouseOver",			LuaButton_SetAnimateGroupMouseOver },
			{ "SetAnimateGroupMouseDown",			LuaButton_SetAnimateGroupMouseDown },
			{ "SetAnimateGroupDisable",				LuaButton_SetAnimateGroupDisable },
			{ "RegisterLButtonDrag",			    LuaButton_RegisterLButtonDrag },
			{ "UnregisterLButtonDrag",			    LuaButton_UnregisterLButtonDrag },
			{ "IsLButtonDragable",				    LuaButton_IsLButtonDragable },
			{ "RegisterRButtonDrag",			    LuaButton_RegisterRButtonDrag },
			{ "UnregisterRButtonDrag",			    LuaButton_UnregisterRButtonDrag },
			{ "IsRButtonDragable",				    LuaButton_IsRButtonDragable },
			{ NULL,									NULL },
		};

		static luaL_reg const s_aWndCheckBoxMetaTable[] =
		{
			//------------WndWindow----------------------
			{ "GetRelPos",							LuaWindow_GetRelPos },
			{ "GetAbsPos",							LuaWindow_GetAbsPos },
			{ "GetSize",							LuaWindow_GetSize },
			{ "SetSize",							LuaWindow_SetSize },
			{ "IsVisible",							LuaWindow_IsVisible },
			{ "IsDisable",							LuaWindow_IsDisable },
			{ "SetRelPos",							LuaWindow_SetRelPos },
			{ "SetAbsPos",							LuaWindow_SetAbsPos },
			{ "SetCursorAbove",						LuaWindow_SetCursorAbove },
			{ "Enable",								LuaWindow_Enable },
			{ "Show",								LuaWindow_Show },
			{ "Hide",								LuaWindow_Hide },
			{ "ToggleVisible",						LuaWindow_ToggleVisible },
			{ "BringToTop",							LuaWindow_BringToTop },
			{ "Scale",							    LuaWindow_Scale },
			{ "CreateItemHandle",					LuaWindow_CreateItemHandle },
			{ "ReleaseItemHandle",					LuaWindow_ReleaseItemHandle },
			{ "Lookup",							    LuaWindow_Lookup },
			{ "GetName",							LuaWindow_GetName },
			{ "SetName",							LuaWindow_SetName },
			{ "GetPrev",							LuaWindow_GetPrev },
			{ "GetNext",							LuaWindow_GetNext },
			{ "GetParent",							LuaWindow_GetParent },
			{ "GetRoot",							LuaWindow_GetRoot },
			{ "GetFirstChild",						LuaWindow_GetFirstChild },
			{ "CorrectPos",							LuaWindow_CorrectPos},
			{ "SetSizeWithAllChild",				LuaWindow_SetSizeWithAllChild },
			{ "SetMousePenetrable",					LuaWindow_SetMousePenetrable },
			{ "SetAlpha",							LuaWindow_SetAlpha },
			{ "SetSelfAlpha",						LuaWindow_SetSelfAlpha },
			{ "GetAlpha",							LuaWindow_GetAlpha },
			{ "GetType",							LuaWindow_GetType },
			{ "IsValid",							LuaWindow_IsValid },
			{ "ChangeRelation",						LuaWindow_ChangeRelation },
			{ "RebuildEventArray",					LuaWindow_RebuildEventArray },
			{ "SetPoint",							LuaWindow_SetPoint },
			{ "SetAreaTestFile",					LuaWindow_SetAreaTestFile },
			{ "Destroy",							LuaWindow_Destroy},
			{ "GetTreePath",						LuaWindow_GetTreePath },
			{ "StartMoving",						LuaWindow_StartMoving },
			{ "EndMoving",							LuaWindow_EndMoving },

//			{ "__newindex",							LuaWindow_NewIndex },
			{ "__eq",								LuaWindow_Equal },

			//------------WndCheckBox----------------------
			{ "IsCheckBoxActive",					LuaCheckBox_IsCheckBoxActive },
			{ "Enable",								LuaCheckBox_Enable },
			{ "IsCheckBoxChecked",					LuaCheckBox_IsCheckBoxChecked },
			{ "Check",								LuaCheckBox_Check },
			{ "ToggleCheck",						LuaCheckBox_ToggleCheck },
			{ "SetAnimation",						LuaCheckBox_SetAnimation },
			{ NULL,									NULL },
		};

		static luaL_reg const s_aWndNewScrollBarMetaTable[] =
		{
			//------------WndWindow----------------------
			{ "GetRelPos",							LuaWindow_GetRelPos },
			{ "GetAbsPos",							LuaWindow_GetAbsPos },
			{ "GetSize",							LuaWindow_GetSize },
			{ "SetSize",							LuaWindow_SetSize },
			{ "IsVisible",							LuaWindow_IsVisible },
			{ "IsDisable",							LuaWindow_IsDisable },
			{ "SetRelPos",							LuaWindow_SetRelPos },
			{ "SetAbsPos",							LuaWindow_SetAbsPos },
			{ "SetCursorAbove",						LuaWindow_SetCursorAbove },
			{ "Enable",								LuaWindow_Enable },
			{ "Show",								LuaWindow_Show },
			{ "Hide",								LuaWindow_Hide },
			{ "ToggleVisible",						LuaWindow_ToggleVisible },
			{ "BringToTop",							LuaWindow_BringToTop },
			{ "Scale",							    LuaWindow_Scale },
			{ "CreateItemHandle",					LuaWindow_CreateItemHandle },
			{ "ReleaseItemHandle",					LuaWindow_ReleaseItemHandle },
			{ "Lookup",							    LuaWindow_Lookup },
			{ "GetName",							LuaWindow_GetName },
			{ "SetName",							LuaWindow_SetName },
			{ "GetPrev",							LuaWindow_GetPrev },
			{ "GetNext",							LuaWindow_GetNext },
			{ "GetParent",							LuaWindow_GetParent },
			{ "GetRoot",							LuaWindow_GetRoot },
			{ "GetFirstChild",						LuaWindow_GetFirstChild },
			{ "CorrectPos",							LuaWindow_CorrectPos},
			{ "SetSizeWithAllChild",				LuaWindow_SetSizeWithAllChild },
			{ "SetMousePenetrable",					LuaWindow_SetMousePenetrable },
			{ "SetAlpha",							LuaWindow_SetAlpha },
			{ "SetSelfAlpha",						LuaWindow_SetSelfAlpha },
			{ "GetAlpha",							LuaWindow_GetAlpha },
			{ "GetType",							LuaWindow_GetType },
			{ "IsValid",							LuaWindow_IsValid },
			{ "ChangeRelation",						LuaWindow_ChangeRelation },
			{ "RebuildEventArray",					LuaWindow_RebuildEventArray },
			{ "SetPoint",							LuaWindow_SetPoint },
			{ "SetAreaTestFile",					LuaWindow_SetAreaTestFile },
			{ "Destroy",							LuaWindow_Destroy},
			{ "GetTreePath",						LuaWindow_GetTreePath },
			{ "StartMoving",						LuaWindow_StartMoving },
			{ "EndMoving",							LuaWindow_EndMoving },

//			{ "__newindex",							LuaWindow_NewIndex },
			{ "__eq",								LuaWindow_Equal },

			{ "SetScrollPos",						LuaNewScrollBar_SetScrollPos },
			{ "GetScrollPos",						LuaNewScrollBar_GetScrollPos },
			{ "SetStepCount",						LuaNewScrollBar_SetStepCount },
			{ "GetStepCount",						LuaNewScrollBar_GetStepCount },
			{ "SetPageStepCount",					LuaNewScrollBar_SetPageStepCount },
			{ "GetPageStepCount",					LuaNewScrollBar_GetPageStepCount },
			{ "ScrollPrev",							LuaNewScrollBar_ScrollPrev },
			{ "ScrollNext",							LuaNewScrollBar_ScrollNext },
			{ "ScrollPagePrev",						LuaNewScrollBar_ScrollPagePrev },
			{ "ScrollPageNext",						LuaNewScrollBar_ScrollPageNext },
			{ "ScrollHome",							LuaNewScrollBar_ScrollHome },
			{ "ScrollEnd",							LuaNewScrollBar_ScrollEnd },
			{ NULL,									NULL },
		};

		static luaL_reg const s_aWndSceneMetaTable[] =
		{
			//------------WndWindow----------------------
			{ "GetRelPos",							LuaWindow_GetRelPos },
			{ "GetAbsPos",							LuaWindow_GetAbsPos },
			{ "GetSize",							LuaWindow_GetSize },
			{ "SetSize",							LuaWindow_SetSize },
			{ "IsVisible",							LuaWindow_IsVisible },
			{ "IsDisable",							LuaWindow_IsDisable },
			{ "SetRelPos",							LuaWindow_SetRelPos },
			{ "SetAbsPos",							LuaWindow_SetAbsPos },
			{ "SetCursorAbove",						LuaWindow_SetCursorAbove },
			{ "Enable",								LuaWindow_Enable },
			{ "Show",								LuaWindow_Show },
			{ "Hide",								LuaWindow_Hide },
			{ "ToggleVisible",						LuaWindow_ToggleVisible },
			{ "BringToTop",							LuaWindow_BringToTop },
			{ "Scale",							    LuaWindow_Scale },
			{ "CreateItemHandle",					LuaWindow_CreateItemHandle },
			{ "ReleaseItemHandle",					LuaWindow_ReleaseItemHandle },
			{ "Lookup",							    LuaWindow_Lookup },
			{ "GetName",							LuaWindow_GetName },
			{ "SetName",							LuaWindow_SetName },
			{ "GetPrev",							LuaWindow_GetPrev },
			{ "GetNext",							LuaWindow_GetNext },
			{ "GetParent",							LuaWindow_GetParent },
			{ "GetRoot",							LuaWindow_GetRoot },
			{ "GetFirstChild",						LuaWindow_GetFirstChild },
			{ "CorrectPos",							LuaWindow_CorrectPos},
			{ "SetSizeWithAllChild",				LuaWindow_SetSizeWithAllChild },
			{ "SetMousePenetrable",					LuaWindow_SetMousePenetrable },
			{ "SetAlpha",							LuaWindow_SetAlpha },
			{ "SetSelfAlpha",						LuaWindow_SetSelfAlpha },
			{ "GetAlpha",							LuaWindow_GetAlpha },
			{ "GetType",							LuaWindow_GetType },
			{ "IsValid",							LuaWindow_IsValid },
			{ "ChangeRelation",						LuaWindow_ChangeRelation },
			{ "RebuildEventArray",					LuaWindow_RebuildEventArray },
			{ "SetPoint",							LuaWindow_SetPoint },
			{ "SetAreaTestFile",					LuaWindow_SetAreaTestFile },
			{ "Destroy",							LuaWindow_Destroy},
			{ "GetTreePath",						LuaWindow_GetTreePath },
			{ "StartMoving",						LuaWindow_StartMoving },
			{ "EndMoving",							LuaWindow_EndMoving },

//			{ "__newindex",							LuaWindow_NewIndex },
			{ "__eq",								LuaWindow_Equal },

			{ "SetScene",							LuaScene_SetScene },
			{ "EnableRenderTerrain",				LuaScene_EnableRenderTerrain },
			{ "EnableRenderSkyBox",					LuaScene_EnableRenderSkyBox },

			{ NULL,									NULL },
		};

		static luaL_reg const s_aWndMovieMetaTable[] =
		{
			//------------WndWindow----------------------
			{ "GetRelPos",							LuaWindow_GetRelPos },
			{ "GetAbsPos",							LuaWindow_GetAbsPos },
			{ "GetSize",							LuaWindow_GetSize },
			{ "SetSize",							LuaWindow_SetSize },
			{ "IsVisible",							LuaWindow_IsVisible },
			{ "IsDisable",							LuaWindow_IsDisable },
			{ "SetRelPos",							LuaWindow_SetRelPos },
			{ "SetAbsPos",							LuaWindow_SetAbsPos },
			{ "SetCursorAbove",						LuaWindow_SetCursorAbove },
			{ "Enable",								LuaWindow_Enable },
			{ "Show",								LuaWindow_Show },
			{ "Hide",								LuaWindow_Hide },
			{ "ToggleVisible",						LuaWindow_ToggleVisible },
			{ "BringToTop",							LuaWindow_BringToTop },
			{ "Scale",							    LuaWindow_Scale },
			{ "CreateItemHandle",					LuaWindow_CreateItemHandle },
			{ "ReleaseItemHandle",					LuaWindow_ReleaseItemHandle },
			{ "Lookup",							    LuaWindow_Lookup },
			{ "GetName",							LuaWindow_GetName },
			{ "SetName",							LuaWindow_SetName },
			{ "GetPrev",							LuaWindow_GetPrev },
			{ "GetNext",							LuaWindow_GetNext },
			{ "GetParent",							LuaWindow_GetParent },
			{ "GetRoot",							LuaWindow_GetRoot },
			{ "GetFirstChild",						LuaWindow_GetFirstChild },
			{ "CorrectPos",							LuaWindow_CorrectPos},
			{ "SetSizeWithAllChild",				LuaWindow_SetSizeWithAllChild },
			{ "SetMousePenetrable",					LuaWindow_SetMousePenetrable },
			{ "SetAlpha",							LuaWindow_SetAlpha },
			{ "SetSelfAlpha",						LuaWindow_SetSelfAlpha },
			{ "GetAlpha",							LuaWindow_GetAlpha },
			{ "GetType",							LuaWindow_GetType },
			{ "IsValid",							LuaWindow_IsValid },
			{ "ChangeRelation",						LuaWindow_ChangeRelation },
			{ "RebuildEventArray",					LuaWindow_RebuildEventArray },
			{ "SetPoint",							LuaWindow_SetPoint },
			{ "SetAreaTestFile",					LuaWindow_SetAreaTestFile },
			{ "Destroy",							LuaWindow_Destroy},
			{ "GetTreePath",						LuaWindow_GetTreePath },
			{ "StartMoving",						LuaWindow_StartMoving },
			{ "EndMoving",							LuaWindow_EndMoving },

//			{ "__newindex",							LuaWindow_NewIndex },
			{ "__eq",								LuaWindow_Equal },

			{ "Play",								LuaMovie_Play },
			{ "Stop",								LuaMovie_Stop },
			{ "SetVolume",							LuaMovie_SetVolume },
			{ "IsFinished",							LuaMovie_IsFinished },

			{ NULL,									NULL },
		};

		static luaL_reg const s_aWndTextureMetaTable[] =
		{
			//------------WndWindow----------------------
			{ "GetRelPos",							LuaWindow_GetRelPos },
			{ "GetAbsPos",							LuaWindow_GetAbsPos },
			{ "GetSize",							LuaWindow_GetSize },
			{ "SetSize",							LuaWindow_SetSize },
			{ "IsVisible",							LuaWindow_IsVisible },
			{ "IsDisable",							LuaWindow_IsDisable },
			{ "SetRelPos",							LuaWindow_SetRelPos },
			{ "SetAbsPos",							LuaWindow_SetAbsPos },
			{ "SetCursorAbove",						LuaWindow_SetCursorAbove },
			{ "Enable",								LuaWindow_Enable },
			{ "Show",								LuaWindow_Show },
			{ "Hide",								LuaWindow_Hide },
			{ "ToggleVisible",						LuaWindow_ToggleVisible },
			{ "BringToTop",							LuaWindow_BringToTop },
			{ "Scale",							    LuaWindow_Scale },
			{ "CreateItemHandle",					LuaWindow_CreateItemHandle },
			{ "ReleaseItemHandle",					LuaWindow_ReleaseItemHandle },
			{ "Lookup",							    LuaWindow_Lookup },
			{ "GetName",							LuaWindow_GetName },
			{ "SetName",							LuaWindow_SetName },
			{ "GetPrev",							LuaWindow_GetPrev },
			{ "GetNext",							LuaWindow_GetNext },
			{ "GetParent",							LuaWindow_GetParent },
			{ "GetRoot",							LuaWindow_GetRoot },
			{ "GetFirstChild",						LuaWindow_GetFirstChild },
			{ "CorrectPos",							LuaWindow_CorrectPos},
			{ "SetSizeWithAllChild",				LuaWindow_SetSizeWithAllChild },
			{ "SetMousePenetrable",					LuaWindow_SetMousePenetrable },
			{ "SetAlpha",							LuaWindow_SetAlpha },
			{ "SetSelfAlpha",						LuaWindow_SetSelfAlpha },
			{ "GetAlpha",							LuaWindow_GetAlpha },
			{ "GetType",							LuaWindow_GetType },
			{ "IsValid",							LuaWindow_IsValid },
			{ "ChangeRelation",						LuaWindow_ChangeRelation },
			{ "RebuildEventArray",					LuaWindow_RebuildEventArray },
			{ "SetPoint",							LuaWindow_SetPoint },
			{ "SetAreaTestFile",					LuaWindow_SetAreaTestFile },
			{ "Destroy",							LuaWindow_Destroy},
			{ "GetTreePath",						LuaWindow_GetTreePath },
			{ "StartMoving",						LuaWindow_StartMoving },
			{ "EndMoving",							LuaWindow_EndMoving },

//			{ "__newindex",							LuaWindow_NewIndex },
			{ "__eq",								LuaWindow_Equal },

			{ "SetTexture",							LuaTexture_SetTexture },

			{ NULL,									NULL },
		};

		static luaL_reg const s_aWndPageSetMetaTable[] =
		{
			//------------WndWindow----------------------
			{ "GetRelPos",							LuaWindow_GetRelPos },
			{ "GetAbsPos",							LuaWindow_GetAbsPos },
			{ "GetSize",							LuaWindow_GetSize },
			{ "SetSize",							LuaWindow_SetSize },
			{ "IsVisible",							LuaWindow_IsVisible },
			{ "IsDisable",							LuaWindow_IsDisable },
			{ "SetRelPos",							LuaWindow_SetRelPos },
			{ "SetAbsPos",							LuaWindow_SetAbsPos },
			{ "SetCursorAbove",						LuaWindow_SetCursorAbove },
			{ "Enable",								LuaWindow_Enable },
			{ "Show",								LuaWindow_Show },
			{ "Hide",								LuaWindow_Hide },
			{ "ToggleVisible",						LuaWindow_ToggleVisible },
			{ "BringToTop",							LuaWindow_BringToTop },
			{ "Scale",							    LuaWindow_Scale },
			{ "CreateItemHandle",					LuaWindow_CreateItemHandle },
			{ "ReleaseItemHandle",					LuaWindow_ReleaseItemHandle },
			{ "Lookup",							    LuaWindow_Lookup },
			{ "GetName",							LuaWindow_GetName },
			{ "SetName",							LuaWindow_SetName },
			{ "GetPrev",							LuaWindow_GetPrev },
			{ "GetNext",							LuaWindow_GetNext },
			{ "GetParent",							LuaWindow_GetParent },
			{ "GetRoot",							LuaWindow_GetRoot },
			{ "GetFirstChild",						LuaWindow_GetFirstChild },
			{ "CorrectPos",							LuaWindow_CorrectPos},
			{ "SetSizeWithAllChild",				LuaWindow_SetSizeWithAllChild },
			{ "SetMousePenetrable",					LuaWindow_SetMousePenetrable },
			{ "SetAlpha",							LuaWindow_SetAlpha },
			{ "SetSelfAlpha",						LuaWindow_SetSelfAlpha },
			{ "GetAlpha",							LuaWindow_GetAlpha },
			{ "GetType",							LuaWindow_GetType },
			{ "IsValid",							LuaWindow_IsValid },
			{ "ChangeRelation",						LuaWindow_ChangeRelation },
			{ "RebuildEventArray",					LuaWindow_RebuildEventArray },
			{ "SetPoint",							LuaWindow_SetPoint },
			{ "SetAreaTestFile",					LuaWindow_SetAreaTestFile },
			{ "Destroy",							LuaWindow_Destroy},
			{ "GetTreePath",						LuaWindow_GetTreePath },
			{ "StartMoving",						LuaWindow_StartMoving },
			{ "EndMoving",							LuaWindow_EndMoving },

//			{ "__newindex",							LuaWindow_NewIndex },
			{ "__eq",								LuaWindow_Equal },

			{ "ActivePage",							LuaPageSet_ActivePage },
			{ "GetActivePageIndex",					LuaPageSet_GetActivePageIndex },
			{ "GetLastActivePageIndex",				LuaPageSet_GetLastActivePageIndex },
			{ "AddPage",							LuaPageSet_AddPage },
			{ "GetActivePage",						LuaPageSet_GetActivePage },
			{ "GetActiveCheckBox",					LuaPageSet_GetActiveCheckBox },
			
			{ NULL,									NULL },
		};

		static luaL_reg const s_aWndFrameMetaTable[] =
		{
			//------------WndWindow----------------------
			{ "GetRelPos",							LuaWindow_GetRelPos },
			{ "GetAbsPos",							LuaWindow_GetAbsPos },
			{ "GetSize",							LuaWindow_GetSize },
			{ "SetSize",							LuaWindow_SetSize },
			{ "IsVisible",							LuaWindow_IsVisible },
			{ "IsDisable",							LuaWindow_IsDisable },
			{ "SetRelPos",							LuaWindow_SetRelPos },
			{ "SetAbsPos",							LuaWindow_SetAbsPos },
			{ "SetCursorAbove",						LuaWindow_SetCursorAbove },
			{ "Enable",								LuaWindow_Enable },
			{ "Show",								LuaWindow_Show },
			{ "Hide",								LuaWindow_Hide },
			{ "ToggleVisible",						LuaWindow_ToggleVisible },
			{ "BringToTop",							LuaWindow_BringToTop },
			{ "Scale",							    LuaWindow_Scale },
			{ "CreateItemHandle",					LuaWindow_CreateItemHandle },
			{ "ReleaseItemHandle",					LuaWindow_ReleaseItemHandle },
			{ "Lookup",							    LuaWindow_Lookup },
			{ "GetName",							LuaWindow_GetName },
			{ "SetName",							LuaWindow_SetName },
			{ "GetPrev",							LuaWindow_GetPrev },
			{ "GetNext",							LuaWindow_GetNext },
			{ "GetParent",							LuaWindow_GetParent },
			{ "GetRoot",							LuaWindow_GetRoot },
			{ "GetFirstChild",						LuaWindow_GetFirstChild },
			{ "CorrectPos",							LuaWindow_CorrectPos},
			{ "SetSizeWithAllChild",				LuaWindow_SetSizeWithAllChild },
			{ "SetMousePenetrable",					LuaWindow_SetMousePenetrable },
			{ "SetAlpha",							LuaWindow_SetAlpha },
			{ "SetSelfAlpha",						LuaWindow_SetSelfAlpha },
			{ "GetAlpha",							LuaWindow_GetAlpha },
			{ "GetType",							LuaWindow_GetType },
			{ "IsValid",							LuaWindow_IsValid },
			{ "ChangeRelation",						LuaWindow_ChangeRelation },
			{ "RebuildEventArray",					LuaWindow_RebuildEventArray },
			{ "SetPoint",							LuaWindow_SetPoint },
			{ "SetAreaTestFile",					LuaWindow_SetAreaTestFile },
			{ "Destroy",							LuaWindow_Destroy},
			{ "GetTreePath",						LuaWindow_GetTreePath },
			{ "StartMoving",						LuaWindow_StartMoving },
			{ "EndMoving",							LuaWindow_EndMoving },

//			{ "__newindex",							LuaWindow_NewIndex },
			{ "__eq",								LuaWindow_Equal },

            { "EnableDrag",							LuaFrame_EnableDrag },
            { "IsDragable",							LuaFrame_IsDragable },
			{ "EnableFollowMouse",					LuaFrame_EnableFollowMouse },
			{ "IsFollowMouseMove",					LuaFrame_IsFollowMouseMove },
			{ "SetDragArea",						LuaFrame_SetDragArea },
			{ "RegisterEvent",						LuaFrame_RegisterEvent },
			{ "FocusPrev",							LuaFrame_FocusPrev },
			{ "FocusNext",							LuaFrame_FocusNext },
			{ "FocusHome",							LuaFrame_FocusHome },
			{ "FocusEnd",							LuaFrame_FocusEnd },
			{ "GetSelf",							LuaFrame_GetSelf },
			{ "FadeIn",								LuaFrame_FadeIn },
			{ "FadeOut",							LuaFrame_FadeOut },
			{ "ShowWhenUIHide",						LuaFrame_ShowWhenUIHide },
			{ "HideWhenUIHide",						LuaFrame_HideWhenUIHide },
			{ "IsVisibleWhenUIHide",				LuaFrame_IsVisibleWhenUIHide },

			{ NULL,									NULL },
		};

		static luaL_reg const s_aWndMinimapMetaTable[] =
		{
			//------------WndWindow----------------------
			{ "GetRelPos",							LuaWindow_GetRelPos },
			{ "GetAbsPos",							LuaWindow_GetAbsPos },
			{ "GetSize",							LuaWindow_GetSize },
			{ "SetSize",							LuaWindow_SetSize },
			{ "IsVisible",							LuaWindow_IsVisible },
			{ "IsDisable",							LuaWindow_IsDisable },
			{ "SetRelPos",							LuaWindow_SetRelPos },
			{ "SetAbsPos",							LuaWindow_SetAbsPos },
			{ "SetCursorAbove",						LuaWindow_SetCursorAbove },
			{ "Enable",								LuaWindow_Enable },
			{ "Show",								LuaWindow_Show },
			{ "Hide",								LuaWindow_Hide },
			{ "ToggleVisible",						LuaWindow_ToggleVisible },
			{ "BringToTop",							LuaWindow_BringToTop },
			{ "Scale",							    LuaWindow_Scale },
			{ "CreateItemHandle",					LuaWindow_CreateItemHandle },
			{ "ReleaseItemHandle",					LuaWindow_ReleaseItemHandle },
			{ "Lookup",							    LuaWindow_Lookup },
			{ "GetName",							LuaWindow_GetName },
			{ "SetName",							LuaWindow_SetName },
			{ "GetPrev",							LuaWindow_GetPrev },
			{ "GetNext",							LuaWindow_GetNext },
			{ "GetParent",							LuaWindow_GetParent },
			{ "GetRoot",							LuaWindow_GetRoot },
			{ "GetFirstChild",						LuaWindow_GetFirstChild },
			{ "CorrectPos",							LuaWindow_CorrectPos},
			{ "SetSizeWithAllChild",				LuaWindow_SetSizeWithAllChild },
			{ "SetMousePenetrable",					LuaWindow_SetMousePenetrable },
			{ "SetAlpha",							LuaWindow_SetAlpha },
			{ "SetSelfAlpha",						LuaWindow_SetSelfAlpha },
			{ "GetAlpha",							LuaWindow_GetAlpha },
			{ "GetType",							LuaWindow_GetType },
			{ "IsValid",							LuaWindow_IsValid },
			{ "ChangeRelation",						LuaWindow_ChangeRelation },
			{ "SetPoint",							LuaWindow_SetPoint },
			{ "SetAreaTestFile",					LuaWindow_SetAreaTestFile },
			{ "Destroy",							LuaWindow_Destroy},
			{ "GetTreePath",						LuaWindow_GetTreePath },
			{ "StartMoving",						LuaWindow_StartMoving },
			{ "EndMoving",							LuaWindow_EndMoving },

//			{ "__newindex",							LuaWindow_NewIndex },
			{ "__eq",								LuaWindow_Equal },

			{ "SetMapPath",							LuaMinimap_SetMapPath },
			{ "SetScale",							LuaMinimap_SetScale },
			{ "GetScale",							LuaMinimap_GetScale },
			{ "UpdataStaticPoint",					LuaMinimap_UpdataStaticPoint },
			{ "UpdataAnimatePoint",					LuaMinimap_UpdataAnimatePoint },
			{ "UpdataArrowPoint",					LuaMinimap_UpdataArrowPoint },
			{ "RemovePoint",						LuaMinimap_RemovePoint },
			{ "UpdataSelfPos",						LuaMinimap_UpdataSelfPos },
			{ "Clear",								LuaMinimap_Clear },
			{ "GetOverObj",							LuaMinimap_GetOverObj },
			{ "GetSendPos",							LuaMinimap_GetSendPos },
			
			{ NULL,									NULL },
		};

		static luaL_reg const s_aWndWebPageMetaTable[] =
		{
			//------------WndWindow----------------------
			{ "GetRelPos",							LuaWindow_GetRelPos },
			{ "GetAbsPos",							LuaWindow_GetAbsPos },
			{ "GetSize",							LuaWindow_GetSize },
			{ "SetSize",							LuaWindow_SetSize },
			{ "IsVisible",							LuaWindow_IsVisible },
			{ "IsDisable",							LuaWindow_IsDisable },
			{ "SetRelPos",							LuaWindow_SetRelPos },
			{ "SetAbsPos",							LuaWindow_SetAbsPos },
			{ "SetCursorAbove",						LuaWindow_SetCursorAbove },
			{ "Enable",								LuaWindow_Enable },
			{ "Show",								LuaWindow_Show },
			{ "Hide",								LuaWindow_Hide },
			{ "ToggleVisible",						LuaWindow_ToggleVisible },
			{ "BringToTop",							LuaWindow_BringToTop },
			{ "Scale",							    LuaWindow_Scale },
			{ "CreateItemHandle",					LuaWindow_CreateItemHandle },
			{ "ReleaseItemHandle",					LuaWindow_ReleaseItemHandle },
			{ "Lookup",							    LuaWindow_Lookup },
			{ "GetName",							LuaWindow_GetName },
			{ "SetName",							LuaWindow_SetName },
			{ "GetPrev",							LuaWindow_GetPrev },
			{ "GetNext",							LuaWindow_GetNext },
			{ "GetParent",							LuaWindow_GetParent },
			{ "GetRoot",							LuaWindow_GetRoot },
			{ "GetFirstChild",						LuaWindow_GetFirstChild },
			{ "CorrectPos",							LuaWindow_CorrectPos},
			{ "SetSizeWithAllChild",				LuaWindow_SetSizeWithAllChild },
			{ "SetMousePenetrable",					LuaWindow_SetMousePenetrable },
			{ "SetAlpha",							LuaWindow_SetAlpha },
			{ "SetSelfAlpha",						LuaWindow_SetSelfAlpha },
			{ "GetAlpha",							LuaWindow_GetAlpha },
			{ "GetType",							LuaWindow_GetType },
			{ "IsValid",							LuaWindow_IsValid },
			{ "ChangeRelation",						LuaWindow_ChangeRelation },
			{ "RebuildEventArray",					LuaWindow_RebuildEventArray },
			{ "SetPoint",							LuaWindow_SetPoint },
			{ "SetAreaTestFile",					LuaWindow_SetAreaTestFile },
			{ "Destroy",							LuaWindow_Destroy},
			{ "GetTreePath",						LuaWindow_GetTreePath },
			{ "StartMoving",						LuaWindow_StartMoving },
			{ "EndMoving",							LuaWindow_EndMoving },

//			{ "__newindex",							LuaWindow_NewIndex },
			{ "__eq",								LuaWindow_Equal },

			{ "Navigate",							LuaWebPage_Navigate },
			{ "CanGoBack",							LuaWebPage_CanGoBack },
			{ "CanGoForward",						LuaWebPage_CanGoForward },
			{ "GoBack",								LuaWebPage_GoBack },
			{ "GoForward",							LuaWebPage_GoForward },
			{ "Refresh",							LuaWebPage_Refresh },
			{ "GetLocationName",					LuaWebPage_GetLocationName },
			{ "GetLocationURL",						LuaWebPage_GetLocationURL },

			{ NULL,									NULL },
		};

        KScriptLoader::RegisterLibTable(L, "Cursor", s_aCursorLibTable);
		KScriptLoader::RegisterLibTable(L, "Wnd", s_aWndLibTable);
		KScriptLoader::RegisterLibTable(L, "Station", s_aStationLibTable);

		KScriptLoader::RegisterMetaTable(L, "WndWindow", s_aWndWindowMetaTable);
		KScriptLoader::RegisterMetaTable(L, "WndEdit",	s_aWndEditMetaTable);
		KScriptLoader::RegisterMetaTable(L, "WndButton", s_aWndButtonMetaTable);
		KScriptLoader::RegisterMetaTable(L, "WndCheckBox", s_aWndCheckBoxMetaTable);
		KScriptLoader::RegisterMetaTable(L, "WndNewScrollBar", s_aWndNewScrollBarMetaTable);
		KScriptLoader::RegisterMetaTable(L, "WndScene", s_aWndSceneMetaTable);
		KScriptLoader::RegisterMetaTable(L, "WndMovie", s_aWndMovieMetaTable);
		KScriptLoader::RegisterMetaTable(L, "WndTexture", s_aWndTextureMetaTable);
		KScriptLoader::RegisterMetaTable(L, "WndPageSet", s_aWndPageSetMetaTable);
		KScriptLoader::RegisterMetaTable(L, "WndPage", s_aWndWindowMetaTable);
		KScriptLoader::RegisterMetaTable(L, "WndFrame", s_aWndFrameMetaTable);
		KScriptLoader::RegisterMetaTable(L, "WndMinimap", s_aWndMinimapMetaTable);
		KScriptLoader::RegisterMetaTable(L, "WndWebPage", s_aWndWebPageMetaTable);
	}


	void * KWndClassScriptTable::ToWndUserData(LPCSTR szWndType, lua_State* L, int nIndex)
	{		
		int nResult = false;
		KWndWindow* pWnd = NULL;

		ASSERT(L);

		int nTopIndex = lua_gettop(L);
		ASSERT(nTopIndex > 0);

		KGLOG_PROCESS_ERROR(lua_istable(L, nIndex));
		lua_pushvalue(L, nIndex);

		lua_getfield(L, -1, STR_OBJ_KEY_NAME);

		pWnd = reinterpret_cast<KWndWindow*>(lua_touserdata(L, -1)); 
		KG_PROCESS_ERROR(pWnd);

		KGLOG_PROCESS_ERROR(pWnd->CanCastTo(szWndType));

		nResult = true;
Exit0:
		lua_settop(L, nTopIndex);
		return nResult ? pWnd : NULL;
	}

	int KWndClassScriptTable::HostStation_Lookup(lua_State* L, KWndWindow *pWnd)
	{
		int nResult = false;
		LPCSTR pcszMetalTable = NULL;

		KG_PROCESS_ERROR(pWnd);

		if (pWnd->GetLuaRef() != LUA_NOREF)
		{
			lua_rawgeti(L, LUA_REGISTRYINDEX, pWnd->GetLuaRef());
			goto Exit1;
		}

		pcszMetalTable = pWnd->GetWndType();
		KGLOG_PROCESS_ERROR(pcszMetalTable);

		lua_newtable(L);
		lua_pushstring(L, STR_OBJ_KEY_NAME);
		lua_pushlightuserdata(L, pWnd);
		lua_settable(L, -3);

		luaL_getmetatable(L, pcszMetalTable);
		lua_setmetatable(L, -2);

		lua_pushvalue(L, -1);
		pWnd->SetLuaRef(luaL_ref(L, LUA_REGISTRYINDEX));

Exit1:
		nResult = true;
Exit0:
		return nResult;
	}

	int KWndClassScriptTable::LuaStation_Lookup(lua_State* L)
	{
		int nRetCode = false;
		int nParamCount = 0;
        LPCSTR szTreePath = NULL;
        KWndWindow* pWnd = NULL;
		KItemNull* pItemNull = NULL;

		ASSERT(L);

		nParamCount = Lua_GetTopIndex(L);		
		KGLOG_PROCESS_ERROR(nParamCount > 0);

		szTreePath = lua_tostring(L, 1);
		KG_PROCESS_ERROR(szTreePath);

		pWnd = g_pUI->m_WndStation.SearchWindow(szTreePath);
		KG_PROCESS_ERROR(pWnd);

		if (nParamCount == 1)
		{
			nRetCode = HostStation_Lookup(L, pWnd);
			KG_PROCESS_ERROR(nRetCode);
		}
        else
        {
            LPCSTR szItemTreePath = NULL;
            KItemHandle* pHandle = NULL;
            
            szItemTreePath = lua_tostring(L, 2);
            KGLOG_PROCESS_ERROR(szItemTreePath);

            pHandle = pWnd->GetItemHandle();
            KG_PROCESS_ERROR(pHandle);

            if (szItemTreePath[0] == '\0')
            {
                pItemNull = g_ItemCast<KItemNull>(pHandle);
                KG_PROCESS_ERROR(pItemNull);
            }
            else
            {
                pItemNull = pHandle->GetItemByTreePath(szItemTreePath);
                KG_PROCESS_ERROR(pItemNull);
            }

            nRetCode = KItemHandleClassScriptTable::HostWndItem_Lookup(L, pItemNull);
            KG_PROCESS_ERROR(nRetCode);
        }

        return 1;
Exit0:
		return 0;
	}

	int KWndClassScriptTable::LuaStation_GetStandardClientSize(Lua_State* L)
	{
		float fW = g_pUI->m_WndStation.GetStandardWidth();
		float fH = g_pUI->m_WndStation.GetStandardHeight();

		Lua_PushNumber(L, fW);
		Lua_PushNumber(L, fH);
		return 2;
	}

	int KWndClassScriptTable::LuaStation_GetClientSize(Lua_State* L)
	{
		float fW = 1280.0f;
		float fH = 960.0f;
		int nScale = true;

		if (Lua_GetTopIndex(L) > 0)
		{
			if (lua_isboolean(L, 1))
				nScale = Lua_ValueToBoolean(L, 1);
			else
				nScale = (int)Lua_ValueToNumber(L, 1);
		}

		fW = g_pUI->m_WndStation.GetClientWidth();
		fH = g_pUI->m_WndStation.GetClientHeight();

		if (nScale)
		{
			float fScale = g_pUI->m_WndStation.GetUIScale();
			fW /= fScale;
			fH /= fScale;
		}

		Lua_PushNumber(L, fW);
		Lua_PushNumber(L, fH);
		return 2;
	}

	int KWndClassScriptTable::LuaStation_GetMaxUIScale(Lua_State* L)
	{
		ASSERT(L);

		float fScale = g_pUI->m_WndStation.GetMaxUIScale();

		Lua_PushNumber(L, fScale);
		return 1;
	}

	int KWndClassScriptTable::LuaStation_OriginalToAdjustPos(Lua_State* L)
	{
		float fX = 640.0f;
		float fY = 480.0f;

		KGLOG_PROCESS_ERROR (Lua_GetTopIndex(L) == 2);

		float fScale = g_pUI->m_WndStation.GetUIScale();

		fX = (float)Lua_ValueToNumber(L, 1) * fScale;
		fY = (float)Lua_ValueToNumber(L, 2) * fScale;

Exit0:
		Lua_PushNumber(L, fX);
		Lua_PushNumber(L, fY);
		return 2;
	}
	int KWndClassScriptTable::LuaStation_AdjustToOriginalPos(Lua_State* L)
	{
		float fX = 640.0f;
		float fY = 480.0f;

		KGLOG_PROCESS_ERROR (Lua_GetTopIndex(L) == 2);

		float fScale = g_pUI->m_WndStation.GetUIScale();

		fX = (float)Lua_ValueToNumber(L, 1) / fScale;
		fY = (float)Lua_ValueToNumber(L, 2) / fScale;

Exit0:
		Lua_PushNumber(L, fX);
		Lua_PushNumber(L, fY);
		return 2;
	}


	int KWndClassScriptTable::LuaStation_GetWindowPosition(Lua_State* L)
	{
		int nX = 0;
		int nY = 0;

		KG_PROCESS_ERROR(lua_gettop(L) == 0);

		nX = g_pUI->m_WndStation.GetWindowX();
		nY = g_pUI->m_WndStation.GetWindowY();

Exit0:
		lua_pushnumber(L, nX);
		lua_pushnumber(L, nY);
		return 2;
	}

	int KWndClassScriptTable::LuaStation_GetClientPosition(Lua_State* L)
	{
		int nX = 0;
		int nY = 0;

		KG_PROCESS_ERROR(lua_gettop(L) == 0);

		nX = g_pUI->m_WndStation.GetClientX();
		nY = g_pUI->m_WndStation.GetClientY();

Exit0:
		lua_pushnumber(L, nX);
		lua_pushnumber(L, nY);
		return 2;
	}

	int KWndClassScriptTable::LuaStation_Paint(Lua_State* L)
	{
        HRESULT hr = E_FAIL;
        BOOL bBeginRenderToScreen = FALSE;
        BOOL bBeginScene = FALSE;

        hr = g_pUI->m_p3DUI->BeginRenderToScreen();
        KG_COM_PROCESS_ERROR(hr);

        bBeginRenderToScreen = TRUE;

        hr = g_pUI->m_p3DUI->BeginScene();
        KGLOG_COM_PROCESS_ERROR(hr);

        bBeginScene = TRUE;

        g_pUI->m_WndStation.Paint();

Exit0:
        if (hr == D3DERR_DRIVERINTERNALERROR || hr == D3DERR_INVALIDCALL)
        {
            KGLogPrintf(KGLOG_ERR, "KGUI LuaStation_Paint 0x%.8x.\n", hr);

            PostMessage(g_pUI->m_hWnd, WM_QUIT, 0, 0);
        }
        if (bBeginScene)
        {
            hr = g_pUI->m_p3DUI->EndScene();
            KGLOG_COM_CHECK_ERROR(hr);
        }
        if (bBeginRenderToScreen)
        {
            hr = g_pUI->m_p3DUI->EndRenderToScreen();
            KGLOG_COM_CHECK_ERROR(hr);

            hr = g_pUI->m_p3DUI->Present();
            KGLOG_COM_CHECK_ERROR(hr);
        }
		return 0;
	}

	int KWndClassScriptTable::LuaStation_GetScreenPos(Lua_State* L)
	{
		RECT rc;
		if (!(::GetWindowRect(g_pUI->m_hWnd, &rc)))
			return 0;

		Lua_PushNumber(L, rc.left);
		Lua_PushNumber(L, rc.top);
		return 2;
	}


	int KWndClassScriptTable::LuaStation_SetUIScale(Lua_State* L)
	{
		float fScale = 1.0f;
		BOOL bForce = FALSE;
		int nParam = Lua_GetTopIndex(L);
		KG_PROCESS_ERROR(nParam > 0);

		fScale = (float)Lua_ValueToNumber(L, 1);
		if (nParam > 1 )
		{
			if (lua_isboolean(L, 2))
				bForce = Lua_ValueToBoolean(L, 2);
			else
				bForce = (BOOL)(int)Lua_ValueToNumber(L, 2);
		}

		g_pUI->m_WndStation.SetUIScale(fScale, bForce);
Exit0:
		return 0;
	}

	int KWndClassScriptTable::LuaStation_GetUIScale(Lua_State* L)
	{
		ASSERT(L);

		float fScale = g_pUI->m_WndStation.GetUIScale();
		Lua_PushNumber(L, fScale);
		return 1;
	}

	int KWndClassScriptTable::LuaStation_IsFullScreen(Lua_State* L)
	{
		ASSERT(L);

		int nFull = g_pUI->m_WndStation.IsFullScreen();
		Lua_PushBoolean(L, nFull);
		return 1;
	}

	int KWndClassScriptTable::LuaStation_IsPanauision(Lua_State* L)
	{
		ASSERT(L);

		int nPanauision = g_pUI->m_WndStation.IsPanauision();
		Lua_PushBoolean(L, nPanauision);
		return 1;
	}

	int KWndClassScriptTable::LuaStation_IsExclusiveMode(Lua_State* L)
	{
		ASSERT(L);

		int nExclusiveMode = g_pUI->m_WndStation.IsExclusiveMode();
		Lua_PushBoolean(L, nExclusiveMode);
		return 1;
	}

	int KWndClassScriptTable::LuaStation_IsVisible(Lua_State* L)
	{
		ASSERT(L);
		int nVisible = g_pUI->m_WndStation.IsVisible();
		Lua_PushBoolean(L, nVisible);
		return 1;
	}

	int KWndClassScriptTable::LuaStation_Show(Lua_State* L)
	{
		ASSERT(L);
		g_pUI->m_WndStation.Show();
		return 0;
	}

	int KWndClassScriptTable::LuaStation_Hide(Lua_State* L)
	{
		ASSERT(L);
		g_pUI->m_WndStation.Hide();
		return 0;
	}

	int KWndClassScriptTable::LuaStation_ToggleWindow(Lua_State* L)
	{
		BOOL bRetCode = FALSE;
		LPCSTR szFileName = NULL;
		LPCSTR szFrameName = NULL;
        int nParamCount = 0;

		ASSERT(L);

		nParamCount = lua_gettop(L);
		KG_PROCESS_ERROR(nParamCount > 0);

		szFileName = lua_tostring(L, 1);

		if (nParamCount > 1)
			szFrameName = lua_tostring(L, 2);
		if (szFrameName == NULL)
			szFrameName = szFileName;

		bRetCode = g_pUI->m_WndStation.ToggleFrame(szFileName, szFrameName);
		KGLOG_PROCESS_ERROR(bRetCode);

Exit0:
		return 0;
	}

	int KWndClassScriptTable::LuaStation_OpenWindow(Lua_State* L)
	{
		LPCSTR szFileName = NULL;
		LPCSTR szFrameName = NULL;
		KWndFrame *pWnd = NULL; 
		int nRetCode = false;
		int nParamCount = 0;
        
		ASSERT(L);

		nParamCount = lua_gettop(L);
		KG_PROCESS_ERROR(nParamCount > 0);

		szFileName = lua_tostring(L, 1);
		KG_PROCESS_ERROR(szFileName);

		if (nParamCount > 1)
			szFrameName = lua_tostring(L, 2);

		if (szFrameName == NULL)
			szFrameName = szFileName;

		pWnd = g_pUI->m_WndStation.OpenFrame(szFileName, szFrameName);
		KGLOG_PROCESS_ERROR(pWnd);

		nRetCode = HostStation_Lookup(L, pWnd);
		KG_PROCESS_ERROR(nRetCode);

		return 1;
Exit0:
		return 0;
	}

	int KWndClassScriptTable::LuaStation_CloseWindow(Lua_State* L)
	{
		ASSERT(L);
		LPCSTR pcszName = NULL;
		int nParamCount = 0;
        
        nParamCount = Lua_GetTopIndex(L);
		KGLOG_PROCESS_ERROR(nParamCount > 0);

		if (Lua_IsTable(L, 1))
		{
			KWndFrame* pFrame = (KWndFrame*)ToWndUserData("WndFrame", L, 1);
			if (pFrame)
				pcszName = pFrame->GetName();
		}
		else
		{
			pcszName = Lua_ValueToString(L, 1);
		}
		KG_PROCESS_ERROR(pcszName);
		g_pUI->m_WndStation.CloseFrame(pcszName);
Exit0:
		return 0;
	}

	int KWndClassScriptTable::LuaStation_KeepWndInClient(Lua_State* L)
	{
		ASSERT(L);
		int nRetCode = false;
		KWndWindow *pWnd = NULL;

		KGLOG_PROCESS_ERROR(Lua_GetTopIndex(L) > 0);

		if (Lua_IsTable(L, 1))
		{
			pWnd = (KWndWindow*)ToWndUserData("WndWindow", L, 1);
			KG_PROCESS_ERROR(pWnd);
		}
		else
		{
			LPCSTR pcszTreePath = Lua_ValueToString(L, 1);
			KG_PROCESS_ERROR(pcszTreePath);

			pWnd = g_pUI->m_WndStation.SearchWindow(pcszTreePath);
			KG_PROCESS_ERROR(pWnd);
		}
		ALW_CorrectWndPosition(pWnd);
Exit0:
		return 0;
	}

	int KWndClassScriptTable::LuaStation_SetPopUpMenuPos(Lua_State* L)
	{
		ASSERT(L);
		int nRetCode = false;
		POINT ptPos;
		KWndWindow *pWnd = NULL;
		int nType = 0;

		KGLOG_PROCESS_ERROR(Lua_GetTopIndex(L) > 3);

		if (Lua_IsTable(L, 1))
		{
			pWnd = (KWndWindow*)ToWndUserData("WndWindow", L, 1);
			KG_PROCESS_ERROR(pWnd);
		}
		else
		{
			LPCSTR pcszTreePath = Lua_ValueToString(L, 1);
			KG_PROCESS_ERROR(pcszTreePath);

			pWnd = g_pUI->m_WndStation.SearchWindow(pcszTreePath);
			KG_PROCESS_ERROR(pWnd);
		}
		ptPos.x = (long)Lua_ValueToNumber(L, 2);
		ptPos.y = (long)Lua_ValueToNumber(L, 3);
		nType = (int)Lua_ValueToNumber(L, 4);

		float fScale = g_pUI->m_WndStation.GetUIScale();

		ptPos.x = (int)(ptPos.x * fScale);
		ptPos.y = (int)(ptPos.y * fScale);

		ALW_CorrectWndPosition(pWnd, ptPos, nType);
Exit0:
		return 0;
	}

	int KWndClassScriptTable::LuaStation_SetTipPosByRect(Lua_State* L)
	{
		ASSERT(L);
		int nRetCode = false;
		RECT rcSurround;
		KWndWindow *pWnd = NULL;
		int nType = 0;

		KGLOG_PROCESS_ERROR(Lua_GetTopIndex(L) > 5);

		if (Lua_IsTable(L, 1))
		{
			pWnd = (KWndWindow*)ToWndUserData("WndWindow", L, 1);
			KG_PROCESS_ERROR(pWnd);
		}
		else
		{
			LPCSTR pcszTreePath = Lua_ValueToString(L, 1);
			KG_PROCESS_ERROR(pcszTreePath);

			pWnd = g_pUI->m_WndStation.SearchWindow(pcszTreePath);
			KG_PROCESS_ERROR(pWnd);
		}
		rcSurround.left = (long)Lua_ValueToNumber(L, 2);
		rcSurround.top = (long)Lua_ValueToNumber(L, 3);
		rcSurround.right = (long)Lua_ValueToNumber(L, 4);
		rcSurround.bottom = (long)Lua_ValueToNumber(L, 5);
		nType = (int)Lua_ValueToNumber(L, 6);

		float fScale = g_pUI->m_WndStation.GetUIScale();
		rcSurround.left = (int)(rcSurround.left * fScale);
		rcSurround.right = (int)(rcSurround.right * fScale);

		rcSurround.top = (int)(rcSurround.top * fScale);
		rcSurround.bottom = (int)(rcSurround.bottom * fScale); 

		ALW_CorrectWndPosition(pWnd, rcSurround, nType);
Exit0:
		return 0;
	}

	int KWndClassScriptTable::LuaStation_SetTipPosByWnd(Lua_State* L)
	{
		ASSERT(L);
		int nRetCode = false;
		KWndWindow *pWnd = NULL;
		KWndWindow *pWnd2 = NULL;
		int nType = 0;
		KGLOG_PROCESS_ERROR(Lua_GetTopIndex(L) > 2);
		if (Lua_IsTable(L, 1))
		{
			pWnd = (KWndWindow*)ToWndUserData("WndWindow", L, 1);
			KG_PROCESS_ERROR(pWnd);
		}
		else
		{
			LPCSTR pcszTreePath = Lua_ValueToString(L, 1);
			KG_PROCESS_ERROR(pcszTreePath);

			pWnd = g_pUI->m_WndStation.SearchWindow(pcszTreePath);
			KG_PROCESS_ERROR(pWnd);
		}

		if (Lua_IsTable(L, 2))
		{
			pWnd2 = (KWndWindow*)ToWndUserData("WndWindow", L, 2);
			KGLOG_PROCESS_ERROR(pWnd);
		}
		else
		{
			LPCSTR pcszTreePath = Lua_ValueToString(L, 2);
			KG_PROCESS_ERROR(pcszTreePath);

			pWnd2 = g_pUI->m_WndStation.SearchWindow(pcszTreePath);
			KG_PROCESS_ERROR(pWnd2);
		}

		nType = (int)Lua_ValueToNumber(L, 3);

		ALW_CorrectWndPosition(pWnd, pWnd2, nType);
Exit0:
		return 0;
	}

	int KWndClassScriptTable::LuaStation_LoadFrameList(Lua_State* L)
	{
		ASSERT(L);
		LPCSTR szFilePath = NULL;

		KGLOG_PROCESS_ERROR(Lua_GetTopIndex(L) > 0);

		szFilePath = Lua_ValueToString(L, 1);
		szFilePath = FormatFilePath(szFilePath);
		KGLOG_PROCESS_ERROR(szFilePath);
		
		g_pUI->m_WndStation.LoadFrameList(szFilePath);
Exit0:
		return 0;
	}

	int KWndClassScriptTable::LuaStation_UnloadFrameList(Lua_State* L)
	{
		ASSERT(L);
		LPCSTR szFilePath = NULL;

		KGLOG_PROCESS_ERROR(Lua_GetTopIndex(L) > 0);

		szFilePath = Lua_ValueToString(L, 1);
		szFilePath = FormatFilePath(szFilePath);
		KGLOG_PROCESS_ERROR(szFilePath);

		g_pUI->m_WndStation.UnloadFrameList(szFilePath);
Exit0:
		return 0;

	}

	int KWndClassScriptTable::LuaStation_AdjustFrameListPosition(Lua_State* L)
	{
		ASSERT(L);
		LPCSTR szFilePath = NULL;
		KGLOG_PROCESS_ERROR(Lua_GetTopIndex(L) > 0);

		szFilePath = Lua_ValueToString(L, 1);
		szFilePath = FormatFilePath(szFilePath);
		KGLOG_PROCESS_ERROR(szFilePath);

		g_pUI->m_WndStation.AdjustFrameListPosition(szFilePath);
Exit0:
		return 0;
	}

	int KWndClassScriptTable::LuaStation_GetMessagePos(Lua_State* L)
	{
		ASSERT(L);

		int nX = 0;
		int nY = 0;
		int nScale = true;

		if (Lua_GetTopIndex(L) > 0)
		{
			if (Lua_IsBoolean(L, 1))
				nScale = Lua_ValueToBoolean(L, 1);
			else
				nScale = (int)Lua_ValueToNumber(L, 1);
		}

		g_pUI->m_WndStation.GetMessagePos(nX, nY);

		if (nScale)
		{
			float fScale = g_pUI->m_WndStation.GetUIScale();
			Lua_PushNumber(L, (float)nX / fScale);
			Lua_PushNumber(L, (float)nY / fScale);
			return 2;
		}

		Lua_PushNumber(L, nX);
		Lua_PushNumber(L, nY);
		return 2;
	}

	int KWndClassScriptTable::LuaStation_GetMessageWheelDelta(Lua_State* L)
	{
		ASSERT(L);
		float fDelta = g_pUI->m_WndStation.GetMessageWheelDelta();
		Lua_PushNumber(L, fDelta);
		return 1;
	}

	int KWndClassScriptTable::LuaStation_GetMessageKey(Lua_State* L)
	{
		ASSERT(L);
		int nKey = g_pUI->m_WndStation.GetMessageKey();
		Lua_PushNumber(L, nKey);
		return 1;
	}

	int KWndClassScriptTable::LuaStation_GetIdleTime(Lua_State* L)
	{
		ASSERT(L);
		DWORD dwIdleTime = g_pUI->m_WndStation.GetIdleTime();
		Lua_PushNumber(L, dwIdleTime);
		return 1;
	}

	int KWndClassScriptTable::LuaStation_ClearIdleTime(Lua_State* L)
	{
		g_pUI->m_WndStation.ClearIdleTime();
		return 0;
	}

	int KWndClassScriptTable::LuaStation_IsInUserAction(Lua_State* L)
	{
		Lua_PushBoolean(L, g_pUI->m_WndStation.IsInUserAction());
		return 1;
	}

	int KWndClassScriptTable::LuaStation_GetMouseOverWindow(Lua_State* L)
	{
		ASSERT(L);
		KWndWindow *pWnd = g_pUI->m_WndStation.GetMouseOverWindow();
		if (pWnd && HostStation_Lookup(L, pWnd))
			return 1;
		return 0;
	}

	int KWndClassScriptTable::LuaStation_SetFocusWindow(Lua_State* L)
	{
		int nRetCode = false;

		ASSERT(L);
		KGLOG_PROCESS_ERROR(Lua_GetTopIndex(L) == 1);

		if (Lua_IsString(L, 1))
		{
			LPCSTR pcszTreePath = Lua_ValueToString(L, 1);
            KGLOG_PROCESS_ERROR(pcszTreePath);

			KWndWindow *pWnd = g_pUI->m_WndStation.SearchWindow(pcszTreePath);
			g_pUI->m_WndStation.SetFocusWindow(pWnd);
		}
		else if(Lua_IsTable(L, 1))
		{
			KWndWindow *pWnd = NULL;

			pWnd = (KWndWindow*)ToWndUserData("WndWindow", L, 1);
			KG_PROCESS_ERROR(pWnd);

			g_pUI->m_WndStation.SetFocusWindow(pWnd);
		}
		else if (lua_isnil(L, 1))
		{
			g_pUI->m_WndStation.SetFocusWindow(NULL);
		}

Exit0:
		return 0;
	}

	int KWndClassScriptTable::LuaStation_GetFocusWindow(Lua_State* L)
	{
		ASSERT(L);
		KWndWindow *pWnd = g_pUI->m_WndStation.GetFocusWindow();
        if (pWnd && HostStation_Lookup(L, pWnd))
            return 1;
		return 0;
	}

	int KWndClassScriptTable::LuaStation_SetActiveFrame(Lua_State* L)
	{
		int nRetCode = false;
		int nBring = false;
		ASSERT(L);
		KGLOG_PROCESS_ERROR(Lua_GetTopIndex(L) > 0);

		if (Lua_GetTopIndex(L) > 1)
		{
			if (Lua_IsBoolean(L, 2))
				nBring = Lua_ValueToBoolean(L, 2);
			else
				nBring = (int)Lua_ValueToNumber(L, 2);
		}

		if (Lua_IsString(L, 1))
		{
			LPCSTR pcszTreePath = Lua_ValueToString(L, 1);
			KWndWindow *pWnd = g_pUI->m_WndStation.SearchWindow(pcszTreePath);

			KWndFrame *pFrame = (KWndFrame*)CastWndWindow(pWnd, "WndFrame");
			KGLOG_PROCESS_ERROR(pFrame);

			g_pUI->m_WndStation.SetActiveFrame(pFrame, nBring ? true: false);
		}
		else if(Lua_IsTable(L, 1))
		{
			KWndFrame *pWnd = NULL;

			pWnd = (KWndFrame*)ToWndUserData("WndFrame", L, 1);
			KG_PROCESS_ERROR(pWnd);

			g_pUI->m_WndStation.SetActiveFrame(pWnd, nBring ? true: false);
		}
		else if (lua_isnil(L, 1))
		{
			g_pUI->m_WndStation.SetActiveFrame(NULL, nBring ? true: false);
		}

Exit0:
		return 0;
	}

	int KWndClassScriptTable::LuaStation_GetActiveFrame(Lua_State* L)
	{
		ASSERT(L);
		KWndFrame *pWnd = g_pUI->m_WndStation.GetActiveFrame();
		if (pWnd && HostStation_Lookup(L, pWnd))
			return 1;
		return 0;
	}

	int KWndClassScriptTable::LuaStation_SetCapture(Lua_State* L)
	{
		int nRetCode = false;
		ASSERT(L);
		KGLOG_PROCESS_ERROR(Lua_GetTopIndex(L) > 0);

		if (Lua_IsString(L, 1))
		{
			LPCSTR pcszTreePath = Lua_ValueToString(L, 1);

			KWndWindow *pWnd = g_pUI->m_WndStation.SearchWindow(pcszTreePath);
			KGLOG_PROCESS_ERROR(pWnd);

			g_pUI->m_WndStation.SetCapture(pWnd);
		}
		else if(Lua_IsTable(L, 1))
		{
			KWndWindow *pWnd = NULL;

			pWnd = (KWndWindow*)ToWndUserData("WndWindow", L, 1);
			KG_PROCESS_ERROR(pWnd);

			g_pUI->m_WndStation.SetCapture(pWnd);
		}
		else if (lua_isnil(L, 1))
		{
			g_pUI->m_WndStation.ReleaseCapture();
		}

Exit0:
		return 0;
	}

	int KWndClassScriptTable::LuaStation_GetCapture(Lua_State* L)
	{
		ASSERT(L);
		KWndWindow *pWnd = g_pUI->m_WndStation.GetCapture();
		if (pWnd && HostStation_Lookup(L, pWnd))
			return 1;
		return 0;
	}

	int KWndClassScriptTable::LuaStation_ReleaseCapture(Lua_State* L)
	{
		ASSERT(L);
		g_pUI->m_WndStation.ReleaseCapture();
		return 0;
	}

	int KWndClassScriptTable::LuaCursor_GetCursorPos(Lua_State* L)
	{
		int nX = 640;
		int nY = 480;
		int nScale = true;

		if (Lua_GetTopIndex(L) > 0)
		{
			if (Lua_IsBoolean(L, 1))
				nScale = Lua_ValueToBoolean(L, 1);
			else
				nScale = (int)Lua_ValueToNumber(L, 1);
		}

		g_pUI->m_Pointer.GetCursorPos(nX, nY);

		if (nScale)
		{
			float fScale = g_pUI->m_WndStation.GetUIScale();
			Lua_PushNumber(L, (float)nX / fScale);
			Lua_PushNumber(L, (float)nY / fScale);
			return 2;
		}

		Lua_PushNumber(L, nX);
		Lua_PushNumber(L, nY);
		return 2;
	}

	int KWndClassScriptTable::LuaCursor_ShowCursor(Lua_State* L)
	{
		ASSERT(L);
		int nShow = true;

		if (Lua_GetTopIndex(L) > 0)
		{
			if (Lua_IsBoolean(L, 1))
				nShow = Lua_ValueToBoolean(L, 1);
			else
				nShow = (int)(Lua_ValueToNumber(L, 1));
		}

		g_pUI->m_Pointer.ShowCursor(nShow != 0);
		return 0;
	}

	int KWndClassScriptTable::LuaCursor_IsVisible(Lua_State* L)
	{
		ASSERT(L);
		int nShow = g_pUI->m_Pointer.IsCursorVisible();
		Lua_PushBoolean(L, nShow);
		return 1;
	}

	int KWndClassScriptTable::LuaCursor_SwitchCursor(Lua_State* L)
	{
		ASSERT(L);
		int nIndex = -1;
		int nIcon = -1;
		KGLOG_PROCESS_ERROR(Lua_GetTopIndex(L) > 0);
		nIndex = (int)Lua_ValueToNumber(L, 1);
		if (Lua_GetTopIndex(L) > 1)
			nIcon = (int)Lua_ValueToNumber(L, 2);
		g_pUI->m_Pointer.SwitchCursor(nIndex, nIcon);
Exit0:
		return 0;
	}

	int KWndClassScriptTable::LuaCursor_GetCurrentCursorIndex(Lua_State* L)
	{
		ASSERT(L);
		int nIndex = g_pUI->m_Pointer.GetCurrentCursorIndex();
		Lua_PushNumber(L, nIndex);
		return 1;
	}

	int KWndClassScriptTable::LuaCursor_RestoreCursor(Lua_State* L)
	{
		ASSERT(L);
		g_pUI->m_Pointer.RestoreCursor();
		return 0;
	}

	int KWndClassScriptTable::LuaCursor_IsExistCursor(Lua_State* L)
	{
		ASSERT(L);
		int nIndex = 0;
		int nExist = false;
		KGLOG_PROCESS_ERROR(Lua_GetTopIndex(L) > 0);
		nIndex = (int)Lua_ValueToNumber(L, 1);
		nExist = g_pUI->m_Pointer.IsExistCursor(nIndex);
Exit0:
		Lua_PushBoolean(L, nExist);
		return 1;
	}

	int KWndClassScriptTable::LuaCursor_LoadCursor(Lua_State* L)
	{
		ASSERT(L);
		int nIndex = 0;
		LPCSTR szCursorFile = NULL;

		KGLOG_PROCESS_ERROR(lua_gettop(L) > 1);

		nIndex = (int)lua_tonumber(L, 1);

		szCursorFile = lua_tostring(L, 2);
		KGLOG_PROCESS_ERROR(szCursorFile);

		szCursorFile = FormatFilePath(szCursorFile);

		g_pUI->m_Pointer.LoadCursor(nIndex, szCursorFile);
Exit0:
		return 0;
	}

	int KWndClassScriptTable::LuaCursor_UnloadCursor(Lua_State* L)
	{
		ASSERT(L);
		int nIndex = 0;

		KGLOG_PROCESS_ERROR(lua_gettop(L) > 0);

		nIndex = (int)lua_tonumber(L, 1);
		g_pUI->m_Pointer.UnloadCursor(nIndex);
Exit0:
		return 0;
	}

	int KWndClassScriptTable::LuaCursor_LoadAllCursor(Lua_State* L)
	{
		ASSERT(L);
		g_pUI->m_Pointer.LoadAllCursor();
		return 0;
	}

	int KWndClassScriptTable::LuaCursor_UnloadAllCursor(Lua_State* L)
	{
		ASSERT(L);
		g_pUI->m_Pointer.UnloadAllCursor();
		return 0;
	}

	int KWndClassScriptTable::LuaCursor_SetCursorPos(Lua_State* L)
	{
		float fX = 640;
		float fY = 480;
		int nScale = true;
		int nParamCount = 0;

		nParamCount = Lua_GetTopIndex(L);
		KG_PROCESS_ERROR(nParamCount > 1);

		fX = (float)Lua_ValueToNumber(L, 1);
		fY = (float)Lua_ValueToNumber(L, 2);

		if (nParamCount > 2)
		{
			if(Lua_IsBoolean(L, 3))
				nScale = Lua_ValueToBoolean(L, 3);
			else
				nScale = (int)Lua_ValueToNumber(L, 3);
		}


		if (nScale)
		{
			float fScale = g_pUI->m_WndStation.GetUIScale();
			fX *= fScale;
			fY *= fScale;
		}

		g_pUI->m_Pointer.SetCursorPos((int)fX, (int)fY);

Exit0:
		return 0;
	}


	int KWndClassScriptTable::LuaWindow_GetRelPos(Lua_State* L)
	{
		int nRetCode = false;
		KWndWindow* pWnd = NULL;
		float fX = 0.0f;
		float fY = 0.0f;
		float fScale = 1.0f;

		ASSERT(L);

		KGLOG_PROCESS_ERROR(Lua_GetTopIndex(L) == 1);

		pWnd = (KWndWindow*)ToWndUserData("WndWindow", L, 1);
		KGLOG_PROCESS_ERROR(pWnd);

		fScale = g_pUI->m_WndStation.GetUIScale();

		fX = pWnd->GetRelX() / fScale;
		fY = pWnd->GetRelY() / fScale;

Exit0:
		Lua_PushNumber(L, fX);
		Lua_PushNumber(L, fY);
		return 2;
	}

	int KWndClassScriptTable::LuaWindow_GetAbsPos(Lua_State* L)
	{
		int nRetCode = false;
		KWndWindow* pWnd = NULL;
		float fX = 0.0f;
		float fY = 0.0f;
		float fScale = g_pUI->m_WndStation.GetUIScale();

		ASSERT(L);

		KGLOG_PROCESS_ERROR(Lua_GetTopIndex(L) == 1);

		pWnd = (KWndWindow*)ToWndUserData("WndWindow", L, 1);
		KGLOG_PROCESS_ERROR(pWnd);

		fX = pWnd->GetAbsX() / fScale;
		fY = pWnd->GetAbsY() / fScale;

Exit0:
		Lua_PushNumber(L, fX);
		Lua_PushNumber(L, fY);
		return 2;
	}

	int KWndClassScriptTable::LuaWindow_GetSize(Lua_State* L)
	{
		int nRetCode = false;
		KWndWindow* pWnd = NULL;
		float fWidth = 0.0f;
		float fHeight = 0.0f;
		float fScale = 1.0f;

		ASSERT(L);
		KGLOG_PROCESS_ERROR(Lua_GetTopIndex(L) == 1);

		pWnd = (KWndWindow*)ToWndUserData("WndWindow", L, 1);
		KGLOG_PROCESS_ERROR(pWnd);

		pWnd->GetSize(&fWidth, &fHeight);

		fScale = g_pUI->m_WndStation.GetUIScale();

		fWidth /= fScale;
		fHeight /= fScale;
Exit0:
		Lua_PushNumber(L, fWidth);
		Lua_PushNumber(L, fHeight);
		return 2;
	}

    int KWndClassScriptTable::LuaWindow_SetSize(Lua_State* L)
    {
		int nRetCode = false;
		KWndWindow* pWnd = NULL;
		float fWidth = 0.0f;
        float fHeight = 0.0f;
		float fScale = 1.0f;

		ASSERT(L);
		KGLOG_PROCESS_ERROR(Lua_GetTopIndex(L) == 3);

		pWnd = (KWndWindow*)ToWndUserData("WndWindow", L, 1);
		KGLOG_PROCESS_ERROR(pWnd);

		fScale = g_pUI->m_WndStation.GetUIScale();

		fWidth = (float)(Lua_ValueToNumber(L, 2)) * fScale;
        fHeight = (float)(Lua_ValueToNumber(L, 3)) * fScale;

		pWnd->SetSize(fWidth, fHeight);
Exit0:
		return 0;
    }

	int KWndClassScriptTable::LuaWindow_IsVisible(Lua_State* L)
	{
		int nRetCode = false;
		KWndWindow* pWnd = NULL;
		int nIsVisible = false;

		ASSERT(L);
		KGLOG_PROCESS_ERROR(Lua_GetTopIndex(L) == 1);

		pWnd = (KWndWindow*)ToWndUserData("WndWindow", L, 1);
		KGLOG_PROCESS_ERROR(pWnd);

		nIsVisible = pWnd->IsVisible();
Exit0:
		Lua_PushBoolean(L, nIsVisible);
		return 1;
	}

	int KWndClassScriptTable::LuaWindow_IsDisable(Lua_State* L)
	{
		int nRetCode = false;
		KWndWindow* pWnd = NULL;
		int nIsDisable = false;

		ASSERT(L);
		KGLOG_PROCESS_ERROR(Lua_GetTopIndex(L) == 1);

		pWnd = (KWndWindow*)ToWndUserData("WndWindow", L, 1);
		KGLOG_PROCESS_ERROR(pWnd);

		nIsDisable = pWnd->IsDisable();
Exit0:
		Lua_PushBoolean(L, nIsDisable);
		return 1;
	}

	int KWndClassScriptTable::LuaWindow_SetRelPos(Lua_State* L)
	{
		int nRetCode = false;
		KWndWindow* pWnd = NULL;
		float fX = 0.0f;
		float fY = 0.0f;
		float fScale = g_pUI->m_WndStation.GetUIScale();

		ASSERT(L);

		KGLOG_PROCESS_ERROR(Lua_GetTopIndex(L) == 3);

		pWnd = (KWndWindow*)ToWndUserData("WndWindow", L, 1);
		KGLOG_PROCESS_ERROR(pWnd);

		fX = (float)lua_tonumber(L, 2) * fScale;
		fY = (float)lua_tonumber(L, 3) * fScale;

		pWnd->SetRelPos(fX, fY);
Exit0:
		return 0;
	}

	int KWndClassScriptTable::LuaWindow_SetAbsPos(Lua_State* L)
	{
		int nRetCode = false;
		KWndWindow* pWnd = NULL;
		float fX = 0.0f;
		float fY = 0.0f;
		float fScale = g_pUI->m_WndStation.GetUIScale();

		ASSERT(L);

		KGLOG_PROCESS_ERROR(Lua_GetTopIndex(L) == 3);

		pWnd = (KWndWindow*)ToWndUserData("WndWindow", L, 1);
		KGLOG_PROCESS_ERROR(pWnd);

		fX = (float)lua_tonumber(L, 2) * fScale;
		fY = (float)lua_tonumber(L, 3) * fScale;

		pWnd->SetAbsPos(fX, fY);
Exit0:
		return 0;
	}


	int KWndClassScriptTable::LuaWindow_SetCursorAbove(Lua_State* L)
	{
		int nRetCode = false;
		KWndWindow* pWnd = NULL;

		ASSERT(L);
		KGLOG_PROCESS_ERROR(Lua_GetTopIndex(L) == 1);

		pWnd = (KWndWindow*)ToWndUserData("WndWindow", L, 1);
		KGLOG_PROCESS_ERROR(pWnd);

		pWnd->SetCursorAbove();
Exit0:
		return 0;
	}

	int KWndClassScriptTable::LuaWindow_Enable(Lua_State* L)
	{
		int nRetCode = false;
		KWndWindow* pWnd = NULL;
		int nEnable = false;

		ASSERT(L);

		KGLOG_PROCESS_ERROR(Lua_GetTopIndex(L) == 2);

		pWnd = (KWndWindow*)ToWndUserData("WndWindow", L, 1);
		KGLOG_PROCESS_ERROR(pWnd);

		if (lua_isboolean(L, 2))
			nEnable = Lua_ValueToBoolean(L, 2);
		else
			nEnable = (int)Lua_ValueToNumber(L, 2);

		pWnd->Enable(nEnable);
Exit0:
		return 0;
	}

	int KWndClassScriptTable::LuaWindow_Show(Lua_State* L)
	{
		int nRetCode = false;
		KWndWindow* pWnd = NULL;
		int nParamCount = Lua_GetTopIndex(L);

		KGLOG_PROCESS_ERROR(nParamCount > 0);

		pWnd = (KWndWindow*)ToWndUserData("WndWindow", L, 1);
		KGLOG_PROCESS_ERROR(pWnd);

		if (nParamCount > 1)
		{
			int nShow = true;
			if (lua_isboolean(L, 2))
				nShow = Lua_ValueToBoolean(L, 2);
			else
				nShow = (int)Lua_ValueToNumber(L, 2);
			if (nShow)
				pWnd->Show();
			else
				pWnd->Hide();
		}
		else
			pWnd->Show();
		
Exit0:
		return 0;
	}

	int KWndClassScriptTable::LuaWindow_Hide(Lua_State* L)
	{
		int nRetCode = false;
		KWndWindow* pWnd = NULL;
		int nParamCount = Lua_GetTopIndex(L);

		KGLOG_PROCESS_ERROR(nParamCount > 0);

		pWnd = (KWndWindow*)ToWndUserData("WndWindow", L, 1);
		KGLOG_PROCESS_ERROR(pWnd);

		if (nParamCount > 1)
		{
			int nHide = true;
			if (lua_isboolean(L, 2))
				nHide = Lua_ValueToBoolean(L, 2);
			else
				nHide = (int)Lua_ValueToNumber(L, 2);
			if (nHide)
				pWnd->Hide();
			else
				pWnd->Show();
		}
		else
			pWnd->Hide();
Exit0:
		return 0;
	}

	int KWndClassScriptTable::LuaWindow_ToggleVisible(Lua_State* L)
	{
		int nRetCode = false;
		KWndWindow* pWnd = NULL;

		ASSERT(L);

		KGLOG_PROCESS_ERROR(Lua_GetTopIndex(L) == 1);

		pWnd = (KWndWindow*)ToWndUserData("WndWindow", L, 1);
		KGLOG_PROCESS_ERROR(pWnd);

		if (pWnd->IsVisible())
			pWnd->Hide();
		else
			pWnd->Show();
Exit0:
		return 0;
	}

	int KWndClassScriptTable::LuaWindow_BringToTop(Lua_State* L)
	{
		int nRetCode = false;
		KWndWindow* pWnd = NULL;

		ASSERT(L);

		KGLOG_PROCESS_ERROR(Lua_GetTopIndex(L) == 1);

		pWnd = (KWndWindow*)ToWndUserData("WndWindow", L, 1);
		KGLOG_PROCESS_ERROR(pWnd);

		pWnd->BringToTop();
Exit0:
		return 0;
	}

    int KWndClassScriptTable::LuaWindow_Scale(Lua_State* L)
    {
		int nRetCode = false;
		KWndWindow* pWnd = NULL;
        float fXScale = 1.0f;
        float fYScale = 1.0f;

		ASSERT(L);

		KGLOG_PROCESS_ERROR(Lua_GetTopIndex(L) == 3);

		pWnd = (KWndWindow*)ToWndUserData("WndWindow", L, 1);
		KGLOG_PROCESS_ERROR(pWnd);

        fXScale = (float)Lua_ValueToNumber(L, 2);
        fYScale = (float)Lua_ValueToNumber(L, 3);
		KGLOG_PROCESS_ERROR(fXScale > 0.0f);
		KGLOG_PROCESS_ERROR(fYScale > 0.0f);

		pWnd->Scale(fXScale, fYScale);
Exit0:
		return 0;
    }

	int KWndClassScriptTable::LuaWindow_CreateItemHandle(Lua_State* L)
	{
		int nRetCode = false;
		int nParamCount = 0;
		KWndWindow* pWnd = NULL;
		KItemHandle *pHandle = NULL;
		KUiComponentsDecoder& decoder = KUiComponentsDecoder::GetSelf();
		float fRelX = 0.0f;
		float fRelY = 0.0f;
		
		ASSERT(L);
		nParamCount = Lua_GetTopIndex(L);
		KGLOG_PROCESS_ERROR(nParamCount > 1);

		pWnd = (KWndWindow*)ToWndUserData("WndWindow", L, 1);
		KGLOG_PROCESS_ERROR(pWnd);

		pHandle = pWnd->GetItemHandle();
		KGLOG_PROCESS_ERROR(!pHandle);

		if (nParamCount == 2)
		{
			LPCSTR pcszString = NULL;
			int nDecodedLen = 0;

			pcszString = Lua_ValueToString(L, 2);
			KGLOG_PROCESS_ERROR(pcszString);

			nRetCode = decoder.CreateItemHandle(
                &pHandle, ATL::CA2W(pcszString, GetCodePage()), nDecodedLen, pWnd, g_pUI->m_WndStation.GetUIScale());
			KGLOG_PROCESS_ERROR(nRetCode);
		}
		else
		{
			LPCSTR pcszIniFile = NULL;
			LPCSTR pcszSectionName = NULL;
			LPCSTR pcszNewName = NULL;
			KSmartIniFile Ini;

			pcszIniFile = Lua_ValueToString(L, 2);
			KGLOG_PROCESS_ERROR(pcszIniFile);

			pcszSectionName = Lua_ValueToString(L, 3);
			KGLOG_PROCESS_ERROR(pcszSectionName);

			if (nParamCount == 4)
				pcszNewName = Lua_ValueToString(L, 4);

			Ini = g_OpenIniFile(pcszIniFile);
			KGLOG_PROCESS_ERROR(Ini);

			nRetCode = decoder.CreateItemHandle(
				&pHandle, &Ini, pcszSectionName, pWnd, g_pUI->m_WndStation.GetUIScale(), pcszNewName);
			KGLOG_PROCESS_ERROR(nRetCode);
		}

		KGLOG_PROCESS_ERROR(pHandle);

		nRetCode = pWnd->SetItemHandle(pHandle);
		if (!nRetCode)
		{
			SAFE_RELEASE(pHandle);
			KGLOG_PROCESS_ERROR(nRetCode);
		}

		pHandle->GetRelPos(fRelX, fRelY);
		pHandle->SetAbsPos(pWnd->GetAbsX() + fRelX, pWnd->GetAbsY() + fRelY);

Exit0:
		return 0;
	}

	int KWndClassScriptTable::LuaWindow_ReleaseItemHandle(Lua_State* L)
	{
		int nRetCode = false;
		KWndWindow* pWnd = NULL;

		ASSERT(L);
		KGLOG_PROCESS_ERROR(Lua_GetTopIndex(L) == 1);

		pWnd = (KWndWindow*)ToWndUserData("WndWindow", L, 1);
		KGLOG_PROCESS_ERROR(pWnd);

		pWnd->ReleaseItemHandle();
Exit0:
		return 0;
	}

	int KWndClassScriptTable::LuaWindow_Lookup(Lua_State *L)
	{
		int nResult = 0;
		int nRetCode = false;
		int nParamCount = 0;
		KWndWindow* pWnd = NULL;
		KItemNull *pItemNull = NULL;
		KItemHandle *pHandle = NULL;
		LPCSTR szTreePath = NULL;
		LPCSTR szItemTreePath = NULL;

		ASSERT(L);

		nParamCount = Lua_GetTopIndex(L);
		KGLOG_PROCESS_ERROR(nParamCount > 1);

		pWnd = (KWndWindow*)ToWndUserData("WndWindow", L, 1);
		KGLOG_PROCESS_ERROR(pWnd);

		szTreePath = lua_tostring(L, 2);
		KG_PROCESS_ERROR(szTreePath);

		if (szTreePath[0] != '\0')
		{
			pWnd = g_pUI->m_WndStation.SearchChildWindowByPath(pWnd, szTreePath);
			KG_PROCESS_ERROR(pWnd);
		}
		
		if (nParamCount == 2)
		{
			nRetCode = HostStation_Lookup(L, pWnd);
			KG_PROCESS_ERROR(nRetCode);
		}
        else
        {
            szItemTreePath = lua_tostring(L, 3);
            KGLOG_PROCESS_ERROR(szItemTreePath);

            pHandle = pWnd->GetItemHandle();
            KG_PROCESS_ERROR(pHandle);

            if (szItemTreePath[0] == '\0')
            {
                pItemNull = g_ItemCast<KItemNull>(pHandle);
                KG_PROCESS_ERROR(pItemNull);
            }
            else
            {
                pItemNull = pHandle->GetItemByTreePath(szItemTreePath);
                KG_PROCESS_ERROR(pItemNull);
            }

            nRetCode = KItemHandleClassScriptTable::HostWndItem_Lookup(L, pItemNull);
            KG_PROCESS_ERROR(nRetCode);
        }

		return 1;
Exit0:
		return nResult;
	}

	int KWndClassScriptTable::LuaWindow_GetName(Lua_State *L)
	{
		int nRetCode = false;
		KWndWindow* pWnd = NULL;
		LPCSTR pcszName = NULL;

		ASSERT(L);

		KGLOG_PROCESS_ERROR(Lua_GetTopIndex(L) == 1);

		pWnd = (KWndWindow*)ToWndUserData("WndWindow", L, 1);
		KGLOG_PROCESS_ERROR(pWnd);

		pcszName = pWnd->GetName();
		ASSERT(pcszName);
		
Exit0:
		if (pcszName)
			Lua_PushString(L, pcszName);
		else
			Lua_PushString(L, "");
		return 1;
	}

	int KWndClassScriptTable::LuaWindow_SetName(Lua_State *L)
	{
		int nRetCode = false;
		KWndWindow* pWnd = NULL;
		LPCSTR pcszName = NULL;

		ASSERT(L);

		KGLOG_PROCESS_ERROR(Lua_GetTopIndex(L) == 2);

		pWnd = (KWndWindow*)ToWndUserData("WndWindow", L, 1);
		KGLOG_PROCESS_ERROR(pWnd);

		pcszName = Lua_ValueToString(L, 2);
		KGLOG_PROCESS_ERROR(pcszName);

		pWnd->SetName(pcszName);
Exit0:
		return 0;
	}

	int KWndClassScriptTable::LuaWindow_GetPrev(Lua_State *L)
	{
		int nResult = 0;
		int nRetCode = false;
		KWndWindow* pWnd = NULL;
		LPCSTR pcszWndType = NULL;

		ASSERT(L);
		KGLOG_PROCESS_ERROR(Lua_GetTopIndex(L) == 1);

		pWnd = (KWndWindow*)ToWndUserData("WndWindow", L, 1);
		KGLOG_PROCESS_ERROR(pWnd);

		pWnd = pWnd->GetPrev();
		KG_PROCESS_ERROR(pWnd);

		nRetCode = HostStation_Lookup(L, pWnd);
		KG_PROCESS_ERROR(nRetCode);

		nResult = 1;
Exit0:
		return nResult;
	}


	int KWndClassScriptTable::LuaWindow_GetNext(Lua_State *L)
	{
		int nResult = 0;
		int nRetCode = false;
		KWndWindow* pWnd = NULL;

		ASSERT(L);
		KGLOG_PROCESS_ERROR(Lua_GetTopIndex(L) == 1);

		pWnd = (KWndWindow*)ToWndUserData("WndWindow", L, 1);
		KGLOG_PROCESS_ERROR(pWnd);

		pWnd = pWnd->GetNext();
		KG_PROCESS_ERROR(pWnd);

		nRetCode = HostStation_Lookup(L, pWnd);
		KG_PROCESS_ERROR(nRetCode);

		nResult = 1;
Exit0:
		return nResult;
	}
	int KWndClassScriptTable::LuaWindow_GetParent(Lua_State *L)
	{
		int nResult = 0;
		int nRetCode = false;
		KWndWindow* pWnd = NULL;

		ASSERT(L);
		KGLOG_PROCESS_ERROR(Lua_GetTopIndex(L) == 1);

		pWnd = (KWndWindow*)ToWndUserData("WndWindow", L, 1);
		KGLOG_PROCESS_ERROR(pWnd);

		pWnd = pWnd->GetParent();
		KG_PROCESS_ERROR(pWnd);

		nRetCode = HostStation_Lookup(L, pWnd);
		KG_PROCESS_ERROR(nRetCode);

		nResult = 1;
Exit0:
		return nResult;
	}

	int KWndClassScriptTable::LuaWindow_GetRoot(Lua_State *L)
	{
		int nResult = 0;
		int nRetCode = false;
		KWndWindow* pWnd = NULL;

		ASSERT(L);
		KGLOG_PROCESS_ERROR(Lua_GetTopIndex(L) == 1);

		pWnd = (KWndWindow*)ToWndUserData("WndWindow", L, 1);
		KGLOG_PROCESS_ERROR(pWnd);

		while (pWnd->GetWndType() != "WndFrame")
		{
			pWnd = pWnd->GetParent();
			ASSERT(pWnd);
		}

		nRetCode = HostStation_Lookup(L, pWnd);
		KG_PROCESS_ERROR(nRetCode);

		nResult = 1;
Exit0:
		return nResult;
	}

	int KWndClassScriptTable::LuaWindow_GetFirstChild(Lua_State *L)
	{
		int nResult = 0;
		int nRetCode = false;
		KWndWindow* pWnd = NULL;
		LPCSTR pcszWndType = NULL;

		ASSERT(L);

		KGLOG_PROCESS_ERROR(Lua_GetTopIndex(L) == 1);

		pWnd = (KWndWindow*)ToWndUserData("WndWindow", L, 1);
		KGLOG_PROCESS_ERROR(pWnd);

		pWnd = pWnd->GetFirstChild();
		KG_PROCESS_ERROR(pWnd);

		nRetCode = HostStation_Lookup(L, pWnd);
		KG_PROCESS_ERROR(nRetCode);

		nResult = 1;
Exit0:
		return nResult;
	}

	int KWndClassScriptTable::LuaWindow_CorrectPos(Lua_State *L)
	{
		int nRetCode = false;
		int nParamCount = 0;
		KWndWindow* pWnd = NULL;
		int nType = ALW_CENTER;
		float fXScale = g_pUI->m_WndStation.GetUIScale();
		float fYScale = g_pUI->m_WndStation.GetUIScale();

		ASSERT(L);
		nParamCount = Lua_GetTopIndex(L);
		KGLOG_PROCESS_ERROR(nParamCount >= 1);

		pWnd = (KWndWindow*)ToWndUserData("WndWindow", L, 1);
		KGLOG_PROCESS_ERROR(pWnd);

		switch (nParamCount)
		{
		case 1:
			ALW_CorrectWndPosition(pWnd);
			break;
		case 2:
			{
				LPCSTR pcszAnotherWnd = Lua_ValueToString(L, 2);

				KWndWindow* pAnotherWnd = g_pUI->m_WndStation.SearchWindow(pcszAnotherWnd);
				KGLOG_PROCESS_ERROR(pAnotherWnd);

				ALW_CorrectWndPosition(pWnd, pAnotherWnd, nType);
			}
			break;
		case 3:
			{
				if (lua_type(L, 2) == LUA_TNUMBER)
				{
					POINT ptPos;
					ptPos.x = (long)(Lua_ValueToNumber(L, 2) * fXScale);
					ptPos.y = (long)(Lua_ValueToNumber(L, 3) * fYScale);
					ALW_CorrectWndPosition(pWnd, ptPos, nType);
				}
				else
				{
					LPCSTR pcszAnotherWnd = Lua_ValueToString(L, 2);

					KWndWindow* pAnotherWnd = g_pUI->m_WndStation.SearchWindow(pcszAnotherWnd);
					KGLOG_PROCESS_ERROR(pAnotherWnd);

					nType = (int)Lua_ValueToNumber(L, 3);
					ALW_CorrectWndPosition(pWnd, pAnotherWnd, nType);
				}
			}
			break;
		case 4:
			{
				POINT ptPos;
				ptPos.x = (long)(Lua_ValueToNumber(L, 2) * fXScale);
				ptPos.y = (long)(Lua_ValueToNumber(L, 3) * fYScale);
				nType = (int)Lua_ValueToNumber(L, 4);
				ALW_CorrectWndPosition(pWnd, ptPos, nType);
			}
			break;
		default:
			if (nParamCount > 4)
			{
				RECT rcPos;
				rcPos.left = (long)(Lua_ValueToNumber(L, 2) * fXScale);
				rcPos.top = (long)(Lua_ValueToNumber(L, 3) * fYScale);
				rcPos.right = rcPos.left + (long)(Lua_ValueToNumber(L, 4) * fXScale);
				rcPos.bottom = rcPos.top + (long)(Lua_ValueToNumber(L, 5) * fYScale);
				if (nParamCount > 5)
					nType = (int)Lua_ValueToNumber(L, 6);
				ALW_CorrectWndPosition(pWnd, rcPos, nType);
			}
			break;
		}

Exit0:
		return 0;
	}

	int KWndClassScriptTable::LuaWindow_SetSizeWithAllChild(Lua_State *L)
	{
		int nRetCode = false;
		KWndWindow* pWnd = NULL;
		BOOL bEnable = FALSE;

		ASSERT(L);

		KGLOG_PROCESS_ERROR(lua_gettop(L) == 2);

		pWnd = (KWndWindow*)ToWndUserData("WndWindow", L, 1);
		KGLOG_PROCESS_ERROR(pWnd);

		if (lua_isboolean(L, 2))
			bEnable = (BOOL)Lua_ValueToBoolean(L, 2);
		else
			bEnable = (int)lua_tonumber(L, 2) != 0;

		pWnd->SetStyle(WND_S_SIZE_WITH_ALL_CHILD, bEnable);

Exit0:
		return 0;
	}

	int KWndClassScriptTable::LuaWindow_SetMousePenetrable(Lua_State *L)
	{
		int nRetCode = false;
		KWndWindow* pWnd = NULL;
		BOOL bEnable = FALSE;

		ASSERT(L);

		KGLOG_PROCESS_ERROR(lua_gettop(L) == 2);

		pWnd = (KWndWindow*)ToWndUserData("WndWindow", L, 1);
		KGLOG_PROCESS_ERROR(pWnd);

		if (lua_isboolean(L, 2))
			bEnable = (BOOL)Lua_ValueToBoolean(L, 2);
		else
			bEnable = (int)lua_tonumber(L, 2) != 0;

		pWnd->SetStyle(WND_S_MOUSE_PENETRABLE, bEnable);

Exit0:
		return 0;
	}

	int KWndClassScriptTable::LuaWindow_SetAlpha(Lua_State *L)
	{
		int nRetCode = false;
		KWndWindow* pWnd = NULL;
		int nAlpha = 255;
		ASSERT(L);
		KGLOG_PROCESS_ERROR(Lua_GetTopIndex(L) == 2);

		pWnd = (KWndWindow*)ToWndUserData("WndWindow", L, 1);
		KGLOG_PROCESS_ERROR(pWnd);

		nAlpha = (int)Lua_ValueToNumber(L, 2);

		pWnd->SetAlpha(nAlpha);

Exit0:
		return 0;
	}

	int KWndClassScriptTable::LuaWindow_SetSelfAlpha(Lua_State *L)
	{
		int nRetCode = false;
		KWndWindow* pWnd = NULL;
		int nAlpha = 255;
		ASSERT(L);
		KGLOG_PROCESS_ERROR(Lua_GetTopIndex(L) == 2);

		pWnd = (KWndWindow*)ToWndUserData("WndWindow", L, 1);
		KGLOG_PROCESS_ERROR(pWnd);

		nAlpha = (int)Lua_ValueToNumber(L, 2);

		pWnd->SetSelfAlpha(nAlpha);

Exit0:
		return 0;
	}

	int KWndClassScriptTable::LuaWindow_NewIndex(Lua_State *L)
	{
		int nTopIndex = lua_gettop(L);
		KWndWindow *pWnd = NULL;

		KGLOG_PROCESS_ERROR(nTopIndex == 3);

		lua_pushvalue(L, 1);
		lua_getfield(L, -1, STR_OBJ_KEY_NAME);
		pWnd = reinterpret_cast<KWndWindow *>(lua_touserdata(L, -1)); 
		KGLOG_PROCESS_ERROR(pWnd);
		lua_settop(L, nTopIndex + 1);
		if (pWnd->GetLuaRef() == LUA_NOREF)
			pWnd->SetLuaRef(luaL_ref(L, LUA_REGISTRYINDEX));
		lua_settop(L, nTopIndex);
		lua_rawset(L, 1);
Exit0:
		lua_settop(L, nTopIndex);
		return 0;
	}

	int KWndClassScriptTable::LuaWindow_Equal(Lua_State *L)
	{
		int nEquare = false;
		void *pLeft = NULL;
		void *pRight = NULL;

		KGLOG_PROCESS_ERROR(lua_gettop(L) == 2);

		lua_getfield(L, -1, STR_OBJ_KEY_NAME);
		pLeft = lua_touserdata(L, -1);
		lua_settop(L, 1);
		lua_getfield(L, -1, STR_OBJ_KEY_NAME);
		pRight = lua_touserdata(L, -1);

		if (pLeft == pRight && pLeft)
			nEquare = true;
Exit0:
		lua_settop(L, 2);
		lua_pushboolean(L, nEquare);
		return 1;
	}

	int KWndClassScriptTable::LuaWindow_GetAlpha(Lua_State *L)
	{
		int nRetCode = false;
		KWndWindow* pWnd = NULL;
		int nAlpha = 255;
		ASSERT(L);
		KGLOG_PROCESS_ERROR(Lua_GetTopIndex(L) == 1);

		pWnd = (KWndWindow*)ToWndUserData("WndWindow", L, 1);
		KGLOG_PROCESS_ERROR(pWnd);

		nAlpha = pWnd->GetAlpha();
Exit0:
		Lua_PushNumber(L, nAlpha);
		return 1;
	}

	int KWndClassScriptTable::LuaWindow_GetType(Lua_State* L)
	{
		int nRetCode = false;
		KWndWindow* pWnd = NULL;
		LPCSTR pcszType = "";

		ASSERT(L);

		KGLOG_PROCESS_ERROR(lua_gettop(L) == 1);

		pWnd = (KWndWindow*)ToWndUserData("WndWindow", L, 1);
		KGLOG_PROCESS_ERROR(pWnd);

		pcszType = pWnd->GetWndType();
Exit0:
		lua_pushstring(L, pcszType);
		return 1;
	}

	int KWndClassScriptTable::LuaWindow_ChangeRelation(Lua_State *L)
	{
		int nRetCode = false;
		KWndWindow* pWnd = NULL;
		KWndWindow* pRelation = NULL;
		int nLayer = false;
		int nBehind = true;
		int nChild = false;
        int nParam = 0;

		ASSERT(L);
		
        nParam = Lua_GetTopIndex(L);
		KGLOG_PROCESS_ERROR(nParam > 1);

		pWnd = (KWndWindow*)ToWndUserData("WndWindow", L, 1);
		KGLOG_PROCESS_ERROR(pWnd);

		KGLOG_PROCESS_ERROR(pWnd->GetParent());

		if (Lua_IsTable(L, 2))
		{
			pRelation = (KWndWindow*)ToWndUserData("WndWindow", L, 2);
			KGLOG_PROCESS_ERROR(pRelation);
		}
		else
		{
			LPCSTR pcszPath = Lua_ValueToString(L, 2);
			KGLOG_PROCESS_ERROR(pcszPath);

			pRelation = g_pUI->m_WndStation.SearchWindow(pcszPath);
			KGLOG_PROCESS_ERROR(pRelation);
		}
		
		if (!pRelation->GetParent() && KWndStation::GetLayerIndexByName(pRelation->GetName()) != WL_UNKNOWN)
		{
			KG_PROCESS_ERROR((KWndFrame*)CastWndWindow(pWnd, "WndFrame"));
			nLayer = true;
		}

		if (nParam > 2)
		{
			if (Lua_IsNumber(L, 3))
				nBehind = (int)Lua_ValueToNumber(L, 3);
			else
				nBehind = Lua_ValueToBoolean(L, 3);
		}

		if (nParam > 3)
		{
			if (Lua_IsNumber(L, 4))
				nChild = (int)Lua_ValueToNumber(L, 4);
			else
				nChild = Lua_ValueToBoolean(L, 4);
		}

		if (nLayer)
		{
			if (!nBehind && pRelation->GetFirstChild()) //放在这一层的最低下
			{
				pRelation->GetFirstChild()->AddBrother(pWnd, true);
				pWnd->AddBrother(pRelation->GetFirstChild(), true);
			}
			else
				pRelation->AddChild(pWnd); //放在末尾。
		}
		else
		{
			if (nChild)
			{
				KGLOG_PROCESS_ERROR(!(KWndFrame*)CastWndWindow(pWnd, "WndFrame"));
				pRelation->AddChild(pWnd);
			}
			else
			{
				if ((KWndFrame*)CastWndWindow(pWnd, "WndFrame"))
				{
					KGLOG_PROCESS_ERROR((KWndFrame*)CastWndWindow(pRelation, "WndFrame"));
				}
				else
				{
					KGLOG_PROCESS_ERROR(!(KWndFrame*)CastWndWindow(pRelation, "WndFrame"));
				}
				if (nBehind)
					pRelation->AddBrother(pWnd, true);
				else
				{
					pRelation->AddBrother(pWnd, true);
					pWnd->AddBrother(pRelation, true);
				}
			}
		}

Exit0:
		return 0;
	}

	int KWndClassScriptTable::LuaWindow_RebuildEventArray(Lua_State *L)
	{
		int nRetCode = false;
		KWndWindow* pWnd = NULL;
		ASSERT(L);
		KGLOG_PROCESS_ERROR(Lua_GetTopIndex(L) == 1);

		pWnd = (KWndWindow*)ToWndUserData("WndWindow", L, 1);
		KGLOG_PROCESS_ERROR(pWnd);

		pWnd->UpdateEvent();

Exit0:
		return 0;
	}

	int KWndClassScriptTable::LuaWindow_GetTreePath(Lua_State *L)
	{
		int nRetCode = false;
		LPCSTR pcszTree = NULL;
		KWndWindow* pWnd = NULL;

		ASSERT(L);

		KGLOG_PROCESS_ERROR(Lua_GetTopIndex(L) == 1);

		pWnd = (KWndWindow*)ToWndUserData("WndWindow", L, 1);
		KGLOG_PROCESS_ERROR(pWnd);

		pcszTree = g_pUI->m_WndStation.GetTreePath(pWnd);
Exit0:
		if (!pcszTree)
			pcszTree = "\0";
		Lua_PushString(L, pcszTree);
		return 1;
	}

	int KWndClassScriptTable::LuaWindow_SetPoint(Lua_State* L)
	{
		BOOL bRetCode = FALSE;
		int nParamCount = 0;
		int nSide = 0;
		int nRelSide = 0;
		float fX = 0.0f;
		float fY = 0.0f;
		float fRelOffsetX = 0.0f;
		float fRelOffsetY = 0.0f;
		LPCSTR szSide = NULL;
		LPCSTR szRelSide = NULL;
		LPCSTR szRelTreePath = NULL;
		KWndWindow* pWnd = NULL;
		float fUIScale = 1.0f;

		ASSERT(L);

		nParamCount = Lua_GetTopIndex(L);
		KGLOG_PROCESS_ERROR(nParamCount == 3 || nParamCount == 7 || nParamCount == 8);

		pWnd = (KWndWindow*)ToWndUserData("WndWindow", L, 1);
		KGLOG_PROCESS_ERROR(pWnd);

		fUIScale = g_pUI->m_WndStation.GetUIScale();

		if (nParamCount == 3)
		{
			fX = (float)Lua_ValueToNumber(L, 2);
			fY = (float)Lua_ValueToNumber(L, 3);

			fX *= fUIScale;
			fY *= fUIScale;

			pWnd->SetPoint(fX, fY);
		}
		else
		{
			KWndWindow* pRelWnd = NULL;

			if (nParamCount == 7)
			{
				szSide = Lua_ValueToString(L, 2);
				KGLOG_PROCESS_ERROR(szSide);

				fX = (float)Lua_ValueToNumber(L, 3);
				fY = (float)Lua_ValueToNumber(L, 4);

				szRelSide = Lua_ValueToString(L, 5);
				KGLOG_PROCESS_ERROR(szRelSide);

				fRelOffsetX = (float)Lua_ValueToNumber(L, 6);
				fRelOffsetY = (float)Lua_ValueToNumber(L, 7);
			}
			else
			{
				szSide = Lua_ValueToString(L, 2);
				KGLOG_PROCESS_ERROR(szSide);

				fX = (float)Lua_ValueToNumber(L, 3);
				fY = (float)Lua_ValueToNumber(L, 4);

				if (Lua_IsTable(L, 5))
				{
					pRelWnd = (KWndWindow*)ToWndUserData("WndWindow", L, 5);
					KGLOG_PROCESS_ERROR(pRelWnd);
				}
				else
				{
					szRelTreePath = Lua_ValueToString(L, 5);

					pRelWnd = g_pUI->m_WndStation.SearchWindow(szRelTreePath);
					KGLOG_PROCESS_ERROR(pRelWnd);
				}

				szRelSide = Lua_ValueToString(L, 6);
				KGLOG_PROCESS_ERROR(szRelSide);

				fRelOffsetX = (float)Lua_ValueToNumber(L, 7);
				fRelOffsetY = (float)Lua_ValueToNumber(L, 8);
			}

			nSide = KWndWindow::GetSide(szSide);
			KGLOG_PROCESS_ERROR(nSide >= WND_SIDE_BEGIN && nSide < WND_SIDE_END);

			nRelSide = KWndWindow::GetSide(szRelSide);
			KGLOG_PROCESS_ERROR(nRelSide >= WND_SIDE_BEGIN && nRelSide < WND_SIDE_END);

			fX *= fUIScale;
			fY *= fUIScale;

			fRelOffsetX *= fUIScale;
			fRelOffsetY *= fUIScale;

			bRetCode = pWnd->SetPoint(nSide, fX, fY, pRelWnd, nRelSide, fRelOffsetX, fRelOffsetY);
			KGLOG_PROCESS_ERROR(bRetCode);
		}

		return 0;
Exit0:
		KGLogPrintf(KGLOG_ERR, "KGUI KWndClassScriptTable::LuaWindow_SetPoint(%d, %d, %f, %f, %f, %f, %s, %s, %s, %s",
			nSide, nRelSide, fX, fY, fRelOffsetX, fRelOffsetY, 
			szSide ? szSide : "", 
			szRelSide ? szRelSide : "", 
			g_pUI->m_WndStation.GetTreePath(pWnd) ? g_pUI->m_WndStation.GetTreePath(pWnd) : "", 
			szRelTreePath ? szRelTreePath : "");
		return 0;
	}

	int KWndClassScriptTable::LuaWindow_SetAreaTestFile(Lua_State* L)
	{
		int nRetCode = false;
		const char * pcszFile = NULL;
		int nParamCount = 0;
		KWndWindow* pWnd = NULL;
		int nAlpha = 255;
		ASSERT(L);

		nParamCount = Lua_GetTopIndex(L);
		KGLOG_PROCESS_ERROR(nParamCount > 0);

		pWnd = (KWndWindow*)ToWndUserData("WndWindow", L, 1);
		KGLOG_PROCESS_ERROR(pWnd);

		if (nParamCount > 1)
			pcszFile = Lua_ValueToString(L, 2);

		if (pcszFile)
			pcszFile = FormatFilePath(pcszFile);

		pWnd->SetAreaTestFile(pcszFile);

Exit0:
		return 0;

	}

	int KWndClassScriptTable::LuaWindow_Destroy(Lua_State* L)
	{
		int nRetCode = false;
		KWndWindow* pWnd = NULL;
		ASSERT(L);
		KGLOG_PROCESS_ERROR(Lua_GetTopIndex(L) == 1);

		pWnd = (KWndWindow*)ToWndUserData("WndWindow", L, 1);
		KGLOG_PROCESS_ERROR(pWnd);

		pWnd->Destroy(false);

Exit0:
		return 0;
	}

	int KWndClassScriptTable::LuaWindow_StartMoving(Lua_State* L)
	{
		int nRetCode = false;
		KWndWindow* pWnd = NULL;
		ASSERT(L);
		KGLOG_PROCESS_ERROR(Lua_GetTopIndex(L) == 1);

		pWnd = (KWndWindow*)ToWndUserData("WndWindow", L, 1);
		KGLOG_PROCESS_ERROR(pWnd);

		g_pUI->m_WndStation.AddMovingWindow(pWnd);

Exit0:
		return 0;

	}

	int KWndClassScriptTable::LuaWindow_EndMoving(Lua_State* L)
	{
		int nRetCode = false;
		KWndWindow* pWnd = NULL;
		ASSERT(L);
		KGLOG_PROCESS_ERROR(Lua_GetTopIndex(L) == 1);

		pWnd = (KWndWindow*)ToWndUserData("WndWindow", L, 1);
		KGLOG_PROCESS_ERROR(pWnd);

		g_pUI->m_WndStation.RemoveMovingWindow(pWnd);

Exit0:
		return 0;
	}


	int KWndClassScriptTable::LuaWindow_IsValid(Lua_State *L)
	{
		int nIsValid = false;
		KWndWindow* pWnd = NULL;

		ASSERT(L);
		KGLOG_PROCESS_ERROR(Lua_GetTopIndex(L) == 1);

		if (ToWndUserData("WndWindow", L, 1))
			nIsValid = true;
Exit0:
		Lua_PushBoolean(L, nIsValid);
		return 1;
	}

	int KWndClassScriptTable::LuaEdit_SetText(Lua_State* L)
	{
		int nRetCode = false;
		LPCSTR szText = NULL;
		KWndEdit* pWnd = NULL;

		ASSERT(L);
		KGLOG_PROCESS_ERROR(Lua_GetTopIndex(L) == 2);

		pWnd = (KWndEdit*)ToWndUserData("WndEdit", L, 1);
		KGLOG_PROCESS_ERROR(pWnd);

		szText = Lua_ValueToString(L, 2);
		if (szText)
			pWnd->SetText(ATL::CA2W(szText, GetCodePage()));
		else
			pWnd->SetText(L"");

Exit0:
		return 0;
	}

	int KWndClassScriptTable::LuaEdit_GetText(Lua_State* L)
	{
		int nRetCode = false;
		LPCWSTR wszText = NULL;
		KWndEdit* pWnd = NULL;

		ASSERT(L);
		KGLOG_PROCESS_ERROR(Lua_GetTopIndex(L) == 1);

		pWnd = (KWndEdit*)ToWndUserData("WndEdit", L, 1);
		KGLOG_PROCESS_ERROR(pWnd);

		wszText = pWnd->GetText();
		KGLOG_PROCESS_ERROR(wszText);

Exit0:
		if (wszText == NULL)
			wszText = L"";
		Lua_PushString(L, ATL::CW2A(wszText, GetCodePage()));
		return 1;
	}

	int KWndClassScriptTable::LuaEdit_GetTextLength(Lua_State* L)
	{
		int nRetCode = false;
		int nLength = 0;
		KWndEdit* pWnd = NULL;
		BOOL bMultiByte = TRUE;
		int nParam = Lua_GetTopIndex(L);
		KGLOG_PROCESS_ERROR(nParam);

		pWnd = (KWndEdit*)ToWndUserData("WndEdit", L, 1);
		KGLOG_PROCESS_ERROR(pWnd);

		if (nParam > 1)
		{
			if (lua_isboolean(L, 2))
				bMultiByte = (BOOL)(Lua_ValueToBoolean(L, 2));
			else
				bMultiByte = (BOOL)(int)(Lua_ValueToNumber(L, 2));
		}

		nLength = pWnd->GetTextLength(bMultiByte);
Exit0:
		Lua_PushNumber(L, nLength);
		return 1;
	}

	int KWndClassScriptTable::LuaEdit_ClearText(Lua_State* L)
	{
		int nRetCode = false;
		KWndEdit* pWnd = NULL;

		ASSERT(L);
		KGLOG_PROCESS_ERROR(Lua_GetTopIndex(L) == 1);

		pWnd = (KWndEdit*)ToWndUserData("WndEdit", L, 1);
		KGLOG_PROCESS_ERROR(pWnd);

		pWnd->ClearText();

Exit0:
		return 0;
	}

	int KWndClassScriptTable::LuaEdit_InsertObj(Lua_State* L)
	{
		int nRetCode = false;
		KWndEdit *pWnd = NULL;
		LPCSTR pcszText = NULL;
		int nRef = LUA_NOREF;
		ASSERT(L);

		int nTopIndex = Lua_GetTopIndex(L);
		KGLOG_PROCESS_ERROR(nTopIndex == 3);

		pWnd = (KWndEdit*)ToWndUserData("WndEdit", L, 1);
		KGLOG_PROCESS_ERROR(pWnd);

		pcszText = Lua_ValueToString(L, 2);
		KGLOG_PROCESS_ERROR(pcszText);

		nRef = luaL_ref(L, LUA_REGISTRYINDEX);
		KGLOG_PROCESS_ERROR(nRef != LUA_NOREF);

		if (!pWnd->InsertObject(K_INPUT_OBJ_NAME, ATL::CA2W(pcszText, GetCodePage()), nRef))
			luaL_unref(L, LUA_REGISTRYINDEX, nRef);

Exit0:
		Lua_SetTopIndex(L, nTopIndex);
		return 0;
	}

    int KWndClassScriptTable::LuaEdit_GetTextStruct(Lua_State* L)
    {
        int nResult  = 0;
        int nRetCode = false;

        KWndEdit *pWnd = NULL;
        
        ASSERT(L);
        KGLOG_PROCESS_ERROR(Lua_GetTopIndex(L) == 1);

		pWnd = (KWndEdit*)ToWndUserData("WndEdit", L, 1);
		KGLOG_PROCESS_ERROR(pWnd);

        nRetCode = pWnd->GetTextLuaStruct(L);
        KGLOG_PROCESS_ERROR(nRetCode);

        nResult = nRetCode; // nRetCode should be 1, GetTextLuaStruct returns a table.
Exit0:
        return nResult;
    }

	int KWndClassScriptTable::LuaEdit_SetType(Lua_State* L)
	{
		int nRetCode = false;
		int nType = 0;
		KWndEdit* pWnd = NULL;

		ASSERT(L);
		KGLOG_PROCESS_ERROR(Lua_GetTopIndex(L) == 2);

		pWnd = (KWndEdit*)ToWndUserData("WndEdit", L, 1);
		KGLOG_PROCESS_ERROR(pWnd);

		nType = (int)Lua_ValueToNumber(L, 2);

		pWnd->SetType(nType);

Exit0:
		return 0;
	}

	int KWndClassScriptTable::LuaEdit_SelectAll(Lua_State* L)
	{
		int nRetCode = false;
		KWndEdit* pWnd = NULL;

		ASSERT(L);
		KGLOG_PROCESS_ERROR(Lua_GetTopIndex(L) == 1);

		pWnd = (KWndEdit*)ToWndUserData("WndEdit", L, 1);
		KGLOG_PROCESS_ERROR(pWnd);

		pWnd->SelectAll();

Exit0:
		return 0;
	}

	int KWndClassScriptTable::LuaEdit_CancelSelect(Lua_State* L)
	{
		int nRetCode = false;
		KWndEdit* pWnd = NULL;

		ASSERT(L);
		KGLOG_PROCESS_ERROR(Lua_GetTopIndex(L) == 1);

		pWnd = (KWndEdit*)ToWndUserData("WndEdit", L, 1);
		KGLOG_PROCESS_ERROR(pWnd);

		pWnd->CancelSelect();

Exit0:
		return 0;
	}

	int KWndClassScriptTable::LuaEdit_SetFontScheme(Lua_State* L)
	{
		int nRetCode = false;
		KWndEdit* pWnd = NULL;
		int nFont = 0;

		ASSERT(L);
		KGLOG_PROCESS_ERROR(Lua_GetTopIndex(L) == 2);

		pWnd = (KWndEdit*)ToWndUserData("WndEdit", L, 1);
		KGLOG_PROCESS_ERROR(pWnd);

		nFont = (int)Lua_ValueToNumber(L, 2);
		pWnd->SetFontScheme(nFont);

Exit0:
		return 0;
	}

	int KWndClassScriptTable::LuaEdit_GetFontScheme(Lua_State* L)
	{
		int nRetCode = false;
		int nFont = 0;
		KWndEdit* pWnd = NULL;

		ASSERT(L);
		KGLOG_PROCESS_ERROR(Lua_GetTopIndex(L) == 1);

		pWnd = (KWndEdit*)ToWndUserData("WndEdit", L, 1);
		KGLOG_PROCESS_ERROR(pWnd);

		nFont = pWnd->GetFontScheme();

Exit0:
		Lua_PushNumber(L, nFont);
		return 1;
	}

	int KWndClassScriptTable::LuaEdit_SetFontColor(Lua_State* L)
	{
		int nRetCode = false;
		KWndEdit* pWnd = NULL;
		DWORD dwR = 255;
		DWORD dwG = 255;
		DWORD dwB = 255;

		ASSERT(L);
		KGLOG_PROCESS_ERROR(Lua_GetTopIndex(L) == 4);

		pWnd = (KWndEdit*)ToWndUserData("WndEdit", L, 1);
		KGLOG_PROCESS_ERROR(pWnd);

		dwR = ((DWORD)Lua_ValueToNumber(L, 2) & 0xFF);
		dwG = ((DWORD)Lua_ValueToNumber(L, 3) & 0xFF);
		dwB = ((DWORD)Lua_ValueToNumber(L, 4) & 0xFF);

		pWnd->SetFontColor(0xFF000000 | dwR << 16 | dwG << 8 | dwB );

Exit0:
		return 0;
	}

	int KWndClassScriptTable::LuaEdit_SetLimit(Lua_State* L)
	{
		int nRetCode = false;
		KWndEdit* pWnd = NULL;
		int nLimit = -1;

		ASSERT(L);
		KGLOG_PROCESS_ERROR(Lua_GetTopIndex(L) == 2);

		pWnd = (KWndEdit*)ToWndUserData("WndEdit", L, 1);
		KGLOG_PROCESS_ERROR(pWnd);

		nLimit = (int)Lua_ValueToNumber(L, 2);

		pWnd->SetLimit(nLimit);
Exit0:
		return 0;
	}

	int KWndClassScriptTable::LuaEdit_GetLimit(Lua_State* L)
	{
		int nRetCode = false;
		KWndEdit* pWnd = NULL;
		int nLimit = -1;

		ASSERT(L);
		KGLOG_PROCESS_ERROR(Lua_GetTopIndex(L) == 1);

		pWnd = (KWndEdit*)ToWndUserData("WndEdit", L, 1);
		KGLOG_PROCESS_ERROR(pWnd);

		nLimit = pWnd->GetLimit();

Exit0:
		Lua_PushNumber(L, nLimit);
		return 1;
	}

	int KWndClassScriptTable::LuaEdit_SetLimitMultiByte(Lua_State* L)
	{
		int nRetCode = false;
		KWndEdit* pWnd = NULL;
		BOOL bMultiByte = TRUE;

		ASSERT(L);
		KGLOG_PROCESS_ERROR(Lua_GetTopIndex(L) == 2);

		pWnd = (KWndEdit*)ToWndUserData("WndEdit", L, 1);
		KGLOG_PROCESS_ERROR(pWnd);

		if (lua_isboolean(L, 2))
			bMultiByte = (BOOL)Lua_ValueToBoolean(L, 2);
		else
			bMultiByte = (BOOL)((int)Lua_ValueToNumber(L, 2));

		pWnd->SetLimitMultiByte(bMultiByte);
Exit0:
		return 0;
	}

	int KWndClassScriptTable::LuaEdit_IsLimitMultiByte(Lua_State* L)
	{
		int nRetCode = false;
		KWndEdit* pWnd = NULL;
		BOOL bMultiByte = TRUE;

		ASSERT(L);
		KGLOG_PROCESS_ERROR(Lua_GetTopIndex(L) == 1);

		pWnd = (KWndEdit*)ToWndUserData("WndEdit", L, 1);
		KGLOG_PROCESS_ERROR(pWnd);

		bMultiByte = (BOOL)pWnd->IsLimitMultiByte();

Exit0:
		Lua_PushBoolean(L, bMultiByte);
		return 1;
	}


	int KWndClassScriptTable::LuaEdit_InsertText(Lua_State* L)
	{
		int nRetCode = false;
		KWndEdit* pWnd = NULL;
		LPCSTR pcszText = NULL;

		ASSERT(L);
		KGLOG_PROCESS_ERROR(Lua_GetTopIndex(L) == 2);

		pWnd = (KWndEdit*)ToWndUserData("WndEdit", L, 1);
		KGLOG_PROCESS_ERROR(pWnd);

		pcszText = (LPCSTR)Lua_ValueToString(L, 2);
		KGLOG_PROCESS_ERROR(pcszText);

		pWnd->AppendString(ATL::CA2W(pcszText, GetCodePage()));

Exit0:
		return 0;
	}


	int KWndClassScriptTable::LuaEdit_Backspace(Lua_State* L)
	{
		int nRetCode = false;
		KWndEdit* pWnd = NULL;

		ASSERT(L);
		KGLOG_PROCESS_ERROR(Lua_GetTopIndex(L) == 1);

		pWnd = (KWndEdit*)ToWndUserData("WndEdit", L, 1);
		KGLOG_PROCESS_ERROR(pWnd);

		pWnd->Backspace();

Exit0:
		return 0;
	}

	int KWndClassScriptTable::LuaEdit_SetMultiLine(Lua_State* L)
	{
		int nRetCode = false;
		KWndEdit* pWnd = NULL;
		BOOL bMultiLine = TRUE;

		ASSERT(L);
		KGLOG_PROCESS_ERROR(Lua_GetTopIndex(L) == 2);

		pWnd = (KWndEdit*)ToWndUserData("WndEdit", L, 1);
		KGLOG_PROCESS_ERROR(pWnd);

		if (lua_isboolean(L, 2))
			bMultiLine = (BOOL)Lua_ValueToBoolean(L, 2);
		else
			bMultiLine = (BOOL)((int)Lua_ValueToNumber(L, 2));

		pWnd->SetStatus(WNDEDIT_ES_MULTI_LINE, bMultiLine);
Exit0:
		return 0;

	}

	int KWndClassScriptTable::LuaEdit_IsMultiLine(Lua_State* L)
	{
		int nRetCode = false;
		KWndEdit* pWnd = NULL;
		BOOL bMultiLine = FALSE;

		ASSERT(L);
		KGLOG_PROCESS_ERROR(Lua_GetTopIndex(L) == 2);

		pWnd = (KWndEdit*)ToWndUserData("WndEdit", L, 1);
		KGLOG_PROCESS_ERROR(pWnd);

		if (lua_isboolean(L, 2))
			bMultiLine = (BOOL)Lua_ValueToBoolean(L, 2);
		else
			bMultiLine = (BOOL)((int)Lua_ValueToNumber(L, 2));

		bMultiLine = pWnd->IsStatus(WNDEDIT_ES_MULTI_LINE);
Exit0:
		Lua_PushBoolean(L, bMultiLine);
		return 1;
	}

	int KWndClassScriptTable::LuaEdit_SetFontSpacing(Lua_State* L)
	{
		int nRetCode = false;
		KWndEdit* pWnd = NULL;
		float fFontSpacing = 0.0f;

		ASSERT(L);
		KGLOG_PROCESS_ERROR(Lua_GetTopIndex(L) == 2);

		pWnd = (KWndEdit*)ToWndUserData("WndEdit", L, 1);
		KGLOG_PROCESS_ERROR(pWnd);

		fFontSpacing = (float)Lua_ValueToNumber(L, 2);

		pWnd->SetFontSpacing(fFontSpacing);
Exit0:
		return 0;
	}

	int KWndClassScriptTable::LuaEdit_SetRowSpacing(Lua_State* L)
	{
		int nRetCode = false;
		KWndEdit* pWnd = NULL;
		float fRowSpacing = 0.0f;

		ASSERT(L);
		KGLOG_PROCESS_ERROR(Lua_GetTopIndex(L) == 2);

		pWnd = (KWndEdit*)ToWndUserData("WndEdit", L, 1);
		KGLOG_PROCESS_ERROR(pWnd);

		fRowSpacing = (float)Lua_ValueToNumber(L, 2);

		pWnd->SetRowSpacing(fRowSpacing);
Exit0:
		return 0;
	}

	int KWndClassScriptTable::LuaEdit_SetFocusBgColor(Lua_State* L)
	{
		int nRetCode = false;
		KWndEdit* pWnd = NULL;
		DWORD dwR = 255;
		DWORD dwG = 255;
		DWORD dwB = 255;

		ASSERT(L);
		KGLOG_PROCESS_ERROR(Lua_GetTopIndex(L) == 4);

		pWnd = (KWndEdit*)ToWndUserData("WndEdit", L, 1);
		KGLOG_PROCESS_ERROR(pWnd);

		dwR = ((DWORD)Lua_ValueToNumber(L, 2) & 0xFF);
		dwG = ((DWORD)Lua_ValueToNumber(L, 3) & 0xFF);
		dwB = ((DWORD)Lua_ValueToNumber(L, 4) & 0xFF);

		pWnd->SetFocusBgColor(0xFF000000 | dwR << 16 | dwG << 8 | dwB );

Exit0:
		return 0;
	}

	int KWndClassScriptTable::LuaEdit_SetSelectBgColor(Lua_State* L)
	{
		int nRetCode = false;
		KWndEdit* pWnd = NULL;
		DWORD dwR = 255;
		DWORD dwG = 255;
		DWORD dwB = 255;

		ASSERT(L);
		KGLOG_PROCESS_ERROR(Lua_GetTopIndex(L) == 4);

		pWnd = (KWndEdit*)ToWndUserData("WndEdit", L, 1);
		KGLOG_PROCESS_ERROR(pWnd);

		dwR = ((DWORD)Lua_ValueToNumber(L, 2) & 0xFF);
		dwG = ((DWORD)Lua_ValueToNumber(L, 3) & 0xFF);
		dwB = ((DWORD)Lua_ValueToNumber(L, 4) & 0xFF);

		pWnd->SetSelectBgColor(0xFF000000 | dwR << 16 | dwG << 8 | dwB );

Exit0:
		return 0;
	}

	int KWndClassScriptTable::LuaEdit_SetSelectFontScheme(Lua_State* L)
	{
		int nRetCode = false;
		KWndEdit* pWnd = NULL;
		int nFont = 0;

		ASSERT(L);
		KGLOG_PROCESS_ERROR(Lua_GetTopIndex(L) == 2);

		pWnd = (KWndEdit*)ToWndUserData("WndEdit", L, 1);
		KGLOG_PROCESS_ERROR(pWnd);

		nFont = (int)Lua_ValueToNumber(L, 2);
		pWnd->SetSelectFontScheme(nFont);

Exit0:
		return 0;
	}

	int KWndClassScriptTable::LuaEdit_SetCaretFontScheme(Lua_State* L)
	{
		int nRetCode = false;
		KWndEdit* pWnd = NULL;
		int nFont = 0;

		ASSERT(L);
		KGLOG_PROCESS_ERROR(Lua_GetTopIndex(L) == 2);

		pWnd = (KWndEdit*)ToWndUserData("WndEdit", L, 1);
		KGLOG_PROCESS_ERROR(pWnd);

		nFont = (int)Lua_ValueToNumber(L, 2);
		pWnd->SetCaretFontScheme(nFont);

Exit0:
		return 0;
	}

	int KWndClassScriptTable::LuaEdit_SetCurSel(Lua_State* L)
	{
		int nRetCode = false;
		KWndEdit* pWnd = NULL;
		int nStart = 1;
		int nEnd = -1;
		int nLen = 0;
		BOOL bMultiByte = TRUE;
		int nParamCount = Lua_GetTopIndex(L);

		KGLOG_PROCESS_ERROR(nParamCount > 1);

		pWnd = (KWndEdit*)ToWndUserData("WndEdit", L, 1);
		KGLOG_PROCESS_ERROR(pWnd);

		nStart = (int)Lua_ValueToNumber(L, 2);
		if (nParamCount > 2)
			nEnd = (int)Lua_ValueToNumber(L, 3);
		if (nParamCount > 3)
		{
			if (lua_isboolean(L, 4))
				bMultiByte = Lua_ValueToBoolean(L, 4);
			else
				bMultiByte = (BOOL)((int)Lua_ValueToNumber(L, 4));
		}

		nLen = pWnd->GetTextLength(TRUE);
		if (nStart < 0)
			nStart = nLen + nStart;
		else if (nStart > 0)
			nStart--;

		if (nEnd < 0)
			nEnd = nLen + nEnd;
		else if (nEnd > 0)
			nEnd--;
		pWnd->SetCurSel(nStart, nEnd, TRUE);
Exit0:
		return 0;
	}

	int KWndClassScriptTable::LuaButton_IsEnabled(Lua_State* L)
	{
		int nRetCode = false;
		KWndButton* pWnd = NULL;
		int nEnabled = false;

		ASSERT(L);
		KG_PROCESS_ERROR(Lua_GetTopIndex(L) == 1);

		pWnd = (KWndButton*)ToWndUserData("WndButton", L, 1);
		KGLOG_PROCESS_ERROR(pWnd);

		nEnabled = pWnd->IsButtonEnabled();
Exit0:
		Lua_PushBoolean(L, nEnabled);
		return 1;
	}

	int KWndClassScriptTable::LuaButton_Enable(Lua_State* L)
	{
		int nRetCode = false;
		int bEnable = false;
		KWndButton* pWnd = NULL;

		ASSERT(L);

		KG_PROCESS_ERROR(Lua_GetTopIndex(L) == 2);

		pWnd = (KWndButton*)ToWndUserData("WndButton", L, 1);
		KGLOG_PROCESS_ERROR(pWnd);

		if (lua_isboolean(L, 2))
			bEnable = Lua_ValueToBoolean(L, 2);
		else
			bEnable = (int)Lua_ValueToNumber(L, 2);

		pWnd->EnableButton(bEnable);
Exit0:
		return 0;
	}

	
	int KWndClassScriptTable::LuaButton_SetAnimateGroupNormal(Lua_State* L)
	{
		int nRetCode = false;
		int nGroup = 0;
		KWndButton* pWnd = NULL;

		ASSERT(L);

		KG_PROCESS_ERROR(Lua_GetTopIndex(L) == 2);

		pWnd = (KWndButton*)ToWndUserData("WndButton", L, 1);
		KGLOG_PROCESS_ERROR(pWnd);

		nGroup = (int)Lua_ValueToNumber(L, 2);

		pWnd->SetAnimateGroupNormal(nGroup);
Exit0:
		return 0;
	}	
	
	int KWndClassScriptTable::LuaButton_SetAnimateGroupMouseOver(Lua_State* L)
	{
		int nRetCode = false;
		int nGroup = 0;
		KWndButton* pWnd = NULL;

		ASSERT(L);

		KG_PROCESS_ERROR(Lua_GetTopIndex(L) == 2);

		pWnd = (KWndButton*)ToWndUserData("WndButton", L, 1);
		KGLOG_PROCESS_ERROR(pWnd);

		nGroup = (int)Lua_ValueToNumber(L, 2);

		pWnd->SetAnimateGroupMouseOver(nGroup);
Exit0:
		return 0;
	}

	int KWndClassScriptTable::LuaButton_SetAnimateGroupMouseDown(Lua_State* L)
	{
		int nRetCode = false;
		int nGroup = 0;
		KWndButton* pWnd = NULL;

		ASSERT(L);

		KG_PROCESS_ERROR(Lua_GetTopIndex(L) == 2);

		pWnd = (KWndButton*)ToWndUserData("WndButton", L, 1);
		KGLOG_PROCESS_ERROR(pWnd);

		nGroup = (int)Lua_ValueToNumber(L, 2);

		pWnd->SetAnimateGroupMouseDown(nGroup);
Exit0:
		return 0;
	}
	
	int KWndClassScriptTable::LuaButton_SetAnimateGroupDisable(Lua_State* L)
	{
		int nRetCode = false;
		int nGroup = 0;
		KWndButton* pWnd = NULL;

		ASSERT(L);

		KG_PROCESS_ERROR(Lua_GetTopIndex(L) == 2);

		pWnd = (KWndButton*)ToWndUserData("WndButton", L, 1);
		KGLOG_PROCESS_ERROR(pWnd);

		nGroup = (int)Lua_ValueToNumber(L, 2);
		
		pWnd->SetAnimateGroupDisable(nGroup);
Exit0:
		return 0;
	}	

	int KWndClassScriptTable::LuaButton_RegisterLButtonDrag(Lua_State* L)
	{
		int nRetCode = false;
		KWndButton* pWnd = NULL;

		ASSERT(L);

		KG_PROCESS_ERROR(Lua_GetTopIndex(L) == 1);

		pWnd = (KWndButton*)ToWndUserData("WndButton", L, 1);
		KGLOG_PROCESS_ERROR(pWnd);

		pWnd->RegisterLButtonDrag();
Exit0:
		return 0;
	}

	int KWndClassScriptTable::LuaButton_UnregisterLButtonDrag(Lua_State* L)
	{
		int nRetCode = false;
		KWndButton* pWnd = NULL;

		ASSERT(L);

		KG_PROCESS_ERROR(Lua_GetTopIndex(L) == 1);

		pWnd = (KWndButton*)ToWndUserData("WndButton", L, 1);
		KGLOG_PROCESS_ERROR(pWnd);

		pWnd->UnregisterLButtonDrag();
Exit0:
		return 0;
	}

	int KWndClassScriptTable::LuaButton_IsLButtonDragable(Lua_State* L)
	{
		int nRetCode = false;
		int nDrageable = false;
		KWndButton* pWnd = NULL;

		ASSERT(L);

		KG_PROCESS_ERROR(Lua_GetTopIndex(L) == 1);

		pWnd = (KWndButton*)ToWndUserData("WndButton", L, 1);
		KGLOG_PROCESS_ERROR(pWnd);

        nDrageable = pWnd->IsLButtonDragable();
Exit0:
		Lua_PushBoolean(L, nDrageable);	
        return 1;
	}

	int KWndClassScriptTable::LuaButton_RegisterRButtonDrag(Lua_State* L)
	{
		int nRetCode = false;
		KWndButton* pWnd = NULL;

		ASSERT(L);

		KG_PROCESS_ERROR(Lua_GetTopIndex(L) == 1);

		pWnd = (KWndButton*)ToWndUserData("WndButton", L, 1);
		KGLOG_PROCESS_ERROR(pWnd);

		pWnd->RegisterRButtonDrag();
Exit0:
		return 0;
	}

	int KWndClassScriptTable::LuaButton_UnregisterRButtonDrag(Lua_State* L)
	{
		int nRetCode = false;
		KWndButton* pWnd = NULL;

		ASSERT(L);

		KG_PROCESS_ERROR(Lua_GetTopIndex(L) == 1);

		pWnd = (KWndButton*)ToWndUserData("WndButton", L, 1);
		KGLOG_PROCESS_ERROR(pWnd);

		pWnd->UnregisterRButtonDrag();
Exit0:
		return 0;
	}

	int KWndClassScriptTable::LuaButton_IsRButtonDragable(Lua_State* L)
	{
		int nRetCode = false;
		int nDrageable = false;
		KWndButton* pWnd = NULL;

		ASSERT(L);

		KG_PROCESS_ERROR(Lua_GetTopIndex(L) == 1);

		pWnd = (KWndButton*)ToWndUserData("WndButton", L, 1);
		KGLOG_PROCESS_ERROR(pWnd);

        nDrageable = pWnd->IsRButtonDragable();
Exit0:
		Lua_PushBoolean(L, nDrageable);	
		return 1;
	}

	int KWndClassScriptTable::LuaCheckBox_IsCheckBoxActive(Lua_State* L)
	{
		int nRetCode = false;
		int nActive = false;
		KWndCheckBox* pWnd = NULL;

		ASSERT(L);

		KG_PROCESS_ERROR(Lua_GetTopIndex(L) == 1);

		pWnd = (KWndCheckBox*)ToWndUserData("WndCheckBox", L, 1);
		KGLOG_PROCESS_ERROR(pWnd);

		nActive = pWnd->IsCheckBoxActive();
Exit0:
		Lua_PushBoolean(L, nActive);
		return 1;
	}

	int KWndClassScriptTable::LuaCheckBox_Enable(Lua_State* L)
	{
		int nRetCode = false;
		int bEnable = false;
		KWndCheckBox* pWnd = NULL;

		ASSERT(L);
		KG_PROCESS_ERROR(Lua_GetTopIndex(L) == 2);

		pWnd = (KWndCheckBox*)ToWndUserData("WndCheckBox", L, 1);
		KGLOG_PROCESS_ERROR(pWnd);

		if (lua_isboolean(L, 2))
			bEnable = Lua_ValueToBoolean(L, 2);
		else
			bEnable = (int)Lua_ValueToNumber(L, 2);

		pWnd->EnableCheck(bEnable);
Exit0:
		return 0;
	}

	int KWndClassScriptTable::LuaCheckBox_IsCheckBoxChecked(Lua_State* L)
	{
		int nRetCode = false;
		int nChecked = false;
		KWndCheckBox* pWnd = NULL;

		ASSERT(L);

		KG_PROCESS_ERROR(Lua_GetTopIndex(L) == 1);

		pWnd = (KWndCheckBox*)ToWndUserData("WndCheckBox", L, 1);
		KGLOG_PROCESS_ERROR(pWnd);

		nChecked = pWnd->IsCheckBoxChecked();
Exit0:
		Lua_PushBoolean(L, nChecked);
		return 1;
	}

	int KWndClassScriptTable::LuaCheckBox_Check(Lua_State* L)
	{
		int nRetCode = false;
		int nChecke = false;
		KWndCheckBox* pWnd = NULL;

		ASSERT(L);

		KG_PROCESS_ERROR(Lua_GetTopIndex(L) == 2);

		pWnd = (KWndCheckBox*)ToWndUserData("WndCheckBox", L, 1);
		KGLOG_PROCESS_ERROR(pWnd);

		if (lua_isboolean(L, 2))
			nChecke = (int)Lua_ValueToBoolean(L, 2);
		else
			nChecke = (int)Lua_ValueToNumber(L, 2);

		pWnd->Check(nChecke);
Exit0:
		return 0;
	}

	int KWndClassScriptTable::LuaCheckBox_ToggleCheck(Lua_State* L)
	{
		int nRetCode = false;
		KWndCheckBox* pWnd = NULL;

		ASSERT(L);

		KG_PROCESS_ERROR(Lua_GetTopIndex(L) == 1);

		pWnd = (KWndCheckBox*)ToWndUserData("WndCheckBox", L, 1);
		KGLOG_PROCESS_ERROR(pWnd);

		if (pWnd->IsCheckBoxChecked())
			pWnd->Check(false);
		else
			pWnd->Check(true);
Exit0:
		return 0;
	}

	int KWndClassScriptTable::LuaCheckBox_SetAnimation(Lua_State* L)
	{
		int nRetCode = false;
		int nUnCheckAndEnable = -1;
		int nCheckAndEnable = -1;
		int nUnCheckAndDisable = -1;
		int nCheckAndDisable = -1; 
		int nChecking = -1;
		int nUnChecking = -1;
		int nCheckedAndEnableWhenMouseOver = -1;
		int nUncheckedAndEnableWhenMouseOver = -1; 
		int nCheckedAndDisableWhenMouseOver = -1;
		int nUncheckedAndDisableWhenMouseOver = -1;
		LPCSTR szImagePath = NULL;
		KWndCheckBox* pWnd = NULL;

		ASSERT(L);

		KG_PROCESS_ERROR(Lua_GetTopIndex(L) == 12);

		pWnd = (KWndCheckBox*)ToWndUserData("WndCheckBox", L, 1);
		KGLOG_PROCESS_ERROR(pWnd);

		szImagePath = Lua_ValueToString(L, 2);
		KGLOG_PROCESS_ERROR(szImagePath);
		szImagePath = FormatFilePath(szImagePath);

		nUnCheckAndEnable = (int)Lua_ValueToNumber(L, 3);
		nCheckAndEnable = (int)Lua_ValueToNumber(L, 4);
		nUnCheckAndDisable = (int)Lua_ValueToNumber(L, 5);
		nCheckAndDisable = (int)Lua_ValueToNumber(L, 6);
		nChecking = (int)Lua_ValueToNumber(L, 7);
		nUnChecking = (int)Lua_ValueToNumber(L, 8);
		nCheckedAndEnableWhenMouseOver = (int)Lua_ValueToNumber(L, 9);
		nUncheckedAndEnableWhenMouseOver = (int)Lua_ValueToNumber(L, 10);
		nCheckedAndDisableWhenMouseOver = (int)Lua_ValueToNumber(L, 11);
		nUncheckedAndDisableWhenMouseOver = (int)Lua_ValueToNumber(L, 12);

		pWnd->SetAnimation(nUnCheckAndEnable, nCheckAndEnable, nUnCheckAndDisable, nCheckAndDisable, 
			nChecking, nUnChecking, nCheckedAndEnableWhenMouseOver, nUncheckedAndEnableWhenMouseOver, 
			nCheckedAndDisableWhenMouseOver, nUncheckedAndDisableWhenMouseOver, szImagePath);

Exit0:
		return 0;
	}

	int KWndClassScriptTable::LuaNewScrollBar_SetScrollPos(Lua_State* L)
	{
		int nRetCode = false;
		int nPos = 0;
		KWndNewScrollBar* pWnd = NULL;

		ASSERT(L);

		KGLOG_PROCESS_ERROR(Lua_GetTopIndex(L) == 2);

		pWnd = (KWndNewScrollBar*)ToWndUserData("WndNewScrollBar", L, 1);
		KGLOG_PROCESS_ERROR(pWnd);

		nPos = (int)Lua_ValueToNumber(L, 2);

		pWnd->SetScrollPos(nPos);
Exit0:
		return 0;
	}	
	
	int KWndClassScriptTable::LuaNewScrollBar_GetScrollPos(Lua_State* L)
	{
		int nRetCode = false;
		int nPos = 0;
		KWndNewScrollBar* pWnd = NULL;

		ASSERT(L);

		KGLOG_PROCESS_ERROR(Lua_GetTopIndex(L) == 1);

		pWnd = (KWndNewScrollBar*)ToWndUserData("WndNewScrollBar", L, 1);
		KGLOG_PROCESS_ERROR(pWnd);

		ASSERT(pWnd);

		nPos = pWnd->GetScrollPos();
Exit0:
		Lua_PushNumber(L, nPos);
		return 1;
	}	
	
	int KWndClassScriptTable::LuaNewScrollBar_SetStepCount(Lua_State* L)
	{
		int nRetCode = false;
		int nStepCount = 0;
		KWndNewScrollBar* pWnd = NULL;

		ASSERT(L);

		KGLOG_PROCESS_ERROR(Lua_GetTopIndex(L) == 2);

		pWnd = (KWndNewScrollBar*)ToWndUserData("WndNewScrollBar", L, 1);
		KGLOG_PROCESS_ERROR(pWnd);

		nStepCount = (int)ceil(Lua_ValueToNumber(L, 2));

		ASSERT(pWnd);

		pWnd->SetStepCount(nStepCount);
Exit0:
		return 0;
	}	
	
	int KWndClassScriptTable::LuaNewScrollBar_GetStepCount(Lua_State* L)
	{
		int nRetCode = false;
		int nStepCount = 0;
		KWndNewScrollBar* pWnd = NULL;

		ASSERT(L);

		KGLOG_PROCESS_ERROR(Lua_GetTopIndex(L) == 1);

		pWnd = (KWndNewScrollBar*)ToWndUserData("WndNewScrollBar", L, 1);
		KGLOG_PROCESS_ERROR(pWnd);

		ASSERT(pWnd);

		nStepCount = pWnd->GetStepCount();
Exit0:
		Lua_PushNumber(L, nStepCount);
		return 1;
	}
	
	int KWndClassScriptTable::LuaNewScrollBar_SetPageStepCount(Lua_State* L)
	{
		int nRetCode = false;
		int nStepCount = 0;
		KWndNewScrollBar* pWnd = NULL;

		ASSERT(L);

		KGLOG_PROCESS_ERROR(Lua_GetTopIndex(L) == 2);

		pWnd = (KWndNewScrollBar*)ToWndUserData("WndNewScrollBar", L, 1);
		KGLOG_PROCESS_ERROR(pWnd);

		ASSERT(pWnd);

		nStepCount = (int)Lua_ValueToNumber(L, 2);

		pWnd->SetPageStepCount(nStepCount);
Exit0:
		return 0;
	}	
	
	int KWndClassScriptTable::LuaNewScrollBar_GetPageStepCount(Lua_State* L)
	{
		int nRetCode = false;
		int nStepCount = 0;
		KWndNewScrollBar* pWnd = NULL;

		ASSERT(L);

		KGLOG_PROCESS_ERROR(Lua_GetTopIndex(L) == 1);

		pWnd = (KWndNewScrollBar*)ToWndUserData("WndNewScrollBar", L, 1);
		KGLOG_PROCESS_ERROR(pWnd);

		ASSERT(pWnd);

		nStepCount = pWnd->GetPageStepCount();
Exit0:
		Lua_PushNumber(L, nStepCount);
		return 1;
	}
	
	int KWndClassScriptTable::LuaNewScrollBar_ScrollPrev(Lua_State* L)
	{
		int nRetCode = false;
		int nStepCount = 1;
		int nParamCount = 0;
		KWndNewScrollBar* pWnd = NULL;

		ASSERT(L);

		nParamCount = Lua_GetTopIndex(L);
		KGLOG_PROCESS_ERROR(nParamCount >= 1);

		pWnd = (KWndNewScrollBar*)ToWndUserData("WndNewScrollBar", L, 1);
		KGLOG_PROCESS_ERROR(pWnd);

		if (nParamCount > 1)
			nStepCount = (int)Lua_ValueToNumber(L, 2);

		pWnd->ScrollPrev(nStepCount);
Exit0:
		return 0;
	}	
	
	int KWndClassScriptTable::LuaNewScrollBar_ScrollNext(Lua_State* L)
	{
		int nRetCode = false;
		int nStepCount = 1;
		int nParamCount = 0;
		KWndNewScrollBar* pWnd = NULL;

		ASSERT(L);

		nParamCount = Lua_GetTopIndex(L);
		KGLOG_PROCESS_ERROR(nParamCount >= 1);

		pWnd = (KWndNewScrollBar*)ToWndUserData("WndNewScrollBar", L, 1);
		KGLOG_PROCESS_ERROR(pWnd);

		if (nParamCount > 1)
			nStepCount = (int)Lua_ValueToNumber(L, 2);

		pWnd->ScrollNext(nStepCount);
Exit0:
		return 0;
	}
	
	int KWndClassScriptTable::LuaNewScrollBar_ScrollPagePrev(Lua_State* L)
	{
		int nRetCode = false;
		KWndNewScrollBar* pWnd = NULL;

		ASSERT(L);

		KGLOG_PROCESS_ERROR(Lua_GetTopIndex(L) == 1);

		pWnd = (KWndNewScrollBar*)ToWndUserData("WndNewScrollBar", L, 1);
		KGLOG_PROCESS_ERROR(pWnd);

		pWnd->ScrollPagePrev();
Exit0:
		return 0;
	}
	
	int KWndClassScriptTable::LuaNewScrollBar_ScrollPageNext(Lua_State* L)
	{
		int nRetCode = false;
		KWndNewScrollBar* pWnd = NULL;

		ASSERT(L);

		KGLOG_PROCESS_ERROR(Lua_GetTopIndex(L) == 1);

		pWnd = (KWndNewScrollBar*)ToWndUserData("WndNewScrollBar", L, 1);
		KGLOG_PROCESS_ERROR(pWnd);

		pWnd->ScrollPageNext();
Exit0:
		return 0;
	}
	
	int KWndClassScriptTable::LuaNewScrollBar_ScrollHome(Lua_State* L)
	{
		int nRetCode = false;
		KWndNewScrollBar* pWnd = NULL;

		ASSERT(L);

		KGLOG_PROCESS_ERROR(Lua_GetTopIndex(L) == 1);

		pWnd = (KWndNewScrollBar*)ToWndUserData("WndNewScrollBar", L, 1);
		KGLOG_PROCESS_ERROR(pWnd);

		pWnd->ScrollHome();
Exit0:
		return 0;
	}
	
	int KWndClassScriptTable::LuaNewScrollBar_ScrollEnd(Lua_State* L)
	{
		int nRetCode = false;
		KWndNewScrollBar* pWnd = NULL;

		ASSERT(L);

		KGLOG_PROCESS_ERROR(Lua_GetTopIndex(L) == 1);

		pWnd = (KWndNewScrollBar*)ToWndUserData("WndNewScrollBar", L, 1);
		KGLOG_PROCESS_ERROR(pWnd);

		pWnd->ScrollEnd();
Exit0:
		return 0;
	}							


	int KWndClassScriptTable::LuaScene_SetScene(Lua_State* L)
	{
		BOOL bRetCode = FALSE;
		KWndScene* pWnd = NULL;
		IKG3DScene** pp3DScene = NULL;
		IKG3DScene* p3DScene = NULL;

		ASSERT(L);

		bRetCode = Lua_GetTopIndex(L) == 2;
		KGLOG_PROCESS_ERROR(bRetCode);

		pWnd = (KWndScene*)ToWndUserData("WndScene", L, 1);
		KGLOG_PROCESS_ERROR(pWnd);

		pp3DScene = (IKG3DScene**)lua_touserdata(L, 2);
		if (pp3DScene)
			p3DScene = *pp3DScene;

		pWnd->SetScene(p3DScene);

		return 0;
Exit0:
		return 0;
	}


	int KWndClassScriptTable::LuaScene_EnableRenderTerrain(Lua_State* L)
	{
		BOOL bRetCode = FALSE;
		KWndScene* pWnd = NULL;
		bool bEnable = false;

		ASSERT(L);

		bRetCode = Lua_GetTopIndex(L) == 2;
		KGLOG_PROCESS_ERROR(bRetCode);

		pWnd = (KWndScene*)ToWndUserData("WndScene", L, 1);
		KGLOG_PROCESS_ERROR(pWnd);

		bEnable = lua_toboolean(L, 2) != 0;

		pWnd->EnableRenderTerrain(bEnable);

		return 0;
Exit0:
		return 0;
	}


	int KWndClassScriptTable::LuaScene_EnableRenderSkyBox(Lua_State* L)
	{
		BOOL bRetCode = FALSE;
		KWndScene* pWnd = NULL;
		bool bEnable = false;

		ASSERT(L);

		bRetCode = Lua_GetTopIndex(L) == 2;
		KGLOG_PROCESS_ERROR(bRetCode);

		pWnd = (KWndScene*)ToWndUserData("WndScene", L, 1);
		KGLOG_PROCESS_ERROR(pWnd);

		bEnable = lua_toboolean(L, 2) != 0;

		pWnd->EnableRenderSkyBox(bEnable);

		return 0;
Exit0:
		return 0;
	}


	int KWndClassScriptTable::LuaMovie_Play(Lua_State* L)
	{
		BOOL bRetCode = FALSE;
		KWndMovie* pWnd = NULL;
		LPCSTR szVideoFile = NULL;

		ASSERT(L);

		bRetCode = Lua_GetTopIndex(L) == 2;
		KGLOG_PROCESS_ERROR(bRetCode);

		pWnd = (KWndMovie*)ToWndUserData("WndMovie", L, 1);
		KGLOG_PROCESS_ERROR(pWnd);

		szVideoFile = lua_tostring(L, 2);
		KGLOG_PROCESS_ERROR(szVideoFile && szVideoFile[0] != '\0');

		pWnd->Play(szVideoFile);

		return 0;
Exit0:
		return 0;
	}


	int KWndClassScriptTable::LuaMovie_Stop(Lua_State* L)
	{
		BOOL bRetCode = FALSE;
		KWndMovie* pWnd = NULL;
		LPCSTR szVideoFile = NULL;

		ASSERT(L);

		bRetCode = Lua_GetTopIndex(L) == 1;
		KGLOG_PROCESS_ERROR(bRetCode);

		pWnd = (KWndMovie*)ToWndUserData("WndMovie", L, 1);
		KGLOG_PROCESS_ERROR(pWnd);

		pWnd->Stop();

		return 0;
Exit0:
		return 0;
	}


	int KWndClassScriptTable::LuaMovie_SetVolume(Lua_State* L)
	{
		BOOL bRetCode = FALSE;
		KWndMovie* pWnd = NULL;
		unsigned int nVolume = 0;

		ASSERT(L);

		bRetCode = Lua_GetTopIndex(L) == 2;
		KGLOG_PROCESS_ERROR(bRetCode);

		pWnd = (KWndMovie*)ToWndUserData("WndMovie", L, 1);
		KGLOG_PROCESS_ERROR(pWnd);

		nVolume = (unsigned int)lua_tonumber(L, 2);

		pWnd->SetVolume(nVolume);

		return 0;
Exit0:
		return 0;
	}


	int KWndClassScriptTable::LuaMovie_IsFinished(Lua_State* L)
	{
		BOOL bRetCode = FALSE;
		KWndMovie* pWnd = NULL;
		bool bFinished = false;

		ASSERT(L);

		bRetCode = Lua_GetTopIndex(L) == 1;
		KGLOG_PROCESS_ERROR(bRetCode);

		pWnd = (KWndMovie*)ToWndUserData("WndMovie", L, 1);
		KGLOG_PROCESS_ERROR(pWnd);

		bFinished = pWnd->IsFinished();

Exit0:
		Lua_PushBoolean(L, bFinished);
		return 1;
	}


	int KWndClassScriptTable::LuaTexture_SetTexture(Lua_State* L)
	{
		BOOL bRetCode = FALSE;
		KWndTexture* pWnd = NULL;
		IKG3DModel** pp3DModel = NULL;
		IKG3DModel* p3DModel = NULL;

		ASSERT(L);

		bRetCode = Lua_GetTopIndex(L) == 2;
		KGLOG_PROCESS_ERROR(bRetCode);

		pWnd = (KWndTexture*)ToWndUserData("WndTexture", L, 1);
		KGLOG_PROCESS_ERROR(pWnd);

		pp3DModel = (IKG3DModel**)lua_touserdata(L, 2);
		KGLOG_PROCESS_ERROR(pp3DModel);

		p3DModel = *pp3DModel;
		KGLOG_PROCESS_ERROR(p3DModel);

		pWnd->SetTexture(p3DModel);
Exit0:
		return 0;
	}


	int KWndClassScriptTable::LuaPageSet_ActivePage(Lua_State* L)
	{
		int nRetCode = false;
		KWndPageSet* pWnd = NULL;

		ASSERT(L);
		KGLOG_PROCESS_ERROR(Lua_GetTopIndex(L) == 2);

		pWnd = (KWndPageSet*)ToWndUserData("WndPageSet", L, 1);
		KGLOG_PROCESS_ERROR(pWnd);

		if (lua_type(L, 2) == LUA_TNUMBER)
		{
			int nPageIndex = (int)Lua_ValueToNumber(L, 2);
			pWnd->ActivePage(nPageIndex);
		}
		else
		{
			LPCSTR pcszWndName = Lua_ValueToString(L, 2);
			KWndWindow *pChild = NULL;
			KWndCheckBox *pCheckBox = NULL;
			KWndWindow* pPage = NULL;
			KG_PROCESS_ERROR(pcszWndName);
			pChild = g_pUI->m_WndStation.SearchChildWindowByPath(pWnd, pcszWndName);
			KG_PROCESS_ERROR(pChild);
			pCheckBox = (KWndCheckBox*)CastWndWindow(pChild, "WndCheckBox");
			if (pCheckBox)
				pWnd->ActivePage(pCheckBox);
			else
			{
				pPage = (KWndWindow*)CastWndWindow(pChild, "WndWindow");
				if (pPage)
				{
					pWnd->ActivePage(pPage);
				}
			}
		}
		
Exit0:
		return 0;
	}

	int KWndClassScriptTable::LuaPageSet_GetActivePageIndex(Lua_State* L)
	{
		int nRetCode = false;
		KWndPageSet* pWnd = NULL;
		int nPageIndex = -1;

		ASSERT(L);
		KGLOG_PROCESS_ERROR(Lua_GetTopIndex(L) == 1);

		pWnd = (KWndPageSet*)ToWndUserData("WndPageSet", L, 1);
		KGLOG_PROCESS_ERROR(pWnd);

		nPageIndex = pWnd->GetActivePageIndex();
Exit0:
		Lua_PushNumber(L, nPageIndex);
		return 1;
	}

	int KWndClassScriptTable::LuaPageSet_GetLastActivePageIndex(Lua_State* L)
	{
		int nRetCode = false;
		KWndPageSet* pWnd = NULL;
		int nPageIndex = -1;

		ASSERT(L);
		KGLOG_PROCESS_ERROR(Lua_GetTopIndex(L) == 1);

		pWnd = (KWndPageSet*)ToWndUserData("WndPageSet", L, 1);
		KGLOG_PROCESS_ERROR(pWnd);

		nPageIndex = pWnd->GetLastActivePageIndex();
Exit0:
		Lua_PushNumber(L, nPageIndex);
		return 1;
	}

	int KWndClassScriptTable::LuaPageSet_AddPage(Lua_State* L)
	{
		int nRetCode = false;
		KWndPageSet* pWnd = NULL;
		int nPageIndex = -1;
		int nParamCount = 0;
		KWndCheckBox* pCheckBox = NULL;
		LPCSTR pcszCheckBox = NULL;
		LPCSTR pcszNewCheckBoxName = NULL;
		KWndWindow* pPage = NULL;
		LPCSTR pcszPage = NULL;
		LPCSTR pcszNewPageName = NULL;

		ASSERT(L);

		nParamCount = Lua_GetTopIndex(L);
		KGLOG_PROCESS_ERROR(nParamCount > 2);

		pWnd = (KWndPageSet*)ToWndUserData("WndPageSet", L, 1);
		KGLOG_PROCESS_ERROR(pWnd);

		if (Lua_IsTable(L, 2))
		{
			pPage = (KWndWindow*)ToWndUserData("WndWindow", L, 2);
			KGLOG_PROCESS_ERROR(pPage);
		}
		else
		{
			pcszPage = Lua_ValueToString(L, 2);
			KGLOG_PROCESS_ERROR(pcszPage);

			pPage = (KWndWindow*)CastWndWindow(g_pUI->m_WndStation.SearchWindow(pcszPage), "WndPage");
			KGLOG_PROCESS_ERROR(pPage);
		}

		if (Lua_IsTable(L, 3))
		{
			pCheckBox = (KWndCheckBox*)ToWndUserData("WndCheckBox", L, 3);
			KGLOG_PROCESS_ERROR(pCheckBox);
		}
		else
		{
			pcszCheckBox = Lua_ValueToString(L, 3);
			KGLOG_PROCESS_ERROR(pcszCheckBox);

			pCheckBox = (KWndCheckBox*)CastWndWindow(g_pUI->m_WndStation.SearchWindow(pcszCheckBox), "WndCheckBox");
			KGLOG_PROCESS_ERROR(pCheckBox);
		}

		if (nParamCount > 3)
		{
			pcszNewPageName = Lua_ValueToString(L, 4);
			if (pcszNewPageName && pcszNewPageName[0] != '\0')
				pPage->SetName(pcszNewPageName);
		}
		if (nParamCount > 4)
		{
			pcszNewCheckBoxName = Lua_ValueToString(L, 5);
			if (pcszNewCheckBoxName && pcszNewCheckBoxName[0] != '\0')
				pCheckBox->SetName(pcszNewCheckBoxName);
		}

		pWnd->AddPage(pPage, pCheckBox);
		return 0;
Exit0:
		KGLogPrintf(KGLOG_ERR, "KGUI KWndClassScriptTable::LuaPageSet_AddPage(%s, %s, %s, %s)\n", 
			pcszCheckBox, pcszNewCheckBoxName, pcszPage, pcszNewPageName);
		return 0;
	}

	int KWndClassScriptTable::LuaPageSet_GetActivePage(Lua_State* L)
	{
		int nResult = 0;
		int nRetCode = false;
		KWndPageSet* pWnd = NULL;
		KWndWindow* pPage = NULL;

		ASSERT(L);
		KGLOG_PROCESS_ERROR(Lua_GetTopIndex(L) == 1);

		pWnd = (KWndPageSet*)ToWndUserData("WndPageSet", L, 1);
		KGLOG_PROCESS_ERROR(pWnd);

		pPage = pWnd->GetActivePage();
		KG_PROCESS_ERROR(pPage);

		nRetCode = HostStation_Lookup(L, pPage);
		KG_PROCESS_ERROR(nRetCode);

		nResult = 1;
Exit0:
		return nResult;
	}

	int KWndClassScriptTable::LuaPageSet_GetActiveCheckBox(Lua_State* L)
	{
		int nResult = 0;
		int nRetCode = false;
		KWndPageSet* pWnd = NULL;
		KWndCheckBox *pCheckBox = NULL;

		ASSERT(L);
		KGLOG_PROCESS_ERROR(Lua_GetTopIndex(L) == 1);

		pWnd = (KWndPageSet*)ToWndUserData("WndPageSet", L, 1);
		KGLOG_PROCESS_ERROR(pWnd);

		pCheckBox = pWnd->GetActiveCheckBox();
		KG_PROCESS_ERROR(pCheckBox);

		nRetCode = HostStation_Lookup(L, pCheckBox);
		KG_PROCESS_ERROR(nRetCode);

		nResult = 1;
Exit0:
		return nResult;
	}


	
	int KWndClassScriptTable::LuaFrame_EnableDrag(Lua_State* L)
	{
		int nRetCode = false;
		KWndFrame* pWnd = NULL;
		int nDragable = false;

		ASSERT(L);

		KGLOG_PROCESS_ERROR(Lua_GetTopIndex(L) == 2);

		pWnd = (KWndFrame*)ToWndUserData("WndFrame", L, 1);
		KGLOG_PROCESS_ERROR(pWnd);

		if (Lua_IsBoolean(L, 2))
			nDragable = (int)Lua_ValueToBoolean(L, 2);
		else
			nDragable = (int)Lua_ValueToNumber(L, 2);

		pWnd->EnableDrag(nDragable);
Exit0:
		return 0;	
	}
	   
	int KWndClassScriptTable::LuaFrame_IsDragable(Lua_State* L)
	{
		int nRetCode = false;
		KWndFrame* pWnd = NULL;
		int nDragable = false;

		ASSERT(L);

		KGLOG_PROCESS_ERROR(Lua_GetTopIndex(L) == 1);

		pWnd = (KWndFrame*)ToWndUserData("WndFrame", L, 1);
		KGLOG_PROCESS_ERROR(pWnd);

		nDragable = pWnd->IsDragable();

Exit0:
		Lua_PushBoolean(L, nDragable);
		return 1;
	}    
	
	int KWndClassScriptTable::LuaFrame_EnableFollowMouse(Lua_State* L)
	{
		int nRetCode = false;
		KWndFrame* pWnd = NULL;
		int nEnable = false;

		ASSERT(L);

		KGLOG_PROCESS_ERROR(Lua_GetTopIndex(L) == 2);

		pWnd = (KWndFrame*)ToWndUserData("WndFrame", L, 1);
		KGLOG_PROCESS_ERROR(pWnd);

		if (Lua_IsBoolean(L, 2))
			nEnable = (int)Lua_ValueToBoolean(L, 2);
		else
			nEnable = (int)Lua_ValueToNumber(L, 2);

		pWnd->EnableFollowMouseMove(nEnable);
Exit0:
		return 0;
	} 	        
   
	int KWndClassScriptTable::LuaFrame_IsFollowMouseMove(Lua_State* L)
	{
		int nRetCode = false;
		KWndFrame* pWnd = NULL;
		int nEnable = false;

		ASSERT(L);

		KGLOG_PROCESS_ERROR(Lua_GetTopIndex(L) == 1);

		pWnd = (KWndFrame*)ToWndUserData("WndFrame", L, 1);
		KGLOG_PROCESS_ERROR(pWnd);

		nEnable = pWnd->IsFollowMouseMove();
Exit0:
		Lua_PushBoolean(L, nEnable);
		return 1;
	} 	        
   

	int KWndClassScriptTable::LuaFrame_SetDragArea(Lua_State* L)
	{
		int nRetCode = false;
		KWndFrame* pWnd = NULL;
		float fX, fY, fW, fH;
		float fScale = g_pUI->m_WndStation.GetUIScale();
		ASSERT(L);

		KGLOG_PROCESS_ERROR(Lua_GetTopIndex(L) == 5);

		pWnd = (KWndFrame*)ToWndUserData("WndFrame", L, 1);
		KGLOG_PROCESS_ERROR(pWnd);

		fX = (float)Lua_ValueToNumber(L, 2);
		fY = (float)Lua_ValueToNumber(L, 3);
		fW = (float)Lua_ValueToNumber(L, 4);
		fH = (float)Lua_ValueToNumber(L, 5);

		fX *= fScale;
		fY *= fScale;
		fW *= fScale;
		fH *= fScale;

		pWnd->SetDragArea(fX, fY, fW, fH);

Exit0:
		return 0;
	}

	int KWndClassScriptTable::LuaFrame_RegisterEvent(Lua_State* L)
	{
		int nRetCode = false;
        int nParamCount = 0;
		KWndFrame* pWnd = NULL;
		LPCSTR szEvent = NULL;
        DWORD dwCookie = 0;

		ASSERT(L);

        nParamCount = lua_gettop(L);
		KGLOG_PROCESS_ERROR(nParamCount == 2 || nParamCount == 3);

		pWnd = (KWndFrame*)ToWndUserData("WndFrame", L, 1);
		KGLOG_PROCESS_ERROR(pWnd);

		szEvent = lua_tostring(L, 2);
		KGLOG_PROCESS_ERROR(szEvent);

        if (nParamCount == 3)
            dwCookie = (DWORD)lua_tonumber(L, 3);

		g_pUI->m_Script.SubscribeEvent(pWnd, szEvent, dwCookie);
Exit0:
		return 0;
	}

	int KWndClassScriptTable::LuaFrame_GetSelf(Lua_State* L)
	{
		int nResult = 0;
		int nRetCode = false;
		int nRef = LUA_NOREF;
		KWndFrame* pWnd = NULL;

		ASSERT(L);

		KGLOG_PROCESS_ERROR(Lua_GetTopIndex(L) == 1);

		pWnd = (KWndFrame*)ToWndUserData("WndFrame", L, 1);
		KGLOG_PROCESS_ERROR(pWnd);

		nRef = pWnd->GetDefaultTableRef();
		KG_PROCESS_ERROR(nRef != LUA_NOREF);

		lua_rawgeti(L, LUA_REGISTRYINDEX, nRef);
		nResult = 1;
Exit0:
		return nResult;
	}

	int KWndClassScriptTable::LuaFrame_FadeIn(Lua_State* L)
	{
		int nRetCode = false;
		int nCount = 0;
		KWndFrame* pWnd = NULL;

		ASSERT(L);

		KGLOG_PROCESS_ERROR(Lua_GetTopIndex(L) == 2);

		pWnd = (KWndFrame*)ToWndUserData("WndFrame", L, 1);
		KGLOG_PROCESS_ERROR(pWnd);

		nCount = (int)Lua_ValueToNumber(L, 2);

		pWnd->FadeIn(nCount);

Exit0:
		return 0;
	}

	int KWndClassScriptTable::LuaFrame_FadeOut(Lua_State* L)
	{
		int nRetCode = false;
		int nCount = 0;
		KWndFrame* pWnd = NULL;

		ASSERT(L);

		KGLOG_PROCESS_ERROR(Lua_GetTopIndex(L) == 2);

		pWnd = (KWndFrame*)ToWndUserData("WndFrame", L, 1);
		KGLOG_PROCESS_ERROR(pWnd);

		nCount = (int)Lua_ValueToNumber(L, 2);

		pWnd->FadeOut(nCount);

Exit0:
		return 0;

	}

	int KWndClassScriptTable::LuaFrame_ShowWhenUIHide(Lua_State* L)
	{
		int nRetCode = false;
		KWndFrame* pWnd = NULL;

		ASSERT(L);

		KGLOG_PROCESS_ERROR(Lua_GetTopIndex(L) == 1);

		pWnd = (KWndFrame*)ToWndUserData("WndFrame", L, 1);
		KGLOG_PROCESS_ERROR(pWnd);

		pWnd->ShowWhenUIHide();

Exit0:
		return 0;
	}

	int KWndClassScriptTable::LuaFrame_HideWhenUIHide(Lua_State* L)
	{
		int nRetCode = false;
		KWndFrame* pWnd = NULL;

		ASSERT(L);

		KGLOG_PROCESS_ERROR(Lua_GetTopIndex(L) == 1);

		pWnd = (KWndFrame*)ToWndUserData("WndFrame", L, 1);
		KGLOG_PROCESS_ERROR(pWnd);

		pWnd->HideWhenUIHide();

Exit0:
		return 0;
	}

	int KWndClassScriptTable::LuaFrame_IsVisibleWhenUIHide(Lua_State* L)
	{
		int nRetCode = false;
		int nVisible = false;
		KWndFrame* pWnd = NULL;

		ASSERT(L);

		KGLOG_PROCESS_ERROR(Lua_GetTopIndex(L) == 1);

		pWnd = (KWndFrame*)ToWndUserData("WndFrame", L, 1);
		KGLOG_PROCESS_ERROR(pWnd);

		nVisible = pWnd->IsVisibleWhenUIHide();

Exit0:
		lua_pushboolean(L, nVisible);
		return 1;
	}



	int KWndClassScriptTable::LuaFrame_FocusPrev(Lua_State* L)
	{
		BOOL bRetCode = FALSE;
		BOOL bResult = FALSE;
		KWndFrame* pWnd = NULL;

		ASSERT(L);

		bRetCode = lua_gettop(L) == 1;
		KGLOG_PROCESS_ERROR(bRetCode);

		pWnd = (KWndFrame*)ToWndUserData("WndFrame", L, 1);
		KGLOG_PROCESS_ERROR(pWnd);

		bResult = pWnd->FocusPrev();

Exit0:
		lua_pushboolean(L, bResult);
		return 1;
	}

	int KWndClassScriptTable::LuaFrame_FocusNext(Lua_State* L)
	{
		BOOL bRetCode = FALSE;
		BOOL bResult = FALSE;
		KWndFrame* pWnd = NULL;

		ASSERT(L);

		bRetCode = lua_gettop(L) == 1;
		KGLOG_PROCESS_ERROR(bRetCode);

		pWnd = (KWndFrame*)ToWndUserData("WndFrame", L, 1);
		KGLOG_PROCESS_ERROR(pWnd);

		bResult = pWnd->FocusNext();

Exit0:
		lua_pushboolean(L, bResult);
		return 1;
	}

	int KWndClassScriptTable::LuaFrame_FocusHome(Lua_State* L)
	{
		BOOL bRetCode = FALSE;
		BOOL bResult = FALSE;
		KWndFrame* pWnd = NULL;

		ASSERT(L);

		bRetCode = lua_gettop(L) == 1;
		KGLOG_PROCESS_ERROR(bRetCode);

		pWnd = (KWndFrame*)ToWndUserData("WndFrame", L, 1);
		KGLOG_PROCESS_ERROR(pWnd);

		bResult = pWnd->FocusHome();

Exit0:
		lua_pushboolean(L, bResult);
		return 1;
	}

	int KWndClassScriptTable::LuaFrame_FocusEnd(Lua_State* L)
	{
		BOOL bRetCode = FALSE;
		BOOL bResult = FALSE;
		KWndFrame* pWnd = NULL;

		ASSERT(L);

		bRetCode = lua_gettop(L) == 1;
		KGLOG_PROCESS_ERROR(bRetCode);

		pWnd = (KWndFrame*)ToWndUserData("WndFrame", L, 1);
		KGLOG_PROCESS_ERROR(pWnd);

		bResult = pWnd->FocusEnd();

Exit0:
		lua_pushboolean(L, bResult);
		return 1;
	}

	int KWndClassScriptTable::LuaMinimap_SetMapPath(Lua_State* L)
	{
		int nRetCode = false;
		KWndMinimap* pWnd = NULL;
		LPCSTR pcszPath = NULL;

		ASSERT(L);

		KGLOG_PROCESS_ERROR(Lua_GetTopIndex(L) == 2);

		pWnd = (KWndMinimap*)ToWndUserData("WndMinimap", L, 1);
		KGLOG_PROCESS_ERROR(pWnd);

		pcszPath = Lua_ValueToString(L, 2);
		pcszPath = FormatFilePath(pcszPath);
		KG_PROCESS_ERROR(pcszPath);

		pWnd->SetMapPath(pcszPath);

Exit0:
		return 0;
	}

	int KWndClassScriptTable::LuaMinimap_SetScale(Lua_State* L)
	{
		int nRetCode = false;
		KWndMinimap* pWnd = NULL;
		float fScale = 1.0f;
		ASSERT(L);

		KGLOG_PROCESS_ERROR(Lua_GetTopIndex(L) == 2);

		pWnd = (KWndMinimap*)ToWndUserData("WndMinimap", L, 1);
		KGLOG_PROCESS_ERROR(pWnd);

		fScale = (float)Lua_ValueToNumber(L, 2);

		pWnd->SetScale(fScale);

Exit0:
		return 0;
	}

	int KWndClassScriptTable::LuaMinimap_GetScale(Lua_State* L)
	{
		int nRetCode = false;
		KWndMinimap* pWnd = NULL;
		float fScale = 1.0f;
		ASSERT(L);

		KGLOG_PROCESS_ERROR(Lua_GetTopIndex(L) == 1);

		pWnd = (KWndMinimap*)ToWndUserData("WndMinimap", L, 1);
		KGLOG_PROCESS_ERROR(pWnd);

		fScale = pWnd->GetScale();

Exit0:
		Lua_PushNumber(L, fScale);
		return 1;
	}

	int KWndClassScriptTable::LuaMinimap_UpdataStaticPoint(Lua_State* L)
	{
		int nRetCode = false;
		KWndMinimap* pWnd = NULL;
		int nLeftCount = -1;
		int nParam = 0;
		ASSERT(L);
		nParam = Lua_GetTopIndex(L);
		KGLOG_PROCESS_ERROR(nParam > 5);

		pWnd = (KWndMinimap*)ToWndUserData("WndMinimap", L, 1);
		KGLOG_PROCESS_ERROR(pWnd);

		DWORD dwType = (DWORD)Lua_ValueToNumber(L, 2);
		DWORD dwID = (DWORD)Lua_ValueToNumber(L, 3);
		int nFrame = (int)Lua_ValueToNumber(L, 4);
		float fX = (float)Lua_ValueToNumber(L, 5);
		float fY = (float)Lua_ValueToNumber(L, 6);
		if (nParam > 6)
			nLeftCount = (int)Lua_ValueToNumber(L, 7);
		pWnd->UpdataStaticPoint(dwType, dwID, nFrame, fX, fY, nLeftCount);
Exit0:
		return 0;
	}

	int KWndClassScriptTable::LuaMinimap_UpdataAnimatePoint(Lua_State* L)
	{
		int nRetCode = false;
		KWndMinimap* pWnd = NULL;
		int nLeftCount = -1;
		int nParam = 0;

		ASSERT(L);

		nParam = Lua_GetTopIndex(L);
		KGLOG_PROCESS_ERROR(nParam > 5);

		pWnd = (KWndMinimap*)ToWndUserData("WndMinimap", L, 1);
		KGLOG_PROCESS_ERROR(pWnd);

		DWORD dwType = (DWORD)Lua_ValueToNumber(L, 2);
		DWORD dwID = (DWORD)Lua_ValueToNumber(L, 3);
		int nGroup = (int)Lua_ValueToNumber(L, 4);
		float fX = (float)Lua_ValueToNumber(L, 5);
		float fY = (float)Lua_ValueToNumber(L, 6);
		if (nParam > 6)
			nLeftCount = (int)Lua_ValueToNumber(L, 7);
		pWnd->UpdataAnimatePoint(dwType, dwID, nGroup, fX, fY, nLeftCount);
Exit0:
		return 0;
	}

	int KWndClassScriptTable::LuaMinimap_UpdataArrowPoint(Lua_State* L)
	{
		int nRetCode = false;
		KWndMinimap* pWnd = NULL;
		int nLeftCount = -1;
		int nParam = 0;
		ASSERT(L);
		nParam = Lua_GetTopIndex(L);
		KGLOG_PROCESS_ERROR(nParam > 6);

		pWnd = (KWndMinimap*)ToWndUserData("WndMinimap", L, 1);
		KGLOG_PROCESS_ERROR(pWnd);

		DWORD dwType = (DWORD)Lua_ValueToNumber(L, 2);
		DWORD dwID = (DWORD)Lua_ValueToNumber(L, 3);
		int nFrame = (int)Lua_ValueToNumber(L, 4);
		int nArrowFrame = (int)Lua_ValueToNumber(L, 5);
		float fX = (float)Lua_ValueToNumber(L, 6);
		float fY = (float)Lua_ValueToNumber(L, 7);
		if (nParam > 7)
			nLeftCount = (int)Lua_ValueToNumber(L, 8);
		pWnd->UpdataArrowPoint(dwType, dwID, nFrame, nArrowFrame, fX, fY, nLeftCount);
Exit0:
		return 0;
	}

	int KWndClassScriptTable::LuaMinimap_RemovePoint(Lua_State* L)
	{
		int nRetCode = false;
		KWndMinimap* pWnd = NULL;
		int nParam = 0;
		ASSERT(L);
		nParam = Lua_GetTopIndex(L);
		KGLOG_PROCESS_ERROR(nParam > 1);

		pWnd = (KWndMinimap*)ToWndUserData("WndMinimap", L, 1);
		KGLOG_PROCESS_ERROR(pWnd);

		DWORD dwType = (DWORD)Lua_ValueToNumber(L, 2);
		if (nParam > 2)
		{
			DWORD dwID = (DWORD)Lua_ValueToNumber(L, 3);
			pWnd->RemovePoint(dwType, dwID);
		}
		else
			pWnd->RemovePoint(dwType);
Exit0:
		return 0;
	}

	int KWndClassScriptTable::LuaMinimap_UpdataSelfPos(Lua_State* L)
	{
		int nRetCode = false;
		KWndMinimap* pWnd = NULL;

		ASSERT(L);

		KGLOG_PROCESS_ERROR(Lua_GetTopIndex(L) == 5);

		pWnd = (KWndMinimap*)ToWndUserData("WndMinimap", L, 1);
		KGLOG_PROCESS_ERROR(pWnd);

		DWORD nLayer = (DWORD)Lua_ValueToNumber(L, 2);
		float fX = (float)Lua_ValueToNumber(L, 3);
		float fY = (float)Lua_ValueToNumber(L, 4);
		float fDirection = (float)Lua_ValueToNumber(L, 5);

		pWnd->UpdataSelfPos(nLayer, fX, fY, fDirection);
Exit0:
		return 0;
	}

	int KWndClassScriptTable::LuaMinimap_Clear(Lua_State* L)
	{
		int nRetCode = false;
		KWndMinimap* pWnd = NULL;
		ASSERT(L);

		KGLOG_PROCESS_ERROR(Lua_GetTopIndex(L) == 1);

		pWnd = (KWndMinimap*)ToWndUserData("WndMinimap", L, 1);
		KGLOG_PROCESS_ERROR(pWnd);

		pWnd->Clear();

Exit0:
		return 0;
	}

	int KWndClassScriptTable::LuaMinimap_GetOverObj(Lua_State* L)
	{
		int nRetCode = false;
		DWORD dwType = 0;
		DWORD dwID = 0;
		KWndMinimap* pWnd = NULL;
		ASSERT(L);

		KGLOG_PROCESS_ERROR(Lua_GetTopIndex(L) == 1);

		pWnd = (KWndMinimap*)ToWndUserData("WndMinimap", L, 1);
		KGLOG_PROCESS_ERROR(pWnd);

		pWnd->GetOverObj(dwType, dwID);

Exit0:
		Lua_PushNumber(L, dwType);
		Lua_PushNumber(L, dwID);
		return 2;
	}

	int KWndClassScriptTable::LuaMinimap_GetSendPos(Lua_State *L)
	{
		int nRetCode = false;
		float fX = 0.0f;
		float fY = 0.0f;
		KWndMinimap* pWnd = NULL;
		ASSERT(L);

		KGLOG_PROCESS_ERROR(Lua_GetTopIndex(L) == 1);

		pWnd = (KWndMinimap*)ToWndUserData("WndMinimap", L, 1);
		KGLOG_PROCESS_ERROR(pWnd);

		pWnd->GetSendPos(fX, fY);

Exit0:
		Lua_PushNumber(L, fX);
		Lua_PushNumber(L, fY);
		return 2;
	}

	int KWndClassScriptTable::LuaWebPage_Navigate(Lua_State* L)
	{
		int nRetCode = false;
		KWndWebPage* pWnd = NULL;
		LPCSTR szURL = NULL;

		ASSERT(L);

		nRetCode = lua_gettop(L) == 2;
		KGLOG_PROCESS_ERROR(nRetCode);

		pWnd = (KWndWebPage*)ToWndUserData("WndWebPage", L, 1);
		KGLOG_PROCESS_ERROR(pWnd);

		szURL = (LPCSTR)lua_tostring(L, 2);
		KGLOG_PROCESS_ERROR(szURL);

		nRetCode = pWnd->Navigate(ATL::CA2W(szURL));
		KGLOG_PROCESS_ERROR(nRetCode);

Exit0:
		return 0;
	}

	int KWndClassScriptTable::LuaWebPage_CanGoBack(Lua_State* L)
	{
		BOOL bCan = FALSE;
		KWndWebPage* pWnd = NULL;
		LPCSTR szURL = NULL;

		ASSERT(L);

		KGLOG_PROCESS_ERROR(lua_gettop(L) == 1);

		pWnd = (KWndWebPage*)ToWndUserData("WndWebPage", L, 1);
		KGLOG_PROCESS_ERROR(pWnd);

		bCan = pWnd->CanGoBack();
Exit0:
		Lua_PushBoolean(L, bCan);
		return 1;
	}

	int KWndClassScriptTable::LuaWebPage_CanGoForward(Lua_State* L)
	{
		BOOL bCan = FALSE;
		KWndWebPage* pWnd = NULL;
		LPCSTR szURL = NULL;

		ASSERT(L);

		KGLOG_PROCESS_ERROR(lua_gettop(L) == 1);

		pWnd = (KWndWebPage*)ToWndUserData("WndWebPage", L, 1);
		KGLOG_PROCESS_ERROR(pWnd);

		bCan = pWnd->CanGoForward();
Exit0:
		Lua_PushBoolean(L, bCan);
		return 1;
	}
	
	int KWndClassScriptTable::LuaWebPage_GoBack(Lua_State* L)
	{
		KWndWebPage* pWnd = NULL;
		LPCSTR szURL = NULL;

		ASSERT(L);

		KGLOG_PROCESS_ERROR(lua_gettop(L) == 1);

		pWnd = (KWndWebPage*)ToWndUserData("WndWebPage", L, 1);
		KGLOG_PROCESS_ERROR(pWnd);
			
		pWnd->GoBack();
Exit0:
		return 0;
	}

	int KWndClassScriptTable::LuaWebPage_GoForward(Lua_State* L)
	{
		KWndWebPage* pWnd = NULL;
		LPCSTR szURL = NULL;

		ASSERT(L);

		KGLOG_PROCESS_ERROR(lua_gettop(L) == 1);

		pWnd = (KWndWebPage*)ToWndUserData("WndWebPage", L, 1);
		KGLOG_PROCESS_ERROR(pWnd);

		pWnd->GoForward();
Exit0:
		return 0;
	}

	int KWndClassScriptTable::LuaWebPage_Refresh(Lua_State* L)
	{
		KWndWebPage* pWnd = NULL;
		LPCSTR szURL = NULL;

		ASSERT(L);

		KGLOG_PROCESS_ERROR(lua_gettop(L) == 1);

		pWnd = (KWndWebPage*)ToWndUserData("WndWebPage", L, 1);
		KGLOG_PROCESS_ERROR(pWnd);

		pWnd->Refresh();
Exit0:
		return 0;
	}

	int KWndClassScriptTable::LuaWebPage_GetLocationName(Lua_State* L)
	{
		LPCWSTR pcwszName = NULL;
		KWndWebPage* pWnd = NULL;
		LPCSTR szURL = NULL;

		ASSERT(L);

		KGLOG_PROCESS_ERROR(lua_gettop(L) == 1);

		pWnd = (KWndWebPage*)ToWndUserData("WndWebPage", L, 1);
		KGLOG_PROCESS_ERROR(pWnd);

		pcwszName = pWnd->GetLocationName();
		KGLOG_PROCESS_ERROR(pcwszName);

Exit0:
		if (!pcwszName)
			pcwszName = L"";
		Lua_PushString(L, ATL::CW2A(pcwszName, GetCodePage()));
		return 1;

	}
	
	int KWndClassScriptTable::LuaWebPage_GetLocationURL(Lua_State* L)
	{
		LPCWSTR pcwszURL = NULL;
		KWndWebPage* pWnd = NULL;
		LPCSTR szURL = NULL;

		ASSERT(L);

		KGLOG_PROCESS_ERROR(lua_gettop(L) == 1);

		pWnd = (KWndWebPage*)ToWndUserData("WndWebPage", L, 1);
		KGLOG_PROCESS_ERROR(pWnd);

		pcwszURL = pWnd->GetLocationURL();
		KGLOG_PROCESS_ERROR(pcwszURL);

Exit0:
		if (!pcwszURL)
			pcwszURL = L"";
		Lua_PushString(L, ATL::CW2A(pcwszURL, GetCodePage()));
		return 1;
	}




}
