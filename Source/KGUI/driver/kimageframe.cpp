#include "stdafx.h"
#include "./kimageframe.h"

namespace UI
{

	KImageFrame::KImageFrame()
	{
		memset(&m_ptLeftTop, 0, sizeof(POINT));
		memset(&m_sizeFrame, 0, sizeof(SIZE));
		m_wFrame = 0;
	}


	bool KImageFrame::Init(POINT const& ptLeftTop, SIZE const& sizeFrame, WORD wFrame)
	{
		KG_PROCESS_ERROR(ptLeftTop.x >= 0);
		KG_PROCESS_ERROR(ptLeftTop.y >= 0);
		KG_PROCESS_ERROR(sizeFrame.cx >= 0);
		KG_PROCESS_ERROR(sizeFrame.cy >= 0);
		KG_PROCESS_ERROR(wFrame >= 0);

		m_ptLeftTop = ptLeftTop;
		m_sizeFrame = sizeFrame;
		m_wFrame = wFrame;

		return true;
Exit0:
		return false;
	}

}

