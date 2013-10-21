////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : IEEditorTypes.h
//  Version     : 1.0
//  Creator     : Chen Tianhong
//  Create Date : 2008-12-18 16:39:33
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _INCLUDE_IEEDITORTYPES_H_
#define _INCLUDE_IEEDITORTYPES_H_

////////////////////////////////////////////////////////////////////////////////
enum
{
     SCENESTATE_SELECT	= 0,
     SCENESTATE_MOVE    = 1,
     SCENESTATE_ROTATE  = 2,
     SCENESTATE_SCALE   = 3,
     SCENESTATE_COPY    = 4,
     SCENESTATE_SELECTREGION   = 5,  //选择REGION
     SCENESTATE_SELECTSECTION  = 6,  //选择SECTION
     SCENESTATE_SELECTZONE     = 7,  //选择ZONE
	
     SCENESTATE_ADDPOLYVERTEX  = 9,  //添加多边形顶点

     SCENESTATE_SFX_GEO  = 10,

     SCENESTATE_TERRAIN  = 100,   //修改地形

     SCENESTATE_PLACEOBJ = 200,  //放置物件

     SCENESTATE_PLACEUNIT = 500,   //放置NPC DOODA
     SCENESTATE_PLACENPCEDITOR = 501,   //放置编辑器内建NPC

     SCENESTATE_CELLLOGICAL		= 400,   //放置物件
     SCENESTATE_CAMERAANIMATION  = 700,   //放置物件
     SCENESTATE_BENCHMARK = 713,

     SCENESTATE_SFX_ADD_LAUNCHER = 300,
     SCENESTATE_SFX_ADD_BILLBOARD = 301,


     SCENESTATE_TERRAINSELECTION   = 401,//场景选择
	//     SCENESTATE_TERRAINPASTE  = 402,//场景复制
     SCENESTATE_TERRAINSELECTIONMODIFY = 403, //场景选择范围编辑状态
     SCENESTATE_COPY_REPRESENTOBJECT = 404, // 场景复制对象 add by suntao

	 SCENESTATE_NONE = 1000,
};

//ExecuteAction用的ID
enum
{
	//界面操作
	EXEACTION_ROTATE_CAMERA	= 1,	//旋转摄像机
	EXEACTION_ZOOM_VIEW		= 2,	//缩放视图
	EXEACTION_PAN_VIEW		= 3,	//Pan视图
	EXEACTION_ROTATE_VIEW	= 4,	//旋转视图
	EXEACTION_INVERT_ACTION = 5,	//反向操作   

	//NPC 
	EXEACTION_GO_FORWARD	= 6,	//向前走
	EXEACTION_GO_BACKWARD	= 7,	//向后
	EXEACTION_TURN_LEFT		= 8,	//转左
	EXEACTION_TURN_RIGHT	= 9,	//右
	EXEACTION_MOV_LEFT		= 10,	//左移
	EXEACTION_MOV_RIGHT		= 11,
	EXEACTION_CAM_CIRCLE	= 12,	//摄像机转圈
	EXEACTION_JUMP			= 13,	//跳
	EXEACTION_TURN_LEFT_VIEW_FOLLOWED = 14,		//左转，摄像机跟随
	EXEACTION_TURN_RIGHT_VIEW_FOLLOWED = 15,	//右转，摄像机跟随

	//画笔
	EXEACTION_PRESURE		= 16,	//压感
	EXEACTION_INNER_CIRCLE	= 17,	//内圈大小
	EXEACTION_OUTTER_CIRCLE = 18,	//外圈

	//特殊
	EXEACTION_LEFE_KEY_DOWN = 19,	//左键按下
	EXEACTION_SELECTION_ADD = 20,	//加选
	EXEACTION_SELECTION_SUTRACT = 21,//减选

	EXEACTION_SMOOTH_TERRAIN = 23,	//平滑地形
	
	EXEACTION_SPEED_UP		= 24,	//加速
	EXEACTION_SPEED_DOWN	= 25,	//减速

	EXEACTION_MOV_UP			= 26,	//上升
	EXEACTION_MOV_DOWN			= 27,	//下降

	EXEACTION_MOUSE_MOVE		= 30,	//鼠标移动，在移动旋转放缩的状态下面要检查坐标。不和其它键配合。state是左键是否按下。WPARAM和LPARAM都对应WM_MOUSEMOVE，wParam可以得到鼠标状态，lParam，low是x，high是y
	EXEACTION_MOUSE_WHEEL		= 31,	//中键，WPARAM和LPARAM都对应WindowsMessage

	EXEACTION_ZOOM_TO_OBJECT = 1001,	//缩放镜头到物体大小
	EXEACTION_PLAY_CAM_ANI = 1002,	//播放镜头动画

	// 策划场景编辑器
	//EXEACTION_DE_FORWARD		= 2000,
	//EXEACTION_DE_BACKWARD		= 2001,
	//EXEACTION_DE_LEFT			= 2002,
	//EXEACTION_DE_RIGHT			= 2003,
	EXEACTION_DE_MOVE_ANEAR_GROUND = 2004, // 物件贴地

	//1234(QWER)
	EXEACTION_SELECT            = 2100,
	EXEACTION_MOVE              = 2101,
	EXEACTION_ROTATE            = 2102,
	EXEACTION_SCALE             = 2103,
};

enum
{
	EM_EDITOR_TOOL_STATE_NOTATIVATED,
	EM_EDITOR_TOOL_STATE_ATIVATED,
	EM_EDITOR_TOOL_STATE_TRANSFORMING,
};

enum	//小心这些东西的更改，造成存盘丢失事情就大了
{
	EM_SAVE_OP_TERRAIN_AND_ENTITY = 1,
	EM_SAVE_OP_SCENE_SETTINGS = 1 << 1,
	EM_SAVE_OP_CAMERA_ANIMATION = 1 << 2,
	EM_SAVE_OP_REGION_INFO = 1 << 3,
	EM_SAVE_OP_ENVIRONMENT = 1 << 4,
    EM_SAVE_OP_EFFECTPARAMS = 1 << 5,
	//EM_SAVE_OP_ROAD = 1 << 6,
	//EM_SAVE_OP_RIVER = 1 << 7,	
	EM_SAVE_OP_COUNT = 6,	//新加的类型加在这个上面，并需要更新EM_SAVE_OP_COUNT，另外在g_GetSaveOptionDesc写上文字描述可以在界面中自动生成多选
	EM_SAVE_OP_ALL = (1 << EM_SAVE_OP_COUNT) - 1,	//只要EM_SAVE_OP_COUNT是对的，这样子运算就可以搞出低位全是1的值，而且刚好等于所有Op的或运算。保存选项面板要用到“EM_SAVE_OP_ALL等于所有Op的或运算”这个假定，所以不要乱改这个值的运算方式
};

struct KEditorOptionDesc 
{
	DWORD dwOp;
	LPCTSTR	strDesc;
};

typedef KEditorOptionDesc (&TypeSaveOptionDescArrayRef)[EM_SAVE_OP_COUNT];	//这是一个对数组的引用的声明语法

static TypeSaveOptionDescArrayRef g_GetSaveOptionDesc()	//这些值放在这里，
{
	static KEditorOptionDesc descSaveOption[EM_SAVE_OP_COUNT] = 
	{
		{EM_SAVE_OP_TERRAIN_AND_ENTITY, _T("地形和物件")},
		{EM_SAVE_OP_SCENE_SETTINGS, _T("SceneSettings")},
		{EM_SAVE_OP_CAMERA_ANIMATION, _T("摄像机动画")},
		{EM_SAVE_OP_REGION_INFO, _T("区域分块")},
		{EM_SAVE_OP_ENVIRONMENT, _T("环境参数,天气")},
        {EM_SAVE_OP_EFFECTPARAMS, _T("PostRender参数")},
		//{EM_SAVE_OP_ROAD, _T("道路")},
		//{EM_SAVE_OP_RIVER, _T("河流")},		
	};
	return descSaveOption;
}

#endif //_INCLUDE_IEEDITORTYPES_H_
