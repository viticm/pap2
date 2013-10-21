////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KG3DMessageWMGListner.h
//  Version     : 1.0
//  Creator     : Chen Tianhong
//  Create Date : 2008-4-1 11:05:29
/*  Comment     : 
KG3DMessage的Sender和Receiver系列实现了Observer模式，而且是生命周期不相关的。
双方在析构的时候都会相互通知

使用方式很简单，

1.发送方从Sender的一种派生，或者拥有一个Sender的实例
2.接收方派生OnReceiveMessage
3.调用一次Sender的RegisterReceiver(pReceiver);
4.Sender方调用PostMessage的时候，所有的注册过的接收方都可以接受到消息
5.不用管对方是否存在，这个机制已经自动处理了生命周期的问题了。

KG3DMessageMSGBroadcaster和KG3DMessageMSGListner是这种模式用于引擎内部处理Windows消息
的类。只是简单的派生了Sender和Receiver，特殊之处是发送和接受的消息是KG3DWindowsMessage而
不是KG3DMessage。

*/
////////////////////////////////////////////////////////////////////////////////

#ifndef _INCLUDE_KG3DMESSAGEWMGLISTNER_H_
#define _INCLUDE_KG3DMESSAGEWMGLISTNER_H_
#include "KG3DMessenger.h"
/************************************************************************/
/*
用一个方法发送消息
*/
/************************************************************************/
class KG3DMessageMSGBroadcaster : public KG3DMessageSender
{
public:
	virtual void PostMessage( const KG3DWindowsMessage& Message );

	virtual bool __stdcall RegisterReceiver( IKG3DMessageReceiverBase* pReceiver );
};
/************************************************************************/
/*
覆盖第一个方法就可以接受消息了
*/
/************************************************************************/
class KG3DMessageMSGListner : public KG3DMessageReceiver
{
public:
	virtual void __stdcall OnReceiveMessage(const KG3DWindowsMessage& Message, IKG3DMessageSenderBase* pSender){NULL;}


public:
	virtual void __stdcall OnReceiveMessage( const KG3DMessage& Message , IKG3DMessageSenderBase* pSender );

};

#endif //_INCLUDE_KG3DMESSAGEWMGLISTNER_H_
