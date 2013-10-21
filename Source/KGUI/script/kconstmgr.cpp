#include "stdafx.h"
#include "KG3DEngine/KG3DInterface.h"
#include "./kconstmgr.h"
#include "./kwndclassscripttable.h"
#include "./kscriptloader.h"
#include "../elem/wndframe.h"
#include "../elem/autolocatewnd.h"
#include "../elem/Components/item.h"
#include "../elem/Components/itemimage.h"
#include "../KGUI.h"
#include "../common/IKG_TabFile.h"

namespace UI
{

	bool KConstMgr::Load(lua_State* L)
	{
		BOOL bRetCode = FALSE;

		static KLuaConst s_aWndSideConst[] =
		{
			{ "TOP",										WND_SIDE_TOP },
			{ "BOTTOM",										WND_SIDE_BOTTOM },
			{ "LEFT",										WND_SIDE_LEFT },
			{ "RIGHT",										WND_SIDE_RIGHT },
			{ "TOPLEFT",									WND_SIDE_TOPLEFT },
			{ "TOPRIGHT",									WND_SIDE_TOPRIGHT },
			{ "BOTTOMLEFT",									WND_SIDE_BOTTOMLEFT },
			{ "BOTTOMRIGHT",								WND_SIDE_BOTTOMRIGHT },
			{ "CENTER",										WND_SIDE_CENTER },
			{ "LEFTCENTER",									WND_SIDE_LEFTCENTER },
			{ "RIGHTCENTER",								WND_SIDE_RIGHTCENTER },
			{ "TOPCENTER",									WND_SIDE_TOPCENTER },
			{ "BOTTOMCENTER",								WND_SIDE_BOTTOMCENTER },
			{ NULL,											NULL },
		};

		static KLuaConst s_aAWLConst[] = 
		{
			{ "CENTER",										ALW_CENTER },
			{ "LEFT_RIGHT",									ALW_LEFT_RIGHT },
			{ "RIGHT_LEFT",									ALW_RIGHT_LEFT },
			{ "TOP_BOTTOM",									ALW_TOP_BOTTOM },
			{ "BOTTOM_TOP",									ALW_BOTTOM_TOP },
			{ "RIGHT_LEFT_AND_BOTTOM_TOP",					ALW_RIGHT_LEFT_AND_BOTTOM_TOP },

			{ NULL,											NULL },
		};

		static KLuaConst s_ItemPositionConst[] = 
		{
			{ "BY_CUSTOM",									POSITION_BY_CUSTOM },
			{ "LEFT_BOTTOM",								POSITION_LEFT_BOTTOM },
			{ "LEFT_TOP",									POSITION_LEFT_TOP },
			{ "LEFT_CENTER",								POSITION_LEFT_CENTER },
			{ "TOP_LEFT",									POSITION_TOP_LEFT },
			{ "TOP_RIGHT",									POSITION_TOP_RIGHT },
			{ "TOP_CENTER",									POSITION_TOP_CENTER },
			{ "RIGHT_TOP",									POSITION_RIGHT_TOP },
			{ "RIGHT_BOTTOM",								POSITION_RIGHT_BOTTOM },
			{ "RIGHT_CENTER",								POSITION_RIGHT_CENTER },
			{ "BOTTOM_LEFT",								POSITION_BOTTOM_LEFT },
			{ "BOTTOM_RIGHT",								POSITION_BOTTOM_RIGHT },
			{ "BOTTOM_CENTER",								POSITION_BOTTOM_CENTER },

			{ NULL,											NULL },
		};


		static KLuaConst s_CursorStateConst[] = 
		{
			{ "NORMAL",										CURSOR_NORMAL },		
			{ "CAST",										CURSOR_CAST },
			{ "UNABLECAST",									CURSOR_UNABLECAST },
			{ "TRAVEL",										CURSOR_TRAVEL },
			{ "UNABLETRAVEL",								CURSOR_UNABLETRAVEL },
			{ "SELL",										CURSOR_SELL },
			{ "UNABLESELL",									CURSOR_UNABLESELL },
			{ "BUYBACK",									CURSOR_BUYBACK },
			{ "UNABLEBUYBACK",								CURSOR_UNABLEBUYBACK },
			{ "REPAIRE",									CURSOR_REPAIRE },
			{ "UNABLEREPAIRE",								CURSOR_UNABLEREPAIRE },
			{ "ATTACK",										CURSOR_ATTACK },			
			{ "UNABLEATTACK",								CURSOR_UNABLEATTACK },	
			{ "SPEAK",										CURSOR_SPEAK },			
			{ "UNABLESPEAK",								CURSOR_UNABLESPEAK },		
			{ "LOOT",										CURSOR_LOOT },			
			{ "UNABLELOOT",									CURSOR_UNABLELOOT },		
			{ "LOCK",										CURSOR_LOCK },			
			{ "UNABLELOCK",									CURSOR_UNABLELOCK },		
			{ "INSPECT",									CURSOR_INSPECT },			
			{ "UNABLEINSPECT",								CURSOR_UNABLEINSPECT },	
			{ "SPLIT",										CURSOR_SPLIT },			
			{ "UNABLESPLIT",								CURSOR_UNABLESPLIT },		
			{ "FLOWER",										CURSOR_FLOWER },			
			{ "UNABLEFLOWER",	    						CURSOR_UNABLEFLOWER },	
			{ "MINE",			    						CURSOR_MINE },			
			{ "UNABLEMINE",									CURSOR_UNABLEMINE },		
			{ "SEARCH",										CURSOR_SEARCH },			
			{ "UNABLESEARCH",	    						CURSOR_UNABLESEARCH },	
			{ "QUEST",			    						CURSOR_QUEST },			
			{ "UNABLEQUEST",								CURSOR_UNABLEQUEST },		
			{ "READ",			    						CURSOR_READ },			
			{ "UNABLEREAD",									CURSOR_UNABLEREAD },

			{ "MARKPRICE",									CURSOR_MARKPRICE },

			{ "TOP_BOTTOM",									CURSOR_TOP_BOTTOM },
			{ "LEFT_RIGHT",									CURSOR_LEFT_RIGHT },
			{ "LEFTTOP_RIGHTBOTTOM",						CURSOR_LEFTTOP_RIGHTBOTTOM },
			{ "RIGHTTOP_LEFTBOTTOM",						CURSOR_RIGHTTOP_LEFTBOTTOM },
			{ "CURSOR_MOVE",								CURSOR_MOVE },

			{ "HAND_OBJECT",								CURSOR_HAND_OBJECT },		

			{ NULL,											NULL },
		};

		static KLuaConst s_MovieConstTable[] = 
		{
			{ "FILTER_NOT",									IPPSPEC_FILTER_NOT },
			{ "FILTER_CUBIC",								IPPSPEC_FILTER_CUBIC },
			{ "FILTER_LINEAR",								IPPSPEC_FILTER_LINEAR },

			{ "QUALITY_PIRATE",								IPPSPEC_QUALITY_PIRATE },
			{ "QUALITY_DISC",								IPPSPEC_QUALITY_DISC },
			{ "QUALITY_CINEMATIC",							IPPSPEC_QUALITY_CINEMATIC },
			{ "QUALITY_CINEMATIC1",							IPPSPEC_QUALITY_CINEMATIC1 },
			{ "QUALITY_CINEMATIC2",							IPPSPEC_QUALITY_CINEMATIC2 },
			{ "QUALITY_CINEMATIC3",							IPPSPEC_QUALITY_CINEMATIC3 },
			{ "QUALITY_CINEMATIC4",							IPPSPEC_QUALITY_CINEMATIC4 },
			{ "QUALITY_MAX",								IPPSPEC_QUALITY_MAX },

			{ "H261",										IPPSPEC_H261 },
			{ "H263",										IPPSPEC_H263 },
			{ "H264",										IPPSPEC_H264 },
			{ "MPEG2",										IPPSPEC_MPEG2 },
			{ "MPEG4",										IPPSPEC_MPEG4 },

			{ "FPS_25",										IPPSPEC_FPS_25 },
			{ "FPS_30",										IPPSPEC_FPS_30 },
			{ "FPS_50",										IPPSPEC_FPS_50 },
			{ "FPS_60",										IPPSPEC_FPS_60 },

			{ NULL,											NULL },
		};

		static KLuaConst s_SoundConstTable[] = 
		{
			{ "BG_MUSIC",									BG_MUSIC },
			{ "UI_SOUND",									UI_SOUND },
			{ "UI_ERROR_SOUND",								UI_ERROR_SOUND },
			{ "SCENE_SOUND",								SCENE_SOUND },
			{ "CHARACTER_SOUND",							CHARACTER_SOUND },
			{ "CHARACTER_SPEAK",							CHARACTER_SPEAK },

			{ NULL,											NULL },
		};

		static KLuaConst s_LoginReasonConstTable[] = 
		{
			{ "START_GAME_LOGIN",							START_GAME_LOGIN },
			{ "RETURN_GAME_LOGIN",							RETURN_GAME_LOGIN },
			{ "RETURN_ROLE_LIST",							RETURN_ROLE_LIST },
			{ "RETURN_CLEAR_UI",							RETURN_CLEAR_UI },
            { "KICK_OUT_BY_GM",                             KICK_OUT_BY_GM},
            { "KICK_OUT_BY_OTHERS",                         KICK_OUT_BY_OTHERS},
            { "AUTO_EXIT",                                  AUTO_EXIT},

			{ NULL,											NULL },
		};

		static KLuaConst s_ImageTypeConstTable[] = 
		{
			// TODO: modify script const name
			{ "NORMAL",										ITEMIMAGE_NORMAL },
			{ "LEFT_RIGHT",									ITEMIMAGE_PART_LEFT_TO_RIGHT },
			{ "RIGHT_LEFT",									ITEMIMAGE_PART_RIGHT_TO_LEFT },
			{ "TOP_BOTTOM",									ITEMIMAGE_PART_TOP_TO_BOTTOM },
			{ "BOTTOM_TOP",									ITEMIMAGE_PART_BOTTOM_TO_TOP },
			{ "TIMER",										ITEMIMAGE_PART_TIMER },
			{ "ROTATE",										ITEMIMAGE_ROTATE },
			{ "FLIP_HORIZONTAL",							ITEMIMAGE_FLIP_HORIZONTAL },
			{ "FLIP_VERTICAL",								ITEMIMAGE_FLIP_VERTICAL },
			{ "FLIP_CENTRAL",								ITEMIMAGE_FLIP_CENTRAL },

			{ "NINE_PART",									ITEMIMAGE_NINE_PART },
			{ "LEFT_CENTER_RIGHT",							ITEMIMAGE_LEFT_CENTER_RIGHT },
			{ "TOP_CENTER_BOTTOM",							ITEMIMAGE_TOP_CENTER_BOTTOM },

			{ NULL,											NULL },
		};

        static KLuaConst s_aFileOpenModeConstTable[] = 
        {
            { "NORMAL",                                      KG_FILE_OPEN_NORMAL },
            { "USE_FILE_MAPPING",                            KG_FILE_OPEN_FILE_MAPPING },
            
            { NULL,                                          NULL },
        };

		static KLuaConstList s_aConstList[] = 
		{
			{ "WNDSIDE",									s_aWndSideConst },
			{ "ALW",										s_aAWLConst },
			{ "ITEM_POSITION",								s_ItemPositionConst },
			{ "CURSOR",										s_CursorStateConst },
			{ "MOVIE",										s_MovieConstTable },
			{ "SOUND",										s_SoundConstTable },
			{ "LOAD_LOGIN_REASON",							s_LoginReasonConstTable },
			{ "IMAGE",										s_ImageTypeConstTable },
            { "FILE_OPEN_MODE",                             s_aFileOpenModeConstTable },

			{ NULL,											NULL },
		};

		ASSERT(L);

		KScriptLoader::RegisterConstList(L, s_aConstList);

		return true;
	}

}


