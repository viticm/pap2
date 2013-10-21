#ifndef UI_DRIVER_K_IMAGE_ALPHA_H
#define UI_DRIVER_K_IMAGE_ALPHA_H

#include "../common/tblock.h"

namespace UI
{

	class KImageAlpha
	{
	private:
		typedef TBlock<BYTE>	KByteArray;

	public:
		KImageAlpha();
		~KImageAlpha();
		
		void SetSize(int nWidth, int nHeight);
		bool GetBuffer(BYTE** ppBuffer, int* pLength);

		bool IsAlpha(int nX, int nY) const;

	private:
		int			m_nWidth;
		int			m_nHeight;

		KByteArray	m_aAlpha;
	};

}

#endif // UI_DRIVER_K_IMAGE_ALPHA_H

