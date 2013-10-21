////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : kitemhandleclassscripttable.h
//  Version     : 1.0
//  Creator     : Hu Changyin
//  Create Date : 2006-3-21 16:35:40
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _INCLUDE_KITEMHANDLECLASSSCRIPTTABLE_H_
#define _INCLUDE_KITEMHANDLECLASSSCRIPTTABLE_H_


#include "./kwndclassscripttable.h"


namespace UI
{

class KItemNull;

class KItemHandleClassScriptTable
{
public:
	static void Load(lua_State* L);

	static int HostWndItem_Lookup(lua_State* L, KItemNull *pItemNull);

private:
	template <typename TItemType>
		static int THostWndItem_GetUserData(Lua_State* L, TItemType** ppItem, int nIndex = 1);

	static int LuaItemNull_Show(Lua_State *L);
	static int LuaItemNull_Hide(Lua_State *L);
	static int LuaItemNull_PtInItem(Lua_State *L);
	static int LuaItemNull_SetRelPos(Lua_State *L);
	static int LuaItemNull_GetRelPos(Lua_State *L);
	static int LuaItemNull_SetAbsPos(Lua_State *L);
	static int LuaItemNull_GetAbsPos(Lua_State *L);
	static int LuaItemNull_SetSize(Lua_State *L);
	static int LuaItemNull_GetSize(Lua_State *L);
	static int LuaItemNull_SetPosType(Lua_State *L);
	static int LuaItemNull_GetPosType(Lua_State *L);
	static int LuaItemNull_IsVisible(Lua_State *L);
	static int LuaItemNull_SetName(Lua_State *L);
	static int LuaItemNull_GetName(Lua_State *L);
	static int LuaItemNull_SetTip(Lua_State *L);
	static int LuaItemNull_GetTip(Lua_State *L);
	static int LuaItemNull_SetUserData(Lua_State *L);
	static int LuaItemNull_GetUserData(Lua_State *L);
	static int LuaItemNull_RegisterEvent(Lua_State *L);
	static int LuaItemNull_ClearEvent(Lua_State *L);
	static int LuaItemNull_EnableScale(Lua_State *L);
	static int LuaItemNull_Scale(Lua_State *L);
	static int LuaItemNull_LockShowAndHide(Lua_State *L);
	static int LuaItemNull_SetAlpha(Lua_State *L);
	static int LuaItemNull_GetAlpha(Lua_State *L);
	static int LuaItemNull_GetParent(Lua_State *L);	
	static int LuaItemNull_GetRoot(Lua_State *L);
	static int LuaItemNull_GetType(Lua_State *L);
	static int LuaItemNull_GetIndex(Lua_State *L);
	static int LuaItemNull_SetIndex(Lua_State *L);
	static int LuaItemNull_ExchangeIndex(Lua_State *L);
	static int LuaItemNull_GetTreePath(Lua_State *L);
	static int LuaItemNull_SetAreaTestFile(Lua_State *L);
	static int LuaItemNull_SetIntPos(Lua_State *L);
	static int LuaItemNull_IsIntPos(Lua_State *L);
    static int LuaItemNull_IsLink(Lua_State *L);
    static int LuaItemNull_GetLinkInfo(Lua_State *L);

	static int LuaItemNull_IsValid(Lua_State *L);
	static int LuaItemNull_NewIndex(Lua_State *L);
	static int LuaItemNull_Equal(Lua_State *L);

	static int LuaItemText_SetFontScheme(Lua_State *L);
	static int LuaItemText_GetFontScheme(Lua_State *L);
	static int LuaItemText_SetText(Lua_State *L);
	static int LuaItemText_GetText(Lua_State *L);
	static int LuaItemText_GetTextLen(Lua_State *L);
	static int LuaItemText_SetVAlign(Lua_State *L);
	static int LuaItemText_GetVAlign(Lua_State *L);
	static int LuaItemText_SetHAlign(Lua_State *L);
	static int LuaItemText_GetHAlign(Lua_State *L);
	static int LuaItemText_SetRowSpacing(Lua_State *L);
	static int LuaItemText_GetRowSpacing(Lua_State *L);
	static int LuaItemText_SetMultiLine(Lua_State *L);
	static int LuaItemText_IsMultiLine(Lua_State *L);
	static int LuaItemText_FormatTextForDraw(Lua_State *L);
	static int LuaItemText_AutoSize(Lua_State *L);
	static int LuaItemText_SetFontScale(Lua_State *L);
	static int LuaItemText_GetFontScale(Lua_State *L);
	static int LuaItemText_SetCenterEachLine(Lua_State *L);
	static int LuaItemText_IsCenterEachLine(Lua_State *L);
	static int LuaItemText_SetFontSpacing(Lua_State *L);
	static int LuaItemText_GetFontSpacing(Lua_State *L);
	static int LuaItemText_SetRichText(Lua_State *L);
	static int LuaItemText_IsRichText(Lua_State *L);
	static int LuaItemText_SetFontID(Lua_State *L);
	static int LuaItemText_SetFontColor(Lua_State *L);
	static int LuaItemText_SetFontBoder(Lua_State *L);
	static int LuaItemText_SetFontShadow(Lua_State *L);
	static int LuaItemText_GetFontID(Lua_State *L);
	static int LuaItemText_GetFontColor(Lua_State *L);
	static int LuaItemText_GetFontBoder(Lua_State *L);
	static int LuaItemText_GetFontProjection(Lua_State *L);
	static int LuaItemText_GetTextExtent(Lua_State *L);
	static int LuaItemText_GetTextPosExtent(Lua_State *L);

	static int LuaItemImage_SetFrame(Lua_State *L);
	static int LuaItemImage_GetFrame(Lua_State *L);
	static int LuaItemImage_AutoSize(Lua_State *L);
	static int LuaItemImage_SetImageType(Lua_State *L);
	static int LuaItemImage_GetImageType(Lua_State *L);
	static int LuaItemImage_SetPercentage(Lua_State *L);
	static int LuaItemImage_GetPercentage(Lua_State *L);
	static int LuaItemImage_SetRotate(Lua_State *L);
	static int LuaItemImage_GetRotate(Lua_State *L);
	static int LuaItemImage_GetImageID(Lua_State *L);
	static int LuaItemImage_FromUITex(Lua_State *L);
	static int LuaItemImage_FromTextureFile(Lua_State *L);
	static int LuaItemImage_FromScene(Lua_State *L);
	static int LuaItemImage_FromImageID(Lua_State *L);
	static int LuaItemImage_FromIconID(Lua_State *L);
    static int LuaItemImage_ToManagedImage(Lua_State *L);

	static int LuaItemShadow_SetShadowColor(Lua_State *L);
	static int LuaItemShadow_GetShadowColor(Lua_State *L);
	static int LuaItemShadow_SetColorRGB(Lua_State *L);
	static int LuaItemShadow_GetColorRGB(Lua_State *L);
	static int LuaItemShadow_SetTriangleFan(Lua_State *L);
	static int LuaItemShadow_IsTriangleFan(Lua_State *L);
	static int LuaItemShadow_AppendTriangleFanPoint(Lua_State *L);
	static int LuaItemShadow_ClearTriangleFanPoint(Lua_State *L);


	static int LuaItemAnimate_SetGroup(Lua_State *L);
	static int LuaItemAnimate_SetLoopCount(Lua_State *L);
	static int LuaItemAnimate_SetImagePath(Lua_State *L);
	static int LuaItemAnimate_SetAnimate(Lua_State *L);
	static int LuaItemAnimate_AutoSize(Lua_State *L);
    static int LuaItemAnimate_Replay(Lua_State *L);

	static int LuaItemScene_SetScene(Lua_State *L);
	static int LuaItemScene_EnableRenderTerrain(Lua_State *L);
	static int LuaItemScene_EnableRenderSkyBox(Lua_State *L);
	static int LuaItemScene_EnableAlpha(Lua_State *L);

	static int LuaItemBox_SetIndex(Lua_State *L);
	static int LuaItemBox_GetIndex(Lua_State *L);
	static int LuaItemBox_SetObject(Lua_State *L);
	static int LuaItemBox_GetObject(Lua_State *L);
	static int LuaItemBox_GetObjectType(Lua_State *L);
	static int LuaItemBox_GetObjectData(Lua_State *L);
	static int LuaItemBox_ClearObject(Lua_State *L);
	static int LuaItemBox_IsEmpty(Lua_State *L);
	static int LuaItemBox_EnableObject(Lua_State *L);
	static int LuaItemBox_IsObjectEnable(Lua_State *L);
	static int LuaItemBox_EnableObjectEquip(Lua_State *L);
	static int LuaItemBox_IsObjectEquipable(Lua_State *L);
	static int LuaItemBox_SetObjectCoolDown(Lua_State *L);
	static int LuaItemBox_IsObjectCoolDown(Lua_State *L);
	static int LuaItemBox_SetObjectSparking(Lua_State *L);
	static int LuaItemBox_SetObjectInUse(Lua_State *L);
	static int LuaItemBox_SetObjectStaring(Lua_State *L);
	static int LuaItemBox_SetObjectSelected(Lua_State *L);
	static int LuaItemBox_IsObjectSelected(Lua_State *L);
	static int LuaItemBox_SetObjectMouseOver(Lua_State *L);
	static int LuaItemBox_IsObjectMouseOver(Lua_State *L);
	static int LuaItemBox_SetObjectPressed(Lua_State *L);
	static int LuaItemBox_IsObjectPressed(Lua_State *L);
	static int LuaItemBox_SetCoolDownPercentage(Lua_State *L);
	static int LuaItemBox_GetCoolDownPercentage(Lua_State *L);
	static int LuaItemBox_SetObjectIcon(Lua_State *L);
	static int LuaItemBox_GetObjectIcon(Lua_State *L);
	static int LuaItemBox_ClearObjectIcon(Lua_State *L);
	static int LuaItemBox_SetOverText(Lua_State *L);
	static int LuaItemBox_GetOverText(Lua_State *L);
	static int LuaItemBox_SetOverTextFontScheme(Lua_State *L);
	static int LuaItemBox_GetOverTextFontScheme(Lua_State *L);
	static int LuaItemBox_SetOverTextPosition(Lua_State *L);
	static int LuaItemBox_GetOverTextPosition(Lua_State *L);

	static int LuaItemHandle_AppendItemFromString(Lua_State *L);
	static int LuaItemHandle_AppendItemFromIni(Lua_State *L);
	static int LuaItemHandle_AdjustItemShowInfo(Lua_State *L);
	static int LuaItemHandle_InsertItemFromString(Lua_State *L);
	static int LuaItemHandle_InsertItemFromIni(Lua_State *L);
	static int LuaItemHandle_FormatAllItemPos(Lua_State *L);
	static int LuaItemHandle_SetHandleStyle(Lua_State *L);
	static int LuaItemHandle_SetMinRowHeight(Lua_State *L);
	static int LuaItemHandle_SetMaxRowHeight(Lua_State *L);
	static int LuaItemHandle_SetRowHeight(Lua_State *L);
	static int LuaItemHandle_SetRowSpacing(Lua_State *L);
	static int LuaItemHandle_RemoveItem(Lua_State *L);
	static int LuaItemHandle_Clear(Lua_State *L);
	static int LuaItemHandle_SetSizeByAllItemSize(Lua_State *L);
	static int LuaItemHandle_GetItemStartRelPos(Lua_State *L);
	static int LuaItemHandle_SetItemStartRelPos(Lua_State *L);
	static int LuaItemHandle_GetAllItemSize(Lua_State *L);
	static int LuaItemHandle_GetItemCount(Lua_State *L);
	static int LuaItemHandle_GetVisibleItemCount(Lua_State *L);
	static int LuaItemHandle_Lookup(Lua_State *L);
	static int LuaItemHandle_EnableFormatWhenAppend(Lua_State *L);
	static int LuaItemHandle_RemoveItemUntilNewLine(Lua_State *L);
	static int LuaItemHandle_ExchangeItemIndex(Lua_State *L);
	static int LuaItemHandle_Sort(Lua_State *L);
	

	static int LuaItemTreeLeaf_IsExpand(Lua_State *L);
	static int LuaItemTreeLeaf_ExpandOrCollapse(Lua_State *L);
	static int LuaItemTreeLeaf_Expand(Lua_State *L);
	static int LuaItemTreeLeaf_Collapse(Lua_State *L);
	static int LuaItemTreeLeaf_SetIndent(Lua_State *L);
	static int LuaItemTreeLeaf_GetIndent(Lua_State *L);
	static int LuaItemTreeLeaf_SetEachIndentWidth(Lua_State *L);
	static int LuaItemTreeLeaf_GetEachIndentWidth(Lua_State *L);
	static int LuaItemTreeLeaf_SetNodeIconSize(Lua_State *L);
	static int LuaItemTreeLeaf_SetIconImage(Lua_State *L);
	static int LuaItemTreeLeaf_PtInIcon(Lua_State *L);
	static int LuaItemTreeLeaf_AdjustNodeIconPos(Lua_State *L);
	static int LuaItemTreeLeaf_AutoSetIconSize(Lua_State *L);
	static int LuaItemTreeLeaf_SetShowIndex(Lua_State *L);
	static int LuaItemTreeLeaf_GetShowIndex(Lua_State *L);

};


}


#endif //_INCLUDE_KITEMHANDLECLASSSCRIPTTABLE_H_
