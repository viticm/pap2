////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KG3DMessenger.h
//  Version     : 1.0
//  Creator     : Chen Tianhong
//  Create Date : 2008-3-11 15:41:38
//  Comment     : 见IKG3DMessenger的说明
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _INCLUDE_KG3DMESSENGER_H_
#define _INCLUDE_KG3DMESSENGER_H_

#include "IKG3DMessenger.h"


/************************************************************************/
//单Sender和单Receiver的声明
//可以和下面的混用
/************************************************************************/

class KG3DMessageReceiver;
class KG3DMessageSender;
class KG3DMessageSingleSenderReceiver;
class KG3DMessageSingleReceiverSender;

class KG3DMessageSingleReceiverSender : public KG3DMessageSenderBase
{
public:
	virtual void PostMessage(const KG3DMessage& Message );
	virtual bool __stdcall UnRegisterReceiver( IKG3DMessageReceiverBase* pReceiver );
	virtual bool __stdcall RegisterReceiver( IKG3DMessageReceiverBase* pReceiver );
protected:
	virtual KG3DMessageReceiverBase* GetReceiver(DWORD_PTR){return m_pReceiver;}
	virtual ULONG	GetCapacity(){return 1;}

	//////////////////////////////////////////////////////////////////////////
private:///禁止拷贝
	KG3DMessageSingleReceiverSender(const KG3DMessageSingleReceiverSender&);
	void operator=(const KG3DMessageSingleReceiverSender&);
public:
	KG3DMessageSingleReceiverSender();
	~KG3DMessageSingleReceiverSender();
private:
	KG3DMessageReceiverBase* m_pReceiver;
};
//////////////////////////////////////////////////////////////////////////
class KG3DMessageSingleSenderReceiver : public KG3DMessageReceiverBase
{
	friend	class KG3DMessageSenderBase;
protected:		
	virtual void __stdcall OnReceiveMessage( const KG3DMessage& Message , IKG3DMessageSenderBase* pSender );
	virtual KG3DMessageSenderBase* GetSender(DWORD_PTR){return m_pSender;}
	virtual ULONG	GetCapacity(){return 1;}
private:	//Sender用KG3DMessageSenderBase的一个函数来调用Receiver的PickMessage，派生类不应该覆盖这个函数
	virtual bool PickMessage( KG3DMessage Message , KG3DMessageSenderBase* pSender );

	//////////////////////////////////////////////////////////////////////////
private:
	KG3DMessageSenderBase* m_pSender;
private:///禁止拷贝
	KG3DMessageSingleSenderReceiver(const KG3DMessageSingleSenderReceiver&);
	void operator=(const KG3DMessageSingleSenderReceiver&);
public:
	KG3DMessageSingleSenderReceiver();
	~KG3DMessageSingleSenderReceiver();
};
//////////////////////////////////////////////////////////////////////////
/************************************************************************/
//多Receiver的Sender和Sender的Receiver的声明
/************************************************************************/
class KG3DMessageSender : public KG3DMessageSenderBase
{
public:
	virtual bool __stdcall UnRegisterReceiver( IKG3DMessageReceiverBase* pReceiver );
	virtual bool __stdcall RegisterReceiver( IKG3DMessageReceiverBase* pReceiver );
	virtual void PostMessage( const KG3DMessage& Message );	//派生类用这个方法发送消息
	virtual size_t	GetReceiverCount(){return m_ListReceiver.size();}
	virtual ULONG	GetCapacity(){return ULONG_MAX;}
protected:	
	virtual KG3DMessageReceiverBase* GetReceiver(DWORD_PTR i);

	//////////////////////////////////////////////////////////////////////////
private:
	typedef vector<KG3DMessageReceiverBase*>  RECEIVER_LIST;
	RECEIVER_LIST m_ListReceiver;
private:///禁止拷贝
	KG3DMessageSender(const KG3DMessageSender&);
	void operator=(const KG3DMessageSender&);
public:
	KG3DMessageSender();
	~KG3DMessageSender();
private:
	bool Remove(IKG3DMessageReceiverBase* pReceiver);
};
//////////////////////////////////////////////////////////////////////////
class KG3DMessageReceiver : public KG3DMessageReceiverBase
{
	friend KG3DMessageSenderBase;
public:
	virtual void __stdcall OnReceiveMessage( const KG3DMessage& Message , IKG3DMessageSenderBase* pSender ){NULL;}
protected:
	virtual KG3DMessageSenderBase* GetSender(DWORD_PTR i);
	virtual ULONG GetCapacity(){return ULONG_MAX;}
private://Sender用KG3DMessageSenderBase的一个函数来调用Receiver的PickMessage，派生类不应该覆盖这个函数
	virtual bool PickMessage( KG3DMessage Message , KG3DMessageSenderBase* pSender );
	
	//////////////////////////////////////////////////////////////////////////
private:///禁止拷贝
	KG3DMessageReceiver(const KG3DMessageReceiver&);
	void operator=(const KG3DMessageReceiver&);
private:
	typedef vector<KG3DMessageSenderBase*>  SENDER_LIST;		
	SENDER_LIST m_ListSender;
public:
	KG3DMessageReceiver();
	~KG3DMessageReceiver();
private:
	bool Remove(IKG3DMessageSenderBase* pSender);
};

class KG3DMessageReceiverBaseDummy: public KG3DMessageReceiverBase
{
protected:
	virtual KG3DMessageSenderBase* GetSender(DWORD_PTR){return NULL;}
	virtual ULONG	GetCapacity(){return 0;}
public:
	virtual void __stdcall OnReceiveMessage( const KG3DMessage& Message , IKG3DMessageSenderBase* pSender ){}
private:
	virtual bool PickMessage( KG3DMessage Message , KG3DMessageSenderBase* pSender ){return false;}
};

//////////////////////////////////////////////////////////////////////////

class KG3DMessageSenderBaseDummy : public KG3DMessageSenderBase
{
public:
	virtual bool __stdcall RegisterReceiver( IKG3DMessageReceiverBase* pReceiver ){return false;}
	virtual bool __stdcall UnRegisterReceiver( IKG3DMessageReceiverBase* pReceiver ){return false;}
protected:
	virtual void PostMessage( const KG3DMessage& Message ){return;}
	virtual KG3DMessageReceiverBase* GetReceiver(DWORD_PTR){return NULL;}
	virtual ULONG	GetCapacity(){return 0;}
};

class KG3DMessageReceiverBaseTest : public KG3DMessageReceiverBaseDummy
{
	virtual bool PickMessage( KG3DMessage Message , KG3DMessageSenderBase* pSender ){return true;}
};
class KG3DMessageSenderBaseTest : public KG3DMessageSenderBaseDummy
{
	virtual bool __stdcall RegisterReceiver( IKG3DMessageReceiverBase* pReceiver ){return true;}
	virtual bool __stdcall UnRegisterReceiver( IKG3DMessageReceiverBase* pReceiver ){return true;}
};
#endif //_INCLUDE_KG3DMESSENGER_H_
