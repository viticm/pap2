////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KG3DMessageWMGListner.cpp
//  Version     : 1.0
//  Creator     : Chen Tianhong
//  Create Date : 2008-4-1 11:05:41
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "KG3DMessageWMGListner.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

////////////////////////////////////////////////////////////////////////////////
void __stdcall KG3DMessageMSGListner::OnReceiveMessage( const KG3DMessage& Message , IKG3DMessageSenderBase* pSender )
{
	if (Message.m_uMessage == KM_WINDOWS_MESSAGE)
	{
		OnReceiveMessage(*reinterpret_cast<KG3DWindowsMessage*>(Message.m_dwWParam), pSender);
	}
}

void KG3DMessageMSGBroadcaster::PostMessage( const KG3DWindowsMessage& Message )
{
	KG3DMessage TempMsg(KM_WINDOWS_MESSAGE, (DWORD_PTR)(&Message), NULL);
	KG3DMessageSender::PostMessage(TempMsg);
}

bool __stdcall KG3DMessageMSGBroadcaster::RegisterReceiver( IKG3DMessageReceiverBase* pReceiver )
{
	if(dynamic_cast<KG3DMessageMSGListner*>(pReceiver) == NULL)//×èÖ¹·ÇListner×¢²á
		return false;
	return KG3DMessageSender::RegisterReceiver(pReceiver);
}

#if defined(KG_ENABLE_TDD)
KG_TEST_BEGIN(KG3DMessageWMGListner)
	{
		enum
		{
			A_WINDOW_MESSAGE = 0x122,
		};
		class DummyBroadcaster //: public KG3DMessageMSGBroadcaster
		{
		public:
			KG3DMessageMSGBroadcaster m_BC;
		};
		class DummyListener : public KG3DMessageMSGListner
		{
		public:
			BOOL bReceived;

			DummyListener():bReceived(FALSE){}			
			virtual void __stdcall OnReceiveMessage(const KG3DWindowsMessage& Message, IKG3DMessageSenderBase* pSender)
			{
				if (Message.m_uMessage == A_WINDOW_MESSAGE)
				{
					bReceived = TRUE;
				}
			}
		};

		DummyBroadcaster BC;
		DummyListener LT;
		_ASSERTE(LT.bReceived == FALSE);

		BC.m_BC.RegisterReceiver(&LT);
		BC.m_BC.PostMessage(KG3DWindowsMessage(A_WINDOW_MESSAGE, NULL, NULL));
		
		_ASSERTE(LT.bReceived == TRUE);

		//::OutputDebugString(_T("KG3DMessageMSGListner&KG3DMessageMSGBroadcaster Test OK"));
	}
KG_TEST_END(KG3DMessageWMGListner)
#endif
