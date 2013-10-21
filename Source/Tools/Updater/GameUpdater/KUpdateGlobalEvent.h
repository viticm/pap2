#ifndef __KUPDATEGLOBALEVENT_H__
#define __KUPDATEGLOBALEVENT_H__

#ifdef _TEST

class KUpdateGlobalEvent
{
protected:
	KUpdateGlobalEvent(void);
	~KUpdateGlobalEvent(void);

public:
	static void SetSignal();

private:
	HANDLE m_hUpdaeDoneEvent;
	static KUpdateGlobalEvent m_UpdateEvent;
};

#endif

#endif
