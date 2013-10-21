#include "stdafx.h"
#include "./kimagealpha.h"

namespace UI
{

	KImageAlpha::KImageAlpha()
        : m_nWidth(0)
		, m_nHeight(0)
	{
	}

	KImageAlpha::~KImageAlpha()
	{
	}

	void KImageAlpha::SetSize(int nWidth, int nHeight)
	{
		ASSERT(nWidth > 0 && nHeight > 0);
		m_nWidth = nWidth;
		m_nHeight = nHeight;
		m_aAlpha.Alloc((nWidth * nHeight + 7) / 8);
	}

	bool KImageAlpha::GetBuffer(BYTE** ppBuffer, int* pLength)
	{
		if (ppBuffer == NULL)
			return false;
		if (*ppBuffer != NULL)
			return false;
		if (pLength == NULL)
			return false;

		if (m_aAlpha.GetSize() > 0)
		{
			*ppBuffer = &m_aAlpha[0];
			*pLength = m_aAlpha.GetSize();
		}
		else
		{
			*pLength = 0;
		}
		return true;
	}

	bool KImageAlpha::IsAlpha(int nX, int nY) const
	{
		if (nX < 0 || nX >= m_nWidth || nY < 0 || nY >= m_nHeight)
			return false;

		int const nIndex = nX + nY * m_nWidth;
		ASSERT(nIndex >= 0 && nIndex < 8 * m_aAlpha.GetSize());
		if ((m_aAlpha[nIndex / 8] & (1 << (nIndex % 8))) == 0)
			return false;

		return true;
	}

}

