#include "stdafx.h"
#include "./kanimateimage.h"

namespace UI
{

	KAnimateImage::KAnimateImage()
		: m_nImageInfoIndex(-1)
		, m_nGroup(0)
		, m_nTotalFrame(0)
		, m_nLoopCount(-1)
		, m_dwInterval(100)
		, m_dwFlipTime(0)
		, m_nFrame(0)
		, m_nCurrentCount(0)
		, m_nFinished(false)
	{
	}

	bool KAnimateImage::operator==(KAnimateImage const& rhs) const
	{
		return false;
	}

	bool KAnimateImage::Init()
	{
		m_dwFlipTime = KG_GetTickCount();
		return true;
	}

	bool KAnimateImage::Activate(DWORD dwElapsed)
	{
		// ¶¯»­
		if (dwElapsed - m_dwFlipTime >= m_dwInterval)
		{
			m_dwFlipTime += m_dwInterval;
			if (++m_nFrame >= m_nTotalFrame)
			{
                ++m_nCurrentCount;
                if (m_nLoopCount != -1 && m_nCurrentCount >= m_nLoopCount)
                {   
                    m_nCurrentCount = m_nLoopCount;
                    m_nFrame        = m_nTotalFrame - 1;
					m_nFinished = true;
                    return true;
                }

				m_nFrame = 0;
				m_dwFlipTime = dwElapsed;
			}
			m_nFinished = false;
			return false;
		}
		m_nFinished = false;
		return false;
	}


}
