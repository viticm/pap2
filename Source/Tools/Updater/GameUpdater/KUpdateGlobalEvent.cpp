#include "StdAfx.h"
#include "KUpdateGlobalEvent.h"

#ifdef _TEST

KUpdateGlobalEvent KUpdateGlobalEvent::m_UpdateEvent;

KUpdateGlobalEvent::KUpdateGlobalEvent(void)
{
	m_hUpdaeDoneEvent == INVALID_HANDLE_VALUE;
	m_hUpdaeDoneEvent = CreateEvent(NULL, true, false, "__KUPDATEGLOBALEVENT__");
	if (m_hUpdaeDoneEvent != INVALID_HANDLE_VALUE)
		ResetEvent(m_hUpdaeDoneEvent);

}

KUpdateGlobalEvent::~KUpdateGlobalEvent(void)
{
	if (m_hUpdaeDoneEvent != INVALID_HANDLE_VALUE)
	{
		CloseHandle(m_hUpdaeDoneEvent);
		m_hUpdaeDoneEvent = INVALID_HANDLE_VALUE;
	}
}

void KUpdateGlobalEvent::SetSignal()
{
	if (m_UpdateEvent.m_hUpdaeDoneEvent != INVALID_HANDLE_VALUE)
		SetEvent(m_UpdateEvent.m_hUpdaeDoneEvent);
}

#endif