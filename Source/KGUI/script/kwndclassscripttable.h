#ifndef UI_SCRIPT_K_WND_CLASS_SCRIPT_TABLE_H
#define UI_SCRIPT_K_WND_CLASS_SCRIPT_TABLE_H


#define STR_OBJ_KEY_NAME "___id"


namespace UI
{
	class KWndWindow;
	class KItemNull;

	class KWndClassScriptTable
	{
	public:
		static void Load(lua_State* L);

		static int HostStation_Lookup(lua_State* L, KWndWindow *pWnd);
		static void * ToWndUserData(LPCSTR szWndType, lua_State* L, int nIndex);
	private:		
		static int LuaStation_Lookup(Lua_State* L);
		static int LuaStation_SetFocusWindow(Lua_State* L);
		static int LuaStation_GetFocusWindow(Lua_State* L);
		static int LuaStation_SetActiveFrame(Lua_State* L);
		static int LuaStation_GetActiveFrame(Lua_State* L);
		static int LuaStation_SetCapture(Lua_State* L);
		static int LuaStation_GetCapture(Lua_State* L);
		static int LuaStation_ReleaseCapture(Lua_State* L);

		//获取客户区的大小，如果有第一个参数并且第一个参数为非0，那么获取的就是没有经过UI缩放的客户区大小，
		//否则就是获取经过UI缩放的客户区大小
		static int LuaStation_GetStandardClientSize(Lua_State* L);

		//获取客户区的大小，第一个，第二个参数是传进来的大小，如果存在第三个参数并且第三个参数为非0，
		//那么传进来的就是没有经过UI缩放的大小，否则传进来的就是经过UI缩放的大小
		static int LuaStation_GetClientSize(Lua_State* L);

		//经过UI缩放的坐标转换为没有经过UI缩放的坐标
		static int LuaStation_OriginalToAdjustPos(Lua_State* L);

		static int LuaStation_AdjustToOriginalPos(Lua_State* L);

		static int LuaStation_GetWindowPosition(Lua_State* L);
		static int LuaStation_GetClientPosition(Lua_State* L);
		static int LuaStation_Paint(Lua_State* L);
		static int LuaStation_GetScreenPos(Lua_State* L);

		//对UI进行缩放操作
		static int LuaStation_SetUIScale(Lua_State* L);
		static int LuaStation_GetUIScale(Lua_State* L);
		static int LuaStation_GetMaxUIScale(Lua_State* L);
		static int LuaStation_IsFullScreen(Lua_State* L);
		static int LuaStation_IsPanauision(Lua_State* L);
		static int LuaStation_IsExclusiveMode(Lua_State* L);

		static int LuaStation_IsVisible(Lua_State* L);
		static int LuaStation_Show(Lua_State* L);
		static int LuaStation_Hide(Lua_State* L);

		static int LuaStation_ToggleWindow(Lua_State* L);
		static int LuaStation_OpenWindow(Lua_State* L);
		static int LuaStation_CloseWindow(Lua_State* L);
		static int LuaStation_KeepWndInClient(Lua_State* L);
		static int LuaStation_SetPopUpMenuPos(Lua_State* L);
		static int LuaStation_SetTipPosByRect(Lua_State* L);
		static int LuaStation_SetTipPosByWnd(Lua_State* L);
		static int LuaStation_LoadFrameList(Lua_State* L);
		static int LuaStation_UnloadFrameList(Lua_State* L);
		static int LuaStation_AdjustFrameListPosition(Lua_State* L);
		static int LuaStation_GetMessagePos(Lua_State* L);
		static int LuaStation_GetMessageWheelDelta(Lua_State* L);
		static int LuaStation_GetMessageKey(Lua_State* L);
		static int LuaStation_GetIdleTime(Lua_State* L);
		static int LuaStation_ClearIdleTime(Lua_State* L);
		static int LuaStation_IsInUserAction(Lua_State* L);
		static int LuaStation_GetMouseOverWindow(Lua_State* L);

		static int LuaCursor_ShowCursor(Lua_State* L);
		static int LuaCursor_IsVisible(Lua_State* L);
		static int LuaCursor_SwitchCursor(Lua_State* L);
		static int LuaCursor_GetCurrentCursorIndex(Lua_State* L);
		static int LuaCursor_RestoreCursor(Lua_State* L);
		static int LuaCursor_IsExistCursor(Lua_State* L);
		static int LuaCursor_LoadCursor(Lua_State* L);
		static int LuaCursor_UnloadCursor(Lua_State* L);
		static int LuaCursor_LoadAllCursor(Lua_State* L);
		static int LuaCursor_UnloadAllCursor(Lua_State* L);
		//获取鼠标位置，如果有第一个参数并且第一个参数为非0，那么获取的就是没有经过UI缩放的鼠标位置，
		//否则就是获取经过UI缩放的鼠标位置
		static int LuaCursor_GetCursorPos(Lua_State* L);
		//获取鼠标位置，第一个，第二个参数是传进来的位置，如果存在第三个参数并且第三个参数为非0，
		//那么传进来的就是没有经过UI缩放的位置，否则传进来的就是经过UI缩放的位置
		static int LuaCursor_SetCursorPos(Lua_State* L);

		static int LuaWindow_GetRelPos(Lua_State* L);
		static int LuaWindow_GetAbsPos(Lua_State* L);
		static int LuaWindow_GetSize(Lua_State* L);
        static int LuaWindow_SetSize(Lua_State* L);
		static int LuaWindow_IsVisible(Lua_State* L);
		static int LuaWindow_IsDisable(Lua_State* L);
		static int LuaWindow_SetRelPos(Lua_State* L);
		static int LuaWindow_SetAbsPos(Lua_State* L);
		static int LuaWindow_SetCursorAbove(Lua_State* L);
		static int LuaWindow_Enable(Lua_State* L);
		static int LuaWindow_Show(Lua_State* L);
		static int LuaWindow_Hide(Lua_State* L);
		static int LuaWindow_ToggleVisible(Lua_State* L);
		static int LuaWindow_BringToTop(Lua_State* L);
        static int LuaWindow_Scale(Lua_State* L);
		static int LuaWindow_CreateItemHandle(Lua_State* L);
		static int LuaWindow_ReleaseItemHandle(Lua_State* L);
		static int LuaWindow_Lookup(Lua_State* L);
		static int LuaWindow_GetName(Lua_State *L);
		static int LuaWindow_SetName(Lua_State *L);
		static int LuaWindow_GetPrev(Lua_State *L);
		static int LuaWindow_GetNext(Lua_State *L);
		static int LuaWindow_GetParent(Lua_State *L);
		static int LuaWindow_GetRoot(Lua_State *L);
		static int LuaWindow_GetFirstChild(Lua_State *L);
		static int LuaWindow_CorrectPos(Lua_State *L);
		static int LuaWindow_SetSizeWithAllChild(Lua_State *L);
		static int LuaWindow_SetMousePenetrable(Lua_State *L);
		static int LuaWindow_SetAlpha(Lua_State *L);
		static int LuaWindow_SetSelfAlpha(Lua_State *L);
		static int LuaWindow_GetAlpha(Lua_State *L);
		static int LuaWindow_GetType(Lua_State *L);
		static int LuaWindow_ChangeRelation(Lua_State *L);
		static int LuaWindow_RebuildEventArray(Lua_State *L);
		static int LuaWindow_GetTreePath(Lua_State *L);
		static int LuaWindow_SetPoint(Lua_State* L);
		static int LuaWindow_SetAreaTestFile(Lua_State* L);
		static int LuaWindow_Destroy(Lua_State* L);
		static int LuaWindow_StartMoving(Lua_State* L);
		static int LuaWindow_EndMoving(Lua_State* L);

		static int LuaWindow_IsValid(Lua_State *L);
		static int LuaWindow_NewIndex(Lua_State *L);
		static int LuaWindow_Equal(Lua_State *L);

		static int LuaEdit_SetText(Lua_State* L);
		static int LuaEdit_GetText(Lua_State* L);
		static int LuaEdit_GetTextLength(Lua_State* L);
		static int LuaEdit_ClearText(Lua_State* L);
		static int LuaEdit_InsertObj(Lua_State* L);
        static int LuaEdit_GetTextStruct(Lua_State* L);
		static int LuaEdit_SetType(Lua_State* L);
		static int LuaEdit_SelectAll(Lua_State* L);
		static int LuaEdit_CancelSelect(Lua_State* L);
		static int LuaEdit_SetFontScheme(Lua_State* L);
		static int LuaEdit_GetFontScheme(Lua_State* L);
		static int LuaEdit_SetFontColor(Lua_State* L);
		static int LuaEdit_SetLimit(Lua_State* L);
		static int LuaEdit_GetLimit(Lua_State* L);
		static int LuaEdit_SetLimitMultiByte(Lua_State* L);
		static int LuaEdit_IsLimitMultiByte(Lua_State* L);
		static int LuaEdit_InsertText(Lua_State* L);
		static int LuaEdit_Backspace(Lua_State* L);
		static int LuaEdit_SetMultiLine(Lua_State* L);
		static int LuaEdit_IsMultiLine(Lua_State* L);
		static int LuaEdit_SetFontSpacing(Lua_State* L);
		static int LuaEdit_SetRowSpacing(Lua_State* L);
		static int LuaEdit_SetFocusBgColor(Lua_State* L);
		static int LuaEdit_SetSelectBgColor(Lua_State* L);
		static int LuaEdit_SetSelectFontScheme(Lua_State* L);
		static int LuaEdit_SetCaretFontScheme(Lua_State* L);
		static int LuaEdit_SetCurSel(Lua_State* L);


		static int LuaButton_IsEnabled(Lua_State* L);
		static int LuaButton_Enable(Lua_State* L);
		static int LuaButton_SetDisableFrame(Lua_State* L);
		static int LuaButton_SetAnimateGroupNormal(Lua_State* L);
		static int LuaButton_SetAnimateGroupMouseOver(Lua_State* L);
		static int LuaButton_SetAnimateGroupMouseDown(Lua_State* L);
		static int LuaButton_SetAnimateGroupDisable(Lua_State* L);
        static int LuaButton_RegisterLButtonDrag(Lua_State* L);
        static int LuaButton_UnregisterLButtonDrag(Lua_State* L);
        static int LuaButton_IsLButtonDragable(Lua_State* L);
        static int LuaButton_RegisterRButtonDrag(Lua_State* L);
        static int LuaButton_UnregisterRButtonDrag(Lua_State* L);
        static int LuaButton_IsRButtonDragable(Lua_State* L);

		static int LuaCheckBox_IsCheckBoxActive(Lua_State* L);
		static int LuaCheckBox_Enable(Lua_State* L);
		static int LuaCheckBox_IsCheckBoxChecked(Lua_State* L);
		static int LuaCheckBox_Check(Lua_State* L);
		static int LuaCheckBox_ToggleCheck(Lua_State* L);
		static int LuaCheckBox_SetAnimation(Lua_State* L);

        static int LuaNewScrollBar_SetScrollPos(Lua_State* L);
        static int LuaNewScrollBar_GetScrollPos(Lua_State* L);
        static int LuaNewScrollBar_SetStepCount(Lua_State* L);
        static int LuaNewScrollBar_GetStepCount(Lua_State* L);
        static int LuaNewScrollBar_SetPageStepCount(Lua_State* L);
        static int LuaNewScrollBar_GetPageStepCount(Lua_State* L);
        static int LuaNewScrollBar_ScrollPrev(Lua_State* L);
        static int LuaNewScrollBar_ScrollNext(Lua_State* L);
        static int LuaNewScrollBar_ScrollPagePrev(Lua_State* L);
        static int LuaNewScrollBar_ScrollPageNext(Lua_State* L);
        static int LuaNewScrollBar_ScrollHome(Lua_State* L);
        static int LuaNewScrollBar_ScrollEnd(Lua_State* L);

		static int LuaScene_SetScene(Lua_State* L);
		static int LuaScene_EnableRenderTerrain(Lua_State* L);
		static int LuaScene_EnableRenderSkyBox(Lua_State* L);

		static int LuaMovie_Play(Lua_State* L);
		static int LuaMovie_Stop(Lua_State* L);
		static int LuaMovie_SetVolume(Lua_State* L);
		static int LuaMovie_IsFinished(Lua_State* L);

		static int LuaTexture_SetTexture(Lua_State* L);

		static int LuaPageSet_ActivePage(Lua_State* L);
		static int LuaPageSet_GetActivePageIndex(Lua_State* L);
		static int LuaPageSet_GetLastActivePageIndex(Lua_State* L);
		static int LuaPageSet_AddPage(Lua_State* L);
		static int LuaPageSet_GetActivePage(Lua_State* L);
		static int LuaPageSet_GetActiveCheckBox(Lua_State* L);

        static int LuaFrame_EnableDrag(Lua_State* L);
        static int LuaFrame_IsDragable(Lua_State* L);
		static int LuaFrame_EnableFollowMouse(Lua_State* L);
		static int LuaFrame_IsFollowMouseMove(Lua_State* L);
		static int LuaFrame_SetDragArea(Lua_State* L);
		static int LuaFrame_RegisterEvent(Lua_State* L);
		static int LuaFrame_FocusPrev(Lua_State* L);
		static int LuaFrame_FocusNext(Lua_State* L);
		static int LuaFrame_FocusHome(Lua_State* L);
		static int LuaFrame_FocusEnd(Lua_State* L);
		static int LuaFrame_GetSelf(Lua_State* L);
		static int LuaFrame_FadeIn(Lua_State* L);
		static int LuaFrame_FadeOut(Lua_State* L);
		static int LuaFrame_ShowWhenUIHide(Lua_State* L);
		static int LuaFrame_HideWhenUIHide(Lua_State* L);
		static int LuaFrame_IsVisibleWhenUIHide(Lua_State* L);

		static int LuaMinimap_SetMapPath(Lua_State* L);
		static int LuaMinimap_SetScale(Lua_State* L);
		static int LuaMinimap_GetScale(Lua_State* L);
		static int LuaMinimap_UpdataStaticPoint(Lua_State* L);
		static int LuaMinimap_UpdataAnimatePoint(Lua_State* L);
		static int LuaMinimap_UpdataArrowPoint(Lua_State* L);
		static int LuaMinimap_RemovePoint(Lua_State* L);
		static int LuaMinimap_UpdataSelfPos(Lua_State* L);
		static int LuaMinimap_Clear(Lua_State* L);
		static int LuaMinimap_GetOverObj(Lua_State* L);
		static int LuaMinimap_GetSendPos(Lua_State* L);

		static int LuaWebPage_Navigate(Lua_State* L);
		static int LuaWebPage_CanGoBack(Lua_State* L);
		static int LuaWebPage_CanGoForward(Lua_State* L);
		static int LuaWebPage_GoBack(Lua_State* L);
		static int LuaWebPage_GoForward(Lua_State* L);
		static int LuaWebPage_Refresh(Lua_State* L);
		static int LuaWebPage_GetLocationName(Lua_State* L);
		static int LuaWebPage_GetLocationURL(Lua_State* L);


	};

}

#endif // UI_SCRIPT_K_WND_CLASS_SCRIPT_TABLE_H

