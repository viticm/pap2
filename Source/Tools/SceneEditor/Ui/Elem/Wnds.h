/*****************************************************************************************
//	剑侠引擎，界面窗口元素体系头文件
//	Copyright : Kingsoft 2002
//	Author	:   Wooy(Wu yue)
//	CreateTime:	2002-7-9
------------------------------------------------------------------------------------------
	界面元素（控件）的共同基类为KWndWindow，Wnd_前缀打头的一组函数用于使窗口形成一个大集合，
完成一些特定的功能。
    窗口系统支持热键(HotKey)功能，最多同时支持24个热键设置，此上限数值可被修改。
*****************************************************************************************/

class KWndWindow;
struct KUiDraggedObject;
struct iKWndGameSpace;

enum CURSOR_INDEX
{
	CURSOR_NORMAL = 0,
	CURSOR_POINT_TO_ATTACKABLE,	//移到攻击目标上
	CURSOR_POINT_TO_OBJ_NPC,	//移到一般OBJ或NPC上
	CURSOR_PICKABLE,			//可拾取
	CURSOR_SKILL_ENABLE,		//移到可使用增益技能的角色对象身上
	CURSOR_BUY,					//买东西
	CURSOR_SELL,				//卖东西
	CURSOR_REPAIR,				//修东西
    CURSOR_USE,                 //看对象的图标
	CURSOR_QUERY_STALL,			//看摊位
	CURSOR_STALL_MARK_PIRCE,	//为摆摊标价
	CURSOR_INDEX_COUNT,
};

//--------------------------------------------------------------------------
//	绘制被拖内容函数原型
//--------------------------------------------------------------------------
//	参数：	x, y -> 鼠标指针当前的坐标位置
//			Obj -> 被拖动的对象信息结构
//			nDropQeuryResult -> 当前目标地方返回的查询可否接受拖来的对象的结果
//	返回：表示绘制完被拖动的物体之后是否要继续绘制鼠标指针。如果鼠标指针单前属于隐藏状态则，此函数返回值被忽略
typedef int (*fnDrawDraggedObj)(int x, int y, const KUiDraggedObject& Obj, int nDropQueryResult);

enum WND_LAYER_LIST
{
	WL_LOWEST,		//最底层
	WL_NORMAL,		//一般
	WL_TOPMOST,		//最顶层
};

void		Wnd_Cleanup();			//清除工作，销毁全部的窗口

void		Wnd_GetScreenSize(int& nWidth, int& nHeight);//获取屏幕范围大小
void		Wnd_SetScreenSize(int nWidth, int nHeight);  //设置屏幕范围大小
void		Wnd_RenderWindows();						//绘制界面系统
void		Wnd_AddWindow(KWndWindow* pWnd, WND_LAYER_LIST eLayer = WL_NORMAL);	//添加窗口
void		Wnd_OnWindowDelete(KWndWindow* pWnd);		//响应窗口的删除
void		Wnd_SetGameSpaceWnd(KWndWindow* pWnd);	//设置运行游戏窗口
void		Wnd_GameSpaceHandleInput(bool bHandle);
BOOL		Wnd_IsReceiveInput();
void		Wnd_TransmitInputToGameSpace(unsigned int uMsg, unsigned int uParam, int nParam);
void		Wnd_ShowHideGameSpace(bool bShow);
//----输入事件处理----
//处理输入，界面系统如果处理了输入事件，则返回非0值，否则返回0值。
void		Wnd_ProcessInput(unsigned int uMsg, unsigned int uParam, int nParam);
void		Wnd_Heartbeat();
//----鼠标指针操作----
void		Wnd_ShowCursor(int bShow);						//改变鼠标指针显示状态
void		Wnd_SetCursorPos(int x, int y);					//设置鼠标指针位置
void		Wnd_GetCursorPos(int* px, int* py);				//获取鼠标指针位置
void		Wnd_LoadCursor(int nIndex, const char* pImgFile);//载入鼠标指针资源
int			Wnd_SwitchCursor(int nIndex);					//切换当前鼠标指针图形
int			Wnd_GetCurrentCursor();							//获得当前鼠标指针图形
void		Wnd_RestoreCursor();							//还原鼠标图形
//----拖动对象----
//开始拖动物体
int			Wnd_DragBegin(const KUiDraggedObject* pObj, fnDrawDraggedObj fnDrawObjFunc);
int			Wnd_GetDragObj(KUiDraggedObject* pObj);
void		Wnd_DragFinished();								//结束拖动状态
//----处理输入焦点窗口----
void		Wnd_SetFocusWnd(KWndWindow* pWnd);				//设置输入焦点窗口
KWndWindow*	Wnd_GetFocusWnd();								//获取输入焦点窗口
//----跟踪鼠标事件----
void		Wnd_SetCapture(KWndWindow* pWnd, bool bDontSendToChild = true);//设置跟踪鼠标动作窗口
void		Wnd_ReleaseCapture();							//释放窗口对鼠标动作的跟踪
//----处理窗口独占----
void		Wnd_SetExclusive(KWndWindow* pWnd);				//设置独占窗口
void		Wnd_ReleaseExclusive(KWndWindow* pWnd);			//释放窗口的独占状态
