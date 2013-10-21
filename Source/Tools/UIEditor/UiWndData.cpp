#include "Stdafx.h"
#include "UiWndData.h"

static	const char* s_pTypeString[UI_WND_TYPE_COUNT][3] = 
{
    { "WndWindow",		            "虚窗体",		        "Wnd"		            },
    { "WndImage",		            "图像",		            "Img"		            },
    { "WndButton",		            "按钮",			        "Btn"		            },
    { "WndNewScrollBar",	        "滚动条",		        "Scroll"		        },
    { "WndEdit",	                "文字编辑框",	        "Edit"		            },
    { "WndShowAnimate",             "动画显示",             "ShowAni"		        },
    { "WndPage",                    "页面",                 "Page"		            },
    { "WndPageSet",                 "页面设置",             "PageSet"		        },
    { "WndFrame",                   "窗口",                 "Frame"                 },
    { "WndCheckBox",                "勾选框",               "CheckBox"              },
    { "WndModel",					"3D模型渲染窗口",       "3DModel"				},
    { "WndTexture",					"3D贴图渲染窗口",       "3DTexture"             },
    { "WndMinimap",					"小地图",				"Minimap"               },
    { "WndScene",					"游戏场景",				"Scene"                 },
    { "WndWebPage",					"内嵌网页",				"WebPage"               },
                                                 
    { "Null",                       "空组件",              	"Null"                  },
    { "Text",                       "文字组件",            	"Text"                  },
    { "Image",                      "图片组件",            	"Image"                 },
    { "Shadow",                     "阴影组件",            	"Shadow"                },
    { "Animate",                    "动画组件",            	"Animate"               },
	{ "Box",						"拖拽框",            	"Box"                   },
    { "Sence",                      "场景组件",            	"Sence"                 },
    { "Handle",                     "容器组件",            	"Handle"                },
    { "TreeLeaf",                   "树形结构一项",       	"TreeLeaf"              },
};


int	g_GetWndType(const char* pszType, UI_WND_TYPE& Type)
{
	if (pszType && pszType[0])
	{
		for (int i = 0; i < UI_WND_TYPE_COUNT; i++)
		{
			if (!stricmp(s_pTypeString[i][0], pszType))
			{
				Type = (UI_WND_TYPE)i;
				return true;
			}
		}
	}
	Type = UI_WND_WINDOW;
	return false;
}

int g_GetWndType(const char* pszType)
{
	if (pszType && pszType[0])
	{
		for (int i = 0; i < UI_WND_TYPE_COUNT; i++)
		{
			if (stricmp(s_pTypeString[i][0], pszType) == 0) 
				return i;
		}
	}

	return UI_WND_TYPE_UNKNOWN;
}

const char* g_GetWndTypeKey(UI_WND_TYPE Type)
{
	if (Type >= 0 && Type < UI_WND_TYPE_COUNT)
		return s_pTypeString[Type][0];
	return "";
}

const char*	g_GetWndTypeString(UI_WND_TYPE Type)
{
	if (Type >= 0 && Type < UI_WND_TYPE_COUNT)
		return s_pTypeString[Type][1];
	return "";
}

const char *g_GetWndTypePrefix(UI_WND_TYPE Type)
{
	if (Type >= 0 && Type < UI_WND_TYPE_COUNT)
		return s_pTypeString[Type][2];
	return "";

}
