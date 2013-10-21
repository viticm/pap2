////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KG3DMessenger.h
//  Version     : 1.0
//  Creator     : Chen Tianhong
//  Create Date : 2006-11-12 10:53:25
//  Comment     : 
/*
用于在各个模块之间传递信息。创建在各个模块之间交换信息的一种机制。
主要用于解决一个资源向资源的拥有者传递消息的问题。一个资源不应该知道它的拥有者是什么
而它的拥有者可以要求资源向其主动报告状态。继承他们的类需要写出OnReceiveMessage之类。

一种类似Observer模式的机制。
Sender是被观察者，向所有Receiver发消息
Receiver是观察者，接受消息
MessageSender和MessageReceiver分别作为
消息的发送者和接受者的基类。用Sender的RegisterReceiver注册了Receiver
的指针之后，Sender可以调用PostMessage来发送消息，Receiver被调用PickMessage。
PickMessage预处理了一些注册，反注册之类的消息之后，通知OnReceiveMessage
Receiver的派生类通过写OnReceiveMessage来处理消息。

要注意的是Sender和Receiver内部处理了对方的析构事件，生命周期无关，
任何一个析构都会通知另外一个清除引用。所以覆盖OnReceiveMessage的时候，可以放心使用
Sender的指针

用法：

1.消息发送者，或者说是被观察者，从KG3DMessageSingleReceiverSender或者KG3DMessageSender派生，
什么也不用覆盖
2.写一个消息的代号，见下面KM_的枚举值。请不要覆盖已经存在的枚举。
3.消息接受者，或者说是观察者，从KG3DMessageSingleSenderReceiver或者KG3DMessageReceiver派生，
覆盖OnReceiveMessage来接受消息，接受到之后自行定义消息的类型和如何处理
4.在适当的时候，如观察者或者被观察者Init的时候，调用Sender.RegisterReceiver(&Receiver);
让接受者注册到发送者。反注册在析构的时候会自动进行，也可以手动反注册。双方析构的时候都会清除
在对方处的引用，不会发生无效指针。
5.在你需要发送消息的时候，调用发送者的PostMessage就可以了
6.用GetReceiver和GetSender可以得到对应的接受者和发送者，但是最后只在类成员内部访问它们
*/
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _INCLUDE_IKG3DMESSENGER_H_
#define _INCLUDE_IKG3DMESSENGER_H_

////////////////////////////////////////////////////////////////////////////////
struct KG3DMessageActionExecutionStruct 
{
	INT	nActionID;	//参考EXEACTION_XXX
	INT nExecState;//按键现在是正在按下，还是弹起，参考InputProxy
	WPARAM wParam;
	LPARAM lParam;	//如果是WM_KEYFIRST和WM_KEYLAST之间且Message不是WM_MOUSEMOVE,或者nExecState是EXEACTION_LEFE_KEY_DOWN int xPos = LOWORD(lParam); int yPos = HIWORD(lParam);		
	KG3DMessageActionExecutionStruct(){ZeroMemory(this, sizeof(KG3DMessageActionExecutionStruct));}
	KG3DMessageActionExecutionStruct(INT nActionID, INT State, WPARAM wP, LPARAM lP):nActionID(nActionID), nExecState(State),wParam(wP), lParam(lP){}
	KG3DMessageActionExecutionStruct(const KG3DMessageActionExecutionStruct& Other)	{memcpy_s(this, sizeof(KG3DMessageActionExecutionStruct), &Other, sizeof(KG3DMessageActionExecutionStruct));}
	KG3DMessageActionExecutionStruct& operator=(const KG3DMessageActionExecutionStruct& Other)	{memcpy_s(this, sizeof(KG3DMessageActionExecutionStruct), &Other, sizeof(KG3DMessageActionExecutionStruct)); return *this;}
};

struct KG3DMessageSceneLoadingPercent 
{
	FLOAT	TerrainLoadingPercent;
	FLOAT	ObjectLoadingPercent;
	FLOAT	RestLoadingPercent;
	KG3DMessageSceneLoadingPercent(){ZeroMemory(this, sizeof(KG3DMessageSceneLoadingPercent));}
};

//Message的类型
typedef enum _KM_MESSEGE
{
	KM_NONE = 0,

	KM_MSG_SENDER_CONSTRUCTED = 10,
	KM_MSG_SENDER_DESTRUCTED = 11,
	KM_MSG_SENDER_REGISTERED = 12,
	KM_MSG_SENDER_UNREGISTERED = 13,
	//KM_MSG_SENDER_REQUEST_DESTRUCTION = 17,
	
	KM_MSG_POSTMAN_DESTRUCTED = 18,
	KM_MSG_POSTMAN_ANOTHER_HOLDER_READY = 19,
	KM_TEXTURE_RELOADED = 20,
	KM_TIME_ON_SOON_REACH_HOUR = 30,///第二个参数是时间
	KM_TIME_ON_REACH_HOUR = 31,		///同上
	KM_REGION_CHANGED = 35,			///第二个参数是区域代表的Index
	KM_DATA_SYNCHRONIZE = 40,
	KM_DATA_REQUEST_FOR_CONTROL = 41,


	KM_LISTENER_TYPE_BEGIN = 90,
	KM_D3DDEVICE_BEGIN = 90,
	KM_D3DDEVICE_CREATE = KM_D3DDEVICE_BEGIN,
	KM_D3DDEVICE_RELEASE = KM_D3DDEVICE_BEGIN + 1,
	KM_D3DDEVICE_LOST = KM_D3DDEVICE_BEGIN + 2,
	KM_D3DDEVICE_RESET = KM_D3DDEVICE_BEGIN + 3,
	KM_D3DDEVICE_END = KM_D3DDEVICE_BEGIN + 4,

	KM_WINDOWS_MESSAGE = 100,		//第二个参数是KG3DWindowsMessage*
	KM_MOUSE_MOUSE = 101,			//特殊消息，wParam和lParam和WM_MOUSE_MOVE相同
	KM_EXECUTE_ACTION = 103,		//第二个参数是KG3DMessageActionExecutionStruct指针，参考EXEACTION_MOUSE_MOVE等对参数的定义。第三个参数是发出消息的窗口IEKG3DSceneOutputWnd的指针，不保证非空。
	KM_RENDER = 104,
	KM_FRAMEMOVE = 105,
	KM_STATE_CHANGE = 106,			//第一个参数是新的State，第二个参数是旧的State
	KM_COMMON_CONTAINER_UPDATED = 107,//用来接收Container被刷新的消息
	KM_LOADING_PERCENT = 108,
	KM_LISTENER_TYPE_END = 110,	//20个，从KM_LISTENER_TYPE_BEGIN，这些消息可以用来做Listener的申请
	KM_LISTENER_TYPE_COUNT = KM_LISTENER_TYPE_END - KM_LISTENER_TYPE_BEGIN,

	KM_SELECT = 300,		//lParam是选择的状态，EM_EVENT_STATE_BEGIN等
	KM_TRANSFORM_TYPE_BEGIN = 301,
	KM_TRANSLATION = KM_TRANSFORM_TYPE_BEGIN,	//wParam是D3DXVECTOR3*，lParam是EM_EVENT_STATE_BEGIN等
	KM_ROTATION = 302,		//wParam是D3DXQUATERNION*，lParam是EM_EVENT_STATE_BEGIN等
	KM_SCALING	= 303,		//wParam是D3DXVECTOR3*，lParam是EM_EVENT_STATE_BEGIN等
	KM_TRANSFORM_TYPE_END = KM_SCALING,

	KM_CAM_POS = 310,		//摄像机位置变化，wParam是D3DXVECTOR3*

	KM_SCENE_LOADING_PERCENT = 400,	//wParam是KG3DMessageSceneLoadingPercent*
}KM_MESSEGE;

//KG3DMessage结构	
struct KG3DMessage
{
	unsigned int m_uMessage;
	DWORD_PTR m_dwWParam;
	DWORD_PTR m_dwLParam;

	KG3DMessage(){ZeroMemory(this,sizeof(KG3DMessage));}
	KG3DMessage( unsigned int uMessage, DWORD_PTR dwWParam = 0, DWORD_PTR dwLParam = 0 ):m_uMessage(uMessage),m_dwWParam(dwWParam),m_dwLParam(dwLParam){}
	inline bool operator==(const KG3DMessage& Other){return 0==memcmp(this, &Other, sizeof(KG3DMessage));}
	inline bool operator!=(const KG3DMessage& Other){return ! operator==(Other);}
	inline KG3DMessage& operator=(const KG3DMessage& Other){memcpy_s(this, sizeof(KG3DMessage), &Other, sizeof(KG3DMessage)); return *this;}
};
struct KG3DWindowsMessage	//注意，这个和KG3DMessage是没有关系的
{
	unsigned int m_uMessage;
	DWORD_PTR m_dwWParam;
	DWORD_PTR m_dwLParam;

	KG3DWindowsMessage(){ZeroMemory(this,sizeof(KG3DWindowsMessage));}
	KG3DWindowsMessage( unsigned int uMessage, DWORD_PTR dwWParam = 0, DWORD_PTR dwLParam = 0 ):m_uMessage(uMessage),m_dwWParam(dwWParam),m_dwLParam(dwLParam){}
	inline bool operator==(const KG3DWindowsMessage& Other){return 0==memcmp(this, &Other, sizeof(KG3DWindowsMessage));}
	inline bool operator!=(const KG3DWindowsMessage& Other){return ! operator==(Other);}
	inline KG3DWindowsMessage& operator=(const KG3DWindowsMessage& Other){memcpy_s(this, sizeof(KG3DWindowsMessage), &Other, sizeof(KG3DWindowsMessage)); return *this;}
};
/************************************************************************/
//接口声明
/************************************************************************/
struct IKG3DMessageSenderBase;
struct IKG3DMessageReceiverBase;

//////////////////////////////////////////////////////////////////////////
struct IKG3DMessageReceiverBase 
{
	virtual void __stdcall OnReceiveMessage( const KG3DMessage& Message , IKG3DMessageSenderBase* pSender ) = 0;
	virtual ~IKG3DMessageReceiverBase() = 0{}
};
//////////////////////////////////////////////////////////////////////////
struct IKG3DMessageSenderBase 
{
	virtual bool __stdcall RegisterReceiver( IKG3DMessageReceiverBase* pReceiver ) = 0;
	virtual bool __stdcall UnRegisterReceiver( IKG3DMessageReceiverBase* pReceiver ) = 0;
	virtual ~IKG3DMessageSenderBase() = 0{}
};
	
////////////////////////////////////////////////////////////////////////////////
/************************************************************************/
//Sender和Receive的虚基类。I字头的接口因为接口规范的限制，不能有protected等限制
//（对接口来说，有也不认），也应该有默认实现函数，所以需要下面两个基类负责处理
//一些问题，注意要用Messenger的机制一定要从下面的两个类派生，不然不认
/************************************************************************/
class KG3DMessageSenderBase;

class KG3DMessageReceiverBase : public IKG3DMessageReceiverBase
{
	friend KG3DMessageSenderBase;
public:
	virtual ~KG3DMessageReceiverBase() = 0{}
protected:
	virtual KG3DMessageSenderBase* GetSender(DWORD_PTR) = 0;	//子类能调用，由子类决定是否暴露
	virtual ULONG	GetCapacity() = 0; //能够接受多少Sender
public:
	virtual void __stdcall OnReceiveMessage( const KG3DMessage& Message , IKG3DMessageSenderBase* pSender ) = 0;
private:
	virtual bool PickMessage( KG3DMessage Message , KG3DMessageSenderBase* pSender ) = 0;	//除了Sender和Receiver之间的相互通信，不要调用这个接口
};

//////////////////////////////////////////////////////////////////////////

class KG3DMessageSenderBase : public IKG3DMessageSenderBase
{
public:
	virtual bool __stdcall RegisterReceiver( IKG3DMessageReceiverBase* pReceiver ) = 0;
	virtual bool __stdcall UnRegisterReceiver( IKG3DMessageReceiverBase* pReceiver ) = 0;
	virtual ~KG3DMessageSenderBase() = 0{}
protected:
	virtual void PostMessage( const KG3DMessage& Message ) = 0;	//只有子类能调用
	virtual KG3DMessageReceiverBase* GetReceiver(DWORD_PTR) = 0;//子类能调用，由子类决定是否暴露
	virtual ULONG	GetCapacity() = 0;	//能接受多少Receiverr
	bool LetReceiverPickMessage(KG3DMessageReceiverBase& Receiver, const KG3DMessage& Message)//因为PickMessage只有KG3DMessageSenderBase才能用，所以要这个方法来转移调用
	{
		return Receiver.PickMessage(Message, this);
	}
};





#endif //_INCLUDE_IKG3DMESSENGER_H_

