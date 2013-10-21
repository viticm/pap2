////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : kitemhandleclassscripttable.cpp
//  Version     : 1.0
//  Creator     : Hu Changyin
//  Create Date : 2006-3-21 16:35:44
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "./kitemhandleclassscripttable.h"
#include "./kscriptloader.h"
#include "./kscriptmgr.h"
#include "../elem/wndwindow.h"
#include "../elem/tipcenter.h"
#include "../elem/Components/item.h"
#include "../elem/Components/itemtext.h"
#include "../elem/Components/itemimage.h"
#include "../elem/Components/itemshadow.h"
#include "../elem/Components/itemscene.h"
#include "../elem/Components/itemanimate.h"
#include "../elem/Components/itemhandle.h"
#include "../elem/Components/itemtreeleaf.h"
#include "../elem/Components/itemeventmgr.h"
#include "../elem/Components/decoder.h"
#include "../KGUI.h"

namespace UI
{
void KItemHandleClassScriptTable::Load(lua_State* L)
{
	static luaL_reg const s_aItemNullMetaTable[] =
	{
		{ "Show",							LuaItemNull_Show},
		{ "Hide",							LuaItemNull_Hide},
		{ "PtInItem",						LuaItemNull_PtInItem},
		{ "SetRelPos",						LuaItemNull_SetRelPos},
		{ "GetRelPos",						LuaItemNull_GetRelPos},
		{ "SetAbsPos",						LuaItemNull_SetAbsPos},
		{ "GetAbsPos",						LuaItemNull_GetAbsPos},
		{ "SetSize",						LuaItemNull_SetSize},
		{ "GetSize",						LuaItemNull_GetSize},
		{ "SetPosType",						LuaItemNull_SetPosType},
		{ "GetPosType",						LuaItemNull_GetPosType},
		{ "IsVisible",						LuaItemNull_IsVisible},
		{ "GetName",						LuaItemNull_GetName},
		{ "SetName",						LuaItemNull_SetName},
		{ "SetTip",							LuaItemNull_SetTip},
		{ "GetTip",							LuaItemNull_GetTip},
		{ "SetUserData",					LuaItemNull_SetUserData},
		{ "GetUserData",					LuaItemNull_GetUserData},
		{ "RegisterEvent",					LuaItemNull_RegisterEvent},
		{ "ClearEvent",						LuaItemNull_ClearEvent},
		{ "EnableScale",					LuaItemNull_EnableScale},
		{ "Scale",							LuaItemNull_Scale},		
		{ "LockShowAndHide",				LuaItemNull_LockShowAndHide},
		{ "SetAlpha",						LuaItemNull_SetAlpha},
		{ "GetAlpha",						LuaItemNull_GetAlpha},
		{ "GetParent",						LuaItemNull_GetParent},
		{ "GetRoot",						LuaItemNull_GetRoot},
		{ "GetType",						LuaItemNull_GetType},
		{ "GetIndex",						LuaItemNull_GetIndex},
		{ "SetIndex",						LuaItemNull_SetIndex},
		{ "ExchangeIndex",					LuaItemNull_ExchangeIndex},
		{ "GetTreePath",					LuaItemNull_GetTreePath},
		{ "SetAreaTestFile",				LuaItemNull_SetAreaTestFile},
		{ "SetIntPos",						LuaItemNull_SetIntPos},
		{ "IsIntPos",						LuaItemNull_IsIntPos},
        { "IsLink",                         LuaItemNull_IsLink},
        { "GetLinkInfo",                    LuaItemNull_GetLinkInfo},

		{ "IsValid",						LuaItemNull_IsValid },
//		{ "__newindex",						LuaItemNull_NewIndex },
		{ "__eq",							LuaItemNull_Equal },
		
		{ NULL,								NULL },
	};

	static luaL_reg const s_aItemTextMetaTable[] =
	{
		//itemnull
		{ "Show",							LuaItemNull_Show},
		{ "Hide",							LuaItemNull_Hide},
		{ "PtInItem",						LuaItemNull_PtInItem},
		{ "SetRelPos",						LuaItemNull_SetRelPos},
		{ "GetRelPos",						LuaItemNull_GetRelPos},
		{ "SetAbsPos",						LuaItemNull_SetAbsPos},
		{ "GetAbsPos",						LuaItemNull_GetAbsPos},
		{ "SetSize",						LuaItemNull_SetSize},
		{ "GetSize",						LuaItemNull_GetSize},
		{ "SetPosType",						LuaItemNull_SetPosType},
		{ "GetPosType",						LuaItemNull_GetPosType},
		{ "IsVisible",						LuaItemNull_IsVisible},
		{ "GetName",						LuaItemNull_GetName},
		{ "SetName",						LuaItemNull_SetName},
		{ "SetTip",							LuaItemNull_SetTip},
		{ "GetTip",							LuaItemNull_GetTip},
		{ "SetUserData",					LuaItemNull_SetUserData},
		{ "GetUserData",					LuaItemNull_GetUserData},
		{ "RegisterEvent",					LuaItemNull_RegisterEvent},
		{ "ClearEvent",						LuaItemNull_ClearEvent},
		{ "EnableScale",					LuaItemNull_EnableScale},
		{ "Scale",							LuaItemNull_Scale},		
		{ "LockShowAndHide",				LuaItemNull_LockShowAndHide},
		{ "SetAlpha",						LuaItemNull_SetAlpha},
		{ "GetAlpha",						LuaItemNull_GetAlpha},
		{ "GetParent",						LuaItemNull_GetParent},
		{ "GetRoot",						LuaItemNull_GetRoot},
		{ "GetType",						LuaItemNull_GetType},
		{ "GetIndex",						LuaItemNull_GetIndex},
		{ "SetIndex",						LuaItemNull_SetIndex},
		{ "ExchangeIndex",					LuaItemNull_ExchangeIndex},
		{ "GetTreePath",					LuaItemNull_GetTreePath},
		{ "SetAreaTestFile",				LuaItemNull_SetAreaTestFile},
		{ "SetIntPos",						LuaItemNull_SetIntPos},
		{ "IsIntPos",						LuaItemNull_IsIntPos},
        { "IsLink",                         LuaItemNull_IsLink},
        { "GetLinkInfo",                    LuaItemNull_GetLinkInfo},

		{ "IsValid",						LuaItemNull_IsValid },
//		{ "__newindex",						LuaItemNull_NewIndex },
		{ "__eq",							LuaItemNull_Equal },

		//text
		{ "SetFontScheme",					LuaItemText_SetFontScheme},
		{ "GetFontScheme",					LuaItemText_GetFontScheme},
		{ "SetText",						LuaItemText_SetText},
		{ "GetText",						LuaItemText_GetText},
		{ "GetTextLen",						LuaItemText_GetTextLen},
		{ "SetVAlign",						LuaItemText_SetVAlign},
		{ "GetVAlign",						LuaItemText_GetVAlign},
		{ "SetHAlign",						LuaItemText_SetHAlign},
		{ "GetHAlign",						LuaItemText_GetHAlign},
		{ "SetRowSpacing",					LuaItemText_SetRowSpacing},
		{ "GetRowSpacing",					LuaItemText_GetRowSpacing},
		{ "SetMultiLine",					LuaItemText_SetMultiLine},
		{ "IsMultiLine",					LuaItemText_IsMultiLine},
		{ "FormatTextForDraw",				LuaItemText_FormatTextForDraw},
		{ "AutoSize",						LuaItemText_AutoSize},
		{ "SetCenterEachLine",				LuaItemText_SetCenterEachLine},
		{ "IsCenterEachLine",				LuaItemText_IsCenterEachLine},
		{ "SetFontSpacing",					LuaItemText_SetFontSpacing},
		{ "GetFontSpacing",					LuaItemText_GetFontSpacing},
		{ "SetRichText",					LuaItemText_SetRichText},
		{ "IsRichText",						LuaItemText_IsRichText},
		{ "GetFontScale",					LuaItemText_GetFontScale},
		{ "SetFontScale",					LuaItemText_SetFontScale},
		{ "SetFontID",						LuaItemText_SetFontID},
		{ "SetFontColor",					LuaItemText_SetFontColor},
		{ "SetFontBorder",					LuaItemText_SetFontBoder},
		{ "SetFontShadow",					LuaItemText_SetFontShadow},
		{ "GetFontID",						LuaItemText_GetFontID},
		{ "GetFontColor",					LuaItemText_GetFontColor},
		{ "GetFontBoder",					LuaItemText_GetFontBoder},
		{ "GetFontProjection",				LuaItemText_GetFontProjection},
		{ "GetTextExtent",					LuaItemText_GetTextExtent },
		{ "GetTextPosExtent",				LuaItemText_GetTextPosExtent },
		
		{ NULL,								NULL },
	};

	static luaL_reg const s_aItemImageMetaTable[] =
	{
		//itemnull
		{ "Show",							LuaItemNull_Show },
		{ "Hide",							LuaItemNull_Hide },
		{ "PtInItem",						LuaItemNull_PtInItem },
		{ "SetRelPos",						LuaItemNull_SetRelPos },
		{ "GetRelPos",						LuaItemNull_GetRelPos },
		{ "SetAbsPos",						LuaItemNull_SetAbsPos },
		{ "GetAbsPos",						LuaItemNull_GetAbsPos },
		{ "SetSize",						LuaItemNull_SetSize },
		{ "GetSize",						LuaItemNull_GetSize },
		{ "SetPosType",						LuaItemNull_SetPosType },
		{ "GetPosType",						LuaItemNull_GetPosType },
		{ "IsVisible",						LuaItemNull_IsVisible },
		{ "GetName",						LuaItemNull_GetName },
		{ "SetName",						LuaItemNull_SetName },
		{ "SetTip",							LuaItemNull_SetTip },
		{ "GetTip",							LuaItemNull_GetTip },
		{ "SetUserData",					LuaItemNull_SetUserData },
		{ "GetUserData",					LuaItemNull_GetUserData },
		{ "RegisterEvent",					LuaItemNull_RegisterEvent },
		{ "ClearEvent",						LuaItemNull_ClearEvent },
		{ "EnableScale",					LuaItemNull_EnableScale },
		{ "Scale",							LuaItemNull_Scale },		
		{ "LockShowAndHide",				LuaItemNull_LockShowAndHide },
		{ "SetAlpha",						LuaItemNull_SetAlpha },
		{ "GetAlpha",						LuaItemNull_GetAlpha },
		{ "GetParent",						LuaItemNull_GetParent },
		{ "GetRoot",						LuaItemNull_GetRoot },
		{ "GetType",						LuaItemNull_GetType },
		{ "GetIndex",						LuaItemNull_GetIndex },
		{ "SetIndex",						LuaItemNull_SetIndex },
		{ "ExchangeIndex",					LuaItemNull_ExchangeIndex },
		{ "GetTreePath",					LuaItemNull_GetTreePath },
		{ "SetAreaTestFile",				LuaItemNull_SetAreaTestFile },
		{ "SetIntPos",						LuaItemNull_SetIntPos},
		{ "IsIntPos",						LuaItemNull_IsIntPos},
        { "IsLink",                         LuaItemNull_IsLink},
        { "GetLinkInfo",                    LuaItemNull_GetLinkInfo},

		{ "IsValid",						LuaItemNull_IsValid },
//		{ "__newindex",						LuaItemNull_NewIndex },
		{ "__eq",							LuaItemNull_Equal },

		//image
		{ "SetFrame",						LuaItemImage_SetFrame },
		{ "GetFrame",						LuaItemImage_GetFrame },
		{ "AutoSize",						LuaItemImage_AutoSize },
		{ "SetImageType",					LuaItemImage_SetImageType },
		{ "GetImageType",					LuaItemImage_GetImageType },
		{ "SetPercentage",					LuaItemImage_SetPercentage },
		{ "GetPercentage",					LuaItemImage_GetPercentage },
		{ "SetRotate",						LuaItemImage_SetRotate },
		{ "GetRotate",						LuaItemImage_GetRotate },
		{ "GetImageID",						LuaItemImage_GetImageID },
		{ "FromUITex",						LuaItemImage_FromUITex },
		{ "FromTextureFile",				LuaItemImage_FromTextureFile },
		{ "FromScene",						LuaItemImage_FromScene},
		{ "FromImageID",					LuaItemImage_FromImageID },
		{ "FromIconID",						LuaItemImage_FromIconID },
		{ "ToManagedImage",					LuaItemImage_ToManagedImage },

		{ NULL,								NULL },
	};

	static luaL_reg const s_aItemShadowMetaTable[] =
	{
		//itemnull
		{ "Show",							LuaItemNull_Show},
		{ "Hide",							LuaItemNull_Hide},
		{ "PtInItem",						LuaItemNull_PtInItem},
		{ "SetRelPos",						LuaItemNull_SetRelPos},
		{ "GetRelPos",						LuaItemNull_GetRelPos},
		{ "SetAbsPos",						LuaItemNull_SetAbsPos},
		{ "GetAbsPos",						LuaItemNull_GetAbsPos},
		{ "SetSize",						LuaItemNull_SetSize},
		{ "GetSize",						LuaItemNull_GetSize},
		{ "SetPosType",						LuaItemNull_SetPosType},
		{ "GetPosType",						LuaItemNull_GetPosType},
		{ "IsVisible",						LuaItemNull_IsVisible},
		{ "GetName",						LuaItemNull_GetName},
		{ "SetName",						LuaItemNull_SetName},
		{ "SetTip",							LuaItemNull_SetTip},
		{ "GetTip",							LuaItemNull_GetTip},
		{ "SetUserData",					LuaItemNull_SetUserData},
		{ "GetUserData",					LuaItemNull_GetUserData},
		{ "RegisterEvent",					LuaItemNull_RegisterEvent},
		{ "ClearEvent",						LuaItemNull_ClearEvent},
		{ "EnableScale",					LuaItemNull_EnableScale},
		{ "Scale",							LuaItemNull_Scale},		
		{ "LockShowAndHide",				LuaItemNull_LockShowAndHide},
		{ "SetAlpha",						LuaItemNull_SetAlpha},
		{ "GetAlpha",						LuaItemNull_GetAlpha},
		{ "GetParent",						LuaItemNull_GetParent},
		{ "GetRoot",						LuaItemNull_GetRoot},
		{ "GetType",						LuaItemNull_GetType},
		{ "GetIndex",						LuaItemNull_GetIndex},
		{ "SetIndex",						LuaItemNull_SetIndex},
		{ "ExchangeIndex",					LuaItemNull_ExchangeIndex},
		{ "GetTreePath",					LuaItemNull_GetTreePath},
		{ "SetAreaTestFile",				LuaItemNull_SetAreaTestFile},
		{ "SetIntPos",						LuaItemNull_SetIntPos},
		{ "IsIntPos",						LuaItemNull_IsIntPos},
        { "IsLink",                         LuaItemNull_IsLink},
        { "GetLinkInfo",                    LuaItemNull_GetLinkInfo},

		{ "IsValid",						LuaItemNull_IsValid },
//		{ "__newindex",						LuaItemNull_NewIndex },
		{ "__eq",							LuaItemNull_Equal },

		//shadow
		{ "SetShadowColor",					LuaItemShadow_SetShadowColor},
		{ "GetShadowColor",					LuaItemShadow_GetShadowColor},
		{ "GetColorRGB",					LuaItemShadow_GetColorRGB},
		{ "SetColorRGB",					LuaItemShadow_SetColorRGB},
		{ "SetTriangleFan",					LuaItemShadow_SetTriangleFan},
		{ "IsTriangleFan",					LuaItemShadow_IsTriangleFan},
		{ "AppendTriangleFanPoint",			LuaItemShadow_AppendTriangleFanPoint},
		{ "ClearTriangleFanPoint",			LuaItemShadow_ClearTriangleFanPoint},
		
		{ NULL,								NULL },
	};

	static luaL_reg const s_aItemAnimateMetaTable[] =
	{
		//itemnull
		{ "Show",							LuaItemNull_Show},
		{ "Hide",							LuaItemNull_Hide},
		{ "PtInItem",						LuaItemNull_PtInItem},
		{ "SetRelPos",						LuaItemNull_SetRelPos},
		{ "GetRelPos",						LuaItemNull_GetRelPos},
		{ "SetAbsPos",						LuaItemNull_SetAbsPos},
		{ "GetAbsPos",						LuaItemNull_GetAbsPos},
		{ "SetSize",						LuaItemNull_SetSize},
		{ "GetSize",						LuaItemNull_GetSize},
		{ "SetPosType",						LuaItemNull_SetPosType},
		{ "GetPosType",						LuaItemNull_GetPosType},
		{ "IsVisible",						LuaItemNull_IsVisible},
		{ "GetName",						LuaItemNull_GetName},
		{ "SetName",						LuaItemNull_SetName},
		{ "SetTip",							LuaItemNull_SetTip},
		{ "GetTip",							LuaItemNull_GetTip},
		{ "SetUserData",					LuaItemNull_SetUserData},
		{ "GetUserData",					LuaItemNull_GetUserData},
		{ "RegisterEvent",					LuaItemNull_RegisterEvent},
		{ "ClearEvent",						LuaItemNull_ClearEvent},
		{ "EnableScale",					LuaItemNull_EnableScale},
		{ "Scale",							LuaItemNull_Scale},		
		{ "LockShowAndHide",				LuaItemNull_LockShowAndHide},
		{ "SetAlpha",						LuaItemNull_SetAlpha},
		{ "GetAlpha",						LuaItemNull_GetAlpha},
		{ "GetParent",						LuaItemNull_GetParent},
		{ "GetRoot",						LuaItemNull_GetRoot},
		{ "GetType",						LuaItemNull_GetType},
		{ "GetIndex",						LuaItemNull_GetIndex},
		{ "SetIndex",						LuaItemNull_SetIndex},
		{ "ExchangeIndex",					LuaItemNull_ExchangeIndex},
		{ "GetTreePath",					LuaItemNull_GetTreePath},
		{ "SetAreaTestFile",				LuaItemNull_SetAreaTestFile},
		{ "SetIntPos",						LuaItemNull_SetIntPos},
		{ "IsIntPos",						LuaItemNull_IsIntPos},
        { "IsLink",                         LuaItemNull_IsLink},
        { "GetLinkInfo",                    LuaItemNull_GetLinkInfo},

		{ "IsValid",						LuaItemNull_IsValid },
//		{ "__newindex",						LuaItemNull_NewIndex },
		{ "__eq",							LuaItemNull_Equal },

		//animate
		{ "SetGroup",						LuaItemAnimate_SetGroup},
		{ "SetLoopCount",					LuaItemAnimate_SetLoopCount},
		{ "SetImagePath",					LuaItemAnimate_SetImagePath},
		{ "SetAnimate",						LuaItemAnimate_SetAnimate},
		{ "AutoSize",						LuaItemAnimate_AutoSize},
        { "Replay",						    LuaItemAnimate_Replay},

		{ NULL,								NULL },
	};

	static luaL_reg const s_aItemSceneMetaTable[] =
	{
		//itemnull
		{ "Show",							LuaItemNull_Show},
		{ "Hide",							LuaItemNull_Hide},
		{ "PtInItem",						LuaItemNull_PtInItem},
		{ "SetRelPos",						LuaItemNull_SetRelPos},
		{ "GetRelPos",						LuaItemNull_GetRelPos},
		{ "SetAbsPos",						LuaItemNull_SetAbsPos},
		{ "GetAbsPos",						LuaItemNull_GetAbsPos},
		{ "SetSize",						LuaItemNull_SetSize},
		{ "GetSize",						LuaItemNull_GetSize},
		{ "SetPosType",						LuaItemNull_SetPosType},
		{ "GetPosType",						LuaItemNull_GetPosType},
		{ "IsVisible",						LuaItemNull_IsVisible},
		{ "GetName",						LuaItemNull_GetName},
		{ "SetName",						LuaItemNull_SetName},
		{ "SetTip",							LuaItemNull_SetTip},
		{ "GetTip",							LuaItemNull_GetTip},
		{ "SetUserData",					LuaItemNull_SetUserData},
		{ "GetUserData",					LuaItemNull_GetUserData},
		{ "RegisterEvent",					LuaItemNull_RegisterEvent},
		{ "ClearEvent",						LuaItemNull_ClearEvent},
		{ "EnableScale",					LuaItemNull_EnableScale},
		{ "Scale",							LuaItemNull_Scale},		
		{ "LockShowAndHide",				LuaItemNull_LockShowAndHide},
		{ "SetAlpha",						LuaItemNull_SetAlpha},
		{ "GetAlpha",						LuaItemNull_GetAlpha},
		{ "GetParent",						LuaItemNull_GetParent},
		{ "GetRoot",						LuaItemNull_GetRoot},
		{ "GetType",						LuaItemNull_GetType},
		{ "GetIndex",						LuaItemNull_GetIndex},
		{ "SetIndex",						LuaItemNull_SetIndex},
		{ "ExchangeIndex",					LuaItemNull_ExchangeIndex},
		{ "GetTreePath",					LuaItemNull_GetTreePath},
		{ "SetAreaTestFile",				LuaItemNull_SetAreaTestFile},
		{ "SetIntPos",						LuaItemNull_SetIntPos},
		{ "IsIntPos",						LuaItemNull_IsIntPos},
        { "IsLink",                         LuaItemNull_IsLink},
        { "GetLinkInfo",                    LuaItemNull_GetLinkInfo},

		{ "IsValid",						LuaItemNull_IsValid },
//		{ "__newindex",						LuaItemNull_NewIndex },
		{ "__eq",							LuaItemNull_Equal },

		//scene
		{ "SetScene",						LuaItemScene_SetScene},
		{ "EnableRenderTerrain",			LuaItemScene_EnableRenderTerrain},
		{ "EnableRenderSkyBox",				LuaItemScene_EnableRenderSkyBox},
		{ "EnableAlpha",					LuaItemScene_EnableAlpha},

		{ NULL,								NULL },
	};

	static luaL_reg const s_aItemBoxMetaTable[] =
	{
		//itemnull
		{ "Show",							LuaItemNull_Show},
		{ "Hide",							LuaItemNull_Hide},
		{ "PtInItem",						LuaItemNull_PtInItem},
		{ "SetRelPos",						LuaItemNull_SetRelPos},
		{ "GetRelPos",						LuaItemNull_GetRelPos},
		{ "SetAbsPos",						LuaItemNull_SetAbsPos},
		{ "GetAbsPos",						LuaItemNull_GetAbsPos},
		{ "SetSize",						LuaItemNull_SetSize},
		{ "GetSize",						LuaItemNull_GetSize},
		{ "SetPosType",						LuaItemNull_SetPosType},
		{ "GetPosType",						LuaItemNull_GetPosType},
		{ "IsVisible",						LuaItemNull_IsVisible},
		{ "GetName",						LuaItemNull_GetName},
		{ "SetName",						LuaItemNull_SetName},
		{ "SetTip",							LuaItemNull_SetTip},
		{ "GetTip",							LuaItemNull_GetTip},
		{ "SetUserData",					LuaItemNull_SetUserData},
		{ "GetUserData",					LuaItemNull_GetUserData},
		{ "RegisterEvent",					LuaItemNull_RegisterEvent},
		{ "ClearEvent",						LuaItemNull_ClearEvent},
		{ "EnableScale",					LuaItemNull_EnableScale},
		{ "Scale",							LuaItemNull_Scale},		
		{ "LockShowAndHide",				LuaItemNull_LockShowAndHide},
		{ "SetAlpha",						LuaItemNull_SetAlpha},
		{ "GetAlpha",						LuaItemNull_GetAlpha},
		{ "GetParent",						LuaItemNull_GetParent},
		{ "GetRoot",						LuaItemNull_GetRoot},
		{ "GetType",						LuaItemNull_GetType},
		{ "GetIndex",						LuaItemNull_GetIndex},
		{ "SetIndex",						LuaItemNull_SetIndex},
		{ "ExchangeIndex",					LuaItemNull_ExchangeIndex},
		{ "GetTreePath",					LuaItemNull_GetTreePath},
		{ "SetAreaTestFile",				LuaItemNull_SetAreaTestFile},
		{ "SetIntPos",						LuaItemNull_SetIntPos},
		{ "IsIntPos",						LuaItemNull_IsIntPos},
        { "IsLink",                         LuaItemNull_IsLink},
        { "GetLinkInfo",                    LuaItemNull_GetLinkInfo},

		{ "IsValid",						LuaItemNull_IsValid },
//		{ "__newindex",						LuaItemNull_NewIndex },
		{ "__eq",							LuaItemNull_Equal },

		//box
		{ "SetBoxIndex",					LuaItemBox_SetIndex},
		{ "GetBoxIndex",					LuaItemBox_GetIndex},
		{ "SetObject",						LuaItemBox_SetObject},
		{ "GetObject",						LuaItemBox_GetObject},
		{ "GetObjectType",					LuaItemBox_GetObjectType},
		{ "GetObjectData",					LuaItemBox_GetObjectData},
		{ "ClearObject",					LuaItemBox_ClearObject},
		{ "IsEmpty",						LuaItemBox_IsEmpty},
		{ "EnableObject",					LuaItemBox_EnableObject},
		{ "IsObjectEnable",					LuaItemBox_IsObjectEnable},
		{ "EnableObjectEquip",				LuaItemBox_EnableObjectEquip},
		{ "IsObjectEquipable",				LuaItemBox_IsObjectEquipable},
		{ "SetObjectCoolDown",				LuaItemBox_SetObjectCoolDown},
		{ "IsObjectCoolDown",				LuaItemBox_IsObjectCoolDown},
		{ "SetObjectSparking",				LuaItemBox_SetObjectSparking},
		{ "SetObjectInUse",					LuaItemBox_SetObjectInUse},
		{ "SetObjectStaring",				LuaItemBox_SetObjectStaring},
		{ "SetObjectSelected",				LuaItemBox_SetObjectSelected},
		{ "IsObjectSelected",				LuaItemBox_IsObjectSelected},
		{ "SetObjectMouseOver",				LuaItemBox_SetObjectMouseOver},
		{ "IsObjectMouseOver",				LuaItemBox_IsObjectMouseOver},
		{ "SetObjectPressed",				LuaItemBox_SetObjectPressed},
		{ "IsObjectPressed",				LuaItemBox_IsObjectPressed},
		{ "SetCoolDownPercentage",			LuaItemBox_SetCoolDownPercentage},
		{ "GetCoolDownPercentage",			LuaItemBox_GetCoolDownPercentage},
		{ "SetObjectIcon",					LuaItemBox_SetObjectIcon},
		{ "GetObjectIcon",					LuaItemBox_GetObjectIcon},
		{ "ClearObjectIcon",				LuaItemBox_ClearObjectIcon},
		{ "SetOverText",					LuaItemBox_SetOverText},
		{ "GetOverText",					LuaItemBox_GetOverText},
		{ "SetOverTextFontScheme",			LuaItemBox_SetOverTextFontScheme},
		{ "GetOverTextFontScheme",			LuaItemBox_GetOverTextFontScheme},
		{ "SetOverTextPosition",			LuaItemBox_SetOverTextPosition},
		{ "GetOverTextPosition",			LuaItemBox_GetOverTextPosition},

		{ NULL,								NULL },
	};

	static luaL_reg const s_aItemHandleMetaTable[] =
	{
		//itemnull
		{ "Show",							LuaItemNull_Show},
		{ "Hide",							LuaItemNull_Hide},
		{ "PtInItem",						LuaItemNull_PtInItem},
		{ "SetRelPos",						LuaItemNull_SetRelPos},
		{ "GetRelPos",						LuaItemNull_GetRelPos},
		{ "SetAbsPos",						LuaItemNull_SetAbsPos},
		{ "GetAbsPos",						LuaItemNull_GetAbsPos},
		{ "SetSize",						LuaItemNull_SetSize},
		{ "GetSize",						LuaItemNull_GetSize},
		{ "SetPosType",						LuaItemNull_SetPosType},
		{ "GetPosType",						LuaItemNull_GetPosType},
		{ "IsVisible",						LuaItemNull_IsVisible},
		{ "GetName",						LuaItemNull_GetName},
		{ "SetName",						LuaItemNull_SetName},
		{ "SetTip",							LuaItemNull_SetTip},
		{ "GetTip",							LuaItemNull_GetTip},
		{ "SetUserData",					LuaItemNull_SetUserData},
		{ "GetUserData",					LuaItemNull_GetUserData},
		{ "RegisterEvent",					LuaItemNull_RegisterEvent},
		{ "ClearEvent",						LuaItemNull_ClearEvent},
		{ "EnableScale",					LuaItemNull_EnableScale},
		{ "Scale",							LuaItemNull_Scale},		
		{ "LockShowAndHide",				LuaItemNull_LockShowAndHide},
		{ "SetAlpha",						LuaItemNull_SetAlpha},
		{ "GetAlpha",						LuaItemNull_GetAlpha},
		{ "GetParent",						LuaItemNull_GetParent},
		{ "GetRoot",						LuaItemNull_GetRoot},
		{ "GetType",						LuaItemNull_GetType},
		{ "GetIndex",						LuaItemNull_GetIndex},
		{ "SetIndex",						LuaItemNull_SetIndex},
		{ "ExchangeIndex",					LuaItemNull_ExchangeIndex},
		{ "GetTreePath",					LuaItemNull_GetTreePath},
		{ "SetAreaTestFile",				LuaItemNull_SetAreaTestFile},
		{ "SetIntPos",						LuaItemNull_SetIntPos},
		{ "IsIntPos",						LuaItemNull_IsIntPos},
        { "IsLink",                         LuaItemNull_IsLink},
        { "GetLinkInfo",                    LuaItemNull_GetLinkInfo},

		{ "IsValid",						LuaItemNull_IsValid },
//		{ "__newindex",						LuaItemNull_NewIndex },
		{ "__eq",							LuaItemNull_Equal },

		//handle
		{ "AppendItemFromString",			LuaItemHandle_AppendItemFromString},
		{ "AppendItemFromIni",				LuaItemHandle_AppendItemFromIni},
		{ "AdjustItemShowInfo",				LuaItemHandle_AdjustItemShowInfo},
		{ "InsertItemFromString",			LuaItemHandle_InsertItemFromString},
		{ "InsertItemFromIni",				LuaItemHandle_InsertItemFromIni},
		{ "FormatAllItemPos",				LuaItemHandle_FormatAllItemPos},
		{ "SetHandleStyle",					LuaItemHandle_SetHandleStyle},
		{ "SetMinRowHeight",				LuaItemHandle_SetMinRowHeight},
		{ "SetMaxRowHeight",				LuaItemHandle_SetMaxRowHeight},
		{ "SetRowHeight",					LuaItemHandle_SetRowHeight},
		{ "SetRowSpacing",					LuaItemHandle_SetRowSpacing},
		{ "RemoveItem",						LuaItemHandle_RemoveItem},
		{ "Clear",							LuaItemHandle_Clear},
		{ "GetItemStartRelPos",				LuaItemHandle_GetItemStartRelPos},
		{ "SetItemStartRelPos",				LuaItemHandle_SetItemStartRelPos},
		{ "SetSizeByAllItemSize",			LuaItemHandle_SetSizeByAllItemSize},
		{ "GetAllItemSize",					LuaItemHandle_GetAllItemSize},
		{ "GetItemCount",					LuaItemHandle_GetItemCount},
		{ "GetVisibleItemCount",			LuaItemHandle_GetVisibleItemCount},
		{ "Lookup",							LuaItemHandle_Lookup},
		{ "EnableFormatWhenAppend",			LuaItemHandle_EnableFormatWhenAppend},
		{ "RemoveItemUntilNewLine",			LuaItemHandle_RemoveItemUntilNewLine},
		{ "ExchangeItemIndex",				LuaItemHandle_ExchangeItemIndex},
		{ "Sort",							LuaItemHandle_Sort },
		
		
		{ NULL,								NULL },
	};

	static luaL_reg const s_aItemTreeLeafMetaTable[] =
	{
		//itemnull
		{ "Show",							LuaItemNull_Show},
		{ "Hide",							LuaItemNull_Hide},
		{ "PtInItem",						LuaItemNull_PtInItem},
		{ "SetRelPos",						LuaItemNull_SetRelPos},
		{ "GetRelPos",						LuaItemNull_GetRelPos},
		{ "SetAbsPos",						LuaItemNull_SetAbsPos},
		{ "GetAbsPos",						LuaItemNull_GetAbsPos},
		{ "SetSize",						LuaItemNull_SetSize},
		{ "GetSize",						LuaItemNull_GetSize},
		{ "SetPosType",						LuaItemNull_SetPosType},
		{ "GetPosType",						LuaItemNull_GetPosType},
		{ "IsVisible",						LuaItemNull_IsVisible},
		{ "GetName",						LuaItemNull_GetName},
		{ "SetName",						LuaItemNull_SetName},
		{ "SetTip",							LuaItemNull_SetTip},
		{ "GetTip",							LuaItemNull_GetTip},
		{ "SetUserData",					LuaItemNull_SetUserData},
		{ "GetUserData",					LuaItemNull_GetUserData},
		{ "RegisterEvent",					LuaItemNull_RegisterEvent},
		{ "ClearEvent",						LuaItemNull_ClearEvent},
		{ "EnableScale",					LuaItemNull_EnableScale},
		{ "Scale",							LuaItemNull_Scale},		
		{ "LockShowAndHide",				LuaItemNull_LockShowAndHide},
		{ "SetAlpha",						LuaItemNull_SetAlpha},
		{ "GetAlpha",						LuaItemNull_GetAlpha},
		{ "GetParent",						LuaItemNull_GetParent},
		{ "GetRoot",						LuaItemNull_GetRoot},
		{ "GetType",						LuaItemNull_GetType},
		{ "GetIndex",						LuaItemNull_GetIndex},
		{ "SetIndex",						LuaItemNull_SetIndex},
		{ "ExchangeIndex",					LuaItemNull_ExchangeIndex},
		{ "GetTreePath",					LuaItemNull_GetTreePath},
		{ "SetAreaTestFile",				LuaItemNull_SetAreaTestFile},
		{ "SetIntPos",						LuaItemNull_SetIntPos},
		{ "IsIntPos",						LuaItemNull_IsIntPos},
        { "IsLink",                         LuaItemNull_IsLink},
        { "GetLinkInfo",                    LuaItemNull_GetLinkInfo},

		{ "IsValid",						LuaItemNull_IsValid },
//		{ "__newindex",						LuaItemNull_NewIndex },
		{ "__eq",							LuaItemNull_Equal },

		//handle
		{ "AppendItemFromString",			LuaItemHandle_AppendItemFromString},
		{ "AppendItemFromIni",				LuaItemHandle_AppendItemFromIni},
		{ "AdjustItemShowInfo",				LuaItemHandle_AdjustItemShowInfo},
		{ "InsertItemFromString",			LuaItemHandle_InsertItemFromString},
		{ "InsertItemFromIni",				LuaItemHandle_InsertItemFromIni},
		{ "FormatAllItemPos",				LuaItemHandle_FormatAllItemPos},
		{ "SetHandleStyle",					LuaItemHandle_SetHandleStyle},
		{ "SetMinRowHeight",				LuaItemHandle_SetMinRowHeight},
		{ "SetMaxRowHeight",				LuaItemHandle_SetMaxRowHeight},
		{ "SetRowHeight",					LuaItemHandle_SetRowHeight},
		{ "SetRowSpacing",					LuaItemHandle_SetRowSpacing},
		{ "RemoveItem",						LuaItemHandle_RemoveItem},
		{ "Clear",							LuaItemHandle_Clear},
		{ "GetItemStartRelPos",				LuaItemHandle_GetItemStartRelPos},
		{ "SetItemStartRelPos",				LuaItemHandle_SetItemStartRelPos},
		{ "SetSizeByAllItemSize",			LuaItemHandle_SetSizeByAllItemSize},
		{ "GetAllItemSize",					LuaItemHandle_GetAllItemSize},
		{ "GetItemCount",					LuaItemHandle_GetItemCount},
		{ "GetVisibleItemCount",			LuaItemHandle_GetVisibleItemCount},
		{ "Lookup",							LuaItemHandle_Lookup},
		{ "EnableFormatWhenAppend",			LuaItemHandle_EnableFormatWhenAppend},
		{ "RemoveItemUntilNewLine",			LuaItemHandle_RemoveItemUntilNewLine},
		{ "ExchangeItemIndex",				LuaItemHandle_ExchangeItemIndex},
		{ "Sort",							LuaItemHandle_Sort },
		

		//treeleaf
		{ "IsExpand",						LuaItemTreeLeaf_IsExpand},
		{ "ExpandOrCollapse",				LuaItemTreeLeaf_ExpandOrCollapse},
		{ "Expand",							LuaItemTreeLeaf_Expand},
		{ "Collapse",						LuaItemTreeLeaf_Collapse},
		{ "SetIndent",						LuaItemTreeLeaf_SetIndent},
		{ "GetIndent",						LuaItemTreeLeaf_GetIndent},
		{ "SetEachIndentWidth",				LuaItemTreeLeaf_SetEachIndentWidth},
		{ "GetEachIndentWidth",				LuaItemTreeLeaf_GetEachIndentWidth},
		{ "SetNodeIconSize",				LuaItemTreeLeaf_SetNodeIconSize},
		{ "SetIconImage",					LuaItemTreeLeaf_SetIconImage},
		{ "PtInIcon",						LuaItemTreeLeaf_PtInIcon},
		{ "AdjustNodeIconPos",				LuaItemTreeLeaf_AdjustNodeIconPos},
		{ "AutoSetIconSize",				LuaItemTreeLeaf_AutoSetIconSize},
		{ "SetShowIndex",					LuaItemTreeLeaf_SetShowIndex},
		{ "GetShowIndex",					LuaItemTreeLeaf_GetShowIndex},

		{ NULL,								NULL },
	};

	KScriptLoader::RegisterMetaTable(L, g_GetWndItemName(ITEM_NULL), s_aItemNullMetaTable);
	KScriptLoader::RegisterMetaTable(L, g_GetWndItemName(ITEM_TEXT), s_aItemTextMetaTable);
	KScriptLoader::RegisterMetaTable(L, g_GetWndItemName(ITEM_IMAGE), s_aItemImageMetaTable);
	KScriptLoader::RegisterMetaTable(L, g_GetWndItemName(ITEM_ANIAMTE), s_aItemAnimateMetaTable);
	KScriptLoader::RegisterMetaTable(L, g_GetWndItemName(ITEM_SHADOW), s_aItemShadowMetaTable);
	KScriptLoader::RegisterMetaTable(L, g_GetWndItemName(ITEM_SCENE), s_aItemSceneMetaTable);
	KScriptLoader::RegisterMetaTable(L, g_GetWndItemName(ITEM_BOX), s_aItemBoxMetaTable);
	KScriptLoader::RegisterMetaTable(L, g_GetWndItemName(ITEM_HANDLE), s_aItemHandleMetaTable);
	KScriptLoader::RegisterMetaTable(L, g_GetWndItemName(ITEM_TREE_LEAFE), s_aItemTreeLeafMetaTable);
}


template <typename TItemType>
int KItemHandleClassScriptTable::THostWndItem_GetUserData(lua_State* L, TItemType** ppItem, int nIndex /*= 1*/)
{
	int nResult = false;
	KItemNull *pItem = NULL;
	int nTopIndex = lua_gettop(L);

	ASSERT(L);
	ASSERT(ppItem);
	ASSERT(!(*ppItem));
	ASSERT(nTopIndex > 0);

	KGLOG_PROCESS_ERROR(lua_istable(L, nIndex));
	lua_pushvalue(L, nIndex);
	lua_getfield(L, -1, STR_OBJ_KEY_NAME);
	pItem = reinterpret_cast<KItemNull *>(lua_touserdata(L, -1)); 
	KG_PROCESS_ERROR(pItem);
	*ppItem = g_ItemCast<TItemType>(pItem);
	KGLOG_PROCESS_ERROR(*ppItem);

	nResult = true;
Exit0:
	lua_settop(L, nTopIndex);
	return nResult;
}

int KItemHandleClassScriptTable::HostWndItem_Lookup(lua_State* L, KItemNull *pItem)
{
	int nResult = false;
	LPCSTR pcszMetalTable = NULL;
	KG_PROCESS_ERROR(pItem);

	if (pItem->GetLuaRef() != LUA_NOREF)
	{
		lua_rawgeti(L, LUA_REGISTRYINDEX, pItem->GetLuaRef());
		goto Exit1;
	}

	pcszMetalTable = g_GetWndItemName(pItem->GetItemType());
	KGLOG_PROCESS_ERROR(pcszMetalTable);

	lua_newtable(L);
	lua_pushstring(L, STR_OBJ_KEY_NAME);
	lua_pushlightuserdata(L, pItem);
	lua_settable(L, -3);

	luaL_getmetatable(L, pcszMetalTable);
	lua_setmetatable(L, -2);

	lua_pushvalue(L, -1);
	pItem->SetLuaRef(luaL_ref(L, LUA_REGISTRYINDEX));

Exit1:
	nResult = true;
Exit0:
	return nResult;
}

int KItemHandleClassScriptTable::LuaItemNull_Show(lua_State *L)
{
	int nRetCode = false;
	KItemNull *pItemNull = NULL;
	int nFoce = false;
	int nParamCount = Lua_GetTopIndex(L);

	KG_PROCESS_ERROR(nParamCount > 0);

	nRetCode = THostWndItem_GetUserData(L, &pItemNull);
	KG_PROCESS_ERROR(nRetCode);
	ASSERT(pItemNull);

	nFoce = pItemNull->IsShowOrHideLocked();
	pItemNull->LockShowAndHide(false);

	if (nParamCount > 1)
	{
		int nShow = true;
		if (lua_isboolean(L, 2))
			nShow = Lua_ValueToBoolean(L, 2);
		else
			nShow = (int)Lua_ValueToNumber(L, 2);
		if (nShow)
			pItemNull->Show();
		else
			pItemNull->Hide();
	}
	else
		pItemNull->Show();

	pItemNull->LockShowAndHide(nFoce);
Exit0:
	return 0;
}

int KItemHandleClassScriptTable::LuaItemNull_Hide(lua_State *L)
{
	int nRetCode = false;
	int nFoce = false;
	KItemNull *pItemNull = NULL;
	int nParamCount = Lua_GetTopIndex(L);

	KG_PROCESS_ERROR(nParamCount > 0);

	nRetCode = THostWndItem_GetUserData(L, &pItemNull);
	KG_PROCESS_ERROR(nRetCode);
	ASSERT(pItemNull);

	nFoce = pItemNull->IsShowOrHideLocked();
	pItemNull->LockShowAndHide(false);

	if (nParamCount > 1)
	{
		int nHide = true;
		if (lua_isboolean(L, 2))
			nHide = Lua_ValueToBoolean(L, 2);
		else
			nHide = (int)Lua_ValueToNumber(L, 2);
		if (nHide)
			pItemNull->Hide();
		else
			pItemNull->Show();
	}
	else
		pItemNull->Hide();

	pItemNull->LockShowAndHide(nFoce);

Exit0:
	return 0;
}

int KItemHandleClassScriptTable::LuaItemNull_PtInItem(lua_State *L)
{
	int nResult = false;
	int nRetCode = false;
	float fAbsX = 0.0f;
	float fAbsY = 0.0f;
	KItemNull *pItemNull = NULL;
	float fScale = 1.0f;

	ASSERT(L);

	KG_PROCESS_ERROR(Lua_GetTopIndex(L) == 3);

	nRetCode = THostWndItem_GetUserData(L, &pItemNull);
	KGLOG_PROCESS_ERROR(nRetCode);
	ASSERT(pItemNull);

	fScale = g_pUI->m_WndStation.GetUIScale();

	fAbsX = (float)Lua_ValueToNumber(L, 2) * fScale;
	fAbsY = (float)Lua_ValueToNumber(L, 3) * fScale;

	nResult = pItemNull->PtInItem(fAbsX, fAbsY);
Exit0:
	Lua_PushBoolean(L, nResult);
	return 1;
}

int KItemHandleClassScriptTable::LuaItemNull_SetRelPos(lua_State *L)
{
	int nRetCode = false;
	float fRelX = 0.0f;
	float fRelY = 0.0f;
	KItemNull *pItemNull = NULL;
	float fScale = 1.0f;

	ASSERT(L);

	KG_PROCESS_ERROR(Lua_GetTopIndex(L) == 3);

	nRetCode = THostWndItem_GetUserData(L, &pItemNull);
	KGLOG_PROCESS_ERROR(nRetCode);
	ASSERT(pItemNull);

	fScale = g_pUI->m_WndStation.GetUIScale();

	fRelX = (float)Lua_ValueToNumber(L, 2) * fScale;
	fRelY = (float)Lua_ValueToNumber(L, 3) * fScale;	

	pItemNull->SetRelPos(fRelX, fRelY);

Exit0:
	return 0;
}

int KItemHandleClassScriptTable::LuaItemNull_GetRelPos(lua_State *L)
{
	int nRetCode = false;
	float fRelX = 0.0f;
	float fRelY = 0.0f;
	KItemNull *pItemNull = NULL;
	float fScale = 1.0f;

	ASSERT(L);

	KG_PROCESS_ERROR(Lua_GetTopIndex(L) == 1);

	nRetCode = THostWndItem_GetUserData(L, &pItemNull);
	KGLOG_PROCESS_ERROR(nRetCode);
	ASSERT(pItemNull);

	pItemNull->GetRelPos(fRelX, fRelY);

	fScale = g_pUI->m_WndStation.GetUIScale();

	fRelX /= fScale;
	fRelY /= fScale;

Exit0:
	Lua_PushNumber(L, fRelX);
	Lua_PushNumber(L, fRelY);
	return 2;
}

int KItemHandleClassScriptTable::LuaItemNull_SetAbsPos(lua_State *L)
{
	int nRetCode = false;
	float fAbsX = 0.0f;
	float fAbsY = 0.0f;
	KItemNull *pItemNull = NULL;
	float fScale = 1.0f;

	ASSERT(L);

	KG_PROCESS_ERROR(Lua_GetTopIndex(L) == 3);

	nRetCode = THostWndItem_GetUserData(L, &pItemNull);
	KGLOG_PROCESS_ERROR(nRetCode);
	ASSERT(pItemNull);

	fScale = g_pUI->m_WndStation.GetUIScale();

	fAbsX = (float)Lua_ValueToNumber(L, 2) * fScale;
	fAbsY = (float)Lua_ValueToNumber(L, 3) * fScale;

	pItemNull->SetAbsPos(fAbsX, fAbsY);

Exit0:
	return 0;
}

int KItemHandleClassScriptTable::LuaItemNull_GetAbsPos(lua_State *L)
{
	int nRetCode = false;
	float fAbsX = 0.0f;
	float fAbsY = 0.0f;
	KItemNull *pItemNull = NULL;
	float fScale = 1.0f;

	ASSERT(L);

	KG_PROCESS_ERROR(Lua_GetTopIndex(L) == 1);

	nRetCode = THostWndItem_GetUserData(L, &pItemNull);
	KGLOG_PROCESS_ERROR(nRetCode);
	ASSERT(pItemNull);

	pItemNull->GetAbsPos(fAbsX, fAbsY);

	fScale = g_pUI->m_WndStation.GetUIScale();

	fAbsX /= fScale;
	fAbsY /= fScale;

Exit0:
	Lua_PushNumber(L, fAbsX);
	Lua_PushNumber(L, fAbsY);
	return 2;
}

int KItemHandleClassScriptTable::LuaItemNull_SetSize(lua_State *L)
{
	int nRetCode = false;
	float fWidth = 0.0f;
	float fHeight = 0.0f;
	KItemNull *pItemNull = NULL;
	float fScale = 1.0f;

	ASSERT(L);

	KG_PROCESS_ERROR(Lua_GetTopIndex(L) == 3);

	nRetCode = THostWndItem_GetUserData(L, &pItemNull);
	KGLOG_PROCESS_ERROR(nRetCode);
	ASSERT(pItemNull);

	fScale = g_pUI->m_WndStation.GetUIScale();

	fWidth = (float)Lua_ValueToNumber(L, 2) * fScale;
	fHeight = (float)Lua_ValueToNumber(L, 3) * fScale;

	pItemNull->SetSize(fWidth, fHeight);

Exit0:
	return 0;
}

int KItemHandleClassScriptTable::LuaItemNull_GetSize(lua_State *L)
{
	int nRetCode = false;
	float fWidth = 0.0f;
	float fHeight = 0.0f;
	KItemNull *pItemNull = NULL;
	float fScale = 1.0f;

	ASSERT(L);

	KG_PROCESS_ERROR(Lua_GetTopIndex(L) == 1);

	nRetCode = THostWndItem_GetUserData(L, &pItemNull);
	KGLOG_PROCESS_ERROR(nRetCode);
	ASSERT(pItemNull);

	pItemNull->GetSize(fWidth, fHeight);

	fScale = g_pUI->m_WndStation.GetUIScale();

	fWidth /= fScale;
	fHeight /= fScale;

Exit0:
	Lua_PushNumber(L, fWidth);
	Lua_PushNumber(L, fHeight);
	return 2;
}

int KItemHandleClassScriptTable::LuaItemNull_SetPosType(lua_State *L)
{
	int nRetCode = false;
	DWORD dwPosType = 0x00000000;
	KItemNull *pItemNull = NULL;

	ASSERT(L);

	KG_PROCESS_ERROR(Lua_GetTopIndex(L) == 2);

	nRetCode = THostWndItem_GetUserData(L, &pItemNull);
	KGLOG_PROCESS_ERROR(nRetCode);
	ASSERT(pItemNull);

	dwPosType = (DWORD)Lua_ValueToNumber(L, 2);

	pItemNull->SetPosType(dwPosType);

Exit0:
	return 0;
}

int KItemHandleClassScriptTable::LuaItemNull_GetPosType(lua_State *L)
{
	int nRetCode = false;
	DWORD dwPosType = 0x00000000;
	KItemNull *pItemNull = NULL;

	ASSERT(L);

	KG_PROCESS_ERROR(Lua_GetTopIndex(L) == 1);

	nRetCode = THostWndItem_GetUserData(L, &pItemNull);
	KGLOG_PROCESS_ERROR(nRetCode);
	ASSERT(pItemNull);

	pItemNull->GetPosType(dwPosType);

Exit0:
	Lua_PushNumber(L, dwPosType);
	return 1;
}

int KItemHandleClassScriptTable::LuaItemNull_IsVisible(lua_State *L)
{
	int nResult = false;
	int nRetCode = false;
	KItemNull *pItemNull = NULL;

	ASSERT(L);

	KG_PROCESS_ERROR(Lua_GetTopIndex(L) == 1);

	nRetCode = THostWndItem_GetUserData(L, &pItemNull);
	KGLOG_PROCESS_ERROR(nRetCode);
	ASSERT(pItemNull);

	nResult = pItemNull->IsVisible();
Exit0:
	Lua_PushBoolean(L, nResult);
	return 1;
}

int KItemHandleClassScriptTable::LuaItemNull_SetName(lua_State *L)
{
	int nRetCode = false;
	LPCSTR szName = NULL;
	KItemNull* pItemNull = NULL;

	ASSERT(L);

	KG_PROCESS_ERROR(Lua_GetTopIndex(L) == 2);

	nRetCode = THostWndItem_GetUserData(L, &pItemNull);
	KGLOG_PROCESS_ERROR(nRetCode);
	ASSERT(pItemNull);

	szName = lua_tostring(L, 2);
	KGLOG_PROCESS_ERROR(szName);

    pItemNull->SetName(szName);

Exit0:
	return 0;
}

int KItemHandleClassScriptTable::LuaItemNull_GetName(lua_State *L)
{
	int nRetCode = false;
	LPCSTR szName = "";
	KItemNull* pItemNull = NULL;

	ASSERT(L);

	KG_PROCESS_ERROR(Lua_GetTopIndex(L) == 1);

	nRetCode = THostWndItem_GetUserData(L, &pItemNull);
	KGLOG_PROCESS_ERROR(nRetCode);
	ASSERT(pItemNull);

	szName = pItemNull->GetName();

Exit0:
    Lua_PushString(L, szName);
	return 1;
}

int KItemHandleClassScriptTable::LuaItemNull_SetTip(lua_State *L)
{
	int nRetCode = false;
	KItemNull *pItemNull = NULL;
	int nParamCount = 0;
	int nTipIndex = -1;

	ASSERT(L);

	nParamCount = Lua_GetTopIndex(L);
	KG_PROCESS_ERROR(nParamCount >= 2);

	nRetCode = THostWndItem_GetUserData(L, &pItemNull);
	KGLOG_PROCESS_ERROR(nRetCode);
	ASSERT(pItemNull);

	if (lua_type(L, 2) == LUA_TNUMBER)
	{
		nTipIndex = (int)Lua_ValueToNumber(L, 2);
		if (nTipIndex < 0)
			nTipIndex = -1;
		KTipCenter::GetSelf().AddRef(nTipIndex);
	}
	else
	{
		KG_PROCESS_ERROR(nParamCount > 2);

		int nShowTipType = 0;
		int nRichText = false;
		int nStrIndex = -1;
        LPCSTR pcszTip = NULL;

		pcszTip = Lua_ValueToString(L, 2);
		KGLOG_PROCESS_ERROR(pcszTip);

		nStrIndex = g_pUI->m_GlobalStrValuableMgr.GetIndexAt(pcszTip);

		nShowTipType = (int)Lua_ValueToNumber(L, 3);

		if (Lua_GetTopIndex(L) == 4)
			nRichText = (int)Lua_ValueToNumber(L, 4);

		nTipIndex = KTipCenter::GetSelf().Append(nStrIndex, nShowTipType, nRichText);
	}

	pItemNull->SetTipIndex(nTipIndex);

Exit0:
	return 0;
}

int KItemHandleClassScriptTable::LuaItemNull_GetTip(lua_State *L)
{
	int nRetCode = false;
	LPCSTR pcszTip = NULL;
	int nShowTipType = 0;
	KItemNull *pItemNull = NULL;

	ASSERT(L);

	KG_PROCESS_ERROR(Lua_GetTopIndex(L) == 1);

	nRetCode = THostWndItem_GetUserData(L, &pItemNull);
	KGLOG_PROCESS_ERROR(nRetCode);
	ASSERT(pItemNull);

	nRetCode = pItemNull->GetTipIndex();
	KG_PROCESS_ERROR(nRetCode >= 0);
	pcszTip = KTipCenter::GetSelf().GetTip(nRetCode);
	nShowTipType = KTipCenter::GetSelf().GetShowTipType(nRetCode);

Exit0:
	Lua_PushString(L, pcszTip);
	Lua_PushNumber(L, nShowTipType);
	return 2;
}

int KItemHandleClassScriptTable::LuaItemNull_SetUserData(lua_State *L)
{
	int nRetCode = false;
	double fUserData = -1;
	KItemNull *pItemNull = NULL;

	ASSERT(L);

	KG_PROCESS_ERROR(Lua_GetTopIndex(L) == 2);

	nRetCode = THostWndItem_GetUserData(L, &pItemNull);
	KGLOG_PROCESS_ERROR(nRetCode);
	ASSERT(pItemNull);

	fUserData = Lua_ValueToNumber(L, 2);

	pItemNull->SetUserData(fUserData);

Exit0:
	return 0;
}

int KItemHandleClassScriptTable::LuaItemNull_GetUserData(lua_State *L)
{
	int nRetCode = false;
	double fUserData = -1.0f;
	KItemNull *pItemNull = NULL;

	ASSERT(L);

	KG_PROCESS_ERROR(Lua_GetTopIndex(L) == 1);

	nRetCode = THostWndItem_GetUserData(L, &pItemNull);
	KGLOG_PROCESS_ERROR(nRetCode);
	ASSERT(pItemNull);

	fUserData = pItemNull->GetUserData();
Exit0:
	Lua_PushNumber(L, fUserData);
	return 1;
}

int KItemHandleClassScriptTable::LuaItemNull_RegisterEvent(Lua_State *L)
{
	int nRetCode = false;
	KItemNull *pItemNull = NULL;
	DWORD dwEventID = 0x00000000;

	ASSERT(L);

	KG_PROCESS_ERROR(Lua_GetTopIndex(L) == 2);

	nRetCode = THostWndItem_GetUserData(L, &pItemNull);
	KGLOG_PROCESS_ERROR(nRetCode);
	ASSERT(pItemNull);

	dwEventID = (DWORD)Lua_ValueToNumber(L, 2);

	pItemNull->RegisterEvent(dwEventID);

Exit0:
	return 0;
}

int KItemHandleClassScriptTable::LuaItemNull_ClearEvent(Lua_State *L)
{
	int nRetCode = false;
	KItemNull *pItemNull = NULL;

	ASSERT(L);

	KG_PROCESS_ERROR(Lua_GetTopIndex(L) == 1);

	nRetCode = THostWndItem_GetUserData(L, &pItemNull);
	KGLOG_PROCESS_ERROR(nRetCode);
	ASSERT(pItemNull);

	pItemNull->ClearEvent();

Exit0:
	return 0;
}

int KItemHandleClassScriptTable::LuaItemNull_EnableScale(Lua_State *L)
{
	int nRetCode = false;
	int nEnable = true;
	KItemNull *pItemNull = NULL;

	ASSERT(L);

	KG_PROCESS_ERROR(Lua_GetTopIndex(L) == 2);

	nRetCode = THostWndItem_GetUserData(L, &pItemNull);
	KGLOG_PROCESS_ERROR(nRetCode);
	ASSERT(pItemNull);

	if (Lua_IsBoolean(L, 2))
		nEnable = (int)Lua_ValueToBoolean(L, 2);
	else
		nEnable = (int)Lua_ValueToNumber(L, 2);

	pItemNull->EnableScale(nEnable);

Exit0:
	return 0;
}

int KItemHandleClassScriptTable::LuaItemNull_Scale(Lua_State *L)
{
	int nRetCode = false;
	float fX = 1.0f;
	float fY = 1.0f;
	KItemNull *pItemNull = NULL;

	ASSERT(L);

	KG_PROCESS_ERROR(Lua_GetTopIndex(L) == 3);

	nRetCode = THostWndItem_GetUserData(L, &pItemNull);
	KGLOG_PROCESS_ERROR(nRetCode);
	ASSERT(pItemNull);

	fX = (float)Lua_ValueToNumber(L, 2);
	fY = (float)Lua_ValueToNumber(L, 3);

	KGLOG_PROCESS_ERROR(fX > 0.0f);
	KGLOG_PROCESS_ERROR(fY > 0.0f);
	pItemNull->Scale(fX, fY);

Exit0:
	return 0;
}

int KItemHandleClassScriptTable::LuaItemNull_LockShowAndHide(Lua_State *L)
{
	int nRetCode = false;
	int nEnable = false;
	KItemNull *pItemNull = NULL;

	ASSERT(L);

	KG_PROCESS_ERROR(Lua_GetTopIndex(L) == 2);

	nRetCode = THostWndItem_GetUserData(L, &pItemNull);
	KGLOG_PROCESS_ERROR(nRetCode);
	ASSERT(pItemNull);

	if (Lua_IsBoolean(L, 2))
		nEnable = (int)Lua_ValueToBoolean(L, 2);
	else
		nEnable = (int)Lua_ValueToNumber(L, 2);

	pItemNull->LockShowAndHide(nEnable);

Exit0:
	return 0;
}

int KItemHandleClassScriptTable::LuaItemNull_SetAlpha(Lua_State *L)
{
	int nRetCode = false;
	KItemNull *pItemNull = NULL;
	int nAlpha = 255;

	ASSERT(L);

	KG_PROCESS_ERROR(Lua_GetTopIndex(L) == 2);

	nRetCode = THostWndItem_GetUserData(L, &pItemNull);
	KGLOG_PROCESS_ERROR(nRetCode);
	ASSERT(pItemNull);

	nAlpha = (int)Lua_ValueToNumber(L, 2);

	pItemNull->SetAlpha(nAlpha);

Exit0:
	return 0;
}

int KItemHandleClassScriptTable::LuaItemNull_GetAlpha(Lua_State *L)
{
	int nRetCode = false;
	KItemNull *pItemNull = NULL;
	int nAlpha = 255;

	ASSERT(L);

	KG_PROCESS_ERROR(Lua_GetTopIndex(L) == 1);

	nRetCode = THostWndItem_GetUserData(L, &pItemNull);
	KGLOG_PROCESS_ERROR(nRetCode);
	ASSERT(pItemNull);

	nAlpha = pItemNull->GetAlpha();

Exit0:
	Lua_PushNumber(L, nAlpha);
	return 1;
}

int KItemHandleClassScriptTable::LuaItemNull_GetParent(Lua_State *L)
{
	int nResult = 0;
	int nRetCode = false;
	KItemNull *pItemNull = NULL;
	KItemNull *pParent = NULL;
	KG_PROCESS_ERROR(Lua_GetTopIndex(L) == 1);

	nRetCode = THostWndItem_GetUserData(L, &pItemNull);
	KGLOG_PROCESS_ERROR(nRetCode);
	ASSERT(pItemNull);

	pParent = pItemNull->GetParent();
	if (pParent)
	{
		nRetCode = HostWndItem_Lookup(L, pParent);
		KGLOG_PROCESS_ERROR(nRetCode);
	}
	else
	{
		KItemHandle *pHandle = g_ItemCast<KItemHandle>(pItemNull);
		KG_PROCESS_ERROR(pHandle);
		KWndWindow *pWnd = pHandle->GetOwner();
		KG_PROCESS_ERROR(pWnd);
		nRetCode = KWndClassScriptTable::HostStation_Lookup(L, pWnd);
		KGLOG_PROCESS_ERROR(nRetCode);
	}

	nResult = 1;
Exit0:
	return nResult;
}

int KItemHandleClassScriptTable::LuaItemNull_GetRoot(Lua_State *L)
{
	int nResult = 0;
	int nRetCode = false;
	KItemNull *pItemNull = NULL;
	KItemNull *pParent = NULL;
	KG_PROCESS_ERROR(Lua_GetTopIndex(L) == 1);

	nRetCode = THostWndItem_GetUserData(L, &pItemNull);
	KGLOG_PROCESS_ERROR(nRetCode);
	ASSERT(pItemNull);

	pParent = pItemNull->GetParent();
	while (pParent)
	{
		pItemNull = pParent;
		pParent = pItemNull->GetParent();
	}	
	KItemHandle *pHandle = g_ItemCast<KItemHandle>(pItemNull);
	KG_PROCESS_ERROR(pHandle);
	KWndWindow *pWnd = pHandle->GetOwner();
	KG_PROCESS_ERROR(pWnd);
	while (pWnd->GetWndType() != "WndFrame")
	{
		pWnd = pWnd->GetParent();
		ASSERT(pWnd);
	}
	nRetCode = KWndClassScriptTable::HostStation_Lookup(L, pWnd);
	KG_PROCESS_ERROR(nRetCode);

	nResult = 1;
Exit0:
	return nResult;
}

int KItemHandleClassScriptTable::LuaItemNull_GetType(Lua_State *L)
{
	int nRetCode = false;
	LPCSTR pcszType = NULL;
	KItemNull *pItem = NULL;

	ASSERT(L);

	KG_PROCESS_ERROR(Lua_GetTopIndex(L) == 1);

	nRetCode = THostWndItem_GetUserData(L, &pItem);
	KGLOG_PROCESS_ERROR(nRetCode);
	ASSERT(pItem);

	pcszType = g_GetWndItemName(pItem->GetItemType());
Exit0:
	if (!pcszType)
		pcszType = "";
	Lua_PushString(L, pcszType);
	return 1;	
}

int KItemHandleClassScriptTable::LuaItemNull_GetIndex(Lua_State *L)
{
	int nRetCode = false;
	int nIndex = -1;
	KItemNull *pItem = NULL;
	KItemHandle *pParent = NULL;
	ASSERT(L);

	KG_PROCESS_ERROR(Lua_GetTopIndex(L) == 1);

	nRetCode = THostWndItem_GetUserData(L, &pItem);
	KGLOG_PROCESS_ERROR(nRetCode);
	ASSERT(pItem);

	pParent = pItem->GetParent();
	KGLOG_PROCESS_ERROR(pParent);
	nIndex = pParent->GetItemIndex(pItem);
Exit0:
	Lua_PushNumber(L, nIndex);
	return 1;	
}

int KItemHandleClassScriptTable::LuaItemNull_SetIndex(Lua_State *L)
{
	int nRetCode = false;
	int nIndex = 0;
	int nNewIndex = 0;
	KItemNull *pItem = NULL;
	KItemHandle *pParent = NULL;
	ASSERT(L);

	KG_PROCESS_ERROR(Lua_GetTopIndex(L) == 2);

	nRetCode = THostWndItem_GetUserData(L, &pItem);
	KGLOG_PROCESS_ERROR(nRetCode);
	ASSERT(pItem);

	pParent = pItem->GetParent();
	KGLOG_PROCESS_ERROR(pParent);
	nIndex = pParent->GetItemIndex(pItem);
	nNewIndex = (int)Lua_ValueToNumber(L, 2);

	pParent->SetItemNewIndex(nIndex, nNewIndex);
Exit0:
	return 0;	
}

int KItemHandleClassScriptTable::LuaItemNull_ExchangeIndex(Lua_State *L)
{
	int nRetCode = false;
	int nIndex = -1;
	int nNewIndex = 0;
	KItemNull *pItem = NULL;
	KItemNull *pItemNext = NULL;
	KItemHandle *pParent = NULL;
	
	ASSERT(L);

	KG_PROCESS_ERROR(Lua_GetTopIndex(L) == 2);

	nRetCode = THostWndItem_GetUserData(L, &pItem);
	KGLOG_PROCESS_ERROR(nRetCode);
	ASSERT(pItem);

	pParent = pItem->GetParent();
	KGLOG_PROCESS_ERROR(pParent);

	if (Lua_IsNumber(L, 2))
	{
		nIndex = (int)Lua_ValueToNumber(L, 2);
	}
	else
	{
		nRetCode = THostWndItem_GetUserData(L, &pItemNext, 2);
		KGLOG_PROCESS_ERROR(nRetCode);
		ASSERT(pItemNext);
		KGLOG_PROCESS_ERROR(pParent == pItemNext->GetParent());
		nIndex = pParent->GetItemIndex(pItemNext);
	}

	pParent->ExchangeIndex(pParent->GetItemIndex(pItem), nIndex);

Exit0:
	return 0;
}

int KItemHandleClassScriptTable::LuaItemNull_GetTreePath(Lua_State *L)
{
	int nRetCode = false;
	KItemNull *pItem = NULL;
	LPCSTR pcszWnd = NULL;
    std::vector<std::string> aPath;
    std::string strItem;

    ASSERT(L);

	KG_PROCESS_ERROR(Lua_GetTopIndex(L) == 1);

	nRetCode = THostWndItem_GetUserData(L, &pItem);
	KGLOG_PROCESS_ERROR(nRetCode);
	ASSERT(pItem);

	pcszWnd = g_pUI->m_WndStation.GetTreePath(pItem->GetWndOwner());

	while (pItem->GetParent())
	{
        aPath.push_back(std::string(pItem->GetName()));
		pItem = pItem->GetParent();
	}

	if (!aPath.empty())
	{
        std::vector<std::string>::reverse_iterator it = aPath.rbegin();
		while (true)
		{
			strItem += *it++;

            if (it == aPath.rend())
				break;

			strItem += "/";
		}
	}

Exit0:
	if (!pcszWnd)
		pcszWnd = "\0";
	Lua_PushString(L, pcszWnd);
	Lua_PushString(L, strItem.c_str());
	return 2;
}

int KItemHandleClassScriptTable::LuaItemNull_SetAreaTestFile(Lua_State *L)
{
	int nRetCode = false;
	int nParam = 0;
	KItemNull *pItem = NULL;
	LPCSTR pcszFile = NULL;
	ASSERT(L);

	nParam = Lua_GetTopIndex(L);
	KG_PROCESS_ERROR(nParam > 0);

	nRetCode = THostWndItem_GetUserData(L, &pItem);
	KGLOG_PROCESS_ERROR(nRetCode);
	ASSERT(pItem);

	if (nParam > 1)
		pcszFile = Lua_ValueToString(L, 2);

	if (pcszFile)
		pcszFile = FormatFilePath(pcszFile);

	pItem->SetAreaTestFile(pcszFile);

Exit0:
	return 0;	
}

int KItemHandleClassScriptTable::LuaItemNull_SetIntPos(Lua_State *L)
{
	int nRetCode = false;
	int nResult = 0;
	int nIsIntPos = false;
	KItemNull *pItem = NULL;

	ASSERT(L);
	KGLOG_PROCESS_ERROR(Lua_GetTopIndex(L) == 2);

	nRetCode = THostWndItem_GetUserData(L, &pItem);
	KGLOG_PROCESS_ERROR(nRetCode);
	ASSERT(pItem);

	if (lua_isboolean(L, 2))
		nIsIntPos = Lua_ValueToBoolean(L, 2);
	else
		nIsIntPos = (int)Lua_ValueToNumber(L, 2);
	pItem->SetIntPos(nIsIntPos);

Exit0:
	return 0;
}

int KItemHandleClassScriptTable::LuaItemNull_IsIntPos(Lua_State *L)
{
	int nRetCode = false;
	int nResult = 0;
	int nIsIntPos = false;
	KItemNull *pItem = NULL;

	ASSERT(L);
	KGLOG_PROCESS_ERROR(Lua_GetTopIndex(L) == 1);

	nRetCode = THostWndItem_GetUserData(L, &pItem);
	KGLOG_PROCESS_ERROR(nRetCode);
	ASSERT(pItem);

	nIsIntPos = pItem->IsIntPos();

Exit0:
	lua_pushboolean(L, nIsIntPos);
	return 1;
}

int KItemHandleClassScriptTable::LuaItemNull_IsLink(Lua_State *L)
{
    int nRetCode = false;
    int nIsLink = false;
    KItemNull *pItem = NULL;

    ASSERT(L);
    KGLOG_PROCESS_ERROR(Lua_GetTopIndex(L) == 1);

    nRetCode = THostWndItem_GetUserData(L, &pItem);
    KGLOG_PROCESS_ERROR(nRetCode);
    ASSERT(pItem);

    nIsLink = pItem->IsLink();

Exit0:
    lua_pushboolean(L, nIsLink);
	return 1;
}

int KItemHandleClassScriptTable::LuaItemNull_GetLinkInfo(Lua_State *L)
{
    int nRetCode = false;
    LPCSTR szLinkInfo = "";
    KItemNull *pItem = NULL;
    
    ASSERT(L);
    KGLOG_PROCESS_ERROR(Lua_GetTopIndex(L) == 1);

    nRetCode = THostWndItem_GetUserData(L, &pItem);
    KGLOG_PROCESS_ERROR(nRetCode);
    ASSERT(pItem);

    szLinkInfo = pItem->GetLinkInfo();

Exit0:
    Lua_PushString(L, szLinkInfo);
    return 1;
}

int KItemHandleClassScriptTable::LuaItemNull_IsValid(Lua_State *L)
{
	int nResult = 0;
	int nIsValid = false;
	KItemNull *pItem = NULL;

	ASSERT(L);
	KGLOG_PROCESS_ERROR(Lua_GetTopIndex(L) == 1);

	if (THostWndItem_GetUserData(L, &pItem) && pItem)
		nIsValid = true;
Exit0:
	lua_pushboolean(L, nIsValid);
	return 1;
}

int KItemHandleClassScriptTable::LuaItemNull_NewIndex(Lua_State *L)
{
	int nTopIndex = lua_gettop(L);
	KItemNull *pItem = NULL;
	KGLOG_PROCESS_ERROR(nTopIndex == 3);

	lua_pushvalue(L, 1);
	lua_getfield(L, -1, STR_OBJ_KEY_NAME);
	pItem = reinterpret_cast<KItemNull *>(lua_touserdata(L, -1)); 
	KGLOG_PROCESS_ERROR(pItem);
	lua_settop(L, nTopIndex + 1);
	if (pItem->GetLuaRef() == LUA_NOREF)
		pItem->SetLuaRef(luaL_ref(L, LUA_REGISTRYINDEX));
	lua_settop(L, nTopIndex);
	lua_rawset(L, 1);
Exit0:
	lua_settop(L, nTopIndex);
	return 0;	
}

int KItemHandleClassScriptTable::LuaItemNull_Equal(Lua_State *L)
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

int KItemHandleClassScriptTable::LuaItemText_SetFontScheme(Lua_State *L)
{
	int nRetCode = false;
	int nFontScheme = 0;
	KItemText *pItemText = NULL;

	ASSERT(L);

	KG_PROCESS_ERROR(Lua_GetTopIndex(L) == 2);

	nRetCode = THostWndItem_GetUserData(L, &pItemText);
	KGLOG_PROCESS_ERROR(nRetCode);
	ASSERT(pItemText);

	nFontScheme = (int)Lua_ValueToNumber(L, 2);

	pItemText->SetFontScheme(nFontScheme);

Exit0:
	return 0;
}

int KItemHandleClassScriptTable::LuaItemText_GetFontScheme(Lua_State *L)
{
	int nRetCode = false;
	int nFontScheme = 0;
	KItemText *pItemText = NULL;

	ASSERT(L);

	KG_PROCESS_ERROR(Lua_GetTopIndex(L) == 1);

	nRetCode = THostWndItem_GetUserData(L, &pItemText);
	KGLOG_PROCESS_ERROR(nRetCode);
	ASSERT(pItemText);

	nFontScheme = pItemText->GetFontScheme();

Exit0:
	Lua_PushNumber(L, nFontScheme);
	return 1;
}

int KItemHandleClassScriptTable::LuaItemText_SetText(Lua_State *L)
{
	int nRetCode = false;
	LPCSTR szText = NULL;
	KItemText* pItemText = NULL;

    ASSERT(L);

	KG_PROCESS_ERROR(Lua_GetTopIndex(L) == 2);

	nRetCode = THostWndItem_GetUserData(L, &pItemText);
	KGLOG_PROCESS_ERROR(nRetCode);
	ASSERT(pItemText);

	szText = Lua_ValueToString(L, 2);
	if(szText)
		pItemText->SetText(ATL::CA2W(szText, GetCodePage()));
	else
		pItemText->SetText(L"");

	return 0;
Exit0:
	KGLogPrintf(KGLOG_ERR, "KGUI KItemHandleClassScriptTable::LuaItemText_SetText(%s)\n", 
        pItemText ? pItemText->GetName() : "");
	return 0;
}

int KItemHandleClassScriptTable::LuaItemText_GetText(Lua_State *L)
{
	int nRetCode = false;
	LPCWSTR wszText = NULL;
	KItemText* pItemText = NULL;

	ASSERT(L);

	KG_PROCESS_ERROR(Lua_GetTopIndex(L) == 1);

	nRetCode = THostWndItem_GetUserData(L, &pItemText);
	KGLOG_PROCESS_ERROR(nRetCode);
	ASSERT(pItemText);

	wszText = pItemText->GetText();

Exit0:
	Lua_PushString(L, ATL::CW2A(wszText, GetCodePage()));
	return 1;
}

int KItemHandleClassScriptTable::LuaItemText_GetTextLen(Lua_State *L)
{
	int nRetCode = false;
	int nTextLength = 0;
	KItemText *pItemText = NULL;

	ASSERT(L);

	KG_PROCESS_ERROR(Lua_GetTopIndex(L) == 1);

	nRetCode = THostWndItem_GetUserData(L, &pItemText);
	KGLOG_PROCESS_ERROR(nRetCode);
	ASSERT(pItemText);

	nTextLength = (int)pItemText->GetTextLength();

Exit0:
	Lua_PushNumber(L, nTextLength);
	return 1;
}

int KItemHandleClassScriptTable::LuaItemText_SetVAlign(Lua_State *L)
{
	int nRetCode = false;
	int nValign = 0;
	KItemText *pItemText = NULL;

	ASSERT(L);

	KG_PROCESS_ERROR(Lua_GetTopIndex(L) == 2);

	nRetCode = THostWndItem_GetUserData(L, &pItemText);
	KGLOG_PROCESS_ERROR(nRetCode);
	ASSERT(pItemText);

	nValign = (int)Lua_ValueToNumber(L, 2);
	pItemText->SetVAlign(nValign);

Exit0:
	return 0;
}

int KItemHandleClassScriptTable::LuaItemText_GetVAlign(Lua_State *L)
{
	int nRetCode = false;
	int nValign = 0;
	KItemText *pItemText = NULL;

	ASSERT(L);

	KG_PROCESS_ERROR(Lua_GetTopIndex(L) == 1);

	nRetCode = THostWndItem_GetUserData(L, &pItemText);
	KGLOG_PROCESS_ERROR(nRetCode);
	ASSERT(pItemText);

	nValign = pItemText->GetVAlign();

Exit0:
	Lua_PushNumber(L, nValign);
	return 1;
}

int KItemHandleClassScriptTable::LuaItemText_SetHAlign(Lua_State *L)
{
	int nRetCode = false;
	int nHalign = 0;
	KItemText *pItemText = NULL;

	ASSERT(L);

	KG_PROCESS_ERROR(Lua_GetTopIndex(L) == 2);

	nRetCode = THostWndItem_GetUserData(L, &pItemText);
	KGLOG_PROCESS_ERROR(nRetCode);
	ASSERT(pItemText);

	nHalign = (int)Lua_ValueToNumber(L, 2);
	pItemText->SetHAlign(nHalign);

Exit0:
	return 0;
}

int KItemHandleClassScriptTable::LuaItemText_GetHAlign(Lua_State *L)
{
	int nRetCode = false;
	int nHalign = 0;
	KItemText *pItemText = NULL;

	ASSERT(L);

	KG_PROCESS_ERROR(Lua_GetTopIndex(L) == 1);

	nRetCode = THostWndItem_GetUserData(L, &pItemText);
	KGLOG_PROCESS_ERROR(nRetCode);
	ASSERT(pItemText);

	nHalign = pItemText->GetHAlign();

Exit0:
	Lua_PushNumber(L, nHalign);
	return 1;
}

int KItemHandleClassScriptTable::LuaItemText_SetRowSpacing(Lua_State *L)
{
	int nRetCode = false;
	float fRowSpacing = 0.0f;
	KItemText *pItemText = NULL;

	ASSERT(L);

	KG_PROCESS_ERROR(Lua_GetTopIndex(L) == 2);

	nRetCode = THostWndItem_GetUserData(L, &pItemText);
	KGLOG_PROCESS_ERROR(nRetCode);
	ASSERT(pItemText);

	fRowSpacing = (float)Lua_ValueToNumber(L, 2);

	fRowSpacing *= g_pUI->m_WndStation.GetUIScale();

	pItemText->SetRowSpacing(fRowSpacing);

Exit0:
	return 0;
}

int KItemHandleClassScriptTable::LuaItemText_GetRowSpacing(Lua_State *L)
{
	int nRetCode = false;
	float fRowSpacing = 0.0f;
	KItemText *pItemText = NULL;

	ASSERT(L);

	KG_PROCESS_ERROR(Lua_GetTopIndex(L) == 1);

	nRetCode = THostWndItem_GetUserData(L, &pItemText);
	KGLOG_PROCESS_ERROR(nRetCode);
	ASSERT(pItemText);

	fRowSpacing = pItemText->GetRowSpacing();

	fRowSpacing /= g_pUI->m_WndStation.GetUIScale();

Exit0:
	Lua_PushNumber(L, fRowSpacing);
	return 1;
}

int KItemHandleClassScriptTable::LuaItemText_SetMultiLine(Lua_State *L)
{
	int nRetCode = false;
	int nMultiLine = false;
	KItemText *pItemText = NULL;

	ASSERT(L);

	KG_PROCESS_ERROR(Lua_GetTopIndex(L) == 2);

	nRetCode = THostWndItem_GetUserData(L, &pItemText);
	KGLOG_PROCESS_ERROR(nRetCode);
	ASSERT(pItemText);

	if (Lua_IsBoolean(L, 2))
		nMultiLine = (BOOL)Lua_ValueToBoolean(L, 2);
	else
		nMultiLine = (int)Lua_ValueToNumber(L, 2);

	pItemText->SetMultiLine(nMultiLine);

Exit0:
	return 0;
}

int KItemHandleClassScriptTable::LuaItemText_IsMultiLine(Lua_State *L)
{
	int nRetCode = false;
	int nMultiLine = false;
	KItemText *pItemText = NULL;

	ASSERT(L);

	KG_PROCESS_ERROR(Lua_GetTopIndex(L) == 1);

	nRetCode = THostWndItem_GetUserData(L, &pItemText);
	KGLOG_PROCESS_ERROR(nRetCode);
	ASSERT(pItemText);

	nMultiLine = pItemText->IsMultiLine();

Exit0:
	lua_pushboolean(L, nMultiLine);
	return 1;
}

int KItemHandleClassScriptTable::LuaItemText_FormatTextForDraw(Lua_State *L)
{
	int nRetCode = false;
	KItemText *pItemText = NULL;

	ASSERT(L);

	KG_PROCESS_ERROR(Lua_GetTopIndex(L) == 1);

	nRetCode = THostWndItem_GetUserData(L, &pItemText);
	KGLOG_PROCESS_ERROR(nRetCode);
	ASSERT(pItemText);

	pItemText->FormatTextForDraw();

Exit0:
	return 0;
}

int KItemHandleClassScriptTable::LuaItemText_AutoSize(Lua_State *L)
{
	int nRetCode = false;
	KItemText *pItemText = NULL;

	ASSERT(L);

	KG_PROCESS_ERROR(Lua_GetTopIndex(L) == 1);

	nRetCode = THostWndItem_GetUserData(L, &pItemText);
	KGLOG_PROCESS_ERROR(nRetCode);
	ASSERT(pItemText);

	pItemText->AutoSize();

Exit0:
	return 0;
}

int KItemHandleClassScriptTable::LuaItemText_SetFontScale(Lua_State *L)
{
	int nRetCode = false;
	float fScale = 1.0f;
	KItemText *pItemText = NULL;

	ASSERT(L);

	KG_PROCESS_ERROR(Lua_GetTopIndex(L) == 2);

	nRetCode = THostWndItem_GetUserData(L, &pItemText);
	KGLOG_PROCESS_ERROR(nRetCode);
	ASSERT(pItemText);

	fScale = (float)Lua_ValueToNumber(L, 2);

	pItemText->SetFontScale(fScale);

Exit0:
	return 0;
}

int KItemHandleClassScriptTable::LuaItemText_GetFontScale(Lua_State *L)
{
	int nRetCode = false;
	float fScale = 1.0f;
	KItemText *pItemText = NULL;

	ASSERT(L);

	KG_PROCESS_ERROR(Lua_GetTopIndex(L) == 1);

	nRetCode = THostWndItem_GetUserData(L, &pItemText);
	KGLOG_PROCESS_ERROR(nRetCode);
	ASSERT(pItemText);

	fScale = pItemText->GetFontScale();

Exit0:
	Lua_PushNumber(L, fScale);
	return 1;
}


int KItemHandleClassScriptTable::LuaItemText_SetCenterEachLine(Lua_State *L)
{
	int nRetCode = false;
	KItemText *pItemText = NULL;
	int nCenterEachLine = false;

	ASSERT(L);

	KG_PROCESS_ERROR(Lua_GetTopIndex(L) == 2);

	nRetCode = THostWndItem_GetUserData(L, &pItemText);
	KGLOG_PROCESS_ERROR(nRetCode);
	ASSERT(pItemText);

	if (Lua_IsBoolean(L, 2))
		nCenterEachLine = (int)Lua_ValueToBoolean(L, 2);
	else
		nCenterEachLine = (int)Lua_ValueToNumber(L, 2);

	pItemText->SetCenterEachLine(nCenterEachLine);

Exit0:
	return 0;
}

int KItemHandleClassScriptTable::LuaItemText_IsCenterEachLine(Lua_State *L)
{
	int nRetCode = false;
	KItemText *pItemText = NULL;
	int nCenterEachLine = false;

	ASSERT(L);

	KG_PROCESS_ERROR(Lua_GetTopIndex(L) == 1);

	nRetCode = THostWndItem_GetUserData(L, &pItemText);
	KGLOG_PROCESS_ERROR(nRetCode);
	ASSERT(pItemText);

	nCenterEachLine = pItemText->IsCenterEachLine();

Exit0:
	lua_pushboolean(L, nCenterEachLine);
	return 1;
}

int KItemHandleClassScriptTable::LuaItemText_SetFontSpacing(Lua_State *L)
{
	int nRetCode = false;
	KItemText *pItemText = NULL;
	float fSpacing = 0.0f;

	ASSERT(L);

	KG_PROCESS_ERROR(Lua_GetTopIndex(L) == 2);

	nRetCode = THostWndItem_GetUserData(L, &pItemText);
	KGLOG_PROCESS_ERROR(nRetCode);
	ASSERT(pItemText);

	fSpacing = (float)Lua_ValueToNumber(L, 2);

	fSpacing *= g_pUI->m_WndStation.GetUIScale();

	pItemText->SetFontSpacing(fSpacing);

Exit0:
	return 0;
}

int KItemHandleClassScriptTable::LuaItemText_GetFontSpacing(Lua_State *L)
{
	int nRetCode = false;
	KItemText *pItemText = NULL;
	float fSpacing = 0.0f;

	ASSERT(L);

	KG_PROCESS_ERROR(Lua_GetTopIndex(L) == 1);

	nRetCode = THostWndItem_GetUserData(L, &pItemText);
	KGLOG_PROCESS_ERROR(nRetCode);
	ASSERT(pItemText);

	fSpacing = pItemText->GetFontSpacing();

	fSpacing /= g_pUI->m_WndStation.GetUIScale();

Exit0:
	Lua_PushNumber(L, fSpacing);
	return 1;
}


int KItemHandleClassScriptTable::LuaItemText_SetRichText(Lua_State *L)
{
	int nRetCode = false;
	KItemText *pItemText = NULL;
	int nRichText = true;

	ASSERT(L);

	KG_PROCESS_ERROR(Lua_GetTopIndex(L) == 2);

	nRetCode = THostWndItem_GetUserData(L, &pItemText);
	KGLOG_PROCESS_ERROR(nRetCode);
	ASSERT(pItemText);

	if (Lua_IsBoolean(L, 2))
		nRichText = (int)Lua_ValueToBoolean(L, 2);
	else
		nRichText = (int)Lua_ValueToNumber(L, 2);

	pItemText->SetRichText(nRichText);

Exit0:
	return 0;
}

int KItemHandleClassScriptTable::LuaItemText_IsRichText(Lua_State *L)
{
	int nRetCode = false;
	KItemText *pItemText = NULL;
	int nRichText = true;

	ASSERT(L);

	KG_PROCESS_ERROR(Lua_GetTopIndex(L) == 1);

	nRetCode = THostWndItem_GetUserData(L, &pItemText);
	KGLOG_PROCESS_ERROR(nRetCode);
	ASSERT(pItemText);

	nRichText = pItemText->IsRichText();

Exit0:
	Lua_PushNumber(L, nRichText);
	return 1;
}

int KItemHandleClassScriptTable::LuaItemText_SetFontID(Lua_State *L)
{
	int nRetCode = false;
	KItemText *pItemText = NULL;
	DWORD dwFontID = 0;

	ASSERT(L);

	KG_PROCESS_ERROR(Lua_GetTopIndex(L) == 2);

	nRetCode = THostWndItem_GetUserData(L, &pItemText);
	KGLOG_PROCESS_ERROR(nRetCode);
	ASSERT(pItemText);

	dwFontID = (DWORD)Lua_ValueToNumber(L, 2);

	pItemText->SetFontID(dwFontID);

Exit0:
	return 0;
}

int KItemHandleClassScriptTable::LuaItemText_SetFontColor(Lua_State *L)
{
	int nRetCode = false;
    
    int nParemCount = 0;
	KItemText *pItemText = NULL;
	int r = 0;
	int g = 0;
	int b = 0;
	DWORD dwColor = 0xFF000000;

	ASSERT(L);

    nParemCount = lua_gettop(L);
	KG_PROCESS_ERROR(nParemCount == 2 || nParemCount == 4);

	nRetCode = THostWndItem_GetUserData(L, &pItemText);
	KGLOG_PROCESS_ERROR(nRetCode);
	ASSERT(pItemText);

    if (nParemCount == 2)
    {
        dwColor = (DWORD)(int)lua_tonumber(L, 2);
    }
    else
    {
        r = (int)Lua_ValueToNumber(L, 2);
        g = (int)Lua_ValueToNumber(L, 3);
        b = (int)Lua_ValueToNumber(L, 4);
        dwColor |= (r << 16);
        dwColor |= (g << 8);
        dwColor |= b; 
    }

	pItemText->SetFontColor(dwColor);

Exit0:
	return 0;
}

int KItemHandleClassScriptTable::LuaItemText_SetFontBoder(Lua_State *L)
{
	int nRetCode = false;
	KItemText *pItemText = NULL;
	WORD wSize = 0;
	int r = 0;
	int g = 0;
	int b = 0;
	DWORD dwColor = 0xFF000000;

	ASSERT(L);

	KG_PROCESS_ERROR(Lua_GetTopIndex(L) == 5);

	nRetCode = THostWndItem_GetUserData(L, &pItemText);
	KGLOG_PROCESS_ERROR(nRetCode);
	ASSERT(pItemText);

	wSize = (WORD)Lua_ValueToNumber(L, 2);
	r = (int)Lua_ValueToNumber(L, 3);
	g = (int)Lua_ValueToNumber(L, 4);
	b = (int)Lua_ValueToNumber(L, 5);
	dwColor |= (r << 16);
	dwColor |= (g << 8);
	dwColor |= b; 

	pItemText->SetFontBorder(wSize, dwColor);

Exit0:
	return 0;
}

int KItemHandleClassScriptTable::LuaItemText_SetFontShadow(Lua_State *L)
{
	int nRetCode = false;
	KItemText *pItemText = NULL;
	WORD wSize = 0;
	int r = 0;
	int g = 0;
	int b = 0;
	DWORD dwColor = 0xFF000000;

	ASSERT(L);

	KG_PROCESS_ERROR(Lua_GetTopIndex(L) == 5);

	nRetCode = THostWndItem_GetUserData(L, &pItemText);
	KGLOG_PROCESS_ERROR(nRetCode);
	ASSERT(pItemText);

	wSize = (WORD)Lua_ValueToNumber(L, 2);
	r = (int)Lua_ValueToNumber(L, 3);
	g = (int)Lua_ValueToNumber(L, 4);
	b = (int)Lua_ValueToNumber(L, 5);
	dwColor |= (r << 16);
	dwColor |= (g << 8);
	dwColor |= b; 

	pItemText->SetFontShadow(wSize, dwColor);

Exit0:
	return 0;
}

int KItemHandleClassScriptTable::LuaItemText_GetFontID(Lua_State *L)
{
	int nRetCode = false;
	KItemText *pItemText = NULL;
	DWORD dwFontID = 0;

	ASSERT(L);

	KG_PROCESS_ERROR(Lua_GetTopIndex(L) == 1);

	nRetCode = THostWndItem_GetUserData(L, &pItemText);
	KGLOG_PROCESS_ERROR(nRetCode);
	ASSERT(pItemText);

	dwFontID = pItemText->GetFontID();

Exit0:
	Lua_PushNumber(L, dwFontID);
	return 1;
}

int KItemHandleClassScriptTable::LuaItemText_GetFontColor(Lua_State *L)
{
	int nRetCode = false;
	KItemText *pItemText = NULL;
	DWORD dwColor = 0;

	ASSERT(L);

	KG_PROCESS_ERROR(Lua_GetTopIndex(L) == 1);

	nRetCode = THostWndItem_GetUserData(L, &pItemText);
	KGLOG_PROCESS_ERROR(nRetCode);
	ASSERT(pItemText);

	dwColor = pItemText->GetFontColor();

Exit0:
	Lua_PushNumber(L, ((dwColor >> 16) & 0x000000FF));
	Lua_PushNumber(L, ((dwColor >> 8) & 0x000000FF));
	Lua_PushNumber(L, (dwColor & 0x000000FF));
	return 3;
}

int KItemHandleClassScriptTable::LuaItemText_GetFontBoder(Lua_State *L)
{
	int nRetCode = false;
	KItemText *pItemText = NULL;
	DWORD dwColor = 0;
	WORD wSize = 0;

	ASSERT(L);

	KG_PROCESS_ERROR(Lua_GetTopIndex(L) == 1);

	nRetCode = THostWndItem_GetUserData(L, &pItemText);
	KGLOG_PROCESS_ERROR(nRetCode);
	ASSERT(pItemText);

	pItemText->GetFontBoder(wSize, dwColor);

Exit0:
	Lua_PushNumber(L, wSize);
	Lua_PushNumber(L, ((dwColor >> 16) & 0x000000FF));
	Lua_PushNumber(L, ((dwColor >> 8) & 0x000000FF));
	Lua_PushNumber(L, (dwColor & 0x000000FF));
	return 4;
}

int KItemHandleClassScriptTable::LuaItemText_GetFontProjection(Lua_State *L)
{
	int nRetCode = false;
	KItemText *pItemText = NULL;
	DWORD dwColor = 0;
	WORD wSize = 0;

	ASSERT(L);

	KG_PROCESS_ERROR(Lua_GetTopIndex(L) == 1);

	nRetCode = THostWndItem_GetUserData(L, &pItemText);
	KGLOG_PROCESS_ERROR(nRetCode);
	ASSERT(pItemText);

	pItemText->GetFontProjection(wSize, dwColor);

Exit0:
	Lua_PushNumber(L, wSize);
	Lua_PushNumber(L, ((dwColor >> 16) & 0x000000FF));
	Lua_PushNumber(L, ((dwColor >> 8) & 0x000000FF));
	Lua_PushNumber(L, (dwColor & 0x000000FF));
	return 4;
}

int KItemHandleClassScriptTable::LuaItemText_GetTextExtent(Lua_State *L)
{
	int nRetCode = false;
	int nParamCount = 0;
	int nTextLength = -1;
	float fWidth = 0.0f;
	float fHeight = 0.0f;
	float fScale = 1.0f;
	KItemText *pItemText = NULL;

	ASSERT(L);

	nParamCount = Lua_GetTopIndex(L);
	KGLOG_PROCESS_ERROR(nParamCount == 1 || nParamCount == 2);

	nRetCode = THostWndItem_GetUserData(L, &pItemText);
	KGLOG_PROCESS_ERROR(nRetCode);
	ASSERT(pItemText);

	if (nParamCount == 2)
		nTextLength = (int)Lua_ValueToNumber(L, 2);

	pItemText->GetTextExtent(nTextLength, fWidth, fHeight);

	fScale = g_pUI->m_WndStation.GetUIScale();

	fWidth /= fScale;
	fHeight /= fScale;

Exit0:
	Lua_PushNumber(L, fWidth);
	Lua_PushNumber(L, fHeight);
	return 2;
}

int KItemHandleClassScriptTable::LuaItemText_GetTextPosExtent(Lua_State *L)
{
	int nRetCode = false;
	int nParamCount = 0;
	int nTextLength = -1;
	float fWidth = 0.0f;
	float fHeight = 0.0f;
	float fScale = 1.0f;
	KItemText *pItemText = NULL;

	ASSERT(L);

	nParamCount = Lua_GetTopIndex(L);
	KGLOG_PROCESS_ERROR(nParamCount == 1 || nParamCount == 2);

	nRetCode = THostWndItem_GetUserData(L, &pItemText);
	KGLOG_PROCESS_ERROR(nRetCode);
	ASSERT(pItemText);

	fScale = g_pUI->m_WndStation.GetUIScale();

	pItemText->GetSize(fWidth, fHeight);
	if (nParamCount == 2)
		fWidth = (float)Lua_ValueToNumber(L, 2) * fScale;

	pItemText->GetTextPosExtent(fWidth, nTextLength);

Exit0:
	Lua_PushNumber(L, nTextLength);
	return 1;
}

int KItemHandleClassScriptTable::LuaItemImage_SetFrame(Lua_State *L)
{
	int nRetCode = false;
	int nFrame = -1;
	KItemImage *pItemImage = NULL;

	ASSERT(L);

	KG_PROCESS_ERROR(Lua_GetTopIndex(L) == 2);

	nRetCode = THostWndItem_GetUserData(L, &pItemImage);
	KGLOG_PROCESS_ERROR(nRetCode);
	ASSERT(pItemImage);

	nFrame = (int)Lua_ValueToNumber(L, 2);

	pItemImage->SetFrame(nFrame);

Exit0:
	return 0;
}

int KItemHandleClassScriptTable::LuaItemImage_GetFrame(Lua_State *L)
{
	int nRetCode = false;
	int nFrame = -1;
	KItemImage *pItemImage = NULL;

	ASSERT(L);

	KG_PROCESS_ERROR(Lua_GetTopIndex(L) == 1);

	nRetCode = THostWndItem_GetUserData(L, &pItemImage);
	KGLOG_PROCESS_ERROR(nRetCode);
	ASSERT(pItemImage);

	nFrame = pItemImage->GetFrame();

Exit0:
	Lua_PushNumber(L, nFrame);
	return 1;
}

int KItemHandleClassScriptTable::LuaItemImage_AutoSize(Lua_State *L)
{
	int nRetCode = false;
	KItemImage *pItemImage = NULL;

	ASSERT(L);

	KG_PROCESS_ERROR(Lua_GetTopIndex(L) == 1);

	nRetCode = THostWndItem_GetUserData(L, &pItemImage);
	KGLOG_PROCESS_ERROR(nRetCode);
	ASSERT(pItemImage);

	pItemImage->AutoSize();

Exit0:
	return 0;
}

int KItemHandleClassScriptTable::LuaItemImage_SetImageType(Lua_State *L)
{
	int nRetCode = false;
	KItemImage *pItemImage = NULL;

	ASSERT(L);

	KG_PROCESS_ERROR(Lua_GetTopIndex(L) == 2);

	nRetCode = THostWndItem_GetUserData(L, &pItemImage);
	KGLOG_PROCESS_ERROR(nRetCode);
	ASSERT(pItemImage);

	nRetCode = (int)Lua_ValueToNumber(L, 2);
	pItemImage->SetImageType(nRetCode);

Exit0:
	return 0;
}

int KItemHandleClassScriptTable::LuaItemImage_GetImageType(Lua_State *L)
{
	int nRetCode = false;
	int nType = 0;
	KItemImage *pItemImage = NULL;

	ASSERT(L);

	KG_PROCESS_ERROR(Lua_GetTopIndex(L) == 1);

	nRetCode = THostWndItem_GetUserData(L, &pItemImage);
	KGLOG_PROCESS_ERROR(nRetCode);
	ASSERT(pItemImage);

	nType = pItemImage->GetImageType();
Exit0:
	Lua_PushNumber(L, nType);
	return 1;
}

int KItemHandleClassScriptTable::LuaItemImage_SetPercentage(Lua_State *L)
{
	int nRetCode = false;
	float fPercentage = 1.0f;
	KItemImage *pItemImage = NULL;

	ASSERT(L);

	KG_PROCESS_ERROR(Lua_GetTopIndex(L) == 2);

	nRetCode = THostWndItem_GetUserData(L, &pItemImage);
	KGLOG_PROCESS_ERROR(nRetCode);
	ASSERT(pItemImage);

	fPercentage = (float)Lua_ValueToNumber(L, 2);
	KG_PROCESS_ERROR(!(fPercentage != fPercentage)); //not a number.
	pItemImage->SetPercentage(fPercentage);

Exit0:
	return 0;
}

int KItemHandleClassScriptTable::LuaItemImage_GetPercentage(Lua_State *L)
{
	int nRetCode = false;
	float fPercentage = 1.0f;
	KItemImage *pItemImage = NULL;

	ASSERT(L);

	KG_PROCESS_ERROR(Lua_GetTopIndex(L) == 1);

	nRetCode = THostWndItem_GetUserData(L, &pItemImage);
	KGLOG_PROCESS_ERROR(nRetCode);
	ASSERT(pItemImage);

	fPercentage = pItemImage->GetPercentage();
Exit0:
	Lua_PushNumber(L, fPercentage);
	return 1;
}

int KItemHandleClassScriptTable::LuaItemImage_SetRotate(Lua_State *L)
{
	int nRetCode = false;
	float fRotate = 0.0f;
	KItemImage *pItemImage = NULL;

	ASSERT(L);

	KG_PROCESS_ERROR(Lua_GetTopIndex(L) == 2);

	nRetCode = THostWndItem_GetUserData(L, &pItemImage);
	KGLOG_PROCESS_ERROR(nRetCode);
	ASSERT(pItemImage);

	fRotate = (float)Lua_ValueToNumber(L, 2);
	pItemImage->SetRotate(fRotate);

Exit0:
	return 0;
}

int KItemHandleClassScriptTable::LuaItemImage_GetRotate(Lua_State *L)
{
	int nRetCode = false;
	float fRotate = 0.0f;
	KItemImage *pItemImage = NULL;

	ASSERT(L);

	KG_PROCESS_ERROR(Lua_GetTopIndex(L) == 1);

	nRetCode = THostWndItem_GetUserData(L, &pItemImage);
	KGLOG_PROCESS_ERROR(nRetCode);
	ASSERT(pItemImage);

	fRotate = pItemImage->GetRotate();
Exit0:
	Lua_PushNumber(L, fRotate);
	return 1;
}

int KItemHandleClassScriptTable::LuaItemImage_GetImageID(Lua_State *L)
{
	int nRetCode = false;
	DWORD dwImageID = 0;
	KItemImage *pItemImage = NULL;

	ASSERT(L);

	KG_PROCESS_ERROR(Lua_GetTopIndex(L) == 1);

	nRetCode = THostWndItem_GetUserData(L, &pItemImage);
	KGLOG_PROCESS_ERROR(nRetCode);
	ASSERT(pItemImage);

	dwImageID = pItemImage->GetImageID();
Exit0:
	Lua_PushNumber(L, dwImageID);
	return 1;
}

int KItemHandleClassScriptTable::LuaItemImage_FromUITex(Lua_State *L)
{
	int nRetCode = false;
	LPCSTR pcszImage = NULL;
	int nFrame = -1;
	KItemImage *pItemImage = NULL;

	ASSERT(L);

	KG_PROCESS_ERROR(Lua_GetTopIndex(L) == 3);

	nRetCode = THostWndItem_GetUserData(L, &pItemImage);
	KGLOG_PROCESS_ERROR(nRetCode);
	ASSERT(pItemImage);

	pcszImage = Lua_ValueToString(L, 2);
	pcszImage = FormatFilePath(pcszImage);
	KG_PROCESS_ERROR(pcszImage);

	nFrame = (int)Lua_ValueToNumber(L, 3);

    pItemImage->FromUITex(pcszImage, nFrame);

Exit0:
	return 0;
}

int KItemHandleClassScriptTable::LuaItemImage_FromTextureFile(Lua_State *L)
{
	int nRetCode = false;
	LPCSTR pcszImage = NULL;
	D3DXVECTOR2 vMin = D3DXVECTOR2(0.0f, 0.0f);
	D3DXVECTOR2 vMax = D3DXVECTOR2(1.0f, 1.0f);

	KItemImage *pItemImage = NULL;
	int nParamCount = 0;

	ASSERT(L);

	nParamCount = Lua_GetTopIndex(L);
	KG_PROCESS_ERROR(nParamCount > 1);

	nRetCode = THostWndItem_GetUserData(L, &pItemImage);
	KGLOG_PROCESS_ERROR(nRetCode);
	ASSERT(pItemImage);

	pcszImage = Lua_ValueToString(L, 2);
	pcszImage = FormatFilePath(pcszImage);
	KG_PROCESS_ERROR(pcszImage);

	if (nParamCount > 2)
		vMin.x = (FLOAT)Lua_ValueToNumber(L, 3);
	if (nParamCount > 3)
		vMin.y = (FLOAT)Lua_ValueToNumber(L, 4);
	if (nParamCount > 4)
		vMax.x = (FLOAT)Lua_ValueToNumber(L, 5);
	if (nParamCount > 5)
		vMax.y = (FLOAT)Lua_ValueToNumber(L, 6);

	pItemImage->FromTextureFile(pcszImage, vMin, vMax);

Exit0:
	return 0;
}

int KItemHandleClassScriptTable::LuaItemImage_FromScene(Lua_State *L)
{
	int nRetCode = false;
	IKG3DScene** pp3DScene = NULL;
	IKG3DScene* p3DScene = NULL;
	KItemImage *pItemImage = NULL;

	ASSERT(L);

	KG_PROCESS_ERROR(Lua_GetTopIndex(L) == 2);

	nRetCode = THostWndItem_GetUserData(L, &pItemImage);
	KGLOG_PROCESS_ERROR(nRetCode);
	ASSERT(pItemImage);

	pp3DScene = (IKG3DScene**)lua_touserdata(L, 2);
	KGLOG_PROCESS_ERROR(pp3DScene);

	p3DScene = *pp3DScene;
	KGLOG_PROCESS_ERROR(p3DScene);

	pItemImage->FromScene(p3DScene);

Exit0:
	return 0;
}

int KItemHandleClassScriptTable::LuaItemImage_FromImageID(Lua_State *L)
{
	int nRetCode = false;
	DWORD dwImageID = 0;
	KItemImage *pItemImage = NULL;

	ASSERT(L);

	KG_PROCESS_ERROR(Lua_GetTopIndex(L) == 2);

	nRetCode = THostWndItem_GetUserData(L, &pItemImage);
	KGLOG_PROCESS_ERROR(nRetCode);
	ASSERT(pItemImage);

	dwImageID = (DWORD)Lua_ValueToNumber(L, 2);

	pItemImage->FromImageID(dwImageID);

Exit0:
	return 0;
}

int KItemHandleClassScriptTable::LuaItemImage_FromIconID(Lua_State *L)
{
	int nRetCode = false;
	int nIcon = -1;
	KItemImage *pItemImage = NULL;

	ASSERT(L);

	KG_PROCESS_ERROR(Lua_GetTopIndex(L) == 2);

	nRetCode = THostWndItem_GetUserData(L, &pItemImage);
	KGLOG_PROCESS_ERROR(nRetCode);
	ASSERT(pItemImage);

	nIcon = (int)Lua_ValueToNumber(L, 2);

	pItemImage->FromIconID(nIcon);

Exit0:
	return 0;
}

int KItemHandleClassScriptTable::LuaItemImage_ToManagedImage(Lua_State *L)
{
	int nRetCode = false;
	KItemImage *pItemImage = NULL;

	ASSERT(L);

	KG_PROCESS_ERROR(Lua_GetTopIndex(L) == 1);

	nRetCode = THostWndItem_GetUserData(L, &pItemImage);
	KGLOG_PROCESS_ERROR(nRetCode);
	ASSERT(pItemImage);

	pItemImage->ToManagedImage();

Exit0:
	return 0;
}

int KItemHandleClassScriptTable::LuaItemShadow_SetShadowColor(Lua_State *L)
{
	int nRetCode = false;
	int nParamCount = 0;
	DWORD dwColor = 0x00000000;
	int nAlpha = 255;
	KItemShadow *pItemShadow = NULL;

	ASSERT(L);

	nParamCount = Lua_GetTopIndex(L);
	KGLOG_PROCESS_ERROR(nParamCount > 1);

	nRetCode = THostWndItem_GetUserData(L, &pItemShadow);
	KGLOG_PROCESS_ERROR(nRetCode);
	ASSERT(pItemShadow);

	switch (nParamCount)
	{
	case 2:
		if (lua_type(L, 2) == LUA_TNUMBER)
			dwColor = (DWORD)Lua_ValueToNumber(L, 2);
		else
		{
			LPCSTR pcszColor = Lua_ValueToString(L, 2);
            int nColorScheme = g_pUI->m_ColorSchemeMgr.FindScheme(ATL::CA2W(pcszColor, GetCodePage()));
            if (g_pUI->m_ColorSchemeMgr.IsExistColor(nColorScheme))
			    dwColor = g_pUI->m_ColorSchemeMgr.GetColor(nColorScheme);
			dwColor = AddAlpha(dwColor, nAlpha);
		}
		break;
	case 3:
		{
			LPCSTR pcszColor = Lua_ValueToString(L, 2);
            int nColorScheme = g_pUI->m_ColorSchemeMgr.FindScheme(ATL::CA2W(pcszColor, GetCodePage()));
            if (g_pUI->m_ColorSchemeMgr.IsExistColor(nColorScheme))
			    dwColor = g_pUI->m_ColorSchemeMgr.GetColor(nColorScheme);
			nAlpha = (DWORD)Lua_ValueToNumber(L, 3);
			dwColor = AddAlpha(dwColor, nAlpha);
		}		
		break;
	default:
		goto Exit0;
	}

	pItemShadow->SetShadowColor(dwColor);

Exit0:
	return 0;
}

int KItemHandleClassScriptTable::LuaItemShadow_GetShadowColor(Lua_State *L)
{
	int nRetCode = false;
	int nParamCount = 0;
	DWORD dwColor = 0xFFFFFFFF;
	KItemShadow *pItemShadow = NULL;

	ASSERT(L);

	nParamCount = Lua_GetTopIndex(L);
	KGLOG_PROCESS_ERROR(nParamCount ==  1);
	nRetCode = THostWndItem_GetUserData(L, &pItemShadow);
	KGLOG_PROCESS_ERROR(nRetCode);
	ASSERT(pItemShadow);

	dwColor = pItemShadow->GetshadowColor();

Exit0:
	Lua_PushNumber(L, dwColor);
	return 1;
}

int KItemHandleClassScriptTable::LuaItemShadow_SetColorRGB(Lua_State *L)
{
	int nRetCode = false;
	int nParamCount = 0;
	DWORD dwColor = 0xFFFFFFFF;
	KItemShadow *pItemShadow = NULL;
	DWORD dwR = 0;
	DWORD dwG = 0;
	DWORD dwB = 0;
	DWORD dwA = 0xFF;

	ASSERT(L);

	nParamCount = Lua_GetTopIndex(L);
	KGLOG_PROCESS_ERROR(nParamCount == 4);
	nRetCode = THostWndItem_GetUserData(L, &pItemShadow);
	KGLOG_PROCESS_ERROR(nRetCode);
	ASSERT(pItemShadow);

	dwR = (DWORD)Lua_ValueToNumber(L, 2);
	dwG = (DWORD)Lua_ValueToNumber(L, 3);
	dwB = (DWORD)Lua_ValueToNumber(L, 4);
	dwA = pItemShadow->GetAlpha();
	dwColor = ((dwA & 0xFF) << 24) | ((dwR & 0xFF) << 16) | ((dwG & 0xFF) << 8) | (dwB & 0xFF);

	pItemShadow->SetShadowColor(dwColor);

Exit0:
	return 0;
}

int KItemHandleClassScriptTable::LuaItemShadow_GetColorRGB(Lua_State *L)
{
	int nRetCode = false;
	int nParamCount = 0;
	DWORD dwColor = 0xFFFFFFFF;
	KItemShadow *pItemShadow = NULL;
	DWORD dwR = 0x00;
	DWORD dwG = 0x00;
	DWORD dwB = 0x00;

	ASSERT(L);

	nParamCount = Lua_GetTopIndex(L);
	KGLOG_PROCESS_ERROR(nParamCount ==  1);
	nRetCode = THostWndItem_GetUserData(L, &pItemShadow);
	KGLOG_PROCESS_ERROR(nRetCode);
	ASSERT(pItemShadow);

	dwColor = pItemShadow->GetshadowColor();
	dwR = (dwColor & 0x00FF0000) >> 16;
	dwG = (dwColor & 0x0000FF00) >> 8;
	dwB = dwColor & 0x000000FF;

Exit0:
	Lua_PushNumber(L, dwR);
	Lua_PushNumber(L, dwG);
	Lua_PushNumber(L, dwB);
	return 3;
}

int KItemHandleClassScriptTable::LuaItemShadow_SetTriangleFan(Lua_State *L)
{
	int nRetCode = false;
	int nParamCount = 0;
	int nTriangleFan = false;
	KItemShadow *pItemShadow = NULL;

	ASSERT(L);

	nParamCount = Lua_GetTopIndex(L);
	KGLOG_PROCESS_ERROR(nParamCount == 2);
	nRetCode = THostWndItem_GetUserData(L, &pItemShadow);
	KGLOG_PROCESS_ERROR(nRetCode);
	ASSERT(pItemShadow);

	if (Lua_IsBoolean(L, 2))
		nTriangleFan = Lua_ValueToBoolean(L, 2);
	else
		nTriangleFan = (int)Lua_ValueToNumber(L, 2);
	pItemShadow->SetTriangleFan(nTriangleFan);
Exit0:
	return 0;
}

int KItemHandleClassScriptTable::LuaItemShadow_IsTriangleFan(Lua_State *L)
{
	int nRetCode = false;
	int nParamCount = 0;
	int nTriangleFan = false;
	KItemShadow *pItemShadow = NULL;

	ASSERT(L);

	nParamCount = Lua_GetTopIndex(L);
	KGLOG_PROCESS_ERROR(nParamCount == 1);
	nRetCode = THostWndItem_GetUserData(L, &pItemShadow);
	KGLOG_PROCESS_ERROR(nRetCode);
	ASSERT(pItemShadow);

	nTriangleFan = pItemShadow->IsTriangleFan();

Exit0:
	lua_pushboolean(L, nTriangleFan);
	return 1;
}

int KItemHandleClassScriptTable::LuaItemShadow_AppendTriangleFanPoint(Lua_State *L)
{
	int nRetCode = false;
	int nParamCount = 0;
	float fX = 0;
	float fY = 0;
	DWORD dwR = 0;
	DWORD dwG = 0;
	DWORD dwB = 0;
	DWORD dwA = 0;
	DWORD dwColor = 0;
	float fScale = g_pUI->m_WndStation.GetUIScale();
	KItemShadow *pItemShadow = NULL;

	ASSERT(L);

	nParamCount = Lua_GetTopIndex(L);
	KGLOG_PROCESS_ERROR(nParamCount == 7);
	nRetCode = THostWndItem_GetUserData(L, &pItemShadow);
	KGLOG_PROCESS_ERROR(nRetCode);
	ASSERT(pItemShadow);

	fX = (float)Lua_ValueToNumber(L, 2) * fScale;
	fY = (float)Lua_ValueToNumber(L, 3) * fScale;
	dwR = (DWORD)Lua_ValueToNumber(L, 4);
	dwG = (DWORD)Lua_ValueToNumber(L, 5);
	dwB = (DWORD)Lua_ValueToNumber(L, 6);
	dwA = (DWORD)Lua_ValueToNumber(L, 7);
	dwColor = ((dwA & 0xFF) << 24) | ((dwR & 0xFF) << 16) | ((dwG & 0xFF) << 8) | (dwB & 0xFF);

	pItemShadow->AppendTriangleFanPoint(fX, fY, dwColor);

Exit0:
	return 0;
}

int KItemHandleClassScriptTable::LuaItemShadow_ClearTriangleFanPoint(Lua_State *L)
{
	int nRetCode = false;
	int nParamCount = 0;
	KItemShadow *pItemShadow = NULL;

	ASSERT(L);

	nParamCount = Lua_GetTopIndex(L);
	KGLOG_PROCESS_ERROR(nParamCount == 1);
	nRetCode = THostWndItem_GetUserData(L, &pItemShadow);
	KGLOG_PROCESS_ERROR(nRetCode);
	ASSERT(pItemShadow);

	pItemShadow->ClearTriangleFanPoint();

Exit0:
	return 0;
}

int KItemHandleClassScriptTable::LuaItemAnimate_SetGroup(Lua_State *L)
{
	int nRetCode = false;
	int nGroup = -1;
	KItemAnimate *pItemAnimate = NULL;

	ASSERT(L);

	KG_PROCESS_ERROR(Lua_GetTopIndex(L) == 2);

	nRetCode = THostWndItem_GetUserData(L, &pItemAnimate);
	KGLOG_PROCESS_ERROR(nRetCode);
	ASSERT(pItemAnimate);

	nGroup = (int)Lua_ValueToNumber(L, 2);
	if (nGroup < 0)
		nGroup = -1;

	pItemAnimate->SetGroup(nGroup);

Exit0:
	return 0;
}

int KItemHandleClassScriptTable::LuaItemAnimate_SetLoopCount(Lua_State *L)
{
	int nRetCode = false;
	int nLoopCount = -1;
	KItemAnimate *pItemAnimate = NULL;

	ASSERT(L);

	KG_PROCESS_ERROR(Lua_GetTopIndex(L) == 2);

	nRetCode = THostWndItem_GetUserData(L, &pItemAnimate);
	KGLOG_PROCESS_ERROR(nRetCode);
	ASSERT(pItemAnimate);

	nLoopCount = (int)Lua_ValueToNumber(L, 2);

	pItemAnimate->SetLoopCount(nLoopCount);

Exit0:
	return 0;
}

int KItemHandleClassScriptTable::LuaItemAnimate_SetImagePath(Lua_State *L)
{
	int nRetCode = false;
	LPCSTR pcszImage = NULL;
	KItemAnimate *pItemAnimate = NULL;

	ASSERT(L);

	KG_PROCESS_ERROR(Lua_GetTopIndex(L) == 2);

	nRetCode = THostWndItem_GetUserData(L, &pItemAnimate);
	KGLOG_PROCESS_ERROR(nRetCode);
	ASSERT(pItemAnimate);

	pcszImage = lua_tostring(L, 2);
	KGLOG_PROCESS_ERROR(pcszImage);

	pItemAnimate->SetImagePath(FormatFilePath(pcszImage));

Exit0:
	return 0;
}

int KItemHandleClassScriptTable::LuaItemAnimate_SetAnimate(Lua_State *L)
{
	int nRetCode = false;
	int nGroup = -1;
	int nLoopCount = -1;
	LPCSTR pcszImage = NULL;
	KItemAnimate *pItemAnimate = NULL;
	int nParameterNumber = 0;

	ASSERT(L);

	nParameterNumber = Lua_GetTopIndex(L);
	KG_PROCESS_ERROR(nParameterNumber == 3 || nParameterNumber == 4);

	nRetCode = THostWndItem_GetUserData(L, &pItemAnimate);
	KGLOG_PROCESS_ERROR(nRetCode);
	ASSERT(pItemAnimate);

	pcszImage = Lua_ValueToString(L, 2);
	pcszImage = FormatFilePath(pcszImage);
	KGLOG_PROCESS_ERROR(pcszImage);

	nGroup = (int)Lua_ValueToNumber(L, 3);
	if (nParameterNumber == 4)
		nLoopCount = (int)Lua_ValueToNumber(L, 4);

    pItemAnimate->SetAnimate(pcszImage, nGroup, nLoopCount);

Exit0:
	return 0;
}

int KItemHandleClassScriptTable::LuaItemAnimate_AutoSize(Lua_State *L)
{
	int nRetCode = false;
	KItemAnimate *pItemAnimate = NULL;

	ASSERT(L);

	KG_PROCESS_ERROR(Lua_GetTopIndex(L) == 1);

	nRetCode = THostWndItem_GetUserData(L, &pItemAnimate);
	KGLOG_PROCESS_ERROR(nRetCode);
	ASSERT(pItemAnimate);

	pItemAnimate->AutoSize();

Exit0:
	return 0;
}

int KItemHandleClassScriptTable::LuaItemAnimate_Replay(Lua_State *L)
{
    int nRetCode = false;
    KItemAnimate *pItemAnimate = NULL;

    ASSERT(L);

    KG_PROCESS_ERROR(Lua_GetTopIndex(L) == 1);

    nRetCode = THostWndItem_GetUserData(L, &pItemAnimate);
    KGLOG_PROCESS_ERROR(nRetCode);
    ASSERT(pItemAnimate);

    pItemAnimate->Replay();

Exit0:
    return 0;
}

int KItemHandleClassScriptTable::LuaItemScene_SetScene(Lua_State *L)
{
	int nRetCode = false;
	IKG3DScene** pp3DScene = NULL;
	IKG3DScene* p3DScene = NULL;
	KItemScene* pItem = NULL;

	ASSERT(L);

	KG_PROCESS_ERROR(Lua_GetTopIndex(L) == 2);

	nRetCode = THostWndItem_GetUserData(L, &pItem);
	KGLOG_PROCESS_ERROR(nRetCode);
	ASSERT(pItem);

	pp3DScene = (IKG3DScene**)lua_touserdata(L, 2);
	if (pp3DScene)
		p3DScene = *pp3DScene;

	ASSERT(pItem);
	pItem->SetScene(p3DScene);

Exit0:
	return 0;
}

int KItemHandleClassScriptTable::LuaItemScene_EnableRenderTerrain(Lua_State *L)
{
	int nRetCode = false;
	KItemScene *pItem = NULL;
	int nEnable = false;

	ASSERT(L);

	KG_PROCESS_ERROR(Lua_GetTopIndex(L) == 2);

	nRetCode = THostWndItem_GetUserData(L, &pItem);
	KGLOG_PROCESS_ERROR(nRetCode);
	ASSERT(pItem);

	if (Lua_IsBoolean(L, 2))
	{
		nEnable = (int)Lua_ValueToBoolean(L, 2);
	}
	else
	{
		nEnable = (int)Lua_ValueToNumber(L, 2);
	}

	pItem->EnableRenderTerrain(nEnable);

Exit0:
	return 0;
}

int KItemHandleClassScriptTable::LuaItemScene_EnableRenderSkyBox(Lua_State *L)
{
	int nRetCode = false;
	KItemScene *pItem = NULL;
	int nEnable = false;

	ASSERT(L);

	KG_PROCESS_ERROR(Lua_GetTopIndex(L) == 2);

	nRetCode = THostWndItem_GetUserData(L, &pItem);
	KGLOG_PROCESS_ERROR(nRetCode);
	ASSERT(pItem);

	if (Lua_IsBoolean(L, 2))
	{
		nEnable = (int)Lua_ValueToBoolean(L, 2);
	}
	else
	{
		nEnable = (int)Lua_ValueToNumber(L, 2);
	}

	pItem->EnableRenderSkyBox(nEnable);

Exit0:
	return 0;
}

int KItemHandleClassScriptTable::LuaItemScene_EnableAlpha(Lua_State *L)
{
	int nRetCode = false;
	KItemScene *pItem = NULL;
	int nEnable = false;

	ASSERT(L);

	KG_PROCESS_ERROR(Lua_GetTopIndex(L) == 2);

	nRetCode = THostWndItem_GetUserData(L, &pItem);
	KGLOG_PROCESS_ERROR(nRetCode);
	ASSERT(pItem);

	if (Lua_IsBoolean(L, 2))
	{
		nEnable = (int)Lua_ValueToBoolean(L, 2);
	}
	else
	{
		nEnable = (int)Lua_ValueToNumber(L, 2);
	}

	pItem->EnableAlpha(nEnable);

Exit0:
	return 0;
}


int KItemHandleClassScriptTable::LuaItemBox_SetIndex(Lua_State *L)
{
	int nRetCode = false;
	int nIndex = -1;
	KItemBox *pItemBox = NULL;

	ASSERT(L);

	KG_PROCESS_ERROR(Lua_GetTopIndex(L) == 2);

	nRetCode = THostWndItem_GetUserData(L, &pItemBox);
	KGLOG_PROCESS_ERROR(nRetCode);
	ASSERT(pItemBox);

	nIndex = (int)Lua_ValueToNumber(L, 2);

	pItemBox->SetIndex(nIndex);

Exit0:
	return 0;
}

int KItemHandleClassScriptTable::LuaItemBox_GetIndex(Lua_State *L)
{
	int nRetCode = false;
	int nIndex = -1;
	KItemBox *pItemBox = NULL;

	ASSERT(L);

	KG_PROCESS_ERROR(Lua_GetTopIndex(L) == 1);

	nRetCode = THostWndItem_GetUserData(L, &pItemBox);
	KGLOG_PROCESS_ERROR(nRetCode);
	ASSERT(pItemBox);

	nIndex = pItemBox->GetIndex();

Exit0:
	Lua_PushNumber(L, nIndex);
	return 1;
}

int KItemHandleClassScriptTable::LuaItemBox_SetObject(Lua_State *L)
{
	int nRetCode = false;
	int nObjectType = -1;
	int nParamCount = 0;
	ITEM_BOX_OBJECT_DATA ObjectData;
	KItemBox *pItemBox = NULL;

	ASSERT(L);

	nParamCount = Lua_GetTopIndex(L);
	KGLOG_PROCESS_ERROR(nParamCount >= 2);

	nRetCode = THostWndItem_GetUserData(L, &pItemBox);
	KGLOG_PROCESS_ERROR(nRetCode);
	ASSERT(pItemBox);

	nObjectType = (int)Lua_ValueToNumber(L, 2);

	for (int i = 0; i < MAX_BOX_OBJECT_DATA_COUNT; ++i)
	{
		int nParamIndex = i + 3;
		if (nParamIndex > nParamCount)
			break;
		ObjectData.fData[i] = Lua_ValueToNumber(L, nParamIndex);
	}

	pItemBox->SetObject(nObjectType, ObjectData);

Exit0:
	return 0;
}

int KItemHandleClassScriptTable::LuaItemBox_GetObject(Lua_State *L)
{
	int nRetCode = false;
	int nObjectType = -1;
	ITEM_BOX_OBJECT_DATA ObjectData;
	KItemBox *pItemBox = NULL;

	ASSERT(L);

	KG_PROCESS_ERROR(Lua_GetTopIndex(L) == 1);

	nRetCode = THostWndItem_GetUserData(L, &pItemBox);
	KGLOG_PROCESS_ERROR(nRetCode);
	ASSERT(pItemBox);

	pItemBox->GetObject(nObjectType, ObjectData);
Exit0:
	Lua_PushNumber(L, nObjectType);
	for (int i = 0; i < MAX_BOX_OBJECT_DATA_COUNT; ++i)
	{
		Lua_PushNumber(L, ObjectData.fData[i]);
	}
	return MAX_BOX_OBJECT_DATA_COUNT + 1;
}


int KItemHandleClassScriptTable::LuaItemBox_GetObjectType(Lua_State *L)
{
	int nRetCode = false;
	int nObjectType = -1;
	KItemBox *pItemBox = NULL;

	ASSERT(L);

	KG_PROCESS_ERROR(Lua_GetTopIndex(L) == 1);

	nRetCode = THostWndItem_GetUserData(L, &pItemBox);
	KGLOG_PROCESS_ERROR(nRetCode);
	ASSERT(pItemBox);

	nObjectType = pItemBox->GetObjectType();

Exit0:
	Lua_PushNumber(L, nObjectType);
	return 1;
}

int KItemHandleClassScriptTable::LuaItemBox_GetObjectData(Lua_State *L)
{
	int nRetCode = false;
	ITEM_BOX_OBJECT_DATA ObjectData;
	KItemBox *pItemBox = NULL;

	ASSERT(L);

	KG_PROCESS_ERROR(Lua_GetTopIndex(L) == 1);

	nRetCode = THostWndItem_GetUserData(L, &pItemBox);
	KGLOG_PROCESS_ERROR(nRetCode);
	ASSERT(pItemBox);

	pItemBox->GetObjectData(ObjectData);
Exit0:
	for (int i = 0; i < MAX_BOX_OBJECT_DATA_COUNT; ++i)
	{
		Lua_PushNumber(L, ObjectData.fData[i]);
	}
	return MAX_BOX_OBJECT_DATA_COUNT;
}

int KItemHandleClassScriptTable::LuaItemBox_ClearObject(Lua_State *L)
{
	int nRetCode = false;
	KItemBox *pItemBox = NULL;

	ASSERT(L);

	KG_PROCESS_ERROR(Lua_GetTopIndex(L) == 1);

	nRetCode = THostWndItem_GetUserData(L, &pItemBox);
	KGLOG_PROCESS_ERROR(nRetCode);
	ASSERT(pItemBox);

	pItemBox->ClearObject();

Exit0:
	return 0;
}

int KItemHandleClassScriptTable::LuaItemBox_IsEmpty(Lua_State *L)
{
	int nRetCode = false;
	int nEmpty = false;
	KItemBox *pItemBox = NULL;

	ASSERT(L);

	KG_PROCESS_ERROR(Lua_GetTopIndex(L) == 1);

	nRetCode = THostWndItem_GetUserData(L, &pItemBox);
	KGLOG_PROCESS_ERROR(nRetCode);
	ASSERT(pItemBox);

	nEmpty = pItemBox->IsEmpty();

Exit0:
	Lua_PushBoolean(L, nEmpty);
	return 1;
}

int KItemHandleClassScriptTable::LuaItemBox_EnableObject(Lua_State *L)
{
	int nRetCode = false;
	KItemBox *pItemBox = NULL;
	int nEnable = true;

	ASSERT(L);

	KG_PROCESS_ERROR(Lua_GetTopIndex(L) == 2);

	nRetCode = THostWndItem_GetUserData(L, &pItemBox);
	KGLOG_PROCESS_ERROR(nRetCode);
	ASSERT(pItemBox);

	if (Lua_IsBoolean(L, 2))
	{
		nEnable = (int)Lua_ValueToBoolean(L, 2);
	}
	else
	{
		nEnable = (int)Lua_ValueToNumber(L, 2);
	}

	pItemBox->EnableObject(nEnable);

Exit0:
	return 0;
}

int KItemHandleClassScriptTable::LuaItemBox_IsObjectEnable(Lua_State *L)
{
	int nRetCode = false;
	KItemBox *pItemBox = NULL;
	int nEnable = true;

	ASSERT(L);

	KG_PROCESS_ERROR(Lua_GetTopIndex(L) == 1);

	nRetCode = THostWndItem_GetUserData(L, &pItemBox);
	KGLOG_PROCESS_ERROR(nRetCode);
	ASSERT(pItemBox);

	nEnable = pItemBox->IsObjectEnable();

Exit0:
	Lua_PushBoolean(L, nEnable);
	return 1;
}


int KItemHandleClassScriptTable::LuaItemBox_EnableObjectEquip(Lua_State *L)
{
	int nRetCode = false;
	int nEnable = true;
	KItemBox *pItemBox = NULL;

	ASSERT(L);

	KG_PROCESS_ERROR(Lua_GetTopIndex(L) == 2);

	nRetCode = THostWndItem_GetUserData(L, &pItemBox);
	KGLOG_PROCESS_ERROR(nRetCode);
	ASSERT(pItemBox);

	if (Lua_IsBoolean(L, 2))
		nEnable = (int)Lua_ValueToBoolean(L, 2);
	else
		nEnable = (int)Lua_ValueToNumber(L, 2);

	pItemBox->EnableObjectEquip(nEnable);

Exit0:
	return 0;
}

int KItemHandleClassScriptTable::LuaItemBox_IsObjectEquipable(Lua_State *L)
{
	int nRetCode = false;
	KItemBox *pItemBox = NULL;
	int nEnable = true;

	ASSERT(L);

	KG_PROCESS_ERROR(Lua_GetTopIndex(L) == 1);

	nRetCode = THostWndItem_GetUserData(L, &pItemBox);
	KGLOG_PROCESS_ERROR(nRetCode);
	ASSERT(pItemBox);

	nEnable = pItemBox->IsObjectEquipable();

Exit0:
	Lua_PushBoolean(L, nEnable);
	return 1;
}

int KItemHandleClassScriptTable::LuaItemBox_SetObjectCoolDown(Lua_State *L)
{
	int nRetCode = false;
	int nCoolDown = false;
	KItemBox *pItemBox = NULL;

	ASSERT(L);

	KG_PROCESS_ERROR(Lua_GetTopIndex(L) == 2);

	nRetCode = THostWndItem_GetUserData(L, &pItemBox);
	KGLOG_PROCESS_ERROR(nRetCode);
	ASSERT(pItemBox);

	if (Lua_IsBoolean(L, 2))
		nCoolDown = (int)Lua_ValueToBoolean(L, 2);
	else
		nCoolDown = (int)Lua_ValueToNumber(L, 2);

	pItemBox->SetObjectCoolDown(nCoolDown);

Exit0:
	return 0;
}

int KItemHandleClassScriptTable::LuaItemBox_IsObjectCoolDown(Lua_State *L)
{
	int nRetCode = false;
	KItemBox *pItemBox = NULL;
	int nCoolDown = false;

	ASSERT(L);

	KG_PROCESS_ERROR(Lua_GetTopIndex(L) == 1);

	nRetCode = THostWndItem_GetUserData(L, &pItemBox);
	KGLOG_PROCESS_ERROR(nRetCode);
	ASSERT(pItemBox);

	nCoolDown = pItemBox->IsObjectCoolDown();

Exit0:
	Lua_PushBoolean(L, nCoolDown);
	return 1;
}

int KItemHandleClassScriptTable::LuaItemBox_SetObjectSparking(Lua_State *L)
{
	int nRetCode = false;
	int nSparking = false;
	KItemBox *pItemBox = NULL;

	ASSERT(L);

	KG_PROCESS_ERROR(Lua_GetTopIndex(L) == 2);

	nRetCode = THostWndItem_GetUserData(L, &pItemBox);
	KGLOG_PROCESS_ERROR(nRetCode);
	ASSERT(pItemBox);

	if (Lua_IsBoolean(L, 2))
		nSparking = (int)Lua_ValueToBoolean(L, 2);
	else
		nSparking = (int)Lua_ValueToNumber(L, 2);

	pItemBox->SetObjectSparking(nSparking);

Exit0:
	return 0;
}

int KItemHandleClassScriptTable::LuaItemBox_SetObjectInUse(Lua_State *L)
{
	int nRetCode = false;
	int nInUse = false;
	KItemBox *pItemBox = NULL;

	ASSERT(L);

	KG_PROCESS_ERROR(Lua_GetTopIndex(L) == 2);

	nRetCode = THostWndItem_GetUserData(L, &pItemBox);
	KGLOG_PROCESS_ERROR(nRetCode);
	ASSERT(pItemBox);

	if (Lua_IsBoolean(L, 2))
		nInUse = (int)Lua_ValueToBoolean(L, 2);
	else
		nInUse = (int)Lua_ValueToNumber(L, 2);

	pItemBox->SetObjectInUse(nInUse);

Exit0:
	return 0;
}

int KItemHandleClassScriptTable::LuaItemBox_SetObjectStaring(Lua_State *L)
{
	int nRetCode = false;
	int nStaring = false;
	KItemBox *pItemBox = NULL;

	ASSERT(L);

	KG_PROCESS_ERROR(Lua_GetTopIndex(L) == 2);

	nRetCode = THostWndItem_GetUserData(L, &pItemBox);
	KGLOG_PROCESS_ERROR(nRetCode);
	ASSERT(pItemBox);

	if (Lua_IsBoolean(L, 2))
		nStaring = (int)Lua_ValueToBoolean(L, 2);
	else
		nStaring = (int)Lua_ValueToNumber(L, 2);

	pItemBox->SetObjectStaring(nStaring);

Exit0:
	return 0;
}


int KItemHandleClassScriptTable::LuaItemBox_SetObjectSelected(Lua_State *L)
{
	int nRetCode = false;
	int nSelcted = false;
	KItemBox *pItemBox = NULL;

	ASSERT(L);

	KG_PROCESS_ERROR(Lua_GetTopIndex(L) == 2);

	nRetCode = THostWndItem_GetUserData(L, &pItemBox);
	KGLOG_PROCESS_ERROR(nRetCode);
	ASSERT(pItemBox);

	if (Lua_IsBoolean(L, 2))
		nSelcted= (int)Lua_ValueToBoolean(L, 2);
	else
		nSelcted= (int)Lua_ValueToNumber(L, 2);

	pItemBox->SetObjectSelected(nSelcted);

Exit0:
	return 0;
}

int KItemHandleClassScriptTable::LuaItemBox_IsObjectSelected(Lua_State *L)
{
	int nRetCode = false;
	int nIsSlected = false;
	KItemBox *pItemBox = NULL;

	ASSERT(L);

	KG_PROCESS_ERROR(Lua_GetTopIndex(L) == 1);

	nRetCode = THostWndItem_GetUserData(L, &pItemBox);
	KGLOG_PROCESS_ERROR(nRetCode);
	ASSERT(pItemBox);

	nIsSlected = pItemBox->IsObjectSelected();

Exit0:
	Lua_PushBoolean(L, nIsSlected);
	return 1;
}

int KItemHandleClassScriptTable::LuaItemBox_SetObjectMouseOver(Lua_State *L)
{
	int nRetCode = false;
	int nMouseOver = false;
	KItemBox *pItemBox = NULL;

	ASSERT(L);

	KG_PROCESS_ERROR(Lua_GetTopIndex(L) == 2);

	nRetCode = THostWndItem_GetUserData(L, &pItemBox);
	KGLOG_PROCESS_ERROR(nRetCode);
	ASSERT(pItemBox);

	if (Lua_IsBoolean(L, 2))
		nMouseOver = (int)Lua_ValueToBoolean(L, 2);
	else
		nMouseOver = (int)Lua_ValueToNumber(L, 2);

	pItemBox->SetObjectMouseOver(nMouseOver);

Exit0:
	return 0;
}

int KItemHandleClassScriptTable::LuaItemBox_IsObjectMouseOver(Lua_State *L)
{
	int nRetCode = false;
	int nOver= false;
	KItemBox *pItemBox = NULL;

	ASSERT(L);

	KG_PROCESS_ERROR(Lua_GetTopIndex(L) == 1);

	nRetCode = THostWndItem_GetUserData(L, &pItemBox);
	KGLOG_PROCESS_ERROR(nRetCode);
	ASSERT(pItemBox);

	nOver = pItemBox->IsObjectMouseOver();

Exit0:
	Lua_PushBoolean(L, nOver);
	return 1;
}

int KItemHandleClassScriptTable::LuaItemBox_SetObjectPressed(Lua_State *L)
{
	int nRetCode = false;
	int nPressed = false;
	KItemBox *pItemBox = NULL;

	ASSERT(L);

	KG_PROCESS_ERROR(Lua_GetTopIndex(L) == 2);

	nRetCode = THostWndItem_GetUserData(L, &pItemBox);
	KGLOG_PROCESS_ERROR(nRetCode);
	ASSERT(pItemBox);

	if (Lua_IsBoolean(L, 2))
		nPressed = (int)Lua_ValueToBoolean(L, 2);
	else
		nPressed = (int)Lua_ValueToNumber(L, 2);

	pItemBox->SetObjectPressed(nPressed);

Exit0:
	return 0;
}

int KItemHandleClassScriptTable::LuaItemBox_IsObjectPressed(Lua_State *L)
{
	int nRetCode = false;
	int nPressed= false;
	KItemBox *pItemBox = NULL;

	ASSERT(L);

	KG_PROCESS_ERROR(Lua_GetTopIndex(L) == 1);

	nRetCode = THostWndItem_GetUserData(L, &pItemBox);
	KGLOG_PROCESS_ERROR(nRetCode);
	ASSERT(pItemBox);

	nPressed = pItemBox->IsObjectPressed();

Exit0:
	Lua_PushBoolean(L, nPressed);
	return 1;
}

int KItemHandleClassScriptTable::LuaItemBox_SetCoolDownPercentage(Lua_State *L)
{
	int nRetCode = false;
	float fCoolingDownPercentage = 0.0f;
	KItemBox *pItemBox = NULL;

	ASSERT(L);

	KG_PROCESS_ERROR(Lua_GetTopIndex(L) == 2);

	nRetCode = THostWndItem_GetUserData(L, &pItemBox);
	KGLOG_PROCESS_ERROR(nRetCode);
	ASSERT(pItemBox);

	fCoolingDownPercentage = (float)Lua_ValueToNumber(L, 2);

	pItemBox->SetCoolDownPercentage(fCoolingDownPercentage);

Exit0:
	return 0;
}

int KItemHandleClassScriptTable::LuaItemBox_GetCoolDownPercentage(Lua_State *L)
{
	int nRetCode = false;
	float fCoolingDownPercentage = 0.0f;
	KItemBox *pItemBox = NULL;

	ASSERT(L);

	KG_PROCESS_ERROR(Lua_GetTopIndex(L) == 1);

	nRetCode = THostWndItem_GetUserData(L, &pItemBox);
	KGLOG_PROCESS_ERROR(nRetCode);
	ASSERT(pItemBox);

	fCoolingDownPercentage = pItemBox->GetCoolDownPercentage();

Exit0:
	Lua_PushNumber(L, fCoolingDownPercentage);
	return 1;
}

int KItemHandleClassScriptTable::LuaItemBox_SetObjectIcon(Lua_State *L)
{
	int nRetCode = false;
	int nObjectIcon = -1;
	KItemBox *pItemBox = NULL;

	ASSERT(L);

	KG_PROCESS_ERROR(Lua_GetTopIndex(L) == 2);

	nRetCode = THostWndItem_GetUserData(L, &pItemBox);
	KGLOG_PROCESS_ERROR(nRetCode);
	ASSERT(pItemBox);

	nObjectIcon = (int)Lua_ValueToNumber(L, 2);

	pItemBox->SetObjectIcon(nObjectIcon);

Exit0:
	return 0;
}

int KItemHandleClassScriptTable::LuaItemBox_GetObjectIcon(Lua_State *L)
{
	int nRetCode = false;
	int nObjectIcon = -1;
	KItemBox *pItemBox = NULL;

	ASSERT(L);

	KG_PROCESS_ERROR(Lua_GetTopIndex(L) == 1);

	nRetCode = THostWndItem_GetUserData(L, &pItemBox);
	KGLOG_PROCESS_ERROR(nRetCode);
	ASSERT(pItemBox);

	nObjectIcon = pItemBox->GetObjectIcon();

Exit0:
	Lua_PushNumber(L, nObjectIcon);
	return 1;
}

int KItemHandleClassScriptTable::LuaItemBox_ClearObjectIcon(Lua_State *L)
{
	int nRetCode = false;
	KItemBox *pItemBox = NULL;

	ASSERT(L);

	KG_PROCESS_ERROR(Lua_GetTopIndex(L) == 1);

	nRetCode = THostWndItem_GetUserData(L, &pItemBox);
	KGLOG_PROCESS_ERROR(nRetCode);
	ASSERT(pItemBox);

	pItemBox->ClearObjectIcon();

Exit0:
	return 0;
}

int KItemHandleClassScriptTable::LuaItemBox_SetOverText(Lua_State *L)
{
	int nRetCode = false;
	int nOverTextIndex = -1;
	LPCSTR szOverText = NULL;
	KItemBox *pItemBox = NULL;

	ASSERT(L);

	KG_PROCESS_ERROR(Lua_GetTopIndex(L) == 3);

	nRetCode = THostWndItem_GetUserData(L, &pItemBox);
	KGLOG_PROCESS_ERROR(nRetCode);
	ASSERT(pItemBox);

	nOverTextIndex = (int)Lua_ValueToNumber(L, 2);
	szOverText = Lua_ValueToString(L, 3);

    {
        ATL::CA2W wszOverText(szOverText, GetCodePage());
        pItemBox->SetOverText(nOverTextIndex, wszOverText);
    }

Exit0:
	return 0;
}

int KItemHandleClassScriptTable::LuaItemBox_GetOverText(Lua_State *L)
{
	int nRetCode = false;
	int nOverTextIndex = -1;
	LPCWSTR wszOverText = NULL;
	KItemBox *pItemBox = NULL;

	ASSERT(L);

	KG_PROCESS_ERROR(Lua_GetTopIndex(L) == 2);

	nRetCode = THostWndItem_GetUserData(L, &pItemBox);
	KGLOG_PROCESS_ERROR(nRetCode);
	ASSERT(pItemBox);

	nOverTextIndex = (int)Lua_ValueToNumber(L, 2);

	wszOverText = pItemBox->GetOverText(nOverTextIndex);

Exit0:
	Lua_PushString(L, ATL::CW2A(wszOverText, GetCodePage()));
	return 1;
}

int KItemHandleClassScriptTable::LuaItemBox_SetOverTextFontScheme(Lua_State *L)
{
	int nRetCode = false;
	int nOverTextIndex = -1;
	int nFontScheme = 0;
	KItemBox *pItemBox = NULL;

	ASSERT(L);

	KG_PROCESS_ERROR(Lua_GetTopIndex(L) == 3);

	nRetCode = THostWndItem_GetUserData(L, &pItemBox);
	KGLOG_PROCESS_ERROR(nRetCode);
	ASSERT(pItemBox);

	nOverTextIndex = (int)Lua_ValueToNumber(L, 2);
	nFontScheme = (int)Lua_ValueToNumber(L, 3);

	pItemBox->SetOverTextFontScheme(nOverTextIndex, nFontScheme);

Exit0:
	return 0;
}

int KItemHandleClassScriptTable::LuaItemBox_GetOverTextFontScheme(Lua_State *L)
{
	int nRetCode = false;
	int nOverTextIndex = -1;
	int nFontScheme = 0;
	KItemBox *pItemBox = NULL;

	ASSERT(L);

	KG_PROCESS_ERROR(Lua_GetTopIndex(L) == 2);

	nRetCode = THostWndItem_GetUserData(L, &pItemBox);
	KGLOG_PROCESS_ERROR(nRetCode);
	ASSERT(pItemBox);

	nOverTextIndex = (int)Lua_ValueToNumber(L, 2);

	nFontScheme = pItemBox->GetOverTextFontScheme(nOverTextIndex);

Exit0:
	Lua_PushNumber(L, nFontScheme);
	return 1;
}

int KItemHandleClassScriptTable::LuaItemBox_SetOverTextPosition(Lua_State *L)
{
	int nRetCode = false;
	int nOverTextIndex = -1;
	int nPos = 0;
	KItemBox *pItemBox = NULL;

	ASSERT(L);

	KG_PROCESS_ERROR(Lua_GetTopIndex(L) == 3);

	nRetCode = THostWndItem_GetUserData(L, &pItemBox);
	KGLOG_PROCESS_ERROR(nRetCode);
	ASSERT(pItemBox);

	nOverTextIndex = (int)Lua_ValueToNumber(L, 2);
	nPos = (int)Lua_ValueToNumber(L, 3);

	pItemBox->SetOverTextPosition(nOverTextIndex, nPos);

Exit0:
	return 0;
}

int KItemHandleClassScriptTable::LuaItemBox_GetOverTextPosition(Lua_State *L)
{
	int nRetCode = false;
	int nOverTextIndex = -1;
	int nPos = 0;
	KItemBox *pItemBox = NULL;

	ASSERT(L);

	KG_PROCESS_ERROR(Lua_GetTopIndex(L) == 2);

	nRetCode = THostWndItem_GetUserData(L, &pItemBox);
	KGLOG_PROCESS_ERROR(nRetCode);
	ASSERT(pItemBox);

	nOverTextIndex = (int)Lua_ValueToNumber(L, 2);

	nPos = pItemBox->GetOverTextPosition(nOverTextIndex);

Exit0:
	Lua_PushNumber(L, nPos);
	return 1;
}

int KItemHandleClassScriptTable::LuaItemHandle_AppendItemFromString(Lua_State *L)
{
	ASSERT(L);
	int nRetCode = false;
	int nDecodedLen = 0;
	int nParamCount = 0;
	KItemHandle *pItemHandle = NULL;
	LPCSTR pcszString = NULL;
	KWndWindow *pWndEventOwner = NULL;

	nParamCount = Lua_GetTopIndex(L);

	KGLOG_PROCESS_ERROR(nParamCount > 1);

	nRetCode = THostWndItem_GetUserData(L, &pItemHandle);
	KGLOG_PROCESS_ERROR(nRetCode);
	ASSERT(pItemHandle);

	pWndEventOwner = pItemHandle->GetWndOwner();

	pcszString = Lua_ValueToString(L, 2);
	KGLOG_PROCESS_ERROR(pcszString);

	nRetCode = KUiComponentsDecoder::GetSelf().AppendItem(
        pItemHandle, ATL::CA2W(pcszString, GetCodePage()), nDecodedLen, pWndEventOwner, g_pUI->m_WndStation.GetUIScale());
	KGLOG_PROCESS_ERROR(nRetCode);

Exit0:
	return 0;
}

int KItemHandleClassScriptTable::LuaItemHandle_AppendItemFromIni(Lua_State *L)
{
	ASSERT(L);
	int nRetCode = false;
	int nParamCount = 0;
    int nRetParamCount = 0;
	KItemHandle *pItemHandle = NULL;
	KWndWindow *pWndEventOwner = NULL;
	LPCSTR pcszIniFile = NULL;
	LPCSTR pcszSectionName = NULL;
	LPCSTR pcszNewName = NULL;
	IIniFile *pIni = NULL;
    KItemNull *pItem = NULL;

	nParamCount = Lua_GetTopIndex(L);

	KGLOG_PROCESS_ERROR(nParamCount > 2);

	nRetCode = THostWndItem_GetUserData(L, &pItemHandle);
	KGLOG_PROCESS_ERROR(nRetCode);
	ASSERT(pItemHandle);

	pWndEventOwner = pItemHandle->GetWndOwner();

	pcszIniFile = Lua_ValueToString(L, 2);
	pcszIniFile = FormatFilePath(pcszIniFile);
	KGLOG_PROCESS_ERROR(pcszIniFile);

	pcszSectionName = Lua_ValueToString(L, 3);
	KGLOG_PROCESS_ERROR(pcszSectionName);

	if (nParamCount > 3)
		pcszNewName = Lua_ValueToString(L, 4);

	pIni = g_pUI->m_Config.OpenConfigFileMaxMatching(pcszIniFile);
	KGLOG_PROCESS_ERROR(pIni);

	pItem = KUiComponentsDecoder::GetSelf().AppendItem(
		pItemHandle, pIni, pcszSectionName, pWndEventOwner, g_pUI->m_WndStation.GetUIScale(), pcszNewName);
	KGLOG_PROCESS_ERROR(pItem);

    nRetCode = HostWndItem_Lookup(L, pItem);
    KGLOG_PROCESS_ERROR(nRetCode);
    nRetParamCount++;

Exit0:
	return nRetParamCount;
}

int KItemHandleClassScriptTable::LuaItemHandle_AdjustItemShowInfo(Lua_State *L)
{
	ASSERT(L);
	int nRetCode = false;
	KItemHandle *pItemHandle = NULL;

	KGLOG_PROCESS_ERROR(Lua_GetTopIndex(L) > 0);

	nRetCode = THostWndItem_GetUserData(L, &pItemHandle);
	KGLOG_PROCESS_ERROR(nRetCode);
	ASSERT(pItemHandle);

	pItemHandle->AdjustItemShowInfo();

Exit0:
	return 0;
}

int KItemHandleClassScriptTable::LuaItemHandle_InsertItemFromString(Lua_State *L)
{
	ASSERT(L);
	
	int nRetCode = false;
	int nParamCount = 0;
	int nDecodedLen = 0;
	int nBehindOrBefore = INSERT_BEHIND;
	KItemHandle *pItemHandle = NULL;
	KItemNull* pItemNull = NULL;
	LPCSTR pcszString = NULL;
	KWndWindow *pWndEventOwner = NULL;
	int nAppend = false;

	nParamCount = Lua_GetTopIndex(L);
	KGLOG_PROCESS_ERROR(nParamCount > 3);

	nRetCode = THostWndItem_GetUserData(L, &pItemHandle);
	KGLOG_PROCESS_ERROR(nRetCode);
	ASSERT(pItemHandle);	

	pWndEventOwner = pItemHandle->GetWndOwner();

	if (pItemHandle->GetItemCount() < 1)
	{
		nAppend = true;
	}
	else if (lua_type(L, 2) == LUA_TNUMBER)
	{
		int nIndex = (int)Lua_ValueToNumber(L, 2);
		if (nIndex < 0)
		{
			nIndex = 0;
			nBehindOrBefore = INSERT_BEFORE;

			pItemNull = pItemHandle->GetItemByIndex(nIndex);
			KGLOG_PROCESS_ERROR(pItemNull);
		}
		else if (nIndex >= pItemHandle->GetItemCount())
		{
			nAppend = true;
		}
		else
		{
			pItemNull = pItemHandle->GetItemByIndex(nIndex);
			KGLOG_PROCESS_ERROR(pItemNull);
			if (Lua_IsBoolean(L, 3))
				nBehindOrBefore = (int)Lua_ValueToBoolean(L, 3);
			else
				nBehindOrBefore = (int)Lua_ValueToNumber(L, 3);
		}	
	}
	else
	{
		LPCSTR pcszItemName = Lua_ValueToString(L, 2);

        pItemNull = pItemHandle->GetItemByTreePath(pcszItemName);
		KGLOG_PROCESS_ERROR(pItemNull);

		if (Lua_IsBoolean(L, 3))
			nBehindOrBefore = (int)Lua_ValueToBoolean(L, 3);
		else
			nBehindOrBefore = (int)Lua_ValueToNumber(L, 3);
	}

	pcszString = Lua_ValueToString(L, 4);
	KGLOG_PROCESS_ERROR(pcszString);

	if (nAppend)
	{
		nRetCode = KUiComponentsDecoder::GetSelf().AppendItem(
            pItemHandle, ATL::CA2W(pcszString, GetCodePage()), nDecodedLen, pWndEventOwner, g_pUI->m_WndStation.GetUIScale());
        KG_PROCESS_ERROR(nRetCode);
	}
	else
	{
		nRetCode = KUiComponentsDecoder::GetSelf().InsertItem(
            pItemHandle, ATL::CA2W(pcszString, GetCodePage()), pItemNull, nBehindOrBefore, nDecodedLen, pWndEventOwner, g_pUI->m_WndStation.GetUIScale());
        KG_PROCESS_ERROR(nRetCode);
	}

Exit0:
	return 0;
}

int KItemHandleClassScriptTable::LuaItemHandle_InsertItemFromIni(Lua_State *L)
{
	ASSERT(L);

	int nRetCode = false;
	int nParamCount = 0;
	int nBehindOrBefore = INSERT_BEHIND;
	KItemHandle *pItemHandle = NULL;
	KItemNull *pItemNull = NULL;
	KWndWindow *pWndEventOwner = NULL;
	IIniFile *pIni = NULL;
	LPCSTR pcszSectionName = NULL;
	LPCSTR pcszNewName = NULL;
	LPCSTR pcszIniFile = NULL;
	int nAppend = false;

	nParamCount = Lua_GetTopIndex(L);
	KGLOG_PROCESS_ERROR(nParamCount > 3);

	nRetCode = THostWndItem_GetUserData(L, &pItemHandle);
	KGLOG_PROCESS_ERROR(nRetCode);
	ASSERT(pItemHandle);

	pWndEventOwner = pItemHandle->GetWndOwner();

	if (pItemHandle->GetItemCount() < 1)
	{
		nAppend = true;
	}
	else if (lua_type(L, 2) == LUA_TNUMBER)
	{
		int nIndex = (int)Lua_ValueToNumber(L, 2);
		if (nIndex < 0)
		{
			nIndex = 0;
			nBehindOrBefore = INSERT_BEFORE;

			pItemNull = pItemHandle->GetItemByIndex(nIndex);
			KGLOG_PROCESS_ERROR(pItemNull);
		}
		else if (nIndex >= pItemHandle->GetItemCount())
		{
			nAppend = true;
		}
		else
		{
			pItemNull = pItemHandle->GetItemByIndex(nIndex);
			KGLOG_PROCESS_ERROR(pItemNull);

			if (Lua_IsBoolean(L, 3))
				nBehindOrBefore = (int)Lua_ValueToBoolean(L, 3);
			else
				nBehindOrBefore = (int)Lua_ValueToNumber(L, 3);
		}	
	}
	else
	{
		LPCSTR pcszItemName = Lua_ValueToString(L, 2);

        pItemNull = pItemHandle->GetItemByTreePath(pcszItemName);
		KGLOG_PROCESS_ERROR(pItemNull);

		if (Lua_IsBoolean(L, 3))
			nBehindOrBefore = (int)Lua_ValueToBoolean(L, 3);
		else
			nBehindOrBefore = (int)Lua_ValueToNumber(L, 3);
	}

	pcszIniFile = Lua_ValueToString(L, 4);
	pcszIniFile = FormatFilePath(pcszIniFile);
	KGLOG_PROCESS_ERROR(pcszIniFile);

	pcszSectionName = Lua_ValueToString(L, 5);
	KGLOG_PROCESS_ERROR(pcszSectionName);

	if (nParamCount > 5) 
		pcszNewName = Lua_ValueToString(L, 6);

	pIni = g_pUI->m_Config.OpenConfigFileMaxMatching(pcszIniFile);
	KGLOG_PROCESS_ERROR(pIni);

	if (nAppend)
	{
		KItemNull* pItem = KUiComponentsDecoder::GetSelf().AppendItem(
			pItemHandle, pIni, pcszSectionName, pWndEventOwner, g_pUI->m_WndStation.GetUIScale(), pcszNewName);
        KG_PROCESS_ERROR(pItem);
	}
	else
	{
		KItemNull* pItem = KUiComponentsDecoder::GetSelf().InsertItem(
            pItemHandle, pItemNull, nBehindOrBefore, pIni, pcszSectionName, pWndEventOwner, g_pUI->m_WndStation.GetUIScale(), pcszNewName);
        KG_PROCESS_ERROR(pItem);
	}

Exit0:
	return 0;
}

int KItemHandleClassScriptTable::LuaItemHandle_FormatAllItemPos(Lua_State *L)
{
	ASSERT(L);

	int nRetCode = false;
	KItemHandle *pItemHandle = NULL;

	KG_PROCESS_ERROR(Lua_GetTopIndex(L) == 1);

	nRetCode = THostWndItem_GetUserData(L, &pItemHandle);
	KGLOG_PROCESS_ERROR(nRetCode);
	ASSERT(pItemHandle);

	pItemHandle->FormatAllItemPos();

Exit0:
	return 0;
}

int KItemHandleClassScriptTable::LuaItemHandle_SetHandleStyle(Lua_State *L)
{
	ASSERT(L);

	int nRetCode = false;
	DWORD dwHandleStyle = 0;
	KItemHandle *pItemHandle = NULL;

	KG_PROCESS_ERROR(Lua_GetTopIndex(L) == 2);

	nRetCode = THostWndItem_GetUserData(L, &pItemHandle);
	KGLOG_PROCESS_ERROR(nRetCode);
	ASSERT(pItemHandle);

	dwHandleStyle = (DWORD)Lua_ValueToNumber(L, 2);

	pItemHandle->SetHandleStyle(dwHandleStyle);

Exit0:
	return 0;
}

int KItemHandleClassScriptTable::LuaItemHandle_SetMinRowHeight(Lua_State *L)
{
	ASSERT(L);

	int nRetCode = false;
	float fHeight = 0.0f;
	KItemHandle *pItemHandle = NULL;

	KG_PROCESS_ERROR(Lua_GetTopIndex(L) == 2);

	nRetCode = THostWndItem_GetUserData(L, &pItemHandle);
	KGLOG_PROCESS_ERROR(nRetCode);
	ASSERT(pItemHandle);

	fHeight = (float)Lua_ValueToNumber(L, 2);

	fHeight *= g_pUI->m_WndStation.GetUIScale();

	pItemHandle->SetMinRowHeight(fHeight);

Exit0:
	return 0;
}

int KItemHandleClassScriptTable::LuaItemHandle_SetMaxRowHeight(Lua_State *L)
{
	ASSERT(L);

	int nRetCode = false;
	float fHeight = 0.0f;
	KItemHandle *pItemHandle = NULL;

	KG_PROCESS_ERROR(Lua_GetTopIndex(L) == 2);

	nRetCode = THostWndItem_GetUserData(L, &pItemHandle);
	KGLOG_PROCESS_ERROR(nRetCode);
	ASSERT(pItemHandle);

	fHeight = (float)Lua_ValueToNumber(L, 2);

	fHeight *= g_pUI->m_WndStation.GetUIScale();

	pItemHandle->SetMaxRowHeight(fHeight);

Exit0:
	return 0;
}

int KItemHandleClassScriptTable::LuaItemHandle_SetRowHeight(Lua_State *L)
{
	ASSERT(L);

	int nRetCode = false;
	float fHeight = 0.0f;
	KItemHandle *pItemHandle = NULL;

	KG_PROCESS_ERROR(Lua_GetTopIndex(L) == 2);

	nRetCode = THostWndItem_GetUserData(L, &pItemHandle);
	KGLOG_PROCESS_ERROR(nRetCode);
	ASSERT(pItemHandle);

	fHeight = (float)Lua_ValueToNumber(L, 2);

	fHeight *= g_pUI->m_WndStation.GetUIScale();

	pItemHandle->SetRowHeight(fHeight);

Exit0:
	return 0;
}

int KItemHandleClassScriptTable::LuaItemHandle_SetRowSpacing(Lua_State *L)
{
	ASSERT(L);

	int nRetCode = false;
	float fSpacing = 0.0f;
	KItemHandle *pItemHandle = NULL;

	KG_PROCESS_ERROR(Lua_GetTopIndex(L) == 2);

	nRetCode = THostWndItem_GetUserData(L, &pItemHandle);
	KGLOG_PROCESS_ERROR(nRetCode);
	ASSERT(pItemHandle);

	fSpacing = (float)Lua_ValueToNumber(L, 2);

	fSpacing *= g_pUI->m_WndStation.GetUIScale();

	pItemHandle->SetRowSpacing(fSpacing);

Exit0:
	return 0;
}

int KItemHandleClassScriptTable::LuaItemHandle_RemoveItem(Lua_State *L)
{
	ASSERT(L);

	int nRetCode = false;
	KItemHandle *pItemHandle = NULL;

	KG_PROCESS_ERROR(Lua_GetTopIndex(L) == 2);

	nRetCode = THostWndItem_GetUserData(L, &pItemHandle);
	KGLOG_PROCESS_ERROR(nRetCode);
	ASSERT(pItemHandle);

	if (lua_type(L, 2) == LUA_TNUMBER)
	{
		int nIndex = (int)Lua_ValueToNumber(L, 2);
		KItemNull *pItemNull = pItemHandle->GetItemByIndex(nIndex);
		pItemHandle->Remove(pItemNull);
	}
	else if (lua_type(L, 2) == LUA_TTABLE)
	{
		KItemNull* pItemNull = NULL;

		nRetCode = THostWndItem_GetUserData(L, &pItemNull, 2);
		KGLOG_PROCESS_ERROR(nRetCode);

		ASSERT(pItemNull);
		pItemHandle->Remove(pItemNull);
	}
	else
	{
		LPCSTR pcszItemName = Lua_ValueToString(L, 2);
        pItemHandle->Remove(pcszItemName);
	}

Exit0:
	return 0;
}

int KItemHandleClassScriptTable::LuaItemHandle_Clear(Lua_State *L)
{
	ASSERT(L);

	int nRetCode = false;
	KItemHandle *pItemHandle = NULL;

	KG_PROCESS_ERROR(Lua_GetTopIndex(L) == 1);

	nRetCode = THostWndItem_GetUserData(L, &pItemHandle);
	KGLOG_PROCESS_ERROR(nRetCode);
	ASSERT(pItemHandle);

	pItemHandle->Clear();

Exit0:
	return 0;
}

int KItemHandleClassScriptTable::LuaItemHandle_SetSizeByAllItemSize(Lua_State *L)
{
	ASSERT(L);

	int nRetCode = false;
	KItemHandle *pItemHandle = NULL;

	KG_PROCESS_ERROR(Lua_GetTopIndex(L) == 1);

	nRetCode = THostWndItem_GetUserData(L, &pItemHandle);
	KGLOG_PROCESS_ERROR(nRetCode);
	ASSERT(pItemHandle);

	pItemHandle->SetSizeByAllItemSize();

Exit0:
	return 0;
}

int KItemHandleClassScriptTable::LuaItemHandle_GetItemStartRelPos(Lua_State *L)
{
	ASSERT(L);

	int nRetCode = false;
	float fX = 0.0f;
	float fY = 0.0f;
	KItemHandle *pItemHandle = NULL;
	float fScale = 1.0f;

	KG_PROCESS_ERROR(Lua_GetTopIndex(L) == 1);

	nRetCode = THostWndItem_GetUserData(L, &pItemHandle);
	KGLOG_PROCESS_ERROR(nRetCode);
	ASSERT(pItemHandle);

	pItemHandle->GetItemStartRelPos(fX, fY);

	fScale = g_pUI->m_WndStation.GetUIScale();

	fX /= fScale;
	fY /= fScale;

Exit0:
	Lua_PushNumber(L, fX);
	Lua_PushNumber(L, fY);
	return 2;
}

int KItemHandleClassScriptTable::LuaItemHandle_SetItemStartRelPos(Lua_State *L)
{
	ASSERT(L);

	int nRetCode = false;
	float fX = 0.0f;
	float fY = 0.0f;
	KItemHandle *pItemHandle = NULL;
	float fScale = 1.0f;

	KG_PROCESS_ERROR(Lua_GetTopIndex(L) == 3);

	nRetCode = THostWndItem_GetUserData(L, &pItemHandle);
	KGLOG_PROCESS_ERROR(nRetCode);
	ASSERT(pItemHandle);

	fScale = g_pUI->m_WndStation.GetUIScale();

	fX = (float)Lua_ValueToNumber(L, 2) * fScale;
	fY = (float)Lua_ValueToNumber(L, 3) * fScale;

	pItemHandle->SetItemStartRelPos(fX, fY);

Exit0:
	return 0;
}

int KItemHandleClassScriptTable::LuaItemHandle_GetAllItemSize(Lua_State *L)
{
	ASSERT(L);

	int nRetCode = false;
	float fW = 0.0f;
	float fH = 0.0f;
	KItemHandle *pItemHandle = NULL;
	float fScale = 1.0f;

	KG_PROCESS_ERROR(Lua_GetTopIndex(L) == 1);

	nRetCode = THostWndItem_GetUserData(L, &pItemHandle);
	KGLOG_PROCESS_ERROR(nRetCode);
	ASSERT(pItemHandle);

	pItemHandle->GetAllItemSize(fW, fH);

	fScale = g_pUI->m_WndStation.GetUIScale();

	fW /= fScale;
	fH /= fScale;

Exit0:
	Lua_PushNumber(L, fW);
	Lua_PushNumber(L, fH);
	return 2;
}

int KItemHandleClassScriptTable::LuaItemHandle_GetItemCount(Lua_State *L)
{
	ASSERT(L);

	int nRetCode = false;
	int nCount = 0;
	KItemHandle *pItemHandle = NULL;

	KG_PROCESS_ERROR(Lua_GetTopIndex(L) == 1);

	nRetCode = THostWndItem_GetUserData(L, &pItemHandle);
	KGLOG_PROCESS_ERROR(nRetCode);
	ASSERT(pItemHandle);

	nCount = pItemHandle->GetItemCount();

Exit0:
	Lua_PushNumber(L, nCount);
	return 1;
}

int KItemHandleClassScriptTable::LuaItemHandle_GetVisibleItemCount(Lua_State *L)
{
	ASSERT(L);

	int nRetCode = false;
	int nCount = 0;
	KItemHandle *pItemHandle = NULL;

	KG_PROCESS_ERROR(Lua_GetTopIndex(L) == 1);

	nRetCode = THostWndItem_GetUserData(L, &pItemHandle);
	KGLOG_PROCESS_ERROR(nRetCode);
	ASSERT(pItemHandle);

	nCount = pItemHandle->GetVisibleItemCount();

Exit0:
	Lua_PushNumber(L, nCount);
	return 1;
}

int KItemHandleClassScriptTable::LuaItemHandle_Lookup(Lua_State *L)
{
	ASSERT(L);
	int nResult = 0;
	int nRetCode = false;
	KItemNull *pItemNull = NULL;
	KItemHandle *pItemHandle = NULL;

	KG_PROCESS_ERROR(Lua_GetTopIndex(L) == 2);

	nRetCode = THostWndItem_GetUserData(L, &pItemHandle);
	KGLOG_PROCESS_ERROR(nRetCode);
	ASSERT(pItemHandle);

	if (lua_type(L, 2) == LUA_TNUMBER)
	{
		int nIndex = (int)Lua_ValueToNumber(L, 2);
		pItemNull = pItemHandle->GetItemByIndex(nIndex);
	}
	else
	{
		LPCSTR pcszItemName = Lua_ValueToString(L, 2);
        pItemNull = pItemHandle->GetItemByTreePath(pcszItemName);
	}
	KG_PROCESS_ERROR(pItemNull);

	nRetCode = HostWndItem_Lookup(L, pItemNull);
	KG_PROCESS_ERROR(nRetCode);

	nResult = 1;
Exit0:
	return nResult;
}

int KItemHandleClassScriptTable::LuaItemHandle_EnableFormatWhenAppend(Lua_State *L)
{
	ASSERT(L);

	int nRetCode = false;
	KItemHandle *pItemHandle = NULL;
	int nFlag = false;

	KG_PROCESS_ERROR(Lua_GetTopIndex(L) == 2);

	nRetCode = THostWndItem_GetUserData(L, &pItemHandle);
	KGLOG_PROCESS_ERROR(nRetCode);
	ASSERT(pItemHandle);

	if (Lua_IsBoolean(L, 2))
		nFlag = (int)Lua_ValueToBoolean(L, 2);
	else
		nFlag = (int)Lua_ValueToNumber(L, 2);

	pItemHandle->EnableFormatWhenAppend(nFlag);

Exit0:
	return 0;
}

int KItemHandleClassScriptTable::LuaItemHandle_RemoveItemUntilNewLine(Lua_State *L)
{
	ASSERT(L);

	int nRetCode = false;
	KItemHandle *pItemHandle = NULL;

	KG_PROCESS_ERROR(Lua_GetTopIndex(L) == 1);

	nRetCode = THostWndItem_GetUserData(L, &pItemHandle);
	KGLOG_PROCESS_ERROR(nRetCode);
	ASSERT(pItemHandle);

	pItemHandle->RemoveItemUntilNewLine();

Exit0:
	return 0;
}

int KItemHandleClassScriptTable::LuaItemHandle_ExchangeItemIndex(Lua_State *L)
{
	ASSERT(L);

	int nRetCode = false;
	KItemHandle *pItemHandle = NULL;
	KItemNull *pItem = NULL;
	int nIndex1 = -1;
	int nIndex2 = -1;

	KG_PROCESS_ERROR(Lua_GetTopIndex(L) == 3);

	nRetCode = THostWndItem_GetUserData(L, &pItemHandle);
	KGLOG_PROCESS_ERROR(nRetCode);
	ASSERT(pItemHandle);

	if (Lua_IsNumber(L, 2))
	{
		nIndex1 = (int)Lua_ValueToNumber(L, 2);
	}
	else
	{
		nRetCode = THostWndItem_GetUserData(L, &pItem, 2);
		KGLOG_PROCESS_ERROR(nRetCode);
		ASSERT(pItem);
		KGLOG_PROCESS_ERROR(pItem->GetParent() == pItemHandle);
		nIndex1 = pItemHandle->GetItemIndex(pItem);
	}

	if (Lua_IsNumber(L, 3))
	{
		nIndex2 = (int)Lua_ValueToNumber(L, 3);
	}
	else
	{
		pItem = NULL;
		nRetCode = THostWndItem_GetUserData(L, &pItem, 3);
		KGLOG_PROCESS_ERROR(nRetCode);
		ASSERT(pItem);
		KGLOG_PROCESS_ERROR(pItem->GetParent() == pItemHandle);
		nIndex2 = pItemHandle->GetItemIndex(pItem);
	}

	pItemHandle->ExchangeIndex(nIndex1, nIndex2);

Exit0:
	return 0;
}


int KItemHandleClassScriptTable::LuaItemHandle_Sort(Lua_State *L)
{
	int nRetCode = false;
	KItemHandle *pItemHandle = NULL;

	ASSERT(L);

	KG_PROCESS_ERROR(Lua_GetTopIndex(L) == 1);

	nRetCode = THostWndItem_GetUserData(L, &pItemHandle);
	KGLOG_PROCESS_ERROR(nRetCode);
	ASSERT(pItemHandle);

	pItemHandle->Sort();

Exit0:
	return 0;
}



int KItemHandleClassScriptTable::LuaItemTreeLeaf_IsExpand(Lua_State *L)
{
	ASSERT(L);

	int nRetCode = false;
	KItemTreeLeaf *pTreeLeaf = NULL;
	int nIsExpand = false;

	KG_PROCESS_ERROR(Lua_GetTopIndex(L) == 1);

	nRetCode = THostWndItem_GetUserData(L, &pTreeLeaf);
	KGLOG_PROCESS_ERROR(nRetCode);
	ASSERT(pTreeLeaf);

	nIsExpand = pTreeLeaf->IsExpand();

Exit0:
	lua_pushboolean(L, nIsExpand);
	return 1;
}

int KItemHandleClassScriptTable::LuaItemTreeLeaf_ExpandOrCollapse(Lua_State *L)
{
	ASSERT(L);

	int nRetCode = false;
	KItemTreeLeaf *pTreeLeaf = NULL;
	int nIsExpand = false;

	KG_PROCESS_ERROR(Lua_GetTopIndex(L) == 1);

	nRetCode = THostWndItem_GetUserData(L, &pTreeLeaf);
	KGLOG_PROCESS_ERROR(nRetCode);
	ASSERT(pTreeLeaf);

	nIsExpand = pTreeLeaf->IsExpand();
	if (nIsExpand)
		pTreeLeaf->Collapse();
	else
		pTreeLeaf->Expand();

Exit0:
	return 0;
}

int KItemHandleClassScriptTable::LuaItemTreeLeaf_Expand(Lua_State *L)
{
	ASSERT(L);

	int nRetCode = false;
	KItemTreeLeaf *pTreeLeaf = NULL;

	KG_PROCESS_ERROR(Lua_GetTopIndex(L) == 1);

	nRetCode = THostWndItem_GetUserData(L, &pTreeLeaf);
	KGLOG_PROCESS_ERROR(nRetCode);
	ASSERT(pTreeLeaf);

	pTreeLeaf->Expand();

Exit0:
	return 0;
}

int KItemHandleClassScriptTable::LuaItemTreeLeaf_Collapse(Lua_State *L)
{
	ASSERT(L);

	int nRetCode = false;
	KItemTreeLeaf *pTreeLeaf = NULL;

	KG_PROCESS_ERROR(Lua_GetTopIndex(L) == 1);

	nRetCode = THostWndItem_GetUserData(L, &pTreeLeaf);
	KGLOG_PROCESS_ERROR(nRetCode);
	ASSERT(pTreeLeaf);

	pTreeLeaf->Collapse();

Exit0:
	return 0;
}

int KItemHandleClassScriptTable::LuaItemTreeLeaf_SetIndent(Lua_State *L)
{
	ASSERT(L);

	int nRetCode = false;
	KItemTreeLeaf *pTreeLeaf = NULL;
	int nIndent = 0;

	KG_PROCESS_ERROR(Lua_GetTopIndex(L) == 2);

	nRetCode = THostWndItem_GetUserData(L, &pTreeLeaf);
	KGLOG_PROCESS_ERROR(nRetCode);
	ASSERT(pTreeLeaf);

	nIndent = (int)Lua_ValueToNumber(L, 2);

	pTreeLeaf->SetIndent(nIndent);

Exit0:
	return 0;
}

int KItemHandleClassScriptTable::LuaItemTreeLeaf_GetIndent(Lua_State *L)
{
	ASSERT(L);

	int nRetCode = false;
	KItemTreeLeaf *pTreeLeaf = NULL;
	int nIndent = 0;

	KG_PROCESS_ERROR(Lua_GetTopIndex(L) == 1);

	nRetCode = THostWndItem_GetUserData(L, &pTreeLeaf);
	KGLOG_PROCESS_ERROR(nRetCode);
	ASSERT(pTreeLeaf);


	nIndent = pTreeLeaf->GetIndent();

Exit0:
	Lua_PushNumber(L, nIndent);
	return 1;
}

int KItemHandleClassScriptTable::LuaItemTreeLeaf_SetEachIndentWidth(Lua_State *L)
{
	ASSERT(L);

	int nRetCode = false;
	KItemTreeLeaf *pTreeLeaf = NULL;
	float fW = 0.0f;

	KG_PROCESS_ERROR(Lua_GetTopIndex(L) == 2);

	nRetCode = THostWndItem_GetUserData(L, &pTreeLeaf);
	KGLOG_PROCESS_ERROR(nRetCode);
	ASSERT(pTreeLeaf);

	fW = (float)Lua_ValueToNumber(L, 2);

	fW *= g_pUI->m_WndStation.GetUIScale();

	pTreeLeaf->SetEachIndentWidth(fW);

Exit0:
	return 0;
}

int KItemHandleClassScriptTable::LuaItemTreeLeaf_GetEachIndentWidth(Lua_State *L)
{
	ASSERT(L);

	int nRetCode = false;
	KItemTreeLeaf *pTreeLeaf = NULL;
	float fW = 0.0f;

	KG_PROCESS_ERROR(Lua_GetTopIndex(L) == 1);

	nRetCode = THostWndItem_GetUserData(L, &pTreeLeaf);
	KGLOG_PROCESS_ERROR(nRetCode);
	ASSERT(pTreeLeaf);

	fW = pTreeLeaf->GetEachIndentWidth();

	fW /= g_pUI->m_WndStation.GetUIScale();

Exit0:
	Lua_PushNumber(L, fW);
	return 1;
}

int KItemHandleClassScriptTable::LuaItemTreeLeaf_SetNodeIconSize(Lua_State *L)
{
	ASSERT(L);

	int nRetCode = false;
	KItemTreeLeaf *pTreeLeaf = NULL;
	float fW = 0.0f;
	float fH = 0.0f;
	float fScale = 1.0f;

	KG_PROCESS_ERROR(Lua_GetTopIndex(L) == 3);

	nRetCode = THostWndItem_GetUserData(L, &pTreeLeaf);
	KGLOG_PROCESS_ERROR(nRetCode);
	ASSERT(pTreeLeaf);

	fScale = g_pUI->m_WndStation.GetUIScale();

	fW = (float)Lua_ValueToNumber(L, 2) * fScale;
	fH = (float)Lua_ValueToNumber(L, 3) * fScale;

	pTreeLeaf->SetNodeIconSize(fW, fH);

Exit0:
	return 0;
}

int KItemHandleClassScriptTable::LuaItemTreeLeaf_SetIconImage(Lua_State *L)
{
	ASSERT(L);

	int nRetCode = false;
	KItemTreeLeaf *pTreeLeaf = NULL;
	LPCSTR pcszImage = NULL;
	int nExpandFrame = 0;
	int nCollapseFrame = 0; 

	KG_PROCESS_ERROR(Lua_GetTopIndex(L) == 4);

	nRetCode = THostWndItem_GetUserData(L, &pTreeLeaf);
	KGLOG_PROCESS_ERROR(nRetCode);
	ASSERT(pTreeLeaf);

	pcszImage = Lua_ValueToString(L, 2);
	pcszImage = FormatFilePath(pcszImage);
	KGLOG_PROCESS_ERROR(pcszImage);

	nExpandFrame = (int)Lua_ValueToNumber(L, 3);
	nCollapseFrame = (int)Lua_ValueToNumber(L, 4);

    pTreeLeaf->SetIconImage(pcszImage, nExpandFrame, nCollapseFrame);

Exit0:
	return 0;
}

int KItemHandleClassScriptTable::LuaItemTreeLeaf_PtInIcon(Lua_State *L)
{
	ASSERT(L);

	int nRetCode = false;
	KItemTreeLeaf *pTreeLeaf = NULL;
	float fX = 0.0f;
	float fY = 0.0f;
	int nInIcon = false;
	float fScale = 1.0f;

	KG_PROCESS_ERROR(Lua_GetTopIndex(L) == 3);

	nRetCode = THostWndItem_GetUserData(L, &pTreeLeaf);
	KGLOG_PROCESS_ERROR(nRetCode);
	ASSERT(pTreeLeaf);

	fScale = g_pUI->m_WndStation.GetUIScale();

	fX = (float)Lua_ValueToNumber(L, 2) * fScale;
	fY = (float)Lua_ValueToNumber(L, 3) * fScale;

	nInIcon = pTreeLeaf->PtInIcon(fX, fY);

Exit0:
	Lua_PushBoolean(L, nInIcon);
	return 1;
}

int KItemHandleClassScriptTable::LuaItemTreeLeaf_AdjustNodeIconPos(Lua_State *L)
{
	ASSERT(L);

	int nRetCode = false;
	KItemTreeLeaf *pTreeLeaf = NULL;

	KG_PROCESS_ERROR(Lua_GetTopIndex(L) == 1);

	nRetCode = THostWndItem_GetUserData(L, &pTreeLeaf);
	KGLOG_PROCESS_ERROR(nRetCode);
	ASSERT(pTreeLeaf);

	pTreeLeaf->AdjustNodeIconPos();

Exit0:
	return 0;
}

int KItemHandleClassScriptTable::LuaItemTreeLeaf_AutoSetIconSize(Lua_State *L)
{
	ASSERT(L);

	int nRetCode = false;
	KItemTreeLeaf *pTreeLeaf = NULL;

	KG_PROCESS_ERROR(Lua_GetTopIndex(L) == 1);

	nRetCode = THostWndItem_GetUserData(L, &pTreeLeaf);
	KGLOG_PROCESS_ERROR(nRetCode);
	ASSERT(pTreeLeaf);

	pTreeLeaf->AutoSetIconSize();

Exit0:
	return 0;
}

int KItemHandleClassScriptTable::LuaItemTreeLeaf_SetShowIndex(Lua_State *L)
{
	ASSERT(L);

	int nRetCode = false;
	int nShowIndex = -1;
	KItemTreeLeaf *pTreeLeaf = NULL;

	KG_PROCESS_ERROR(Lua_GetTopIndex(L) == 2);

	nRetCode = THostWndItem_GetUserData(L, &pTreeLeaf);
	KGLOG_PROCESS_ERROR(nRetCode);
	ASSERT(pTreeLeaf);

	nShowIndex = (int)Lua_ValueToNumber(L, 2);

	pTreeLeaf->SetShowIndex(nShowIndex);

Exit0:
	return 0;
}

int KItemHandleClassScriptTable::LuaItemTreeLeaf_GetShowIndex(Lua_State *L)
{
	ASSERT(L);

	int nRetCode = false;
	int nShowIndex = -1;
	KItemTreeLeaf *pTreeLeaf = NULL;

	KG_PROCESS_ERROR(Lua_GetTopIndex(L) == 1);

	nRetCode = THostWndItem_GetUserData(L, &pTreeLeaf);
	KGLOG_PROCESS_ERROR(nRetCode);
	ASSERT(pTreeLeaf);

	nShowIndex = pTreeLeaf->GetShowIndex();

Exit0:
	Lua_PushNumber(L, nShowIndex);
	return 1;
}

}